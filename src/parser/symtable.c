/**
 * Tento soubor obsahuje implementaci tabulky symbolů pomocí tabulky
 * s rozptýlenými položkami.
 *
 * @author Daniel Dolejška (xdolej08)
 * @date 21.11.2017
 * @project IFJcode17Parser
 * @subject Formální jazyky a překladače (IFJ) - FIT VUT v Brně
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

#ifndef _symtable_c
#define _symtable_c

#include "symtable.h"
#include "parser.h"

#ifdef DEBUG_INCLUDE
//#include "../generator/generator.h"
//#include "../generator/instruction_list.h"
#include "../support/error_codes.h"
#include "../support/strings.h"
#else
//#include "generator.h"
//#include "instruction_list.h"
#include "error_codes.h"
#include "strings.h"
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
#define TEMPVAR_INTERNAL_NAME "__INTERNAL__VAR"

//==================================================================d=d=
//  DEKLARACE A DEFINICE ENUMERÁTORÙ A STRUKTUR
//======================================================================


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
SymbolTablePtr SymbolTable_create()
{
	//	Vytvoření struktury
	SymbolTablePtr st = (SymbolTablePtr) malloc(sizeof(SymbolTable));
	//	Inicializace struktury
	st->size  = SYMBOL_TABLE_SIZE;
	st->array = (SymbolPtr *) malloc(sizeof(Symbol) * st->size);
	st->condCount = 0;
	st->loopCount = 0;

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
	//  nebo se daný symbol nachází na framu, který je na stacku
	//	pokračuje se ve vyhledávání na daném hashi
	while (symbol != NULL && strcmp(symbol->key, key) != 0 && symbol->location <= LOCAL_FRAME)
		symbol = symbol->next;

	//	Pokud na daném hashi nic nebylo a nebo nebyl symbol nalezen vrací se NULL
	if (symbol == NULL || strcmp(symbol->key, key) != 0)
		return NULL;

	return symbol;
}

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
SymbolPtr SymbolTable_getByToken(SymbolTablePtr st, TokenPtr token)
{
    if (token->type == IDENTIFIER)
    {
        return SymbolTable_get(st, token->attr);
    }
    else if (Token_isConstant(token) == true)
    {
        SymbolPtr symbol = SymbolTable_get(st, token->attr);
        if (symbol == NULL)
        {
            int symbol_result = SymbolTable_insert(st, token->attr, TokenType_toSymbolType(token->type), CONSTANT, token->attr, &symbol);
            if (symbol_result != NO_ERROR)
            {
                return NULL;
            }
        }

        return symbol;
    }
    else
    {
        return NULL;
    }
}

/**
 * Funkce získá symbol pro dočasnou proměnnou zvoleného typu. Pokud
 * daná proměnná v tabulce symbolů ještě není, bude vytvořena.
 *
 * @param[in,out]	SymbolTablePtr      st		Ukazatel na existující tabulku symbolů
 * @param[in]		InstructionListPtr	ilist   Ukazatel na existující list instrukcí
 * @param[in]		SymbolType	        type    Typ symbolu
 * @param[in]		unsigned	        id      Identifikátor symbolu
 *
 * @retval	SymbolPtr|NULL	Ukazatel na vyhledanou položku v tabulce
 */
SymbolPtr SymbolTable_getTempVar(SymbolTablePtr st, InstructionListPtr ilist, SymbolType type, unsigned id)
{
    char *source = "symtable-getTempVar";

    //  Vytvoření názvu proměnné
    DEBUG_LOG(source, "creating temp var name");
    char tempvar_symbol_id[5];
    snprintf(tempvar_symbol_id, 5, "%i", id);

    char *name = String_concat(TEMPVAR_INTERNAL_NAME, tempvar_symbol_id, "_");
    if (name == NULL)
    {
        DEBUG_ERR(source, "failed to create variable name");
        return NULL;
    }

    DEBUG_LOG(source, "getting temp var");
    SymbolPtr s = SymbolTable_get(st, name);
    if (s == NULL)
    {
        //  Symbol nebyl v tabulce nalezen, bude vytvořen
        DEBUG_LOG(source, "creating new temp var");
        int result = SymbolTable_insert(st, name, type, TEMPORARY_FRAME, name, &s);
        if (result != NO_ERROR)
        {
            DEBUG_ERR(source, "failed to insert symbol to table");
            return NULL;
        }

        result = Instruction_defvar(ilist, s);
        if (result != NO_ERROR)
        {
            DEBUG_ERR(source, "failed to create instruction");
            return NULL;
        }

        return s;
    }
    else
    {
        //  Symbol byl nalezen
        s->type = type;
        return s;
    }
}

