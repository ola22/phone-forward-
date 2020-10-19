#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "wczytywanie.h"
#include "parser.h"
#include "baza.h"

#define SIZE    20
#define INSTR_AMOUNT 3




/** @brief Pobiera ze standardowego wejścia kolejny znak.
 * Sprawdza, czy nie jest on eofem.
 * @param[in] errorAppeared - wskaźnik na zmienną, informującą o tym, czy wystąpił
 *            jakiś błąd składniowy bądź wykonywania.
 * @param[in] error - zmienna wskazująca na to, czy w przypadku wczytania znaku końca pliku
 *            program ma odrazu wypisać błąd składniowy;
 * @param[in] whereIsError - zmienna mówiąca, który znak wejścia jest błędny;
 * @param[in] suddenError - zmienna mówiąca, czy w przypadku wczytania znaku końca pliku
 *            program ma wypisać błąd: ERROR EOF.
 * @return Wczytany z wejścia znak.
 */
static char getNextChar(bool *errorAppeared, bool error, int whereIsError, bool suddenError) {
    int cInt = getchar();

    // sprawdzamy, czy pobrany znak nie jest eofem.
    if (cInt == EOF) {
        if (error && !suddenError) {
            printUnreadableError(whereIsError);
            (*errorAppeared) = true;
        }

        if (suddenError) {
            printSuddenError();
            (*errorAppeared) = true;
        }

        return EOF;
    }

    char c = (char)cInt;

    return c;
}


/** @brief Wczytuje z wejścia cały komentarz.
 * @param[in] charCounter - wskaźnik na licznik znaków wejścia;
 * @param[in] errorAppeared - wskaźnik na zmienną, informującą o tym, czy wystąpił
 *            jakiś błąd składniowy bądź wykonywania;
 * @param[in] suddenError - zmienna mówiąca, czy w przypadku wczytania znaku końca pliku
 *            program ma wypisać błąd: ERROR EOF.
 */
static void getWholeComment(int *charCounter, bool *errorAppeared, bool suddenError) {
    int error = (*charCounter - 1);
    char c = getNextChar(errorAppeared, true, error, suddenError);
    (*charCounter)++;

    if (c == '$') {
        c = getNextChar(errorAppeared, true, error, false);
        (*charCounter)++;

        if (c == '$')
            return;
    }

    // wczytywanie całego komentarza.
    while (c != '$' && !(*errorAppeared)) {
        while (c != '$' && !(*errorAppeared)) {
            (*charCounter)++;
            c = getNextChar(errorAppeared, true, error, suddenError);
        }

        if (c == EOF)
            break;

        // pobranie drugiego znaku w celu sprawdzenia, czy komentarz się kończy.
        c = getNextChar(errorAppeared, true, error, false);
        (*charCounter)++;
    }
}


/** @brief Sprawdza, czy przekazany znak c jest początkiem komentarza.
 * Jeśli tak wczytuje z wejścia cały komentarz.
 * @param[in] c - sprawdzany znak;
 * @param[in] charCounter - wskaźnik na licznik znaków wejścia;
 * @param[in] errorAppeared - wskaźnik na zmienną, informującą o tym, czy wystąpił
 *            jakiś błąd składniowy bądź wykonywania.
 * @param[in] suddenError - zmienna mówiąca, czy w przypadku wczytania znaku końca pliku
 *            program ma wypisać błąd: ERROR EOF.
 * @return Wartość @p true, jeśli dany znak był początkiem komentarza.
 *         Wartość @p false, jeśli nim nie był.
 */
static bool checkIfComment(char c, int *charCounter, bool *errorAppeared, bool suddenError) {
    if (c == '$') {
        c = getNextChar(errorAppeared, true, (*charCounter), false);
        (*charCounter)++;

        // mamy początek komentarza
        if (c == '$') {
            getWholeComment(charCounter, errorAppeared, suddenError);
            return true;
        }
        // po jednym znaku $ nie mamy drugiego, co jest błędem.
        else {
            if (c != EOF)
                printUnreadableError((*charCounter) - 1);

            (*errorAppeared) = true;
            return false;
        }
    }

    return false;
}


/** @brief Wczytuje z wejscia cały identyfikator.
 * @param[in] c - pierwszy znak identyfikatora;
 * @param[in] charCounter - wskaźnik na licznik znaków wejścia;
 * @param[in] errorAppeared - wskaźnik na zmienną, informującą o tym, czy wystąpił
 *            jakiś błąd składniowy bądź wykonywania;
 * @param[in] memoryProblems - wskażnik na zmienną, przechowującą informację o tym,
 *            czy wystąpiły problemy z alokacją pamięci;
 * @param[in] c2 - wskaźnik na zmienną,przechowującą wczytany z wejścia znak
 *            nie będący już identyfikatorem;
 * @param[in] tab - wskaźnik tablicy wczytanych dotąd leksemów.
 * @return Wskźnik na leksem (element struktury Instruction),
 *         zawierający wczytany identyfikator.
 */
