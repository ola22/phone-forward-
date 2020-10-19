#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include "phone_forward.h"

#define DIGITS  12



/** @brief Usuwa całą strukturę listy List.
 * @param[in] l - wskaźnikna listę do usunięcia.
 */
static void cleanList(List *l) {
    if (l != NULL) {
        while (l != NULL) {
            List *temp = l;
            l = l->next;

            if (temp->revNum != NULL)
                free(temp->revNum);

            free(temp);
        }
    }
}


/** @brief Usuwa pojedynczy element listy.
 * @param[in] toDel - wskaźnik na element listy do usunięcia.
 */
static void removeRevListEl(List *toDel) {
    List *pop = toDel->prev;
    List *nast = toDel->next;

    pop->next = toDel->next;
    if (nast != NULL)
        nast->prev = toDel->prev;

    free(toDel->revNum);
    toDel->revNum = NULL;

    free(toDel);
}


/** @brief Tworzy pojedynczy element listy.
 * Umieszcza nowo utworzony element listy rev danego numeru za atrapą.
 * @param[in] rev - wskaźnik na listę, do której dodajemy element;
 * @param[in] num - wskaźnik na numer, który dodajemy do listy.
 * @return Wskaźnik nowo utworzonego elementu listy
 *         lub NULL  wprzypadku błędu alokacji pamięci.
 */
static List * addRevListEl(List *rev, char *num) {
    List *new = (List*)malloc(sizeof(List));
    List *first = rev;

    // błędy z alokacją pamięci.
    if (new == NULL)
        return NULL;

    new->next = first->next;
    new->prev = first;

    if (first->next != NULL)
        (first->next)->prev = new;
    first->next = new;

    new->revNum = num;

    return new;
}


void phfwdDelete(struct PhoneForward *pf) {
    if (pf == NULL)
        return;

    for (int i = 0; i < DIGITS; i++) {
        phfwdDelete(pf->digits[i]);
    }

    // usunięcie kolejnych pól struktury.
    if (pf->number != NULL)
        free(pf->number);
    pf->number = NULL;

    if (pf->rev != NULL)
        cleanList(pf->rev);
    pf->rev = NULL;

    free(pf);
}


/** @brief Tworzy pojedynczy element struktury PhoneForward.
 * Ustawia jej pola na nulle.
 * @return Wskaźnik nowo utworzonego elementu lub NULL w przypadku problemów z alokacją pamięci.
 */
static struct PhoneForward * createNewElement() {
    struct PhoneForward *newEl = malloc(sizeof(struct PhoneForward));

    if (newEl == NULL)
        return NULL;

    for (int i = 0; i < DIGITS; i++) {
        newEl->digits[i] = NULL;
    }

    List *atrap = (List*)malloc(sizeof(List));
    atrap->next = NULL;
    atrap->prev = NULL;
    atrap->revNum = NULL;

    newEl->rev = atrap;
    newEl->this = NULL;
    newEl->number = NULL;

    return newEl;
}


struct PhoneForward * phfwdNew(void) {
    // w razie problemów z alokacją pamięci funkcja createNewElement zwróci NULL.
    struct PhoneForward *pf = createNewElement();

    return pf;
}


/** @brief Sprawdza, czy otrzymana tablica znaków zawiera numer.
 * Zwróci fałsz przy pierwszym napotkanym znaku, nie reprezentującym liczby.
 * @param[in] num – wskaźnik na napis reprezentujący potencjalny numer.
 * @return Wartość @p true, jeśli wszystkie znaki są cyframi,
 *         Wartość @p false, jeśli chociaż jeden znak okazał się nie być cyfrą.
 */
static bool checkIfNumber(char const *num) {
    int i = 0;
    char c = num[i];

    while (c != '\0') {
        if (!isdigit(c) && !(c == ':') && !(c == ';'))
            return false;

        i++;
        c = num[i];
    }

    return true;
}


