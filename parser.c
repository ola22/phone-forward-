#include <stdio.h>
#include <string.h>
#include "parser.h"
#include "baza.h"
#include "phone_forward.h"



void printSuddenError() {
    fprintf(stderr, "ERROR EOF\n");
}


void printUnreadableError(int n) {
    fprintf(stderr, "ERROR %d\n", n);
}


/** @brief Wypisuje informacje o błędzie wykonywania.
 * @param[in] operator - wskaźnik na operator,
 *            w wyniku (wykonywania) którego powstał błąd;
 * @param[in] n - numer pierwszego znaku operatora,
 *            w wyniku (wykonywania) którego powstał błąd;
 */
static void printMakingError(char *operator, int n) {
    fprintf(stderr, "ERROR %s %d\n", operator, n);
}


/** @brief Wypisuje wynik działania funkcji phfwdNonTrivialCount.
 * @param result - wynik działania funkcji phfwdNonTrivialCount.
 */
static void printNonTrivialResult(size_t result) {
    fprintf(stdout, "%zu\n", result);
}


/** @brief Wypisuje numery przechowywane przez strukturę phoneNumbers,
 * usuwa strukturę.
 * @param[in] list - wskaźnik na strukturę phoneNumbers.
 */
static void printNumbers(struct PhoneNumbers const *list) {
    char const *num;
    size_t idx = 0;

    while ((num = phnumGet(list, idx++)) != NULL)
        fprintf(stdout, "%s\n", num);

    phnumDelete(list);
}


/** @brief Tworzy kopię numeru pobranego ze struktury PhoneForward,
 * zmieniając jednocześnie ich typ na char const*
 * @param[in] num – wskaźnik na napis reprezentujący numer do skopiowania;
 * @param[in] memoryProblems - wskażnik na zmienną, przechowującą informację o tym,
 *            czy wystąpiły problemy z alokacją pamięci;
 * @return Wskaźnik na kopię numeru, przesłaną użytkownikowi.
 */
static char const * convertToCharConst(char *num, bool *memoryProblems) {
    if (num == NULL)
        return NULL;

    // dodanie jedynki rezerwuje miejsce na '\0'
    size_t n = strlen(num) + 1;

    char *convertedNum = (char*)malloc(sizeof(char) * n);

    // gdyby wystąpiły problemy z alokacją pamięci.
    if (convertedNum == NULL) {
        (*memoryProblems) = true;
        return NULL;
    }

    strcpy(convertedNum, num);

    return convertedNum;
}


/** @brief Tworzy kopię podanej nazwy bazy.
 * @param[in] name - wskaźnik na nazwę bazy do skopiowania;
 * @param[in] memoryProblems - wskażnik na zmienną, przechowującą informację o tym,
 *            czy wystąpiły problemy z alokacją pamięci;
 * @return Wskaźnik na nowo utworzoną nazwę bazy.
 */
static char * copyName(char *name, bool *memoryProblems) {
    if (name == NULL)
        return NULL;

    // dodanie jedynki rezerwuje miejsce na '\0'
    size_t n = strlen(name) + 1;

    char *copiedName = (char*)malloc(sizeof(char) * n);

    // gdyby wystąpiły problemy z alokacją pamięci.
    if (copiedName == NULL) {
        (*memoryProblems) = true;
        return NULL;
    }

    strcpy(copiedName, name);

    return copiedName;
}


/** @brief Sprawdza, czy dana operacja na identyfikatorach nie jest zakazana
 * (czy ktoś nie chce utworzyć lub usunąć bazy o nazwie DEL lub NEW)
 * @param[in] instr - wskaźnik na identyfikator, podejrzany o bycie zakazanym;
 * @param[in] errorAppeared - wskaźnik na zmienną, informującą o tym, czy wystąpił
 *            jakiś błąd składniowy bądź wykonywania;
 * @param[in] charCounter - numer pierwszego znaku danego identyfikatora;
 * @return Wartość @p true, jeśli dana operacja jest zakazana.
 *         Wartość @p false, w przeciwnym wypadku.
 */
