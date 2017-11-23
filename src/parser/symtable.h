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
#else
#endif

//==================================================================d=d=
//  DEKLARACE A DEFINICE ENUMERÁTORŮ A STRUKTUR
//======================================================================

typedef enum E_SymbolType {
	ST_BOOLEAN,  ///< Proměnná datového typu boolean
	ST_STRING,   ///< Proměnná datového typu string
	ST_DOUBLE,	 ///< Proměnná datového typu double
	ST_INTEGER,  ///< Proměnná datového typu integer
	ST_NONE,     ///< NULL
	ST_FUNCTION, ///< Funkce
} SymbolType; ///< Typ symbolu

typedef enum E_SymbolLocation {
    GLOBAL_FRAME,    ///< Globální rámec
    LOCAL_FRAME,     ///< Lokální rámec
    TEMPORARY_FRAME, ///< Dočasný rámec
    CONSTANT,        ///< Konstanta
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
    SymbolType  dataType; ///< Datový typ parametru funkce
    char        *name;    ///< Název parametru (proměnné)
}; ///< Struktura pro uložení vlastností parametru funkce

typedef struct S_SymbolInfo_Function
    SymbolInfo_Function,
   *SymbolInfo_FunctionPtr;
struct S_SymbolInfo_Function {
    SymbolInfo_Function_Parameter   **params; ///< Pole parametrů funkce
    SymbolType  returnDataType; ///< Datový typ návratové hodnoty
    bool        isDefined;      ///< Označuje zda je funkce definována
}; ///< Struktura pro uložení vlastností funkce

typedef struct S_SymbolTable
    SymbolTable,
   *SymbolTablePtr;
struct S_SymbolTable {
	Symbol	    **array; ///< Pole ukazatelů na jednotlivé symboly
	unsigned	size;	 ///< Velikost tabulky symbolů
}; ///< Struktura tabulky symbolů


//==================================================================d=d=
//  DEKLARACE FUNKCÍ
//======================================================================

/**
 * Funkce vytvářející a inicializující tabulku symbolů.
 *
 * @retval	SymbolTablePtr	Ukazatel na nově vytvořenou tabulku symbolů
 */
SymbolTablePtr SymbolTable_create();

/**
 * Funkce pro zrušení existující tabulky symbolů.
 *
 * @param[in,out]	SymbolTablePtr	*st	Ukazatel na existující tabulku symbolů
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
 * @param[in,out]	SymbolTable	*st		Ukazatel na existující tabulku symbolů
 * @param[in]		char		*key	Identifikátor položky
 *
 * @retval	SymbolPtr|NULL	Ukazatel na vyhledanou položku v tabulce
 */
SymbolPtr SymbolTable_get(SymbolTable *st, char *key);

/**
 * Funkce vloží novou položku do dané tabulky s daným klíčem a hodnotou.
 *
 * @param[in,out]	SymbolTable	    *st		    Ukazatel na existující tabulku symbolů
 * @param[in]		char		    *key	    Identifikátor položky
 * @param[in]		SymbolType	    type	    Typ symbolu
 * @param[in]	    SymbolLocation	location	Umístění symbolu
 * @param[in]		void		    *value	    Hodnota položky
 * @param[out]		SymbolPtr	    *symbol     Ukazatel na nově vytvořený symbol
 *
 * @retval	int Kód se kterým bylo vložení symbolu ukončeno
 */
int SymbolTable_insert(SymbolTable *st, char *key, SymbolType type, SymbolLocation location, void *value, SymbolPtr *symbol);

/**
 * Funkce odstraní položku s daným jménem z tabulky.
 *
 * @param[in,out]	SymbolTable	*st		Ukazatel na existující tabulku symbolů
 * @param[in]		char		*key	Identifikátor položky
 */
void SymbolTable_delete(SymbolTable *st, char *key);

/**
 * Funkce zobrazí obsah tabulky na stderr.
 *
 * @param[in,out]	SymbolTable	*st Ukazatel na existující tabulku symbolů
 */
void SymbolTable_debugPrint(SymbolTable *st);

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

SymbolInfo_FunctionPtr SymbolInfo_Function_create();

void SymbolInfo_Function_destroy(SymbolInfo_FunctionPtr *s);

SymbolInfo_Function_ParameterPtr SymbolInfo_Function_Parameter_create();

void SymbolInfo_Function_Parameter_destroy(SymbolInfo_Function_ParameterPtr *s);

char *SymbolType_toString(SymbolType type);

#endif
