#include <stdio.h>
#include "baza.h"
#include "phone_forward.h"


#define SIZE    20




Instruction * createNewElement(char *nam, int counter, bool *memoryProblems) {
    Instruction *new = malloc(sizeof(Instruction));
    char *instru = malloc(SIZE * sizeof(char));

    // wystąpiły problemy z alokacją pamięci.
    if (new == NULL || instru == NULL) {
        (*memoryProblems) = true;
        return NULL;
    }

    new->name = nam;
    new->charCounter = counter;
    new->instr = instru;

    return new;
}


void deleteInstruction(Instruction *toDel) {
    if (toDel == NULL)
        return;

    if (toDel->instr != NULL)
        free(toDel->instr);
    toDel->instr = NULL;

    free(toDel);
}


PfList * createMainBaseElement(char *name, bool *memoryProblems) {
    PfList *new = malloc(sizeof(PfList));
    struct PhoneForward *newPf = phfwdNew();

    // wystąpiły problemy z alokacją pamięci.
    if (newPf == NULL || new == NULL) {
        (*memoryProblems) = true;
        return NULL;
    }

    new->pf = newPf;
    new->baseName = name;
    new->next = NULL;

    return new;
}


void deleteSingleBase(PfList *temp) {
    if (temp == NULL)
        return;

    if (temp->baseName != NULL)
        free(temp->baseName);
    temp->baseName = NULL;

    if (temp->pf != NULL)
        phfwdDelete(temp->pf);
    temp->pf = NULL;

    free(temp);
}


void deleteWholeBase(PfList *base) {
    if (base != NULL) {
        while (base != NULL) {
            PfList *temp = base;
            base = base->next;

            deleteSingleBase(temp);
        }
    }
}
