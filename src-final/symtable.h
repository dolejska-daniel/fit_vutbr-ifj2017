/**
 * Tento soubor obsahuje deklarace funkcí využité při implementaci
 * tabulky symbolů pomocí tabulky s rozptýlenými položkami.
 *
 * @author Daniel Dolejška
 */

#ifndef _symtable_h
#define _symtable_h

//==================================================================d=d=
//  DEKLARACE A DEFINICE ENUMERÁTORŮ A STRUKTUR
//======================================================================

typedef enum E_SymbolType {
	DT_STRING,  ///< Proměnná datového typu string
	DT_DOUBLE,	///< Proměnná datového typu double
	DT_INTEGER,	///< Proměnná datového typu integer
	FUNCTION,	///< Funkce
	SUB_ST,		///< Podřízená tabulka symbolů
} SymbolType;

typedef struct S_Symbol
    Symbol,
   *SymbolPtr;
struct S_Symbol {
	SymbolType 	type;	///< Typ symbolu
	SymbolPtr	next;	///< Ukazatel na další prvek se stejným otiskem (synonymum)
	char 		*key;	///< Název/Identifikátor symbolu
	void		*value;	///< Ukazatel na další informace o symbolu
};

typedef struct S_SymbolTable
    SymbolTable,
   *SymbolTablePtr;
struct S_SymbolTable {
	unsigned	size;		///< Velikost tabulky symbolů
	Symbol	    **array;	///< Pole ukazatelů na jednotlivé symboly
};


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
 * Funkce získá poslední položku ze seznamu s daným identifikátorem.
 *
 * @param[in,out]	SymbolTable	*st		Ukazatel na existující tabulku symbolů
 * @param[in]		char		*key	Identifikátor položky
 *
 * @retval	SymbolPtr|NULL	Ukazatel na vyhledanou položku v tabulce
 */
SymbolPtr SymbolTable_getLast(SymbolTable *st, char *key);

/**
 * Funkce vloží novou položku do dané tabulky s daným klíčem a hodnotou.
 *
 * @param[in,out]	SymbolTable	*st		Ukazatel na existující tabulku symbolů
 * @param[in]		char		*key	Identifikátor položky
 * @param[in]		SymbolType	type	Typ symbolu
 * @param[in]		void		*value	Hodnota položky
 *
 * @retval	SymbolPtr	Ukazatel na nově vytvoření položku tabulky
 */
SymbolPtr SymbolTable_insert(SymbolTable *st, char *key, SymbolType type, void *value);

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
 * @param[in]	char		*key	Identifikátor položky
 * @param[in]	SymbolType	type	Typ symbolu
 * @param[in]	void		*value	Hodnota položky
 *
 * @retval	SymbolPtr   Ukazatel na nově vytvořenou položku
 */
SymbolPtr Symbol_create(char *key, SymbolType type, void *value);

/**
 * Funkce odstraní položku dle daného ukazatele.
 *
 * @param[in,out]	SymbolPtr	*s		Ukazatel na existující tabulku symbolů
 *
 * @retval	void
 */
void Symbol_destroy(SymbolPtr s);

#endif
