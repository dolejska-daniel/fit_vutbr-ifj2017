/**
 * Tento soubor obsahuje implementaci stacku Tokenů.
 *
 * @author Daniel Dolejška (xdolej08)
 * @date 21.11.2017
 * @project IFJcode17Parser
 * @subject Formální jazyky a překladače (IFJ) - FIT VUT v Brně
 */

#include <stdio.h>
#include <stdbool.h>
#include <malloc.h>

#include "token_stack.h"
#include "error_codes.h"

#ifndef _token_stack_c
#define _token_stack_c

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

#define TOKEN_STACK_SIZE 50

//==================================================================d=d=
//  DEKLARACE A DEFINICE ENUMERÁTORŮ A STRUKTUR
//======================================================================


//==================================================================d=d=
//  DEKLARACE FUNKCÍ
//======================================================================

TokenStackPtr TokenStack_create()
{
    TokenStackPtr s = (TokenStackPtr) malloc(sizeof(TokenStack));
    if (s == NULL)
    {
        DEBUG_ERR("char_stack-create", "failed to mallocate TokenStack");
        return NULL;
    }

    for (int i = 0; i < TOKEN_STACK_SIZE; i++)
        s->array[i] = NULL;
    s->index = -1;

    return s;
}

void TokenStack_destroy(TokenStackPtr *s)
{
    free(*s);
    *s = NULL;
}

int TokenStack_push(TokenStackPtr s, TokenPtr token)
{
    if (TokenStack_isFull(s) == true)
    {
        DEBUG_ERR("char_stack-push", "cannot PUSH, stack full!");
        return INTERNAL_ERROR;
    }

    s->array[++(s->index)] = token;
    return NO_ERROR;
}

int TokenStack_pop(TokenStackPtr s)
{
    if (TokenStack_isEmpty(s) == true)
    {
        DEBUG_ERR("char_stack-pop", "cannot POP, stack empty!");
        return INTERNAL_ERROR;
    }

    s->array[(s->index)--] = NULL;
    return NO_ERROR;
}

TokenPtr TokenStack_top(TokenStackPtr s)
{
    return s->array[s->index];
}

bool TokenStack_isEmpty(TokenStackPtr s)
{
    return s->index == -1 ? true : false;
}

bool TokenStack_isFull(TokenStackPtr s)
{
    return s->index == TOKEN_STACK_SIZE - 1 ? true : false;
}

void TokenStack_debugPrint(TokenStackPtr s)
{
    #ifdef DEBUG_PRINT_TOKENSTACK
    fprintf(stderr, "DEBUG | TokenStack (%p): {\n", s);
    for (int index = TOKEN_STACK_SIZE - 1; index >= 0; index--)
    {
        if (s->array[index] != NULL)
        {
            fprintf(stderr, "\t%2i: <(%p): %s (%i), %s>\n", index, s->array[index], TokenType_toString(s->array[index]->type), s->array[index]->type, s->array[index]->attr);
        }
        else
        {
            fprintf(stderr, "\t%2i:\n", index);
        }
    }
    fprintf(stderr, "}\n");
    #endif
}


#endif
