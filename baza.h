#ifndef _BAZA_H
#define _BAZA_H

#include <stdbool.h>



/**
 * wewnętrzna struktura zawierająca pojedynczy leksem.
 */
struct instruction;

typedef struct instruction Instruction;

struct instruction {
    char *name; // nazwa leksemu (rodzaj)
    char *instr; // tablica z leksemem
    int charCounter; // zmienna wskazująca na to, który to znak wejścia
};

/**
 * Struktura przechowująca bazy przkierowań.
 */
struct pfList;

typedef struct pfList PfList;

struct pfList{
    struct PhoneForward *pf;
    char *baseName;
    PfList* next;
};


/** @brief Tworzy pojedynczy element struktury baz przekierowań.
 * @param[in] name - wskażnik na tablicę przechowującą nazwę bazy;
 * @param[in] memoryProblems - wskażnik na zmienną, przechowującą informację o tym,
 *            czy wystąpiły problemy z alokacją pamięci.
 * @return wskaźnik na nowo utworzoną bazę.
 */
PfList * createMainBaseElement(char *name, bool *memoryProblems);


/** @brief Usuwa całą strukturę przechowującą bazy przekierowań.
 * @param[in] base
 */
void deleteWholeBase(PfList *base);


/** @brief Usuwa pojedynczą bazę przekierowań.
 * @param[in] temp - wskaźnik na bazę do usunięcia.
 */
void deleteSingleBase(PfList *temp);


/** @brief Tworzy nowy element struktury instruction, pojedynczy leksem.
 * @param[in] nam - wskażnik na tablicę zwierającą nazwę leksemu;
 * @param[in] counter - numer bajtu pierwszego wczytanego znaku danego leksemu;
 * @param[in] memoryProblems - wskażnik na zmienną, przechowującą informację o tym,
 *            czy wystąpiły problemy z alokacją pamięci.
 * @return wskaźnik na nowo utworzony element.
 */
Instruction * createNewElement(char *nam, int counter, bool *memoryProblems);


/** @brief Usuwa pojedynczy element struktury Instruction.
 * @param[in] toDel - wskaźnik na element do usunięcia.
 */
void deleteInstruction(Instruction *toDel);


#endif
