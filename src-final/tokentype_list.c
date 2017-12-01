/**
 * Tento soubor obsahuje implementaci seznamu typů tokenů.
 *
 * @author Daniel Dolejška (xdolej08)
 * @date 21.11.2017
 * @project IFJcode17Parser
 * @subject Formální jazyky a překladače (IFJ) - FIT VUT v Brně
 */

#include <stdio.h>

#include "tokentype_list.h"
#include "error_codes.h"

#ifndef _tokenType_list_c
#define _tokenType_list_c

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
//  TokenTypeList functions
//-----------------------------------------------------

TokenTypeListPtr TokenTypeList_create()
{
    TokenTypeListPtr l = (TokenTypeListPtr) malloc(sizeof(TokenTypeList));
    if (l == NULL)
    {
        DEBUG_ERR("tokenType_list-create", "failed to mallocate TokenTypeList");
        return NULL;
    }

    l->first  = NULL;
    l->active = NULL;

    return l;
}

void TokenTypeList_destroy(TokenTypeListPtr *l)
{
    TokenTypeListPtr list = *l;
    while (list->first != NULL)
        TokenTypeList_deleteFirst(list);

    free(list);
    *l = NULL;
}

int TokenTypeList_insert(TokenTypeListPtr l, TokenType type)
{
    TokenTypeListItemPtr i = TokenTypeListItem_create(type);
    if (i == NULL)
    {
        return INTERNAL_ERROR;
    }

    if (l->active == NULL)
    {
        if (l->first != NULL)
        {
            DEBUG_ERR("tokenType_list-insert", "active is NULL, but there are items in the list!");
            return INTERNAL_ERROR;
        }

        l->first = i;
        TokenTypeList_first(l);
    }
    else
    {
        l->active->next = i;
        TokenTypeList_next(l);
    }

    return NO_ERROR;
}

void TokenTypeList_first(TokenTypeListPtr l)
{
    l->active = l->first;
}

void TokenTypeList_next(TokenTypeListPtr l)
{
    if (l->active != NULL)
    {
        l->active = l->active->next;
    }
}

TokenTypeListItemPtr TokenTypeList_get(TokenTypeListPtr l)
{
    return l->active;
}

TokenTypeListItemPtr TokenTypeList_getNext(TokenTypeListPtr l)
{
    if (l->active == NULL)
    {
        return NULL;
    }

    TokenTypeList_next(l);
    return TokenTypeList_get(l);
}

void TokenTypeList_deleteFirst(TokenTypeListPtr l)
{
    if (l->first == NULL)
    {
        return;
    }

    TokenTypeListItemPtr i = l->first;
    l->first = l->first->next;
    TokenTypeListItem_destroy(&i);
}


//-------------------------------------------------d-d-
//  TokenTypeListItem functions
//-----------------------------------------------------

TokenTypeListItemPtr TokenTypeListItem_create(TokenType type)
{
    TokenTypeListItemPtr i = (TokenTypeListItemPtr) malloc(sizeof(TokenTypeListItem));
    if (i == NULL)
    {
        DEBUG_ERR("tokenType_list-item_create", "failed to mallocate TokenTypeListItem");
        return NULL;
    }

    i->type = type;
    i->next = NULL;

    return i;
}

void TokenTypeListItem_destroy(TokenTypeListItemPtr *i)
{
    free(*i);
    *i = NULL;
}

#endif
