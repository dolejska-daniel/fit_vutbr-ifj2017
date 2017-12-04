/**
 * Tento soubor obsahuje implementaci "zanořovacího seznamu" pomocí
 * dvousměrně vázaného seznamu.
 *
 * @author Daniel Dolejška (xdolej08)
 * @date 21.11.2017
 * @project IFJcode17Parser
 * @subject Formální jazyky a překladače (IFJ) - FIT VUT v Brně
 */

#include <stdio.h>

#include "symtable.h"

#ifndef _nesting_list_h
#define _nesting_list_h

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
//  DEKLARACE A DEFINICE ENUMERÁTORÙ A STRUKTUR
//======================================================================

typedef enum E_NestingType {
    NESTING_NONE,       ///< Žádné zanoření - nejvyšší úroveň
    NESTING_SCOPE,      ///< Instrukce v hlavním programu
    NESTING_FUNCTION,   ///< Zanoření instrukcí ve funkci
    NESTING_LOOP,       ///< Zanoření instrukcí v cyklu
    NESTING_CONDITION,  ///< Zanoření instrukcí v IFu
} NestingType; ///< Typ zanoření instrukcí


typedef struct S_NestingLevel
    NestingLevel,
    *NestingLevelPtr;
struct S_NestingLevel {
    NestingType     type;   ///< Typ dané úrovně zanoření
    SymbolPtr       symbol; ///< Ukazatel na symbol aktuální úrovně (cyklus, funkce, apod.)
    NestingLevelPtr next;   ///< Ukazatel na následující úroveň
    NestingLevelPtr prev;   ///< Ukazatel na předchozí úroveň
}; ///< Struktura daného zanoření

typedef struct S_NestingList
    NestingList,
    *NestingListPtr;
struct S_NestingList {
    NestingLevelPtr first;  ///< Ukazatel na první úroveň v seznamu
    NestingLevelPtr active; ///< Ukazatel na aktivní úroveň v seznamu
    NestingLevelPtr last;   ///< Ukazatel na poslední úroveň v seznamu
}; ///< Struktura pro ukládání ůrovní zanoření ve vstupním kódu


//==================================================================d=d=
//  DEKLARACE FUNKCÍ
//======================================================================

//-------------------------------------------------d-d-
//  List operations
//-----------------------------------------------------

/**
 * Funkce vytvoří (alokuje) novou řídící strukturu pro NestingList.
 *
 * @retval  NestingListPtr  Ukazatel na nově vytvořenou strukturu
 */
NestingListPtr NestingList_create();

/**
 * Funkce zruší a uvolní existující řídící strukturu pro NestingList.
 * Pokud je neprázdná (obsahuje neuvolněné prvky) jsou tyto prvky zrušeny
 * a uvolněny.
 *
 * @param[in,out]   NestingListPtr  l   Ukazatel na existující řídící strukturu seznamu
 */
void NestingList_destroy(NestingListPtr *l);

/**
 * Vloží novou úroveň na začátek seznamu.
 *
 * @param[in,out]   NestingListPtr  l       Ukazatel na řídící strukturu seznamu
 * @param[in]       NestingType     type    Samotný obsah úrovně - typ zanoření
 * @param[in]       SymbolPtr       symbol  Ukazatel na symbol dané úrovně (cyklus, funkce, apod.)
 *
 * @retval  NestingLevelPtr Ukazatel na nově vytvořenou strukturu
 */
NestingLevelPtr NestingList_insertFirst(NestingListPtr l, NestingType type, SymbolPtr symbol);

/**
 * Vloží novou úroveň na konec seznamu.
 *
 * @param[in,out]   NestingListPtr  l       Ukazatel na řídící strukturu seznamu
 * @param[in]       NestingType     type    Samotný obsah úrovně - typ zanoření
 * @param[in]       SymbolPtr       symbol  Ukazatel na symbol dané úrovně (cyklus, funkce, apod.)
 *
 * @retval  NestingLevelPtr Ukazatel na nově vytvořenou strukturu
 */
NestingLevelPtr NestingList_insertLast(NestingListPtr l, NestingType type, SymbolPtr symbol);

/**
 * Nastaví první úroveň na aktivní.
 *
 * @param[in,out]   NestingListPtr  l   Ukazatel na řídící strukturu seznamu
 *
 * @retval  NestingLevelPtr Ukazatel na právě aktivní strukturu
 */
