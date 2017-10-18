/**
 * Tento soubor obsahuje implementaci tabulky symbolů pomocí tabulky
 * s rozptýlenými položkami.
 *
 * @author Daniel Dolejška
 */

#include "symtable.h"

#include <string.h>
#include <malloc.h>

#ifndef _symtable_c
#define _symtable_c
#define SYMBOL_TABLE_SIZE 100

#ifdef DEBUG
#define DEBUG_PRINT(...) do{ fprintf( stderr, __VA_ARGS__ ); } while( 0 )
#else
#define DEBUG_PRINT(...) do{ } while ( 0 )
#endif

//==================================================================d=d=
//  DEKLARACE A DEFINICE ENUMERÁTORŮ A STRUKTUR
//======================================================================


//==================================================================d=d=
//  DEKLARACE FUNKCÍ
//======================================================================

/**
 * Funkce vytvářející a inicializující tabulku symbolů.
 *
 * @retval	SymbolTablePtr	Ukazatel na nově vytvořenou tabulku symbolů
 */
SymbolTablePtr SymbolTable_create()
{
	//	Vytvoření struktury
	SymbolTablePtr st = (SymbolTablePtr) malloc(sizeof(SymbolTable));
	//	Inicializace struktury
	st->size  = SYMBOL_TABLE_SIZE;
	st->array = (SymbolPtr *) malloc(sizeof(Symbol) * st->size);

	return st;
}

/**
 * Funkce pro zrušení existující tabulky symbolů.
 *
 * @param[in,out]	SymbolTable	*st	Ukazatel na existující tabulku symbolů
 *
 * @retval	void
 */
void SymbolTable_destroy(SymbolTable *st)
{
	free(st->array);
	free(st);
}

/**
 * Funkce pro výpočet klíče na základě jména položky.
 *
 * @param[in,out]	SymbolTable	*st		Ukazatel na existující tabulku symbolů
 * @param[in]		char		*key	Identifikátor položky
 *
 * @retval	unsigned	Vypočtený otisk (index) položky s daným identifikátorem
 */
unsigned SymbolTable_hash(SymbolTable *st, char *key)
{
	unsigned hash = 0;
	unsigned i	  = 0;
	while (key[i] != '\0')
		hash += key[i++];

	return hash % st->size;
}

/**
 * Funkce získá hledanou položku ze seznamu s daným identifikátorem.
 *
 * @param[in,out]	SymbolTable	*st		Ukazatel na existující tabulku symbolů
 * @param[in]		char		*key	Identifikátor položky
 *
 * @retval	SymbolPtr|NULL	Ukazatel na vyhledanou položku v tabulce
 */
SymbolPtr SymbolTable_get(SymbolTable *st, char *key)
{
	//	Výpočet hashe
	unsigned hash = SymbolTable_hash(st, key);
	//	Získání prvotního symbolu z tabulky
	SymbolPtr symbol = st->array[hash];

	//	Dokud symbol není NULL, nebo není nalezen symbol s požadovaným jménem,
	//	pokračuje se ve vyhledávání na daném hashi
	while (symbol != NULL && strcmp(symbol->key, key) != 0)
		symbol = symbol->next;

	//	Pokud na daném hashi nic nebylo a nebo nebyl symbol nalezen vrací se NULL
	if (symbol == NULL || strcmp(symbol->key, key) != 0)
		return NULL;

	return symbol;
}

/**
 * Funkce získá poslední položku ze seznamu s daným identifikátorem.
 *
 * @param[in,out]	SymbolTable	*st		Ukazatel na existující tabulku symbolů
 * @param[in]		char		*key	Identifikátor položky
 *
 * @retval	SymbolPtr|NULL	Ukazatel na vyhledanou položku v tabulce
 */
SymbolPtr SymbolTable_getLast(SymbolTable *st, char *key)
{
	//	Výpočet hashe
	unsigned hash = SymbolTable_hash(st, key);
	//	Získání prvotního symbolu z tabulky
	SymbolPtr symbol = st->array[hash];

	//	V tabulce se vyhledává poslední položka s daným hashem
	while (1)
	{
		if (symbol->next == NULL)
			break;

		symbol = symbol->next;
	}

	return symbol;
}

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
SymbolPtr SymbolTable_insert(SymbolTable *st, char *key, SymbolType type, void *value)
{
	//	Vytvoření struktury
	SymbolPtr s = Symbol_create(key, type, value);

	//	Získání poslední položky v tabulce s daným identifikátorem
	SymbolPtr last = SymbolTable_getLast(st, key);
	if (last)
	{
		//	V tabulce na daném identifikátoru byl nalezen symbol
		//	přidáme nový symbol za něj
		last->next = s;
	}
	else
	{
		//	V tabulce na daném identifikátoru nebyl nalezen žádný symbol
		//	přidáme nový symbol na daný identifikátor do tabulky
		unsigned hash   = SymbolTable_hash(st, key);
		st->array[hash] = s;
	}

	return s;
}

/**
 * Funkce odstraní položku s daným jménem z tabulky.
 *
 * @param[in,out]	SymbolTable	*st		Ukazatel na existující tabulku symbolů
 * @param[in]		char		*key	Identifikátor položky
 *
 * @retval	void
 */
void SymbolTable_delete(SymbolTable *st, char *key)
{
	//	Výpočet hashe
	unsigned hash = SymbolTable_hash(st, key);
	//	Získání prvotního symbolu z tabulky
	SymbolPtr symbol = st->array[hash];
	SymbolPtr prev   = NULL;

	//	Dokud symbol není NULL, nebo není nalezen symbol s požadovaným jménem,
	//	pokračuje se ve vyhledávání na daném hashi
	while (symbol != NULL && strcmp(symbol->key, key) != 0)
    {
        //  Uložení předcházející položky, kvůli možné nutnosti navazovat ukazatele
        prev   = symbol;
        symbol = symbol->next;
    }

    //  Byla nalezena položka ke smazání odpovídající zadání?
	if (symbol != NULL && strcmp(symbol->key, key) == 0)
    {
        if (prev == NULL)
        {
            //  Před tímto prvkem není žádný předcházející
            st->array[hash] = symbol->next;
        }
        else
        {
            //  Existuje předcházející prvek, musíme nahradit ukazatele
            prev->next = symbol->next;
        }

        Symbol_destroy(symbol);
    }
}

/**
 * Funkce vytvoří (naalokuje) novou položku pro seznam.
 *
 * @param[in]	char		*key	Identifikátor položky
 * @param[in]	SymbolType	type	Typ symbolu
 * @param[in]	void		*value	Hodnota položky
 *
 * @retval	SymbolPtr   Ukazatel na nově vytvořenou položku
 */
SymbolPtr Symbol_create(char *key, SymbolType type, void *value)
{
    //	Vytvoření struktury
	SymbolPtr s = (SymbolPtr) malloc(sizeof(Symbol));
	//	Inicializace struktury
	s->next  = NULL;
	s->key   = key;
	s->type  = type;
	s->value = value;

	return s;
}

/**
 * Funkce odstraní položku dle daného ukazatele.
 *
 * @param[in,out]	SymbolPtr	*s		Ukazatel na existující tabulku symbolů
 *
 * @retval	void
 */
void Symbol_destroy(SymbolPtr s)
{
    if (s == NULL)
        return;

    free(s->value);
    free(s);
}


#endif
