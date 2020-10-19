#ifndef _WCZYTYWANIE_H
#define _WCZYTYWANIE_H

#include <stdbool.h>
#include "baza.h"



/** @brief Wczytuje wejście, zapisując kolejne leksemy, jako elementy struktury Instruction,
 * Następnie przesyła je do funkcji parsującej.
 * @param[in] base - wskaźnik na strukturę przechowującą bazy przekierowań;
 * @param[in] errorAppeared - wskaźnik na zmienną informującą o tym,
*              czy wystąpił błąd składniowy lub wykonywania.
 * @param[in] memoryProblems - wskażnik na zmienną, przechowującą informację o tym,
 *             czy wystąpiły problemy z alokacją pamięci.
 */
void readInput(PfList *base, bool *errorAppeared, bool *memoryProblems);


#endif