/**
 * Tento soubor obsahuje deklarace funkcí využité při implementaci
 * tabulky symbolů pomocí tabulky s rozptýlenými položkami.
 *
 * @author Daniel Dolejška (xdolej08)
 * @project IFJcode17Parser
 */

#include <stdbool.h>

#ifndef _symtable_h
#define _symtable_h

#ifdef DEBUG_INCLUDE
#include "../scanner/token.h"
#else
#include "token.h"
#endif

//==================================================================d=d=
//  DEKLARACE A DEFINICE ENUMERÁTORŮ A STRUKTUR
//======================================================================

typedef enum E_SymbolType {
	ST_BOOLEAN,  ///< Proměnná datového typu boolean
	ST_STRING,   ///< Proměnná datového typu string
	ST_DOUBLE,	 ///< Proměnná datového typu double
	ST_INTEGER,  ///< Proměnná datového typu integer
	ST_NONE,     ///< Datový typ void (pro návratové typy funkcí)
	ST_FUNCTION, ///< Funkce
	ST_LOOP,     ///< Cyklus
	ST_COND,     ///< Podmínka
} SymbolType; ///< Typ symbolu

typedef enum E_SymbolLocation {
    GLOBAL_FRAME    = 1, ///< Globální rámec
    LOCAL_FRAME     = 3, ///< Lokální rámec
    TEMPORARY_FRAME = 2, ///< Dočasný rámec
    CONSTANT        = 0, ///< Konstanta
} SymbolLocation; ///< Umístění symbolu

typedef struct S_Symbol
    Symbol,
   *SymbolPtr;
struct S_Symbol {
	SymbolType 	    type;	  ///< Typ symbolu
    SymbolLocation  location; ///< Umístění symbolu
	SymbolPtr	    next;	  ///< Ukazatel na další prvek se stejným otiskem (synonymum)
	char 		    *key;	  ///< Název/Identifikátor symbolu
	void		    *value;	  ///< Ukazatel na další informace o symbolu
}; ///< Struktura symbolu

typedef struct S_SymbolInfo_Function_Parameter
    SymbolInfo_Function_Parameter,
   *SymbolInfo_Function_ParameterPtr;
struct S_SymbolInfo_Function_Parameter {
    SymbolInfo_Function_ParameterPtr next; ///< Ukazatel na následující položku v seznamu
    SymbolType dataType; ///< Datový typ parametru funkce
    char       *name;    ///< Název parametru (proměnné)
}; ///< Struktura pro uložení vlastností parametru funkce

typedef struct S_SymbolInfo_Function_ParameterList
    SymbolInfo_Function_ParameterList,
   *SymbolInfo_Function_ParameterListPtr;
struct S_SymbolInfo_Function_ParameterList {
    SymbolInfo_Function_ParameterPtr first;  ///< Uzakatel na první prvek v seznamu
    SymbolInfo_Function_ParameterPtr active; ///< Ukazatel na aktivní prvek v seznamu
}; ///< Seznam parametrů funkce

typedef struct S_SymbolInfo_Function
    SymbolInfo_Function,
   *SymbolInfo_FunctionPtr;
struct S_SymbolInfo_Function {
    SymbolInfo_Function_ParameterListPtr params; ///< Seznam parametrů funkce
    SymbolType  dataType;   ///< Datový typ návratové hodnoty
    bool        isDefined;  ///< Označuje zda je funkce definována
}; ///< Struktura pro uložení vlastností funkce

typedef struct S_SymbolInfo_Loop
    SymbolInfo_Loop,
   *SymbolInfo_LoopPtr;
struct S_SymbolInfo_Loop {
    char *begin_label; ///< Název návěstí na které se skočí vždy po dokončení iterace
    char *end_label;   ///< Název návěstí na které se skočí po ukončení cyklu
}; ///< Struktura pro uložení vlastností cyklu

typedef struct S_SymbolTable
    SymbolTable,
   *SymbolTablePtr;
struct S_SymbolTable {
	Symbol	    **array;   ///< Pole ukazatelů na jednotlivé symboly
	unsigned	size;	   ///< Velikost tabulky symbolů
	unsigned    condCount; ///< Index pro nově vytvářené podmínky
	unsigned    loopCount; ///< Index pro nově vytvářené cykly
}; ///< Struktura tabulky symbolů


//==================================================================d=d=
//  DEKLARACE FUNKCÍ
//======================================================================

//-------------------------------------------------d-d-
//  SymbolTable
//-----------------------------------------------------

/**
 * Funkce vytvářející a inicializující tabulku symbolů.
 *
 * @retval	SymbolTablePtr	Ukazatel na nově vytvořenou tabulku symbolů
 */
SymbolTablePtr SymbolTable_create();

/**
 * Funkce pro zrušení existující tabulky symbolů.
 *
 * @param[in,out]	SymbolTablePtr	st	Ukazatel na existující tabulku symbolů
 */
void SymbolTable_destroy(SymbolTablePtr *st);

/**
 * Funkce pro výpočet klíče na základě jména položky.
 *
 * @param[in,out]	SymbolTablePtr  st		Ukazatel na existující tabulku symbolů
 * @param[in]		char		    *key	Identifikátor položky
 *
 * @retval	unsigned	Vypočtený otisk (index) položky s daným identifikátorem
 */
unsigned SymbolTable_hash(SymbolTablePtr st, char *key);

/**
 * Funkce získá hledanou položku ze seznamu s daným identifikátorem.
 *
 * @param[in,out]	SymbolTablePtr	st		Ukazatel na existující tabulku symbolů
 * @param[in]		char		    *key	Identifikátor položky
 *
 * @retval	SymbolPtr|NULL	Ukazatel na vyhledanou položku v tabulce
 */
SymbolPtr SymbolTable_get(SymbolTablePtr st, char *key);