/** @brief Tworzy kopię numeru, otrzymanego od użytkownika.
 * @param[in] num – wskaźnik na napis reprezentujący numer do skopiowania.
 * @return wskaźnik na utworzoną kopię numeru
 *         lub NULL w przypadku błędów alokacji pamięci.
 */
static char * copyNumber(char const *num) { // num napewno nie jest nullem
    // dodana 1 rezerwuje miejsce na '\0'
    size_t n = strlen(num) + 1;

    char *numToAdd = malloc(n * sizeof(char));

    // problemy z alokacją pamięci.
    if (numToAdd == NULL)
        return NULL;

    strcpy(numToAdd, num);

    return numToAdd;
}


/** @brief Funkcja pomocnicza do phfwdAdd.
 * Odnajduje w strukturze PhoneForward dany numer.
 * @param[in] num - odnajdywany numer;
 * @param[in] pf - wskaźnik na strukturę PhoneForward, w której szukamy danego numeru.
 * @return Wskaźnik do węzła struktury PhoneForward, reprezentującego szukany numer.
 */
static struct PhoneForward * findNumInStructure(char const *num, struct PhoneForward *pf) {
    struct PhoneForward *temp = pf;
    int i = 0;
    int x;
    char c;

    // szukanie num w strukturze phoneForward
    c = num[i];
    while (c != '\0') {
        x = (int) c - (int) '0';

        if (temp->digits[x] == NULL) {
            struct PhoneForward *newEl = createNewElement();

            // problem z alokacją pamięci.
            if (newEl == NULL)
                return false;

            temp->digits[x] = newEl;
        }

        temp = temp->digits[x];
        i++;
        c = num[i];
    }

    return temp;
}


bool phfwdAdd(struct PhoneForward *pf, char const *num1, char const *num2) {
    // num1 lub num2 lub struktura pf są nulami.
    if (num1 == NULL || num2 == NULL || pf == NULL)
        return false;

    // num1 lub num2 reprezentują pusty ciąg.
    if (num1[0] == '\0' || num2[0] == '\0')
        return false;

    bool isDigitNum1 = checkIfNumber(num1);
    bool isDigitNum2 = checkIfNumber(num2);
    int isTheSame = strcmp(num1, num2);
    struct PhoneForward *temp1;
    struct PhoneForward *temp2;
    List *new;

    // num1 nie reprezentują numeru, lub są takie same.
    if (!isDigitNum1 || !isDigitNum2 || isTheSame == 0)
        return false;


    // dodawanie przekierowania dla num1.
    // szukanie num1 w strukturze phoneForward
    temp1 = findNumInStructure(num1, pf);

    // dodanie num2 do struktury.
    char *numToAdd = copyNumber(num2);

    // problem z alokacją pamięci.
    if (numToAdd == NULL)
        return false;

    if (temp1->number != NULL) {
        // usuwanie starego przekierowania.
        free(temp1->number);

        // usuwanie num1 z listy rev starego przekierowania.
        removeRevListEl(temp1->this);
    }

    temp1->number = numToAdd;


    // dodawanie odwrotnego przekierowania do listy rev dla num2.
    // szukanie num2 w strukturze phoneForward.
    temp2 = findNumInStructure(num2, pf);

    // dodanie num1 do listy rev.
    char *numToAdd2 = copyNumber(num1);

    // problem z alokacją pamięci.
    if (numToAdd2 == NULL)
        return false;

    new = addRevListEl(temp2->rev, numToAdd2);
    temp1->this = new;

    // problem z alokacją pamięci.
    if (new == NULL)
        return false;

    return true;
}


/** @brief Usuwa wybrane przekierowania ze struktury PhoneForward,
 * Wskaźnik na usunięte pola ustawia na NULL.
 * @param[in] pf  – wskaźnik na strukturę przechowującą przekierowania numerów.
 */
static void removeChosenNumbers(struct PhoneForward *pf) {
    if (pf == NULL)
        return;
    else {
        for (int i = 0; i < DIGITS; i++) {
            removeChosenNumbers(pf->digits[i]);
        }

        if (pf->number != NULL) {
            free(pf->number);
            removeRevListEl(pf->this);
        }

        pf->number = NULL;
        pf->this = NULL;
    }
}


