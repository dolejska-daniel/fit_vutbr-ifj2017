/**
 * Tento soubor obsahuje implementaci stacku charů.
 *
 * @author Daniel Dolejška (xdolej08)
 * @date 21.11.2017
 * @project IFJcode17Parser
 * @subject Formální jazyky a překladače (IFJ) - FIT VUT v Brně
 */

#include <stdio.h>
#include <stdbool.h>
#include <malloc.h>

#include "char_stack.h"
#include "error_codes.h"

#ifndef _char_stack_c
#define _char_stack_c

#ifdef DEBUG_INCLUDE
#else
#endif

#ifdef DEBUG_PRINT_ENABLED
#define DEBUG_PRINT(...) do{ fprintf( stderr, __VA_ARGS__ ); } while( 0 )
#else
#define DEBUG_PRINT(...) do{ } while ( 0 )
#endif

#ifdef DEBUG_LOG_ENABLED
#define DEBUG_LOG(...) do{ fprintf( stderr, "[%s]     %s\n", __VA_ARGS__ ); } while( 0 )
#else
#define DEBUG_LOG(...) do{ } while ( 0 )
#endif

#ifdef DEBUG_ERR_ENABLED
#define DEBUG_ERR(...) do{ fprintf( stderr, "[%s] ERR %s\n", __VA_ARGS__ ); } while( 0 )
#else
#define DEBUG_ERR(...) do{ } while ( 0 )
#endif

#define CHAR_STACK_SIZE 50

//==================================================================d=d=
//  DEKLARACE A DEFINICE ENUMERÁTORŮ A STRUKTUR
//======================================================================


//==================================================================d=d=
//  DEKLARACE FUNKCÍ
//======================================================================

CharStackPtr CharStack_create()
{
    CharStackPtr s = (CharStackPtr) malloc(sizeof(CharStack));
    if (s == NULL)
    {
        DEBUG_ERR("char_stack-create", "failed to mallocate CharStack");
        return NULL;
    }

    s->index = -1;

    return s;
}

void CharStack_destroy(CharStackPtr *s)
{
    free(*s);
    *s = NULL;
}

int CharStack_push(CharStackPtr s, char ch)
{
    if (SymbolStack_isFull(s) == true)
    {
        DEBUG_ERR("char_stack-push", "cannot push, stack is already full!");
        return INTERNAL_ERROR;
    }

    s->array[++(s->index)] = ch;
    return NO_ERROR;
}

int CharStack_pop(CharStackPtr s)
{
    if (CharStack_isEmpty(s) == true)
    {
        DEBUG_ERR("char_stack-pop", "cannot pop, stack empty!");
        return INTERNAL_ERROR;
    }

    s->array[(s->index)--] = NULL;
    return NO_ERROR;
}

char CharStack_top(CharStackPtr s)
{
    return s->array[s->index];
}

bool CharStack_isEmpty(CharStackPtr s)
{
    return s->index == -1 ? true : false;
}

bool CharStack_isFull(CharStackPtr s)
{
    return s->index == CHAR_STACK_SIZE - 1 ? true : false;
}


#endif
