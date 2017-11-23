/**
 * Tento soubor obsahuje implementaci tabulky symbolů pomocí tabulky
 * s rozptýlenými položkami.
 *
 * @author Daniel Dolejška (xdolej08)
 * @date 21.11.2017
 * @project IFJcode17Parser
 * @subject Formální jazyky a překladače (IFJ) - FIT VUT v Brně
 */

#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "symtable.h"

#ifndef _symtable_c
#define _symtable_c

#ifdef DEBUG_INCLUDE
#include "../support/error_codes.h"
#else
#include "error_codes.h"
#endif

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

	//  Inicializace položek
	for (unsigned i = 0; i < st->size; i++)
        st->array[i] = NULL;

	return st;
}

/**
 * Funkce pro zrušení existující tabulky symbolů.
 *
 * @param[in,out]	SymbolTablePtr  *st	Ukazatel na existující tabulku symbolů
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
 * @param[in,out]	SymbolTablePtr  st		Ukazatel na existující tabulku symbolů
 * @param[in]		char		    *key    Identifikátor položky
 *
 * @retval	unsigned	Vypočtený otisk (index) položky s daným identifikátorem
 */
unsigned SymbolTable_hash(SymbolTablePtr st, char *key)
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
 * @param[in,out]	SymbolTablePtr	st		Ukazatel na existující tabulku symbolů
 * @param[in]		char		    *key	Identifikátor položky
 *
 * @retval	SymbolPtr|NULL	Ukazatel na vyhledanou položku v tabulce
 */
SymbolPtr SymbolTable_get(SymbolTablePtr st, char *key)
{
	//	Výpočet hashe
	unsigned hash = SymbolTable_hash(st, key);
	//	Získání prvního symbolu z tabulky
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
int SymbolTable_insert(SymbolTablePtr st, char *key, SymbolType type, SymbolLocation location, void *value, SymbolPtr *symbol)
{
	//  Vyhledání sybolu na základě klíče
	SymbolPtr s = SymbolTable_get(st, key);
	if (s != NULL)
    {
        //  Symbol s daným klíčem byl v tabulce již nalezen
        DEBUG_ERR("symtable-insert", "Symbol already exists");
        return SEMANTICAL_DEFINITION_ERROR;
    }
    else
    {
        //	Vytvoření struktury
        SymbolPtr new_symbol = Symbol_create(key, type, location, value);
        if (new_symbol == NULL)
        {
            DEBUG_ERR("symtable-insert", "failed to mallocate Symbol");
            return INTERNAL_ERROR;
        }

        //  Uložení nové struktury do tabulky
        unsigned hash = SymbolTable_hash(st, key);
        SymbolPtr first = st->array[hash];
        new_symbol->next = first;
        st->array[hash] = new_symbol;

        *symbol = new_symbol;
        return NO_ERROR;
    }
}

/**
 * Funkce odstraní položku s daným jménem z tabulky.
 *
 * @param[in,out]	SymbolTablePtr  st		Ukazatel na existující tabulku symbolů
 * @param[in]		char		    *key	Identifikátor položky
 */
void SymbolTable_delete(SymbolTablePtr st, char *key)
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

        Symbol_destroy(&symbol);
    }
}

/**
 * Funkce zobrazí obsah tabulky na stderr.
 *
 * @param[in,out]	SymbolTablePtr  st Ukazatel na existující tabulku symbolů
 */
void SymbolTable_debugPrint(SymbolTablePtr st)
{
    #ifdef DEBUG_PRINT_SYMTABLE
    for (int i = 0; i < SYMBOL_TABLE_SIZE; i++)
    {
        fprintf(stderr, "%i:", i);
        SymbolPtr s = st->array[i];
        if (s == NULL)
        {
            fprintf(stderr, "\n");
            continue;
        }

        while (s != NULL)
        {
            fprintf(stderr, " <K: %s (%s), L: %i>", s->key, SymbolType_toString(s->type), s->location);
            s = s->next;
            if (s != NULL)
                fprintf(stderr, ", ");
        }
        fprintf(stderr, "\n");
    }
    #endif
}

/**
 * Funkce vytvoří (naalokuje) novou položku pro seznam.
 *
 * @param[in]	char		    *key	    Identifikátor položky
 * @param[in]	SymbolType	    type	    Typ symbolu
 * @param[in]	SymbolLocation	location	Umístění symbolu
 * @param[in]	void		    *value	    Hodnota položky
 *
 * @retval	SymbolPtr|NULL   Ukazatel na nově vytvořenou položku
 */
SymbolPtr Symbol_create(char *key, SymbolType type, SymbolLocation location, void *value)
{
    //	Vytvoření struktury
	SymbolPtr s = (SymbolPtr) malloc(sizeof(Symbol));
	if (s == NULL)
    {
        DEBUG_ERR("symtable", "failed to mallocate Symbol");
        return NULL;
    }

	//	Inicializace struktury
	s->next     = NULL;
	s->key      = key;
	s->type     = type;
	s->location = location;
	s->value    = value;

	return s;
}

/**
 * Funkce odstraní položku dle daného ukazatele.
 *
 * @param[in,out]	SymbolPtr	*s	Ukazatel na existující symbol
 */
void Symbol_destroy(SymbolPtr *s)
{
    SymbolPtr symbol = *s;
    if (symbol == NULL)
        return;

    if (symbol->value != NULL)
        free(symbol->value);

    free(symbol);
    *s = NULL;
}

char *SymbolType_toString(SymbolType type)
{
    switch (type)
    {
        case ST_BOOLEAN:
            return "bool";
        case ST_DOUBLE:
            return "float";
        case ST_INTEGER:
            return "int";
        case ST_STRING:
            return "string";
        case ST_FUNCTION:
            return "function";
        case ST_NONE:
            return "NONE";
        default:
            return "_UNKNOWN_";
    }
}


#endif