void phfwdRemove(struct PhoneForward *pf, char const *num) {
    // num lub strunktura są nullami.
    if (num == NULL || pf == NULL)
        return;

    bool isDigitNum = checkIfNumber(num);
    int i = 0;
    char c = num[i];

    // num nie reprezentuje numeru.
    if (!isDigitNum || c == '\0')
        return;

    // poniższa pętla szuka miejsca, od którego przekierowania powinny być usunięte.
    struct PhoneForward *temp = pf;

    while (c != '\0') {
        int x = (int) c - (int) '0';

        if (temp->digits[x] != NULL) {
            temp = temp->digits[x];
        }
        // nie znaleziono przekierowań do usunięcia.
        else
            return;

        i++;
        c = num[i];
    }

    removeChosenNumbers(temp);
}


/** @brief Tworzy pojedynczy element struktury PhoneNumbers,
 * ustawiając jednocześnie jego pola na NULL.
 * @return Wskaźnik na utworzony element struktury phoneNumbers.
 */
static struct PhoneNumbers * createPhoneNumbers() {
    struct PhoneNumbers *phnum = malloc(sizeof(struct PhoneNumbers));

    phnum->next = NULL;
    phnum->number = NULL;

    return phnum;
}


/** @brief Wyznacza przekierowanie podanego numeru dla funkcji pfwdGet.
 * @param[in] where  – wartość, wskazująca na miejsce, w którym skończy się
 *            przekierowanie w tworzonym numerze;
 * @param[in] num – wskaźnik na napis reprezentujący numer do przekierowania;
 * @param [in] foundNum – wskaźnik na napis reprezentujący znalezione przekierowanie;
 * @return Wskaźnik na nowo utworzony numer.
 */
static char * createFinalNumber(char const *num, char *foundNum, int where) {
    // obliczenie rozmiaru tablicy na tworzony numer.
    // foundNum i num nie są nullami.
    int numLength = (int) strlen(num);
    int foundNumLength = (int) strlen(foundNum);

    int n = numLength + 1 - where + foundNumLength;

    char *finalNumber = malloc(sizeof(char)*n);
    for (int j = 0; j < foundNumLength; j++) {
        finalNumber[j] = foundNum[j];
    }

    for (int j = where; j < numLength; j++) {
        finalNumber[foundNumLength + j - where] = num[j];
    }
    finalNumber[n-1] = '\0';

    return finalNumber;
}


/** @brief Wyznacza przekierowanie numeru,
 * szuka najdłuższego pasującego prefiksu;
 * @param[in] pf  – wskaźnik na strukturę przechowującą przekierowania numerów;
 * @param[in] num – wskaźnik na napis reprezentujący numer;
 * @param[in] counter  – wskazuje, na to która cyfra z tablicy numerów jest aktualnie rozpatrywana;
 * @param[in] memoryProblems – zmienna, zmieni wartość na 1, jeśli wystąpią problemy z alokacją pamięci;
 * @return Wskaźnik na strukturę przechowującą ciąg numerów lub NULL, gdy nie
 *         udało się zaalokować pamięci.
 */
static struct PhoneNumbers * findRightNumber(struct PhoneForward *pf, char const *num, int counter, int *memoryProblems) {
    char c = num[counter]; // num nie jest nullem.
    struct PhoneNumbers *prev = NULL;
    int x = (int) c - (int) '0';

    if (c != '\0' && pf->digits[x] != NULL) {
        prev = findRightNumber(pf->digits[x], num, counter + 1, memoryProblems);
    }

    if (prev == NULL && pf->number == NULL)
        return NULL;
    else {
        // nejdłuższy numer właśnie został znaleziony.
        if (prev == NULL && pf->number != NULL) {
            //tworzenie zwracanej struktury.
            struct PhoneNumbers *pnum = createPhoneNumbers();
            char *newNumber = createFinalNumber(num, pf->number, counter);

            // problem z alokacją pamięci.
            if (pnum == NULL || newNumber == NULL)
                (*memoryProblems) = 1;
            else
                pnum->number = newNumber;

            return pnum;
        }
        else
            return prev;
    }
}


