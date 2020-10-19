#ifndef _PARSER_H
#define _PARSER_H

#include <stdbool.h>
#include "baza.h"



/** @brief Parsuje wczytywane wejście.
 * @param[in] tab - wskaźnik na tablicę z kolejnymi leksemami z wejścia;
 * @param[in] j - wskaźnik na zmienną wskazującą na indeks tablicy leksemów;
 * @param[in] actual - adres wskźnika, wskazujacego na aktualną bazę przekierowań;
 * @param[in] base - wskaźnik na strukturę, przechowującą bazy przekierowań;
 * @param[in] errorAppeared - wskaźnik na zmienną, informującą o tym, czy wystąpił
 *                       jakiś błąd składniowy bądź wykonywania;
 * @param[in] memoryProblems - wskażnik na zmienną, przechowującą informację o tym,
 *            czy wystąpiły problemy z alokacją pamięci.
 * @return Wartość @p true, jeśli udało się dopasować i wykonać, jakąś operację.
 *         Wartość @p false, jeśli zadanie to nie powiodło się.
 */
bool parseInstruction(Instruction *tab[], int *j, PfList **actual, PfList *base, bool *errorAppeared, bool *memoryProblems);


/**
 * @brief Wypisuje informacje o błędzie składniowym.
 * @param[in] n - numer znaku wejścia, którego nie możemy zinterpretować jako poprawne wejście.
 */
void printUnreadableError(int n);


/**
 * @brief Wypisuje informacje o nagłym błędzie (ERROR EOF).
 */
void printSuddenError();


#endif
