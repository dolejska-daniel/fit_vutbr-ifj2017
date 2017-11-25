/**
 * Tento soubor obsahuje implementaci postfixového seznamu
 * pro Symboly a Tokeny.
 *
 * @author Daniel Dolejška (xdolej08)
 * @date 21.11.2017
 * @project IFJcode17Parser
 * @subject Formální jazyky a překladače (IFJ) - FIT VUT v Brně
 */

#include <stdio.h>
#include <stdbool.h>
#include <malloc.h>

#include "postfix_list.h"
#include "error_codes.h"

#ifndef _postfix_list_c
#define _postfix_list_c

#ifdef DEBUG_INCLUDE
#include "../parser/symtable.h"
#include "../scanner/token.h"
#else
#include "symtable.h"
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
//  PostfixList functions
//-----------------------------------------------------

PostfixListPtr PostfixList_create()
{
    PostfixListPtr l = (PostfixListPtr) malloc(sizeof(PostfixList));
    if (l == NULL)
    {
        DEBUG_ERR("postfix_list-create", "failed to mallocate PostfixList");
        return NULL;
    }

    l->first  = NULL;
    l->active = NULL;

    return l;
}

void PostfixList_destroy(PostfixListPtr *l)
{
    PostfixListPtr list = *l;
    while (list->first != NULL)
        PostfixList_deleteFirst(list);

    free(list);
    *l = NULL;
}

int PostfixList_insert(PostfixListPtr l, PostfixListItemPtr i)
{
    if (i == NULL)
    {
        return INTERNAL_ERROR;
    }

    if (l->active == NULL)
    {
        if (l->first != NULL)
        {
            DEBUG_ERR("postfix_list-insert", "active is NULL, but there are items in the list!");
            return INTERNAL_ERROR;
        }

        l->first = i;
        PostfixList_first(l);
    }
    else
    {
        l->active->next = i;
        PostfixList_next(l);
    }

    return NO_ERROR;
}

int PostfixList_insertOperator(PostfixListPtr l, TokenPtr token)
{
    PostfixListItemPtr i = PostfixListItem_createOperator(token);
    return PostfixList_insert(l, i);
}

int PostfixList_insertSymbol(PostfixListPtr l, SymbolPtr symbol)
{
    PostfixListItemPtr i = PostfixListItem_createSymbol(symbol);
    return PostfixList_insert(l, i);
}

void PostfixList_first(PostfixListPtr l)
{
    l->active = l->first;
}

void PostfixList_next(PostfixListPtr l)
{
    if (l->active != NULL)
    {
        l->active = l->active->next;
    }
}

PostfixListItemPtr PostfixList_get(PostfixListPtr l)
{
    return l->active;
}

PostfixListItemPtr PostfixList_getNext(PostfixListPtr l)
{
    if (l->active == NULL)
    {
        return NULL;
    }

    PostfixList_next(l);
    return PostfixList_get(l);
}

void PostfixList_debugPrint(PostfixListPtr l)
{
    #ifdef DEBUG_PRINT_POSTFIXLIST
    PostfixListItemPtr active = l->active;
    fprintf(stderr, "DEBUG | PostfixList (%p): {\n", l);
    PostfixList_first(l);
    while (l->active != NULL)
    {
        fprintf(stderr, "\t");
        PostfixListItem_debugPrint(PostfixList_get(l));
        PostfixList_next(l);
    }
    fprintf(stderr, "}\n");
    l->active = active;
    #endif
}

void PostfixList_deleteFirst(PostfixListPtr l)
{
    if (l->first == NULL)
    {
        return;
    }

    PostfixListItemPtr i = l->first;
    l->first = l->first->next;
    PostfixListItem_destroy(&i);
}


//-------------------------------------------------d-d-
//  PostfixListItem functions
//-----------------------------------------------------

PostfixListItemPtr PostfixListItem_createOperator(TokenPtr token)
{
    PostfixListItemPtr i = (PostfixListItemPtr) malloc(sizeof(PostfixListItem));
    if (i == NULL)
    {
        DEBUG_ERR("postfix_list-item_create", "failed to mallocate PostfixListItem");
        return NULL;
    }

    i->isOperator   = true;
    i->token        = token;
    i->symbol       = NULL;
    i->next         = NULL;

    return i;
}

PostfixListItemPtr PostfixListItem_createSymbol(SymbolPtr symbol)
{
    PostfixListItemPtr i = (PostfixListItemPtr) malloc(sizeof(PostfixListItem));
    if (i == NULL)
    {
        DEBUG_ERR("postfix_list-item_create", "failed to mallocate PostfixListItem");
        return NULL;
    }

    i->isOperator   = false;
    i->token        = NULL;
    i->symbol       = symbol;
    i->next         = NULL;

    return i;
}

void PostfixListItem_debugPrint(PostfixListItemPtr i)
{
    if (i->isOperator == true)
    {
        fprintf(stderr, "<OPERATOR, T: %s (%i), %s>\n", TokenType_toString(i->token->type), i->token->type, i->token->attr);
    }
    else
    {
        fprintf(stderr, "<SYMBOL, T: %s (%i), %s>\n", SymbolType_toString(i->symbol->type), i->symbol->type, i->symbol->value);
    }
}

void PostfixListItem_destroy(PostfixListItemPtr *i)
{
    if (*i == NULL)
        return;

    free(*i);
    *i = NULL;
}


#endif