static bool checkIfNotForbidden(char *instr, bool *errorAppeared, int charCounter) {
    if (instr == NULL)
        return false;

    if (strcmp(instr, "NEW") == 0 || strcmp(instr, "DEL") == 0) {
        (*errorAppeared) = true;
        printUnreadableError(charCounter);
        return true;
    }

    return false;
}


/** @brief Wypisuje przekierowania na podany numer.
 * Wywołuje funkcję phfwdReverse, a następnie wypisuje wynik jej działania.
 * @param[in] num - wkaźnik na numer, na ktory szukane będą przekierowania;
 * @param[in] actual - wskaźnik, wskazujacy na aktualną bazę przekierowań;
 * @param[in] errorAppeared - wskaźnik na zmienną, informującą o tym, czy wystąpił
 *            jakiś błąd składniowy bądź wykonywania;
 * @param[in] operator - wskaźnik na operator operacji (w razie wystapienia błędu);
 * @param[in] charCounter - numer pierwszego znaku danego identyfikatora;
 * @param[in] memoryProblems - wskaźnik na zmienną, przechowującą informację o tym,
 *            czy wystąpiły problemy z alokacją pamięci;
 */
static void makeReverse(char *num, PfList *actual, bool *errorAppeared, char *operator, int charCounter,
                        bool *memoryProblems) {
    char const *number = convertToCharConst(num, memoryProblems);

    // gdyby wystąpiły problemy z alokacją pamięci.
    if (*memoryProblems)
        return;

    if (actual != NULL) {
        struct PhoneNumbers const *list = phfwdReverse(actual->pf, number);

        // nie znaleziono żadnego numeru lub błąd w alokacji pamięci (błąd wykonywania).
        if (list == NULL) {
            (*errorAppeared) = true;
            printMakingError(operator, charCounter);
            return;
        }

        printNumbers(list);
    }
    else {
        (*errorAppeared) = true;
        printMakingError(operator, charCounter);
    }

    free((void*)number);
}

/** @brief Wypisuje liczbę nietrywialnych numerów.
 * Wywołuje funkcję phfwdNonTrivialCount,
 * następnie wypisuje zwrócony przez nią wynik.
 * @param[in] num - set podany przez użytkownika;
 * @param[in] actual - wskaźnik wskazujący na aktualną bazę przekierowań;
 * @param[in] errorAppeared - wskaźnik na zmienną, informującą o tym, czy wystąpił
 *        jakiś błąd składniowy bądź wykonywania;
 * @param[in] operator - wskaźnik na operator operacji (w razie wystapienia błędu);
 * @param[in] charCounter - numer pierwszego znaku danego identyfikatora;
 */
static void makeNonTrivial(char *num, PfList *actual, bool *errorAppeared, char *operator, int charCounter) {

    int setLen = (int) strlen(num) - 12;
    size_t len;
    size_t result;

    if (actual != NULL) {
        if (0 > setLen)
            len = 0;
        else
            len = (size_t) setLen;

        result = phfwdNonTrivialCount(actual->pf, num, len);

        printNonTrivialResult(result);
    }
    else {
        (*errorAppeared) = true;
        printMakingError(operator, charCounter);
    }
}


/** @brief Dodaje nowe przekierowanie numeru do aktualnej bazy.
 * @param[in] numb1 - wskaźnik na numer, który jest przekierowywany;
 * @param[in] numb2 - wkaźnik na numer, na który jest przekierowanie;
 * @param[in] actual - wskaźnik, wskazujacy na aktualną bazę przekierowań;
 * @param[in] errorAppeared  - wskaźnik na zmienną, informującą o tym, czy wystąpił
 *            jakiś błąd składniowy bądź wykonywania;
 * @param[in] operator - wskaźnik na operator operacji (w razie wystapienia błędu);
 * @param[in] charCounter - numer pierwszego znaku danego identyfikatora;
 * @param[in] memoryProblems - wskaźnik na zmienną, przechowującą informację o tym,
 *            czy wystąpiły problemy z alokacją pamięci.
 */
