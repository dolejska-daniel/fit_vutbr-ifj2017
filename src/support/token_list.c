/**
 * Tento soubor obsahuje implementaci dvousměrně vázaného
 * seznamu pro Tokeny.
 *
 * @author Daniel Dolejška (xdolej08)
 * @date 21.11.2017
 * @project IFJcode17Parser
 * @subject Formální jazyky a překladače (IFJ) - FIT VUT v Brně
 */

#include <stdio.h>
#include <stdbool.h>
#include <malloc.h>

#include "token_list.h"
#include "error_codes.h"

#ifndef _token_list_c
#define _token_list_c

#ifdef DEBUG_INCLUDE
#include "../scanner/token.h"
#else
#include "token.h"
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

//==================================================================d=d=
//  DEKLARACE A DEFINICE ENUMERÁTORŮ A STRUKTUR
//======================================================================


//==================================================================d=d=
//  DEKLARACE FUNKCÍ
//======================================================================

//-------------------------------------------------d-d-
//  TokenList functions
//-----------------------------------------------------

TokenListPtr TokenList_create()
{
    TokenListPtr l = (TokenListPtr) malloc(sizeof(TokenList));
    if (l == NULL)
    {
        DEBUG_ERR("tokenType_list-create", "failed to mallocate TokenList");
        return NULL;
    }

    l->first  = NULL;
    l->active = NULL;

    return l;
}

void TokenList_destroy(TokenListPtr *l)
{
    TokenListPtr list = *l;
    while (list->first != NULL)
        TokenList_deleteFirst(list);

    free(list);
    *l = NULL;
}

int TokenList_insert(TokenListPtr l, TokenPtr token)
{
    TokenListItemPtr i = TokenListItem_create(token);
    if (i == NULL)
    {
        return INTERNAL_ERROR;
    }

    if (l->active == NULL)
    {
        if (l->first != NULL)
        {
            DEBUG_ERR("token_list-insert", "active is NULL, but there are items in the list!");
            return INTERNAL_ERROR;
        }

        l->first = i;
        TokenList_first(l);
    }
    else
    {
        l->active->next = i;
        TokenList_next(l);
    }

    return NO_ERROR;
}

void TokenList_first(TokenListPtr l)
{
    l->active = l->first;
}

void TokenList_next(TokenListPtr l)
{
    if (l->active != NULL)
    {
        l->active = l->active->next;
    }
}

TokenListItemPtr TokenList_get(TokenListPtr l)
{
    return l->active;
}

TokenListItemPtr TokenList_getNext(TokenListPtr l)
{
    if (l->active == NULL)
    {
        return NULL;
    }

    TokenList_next(l);
    return TokenList_get(l);
}

TokenPtr TokenList_findFirstToken(TokenListPtr l, TokenType type)
{
    TokenList_first(l);
    while (TokenList_get(l) != NULL && TokenList_get(l)->token->type != type)
        TokenList_next(l);

    return TokenList_get(l)->token;
}

TokenPtr TokenList_findNextToken(TokenListPtr l, TokenType type)
{
    while (TokenList_get(l) != NULL && TokenList_get(l)->token->type != type)
        TokenList_next(l);

    return TokenList_get(l)->token;
}

void TokenList_deleteFirst(TokenListPtr l)
{
    if (l->first == NULL)
    {
        return;
    }

    TokenListItemPtr i = l->first;
    l->first = l->first->next;
    TokenListItem_destroy(&i);
}


//-------------------------------------------------d-d-
//  TokenListItem functions
//-----------------------------------------------------

TokenListItemPtr TokenListItem_create(TokenPtr token)
{
    TokenListItemPtr i = (TokenListItemPtr) malloc(sizeof(TokenListItem));
    if (i == NULL)
    {
        DEBUG_ERR("token_list-item_create", "failed to mallocate TokenTypeListItem");
        return NULL;
    }

    i->token = token;
    i->next  = NULL;

    return i;
}

void TokenListItem_destroy(TokenListItemPtr *i)
{
    free(*i);
    *i = NULL;
}

#endif
