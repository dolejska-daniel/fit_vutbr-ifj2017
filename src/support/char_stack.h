/**
 * Tento soubor obsahuje deklarace funkcí využité při implementaci
 * stacku charů.
 *
 * @author Daniel Dolejška (xdolej08)
 * @date 21.11.2017
 * @project IFJcode17Parser
 * @subject Formální jazyky a překladače (IFJ) - FIT VUT v Brně
 */

#include <stdbool.h>
#include <malloc.h>

#ifndef _char_stack_h
#define _char_stack_h

#ifdef DEBUG_INCLUDE
#else
#endif

#define CHAR_STACK_SIZE 50

//==================================================================d=d=
//  DEKLARACE A DEFINICE ENUMERÁTORŮ A STRUKTUR
//======================================================================

typedef struct S_CharStack
    CharStack,
    *CharStackPtr;
struct S_CharStack {
    char    array[CHAR_STACK_SIZE];
    int     index;
};


//==================================================================d=d=
//  DEKLARACE FUNKCÍ
//======================================================================

CharStackPtr CharStack_create();

void CharStack_destroy(CharStackPtr *s);

int CharStack_push(CharStackPtr s, char ch);

int CharStack_pop(CharStackPtr s);

char CharStack_top(CharStackPtr s);

bool CharStack_isEmpty(CharStackPtr s);

bool CharStack_isFull(CharStackPtr s);


#endif