static void addNewNumber(char *numb1, char *numb2, PfList *actual, bool *errorAppeared, char *operator, int charCounter,
                         bool *memoryProblems) {
    char const *num1 = convertToCharConst(numb1, memoryProblems);
    char const *num2 = convertToCharConst(numb2, memoryProblems);

    // gdyby wystąpiły problemy z alokacją pamięci.
    if (*memoryProblems)
        return;

    if (actual != NULL) {
        bool added = phfwdAdd(actual->pf, num1, num2);
        if (!added) {
            (*errorAppeared) = true;
            printMakingError(operator, charCounter);
        }
    }
    // brak aktualnej bazy, do której możnaby dodać przekierowanie lub błąd alokacji pamięci.
    else {
        (*errorAppeared) = true;
        printMakingError(operator, charCounter);
    }

    free((void*)num1);
    free((void*)num2);
}


/** @brief Wypisuje przekierowanie z podanego numeru.
 * @param[in] numb - numer, z którego wypisywane są przekierowania;
 * @param[in] actual - wskaźnik, wskazujacy na aktualną bazę przekierowań;
 * @param[in] errorAppeared  - wskaźnik na zmienną, informującą o tym, czy wystąpił
 *            jakiś błąd składniowy bądź wykonywania;
 * @param[in] operator - wskaźnik na operator operacji (w razie wystapienia błędu);
 * @param[in] charCounter - numer pierwszego znaku danego identyfikatora;
 * @param[in] memoryProblems - wskaźnik na zmienną, przechowującą informację o tym,
 *            czy wystąpiły problemy z alokacją pamięci.
 */
static void getNumber(char *numb, PfList *actual, bool *errorAppeared, char *operator, int charCounter,
                      bool *memoryProblems) {
    char const *num = convertToCharConst(numb, memoryProblems);

    // gdyby wystąpiły problemy z alokacją pamięci.
    if (*memoryProblems)
        return;

    if (actual != NULL) {
        struct PhoneNumbers const *number = phfwdGet(actual->pf, num);

        // w przypadku, gdy nie znajdziemy żadnego numeru lub wystapią błędy alokacji pamięci zwracamy bład.
        if (number == NULL) {
            (*errorAppeared) = true;
            printMakingError(operator, charCounter);
            return;
        }

        printNumbers(number);
    }
    else {
        (*errorAppeared) = true;
        printMakingError(operator, charCounter);
    }

    free((void*)num);
}


/** @brief Usuwa wszystkie przekierowania, których number jest prefiksem.
 * @param[in] number - wskźnik na dany numer;
 * @param[in] errorAppeared  - wskaźnik na zmienną, informującą o tym, czy wystąpił
 *            jakiś błąd składniowy bądź wykonywania;
 * @param[in] actual - wskaźnik, wskazujacy na aktualną bazę przekierowań;
 * @param[in] operator - wskaźnik na operator operacji (w razie wystapienia błędu);
 * @param[in] charCounter - numer pierwszego znaku danego identyfikatora;
 * @param[in] memoryProblems - wskaźnik na zmienną, przechowującą informację o tym,
 *            czy wystąpiły problemy z alokacją pamięci.
 */
static void delNumber(char *number, bool *errorAppeared, PfList *actual, char *operator, int charCounter,
                      bool *memoryProblems) {
    char const *num = convertToCharConst(number, memoryProblems);

    // gdyby wystąpiły problemy z alokacją pamięci.
    if (*memoryProblems)
        return;

    if (actual != NULL) {
        phfwdRemove(actual->pf, num);
    }
    // brak żądanej do unięcia bazy lub możliwe błędy alkoacji pamięci.
    else {
        (*errorAppeared) = true;
        printMakingError(operator, charCounter);
    }

    free((void*)num);
}