static Instruction * readIdentifier(char c, int *charCounter, bool *errorAppeared, bool *memoryProblems, char *c2,
                                    Instruction *tab[]) {
    Instruction *identifier = createNewElement("identifier", (*charCounter), memoryProblems);
    char *instruct = identifier->instr;
    bool isComment = false;
    bool suddenError = false;
    unsigned int i = 0;
    size_t n = SIZE;

    // gdyby wystąpiły problemy z alokacją pamięci.
    if (*memoryProblems)
        return NULL;

    // jeśli jak dotąd mamy poprawne dane wejsciowe,zmienna suddenError przyjmie wartość true.
    if (tab[1] == NULL && tab[2] == NULL && tab[0] != NULL)
        if (strcmp(tab[0]->instr, "NEW") == 0 || strcmp(tab[0]->instr, "DEL") == 0)
            suddenError = true;


    while (((c <= 'Z' && c >= 'A') || (c <= 'z' && c >= 'a') || c == '$' || (c <= '9' && c >= '0'))
           && !(*errorAppeared)) {
        isComment = checkIfComment(c, charCounter, errorAppeared, suddenError);
        // jeśli natrafimy na komentarz, kończymy wczytywanie danego identyfikatora.
        if (isComment || (*errorAppeared))
            break;

        if (i == n) {
            instruct = realloc(instruct, (n + SIZE) * sizeof(char));

            // gdyby wystąpiły problemy z alokacją pamięci.
            if (instruct == NULL) {
                (*memoryProblems) = true;
                return NULL;
            }

            n += SIZE;
        }

        if (c != '$') {
            instruct[i] = c;
            i++;
        }

        c = getNextChar(errorAppeared, false, 0, false);
        (*charCounter)++;
    }

    if (isComment) {
        c = getNextChar(errorAppeared, false, 0, false);
        (*charCounter)++;
    }

    if (i == n) {
        instruct = realloc(instruct, (n + 1) * sizeof(char));

        // gdyby wystąpiły problemy z alokacją pamięci.
        if (instruct == NULL) {
            (*memoryProblems) = true;
            return NULL;
        }
    }

    instruct[i] = '\0';
    identifier->instr = instruct;

    // przypisanie do zmiennej c2 ostatniego wczytanego znaku, nie należącego już do identyfikatora.
    (*c2) = c;

    return identifier;
}


/** @brief Wczytuje z wejścia cały numer.
 * @param[in] c - pierwszy znak numeru.
 * @param[in] charCounter - wskaźnik na licznik znaków wejścia;
 * @param[in] errorAppeared  - wskaźnik na zmienną, informującą o tym, czy wystąpił
 *            jakiś błąd składniowy bądź wykonywania;
 * @param[in] memoryProblems - wskażnik na zmienną, przechowującą informację o tym,
 *            czy wystąpiły problemy z alokacją pamięci;
 * @param[in] c2 - wskaźnik na zmienną,przechowującą wczytany z wejścia znak
 *            nie będący już numerem;
 * @param[in] tab - wskaźnik tablicy wczytanych dotąd leksemów.
 * @return Wskaźnik na leksem (element struktury Instruction),
 *         zawierający wczytany numer.
 */