struct PhoneNumbers const * phfwdGet(struct PhoneForward *pf, char const *num) {
    if (pf == NULL || num == NULL)
        return NULL;

    struct PhoneNumbers *pnum;

    bool isDigitNum = checkIfNumber(num);
    // num nie reprezentuje numeru.
    if (!isDigitNum || num[0] == '\0') {
        pnum = createPhoneNumbers();
    }
    else {
        // gdy wystąpią problemy z alokacją pamięci wartość zmiennej wyniesie 1.
        int memoryProblems = 0;
        pnum = findRightNumber(pf, num, 0, &memoryProblems); // pf nie jest nullem.

        // problem z alokacją pamięci.
        if (memoryProblems == 1)
            return NULL;

        // nie znaleziono żadnego przekierowania.
        if (pnum == NULL) {
            pnum = createPhoneNumbers();
            char *newNumber = copyNumber(num);

            // problem z alokacją pamięci.
            if (newNumber == NULL)
                return NULL;

            pnum->number = newNumber;
        }
    }

    return pnum;
}


/** @brief Zlicza elementy struktury PhoneNumbers,
 * @param[in] pnum  – wskaźnik na strukturę przechowującą numery telefonów;
 * @return Liczbę całkowitą, wskazującą na liczbę elementów strunktury PnhoneNumbers.
 */
static int pnumElCounter(struct PhoneNumbers const *pnum) {
    int counter = 0;
    struct PhoneNumbers const *temp = pnum;

    while (temp != NULL) {
        counter++;
        temp = temp->next;
    }

    return counter;
}


char const * phnumGet(struct PhoneNumbers const *pnum, size_t idx) {
    if (pnum ==  NULL)
        return NULL;

    int idxInt = (int) idx;
    int elAmount = pnumElCounter(pnum);

    // index jest spoza zakresu.
    if (elAmount <= idxInt)
        return NULL;

    // Szukanie właściwego numeru.
    int counter = 0;
    struct PhoneNumbers const *temp = pnum;

    while (counter < idxInt) {
        counter++;
        temp = temp->next;
    }

    return temp->number;
}


/** @brief Szuka miejsca w liście, w którym należy dodać jej nowy element,
 * aby numery były uporządkowane leksykograficznie.
 * @param[in] l - wskaźnik na strukturę PhoneNumbers, w której poszukujemy odpowiedniego miejsca;
 * @param[in] num - numer, który próbujemy umiejscowić w liście.
 * @param[in] alreadyExsist - wskaźnik na zmienną, przechowującąinformację o tym, czy dany numer
 *            został już wcześniej dodany. Jeśli w strukturze l natrafimy na numer num
 *            wartość zmiennej wyniesie true.
 * @return Wskaźnik na miejsce w strukturze PhoneNumbers, w którym należy dodać nowy element.
 */
static struct PhoneNumbers * findPlaceInList(struct PhoneNumbers *l, char const *num, bool *alreadyExsist) {
    // poniższa pętla sprawdza, który numer jest większy.
    while (l->next!=NULL && strcmp((l->next)->number, num) < 0) {
        l = l->next;
    }

    if (l->next!=NULL && strcmp((l->next)->number, num) == 0)
        (*alreadyExsist) = true;

    return l;
}


/** @brief Funkcja pomocnicza dla funkcji phfwdReverse.
 * Dodaje kolejny elemnt struktury PhoneNumbers, zawierający numer num
 * do wynikowej listy funkcji phfwdReverse.
 * @param[in] num - wskaźnik na numer. który mamy dodać;
 * @param[in] place - wskaźnik na miejsce w strukturze PhoneNumbers,
 *            w którym ma zostać dodany nowy element, aby zachować
 *            porządek leksykograficzny numerów;
 * @param[in] memoryProblems - wskażnik na zmienną, przechowującą informację o tym,
 *            czy wystąpiły problemy z alokacją pamięci.
 **/