/** @brief Szuka bazy o danej nazwie.
 * @param[in] name - wskaźnik na nazwę szukanej bazy;
 * @param[in] base - wskźnik na strukturę, przechowującą bazy przekierowań;
 * @return Wskaźnik na odnalezioną bazę lub NULL jeśli szukanej bazy nie ma.
 */
static PfList * findRightBase(char *name, PfList *base) {
    PfList *temp = base->next;

    while (temp != NULL && temp->baseName != NULL && strcmp(temp->baseName, name) != 0) {
        temp = temp->next;
    }

    return temp;
}


/** @brief Szuka wskaźnika na bazę poprzedzającą bazę do usunięcia.
 * @param[in] name - wskaźnik na nazwę bazy, którą mamy usunąć;
 * @param[in] base - wskźnik na strukturę, przechowującą bazy przekierowań;
 * @return Wskaźnik na bazę poprzedzającą bazę do usunięcia.
 */
static PfList * findBaseToDel(char *name, PfList *base) {
    PfList *temp = base;

    if (temp == NULL)
        return NULL;

    // nigdy nie będzie on pierwszym elementem, bo na początku mamy atrapę.
    while (temp->next != NULL && temp->next->baseName != NULL && strcmp((temp->next)->baseName, name) != 0) {
        temp = temp->next;
    }

    return temp;
}


/** @brief Dodaje nową bazę o danej nazwie do struktury przechowującej bazy przekierowań
 * i ustawia ją, jako aktualną;
 * Jeśli dana baza już istnieje ustawia ją, jako aktualną.
 * @param[in] name - wskaźnik na nazwę bazy;
 * @param[in] actual - adres wskźnika, wskazujacego na aktualną bazę przekierowań;
 * @param[in] base - wskaźnik na strukturę, przechowującą bazy przekierowań;
 * @param[in] memoryProblems - wskaźnik na zmienną, przechowującą informację o tym,
 *            czy wystąpiły problemy z alokacją pamięci.
 */
static void addNewBase(char *name, PfList **actual, PfList *base, bool *memoryProblems) {
    PfList *exsistingBase = findRightBase(name, base);

    // gdy podana baza już istnieje.
    if (exsistingBase != NULL) {
        (*actual) = exsistingBase;
        return;
    }

    // gdy tworzymy nową bazę.
    char *bName = copyName(name, memoryProblems);

    PfList *new = createMainBaseElement(bName, memoryProblems);

    // gdyby wystąpiły problemy z alokacją pamięci.
    if (*memoryProblems)
        return;

    PfList *temp = base->next;
    base->next = new;
    new->next = temp;

    (*actual) = new;
}


/** @brief Usuwa bazę o danej nazwie ze struktury baz przekierowań.
 * @param[in] name - wskaźnik na nazwę bazy do usunięcia;
 * @param[in] actual - adres wskaźnika, wskazujacego na aktualną bazę przekierowań;
 * @param[in] errorAppeared  - wskaźnik na zmienną, informującą o tym, czy wystąpił
 *            jakiś błąd składniowy bądź wykonywania;
 * @param[in] base - wskaźnik na strukturę, przechowującą bazy przekierowań;
 * @param[in] operator - wskaźnik na operator operacji (w razie wystapienia błędu);
 * @param[in] charCounter - numer pierwszego znaku danego identyfikatora;
 */
static void delBase(char *name, PfList **actual, bool *errorAppeared, PfList *base, char *operator, int charCounter) {
    PfList *exsistingBase = findBaseToDel(name, base);

    // gdy podana baza nie istnieje.
    if (exsistingBase->next == NULL) {
        printMakingError(operator, charCounter);
        (*errorAppeared) = true;
        return;
    }

    // usuwanie wybranej bazy.
    PfList *temp = exsistingBase->next;
    exsistingBase->next = temp->next;

    if ((*actual) == temp)
        (*actual) = NULL;

    deleteSingleBase(temp);
}


