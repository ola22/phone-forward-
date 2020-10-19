#include <stdio.h>
#include <stdbool.h>
#include "wczytywanie.h"
#include "baza.h"




int main() {
    bool errorAppeared = false;
    bool memoryProblems = false;

    // tworzenie głównej bazy z atrapą.
    PfList *base = createMainBaseElement(NULL, &memoryProblems);

    readInput(base, &errorAppeared, &memoryProblems);

    deleteWholeBase(base);

    if (errorAppeared || memoryProblems)
        return 1;
    else return 0;
}
