/**
 * Tento soubor obsahuje deklarace funkcí využité při implementaci
 * dvousměrně vázaného seznamu pro Tokeny.
 *
 * @author Daniel Dolejška (xdolej08)
 * @date 21.11.2017
 * @project IFJcode17Parser
 * @subject Formální jazyky a překladače (IFJ) - FIT VUT v Brně
 */

#include <stdbool.h>

#ifndef _token_list_h
#define _token_list_h

#ifdef DEBUG_INCLUDE
#include "../scanner/token.h"
#else
#include "token.h"
#endif

//==================================================================d=d=
//  DEKLARACE A DEFINICE ENUMERÁTORŮ A STRUKTUR
//======================================================================

typedef struct S_TokenListItem
    TokenListItem,
    *TokenListItemPtr;
struct S_TokenListItem {
    TokenPtr         token;      ///< Ukazatel na token
    TokenListItemPtr next;       ///< Ukazatel na následující prvek v seznamu
};

typedef struct S_TokenList
    TokenList,
    *TokenListPtr;
struct S_TokenList {
    TokenListItemPtr first;   ///< Ukazatel na první prvek seznamu
    TokenListItemPtr active;  ///< Ukazatel na aktivní prvek seznamu
};


//==================================================================d=d=
//  DEKLARACE FUNKCÍ
//======================================================================

//-------------------------------------------------d-d-
//  TokenList functions
//-----------------------------------------------------

TokenListPtr TokenList_create();

void TokenList_destroy(TokenListPtr *l);

int TokenList_insert(TokenListPtr l, TokenPtr token);

void TokenList_first(TokenListPtr l);

void TokenList_next(TokenListPtr l);

TokenListItemPtr TokenList_get(TokenListPtr l);

TokenListItemPtr TokenList_getNext(TokenListPtr l);

TokenPtr TokenList_findFirstToken(TokenListPtr l, TokenType type);

TokenPtr TokenList_findNextToken(TokenListPtr l, TokenType type);

void TokenList_deleteFirst(TokenListPtr l);


//-------------------------------------------------d-d-
//  TokenListItem functions
//-----------------------------------------------------

TokenListItemPtr TokenListItem_create(TokenPtr token);

void TokenListItem_destroy(TokenListItemPtr *i);

#endif
