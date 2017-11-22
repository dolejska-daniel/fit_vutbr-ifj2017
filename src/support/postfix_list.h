/**
 * Tento soubor obsahuje deklarace funkcí využité při implementaci
 * postfixového seznamu pro Symboly a Tokeny.
 *
 * @author Daniel Dolejška (xdolej08)
 * @date 21.11.2017
 * @project IFJcode17Parser
 * @subject Formální jazyky a překladače (IFJ) - FIT VUT v Brně
 */

#include <stdbool.h>
#include <malloc.h>

#ifndef _postfix_list_h
#define _postfix_list_h

#ifdef DEBUG_INCLUDE
#include "../parser/symtable.h"
#include "../scanner/token.h"
#else
#include "symtable.h"
#include "token.h"
#endif

//==================================================================d=d=
//  DEKLARACE A DEFINICE ENUMERÁTORŮ A STRUKTUR
//======================================================================

typedef struct S_PostfixListItem
    PostfixListItem,
    *PostfixListItemPtr;
struct S_PostfixListItem {
    bool                isOperator; ///< Proměnná určující typ prvku
    TokenPtr            token;      ///< Ukazatel na token
    SymbolPtr           symbol;     ///< Ukazatel na symbol
    PostfixListItemPtr  next;       ///< Ukazatel na následující prvek v seznamu
};

typedef struct S_PostfixList
    PostfixList,
    *PostfixListPtr;
struct S_PostfixList {
    PostfixListItemPtr first;   ///< Ukazatel na první prvek seznamu
    PostfixListItemPtr active;  ///< Ukazatel na aktivní prvek seznamu
};


//==================================================================d=d=
//  DEKLARACE FUNKCÍ
//======================================================================

//-------------------------------------------------d-d-
//  PostfixList functions
//-----------------------------------------------------

PostfixListPtr PostfixList_create();

void PostfixList_destroy(PostfixListPtr *l);

int PostfixList_insertOperator(PostfixListPtr l, TokenPtr token);

int PostfixList_insertSymbol(PostfixListPtr l, SymbolPtr symbol);

void PostfixList_first(PostfixListPtr l);

void PostfixList_next(PostfixListPtr l);

PostfixListItemPtr PostfixList_get(PostfixListPtr l);

PostfixListItemPtr PostfixList_getNext(PostfixListPtr l);

void PostfixList_delete(PostfixListPtr l);

void PostfixList_deleteFirst(PostfixListPtr l);


//-------------------------------------------------d-d-
//  PostfixListItem functions
//-----------------------------------------------------

PostfixListItemPtr PostfixListItem_createOperator(TokenPtr token);

PostfixListItemPtr PostfixListItem_createSymbol(SymbolPtr symbol);

void PostfixListItem_destroy(PostfixListItemPtr *i);

#endif