static Instruction * readNumber(char c, int *charCounter, bool *errorAppeared, bool *memoryProblems, char *c2,
                                Instruction *tab[]) {
    Instruction *number = createNewElement("number", (*charCounter), memoryProblems);
    char *instruct = number->instr;
    bool isComment = false;
    bool suddenError = false;
    unsigned int i = 0;
    size_t n = SIZE;

    // gdyby wystąpiły problemy z alokacją pamięci.
    if (*memoryProblems)
        return NULL;

    // jeśli jak dotąd mamy poprawne dane wejsciowe,zmienna suddenError przyjmie wartość true.
    if (tab[1] == NULL && tab[2] == NULL && tab[0] != NULL)
        if (strcmp(tab[0]->name, "questionMark") == 0 || strcmp(tab[0]->instr, "DEL") == 0)
            suddenError = true;

    if (tab[0] == NULL && tab[1] == NULL && tab[2] == NULL)
        suddenError = true;

    if (tab[2] == NULL && tab[0] != NULL && tab[1] != NULL)
        if (strcmp(tab[0]->name, "number") == 0 || strcmp(tab[1]->name, "majorityMark") == 0)
            suddenError = true;


    while ((c == '$' || (c <= ';' && c >= '0')) && !(*errorAppeared)) {
        isComment = checkIfComment(c, charCounter, errorAppeared, suddenError);

        // jeśli natrafimy na komentarz, kończymy wczytywanie danego numeru.
        if (isComment || (*errorAppeared))
            break;

        if (i == n) {
            instruct = realloc(instruct, (n + SIZE) * sizeof(char));

            // gdyby wystąpiły problemy z alokacją pamięci.
            if (instruct == NULL) {
                (*memoryProblems) = true;
                return NULL;
            }

            n += SIZE;
        }

        if (c != '$') {
            instruct[i] = c;
            i++;
        }

        c = getNextChar(errorAppeared, false, 0, false);
        (*charCounter)++;
    }

    if (isComment) {
        c = getNextChar(errorAppeared, false, 0, false);
        (*charCounter)++;
    }

    if (i == n) {
        instruct = realloc(instruct, (n + 1) * sizeof(char));

        // gdyby wystąpiły problemy z alokacją pamięci.
        if (instruct == NULL) {
            (*memoryProblems) = true;
            return NULL;
        }
    }

    instruct[i] = '\0';
    number->instr = instruct;

    // przypisanie do zmiennej c2 ostatniego wczytanego znaku, nie należącego już do numeru.
    (*c2) = c;

    return number;
}


/** @brief Tworzy leksem, będący znakiem zapytania.
 * @param[in] c - znak zapytania.
 * @param[in] charCounter - zmienna, mówiąca, którym z kolei znakiem wejścia jest ropatrywany znak zapytania;
 * @param[in] memoryProblems - wskażnik na zmienną, przechowującą informację o tym,
 *            czy wystąpiły problemy z alokacją pamięci;
 * @return Wskaźnik na leksem (element struktury Instruction),
 *         zawierający znak zapytania.
 */
static Instruction * createQuestionMark(char c, int charCounter, bool *memoryProblems) {
    Instruction *questionMark = createNewElement("questionMark", charCounter, memoryProblems);

    // gdyby wystąpiły problemy z alokacją pamięci.
    if (*memoryProblems)
        return NULL;

    questionMark->instr[0] = c;
    questionMark->instr[1] = '\0';

    return questionMark;
}


/** @brief Tworzy leksem, będący znakiem większości.
 * @param[in] c - znak większości.
 * @param[in] charCounter - zmienna, mówiąca, którym z kolei znakiem wejścia jest ropatrywany znak większości;
 * @param[in] memoryProblems - wskażnik na zmienną, przechowującą informację o tym,
 *            czy wystąpiły problemy z alokacją pamięci;
 * @return Wskaźnik na leksem (element struktury Instruction),
 *         zawierający znak większości.
 */
static Instruction * createMajorityMark(char c, int charCounter, bool *memoryProblems) {
    Instruction *majorityMark = createNewElement("majorityMark", charCounter, memoryProblems);

    // gdyby wystąpiły problemy z alokacją pamięci.
    if (*memoryProblems)
        return NULL;

    majorityMark->instr[0] = c;
    majorityMark->instr[1] = '\0';

    return majorityMark;
}


/** @brief Tworzy leksem, będący znakiem @
 * @param[in] c - znak @;
 * @param[in] charCounter - zmienna, mówiąca, którym z kolei znakiem wejścia jest ropatrywany znak @;
 * @param[in] memoryProblems - wskażnik na zmienną, przechowującą informację o tym,
 *            czy wystąpiły problemy z alokacją pamięci;
 * @return - Wskaźnik na leksem (element struktury Instruction),
 *           zawierający znak @.
 */
static Instruction * createAtSign(char c, int charCounter, bool *memoryProblems) {
    Instruction *at = createNewElement("at", charCounter, memoryProblems);

    // gdyby wystąpiły problemy z alokacją pamięci.
    if (*memoryProblems)
        return NULL;

    at->instr[0] = c;
    at->instr[1] = '\0';

    return at;
}


/** @brief Usuwa trzyelementową tablicę leksemów.
 * @param[in] tab - wskaźnik tablicy do usunięcia.
 */
static void cleanTab(Instruction *tab[]) {
    for (int i = 0; i < INSTR_AMOUNT; i++) {
        if (tab[i] != NULL)
            deleteInstruction(tab[i]);

        tab[i] = NULL;
    }
}


/** @brief Ustawia wartości tablicy leksemów na nulle.
 * @param[in] tab - wskaźnik tablicy do wypełnienia nullami.
 */
