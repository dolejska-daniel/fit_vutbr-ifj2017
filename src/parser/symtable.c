/**
 * Tento soubor obsahuje implementaci tabulky symbolů pomocí tabulky
 * s rozptýlenými položkami.
 *
 * @author Daniel Dolejška (xdolej08)
 * @date 21.11.2017
 * @project IFJcode17Parser
 * @subject Formální jazyky a překladače (IFJ) - FIT VUT v Brně
 */

#include <string.h>
#include <malloc.h>

#include "symtable.h"

#ifndef _symtable_c
#define _symtable_c

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

#define SYMBOL_TABLE_SIZE 100

//==================================================================d=d=
//  DEKLARACE A DEFINICE ENUMERÁTORÙ A STRUKTUR
//======================================================================


//==================================================================d=d=
//  DEKLARACE FUNKCÍ
//======================================================================

/**
 * Funkce vytvářející a inicializující tabulku symbolù.
 *
 * @retval	SymbolTablePtr	Ukazatel na novì vytvoøenou tabulku symbolù
 */
SymbolTablePtr SymbolTable_create()
{
	//	Vytvoření struktury
	SymbolTablePtr st = (SymbolTablePtr) malloc(sizeof(SymbolTable));
	//	Inicializace struktury
	st->size  = SYMBOL_TABLE_SIZE;
	st->array = (SymbolPtr *) malloc(sizeof(Symbol) * st->size);

	for (unsigned i = 0; i < st->size; i++)
        st->array[i] = NULL;

	return st;
}

/**
 * Funkce pro zrušení existující tabulky symbolů.
 *
 * @param[in,out]	SymbolTablePtr  *st	Ukazatel na existující tabulku symbolù
 *
 * @retval	void
 */
void SymbolTable_destroy(SymbolTablePtr *st)
{
    SymbolTablePtr table = *st;
    //  TODO: Uvolnění položek

    //  Uvolnění paměti pro pole symbolů
	free(table->array);
	//  Uvolnění pamìti pro tabulku
	free(table);

	*st = NULL;
}

/**
 * Funkce pro výpočet klíče na základě jména položky.
 *
 * @param[in,out]	SymbolTable	*st		Ukazatel na existující tabulku symbolù
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
 * @param[in,out]	SymbolTable	*st		Ukazatel na existující tabulku symbolù
 * @param[in]		char		*key	Identifikátor položky
 *
 * @retval	SymbolPtr|NULL	Ukazatel na vyhledanou položku v tabulce
 */
SymbolPtr SymbolTable_get(SymbolTable *st, char *key)
{
	//	Výpoèet hashe
	unsigned hash = SymbolTable_hash(st, key);
	//	Získání prvotního symbolu z tabulky
	SymbolPtr symbol = st->array[hash];

	//	Dokud symbol není NULL, nebo není nalezen symbol s požadovaným jménem,
	//	pokraèuje se ve vyhledávání na daném hashi
	while (symbol != NULL && strcmp(symbol->key, key) != 0)
		symbol = symbol->next;

	//	Pokud na daném hashi nic nebylo a nebo nebyl symbol nalezen vrací se NULL
	if (symbol == NULL || strcmp(symbol->key, key) != 0)
		return NULL;

	return symbol;
}

/**
 * Funkce vloží novou položku do dané tabulky s daným klíèem a hodnotou.
 *
 * @param[in,out]	SymbolTable	    *st		    Ukazatel na existující tabulku symbolů
 * @param[in]		char		    *key	    Identifikátor položky
 * @param[in]		SymbolType	    type	    Typ symbolu
 * @param[in]	    SymbolLocation	location	Umístění symbolu
 * @param[in]		void		    *value	    Hodnota položky
 *
 * @retval	SymbolPtr	Ukazatel na nově vytvořenou položku tabulky
 */
SymbolPtr SymbolTable_insert(SymbolTable *st, char *key, SymbolType type, SymbolLocation location, void *value)
{
	//	Vytvoření struktury
	SymbolPtr s = Symbol_create(key, type, location, value);

    unsigned hash = SymbolTable_hash(st, key);
	SymbolPtr first = st->array[hash];
	s->next = first;
    st->array[hash] = s;

	return s;
}

/**
 * Funkce odstraní položku s daným jménem z tabulky.
 *
 * @param[in,out]	SymbolTable	*st		Ukazatel na existující tabulku symbolù
 * @param[in]		char		*key	Identifikátor položky
 *
 * @retval	void
 */
void SymbolTable_delete(SymbolTable *st, char *key)
{
	//	Výpoèet hashe
	unsigned hash = SymbolTable_hash(st, key);
	//	Získání prvotního symbolu z tabulky
	SymbolPtr symbol = st->array[hash];
	SymbolPtr prev   = NULL;

	//	Dokud symbol není NULL, nebo není nalezen symbol s požadovaným jménem,
	//	pokraèuje se ve vyhledávání na daném hashi
	while (symbol != NULL && strcmp(symbol->key, key) != 0)
    {
        //  Uložení pøedcházející položky, kvùli možné nutnosti navazovat ukazatele
        prev   = symbol;
        symbol = symbol->next;
    }

    //  Byla nalezena položka ke smazání odpovídající zadání?
	if (symbol != NULL && strcmp(symbol->key, key) == 0)
    {
        //  Ano byla
        if (prev == NULL)
        {
            //  Pøed tímto prvkem není žádný pøedcházející
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
 * @param[in]	char		    *key	    Identifikátor položky
 * @param[in]	SymbolType	    type	    Typ symbolu
 * @param[in]	SymbolLocation	location	Umístění symbolu
 * @param[in]	void		    *value	    Hodnota položky
 *
 * @retval	SymbolPtr   Ukazatel na novì vytvoøenou položku
 */
SymbolPtr Symbol_create(char *key, SymbolType type, SymbolLocation location, void *value)
{
    //	Vytvoøení struktury
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
 * @param[in,out]	SymbolPtr	*s		Ukazatel na existující tabulku symbolù
 *
 * @retval	void
 */
void Symbol_destroy(SymbolPtr *s)
{
    if (*s == NULL)
        return;

    if ((*s)->value != NULL)
        free((*s)->value);

    free(*s);
    *s = NULL;
}


#endif
