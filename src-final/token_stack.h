/**
 * Tento soubor obsahuje deklarace funkcí využité při implementaci
 * stacku Tokenů.
 *
 * @author Daniel Dolejška (xdolej08)
 * @date 21.11.2017
 * @project IFJcode17Parser
 * @subject Formální jazyky a překladače (IFJ) - FIT VUT v Brně
 */

#include <stdbool.h>
#include <malloc.h>

#ifndef _token_stack_h
#define _token_stack_h

#ifdef DEBUG_INCLUDE
#include "../scanner/token.h"
#else
#include "token.h"
#endif

#define TOKEN_STACK_SIZE 50

//==================================================================d=d=
//  DEKLARACE A DEFINICE ENUMERÁTORŮ A STRUKTUR
//======================================================================

typedef struct S_TokenStack
    TokenStack,
    *TokenStackPtr;
struct S_TokenStack {
    TokenPtr array[TOKEN_STACK_SIZE];
    int      index;
};


//==================================================================d=d=
//  DEKLARACE FUNKCÍ
//======================================================================

TokenStackPtr TokenStack_create();

void TokenStack_destroy(TokenStackPtr *s);

int TokenStack_push(TokenStackPtr s, TokenPtr token);

int TokenStack_pop(TokenStackPtr s);

TokenPtr TokenStack_top(TokenStackPtr s);

bool TokenStack_isEmpty(TokenStackPtr s);

bool TokenStack_isFull(TokenStackPtr s);

void TokenStack_debugPrint(TokenStackPtr s);


#endif