bool parseInstruction(Instruction *tab[], int *j, PfList **actual, PfList *base, bool *errorAppeared,
                      bool *memoryProblems) {
    bool forbidden = false;

    // nie mamy żadnej instrukcji do wykonania.
    if (tab[0] == NULL || tab[1] == NULL)
        return false;

    // mamy przekierowanie numeru.
    if (strcmp(tab[0]->name, "questionMark") == 0) {
        if (strcmp(tab[1]->name, "number") == 0) {
            makeReverse(tab[1]->instr, (*actual), errorAppeared, tab[0]->instr, tab[0]->charCounter, memoryProblems);
            (*j) = 0;
            return true;
        }

        printUnreadableError(tab[1]->charCounter);
        (*errorAppeared) = true;

        return false;
    }

    // mamy funkcje nonTrivial
    if (strcmp(tab[0]->name, "at") == 0) {
        if (strcmp(tab[1]->name, "number") == 0) {
            makeNonTrivial(tab[1]->instr, (*actual), errorAppeared, tab[0]->instr, tab[0]->charCounter);
            (*j) = 0;
            return true;
        }

        printUnreadableError(tab[1]->charCounter);
        (*errorAppeared) = true;

        return false;
    }

    // mamy numer
    if (strcmp(tab[0]->name, "number") == 0) {
        if (strcmp(tab[1]->name, "questionMark") == 0) {
            getNumber(tab[0]->instr, (*actual), errorAppeared, tab[1]->instr, tab[1]->charCounter, memoryProblems);
            (*j) = 0;
            return true;
        }

        if (strcmp(tab[1]->name, "majorityMark") == 0) {
            if (tab[2] == NULL)
                return false;

            if (strcmp(tab[2]->name, "number") == 0) {
                addNewNumber(tab[0]->instr, tab[2]->instr, (*actual), errorAppeared, tab[1]->instr,
                             tab[1]->charCounter, memoryProblems);
                (*j) = 0;
                return true;
            }

            printUnreadableError(tab[2]->charCounter);
            (*errorAppeared) = true;

            return false;
        }

        printUnreadableError(tab[1]->charCounter);
        (*errorAppeared) = true;

        return false;

    }

    //instrukcja zaczynająca się od identyfikatora.
    if (strcmp(tab[0]->name, "identifier") == 0) {
        if (strcmp(tab[0]->instr, "NEW") == 0) {
            forbidden = checkIfNotForbidden(tab[1]->instr, errorAppeared, tab[1]->charCounter);

            if (forbidden)
                return false;

            if (strcmp(tab[1]->name, "identifier") == 0) {
                addNewBase(tab[1]->instr, actual, base, memoryProblems);
                (*j) = 0;
                return true;
            }

            printUnreadableError(tab[1]->charCounter);
            (*errorAppeared) = true;

            return false;
        }

        if (strcmp(tab[0]->instr, "DEL") == 0) {
            forbidden = checkIfNotForbidden(tab[1]->instr, errorAppeared, tab[1]->charCounter);

            if (forbidden)
                return false;

            if (strcmp(tab[1]->name, "identifier") == 0) {
                delBase(tab[1]->instr, actual, errorAppeared, base, tab[0]->instr, tab[0]->charCounter);
                (*j) = 0;
                return true;
            }

            if (strcmp(tab[1]->name, "number") == 0) {
                delNumber(tab[1]->instr, errorAppeared, (*actual), tab[0]->instr, tab[0]->charCounter, memoryProblems);
                (*j) = 0;
                return true;
            }

            printUnreadableError(tab[1]->charCounter);
            (*errorAppeared) = true;

            return false;
        }

        printUnreadableError(tab[0]->charCounter);
        (*errorAppeared) = true;

        return false;
    }

    printUnreadableError(tab[0]->charCounter);
    (*errorAppeared) = true;

    return false;
}