NestingLevelPtr NestingList_first(NestingListPtr l);

/**
 * Nastaví poslední úroveň na aktivní.
 *
 * @param[in,out]   NestingListPtr  l   Ukazatel na řídící strukturu seznamu
 *
 * @retval  NestingLevelPtr Ukazatel na právě aktivní strukturu
 */
NestingLevelPtr NestingList_last(NestingListPtr l);

/**
 * Vrátí ukazatel na právě aktivní úroveň seznamu.
 *
 * @param[in,out]   NestingListPtr  l   Ukazatel na řídící strukturu seznamu
 *
 * @retval  NestingLevelPtr Ukazatel na právě aktivní strukturu
 */
NestingLevelPtr NestingList_active(NestingListPtr l);

void NestingList_debugPrint(NestingListPtr l);

/**
 * Zruší a uvolní úroveň v seznamu na základě ukazatele na danou
 * úroveň.
 *
 * @param[in,out]   NestingListPtr  l       Ukazatel na řídící strukturu seznamu
 * @param[in,out]   NestingLevelPtr level   Ukazatel na strukturu úrovně v seznamu
 */
void NestingList_delete(NestingListPtr l, NestingLevelPtr level);

/**
 * Zruší a uvolní první úroveň v seznamu.
 *
 * @param[in,out]   NestingListPtr  l   Ukazatel na řídící strukturu seznamu
 */
void NestingList_deleteFirst(NestingListPtr l);

/**
 * Zruší a uvolní poslední úroveň v seznamu.
 *
 * @param[in,out]   NestingListPtr  l   Ukazatel na řídící strukturu seznamu
 */
void NestingList_deleteLast(NestingListPtr l);


//-------------------------------------------------d-d-
//  Level operations
//-----------------------------------------------------

/**
 * Funkce vytvoří (alokuje) novou úroveň seznamu.
 *
 * @param[in]   NestingType type    Typ zanoření úrovně
 * @param[in]   SymbolPtr   symbol  Ukazatel na symbol dané úrovně (cyklus, funkce, apod.)
 *
 * @retval  NestingLevelPtr Ukazatel na nově vytvořenou strukturu
 */
NestingLevelPtr NestingLevel_create(NestingType type, SymbolPtr symbol);

/**
 * Funkce zruší (uvolní) existující úroveň seznamu.
 *
 * @param[in,out]   NestingLevelPtr *l  Ukazatel na existující strukturu úrovně seznamu
 */
void NestingLevel_destroy();


//-------------------------------------------------d-d-
//  Special operations / Aliases
//-----------------------------------------------------

/**
 * Prohledá předcházející úrovně seznamu a zjistí, zda se zde nachází
 * alespoň jedna úroveň s daným typem.
 *
 * @param[in,out]   NestingListPtr  l       Ukazatel na řídící strukturu seznamu
 * @param[in]       NestingType     type    Samotný obsah úrovně - typ zanoření
 *
 * @param[in,out]   NestingListPtr  l   Ukazatel na řídící strukturu seznamu
 */
NestingLevelPtr NestingList_isNestedIn(NestingListPtr l, NestingType type);

/**
 * Vytvoří novou úroveň v seznamu (automaticky jako poslední) a nastaví
 * ji jako aktivní.
 *
 * @param[in,out]   NestingListPtr  l       Ukazatel na řídící strukturu seznamu
 * @param[in]       NestingType     type    Samotný obsah úrovně - typ zanoření
 * @param[in]       SymbolPtr       symbol  Ukazatel na symbol aktuální úrovně (cyklus, funkce, apod.)
 *
 * @param[in,out]   NestingListPtr  l   Ukazatel na řídící strukturu seznamu
 */
NestingLevelPtr NestingList_newLevel(NestingListPtr l, NestingType type, SymbolPtr symbol);

/**
 * Odstraní poslední úroveň v seznamu a nastaví aktivitu na předcházející
 * úroveň.
 *
 * @param[in,out]   NestingListPtr  l       Ukazatel na řídící strukturu seznamu
 * @param[in]       NestingType     type    Samotný obsah úrovně - typ zanoření
 *
 * @param[in,out]   NestingListPtr  l   Ukazatel na řídící strukturu seznamu
 */
NestingLevelPtr NestingList_leaveCurrentLevel(NestingListPtr l);

char *NestingType_toString(NestingType type);

#endif
