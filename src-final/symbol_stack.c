/**
 * Tento soubor obsahuje implementaci stacku symbolů.
 *
 * @author Daniel Dolejška (xdolej08)
 * @date 21.11.2017
 * @project IFJcode17Parser
 * @subject Formální jazyky a překladače (IFJ) - FIT VUT v Brně
 */

#include <stdio.h>
#include <stdbool.h>
#include <malloc.h>

#include "symbol_stack.h"
#include "error_codes.h"

#ifndef _symbol_stack_c
#define _symbol_stack_c

#ifdef DEBUG_INCLUDE
#include "../parser/symtable.h"
#else
#include "symtable.h"
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

#define SYMBOL_STACK_SIZE 50

//==================================================================d=d=
//  DEKLARACE A DEFINICE ENUMERÁTORŮ A STRUKTUR
//======================================================================


//==================================================================d=d=
//  DEKLARACE FUNKCÍ
//======================================================================

SymbolStackPtr SymbolStack_create()
{
    SymbolStackPtr s = (SymbolStackPtr) malloc(sizeof(SymbolStack));
    if (s == NULL)
    {
        DEBUG_ERR("symbol_stack-create", "failed to mallocate SymbolStack");
        return NULL;
    }

    s->index = -1;

    return s;
}

void SymbolStack_destroy(SymbolStackPtr *s)
{
    free(*s);
    *s = NULL;
}

int SymbolStack_push(SymbolStackPtr s, SymbolPtr symbol)
{
    if (SymbolStack_isFull(s))
    {
        DEBUG_ERR("symbol_stack-push", "cannot push, stack is already full!");
        return INTERNAL_ERROR;
    }

    s->array[++(s->index)] = symbol;
    return NO_ERROR;
}

int SymbolStack_pop(SymbolStackPtr s)
{
    if (SymbolStack_isEmpty(s) == true)
    {
        DEBUG_ERR("symbol_stack-pop", "cannot pop, stack empty!");
        return INTERNAL_ERROR;
    }

    s->array[(s->index)--] = NULL;
    return NO_ERROR;
}

SymbolPtr SymbolStack_top(SymbolStackPtr s)
{
    if (SymbolStack_isEmpty(s) == true)
    {
        DEBUG_ERR("symbol_stack-top", "cannot get top, stack empty!");
        return NULL;
    }

    return s->array[s->index];
}

SymbolPtr SymbolStack_afterTop(SymbolStackPtr s)
{
    if (s->index - 1 < 0)
    {
        DEBUG_ERR("symbol_stack-afterTop", "cannot get after top, stack empty or only with one symbol!");
        return NULL;
    }

    return s->array[s->index - 1];
}

bool SymbolStack_isEmpty(SymbolStackPtr s)
{
    return s->index == -1 ? true : false;
}

bool SymbolStack_isFull(SymbolStackPtr s)
{
    return s->index == SYMBOL_STACK_SIZE - 1 ? true : false;
}


#endif