static void addToFinalList(char *num, struct PhoneNumbers *place, bool *memoryProblems) {
    struct PhoneNumbers *newEl = createPhoneNumbers();
    struct PhoneNumbers *temp = place->next;

    if (newEl == NULL) {
        (*memoryProblems) = true;
        return;
    }

    place->next = newEl;
    newEl->next = temp;
    newEl->number = num;
}


/** @brief Funkcja pomocnicza dla funkcji phfwdReverse.
 * Tworzy numer dla odwrotnego przekierowania.
 * @param[in] usersNum - wskaźnik na numer otrzymany od użytkownika;
 * @param[in] where - liczba cyfr numeru usersNum, które należy pominąć
 *            (miejsce, odktóregomamy zacząć kopiować dane);
 * @param[in] revNum - numer odwrotnego przekierowania.
 * @return Wskaźnik na nowo utworzony numer wynikowy.
 */
static char * createReverseNumber(char const *usersNum, int where, char *revNum) {
    size_t len1 = strlen(usersNum);
    size_t len2 = strlen(revNum);

    // dodana 1 rezerwuje miejsce na '\0'
    size_t n = len1 - where + len2 + 1;

    char *new = malloc(n * sizeof(char));

    if (new == NULL)
        return NULL;

    for (size_t j = 0; j < len2; j++)
        new[j] = revNum[j];

    for (size_t j = 0; j < len1 - where; j++)
        new[len2 + j] = usersNum[where +j];

    new[n - 1] = '\0';

    return new;
}


struct PhoneNumbers const * phfwdReverse(struct PhoneForward *pf, char const *num) {
    if (pf == NULL || num == NULL)
        return NULL;

    bool isDigitNum = checkIfNumber(num);
    // num nie reprezentuje numeru lub num jest pustym ciagiem.
    if (!isDigitNum || num[0] == '\0')
        return createPhoneNumbers();


    int i = 0;
    char c = num[i];
    char *newNum;
    bool memoryProblems = false;
    bool alreadyExsist = false;
    struct PhoneNumbers *place;
    struct PhoneNumbers *list = createPhoneNumbers();
    struct PhoneForward *temp = pf;

    // problemy z alokacją pamięci.
    if (list == NULL)
        return NULL;

    while (c != '\0') {
        int x = (int) c - (int) '0';

        temp = temp->digits[x];

        // napewno niżej nie ma żadnych przekierowań.
        if (temp == NULL)
            break;

        // dany numer posiada jakieś elementy na swojej liście rev.
        if ((temp->rev)->next != NULL) {
            List *l = temp->rev;

            // dodajemy numery reverse dla kolejnych numerów z listy rev danego węzła.
            while (l->next != NULL) {
                alreadyExsist = false;
                l = l->next;
                newNum = createReverseNumber(num, i + 1, l->revNum);

                // problemy z alokacją pamięci.
                if (newNum == NULL)
                    return NULL;

                place = findPlaceInList(list, newNum, &alreadyExsist);

                if (!alreadyExsist)
                    addToFinalList(newNum, place, &memoryProblems);
                else
                    free(newNum);

                if (memoryProblems)
                    return NULL;
            }
        }

        i++;
        c = num[i];
    }

    // dodanie numeru otrzymanego od uzytkownika.
    alreadyExsist = false;
    memoryProblems = false;
    place = findPlaceInList(list, num, &alreadyExsist);

    if (!alreadyExsist) {
        char *usersNum = copyNumber(num);

        // problem z alokacją pamięci.
        if (usersNum == NULL)
            return NULL;

        addToFinalList(usersNum, place, &memoryProblems);

        // problem z alokacją pamięci.
        if (memoryProblems)
            return NULL;
    }

    // usuwanie pierwszego, pustego elementu.
    struct PhoneNumbers *p = list;
    list = list->next;
    free(p);

    return list;
}


