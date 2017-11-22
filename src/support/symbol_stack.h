/**
 * Tento soubor obsahuje deklarace funkcí využité při implementaci
 * stacku symbolů.
 *
 * @author Daniel Dolejška (xdolej08)
 * @date 21.11.2017
 * @project IFJcode17Parser
 * @subject Formální jazyky a překladače (IFJ) - FIT VUT v Brně
 */

#include <stdbool.h>
#include <malloc.h>

#ifndef _symbol_stack_h
#define _symbol_stack_h

#ifdef DEBUG_INCLUDE
#include "../parser/symtable.h"
#else
#include "symtable.h"
#endif

#define SYMBOL_STACK_SIZE 50

//==================================================================d=d=
//  DEKLARACE A DEFINICE ENUMERÁTORŮ A STRUKTUR
//======================================================================

typedef struct S_SymbolStack
    SymbolStack,
    *SymbolStackPtr;
struct S_SymbolStack {
    SymbolPtr   array[SYMBOL_STACK_SIZE];
    int         index;
};


//==================================================================d=d=
//  DEKLARACE FUNKCÍ
//======================================================================

SymbolStackPtr SymbolStack_create();

void SymbolStack_destroy(SymbolStackPtr *s);

int SymbolStack_push(SymbolStackPtr s, SymbolPtr symbol);

int SymbolStack_pop(SymbolStackPtr s);

SymbolPtr SymbolStack_top(SymbolStackPtr s);

bool SymbolStack_isEmpty(SymbolStackPtr s);

bool SymbolStack_isFull(SymbolStackPtr s);


#endif
