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

#ifdef DEBUG
#define DEBUG_PRINT(...) do{ fprintf( stderr, __VA_ARGS__ ); } while( 0 )
#else
#define DEBUG_PRINT(...) do{ } while ( 0 )
#endif

//==================================================================d=d=
//  DEKLARACE A DEFINICE ENUMERÁTORŮ A STRUKTUR
//======================================================================

typedef enum E_SymbolType {
	DT_BOOLEAN, ///< Proměnná datového typu boolean
	DT_STRING,  ///< Proměnná datového typu string
	DT_DOUBLE,	///< Proměnná datového typu double
	DT_INTEGER, ///< Proměnná datového typu integer
	FUNCTION,   ///< Funkce
	SUB_ST,		///< Podřízená tabulka symbolů
} SymbolType; ///< Typ symbolu

typedef enum E_SymbolLocation {
    GLOBAL,     ///< Globální rámec
    LOCAL,      ///< Lokální rámec
    TEMPORARY,  ///< Dočasný rámec
    CONSTANT,   ///< Konstanta
} SymbolLocation; ///< Umístění symbolu

typedef struct S_Symbol
    Symbol,
   *SymbolPtr;
struct S_Symbol {
	SymbolType 	    type;	    ///< Typ symbolu
    SymbolLocation  location;   ///< Umístění symbolu
	SymbolPtr	    next;	    ///< Ukazatel na další prvek se stejným otiskem (synonymum)
	char 		    *key;	    ///< Název/Identifikátor symbolu
	void		    *value;	    ///< Ukazatel na další informace o symbolu
}; ///< Struktura symbolu

typedef struct S_SymbolInfo_Function_Parameter
    SymbolInfo_Function_Parameter,
   *SymbolInfo_Function_ParameterPtr;
struct S_SymbolInfo_Function_Parameter {
    SymbolType  dataType;   ///< Datový typ parametru funkce
    char        *name;      ///< Název parametru (proměnné)
};

typedef struct S_SymbolInfo_Function
    SymbolInfo_Function,
   *SymbolInfo_FunctionPtr;
struct S_SymbolInfo_Function {
    SymbolInfo_Function_Parameter   **params; ///< Pole parametrů funkce
    bool isDefined; ///< Označuje zda je funkce definována
};

typedef struct S_SymbolTable
    SymbolTable,
   *SymbolTablePtr;
struct S_SymbolTable {
	Symbol	    **array;	///< Pole ukazatelů na jednotlivé symboly
	unsigned	size;		///< Velikost tabulky symbolů
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
 * @param[in,out]	SymbolTable	*st	Ukazatel na existující tabulku symbolů
 *
 * @retval	void
 */
void SymbolTable_destroy(SymbolTable *st);

/**
 * Funkce pro výpočet klíče na základě jména položky.
 *
 * @param[in,out]	SymbolTable	*st		Ukazatel na existující tabulku symbolů
 * @param[in]		char		*key	Identifikátor položky
 *
 * @retval	unsigned	Vypočtený otisk (index) položky s daným identifikátorem
 */
unsigned SymbolTable_hash(SymbolTable *st, char *key);

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
 *
 * @retval	SymbolPtr	Ukazatel na nově vytvoření položku tabulky
 */
SymbolPtr SymbolTable_insert(SymbolTable *st, char *key, SymbolType type, SymbolLocation location, void *value);

/**
 * Funkce odstraní položku s daným jménem z tabulky.
 *
 * @param[in,out]	SymbolTable	*st		Ukazatel na existující tabulku symbolů
 * @param[in]		char		*key	Identifikátor položky
 *
 * @retval	void
 */
void SymbolTable_delete(SymbolTable *st, char *key);

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
 *
 * @retval	void
 */
void Symbol_destroy(SymbolPtr s);

#endif