/** @brief Funkcja pomocnicza dla phfwdNonTrivialCount.
 * Oblicza wynik działania a^b.
 * @param[in] a - podstawa poęgi;
 * @param[in] b - wykładnik potęgi.
 * @return Wynik działania a^b.
 */
static size_t raiseToPower(size_t a, size_t b) {
    size_t result = 1;

    while (b > 0) {
        if (b % 2 == 1)
            result *= a;

        a *= a;
        b /= 2;
    }

    return result;
}


/** @brief Funkcja pomocnicza dla phfwdNonTrivialCount.
 * Wywołuje się rekurencyjnie na całym drzewie, zliczając nietrywialne numery.
 * @param[in] pf - wskaźnik na strukturę przekierowań, na której operujemy.
 * @param[in] tab - tablica, mówiąca nam o tym, które cyfry wystapiły w secie;
 * @param[in] len - długość numeru, jaką rozpatrujemy;
 * @param[in] actLen - aktualna głębokość węzła w którym jesteśmy;
 * @param[in] n - liczba różnych cyfr w napisie set.
 * @return Wyliczoną liczbę nietrywialnych numerów.
 */
size_t countNonTrivial(struct PhoneForward *pf, bool tab[], size_t len, size_t actLen, size_t n) {
    size_t result = 0;

    if (pf == NULL)
        return result;

    // zeszliśmy na maksymalną głębokość, nie szukamy dłuższego numeru.
    if (actLen == len) {
        if ((pf->rev)->next != NULL)
            result = 1;

        return result;
    }

    // dany węzeł posiada jakieś odwrotne przekierowania, dodajemy odpowiednią wartość do wyniku,
    // nie szchodzimy już dojego synów.
    if ((pf->rev)->next != NULL) {
        result = raiseToPower(n, len - actLen);

        return result;
    }

    // jeśli nie posiadał żadnych numerów na liście rev,
    // wywołujemy się rekurencyjnie na synach, będących w secie.
    for (size_t i = 0; i < 12; i++) {
        if (tab[i])
            result += countNonTrivial(pf->digits[i], tab, len, actLen + 1, n);
    }

    return result;
}


/** @brief Przegląda otrzymany zestaw znaków set, pomijając te, które nie są cyframi.
 * Zaznacza również w tablicy tab, które cyfry wystąpiły w secie.
 * @param set - wskaźnik na rozpatrywany zestaw znaków set;
 * @param[in] tab - tablica, na której zaznaczamy, które cyfry pojawiły się w secie;
 * @param[in] d - wskaźnik na zmienną, zliczającą liczbę różnych cyfr w numerze num.
 */
static void selectJustDigits(char const *set, bool tab[], size_t *d) {
    if (set == NULL)
        return;

    size_t n = strlen(set);
    int x;
    char c;

    // w tablicy tab zaznaczamy wystepujące w napisie set cyfry.
    for (size_t i = 0; i < n; i++) {
        c = set[i];

        if (c <= ';' && c >= '0') {
            x = (int) c - (int) '0';

            if (!tab[x]) {
                tab[x] = true;
                (*d) += 1;
            }
        }
    }
}


size_t phfwdNonTrivialCount(struct PhoneForward *pf, char const *set, size_t len) {
    if (pf == NULL || set == NULL)
        return 0;

    //  num jest pustym ciagiem, lub len wynosi 0, zwracamy 0.
    if (set[0] == '\0' || len == 0)
        return 0;

    // tablica służąca uniknięciu powtórzeń w zestawie set.
    bool tab[12] = {false};
    size_t result = 0;
    size_t d = 0;

    selectJustDigits(set, tab, &d);

    result = countNonTrivial(pf, tab, len, 0, d);

    return result;
}


void phnumDelete(struct PhoneNumbers const *pnum) {
    if (pnum != NULL) {
        while (pnum != NULL) {
            struct PhoneNumbers const *temp = pnum;
            pnum = pnum->next;

            if (temp->number != NULL)
                free(temp->number);
            
            free((void*)temp);
        }
    }
    else return;
}