static void nullTab(Instruction *tab[]) {
    for (int i = 0; i < INSTR_AMOUNT; i++) {
        tab[i] = NULL;
    }
}


/** @brief Zwiększa liczniki w każdym obrocie pętli w funkcji readInput.
 * @param[in] j - wskaźnik na zmienną wskazującą na indeks tablicy leksemów;
 * @param[in] charCounter - wskaźnik na licznik znaków wejścia;
 * @param[in] errorAppeared  - wskaźnik na zmienną, informującą o tym, czy wystąpił
 *            jakiś błąd składniowy bądź wykonywania;
 * @param[in] jChange - zmienna mówiąca o tym, czy należy przesunąć indeks w tablicy lesemów;
 * @return  Kolejny znak wczytany z wejścia.
 */
static char makeLoopTurn(int *j, int *charCounter, bool *errorAppeared, bool jChange) {
    char c = getNextChar(errorAppeared, false, 0, false);
    (*charCounter)++;

    if (jChange)
        (*j)++;

    return c;
}


void readInput(PfList *base, bool *errorAppeared, bool *memoryProblems) {
    Instruction *tab[INSTR_AMOUNT];
    nullTab(tab);
    PfList *actual = NULL;
    int charCounter = 1;
    bool instrMade = false;
    char c = getNextChar(errorAppeared, false, 0, false);
    int j = 0;

    while (!(*errorAppeared) && !(*memoryProblems)) {
        instrMade = false;

        // wczytany znak wskazuje na identyfikator.
        if ((c <= 'Z' && c >= 'A') || (c <= 'z' && c >= 'a')) {
            char c2;
            Instruction *identifier = readIdentifier(c, &charCounter, errorAppeared, memoryProblems, &c2, tab);

            tab[j] = identifier;
            instrMade = parseInstruction(tab, &j, &actual, base, errorAppeared, memoryProblems);

            if (instrMade)
                cleanTab(tab);
            else
                j++;

            c = c2;


            continue;
        }

        // wczytany znak wskazuje na numer.
        if (c <= ';' && c >= '0') {
            char c2;
            Instruction *number = readNumber(c, &charCounter, errorAppeared, memoryProblems, &c2, tab);

            tab[j] = number;
            instrMade = parseInstruction(tab, &j, &actual, base, errorAppeared, memoryProblems);

            if (instrMade)
                cleanTab(tab);
            else
                j++;

            c = c2;


            continue;
        }

        if (c == '@') {
            Instruction *at = createAtSign(c, charCounter, memoryProblems);

            tab[j] = at;
            instrMade = parseInstruction(tab, &j, &actual, base, errorAppeared, memoryProblems);

            if (instrMade) {
                cleanTab(tab);
                c = makeLoopTurn(&j, &charCounter, errorAppeared, false);
            } else
                c = makeLoopTurn(&j, &charCounter, errorAppeared, true);

            continue;
        }

        if (c == '?') {
            Instruction *questionMark = createQuestionMark(c, charCounter, memoryProblems);

            tab[j] = questionMark;
            instrMade = parseInstruction(tab, &j, &actual, base, errorAppeared, memoryProblems);

            if (instrMade) {
                cleanTab(tab);
                c = makeLoopTurn(&j, &charCounter, errorAppeared, false);
            } else
                c = makeLoopTurn(&j, &charCounter, errorAppeared, true);

            continue;
        }

        if (c == '>') {
            Instruction *majorityMark = createMajorityMark(c, charCounter, memoryProblems);

            tab[j] = majorityMark;
            instrMade = parseInstruction(tab, &j, &actual, base, errorAppeared, memoryProblems);

            if (instrMade) {
                cleanTab(tab);
                c = makeLoopTurn(&j, &charCounter, errorAppeared, false);
            } else
                c = makeLoopTurn(&j, &charCounter, errorAppeared, true);

            continue;
        }

        // wczytany znak wskazuje na komentarz.
        if (c == '$') {
            checkIfComment(c, &charCounter, errorAppeared, true);
            c = makeLoopTurn(&j, &charCounter, errorAppeared, false);
            continue;
        }

        // został wczytany biały znak.
        if (c == ' ' || c == '\n' || c == '\r' || c == '\t') {
            c = makeLoopTurn(&j, &charCounter, errorAppeared, false);
            continue;
        }

        if (c == EOF) {
            if (tab[0] != NULL) {
                printSuddenError();
                (*errorAppeared) = true;
            }

            break;
        }

        // wczytanego znaku napewno nie da się zinterpretować, jako poprawne wejście.
        printUnreadableError(charCounter);
        (*errorAppeared) = true;
        cleanTab(tab);
    }

    if ((*errorAppeared))
        cleanTab(tab);
}






