/**
 * Funkce odstraní symbol pro dočasnou proměnnou z tabulky symbolů.
 *
 * Při dalším použití je tak program nucen ji znovu inicializovat.
 *
 * @param[in,out]	SymbolTablePtr      st		Ukazatel na existující tabulku symbolů
 * @param[in]		InstructionListPtr	ilist   Ukazatel na existující list instrukcí
 * @param[in]		SymbolType	        type    Typ symbolu
 * @param[in]		unsigned	        id      Identifikátor symbolu
 */
void SymbolTable_deleteTempVar(SymbolTablePtr st, unsigned id)
{
    char *source = "symtable-deleteTempVar";

    //  Vytvoření názvu proměnné
    DEBUG_LOG(source, "creating temp var name");
    char tempvar_symbol_id[5];
    snprintf(tempvar_symbol_id, 5, "%i", id);

    char *name = String_concat(TEMPVAR_INTERNAL_NAME, tempvar_symbol_id, "_");
    SymbolTable_delete(st, name);
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
        DEBUG_ERR("symtable-insert", "symbol with given name already exists");
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
 * Funkce posune definované proměnné na vyšší úroveň rámce.
 *
 * @param[in,out]	SymbolTablePtr	st Ukazatel na existující tabulku symbolů
 */
void SymbolTable_pushFrame(SymbolTablePtr st)
{
    SymbolPtr s;
    for (int i = 0; i < SYMBOL_TABLE_SIZE; i++)
    {
        s = st->array[i];
        while (s != NULL)
        {
            if (s->location == LOCAL_FRAME || s->location == TEMPORARY_FRAME)
                s->location++;
            s = s->next;
        }
    }
}

/**
 * Funkce posune definované proměnné na nižší úroveň rámce.
 *
 * Proměnné které jsou aktuálně na TF jsou odstraněny.
 *
 * @param[in,out]	SymbolTablePtr	st Ukazatel na existující tabulku symbolů
 */
void SymbolTable_popFrame(SymbolTablePtr st)
{
    DEBUG_LOG("symtable-popFrame", "preparing to pop frame");
    SymbolTable_debugPrint(st);

    SymbolPtr s;
    SymbolPtr s2;
    for (int i = 0; i < SYMBOL_TABLE_SIZE; i++)
    {
        s = st->array[i];
        while (s != NULL)
        {
            if (s->location > TEMPORARY_FRAME)
            {
                s->location--;
            }
            else if (s->location == TEMPORARY_FRAME)
            {
                DEBUG_LOG("symtable-popFrame", "deleting symbol");
                Symbol_debugPrint(s);

                s2 = s->next;
                SymbolTable_deleteByPtr(st, &s);
                s = s2;
                continue;
            }
            s = s->next;
        }
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
            //  Před tímto prvkem není žádný předcházející
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
 * Funkce odstraní položku s daným ukazatelem z tabulky.
 *
 * @param[in,out]	SymbolTablePtr  st  Ukazatel na existující tabulku symbolů
 * @param[in]		SymbolPtr       *s  Ukazatel na položku k odstranění
 */
void SymbolTable_deleteByPtr(SymbolTablePtr st, SymbolPtr *s)
{
    SymbolPtr symbol = *s;
    if (symbol == NULL)
    {
        return;
    }

	//	Výpočet hashe
	unsigned hash = SymbolTable_hash(st, symbol->key);

    if (symbol == st->array[hash])
    {
        //  Tato položka je první na daném indexu
        st->array[hash] = symbol->next;
        Symbol_destroy(&symbol);
    }
    else
    {
        //  Tato položka není první na daném indexu
        SymbolPtr prev = st->array[hash];
        while (prev != NULL && prev->next != symbol)
            prev = prev->next;

        prev->next = symbol->next;
        Symbol_destroy(&symbol);
    }
    *s = NULL;
}

/**
 * Funkce zobrazí obsah tabulky na stderr.
 *
 * @param[in,out]	SymbolTablePtr  st Ukazatel na existující tabulku symbolů
 */
void SymbolTable_debugPrint(SymbolTablePtr st)
{
    #ifdef DEBUG_PRINT_SYMTABLE
    fprintf(stderr, "DEBUG | SymbolTable (%p): {\n", st);
    for (int i = 0; i < SYMBOL_TABLE_SIZE; i++)
    {
        fprintf(stderr, "\t%i:", i);
        SymbolPtr s = st->array[i];
        if (s == NULL)
        {
            fprintf(stderr, "\n");
            continue;
        }

        while (s != NULL)
        {
            fprintf(stderr, " <%s%s (%s)>", s->key, SymbolLocation_toString(s->location), SymbolType_toString(s->type));
            s = s->next;
            if (s != NULL)
                fprintf(stderr, ", ");
        }
        fprintf(stderr, "\n");
    }
    fprintf(stderr, "}\n", st);
    #endif
}

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
	s->value2   = NULL;

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

void Symbol_debugPrint(SymbolPtr symbol)
{
    #ifdef DEBUG_PRINT_SYMBOL
    if (symbol == NULL)
    {
        fprintf(stderr, "DEBUG | Symbol: NULL\n");
    }
    else
    {
        fprintf(stderr, "DEBUG | Symbol (%p): {\n", symbol);
        fprintf(stderr, "\tkey: %s,\n", symbol->key);
        fprintf(stderr, "\tlocation: %s,\n", SymbolLocation_toString(symbol->location));
        fprintf(stderr, "\ttype: %s,\n", SymbolType_toString(symbol->type));
        fprintf(stderr, "\tvalue: %p,\n", symbol->value);
        fprintf(stderr, "\tvalue2: %p,\n", symbol->value2);
        fprintf(stderr, "\tnext: %p,\n", symbol->next);
        fprintf(stderr, "}\n");
    }
    #endif
}

bool Symbol_isVariable(SymbolPtr s)
{
    switch (s->type)
    {
        case ST_BOOLEAN:
        case ST_DOUBLE:
        case ST_INTEGER:
        case ST_STRING:
            return true;
        default:
            return false;
    }
}

bool Symbol_isFunction(SymbolPtr s)
{
    switch (s->type)
    {
        case ST_FUNCTION:
            return true;
        default:
            return false;
    }
}

//-------------------------------------------------d-d-
//  SymbolInfo_Function
//-----------------------------------------------------

SymbolInfo_FunctionPtr SymbolInfo_Function_create(SymbolType dataType, SymbolInfo_Function_ParameterListPtr paramList)
{
    //	Vytvoření struktury
	SymbolInfo_FunctionPtr s = (SymbolInfo_FunctionPtr) malloc(sizeof(SymbolInfo_Function));
	if (s == NULL)
    {
        DEBUG_ERR("symtable-func_create", "failed to mallocate SymbolInfo_Function");
        return NULL;
    }

	//	Inicializace struktury
	s->dataType  = dataType;
	s->params    = paramList;
	s->isDefined = false;

	return s;
}

void SymbolInfo_Function_destroy(SymbolInfo_FunctionPtr *s)
{
    if (*s == NULL)
        return;

    SymbolInfo_Function_ParameterList_destroy(&((*s)->params));
    free(*s);
    *s = NULL;
}

void SymbolInfo_Function_debugPrint(SymbolInfo_FunctionPtr s)
{
    #ifdef DEBUG_PRINT_SYMBOLINFO
    fprintf(stderr, "DEBUG | SymbolInfo_Function (%p): {\n", s);
    fprintf(stderr, "\tdataType: %s,\n", SymbolType_toString(s->dataType));
    fprintf(stderr, "\tisDef: %i,\n", s->isDefined);
    SymbolInfo_Function_ParameterList_debugPrint(s->params);
    fprintf(stderr, "}\n", s);
    #endif
}

//-------------------------------------------------d-d-
//  SymbolInfo_Function_Parameter
//-----------------------------------------------------

SymbolInfo_Function_ParameterPtr SymbolInfo_Function_Parameter_create(char *name, SymbolType dataType)
{
    //	Vytvoření struktury
	SymbolInfo_Function_ParameterPtr s = (SymbolInfo_Function_ParameterPtr) malloc(sizeof(SymbolInfo_Function_Parameter));
	if (s == NULL)
    {
        DEBUG_ERR("symtable-func-param_create", "failed to mallocate SymbolInfo_Function_Parameter");
        return NULL;
    }

	//	Inicializace struktury
	s->dataType = dataType;
	s->name     = name;
	s->next     = NULL;

	return s;
}

void SymbolInfo_Function_Parameter_destroy(SymbolInfo_Function_ParameterPtr *s)
{
    if (*s == NULL)
        return;

    String_destroy(&((*s)->name));
    free(*s);
    *s = NULL;
}

void SymbolInfo_Function_Parameter_debugPrint(SymbolInfo_Function_ParameterPtr s)
{
    #ifdef DEBUG_PRINT_SYMBOLINFO
    fprintf(stderr, "DEBUG | SymbolInfo_FunctionParameter (%p): {\n", s);
    fprintf(stderr, "\tdataType: %s (%i)\n", SymbolType_toString(s->dataType), s->dataType);
    fprintf(stderr, "\tname: %s\n", s->name);
    fprintf(stderr, "}\n");
    #endif
}

//-------------------------------------------------d-d-
//  SymbolInfo_Function_ParameterList
//-----------------------------------------------------

SymbolInfo_Function_ParameterListPtr SymbolInfo_Function_ParameterList_create()
{
    SymbolInfo_Function_ParameterListPtr l = (SymbolInfo_Function_ParameterListPtr) malloc(sizeof(SymbolInfo_Function_ParameterList));
    if (l == NULL)
    {
        DEBUG_ERR("symtable-func-paramList_create", "failed to mallocate SymbolInfo_Function_ParameterList");
        return NULL;
    }

    l->active = NULL;
    l->first  = NULL;

    return l;
}

void SymbolInfo_Function_ParameterList_destroy(SymbolInfo_Function_ParameterListPtr *l)
{
    free(*l);
    *l = NULL;
}

int SymbolInfo_Function_ParameterList_insert(SymbolInfo_Function_ParameterListPtr l, SymbolInfo_Function_ParameterPtr param)
{
    if (param == NULL || l == NULL)
    {
        return INTERNAL_ERROR;
    }

    if (SymbolInfo_Function_ParameterList_parameterExistsWithName(l, param->name) == true)
    {
        DEBUG_ERR("symtable-func-paramList_insert", "parameter with this name already exists");
        return SEMANTICAL_DEFINITION_ERROR;
    }

    if (l->active == NULL)
    {
        if (l->first != NULL)
        {
            DEBUG_ERR("symtable-func-paramList_insert", "active is NULL, but there are items in the list!");
            return INTERNAL_ERROR;
        }

        l->first = param;
        SymbolInfo_Function_ParameterList_first(l);
    }
    else
    {
        l->active->next = param;
        SymbolInfo_Function_ParameterList_next(l);
    }

    return NO_ERROR;
}

void SymbolInfo_Function_ParameterList_first(SymbolInfo_Function_ParameterListPtr l)
{
    l->active = l->first;
}

void SymbolInfo_Function_ParameterList_next(SymbolInfo_Function_ParameterListPtr l)
{
    if (l->active != NULL)
    {
        l->active = l->active->next;
    }
}

SymbolInfo_Function_ParameterPtr SymbolInfo_Function_ParameterList_get(SymbolInfo_Function_ParameterListPtr l)
{
    return l->active;
}

SymbolInfo_Function_ParameterPtr SymbolInfo_Function_ParameterList_getNext(SymbolInfo_Function_ParameterListPtr l)
{
    if (l->active == NULL)
    {
        return NULL;
    }

    SymbolInfo_Function_ParameterList_next(l);
    return SymbolInfo_Function_ParameterList_get(l);
}

bool SymbolInfo_Function_ParameterList_parameterExistsWithName(SymbolInfo_Function_ParameterListPtr l, char *name)
{
    bool exists = false;

    SymbolInfo_Function_ParameterPtr active = l->active;
    SymbolInfo_Function_ParameterPtr param;
    SymbolInfo_Function_ParameterList_first(l);
    while (l->active != NULL)
    {
        param = SymbolInfo_Function_ParameterList_get(l);
        if (strcmp(param->name, name) == 0)
        {
            return true;
        }
        SymbolInfo_Function_ParameterList_next(l);
    }

    l->active = active;
    return exists;
}

void SymbolInfo_Function_ParameterList_deleteFirst(SymbolInfo_Function_ParameterListPtr l)
{
    if (l->first == NULL)
    {
        return;
    }

    SymbolInfo_Function_ParameterPtr i = l->first;
    l->first = l->first->next;
    SymbolInfo_Function_Parameter_destroy(&i);
}

void SymbolInfo_Function_ParameterList_debugPrint(SymbolInfo_Function_ParameterListPtr l)
{
    #ifdef DEBUG_PRINT_SYMBOLINFO
    SymbolInfo_Function_ParameterPtr active = l->active;
    SymbolInfo_Function_ParameterList_first(l);
    fprintf(stderr, "DEBUG | SymbolInfo_FunctionParameterList (%p): {\n", l);
    fprintf(stderr, "\tactive: %p\n", l->active);
    fprintf(stderr, "\tfirst: %p\n", l->first);
    while (l->active != NULL)
    {
        SymbolInfo_Function_Parameter_debugPrint(SymbolInfo_Function_ParameterList_get(l));
        SymbolInfo_Function_ParameterList_next(l);
    }
    fprintf(stderr, "}\n");
    l->active = active;
    #endif
}

//-------------------------------------------------d-d-
//  SymbolInfo_Loop
//-----------------------------------------------------

SymbolInfo_LoopPtr SymbolInfo_Loop_create(char *begin_label, char *end_label)
{
    SymbolInfo_LoopPtr i = (SymbolInfo_LoopPtr) malloc(sizeof(SymbolInfo_Loop));
    if (i == NULL)
    {
        DEBUG_ERR("symtable-info_loop_create", "faile to mallocate SymbolInfo_Loop");
        return NULL;
    }

    i->begin_label = begin_label;
    i->end_label   = end_label;

    return i;
}

void SymbolInfo_Loop_destroy(SymbolInfo_LoopPtr *s)
{
    SymbolInfo_LoopPtr symbol = *s;
    String_destroy(&(symbol->begin_label));
    String_destroy(&(symbol->end_label));
    free(symbol);
    *s = NULL;
}

//-------------------------------------------------d-d-
//  SymbolType
//-----------------------------------------------------

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
        case ST_LOOP:
            return "loop";
        case ST_COND:
            return "condition";
        case ST_NONE:
            return "NONE";
        default:
            return "_UNKNOWN_";
    }
}

bool SymbolType_isBinaryOperationOk(SymbolType type1, SymbolType type2, TokenPtr o)
{
    return SymbolType_isOperationOk(type1, o) && SymbolType_isOperationOk(type2, o);
}

bool SymbolType_isOperationOk(SymbolType type, TokenPtr o)
{
    //
    //  MATEMATIKA
    //
    if (o->type == PLUS)
    {
        //  Sčítání
        switch (type)
        {
            case ST_DOUBLE:
            case ST_INTEGER:
            case ST_STRING:
                return true;
            default:
                return false;
        }
    }
    else if (o->type == MINUS)
    {
        //  Odčítání
        switch (type)
        {
            case ST_DOUBLE:
            case ST_INTEGER:
                return true;
            default:
                return false;
        }
    }
    else if (o->type == STAR)
    {
        //  Násobení
        switch (type)
        {
            case ST_DOUBLE:
            case ST_INTEGER:
                return true;
            default:
                return false;
        }
    }
    else if (o->type == SLASH)
    {
        //  Dělení
        switch (type)
        {
            case ST_DOUBLE:
            case ST_INTEGER:
                return true;
            default:
                return false;
        }
    }
    else if (o->type == BACK_SLASH)
    {
        //  Celočíselné dělení
        switch (type)
        {
            case ST_INTEGER:
                return true;
            default:
                return false;
        }
    }
    //
    //  LOGIKA
    //
    else if (o->type == EQ)
    {
        //  Porovnání (equals)
        switch (type)
        {
            case ST_DOUBLE:
            case ST_INTEGER:
            case ST_STRING:
            case ST_BOOLEAN:
                return true;
            default:
                return false;
        }
    }
    else if (o->type == LTGT)
    {
        //  Porovnání (not equals)
        switch (type)
        {
            case ST_DOUBLE:
            case ST_INTEGER:
            case ST_STRING:
            case ST_BOOLEAN:
                return true;
            default:
                return false;
        }
    }
    else if (o->type == LT)
    {
        //  Porovnání (less than)
        switch (type)
        {
            case ST_DOUBLE:
            case ST_INTEGER:
            case ST_STRING:
            //case ST_BOOLEAN:
                return true;
            default:
                return false;
        }

    }
    else if (o->type == GT)
    {
        //  Porovnání (greater than)
        switch (type)
        {
            case ST_DOUBLE:
            case ST_INTEGER:
            case ST_STRING:
            //case ST_BOOLEAN:
                return true;
            default:
                return false;
        }
    }
    else if (o->type == AND)
    {
        //  Konjunkce
        switch (type)
        {
            case ST_DOUBLE:
            case ST_INTEGER:
            case ST_STRING:
            case ST_BOOLEAN:
                return true;
            default:
                return false;
        }
    }
    else if (o->type == OR)
    {
        //  Disjunkce
        switch (type)
        {
            case ST_DOUBLE:
            case ST_INTEGER:
            case ST_STRING:
            case ST_BOOLEAN:
                return true;
            default:
                return false;
        }
    }
    else if (o->type == NOT)
    {
        //  Negace
        switch (type)
        {
            case ST_DOUBLE:
            case ST_INTEGER:
            case ST_STRING:
            case ST_BOOLEAN:
                return true;
            default:
                return false;
        }
    }
    else
    {
        //  Neočekávaný token
        DEBUG_ERR("symbolType-isOpOk", "this type of token was not expected!");
        Token_debugPrint(o);
        return false;
    }
}

bool SymbolType_canBeConvertedTo(SymbolType source, SymbolType target)
{
    switch (source)
    {
        case ST_DOUBLE:
        case ST_INTEGER:
            if (target == ST_DOUBLE || target == ST_INTEGER)
                return true;
            return false;
        default:
            return false;
    }
}

bool SymbolType_hasToConvertOperator1(SymbolType operator1, SymbolType operator2, SymbolType *dataType)
{
    //  Implicitně ponecháme stejný datový typ
    *dataType = operator1;

    if (operator1 == ST_INTEGER)
    {
        //  První operátor je celé číslo
        if (operator2 == ST_DOUBLE)
        {
            //  INT + DOUBLE
            //  =
            //  DOUBLE + DOUBLE
            *dataType = ST_DOUBLE;
            return true;
        }
        else
        {
            //  Jedná se buď o neplatnou kombinaci datových typů, nebo není třeba konverze
            return false;
        }
    }
    else if (operator1 == ST_DOUBLE)
    {
        //  Jedná se buď o neplatnou kombinaci datových typů, nebo není třeba konverze
        return false;
    }
    else
    {
        //  Jedná se buď o neplatnou kombinaci datových typů, nebo není třeba konverze
        return false;
    }
}

bool SymbolType_hasToConvertOperator2(SymbolType operator1, SymbolType operator2, SymbolType *dataType)
{
    //  Implicitně ponecháme stejný datový typ
    *dataType = operator2;

    if (operator1 == ST_INTEGER)
    {
        //  Jedná se buď o neplatnou kombinaci datových typů, nebo není třeba konverze
        return false;
    }
    else if (operator1 == ST_DOUBLE)
    {
        //  První operátor je celé číslo
        if (operator2 == ST_INTEGER)
        {
            //  DOUBLE + INT
            //  =
            //  DOUBLE + DOUBLE
            *dataType = ST_DOUBLE;
            return true;
        }
        else
        {
            //  Jedná se buď o neplatnou kombinaci datových typů, nebo není třeba konverze
            return false;
        }
    }
    else
    {
        //  Jedná se buď o neplatnou kombinaci datových typů, nebo není třeba konverze
        return false;
    }
}

char *SymbolLocation_toString(SymbolLocation location)
{
    if (location > LOCAL_FRAME)
        return "@_FRAMESTACK";

    switch (location)
    {
        case GLOBAL_FRAME:
            return "@GF";
        case LOCAL_FRAME:
            return "@LF";
        case TEMPORARY_FRAME:
            return "@TF";
        case CONSTANT:
            return "@_CONSTANT";
        default:
            return "@_UNKNOWN_";
    }
}

#endif
