/**
 * Tento soubor obsahuje deklarace funkcí využité při implementaci
 * seznamu typů tokenů.
 *
 * @author Daniel Dolejška (xdolej08)
 * @date 21.11.2017
 * @project IFJcode17Parser
 * @subject Formální jazyky a překladače (IFJ) - FIT VUT v Brně
 */

#include <stdbool.h>
#include <malloc.h>

#ifndef _tokenType_list_h
#define _tokenType_list_h

#ifdef DEBUG_INCLUDE
#include "../scanner/token.h"
#else
#include "token.h"
#endif

//==================================================================d=d=
//  DEKLARACE A DEFINICE ENUMERÁTORŮ A STRUKTUR
//======================================================================

typedef struct S_TokenTypeListItem
    TokenTypeListItem,
    *TokenTypeListItemPtr;
struct S_TokenTypeListItem {
    TokenType            type;  ///< Typ tokenu
    TokenTypeListItemPtr next;  ///< Ukazatel na následující prvek v seznamu
};

typedef struct S_TokenTypeList
    TokenTypeList,
    *TokenTypeListPtr;
struct S_TokenTypeList {
    TokenTypeListItemPtr first;   ///< Ukazatel na první prvek seznamu
    TokenTypeListItemPtr active;  ///< Ukazatel na aktivní prvek seznamu
};


//==================================================================d=d=
//  DEKLARACE FUNKCÍ
//======================================================================

//-------------------------------------------------d-d-
//  TokenTypeList functions
//-----------------------------------------------------

TokenTypeListPtr TokenTypeList_create();

void TokenTypeList_destroy(TokenTypeListPtr *l);

int TokenTypeList_insert(TokenTypeListPtr l, TokenType type);

void TokenTypeList_first(TokenTypeListPtr l);

void TokenTypeList_next(TokenTypeListPtr l);

TokenTypeListItemPtr TokenTypeList_get(TokenTypeListPtr l);

TokenTypeListItemPtr TokenTypeList_getNext(TokenTypeListPtr l);

void TokenTypeList_deleteFirst(TokenTypeListPtr l);


//-------------------------------------------------d-d-
//  TokenTypeListItem functions
//-----------------------------------------------------

TokenTypeListItemPtr TokenTypeListItem_create(TokenType type);

void TokenTypeListItem_destroy(TokenTypeListItemPtr *i);

#endif