/**
 * Funkce získá hledanou položku ze seznamu na základě informací
 * z daného tokenu. Pokud se jedná o konstantu a ta v tabulce ještě
 * nebyla nefinována, je automaticky definována a navrácena.
 *
 * @param[in,out]	SymbolTablePtr  st		Ukazatel na existující tabulku symbolů
 * @param[in]		TokenPtr	    token	Ukazatel na existující token
 *
 * @retval	SymbolPtr|NULL	Ukazatel na vyhledanou položku v tabulce
 */
SymbolPtr SymbolTable_getByToken(SymbolTablePtr st, TokenPtr token);

/**
 * Funkce vloží novou položku do dané tabulky s daným klíčem a hodnotou.
 *
 * @param[in,out]	SymbolTablePtr  st		    Ukazatel na existující tabulku symbolů
 * @param[in]		char		    *key	    Identifikátor položky
 * @param[in]		SymbolType	    type	    Typ symbolu
 * @param[in]	    SymbolLocation	location	Umístění symbolu
 * @param[in]		void		    *value	    Hodnota položky
 * @param[out]		SymbolPtr	    *symbol     Ukazatel na nově vytvořený symbol
 *
 * @retval	int Kód se kterým bylo vložení symbolu ukončeno
 */
int SymbolTable_insert(SymbolTablePtr st, char *key, SymbolType type, SymbolLocation location, void *value, SymbolPtr *symbol);

/**
 * Funkce odstraní položku s daným jménem z tabulky.
 *
 * @param[in,out]	SymbolTablePtr	st		Ukazatel na existující tabulku symbolů
 * @param[in]		char		    *key	Identifikátor položky
 */
void SymbolTable_delete(SymbolTablePtr st, char *key);

/**
 * Funkce zobrazí obsah tabulky na stderr.
 *
 * @param[in,out]	SymbolTablePtr	st Ukazatel na existující tabulku symbolů
 */
void SymbolTable_debugPrint(SymbolTablePtr st);

//-------------------------------------------------d-d-
//  Symbol
//-----------------------------------------------------

/**
 * Funkce vytvoří (naalokuje) novou položku pro seznam.
 *
 * @param[in]	char		    *key	    Identifikátor položky
 * @param[in]	SymbolType	    type	    Typ symbolu
 * @param[in]	SymbolLocation	location	Umístění symbolu
 * @param[in]	void		    *value	    Hodnota položky
 *
 * @retval	SymbolPtr   Ukazatel na nově vytvořenou položku
 */
SymbolPtr Symbol_create(char *key, SymbolType type, SymbolLocation location, void *value);

/**
 * Funkce odstraní položku dle daného ukazatele.
 *
 * @param[in,out]	SymbolPtr	*s		Ukazatel na existující tabulku symbolů
 */
void Symbol_destroy(SymbolPtr *s);

bool Symbol_isVariable(SymbolPtr s);

bool Symbol_isFunction(SymbolPtr s);

void Symbol_debugPrint(SymbolPtr symbol);

//-------------------------------------------------d-d-
//  SymbolInfo_Function
//-----------------------------------------------------

SymbolInfo_FunctionPtr SymbolInfo_Function_create(SymbolType dataType, SymbolInfo_Function_ParameterListPtr paramList);

void SymbolInfo_Function_destroy(SymbolInfo_FunctionPtr *s);

void SymbolInfo_Function_debugPrint(SymbolInfo_FunctionPtr s);

//-------------------------------------------------d-d-
//  SymbolInfo_Function_Parameter
//-----------------------------------------------------

SymbolInfo_Function_ParameterPtr SymbolInfo_Function_Parameter_create(char *name, SymbolType dataType);

void SymbolInfo_Function_Parameter_destroy(SymbolInfo_Function_ParameterPtr *s);

void SymbolInfo_Function_Parameter_debugPrint(SymbolInfo_Function_ParameterPtr s);

//-------------------------------------------------d-d-
//  SymbolInfo_Function_ParameterList
//-----------------------------------------------------

SymbolInfo_Function_ParameterListPtr SymbolInfo_Function_ParameterList_create();

void SymbolInfo_Function_ParameterList_destroy(SymbolInfo_Function_ParameterListPtr *l);

int SymbolInfo_Function_ParameterList_insert(SymbolInfo_Function_ParameterListPtr l, SymbolInfo_Function_ParameterPtr param);

void SymbolInfo_Function_ParameterList_first(SymbolInfo_Function_ParameterListPtr l);

void SymbolInfo_Function_ParameterList_next(SymbolInfo_Function_ParameterListPtr l);

SymbolInfo_Function_ParameterPtr SymbolInfo_Function_ParameterList_get(SymbolInfo_Function_ParameterListPtr l);

SymbolInfo_Function_ParameterPtr SymbolInfo_Function_ParameterList_getNext(SymbolInfo_Function_ParameterListPtr l);

bool SymbolInfo_Function_ParameterList_parameterExistsWithName(SymbolInfo_Function_ParameterListPtr l, char *name);

void SymbolInfo_Function_ParameterList_deleteFirst(SymbolInfo_Function_ParameterListPtr l);

void SymbolInfo_Function_ParameterList_debugPrint(SymbolInfo_Function_ParameterListPtr l);

//-------------------------------------------------d-d-
//  SymbolInfo_Loop
//-----------------------------------------------------

SymbolInfo_LoopPtr SymbolInfo_Loop_create(char *begin_label, char *end_label);

void SymbolInfo_Loop_destroy(SymbolInfo_LoopPtr *s);

//-------------------------------------------------d-d-
//  SymbolType
//-----------------------------------------------------

char *SymbolType_toString(SymbolType type);

char *SymbolLocation_toString(SymbolLocation location);

#endif
