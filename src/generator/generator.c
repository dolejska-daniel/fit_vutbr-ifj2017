/**
 * Tento soubor obsahuje deklarace funkcí využité při implementaci
 * generátoru cílového kódu (generator).
 *
 * @author Lukáš Kulda (xkulda01)
 * @date 21.11.2017
 * @project IFJcode17Parser
 * @subject Formální jazyky a překladače (IFJ) - FIT VUT v Brně
 */

#include <stdio.h>

#include "generator.h"
#include "instruction_list.h"

#ifndef _generator_c
#define _generator_c

#ifdef DEBUG_INCLUDE
#include "../parser/symtable.h"
#include "../scanner/token.h"
#include "../support/error_codes.h"
#include "../support/strings.h"
#else
#include "symtable.h"
#include "token.h"
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

//==================================================================d=d=
//  DEKLARACE A DEFINICE ENUMERÁTORŮ A STRUKTUR
//======================================================================


//==================================================================d=d=
//  DEKLARACE FUNKCÍ
//======================================================================

/**
 * Funkce pro vytvoření řetězce symbolu v patřičném tvaru.
 *
 * @param[in]       SymbolPtr           symbol  Symbol obsahující informace pro vytvoření řetězce
 * @param[in,out]       char**           output  Výsledný řetězec
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_getsymbol(SymbolPtr symbol, char **output)
{
    char *s_location;
    if (symbol->location == GLOBAL_FRAME) {
        s_location = "GF";
    }
    else if (symbol->location == LOCAL_FRAME) {
        s_location = "LF";
    }
    else if (symbol->location == TEMPORARY_FRAME) {
        s_location = "TF";
    }
    else { // CONSTANT
        char *s_type;
        if (symbol->type == ST_BOOLEAN) {
            s_type = "bool";
        }
        else if (symbol->type == ST_STRING) {
            s_type = "string";
        }
        else if (symbol->type == ST_DOUBLE) {
            s_type = "float";
        }
        else if (symbol->type == ST_INTEGER) {
            s_type = "int";
        }
        else if (symbol->type == ST_NONE) {
            return INTERNAL_ERROR;
        }
        else { // ST_FUNCTION
            return INTERNAL_ERROR;
        }
        *output = String_concat(s_type, symbol->value, "@");
        if (*output == NULL)
        {
            DEBUG_ERR("generator", "instruction failed to allocate");
            return INTERNAL_ERROR;
        }
        return NO_ERROR;
    }
    *output = String_concat(s_location, symbol->value, "@");
    if (*output == NULL)
    {
        DEBUG_ERR("generator", "instruction failed to allocate");
        return INTERNAL_ERROR;
    }
    return NO_ERROR;
}
/**
 * Zapíše instrukce značící začátek funkce. Po této funkci budou
 * následovat samotné instrukce dané funkce. Toto chování bude ukončeno
 * voláním funkce Instruction_function_end.
 *
 * @param[in,out]   InstructionlistPtr  l       Ukazatel na existující seznam instrukcí
 * @param[in]       SymbolPtr           symbol  Symbol obsahující informace o funkci
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_function_begin(InstructionListPtr l, SymbolPtr symbol);

/**
 * Zapíše instrukce značící konec funkce. Postará se o uklizení rámce
 * po návratu z funkce.
 *
 * @param[in,out]   InstructionlistPtr  l       Ukazatel na existující seznam instrukcí
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_function_end(InstructionListPtr l);

/**
 * Zapíše instrukce pro návrat hodnoty funkce.
 * Tato funkce musí být volána před samotným ukončením funkce
 * (Instruction_function_end)!
 *
 * @param[in,out]   InstructionlistPtr  l       Ukazatel na existující seznam instrukcí
 * @param[in]       SymbolPtr           symbol  Symbol obsahující informace o proměnné ve funkci jejíž hodnota bude navrácena
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_function_return(InstructionListPtr l, SymbolPtr symbol);

/**
 * Zapíše instrukce pro přečtení/uložení návratové hodnoty funkce.
 * Tato funkce musí být volána po funkci Instruction_function_return
 * a samotným ukončením funkce (Instruction_function_end)!
 *
 * @param[in,out]   InstructionlistPtr  l       Ukazatel na existující seznam instrukcí
 * @param[in]       SymbolPtr           symbol  Symbol obsahující informace o proměnné, do které bude návratová hodnota funkce uložena
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_function_return_read(InstructionListPtr l, SymbolPtr symbol);

/**
 * Zapíše instrukce pro volání funkce. Připraví rámce s parametry
 * volané funkce. Parametry jsou určeny předem funkcí
 * Instruction_function_call_addParameter.
 *
 * @param[in,out]   InstructionlistPtr  l       Ukazatel na existující seznam instrukcí
 * @param[in]       SymbolPtr           symbol  Symbol obsahující informace o volané funkci
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_function_call(InstructionListPtr l, SymbolPtr symbol);

/**
 * Zapíše instrukce pro přípravu rámce - parametrů před voláním funkce.
 * Za touto funkcí může následovat pouze volání Instruction_function_call
 * či příprava dalších parametrů.
 *
 * @param[in,out]   InstructionlistPtr  l       Ukazatel na existující seznam instrukcí
 * @param[in]       SymbolPtr           symbol  Symbol obsahující informace o proměnné/konstantě, sloužící jako parametr volané funkce
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_function_call_addParameter(InstructionListPtr l, SymbolPtr symbol);

/**
 * Zapíše instrukce pro přiřazení hodnoty do proměnné.
 *
 * @param[in,out]   InstructionlistPtr  l       Ukazatel na existující seznam instrukcí
 * @param[in]       SymbolPtr           variable    Symbol obsahující informace o proměnné
 * @param[in]       SymbolPtr           symbol      Symbol obsahující informace o proměnné/konstantě, sloužící jako hodnota proměnné
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_move(InstructionListPtr l, SymbolPtr variable, SymbolPtr symbol)
{
    if (variable->location == CONSTANT) {
        DEBUG_ERR("generator", "first operand cannot be constant");
        return INTERNAL_ERROR;
    }
    char *s_var;
    Instruction_getsymbol(variable, &s_var);

    char *s_symbol;
    Instruction_getsymbol(symbol, &s_symbol);

    char *instruction = String_printf("MOVE %s %s", s_var, s_symbol, NULL, NULL);
    if (instruction == NULL)
    {
        DEBUG_ERR("generator", "instruction failed to allocate");
        return INTERNAL_ERROR;
    }

    InstructionPtr ins_ptr = InstructionList_insertLast(l, instruction);
    if (ins_ptr == NULL)
    {
        DEBUG_ERR("generator", "instruction pointer failed to allocate");
        return INTERNAL_ERROR;
    }
    return NO_ERROR;
}

/**
 * Zapíše instrukce pro vytvoření nového dočasného rámce.
 *
 * @param[in,out]   InstructionlistPtr  l       Ukazatel na existující seznam instrukcí
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_createframe(InstructionListPtr l)
{
    char *instruction = String_create("CREATEFRAME");
    if (instruction == NULL)
    {
        DEBUG_ERR("generator", "instruction failed to allocate");
        return INTERNAL_ERROR;
    }

    InstructionPtr ins_ptr = InstructionList_insertLast(l, instruction);
    if (ins_ptr == NULL)
    {
        DEBUG_ERR("generator", "instruction pointer failed to allocate");
        return INTERNAL_ERROR;
    }
    return NO_ERROR;
}

/**
 * Zapíše instrukce pro přesun dočasného rámce na zásobník rámců.
 *
 * @param[in,out]   InstructionlistPtr  l       Ukazatel na existující seznam instrukcí
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_pushframe(InstructionListPtr l)
{
    char *instruction = String_create("PUSHFRAME");
    if (instruction == NULL)
    {
        DEBUG_ERR("generator", "instruction failed to allocate");
        return INTERNAL_ERROR;
    }

    InstructionPtr ins_ptr = InstructionList_insertLast(l, instruction);
    if (ins_ptr == NULL)
    {
        DEBUG_ERR("generator", "instruction pointer failed to allocate");
        return INTERNAL_ERROR;
    }
    return NO_ERROR;
}

/**
 * Zapíše instrukce pro přesun aktuálního rámce do dočasného.
 *
 * @param[in,out]   InstructionlistPtr  l       Ukazatel na existující seznam instrukcí
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_popframe(InstructionListPtr l)
{
    char *instruction = String_create("POPFRAME");
    if (instruction == NULL)
    {
        DEBUG_ERR("generator", "instruction failed to allocate");
        return INTERNAL_ERROR;
    }

    InstructionPtr ins_ptr = InstructionList_insertLast(l, instruction);
    if (ins_ptr == NULL)
    {
        DEBUG_ERR("generator", "instruction pointer failed to allocate");
        return INTERNAL_ERROR;
    }
    return NO_ERROR;
}

/**
 * Zapíše instrukce pro definování proměnné v určeném rámci.
 *
 * @param[in,out]   InstructionlistPtr  l       Ukazatel na existující seznam instrukcí
 * @param[in]       SymbolPtr           symbol  Symbol obsahující informace o definované proměnné
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_defvar(InstructionListPtr l, SymbolPtr symbol)
{
    char *s_symbol;
    Instruction_getsymbol(symbol, &s_symbol);

    char *instruction = String_concat("DEFVAR", s_symbol, " ");
    if (instruction == NULL)
    {
        DEBUG_ERR("generator", "instruction failed to allocate");
        return INTERNAL_ERROR;
    }

    InstructionPtr ins_ptr = InstructionList_insertLast(l, instruction);
    if (ins_ptr == NULL)
    {
        DEBUG_ERR("generator", "instruction pointer failed to allocate");
        return INTERNAL_ERROR;
    }
    return NO_ERROR;
}

/**
 * Zapíše instrukce pro skok na návěstí s podporou návratu.
 *
 * @param[in,out]   InstructionlistPtr  l       Ukazatel na existující seznam instrukcí
 * @param[in]       SymbolPtr           symbol  Symbol obsahující informace o návěstí
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_call(InstructionListPtr l, SymbolPtr symbol) /// TODO:
{
    if (symbol->type == ST_FUNCTION) {
        DEBUG_ERR("generator", "first operand has to be label of function");
        return INTERNAL_ERROR;
    }
    char *s_symbol;
    Instruction_getsymbol(symbol, &s_symbol);

    char *instruction = String_concat("CALL", s_symbol, " ");
    if (instruction == NULL)
    {
        DEBUG_ERR("generator", "instruction failed to allocate");
        return INTERNAL_ERROR;
    }

    InstructionPtr ins_ptr = InstructionList_insertLast(l, instruction);
    if (ins_ptr == NULL)
    {
        DEBUG_ERR("generator", "instruction pointer failed to allocate");
        return INTERNAL_ERROR;
    }
    return NO_ERROR;
}

/**
 * Zapíše instrukce pro návrat na pozici uloženou instrukcí CALL.
 *
 * @param[in,out]   InstructionlistPtr  l       Ukazatel na existující seznam instrukcí
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_return(InstructionListPtr l)
{
    char *instruction = String_create("RETURN");
    if (instruction == NULL)
    {
        DEBUG_ERR("generator", "instruction failed to allocate");
        return INTERNAL_ERROR;
    }

    InstructionPtr ins_ptr = InstructionList_insertLast(l, instruction);
    if (ins_ptr == NULL)
    {
        DEBUG_ERR("generator", "instruction pointer failed to allocate");
        return INTERNAL_ERROR;
    }
    return NO_ERROR;
}


//-------------------------------------------------d-d-
//  Instructions: Variables
//-----------------------------------------------------

/**
 * Zapíše instrukce pro deklaraci proměnné. Nastaví její hodnotu na
 * implicitní dle jejího typu.
 *
 * @param[in,out]   InstructionlistPtr  l       Ukazatel na existující seznam instrukcí
 * @param[in]       SymbolPtr           symbol  Symbol obsahující informace o proměnné
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_variable_declare(InstructionListPtr l, SymbolPtr symbol)
{
    int result;
    char *default_value;

    switch (symbol->type)
    {
        case ST_BOOLEAN:
            default_value = "false";
            break;
        case ST_STRING:
            default_value = "";
            break;
        case ST_DOUBLE:
            default_value = "0.0";
            break;
        case ST_INTEGER:
            default_value = "0";
            break;
        default:
            return INTERNAL_ERROR;
    }

    SymbolPtr newSymbol = Symbol_create(symbol->key, symbol->type, CONSTANT, default_value);
    if (newSymbol == NULL)
    {
        return INTERNAL_ERROR;
    }

    result = Instruction_defvar(l, symbol);
    if (result != NO_ERROR)
        return result;

    result = Instruction_move(l, symbol, newSymbol);
    if (result != NO_ERROR)
        return result;

    Symbol_destroy(&newSymbol);

    return NO_ERROR;
}
//TODO:
/**
 * Zapíše instrukce pro definici proměnné.
 *
 * @param[in,out]   InstructionlistPtr  l           Ukazatel na existující seznam instrukcí
 * @param[in]       SymbolPtr           variable    Symbol obsahující informace o proměnné, jejíž hodnota bude definována
 * @param[in]       SymbolPtr           symbol      Symbol obsahující informace o proměnné/konstantě, sloužící jako hodnota proměnné
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_variable_assign(InstructionListPtr l, SymbolPtr variable, SymbolPtr symbol)
{
    int result;
    result = Instruction_defvar(l, variable);
    if (result != NO_ERROR)
        return result;

    result = Instruction_move(l, variable, symbol);
    if (result != NO_ERROR)
        return result;

    return NO_ERROR;
}

/**
 * Zapíše instrukce pro definici proměnné pomocí hodnoty ze stacku.
 *
 * @param[in,out]   InstructionlistPtr  l           Ukazatel na existující seznam instrukcí
 * @param[in]       SymbolPtr           variable    Symbol obsahující informace o proměnné, jejíž hodnota bude definována
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_variable_assignFromStack(InstructionListPtr l, SymbolPtr variable)
{
    return Instruction_stack_pop(l, variable);
}


//-------------------------------------------------d-d-
//  Instructions: Stack
//-----------------------------------------------------

/**
 * Zapíše instrukce pro vložení hodnoty na vrchol datového zásobníku.
 *
 * @param[in,out]   InstructionlistPtr  l       Ukazatel na existující seznam instrukcí
 * @param[in]       SymbolPtr           symbol  Symbol obsahující informace o proměnné/konstantě
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_stack_push(InstructionListPtr l, SymbolPtr symbol)
{
    char *s_symbol;
    Instruction_getsymbol(symbol, &s_symbol);

    char *instruction = String_concat("PUSHS", s_symbol, " ");
    if (instruction == NULL)
    {
        DEBUG_ERR("generator", "instruction failed to allocate");
        return INTERNAL_ERROR;
    }

    InstructionPtr ins_ptr = InstructionList_insertLast(l, instruction);
    if (ins_ptr == NULL)
    {
        DEBUG_ERR("generator", "instruction pointer failed to allocate");
        return INTERNAL_ERROR;
    }
    return NO_ERROR;
}

/**
 * Zapíše instrukce pro vyjmutí hodnoty z vrcholu datového zásobníku.
 *
 * @param[in,out]   InstructionlistPtr  l       Ukazatel na existující seznam instrukcí
 * @param[in]       SymbolPtr           symbol  Symbol obsahující informace o proměnné/konstantě
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_stack_pop(InstructionListPtr l, SymbolPtr symbol)
{
    if (symbol->location == CONSTANT) {
        DEBUG_ERR("generator", "first operand cannot be constant");
        return INTERNAL_ERROR;
    }
    char *s_symbol;
    Instruction_getsymbol(symbol, &s_symbol);

    char *instruction = String_concat("POPS", s_symbol, " ");
    if (instruction == NULL)
    {
        DEBUG_ERR("generator", "instruction failed to allocate");
        return INTERNAL_ERROR;
    }

    InstructionPtr ins_ptr = InstructionList_insertLast(l, instruction);
    if (ins_ptr == NULL)
    {
        DEBUG_ERR("generator", "instruction pointer failed to allocate");
        return INTERNAL_ERROR;
    }
    return NO_ERROR;
}

/**
 * Zapíše instrukce pro vyjmutí hodnoty z vrcholu datového zásobníku.
 *
 * @param[in,out]   InstructionlistPtr  l       Ukazatel na existující seznam instrukcí
 * @param[in]       SymbolPtr           symbol  Symbol obsahující informace o proměnné/konstantě
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_stack_clear(InstructionListPtr l)
{
    char *instruction = String_create("CLEARS");
    if (instruction == NULL)
    {
        DEBUG_ERR("generator", "instruction failed to allocate");
        return INTERNAL_ERROR;
    }

    InstructionPtr ins_ptr = InstructionList_insertLast(l, instruction);
    if (ins_ptr == NULL)
    {
        DEBUG_ERR("generator", "instruction pointer failed to allocate");
        return INTERNAL_ERROR;
    }
    return NO_ERROR;
}


//-------------------------------------------------d-d-
//  Instructions: Math
//-----------------------------------------------------

/**
 * Zapíše instrukce pro součet proměnných/konstant.
 * V případě nutnosti provádí požadované konverze datových typů.
 *
 * @param[in,out]   InstructionlistPtr  l           Ukazatel na existující seznam instrukcí
 * @param[in]       SymbolPtr           variable    Symbol obsahující informace o proměnné, kam bude hodnota výpočtu uložena
 * @param[in]       SymbolPtr           symbol1     Symbol obsahující informace o proměnné/konstantě, sloužící jako hodnota ve výpočtu
 * @param[in]       SymbolPtr           symbol2     Symbol obsahující informace o proměnné/konstantě, sloužící jako hodnota ve výpočtu
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_add(InstructionListPtr l, SymbolPtr variable, SymbolPtr symbol1, SymbolPtr symbol2)
{
    if (variable->location == CONSTANT) {
        DEBUG_ERR("generator", "first operand cannot be constant");
        return INTERNAL_ERROR;
    }
    char *s_var;
    Instruction_getsymbol(variable, &s_var);

    char *s_symbol1;
    Instruction_getsymbol(symbol1, &s_symbol1);

    char *s_symbol2;
    Instruction_getsymbol(symbol2, &s_symbol2);

    char *instruction = String_printf("ADD %s %s %s", s_var, s_symbol1, s_symbol2, NULL);
    if (instruction == NULL)
    {
        DEBUG_ERR("generator", "instruction failed to allocate");
        return INTERNAL_ERROR;
    }

    InstructionPtr ins_ptr = InstructionList_insertLast(l, instruction);
    if (ins_ptr == NULL)
    {
        DEBUG_ERR("generator", "instruction pointer failed to allocate");
        return INTERNAL_ERROR;
    }
    return NO_ERROR;
}

/**
 * Zapíše instrukce pro rozdíl proměnných/konstant.
 * V případě nutnosti provádí požadované konverze datových typů.
 *
 * @param[in,out]   InstructionlistPtr  l           Ukazatel na existující seznam instrukcí
 * @param[in]       SymbolPtr           variable    Symbol obsahující informace o proměnné, kam bude hodnota výpočtu uložena
 * @param[in]       SymbolPtr           symbol1     Symbol obsahující informace o proměnné/konstantě, sloužící jako hodnota ve výpočtu
 * @param[in]       SymbolPtr           symbol2     Symbol obsahující informace o proměnné/konstantě, sloužící jako hodnota ve výpočtu
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_sub(InstructionListPtr l, SymbolPtr variable, SymbolPtr symbol1, SymbolPtr symbol2)
{
    if (variable->location == CONSTANT) {
        DEBUG_ERR("generator", "first operand cannot be constant");
        return INTERNAL_ERROR;
    }
    char *s_var;
    Instruction_getsymbol(variable, &s_var);

    char *s_symbol1;
    Instruction_getsymbol(symbol1, &s_symbol1);

    char *s_symbol2;
    Instruction_getsymbol(symbol2, &s_symbol2);

    char *instruction = String_printf("SUB %s %s %s", s_var, s_symbol1, s_symbol2, NULL);
    if (instruction == NULL)
    {
        DEBUG_ERR("generator", "instruction failed to allocate");
        return INTERNAL_ERROR;
    }

    InstructionPtr ins_ptr = InstructionList_insertLast(l, instruction);
    if (ins_ptr == NULL)
    {
        DEBUG_ERR("generator", "instruction pointer failed to allocate");
        return INTERNAL_ERROR;
    }
    return NO_ERROR;
}

/**
 * Zapíše instrukce pro dělení proměnných/konstant.
 * V případě nutnosti provádí požadované konverze datových typů.
 *
 * @param[in,out]   InstructionlistPtr  l           Ukazatel na existující seznam instrukcí
 * @param[in]       SymbolPtr           variable    Symbol obsahující informace o proměnné, kam bude hodnota výpočtu uložena
 * @param[in]       SymbolPtr           symbol1     Symbol obsahující informace o proměnné/konstantě, sloužící jako hodnota ve výpočtu
 * @param[in]       SymbolPtr           symbol2     Symbol obsahující informace o proměnné/konstantě, sloužící jako hodnota ve výpočtu
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_div(InstructionListPtr l, SymbolPtr variable, SymbolPtr symbol1, SymbolPtr symbol2)
{
    if (variable->location == CONSTANT) {
        DEBUG_ERR("generator", "first operand cannot be constant");
        return INTERNAL_ERROR;
    }
    char *s_var;
    Instruction_getsymbol(variable, &s_var);

    char *s_symbol1;
    Instruction_getsymbol(symbol1, &s_symbol1);

    char *s_symbol2;
    Instruction_getsymbol(symbol2, &s_symbol2);

    char *instruction = String_printf("DIV %s %s %s", s_var, s_symbol1, s_symbol2, NULL);
    if (instruction == NULL)
    {
        DEBUG_ERR("generator", "instruction failed to allocate");
        return INTERNAL_ERROR;
    }

    InstructionPtr ins_ptr = InstructionList_insertLast(l, instruction);
    if (ins_ptr == NULL)
    {
        DEBUG_ERR("generator", "instruction pointer failed to allocate");
        return INTERNAL_ERROR;
    }
    return NO_ERROR;
}

/**
 * Zapíše instrukce pro násobení proměnných/konstant.
 * V případě nutnosti provádí požadované konverze datových typů.
 *
 * @param[in,out]   InstructionlistPtr  l           Ukazatel na existující seznam instrukcí
 * @param[in]       SymbolPtr           variable    Symbol obsahující informace o proměnné, kam bude hodnota výpočtu uložena
 * @param[in]       SymbolPtr           symbol1     Symbol obsahující informace o proměnné/konstantě, sloužící jako hodnota ve výpočtu
 * @param[in]       SymbolPtr           symbol2     Symbol obsahující informace o proměnné/konstantě, sloužící jako hodnota ve výpočtu
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_mul(InstructionListPtr l, SymbolPtr variable, SymbolPtr symbol1, SymbolPtr symbol2)
{
    if (variable->location == CONSTANT) {
        DEBUG_ERR("generator", "first operand cannot be constant");
        return INTERNAL_ERROR;
    }
    char *s_var;
    Instruction_getsymbol(variable, &s_var);

    char *s_symbol1;
    Instruction_getsymbol(symbol1, &s_symbol1);

    char *s_symbol2;
    Instruction_getsymbol(symbol2, &s_symbol2);

    char *instruction = String_printf("MUL %s %s %s", s_var, s_symbol1, s_symbol2, NULL);
    if (instruction == NULL)
    {
        DEBUG_ERR("generator", "instruction failed to allocate");
        return INTERNAL_ERROR;
    }

    InstructionPtr ins_ptr = InstructionList_insertLast(l, instruction);
    if (ins_ptr == NULL)
    {
        DEBUG_ERR("generator", "instruction pointer failed to allocate");
        return INTERNAL_ERROR;
    }
    return NO_ERROR;
}

//-------------------------------------------------d-d-
//  Instructions: Math for stack
//-----------------------------------------------------

/**
 * Zapíše instrukce pro součet proměnných/konstant na stacku.
 *
 * @param[in,out]   InstructionlistPtr  l   Ukazatel na existující seznam instrukcí
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_add_stack(InstructionListPtr l)
{
    char *instruction = String_create("ADDS");
    if (instruction == NULL)
    {
        DEBUG_ERR("generator", "instruction failed to allocate");
        return INTERNAL_ERROR;
    }

    InstructionPtr ins_ptr = InstructionList_insertLast(l, instruction);
    if (ins_ptr == NULL)
    {
        DEBUG_ERR("generator", "instruction pointer failed to allocate");
        return INTERNAL_ERROR;
    }
    return NO_ERROR;
}

/**
 * Zapíše instrukce pro rozdíl proměnných/konstant na stacku.
 *
 * @param[in,out]   InstructionlistPtr  l   Ukazatel na existující seznam instrukcí
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_sub_stack(InstructionListPtr l)
{
    char *instruction = String_create("SUBS");
    if (instruction == NULL)
    {
        DEBUG_ERR("generator", "instruction failed to allocate");
        return INTERNAL_ERROR;
    }

    InstructionPtr ins_ptr = InstructionList_insertLast(l, instruction);
    if (ins_ptr == NULL)
    {
        DEBUG_ERR("generator", "instruction pointer failed to allocate");
        return INTERNAL_ERROR;
    }
    return NO_ERROR;
}

/**
 * Zapíše instrukce pro dělení proměnných/konstant na stacku.
 *
 * @param[in,out]   InstructionlistPtr  l   Ukazatel na existující seznam instrukcí
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_div_stack(InstructionListPtr l)
{
    char *instruction = String_create("DIVS");
    if (instruction == NULL)
    {
        DEBUG_ERR("generator", "instruction failed to allocate");
        return INTERNAL_ERROR;
    }

    InstructionPtr ins_ptr = InstructionList_insertLast(l, instruction);
    if (ins_ptr == NULL)
    {
        DEBUG_ERR("generator", "instruction pointer failed to allocate");
        return INTERNAL_ERROR;
    }
    return NO_ERROR;
}

/**
 * Zapíše instrukce pro násobení proměnných/konstant na stacku.
 *
 * @param[in,out]   InstructionlistPtr  l   Ukazatel na existující seznam instrukcí
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_mul_stack(InstructionListPtr l)
{
    char *instruction = String_create("MULS");
    if (instruction == NULL)
    {
        DEBUG_ERR("generator", "instruction failed to allocate");
        return INTERNAL_ERROR;
    }

    InstructionPtr ins_ptr = InstructionList_insertLast(l, instruction);
    if (ins_ptr == NULL)
    {
        DEBUG_ERR("generator", "instruction pointer failed to allocate");
        return INTERNAL_ERROR;
    }
    return NO_ERROR;
}

//-------------------------------------------------d-d-
//  Instructions: Logic
//-----------------------------------------------------

/**
 * Zapíše instrukce pro porovnání dvou proměnných/konstant relačním operátorem EQ.
 *
 * @param[in,out]   InstructionlistPtr  l           Ukazatel na existující seznam instrukcí
 * @param[in]       SymbolPtr           variable    Symbol obsahující informace o booleovské proměnné, kam bude výsledek porovnání uložena
 * @param[in]       SymbolPtr           symbol1     Symbol obsahující informace o porovnávané proměnné/konstantě
 * @param[in]       SymbolPtr           symbol2     Symbol obsahující informace o porovnávané proměnné/konstantě
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_logic_eq(InstructionListPtr l, SymbolPtr variable, SymbolPtr symbol1, SymbolPtr symbol2)
{
    if (variable->location == CONSTANT) {
        DEBUG_ERR("generator", "first operand cannot be constant");
        return INTERNAL_ERROR;
    }
    /*if (symbol1->type != symbol2->type) {
        DEBUG_ERR("generator", "second and third operand have to be same type");
        return SEMANTICAL_DATATYPE_ERROR;
    }*/
    char *s_var;
    Instruction_getsymbol(variable, &s_var);

    char *s_symbol1;
    Instruction_getsymbol(symbol1, &s_symbol1);

    char *s_symbol2;
    Instruction_getsymbol(symbol2, &s_symbol2);

    char *instruction = String_printf("EQ %s %s %s", s_var, s_symbol1, s_symbol2, NULL);
    if (instruction == NULL)
    {
        DEBUG_ERR("generator", "instruction failed to allocate");
        return INTERNAL_ERROR;
    }

    InstructionPtr ins_ptr = InstructionList_insertLast(l, instruction);
    if (ins_ptr == NULL)
    {
        DEBUG_ERR("generator", "instruction pointer failed to allocate");
        return INTERNAL_ERROR;
    }
    return NO_ERROR;
}

/**
 * Zapíše instrukce pro porovnání dvou proměnných/konstant relačním operátorem LT.
 *
 * @param[in,out]   InstructionlistPtr  l           Ukazatel na existující seznam instrukcí
 * @param[in]       SymbolPtr           variable    Symbol obsahující informace o booleovské proměnné, kam bude výsledek porovnání uložena
 * @param[in]       SymbolPtr           symbol1     Symbol obsahující informace o porovnávané proměnné/konstantě
 * @param[in]       SymbolPtr           symbol2     Symbol obsahující informace o porovnávané proměnné/konstantě
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_logic_lt(InstructionListPtr l, SymbolPtr variable, SymbolPtr symbol1, SymbolPtr symbol2)
{
    if (variable->location == CONSTANT) {
        DEBUG_ERR("generator", "first operand cannot be constant");
        return INTERNAL_ERROR;
    }
    /*if (symbol1->type != symbol2->type) {
        DEBUG_ERR("generator", "second and third operand have to be same type");
        return SEMANTICAL_DATATYPE_ERROR;
    }*/
    char *s_var;
    Instruction_getsymbol(variable, &s_var);

    char *s_symbol1;
    Instruction_getsymbol(symbol1, &s_symbol1);

    char *s_symbol2;
    Instruction_getsymbol(symbol2, &s_symbol2);

    char *instruction = String_printf("LT %s %s %s", s_var, s_symbol1, s_symbol2, NULL);
    if (instruction == NULL)
    {
        DEBUG_ERR("generator", "instruction failed to allocate");
        return INTERNAL_ERROR;
    }

    InstructionPtr ins_ptr = InstructionList_insertLast(l, instruction);
    if (ins_ptr == NULL)
    {
        DEBUG_ERR("generator", "instruction pointer failed to allocate");
        return INTERNAL_ERROR;
    }
    return NO_ERROR;
}

/**
 * Zapíše instrukce pro porovnání dvou proměnných/konstant relačním operátorem GT.
 *
 * @param[in,out]   InstructionlistPtr  l           Ukazatel na existující seznam instrukcí
 * @param[in]       SymbolPtr           variable    Symbol obsahující informace o booleovské proměnné, kam bude výsledek porovnání uložena
 * @param[in]       SymbolPtr           symbol1     Symbol obsahující informace o porovnávané proměnné/konstantě
 * @param[in]       SymbolPtr           symbol2     Symbol obsahující informace o porovnávané proměnné/konstantě
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_logic_gt(InstructionListPtr l, SymbolPtr variable, SymbolPtr symbol1, SymbolPtr symbol2)
{
    if (variable->location == CONSTANT) {
        DEBUG_ERR("generator", "first operand cannot be constant");
        return INTERNAL_ERROR;
    }
    /*if (symbol1->type != symbol2->type) {
        DEBUG_ERR("generator", "second and third operand have to be same type");
        return SEMANTICAL_DATATYPE_ERROR;
    }*/
    char *s_var;
    Instruction_getsymbol(variable, &s_var);

    char *s_symbol1;
    Instruction_getsymbol(symbol1, &s_symbol1);

    char *s_symbol2;
    Instruction_getsymbol(symbol2, &s_symbol2);

    char *instruction = String_printf("GT %s %s %s", s_var, s_symbol1, s_symbol2, NULL);
    if (instruction == NULL)
    {
        DEBUG_ERR("generator", "instruction failed to allocate");
        return INTERNAL_ERROR;
    }

    InstructionPtr ins_ptr = InstructionList_insertLast(l, instruction);
    if (ins_ptr == NULL)
    {
        DEBUG_ERR("generator", "instruction pointer failed to allocate");
        return INTERNAL_ERROR;
    }
    return NO_ERROR;
}

/**
 * Zapíše instrukce pro konjukci (logické A) dvou proměnných/konstant.
 *
 * @param[in,out]   InstructionlistPtr  l           Ukazatel na existující seznam instrukcí
 * @param[in]       SymbolPtr           variable    Symbol obsahující informace o booleovské proměnné, kam bude hodnota porovnání uložena
 * @param[in]       SymbolPtr           symbol1     Symbol obsahující informace o porovnávané proměnné/konstantě
 * @param[in]       SymbolPtr           symbol2     Symbol obsahující informace o porovnávané proměnné/konstantě
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_logic_and(InstructionListPtr l, SymbolPtr variable, SymbolPtr symbol1, SymbolPtr symbol2)
{
    if (variable->location == CONSTANT) {
        DEBUG_ERR("generator", "first operand cannot be constant");
        return INTERNAL_ERROR;
    }
    char *s_var;
    Instruction_getsymbol(variable, &s_var);

    char *s_symbol1;
    Instruction_getsymbol(symbol1, &s_symbol1);

    char *s_symbol2;
    Instruction_getsymbol(symbol2, &s_symbol2);

    char *instruction = String_printf("AND %s %s %s", s_var, s_symbol1, s_symbol2, NULL);
    if (instruction == NULL)
    {
        DEBUG_ERR("generator", "instruction failed to allocate");
        return INTERNAL_ERROR;
    }

    InstructionPtr ins_ptr = InstructionList_insertLast(l, instruction);
    if (ins_ptr == NULL)
    {
        DEBUG_ERR("generator", "instruction pointer failed to allocate");
        return INTERNAL_ERROR;
    }
    return NO_ERROR;
}

/**
 * Zapíše instrukce pro disjunkci (logické NEBO) dvou proměnných/konstant.
 *
 * @param[in,out]   InstructionlistPtr  l           Ukazatel na existující seznam instrukcí
 * @param[in]       SymbolPtr           variable    Symbol obsahující informace o booleovské proměnné, kam bude hodnota porovnání uložena
 * @param[in]       SymbolPtr           symbol1     Symbol obsahující informace o porovnávané proměnné/konstantě
 * @param[in]       SymbolPtr           symbol2     Symbol obsahující informace o porovnávané proměnné/konstantě
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_logic_or(InstructionListPtr l, SymbolPtr variable, SymbolPtr symbol1, SymbolPtr symbol2)
{
    if (variable->location == CONSTANT) {
        DEBUG_ERR("generator", "first operand cannot be constant");
        return INTERNAL_ERROR;
    }
    char *s_var;
    Instruction_getsymbol(variable, &s_var);

    char *s_symbol1;
    Instruction_getsymbol(symbol1, &s_symbol1);

    char *s_symbol2;
    Instruction_getsymbol(symbol2, &s_symbol2);

    char *instruction = String_printf("OR %s %s %s", s_var, s_symbol1, s_symbol2, NULL);
    if (instruction == NULL)
    {
        DEBUG_ERR("generator", "instruction failed to allocate");
        return INTERNAL_ERROR;
    }

    InstructionPtr ins_ptr = InstructionList_insertLast(l, instruction);
    if (ins_ptr == NULL)
    {
        DEBUG_ERR("generator", "instruction pointer failed to allocate");
        return INTERNAL_ERROR;
    }
    return NO_ERROR;
}

/**
 * Zapíše instrukce pro negaci (NOT) proměnné/konstanty.
 *
 * @param[in,out]   InstructionlistPtr  l           Ukazatel na existující seznam instrukcí
 * @param[in]       SymbolPtr           variable    Symbol obsahující informace o booleovské proměnné, kam bude hodnota výsledku uložena
 * @param[in]       SymbolPtr           symbol    Symbol obsahující informace o negované proměnné/konstantě
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_logic_not(InstructionListPtr l, SymbolPtr variable, SymbolPtr symbol)
{
    if (variable->location == CONSTANT) {
        DEBUG_ERR("generator", "first operand cannot be constant");
        return INTERNAL_ERROR;
    }
    char *s_var;
    Instruction_getsymbol(variable, &s_var);

    char *s_symbol;
    Instruction_getsymbol(symbol, &s_symbol);

    char *instruction = String_printf("NOT %s %s", s_var, s_symbol, NULL, NULL);
    if (instruction == NULL)
    {
        DEBUG_ERR("generator", "instruction failed to allocate");
        return INTERNAL_ERROR;
    }

    InstructionPtr ins_ptr = InstructionList_insertLast(l, instruction);
    if (ins_ptr == NULL)
    {
        DEBUG_ERR("generator", "instruction pointer failed to allocate");
        return INTERNAL_ERROR;
    }
    return NO_ERROR;
}

//-------------------------------------------------d-d-
//  Instructions: Logic for stack
//-----------------------------------------------------

/**
 * Zapíše instrukce pro porovnání dvou proměnných/konstant relačním operátorem EQ na stacku.
 *
 * @param[in,out]   InstructionlistPtr  l           Ukazatel na existující seznam instrukcí
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_logic_eq_stack(InstructionListPtr l)
{
    char *instruction = String_create("EQS");
    if (instruction == NULL)
    {
        DEBUG_ERR("generator", "instruction failed to allocate");
        return INTERNAL_ERROR;
    }

    InstructionPtr ins_ptr = InstructionList_insertLast(l, instruction);
    if (ins_ptr == NULL)
    {
        DEBUG_ERR("generator", "instruction pointer failed to allocate");
        return INTERNAL_ERROR;
    }
    return NO_ERROR;
}

/**
 * Zapíše instrukce pro porovnání dvou proměnných/konstant relačním operátorem LT na stacku.
 *
 * @param[in,out]   InstructionlistPtr  l           Ukazatel na existující seznam instrukcí
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_logic_lt_stack(InstructionListPtr l)
{
    char *instruction = String_create("LTS");
    if (instruction == NULL)
    {
        DEBUG_ERR("generator", "instruction failed to allocate");
        return INTERNAL_ERROR;
    }

    InstructionPtr ins_ptr = InstructionList_insertLast(l, instruction);
    if (ins_ptr == NULL)
    {
        DEBUG_ERR("generator", "instruction pointer failed to allocate");
        return INTERNAL_ERROR;
    }
    return NO_ERROR;
}

/**
 * Zapíše instrukce pro porovnání dvou proměnných/konstant relačním operátorem GT na stacku.
 *
 * @param[in,out]   InstructionlistPtr  l           Ukazatel na existující seznam instrukcí
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_logic_gt_stack(InstructionListPtr l)
{
    char *instruction = String_create("GTS");
    if (instruction == NULL)
    {
        DEBUG_ERR("generator", "instruction failed to allocate");
        return INTERNAL_ERROR;
    }

    InstructionPtr ins_ptr = InstructionList_insertLast(l, instruction);
    if (ins_ptr == NULL)
    {
        DEBUG_ERR("generator", "instruction pointer failed to allocate");
        return INTERNAL_ERROR;
    }
    return NO_ERROR;
}

/**
 * Zapíše instrukce pro konjukci (logické A) dvou proměnných/konstant na stacku.
 *
 * @param[in,out]   InstructionlistPtr  l           Ukazatel na existující seznam instrukcí
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_logic_and_stack(InstructionListPtr l)
{
    char *instruction = String_create("ANDS");
    if (instruction == NULL)
    {
        DEBUG_ERR("generator", "instruction failed to allocate");
        return INTERNAL_ERROR;
    }

    InstructionPtr ins_ptr = InstructionList_insertLast(l, instruction);
    if (ins_ptr == NULL)
    {
        DEBUG_ERR("generator", "instruction pointer failed to allocate");
        return INTERNAL_ERROR;
    }
    return NO_ERROR;
}

/**
 * Zapíše instrukce pro disjunkci (logické NEBO) dvou proměnných/konstant na stacku.
 *
 * @param[in,out]   InstructionlistPtr  l           Ukazatel na existující seznam instrukcí
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_logic_or_stack(InstructionListPtr l)
{
    char *instruction = String_create("ORS");
    if (instruction == NULL)
    {
        DEBUG_ERR("generator", "instruction failed to allocate");
        return INTERNAL_ERROR;
    }

    InstructionPtr ins_ptr = InstructionList_insertLast(l, instruction);
    if (ins_ptr == NULL)
    {
        DEBUG_ERR("generator", "instruction pointer failed to allocate");
        return INTERNAL_ERROR;
    }
    return NO_ERROR;
}

/**
 * Zapíše instrukce pro negaci (NOT) proměnné/konstanty na stacku.
 *
 * @param[in,out]   InstructionlistPtr  l           Ukazatel na existující seznam instrukcí
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_logic_not_stack(InstructionListPtr l)
{
    char *instruction = String_create("NOTS");
    if (instruction == NULL)
    {
        DEBUG_ERR("generator", "instruction failed to allocate");
        return INTERNAL_ERROR;
    }

    InstructionPtr ins_ptr = InstructionList_insertLast(l, instruction);
    if (ins_ptr == NULL)
    {
        DEBUG_ERR("generator", "instruction pointer failed to allocate");
        return INTERNAL_ERROR;
    }
    return NO_ERROR;
}

//-------------------------------------------------d-d-
//  Instructions: Conversions
//-----------------------------------------------------

/**
 * Zapíše instrukce pro uložení ordinální hodnoty znaku v řetězci na pozici indexu.
 *
 * @param[in,out]   InstructionlistPtr  l           Ukazatel na existující seznam instrukcí
 * @param[in]       SymbolPtr           variable    Symbol obsahující informace o booleovské proměnné, kam bude hodnota uložena
 * @param[in]       SymbolPtr           symbol1     Symbol obsahující informace o proměnné/konstantě, sloužící jako řetězec
 * @param[in]       SymbolPtr           symbol2     Symbol obsahující informace o proměnné/konstantě, sloužící jako index
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_stri2int(InstructionListPtr l, SymbolPtr variable, SymbolPtr symbol1, SymbolPtr symbol2)
{
    if (variable->location == CONSTANT) {
        DEBUG_ERR("generator", "first operand cannot be constant");
        return INTERNAL_ERROR;
    }
    char *s_var;
    Instruction_getsymbol(variable, &s_var);

    char *s_symbol1;
    Instruction_getsymbol(symbol1, &s_symbol1);

    char *s_symbol2;
    Instruction_getsymbol(symbol2, &s_symbol2);

    char *instruction = String_printf("STRI2INT %s %s %s", s_var, s_symbol1, s_symbol2, NULL);
    if (instruction == NULL)
    {
        DEBUG_ERR("generator", "instruction failed to allocate");
        return INTERNAL_ERROR;
    }

    InstructionPtr ins_ptr = InstructionList_insertLast(l, instruction);
    if (ins_ptr == NULL)
    {
        DEBUG_ERR("generator", "instruction pointer failed to allocate");
        return INTERNAL_ERROR;
    }
    return NO_ERROR;
}

/**
 * Zapíše instrukce pro převod celého čísla na znak.
 *
 * @param[in,out]   InstructionlistPtr  l           Ukazatel na existující seznam instrukcí
 * @param[in]       SymbolPtr           variable    Symbol obsahující informace o booleovské proměnné, kam bude jednoznakový řetězec uložen
 * @param[in]       SymbolPtr           symbol     Symbol obsahující informace o převáděné proměnné/konstantě
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_int2char(InstructionListPtr l, SymbolPtr variable, SymbolPtr symbol)
{
    if (variable->location == CONSTANT) {
        DEBUG_ERR("generator", "first operand cannot be constant");
        return INTERNAL_ERROR;
    }
    char *s_var;
    Instruction_getsymbol(variable, &s_var);

    char *s_symbol;
    Instruction_getsymbol(symbol, &s_symbol);

    char *instruction = String_printf("INT2CHAR %s %s", s_var, s_symbol, NULL, NULL);
    if (instruction == NULL)
    {
        DEBUG_ERR("generator", "instruction failed to allocate");
        return INTERNAL_ERROR;
    }

    InstructionPtr ins_ptr = InstructionList_insertLast(l, instruction);
    if (ins_ptr == NULL)
    {
        DEBUG_ERR("generator", "instruction pointer failed to allocate");
        return INTERNAL_ERROR;
    }
    return NO_ERROR;
}

/**
 * Zapíše instrukce pro převod celočíselné hodnoty na desetinnou.
 *
 * @param[in,out]   InstructionlistPtr  l           Ukazatel na existující seznam instrukcí
 * @param[in]       SymbolPtr           variable    Symbol obsahující informace o booleovské proměnné, kam bude hodnota uložena
 * @param[in]       SymbolPtr           symbol     Symbol obsahující informace o převáděné proměnné/konstantě
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_int2float(InstructionListPtr l, SymbolPtr variable, SymbolPtr symbol)
{
    if (variable->location == CONSTANT) {
        DEBUG_ERR("generator", "first operand cannot be constant");
        return INTERNAL_ERROR;
    }
    char *s_var;
    Instruction_getsymbol(variable, &s_var);

    char *s_symbol;
    Instruction_getsymbol(symbol, &s_symbol);

    char *instruction = String_printf("INT2FLOAT %s %s", s_var, s_symbol, NULL, NULL);
    if (instruction == NULL)
    {
        DEBUG_ERR("generator", "instruction failed to allocate");
        return INTERNAL_ERROR;
    }

    InstructionPtr ins_ptr = InstructionList_insertLast(l, instruction);
    if (ins_ptr == NULL)
    {
        DEBUG_ERR("generator", "instruction pointer failed to allocate");
        return INTERNAL_ERROR;
    }
    return NO_ERROR;
}

/**
 * Zapíše instrukce pro převod desetinné hodnoty na celočíselnou.
 *
 * @param[in,out]   InstructionlistPtr  l           Ukazatel na existující seznam instrukcí
 * @param[in]       SymbolPtr           variable    Symbol obsahující informace o booleovské proměnné, kam bude hodnota uložena
 * @param[in]       SymbolPtr           symbol     Symbol obsahující informace o převáděné proměnné/konstantě
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_float2int(InstructionListPtr l, SymbolPtr variable, SymbolPtr symbol)
{
    if (variable->location == CONSTANT) {
        DEBUG_ERR("generator", "first operand cannot be constant");
        return INTERNAL_ERROR;
    }
    char *s_var;
    Instruction_getsymbol(variable, &s_var);

    char *s_symbol;
    Instruction_getsymbol(symbol, &s_symbol);

    char *instruction = String_printf("FLOAT2INT %s %s", s_var, s_symbol, NULL, NULL);
    if (instruction == NULL)
    {
        DEBUG_ERR("generator", "instruction failed to allocate");
        return INTERNAL_ERROR;
    }

    InstructionPtr ins_ptr = InstructionList_insertLast(l, instruction);
    if (ins_ptr == NULL)
    {
        DEBUG_ERR("generator", "instruction pointer failed to allocate");
        return INTERNAL_ERROR;
    }
    return NO_ERROR;
}

/**
 * Zapíše instrukce pro zaokrouhlení na celé číslo (na sudou).
 *
 * @param[in,out]   InstructionlistPtr  l           Ukazatel na existující seznam instrukcí
 * @param[in]       SymbolPtr           variable    Symbol obsahující informace o booleovské proměnné, kam bude hodnota uložena
 * @param[in]       SymbolPtr           symbol     Symbol obsahující informace o zaokrouhlované proměnné/konstantě
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_float2r2eint(InstructionListPtr l, SymbolPtr variable, SymbolPtr symbol)
{
    if (variable->location == CONSTANT) {
        DEBUG_ERR("generator", "first operand cannot be constant");
        return INTERNAL_ERROR;
    }
    char *s_var;
    Instruction_getsymbol(variable, &s_var);

    char *s_symbol;
    Instruction_getsymbol(symbol, &s_symbol);

    char *instruction = String_printf("FLOAT2R2EINT %s %s", s_var, s_symbol, NULL, NULL);
    if (instruction == NULL)
    {
        DEBUG_ERR("generator", "instruction failed to allocate");
        return INTERNAL_ERROR;
    }

    InstructionPtr ins_ptr = InstructionList_insertLast(l, instruction);
    if (ins_ptr == NULL)
    {
        DEBUG_ERR("generator", "instruction pointer failed to allocate");
        return INTERNAL_ERROR;
    }
    return NO_ERROR;
}

/**
 * Zapíše instrukce pro zaokrouhlení na celé číslo (na lichou).
 *
 * @param[in,out]   InstructionlistPtr  l           Ukazatel na existující seznam instrukcí
 * @param[in]       SymbolPtr           variable    Symbol obsahující informace o booleovské proměnné, kam bude hodnota uložena
 * @param[in]       SymbolPtr           symbol     Symbol obsahující informace o zaokrouhlované proměnné/konstantě
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_float2r2oint(InstructionListPtr l, SymbolPtr variable, SymbolPtr symbol)
{
    if (variable->location == CONSTANT) {
        DEBUG_ERR("generator", "first operand cannot be constant");
        return INTERNAL_ERROR;
    }
    char *s_var;
    Instruction_getsymbol(variable, &s_var);

    char *s_symbol;
    Instruction_getsymbol(symbol, &s_symbol);

    char *instruction = String_printf("FLOAT2R2OINT %s %s", s_var, s_symbol, NULL, NULL);
    if (instruction == NULL)
    {
        DEBUG_ERR("generator", "instruction failed to allocate");
        return INTERNAL_ERROR;
    }

    InstructionPtr ins_ptr = InstructionList_insertLast(l, instruction);
    if (ins_ptr == NULL)
    {
        DEBUG_ERR("generator", "instruction pointer failed to allocate");
        return INTERNAL_ERROR;
    }
    return NO_ERROR;
}

//-------------------------------------------------d-d-
//  Instructions: Conversions for stack
//-----------------------------------------------------

/**
 * Zapíše instrukce pro převod celého čísla na znak pro stack.
 *
 * @param[in,out]   InstructionlistPtr  l           Ukazatel na existující seznam instrukcí
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_int2char_stack(InstructionListPtr l)
{
    char *instruction = String_create("INT2CHARS");
    if (instruction == NULL)
    {
        DEBUG_ERR("generator", "instruction failed to allocate");
        return INTERNAL_ERROR;
    }

    InstructionPtr ins_ptr = InstructionList_insertLast(l, instruction);
    if (ins_ptr == NULL)
    {
        DEBUG_ERR("generator", "instruction pointer failed to allocate");
        return INTERNAL_ERROR;
    }
    return NO_ERROR;
}

/**
 * Zapíše instrukce pro uložení ordinální hodnoty znaku v řetězci na pozici indexu pro stack.
 *
 * @param[in,out]   InstructionlistPtr  l           Ukazatel na existující seznam instrukcí
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_stri2int_stack(InstructionListPtr l)
{
    char *instruction = String_create("STRI2INTS");
    if (instruction == NULL)
    {
        DEBUG_ERR("generator", "instruction failed to allocate");
        return INTERNAL_ERROR;
    }

    InstructionPtr ins_ptr = InstructionList_insertLast(l, instruction);
    if (ins_ptr == NULL)
    {
        DEBUG_ERR("generator", "instruction pointer failed to allocate");
        return INTERNAL_ERROR;
    }
    return NO_ERROR;
}

/**
 * Zapíše instrukce pro převod celočíselné hodnoty na desetinnou pro stack.
 *
 * @param[in,out]   InstructionlistPtr  l           Ukazatel na existující seznam instrukcí
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_int2float_stack(InstructionListPtr l)
{
    char *instruction = String_create("INT2FLOATS");
    if (instruction == NULL)
    {
        DEBUG_ERR("generator", "instruction failed to allocate");
        return INTERNAL_ERROR;
    }

    InstructionPtr ins_ptr = InstructionList_insertLast(l, instruction);
    if (ins_ptr == NULL)
    {
        DEBUG_ERR("generator", "instruction pointer failed to allocate");
        return INTERNAL_ERROR;
    }
    return NO_ERROR;
}

/**
 * Zapíše instrukce pro převod desetinné hodnoty na celočíselnou pro stack.
 *
 * @param[in,out]   InstructionlistPtr  l           Ukazatel na existující seznam instrukcí
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_float2int_stack(InstructionListPtr l)
{
    char *instruction = String_create("FLOAT2INTS");
    if (instruction == NULL)
    {
        DEBUG_ERR("generator", "instruction failed to allocate");
        return INTERNAL_ERROR;
    }

    InstructionPtr ins_ptr = InstructionList_insertLast(l, instruction);
    if (ins_ptr == NULL)
    {
        DEBUG_ERR("generator", "instruction pointer failed to allocate");
        return INTERNAL_ERROR;
    }
    return NO_ERROR;
}

/**
 * Zapíše instrukce pro zaokrouhlení na celé číslo (na sudou) pro stack.
 *
 * @param[in,out]   InstructionlistPtr  l           Ukazatel na existující seznam instrukcí
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_float2r2eint_stack(InstructionListPtr l)
{
    char *instruction = String_create("FLOAT2R2EINTS");
    if (instruction == NULL)
    {
        DEBUG_ERR("generator", "instruction failed to allocate");
        return INTERNAL_ERROR;
    }

    InstructionPtr ins_ptr = InstructionList_insertLast(l, instruction);
    if (ins_ptr == NULL)
    {
        DEBUG_ERR("generator", "instruction pointer failed to allocate");
        return INTERNAL_ERROR;
    }
    return NO_ERROR;
}

/**
 * Zapíše instrukce pro zaokrouhlení na celé číslo (na lichou) pro stack.
 *
 * @param[in,out]   InstructionlistPtr  l           Ukazatel na existující seznam instrukcí
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_float2r2oint_stack(InstructionListPtr l)
{
    char *instruction = String_create("FLOAT2R2OINTS");
    if (instruction == NULL)
    {
        DEBUG_ERR("generator", "instruction failed to allocate");
        return INTERNAL_ERROR;
    }

    InstructionPtr ins_ptr = InstructionList_insertLast(l, instruction);
    if (ins_ptr == NULL)
    {
        DEBUG_ERR("generator", "instruction pointer failed to allocate");
        return INTERNAL_ERROR;
    }
    return NO_ERROR;
}


//-------------------------------------------------d-d-
//  Instructions: IO
//-----------------------------------------------------

/**
 * Zapíše instrukce pro načtení hodnoty ze standardního vstupu.
 *
 * @param[in,out]   InstructionlistPtr  l           Ukazatel na existující seznam instrukcí
 * @param[in]       SymbolPtr           symbol     Symbol obsahující informace o proměnné kam bude hodnota uložena
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_input(InstructionListPtr l, SymbolPtr symbol)
{
    if (symbol->location == CONSTANT) {
        DEBUG_ERR("generator", "first operand cannot be constant");
        return INTERNAL_ERROR;
    }
    char *s_symbol;
    Instruction_getsymbol(symbol, &s_symbol);

    char *s_type;
    if (symbol->type == ST_BOOLEAN) {
        s_type = "bool";
    }
    else if (symbol->type == ST_STRING) {
        s_type = "string";
    }
    else if (symbol->type == ST_DOUBLE) {
        s_type = "float";
    }
    else if (symbol->type == ST_INTEGER) {
        s_type = "int";
    }
    else if (symbol->type == ST_NONE) {
        return INTERNAL_ERROR;
    }
    else { // ST_FUNCTION
        return INTERNAL_ERROR;
    }

    char *instruction = String_printf("READ %s %s", s_symbol, s_type, NULL, NULL);
    if (instruction == NULL)
    {
        DEBUG_ERR("generator", "instruction failed to allocate");
        return INTERNAL_ERROR;
    }

    InstructionPtr ins_ptr = InstructionList_insertLast(l, instruction);
    if (ins_ptr == NULL)
    {
        DEBUG_ERR("generator", "instruction pointer failed to allocate");
        return INTERNAL_ERROR;
    }
    return NO_ERROR;
}

/**
 * Zapíše instrukce pro výpis hodnoty na standardní výstup.
 *
 * @param[in,out]   InstructionlistPtr  l           Ukazatel na existující seznam instrukcí
 * @param[in]       SymbolPtr           symbol     Symbol obsahující informace o proměnné/konstantě, jejíž hodnota bude vypsána
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_write(InstructionListPtr l, SymbolPtr symbol)
{
    char *s_symbol;
    Instruction_getsymbol(symbol, &s_symbol);

    char *instruction = String_concat("WRITE", s_symbol, " ");
    if (instruction == NULL)
    {
        DEBUG_ERR("generator", "instruction failed to allocate");
        return INTERNAL_ERROR;
    }

    InstructionPtr ins_ptr = InstructionList_insertLast(l, instruction);
    if (ins_ptr == NULL)
    {
        DEBUG_ERR("generator", "instruction pointer failed to allocate");
        return INTERNAL_ERROR;
    }
    return NO_ERROR;
}

//-------------------------------------------------d-d-
//  Instructions: Strings
//-----------------------------------------------------

/**
 * Zapíše instrukce pro konkatenaci dvou řetězcových operandů (jiné typy nejsou povoleny).
 *
 * @param[in,out]   InstructionlistPtr  l           Ukazatel na existující seznam instrukcí
 * @param[in]       SymbolPtr           variable    Symbol obsahující informace o proměnné, kam bude výsledný řetězec uložen
 * @param[in]       SymbolPtr           symbol1     Symbol obsahující informace o konkatenované proměnné/konstantě
 * @param[in]       SymbolPtr           symbol2     Symbol obsahující informace o konkatenované proměnné/konstantě
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_concat(InstructionListPtr l, SymbolPtr variable, SymbolPtr symbol1, SymbolPtr symbol2)
{
    if (variable->location == CONSTANT) {
        DEBUG_ERR("generator", "first operand cannot be constant");
        return INTERNAL_ERROR;
    }
    if (symbol1->type != ST_STRING) {
        DEBUG_ERR("generator", "second operand has to be string");
        return SEMANTICAL_DATATYPE_ERROR;
    }
    if (symbol2->type != ST_STRING) {
        DEBUG_ERR("generator", "third operand has to be string");
        return SEMANTICAL_DATATYPE_ERROR;
    }
    char *s_var;
    Instruction_getsymbol(variable, &s_var);

    char *s_symbol1;
    Instruction_getsymbol(symbol1, &s_symbol1);

    char *s_symbol2;
    Instruction_getsymbol(symbol2, &s_symbol2);

    char *instruction = String_printf("CONCAT %s %s %s", s_var, s_symbol1, s_symbol2, NULL);
    if (instruction == NULL)
    {
        DEBUG_ERR("generator", "instruction failed to allocate");
        return INTERNAL_ERROR;
    }

    InstructionPtr ins_ptr = InstructionList_insertLast(l, instruction);
    if (ins_ptr == NULL)
    {
        DEBUG_ERR("generator", "instruction pointer failed to allocate");
        return INTERNAL_ERROR;
    }
    return NO_ERROR;
}

/**
 * Zapíše instrukce pro zjištění délky řetězce.
 *
 * @param[in,out]   InstructionlistPtr  l           Ukazatel na existující seznam instrukcí
 * @param[in]       SymbolPtr           variable    Symbol obsahující informace o proměnné, kam bude uložena délka řetězce jako celé číslo
 * @param[in]       SymbolPtr           symbol     Symbol obsahující informace o proměnné/konstantě
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_strlen(InstructionListPtr l, SymbolPtr variable, SymbolPtr symbol)
{
    if (variable->location == CONSTANT) {
        DEBUG_ERR("generator", "first operand cannot be constant");
        return INTERNAL_ERROR;
    }
    char *s_var;
    Instruction_getsymbol(variable, &s_var);

    char *s_symbol;
    Instruction_getsymbol(symbol, &s_symbol);

    char *instruction = String_printf("STRLEN %s %s", s_var, s_symbol, NULL, NULL);
    if (instruction == NULL)
    {
        DEBUG_ERR("generator", "instruction failed to allocate");
        return INTERNAL_ERROR;
    }

    InstructionPtr ins_ptr = InstructionList_insertLast(l, instruction);
    if (ins_ptr == NULL)
    {
        DEBUG_ERR("generator", "instruction pointer failed to allocate");
        return INTERNAL_ERROR;
    }
    return NO_ERROR;
}

/**
 * Zapíše instrukce pro vrácení znaku řetězce.
 *
 * @param[in,out]   InstructionlistPtr  l           Ukazatel na existující seznam instrukcí
 * @param[in]       SymbolPtr           variable    Symbol obsahující informace o proměnné, kam bude výsledný řetězec uložen
 * @param[in]       SymbolPtr           symbol1     Symbol obsahující informace o proměnné/konstantě, ve které se bude vyhledávat
 * @param[in]       SymbolPtr           symbol2     Symbol obsahující informace o proměnné/konstantě, která slouží jako index znaku v řetězci
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_getchar(InstructionListPtr l, SymbolPtr variable, SymbolPtr symbol1, SymbolPtr symbol2)
{
    if (variable->location == CONSTANT) {
        DEBUG_ERR("generator", "first operand cannot be constant");
        return INTERNAL_ERROR;
    }
    char *s_var;
    Instruction_getsymbol(variable, &s_var);

    char *s_symbol1;
    Instruction_getsymbol(symbol1, &s_symbol1);

    char *s_symbol2;
    Instruction_getsymbol(symbol2, &s_symbol2);

    char *instruction = String_printf("GETCHAR %s %s %s", s_var, s_symbol1, s_symbol2, NULL);
    if (instruction == NULL)
    {
        DEBUG_ERR("generator", "instruction failed to allocate");
        return INTERNAL_ERROR;
    }

    InstructionPtr ins_ptr = InstructionList_insertLast(l, instruction);
    if (ins_ptr == NULL)
    {
        DEBUG_ERR("generator", "instruction pointer failed to allocate");
        return INTERNAL_ERROR;
    }
    return NO_ERROR;
}

/**
 * Zapíše instrukce pro změnu znaku řetězce.
 *
 * @param[in,out]   InstructionlistPtr  l           Ukazatel na existující seznam instrukcí
 * @param[in]       SymbolPtr           variable    Symbol obsahující informace o proměnné, ve které se bude znak měnit
 * @param[in]       SymbolPtr           symbol1     Symbol obsahující informace o proměnné/konstantě, která slouží jako index znaku v řetězci
 * @param[in]       SymbolPtr           symbol2     Symbol obsahující informace o proměnné/konstantě obsahující znak na který se bude měnit
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_setchar(InstructionListPtr l, SymbolPtr variable, SymbolPtr symbol1, SymbolPtr symbol2)
{
    if (variable->location == CONSTANT) {
        DEBUG_ERR("generator", "first operand cannot be constant");
        return INTERNAL_ERROR;
    }
    char *s_var;
    Instruction_getsymbol(variable, &s_var);

    char *s_symbol1;
    Instruction_getsymbol(symbol1, &s_symbol1);

    char *s_symbol2;
    Instruction_getsymbol(symbol2, &s_symbol2);

    char *instruction = String_printf("SETCHAR %s %s %s", s_var, s_symbol1, s_symbol2, NULL);
    if (instruction == NULL)
    {
        DEBUG_ERR("generator", "instruction failed to allocate");
        return INTERNAL_ERROR;
    }

    InstructionPtr ins_ptr = InstructionList_insertLast(l, instruction);
    if (ins_ptr == NULL)
    {
        DEBUG_ERR("generator", "instruction pointer failed to allocate");
        return INTERNAL_ERROR;
    }
    return NO_ERROR;
}


//-------------------------------------------------d-d-
//  Instructions: Types
//-----------------------------------------------------

/**
 * Zapíše instrukce pro zjištění typu daného symbolu.
 *
 * @param[in,out]   InstructionlistPtr  l           Ukazatel na existující seznam instrukcí
 * @param[in]       SymbolPtr           variable    Symbol obsahující informace o proměnné, do které se uloží typ symbolu
 * @param[in]       SymbolPtr           symbol     Symbol obsahující informace o proměnné/konstantě
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_type(InstructionListPtr l, SymbolPtr variable, SymbolPtr symbol)
{
    if (variable->location == CONSTANT) {
        DEBUG_ERR("generator", "first operand cannot be constant");
        return INTERNAL_ERROR;
    }
    char *s_var;
    Instruction_getsymbol(variable, &s_var);

    char *s_symbol;
    Instruction_getsymbol(symbol, &s_symbol);

    char *instruction = String_printf("TYPE %s %s", s_var, s_symbol, NULL, NULL);
    if (instruction == NULL)
    {
        DEBUG_ERR("generator", "instruction failed to allocate");
        return INTERNAL_ERROR;
    }

    InstructionPtr ins_ptr = InstructionList_insertLast(l, instruction);
    if (ins_ptr == NULL)
    {
        DEBUG_ERR("generator", "instruction pointer failed to allocate");
        return INTERNAL_ERROR;
    }
    return NO_ERROR;
}

//-------------------------------------------------d-d-
//  Instructions: Jumps
//-----------------------------------------------------

/**
 * Zapíše instrukce pro definici návěstí.
 *
 * @param[in,out]   InstructionlistPtr  l           Ukazatel na existující seznam instrukcí
 * @param[in]       char*           label    Řetězec obsahující název návěstí
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_label(InstructionListPtr l, char *label)
{
    char *instruction = String_concat("LABEL", label, " ");
    if (instruction == NULL)
    {
        DEBUG_ERR("generator", "instruction failed to allocate");
        return INTERNAL_ERROR;
    }

    InstructionPtr ins_ptr = InstructionList_insertLast(l, instruction);
    if (ins_ptr == NULL)
    {
        DEBUG_ERR("generator", "instruction pointer failed to allocate");
        return INTERNAL_ERROR;
    }
    return NO_ERROR;
}

/**
 * Zapíše instrukce pro nepodmíněný skok na návěstí.
 *
 * @param[in,out]   InstructionlistPtr  l           Ukazatel na existující seznam instrukcí
 * @param[in]       char*           label    Řetězec obsahující název návěstí
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_jump(InstructionListPtr l, char *label)
{
    char *instruction = String_concat("JUMP", label, " ");
    if (instruction == NULL)
    {
        DEBUG_ERR("generator", "instruction failed to allocate");
        return INTERNAL_ERROR;
    }

    InstructionPtr ins_ptr = InstructionList_insertLast(l, instruction);
    if (ins_ptr == NULL)
    {
        DEBUG_ERR("generator", "instruction pointer failed to allocate");
        return INTERNAL_ERROR;
    }
    return NO_ERROR;
}

/**
 * Zapíše instrukce pro podmíněný skok na návěstí při rovnosti.
 *
 * @param[in,out]   InstructionlistPtr  l           Ukazatel na existující seznam instrukcí
 * @param[in]       char*           label    Řetězec obsahující název návěstí
 * @param[in]       SymbolPtr           symbol1     Symbol obsahující informace o porovnávané proměnné/konstantě
 * @param[in]       SymbolPtr           symbol2     Symbol obsahující informace o porovnávané proměnné/konstantě
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_jumpifeq(InstructionListPtr l, char *label, SymbolPtr symbol1, SymbolPtr symbol2)
{
    char *s_symbol1;
    Instruction_getsymbol(symbol1, &s_symbol1);

    char *s_symbol2;
    Instruction_getsymbol(symbol2, &s_symbol2);

    char *instruction = String_printf("JUMPIFEQ %s %s %s", label, s_symbol1, s_symbol2, NULL);
    if (instruction == NULL)
    {
        DEBUG_ERR("generator", "instruction failed to allocate");
        return INTERNAL_ERROR;
    }

    InstructionPtr ins_ptr = InstructionList_insertLast(l, instruction);
    if (ins_ptr == NULL)
    {
        DEBUG_ERR("generator", "instruction pointer failed to allocate");
        return INTERNAL_ERROR;
    }
    return NO_ERROR;
}

/**
 * Zapíše instrukce pro podmíněný skok na návěstí při nerovnosti.
 *
 * @param[in,out]   InstructionlistPtr  l           Ukazatel na existující seznam instrukcí
 * @param[in]       char*           label    Řetězec obsahující název návěstí
 * @param[in]       SymbolPtr           symbol1     Symbol obsahující informace o porovnávané proměnné/konstantě
 * @param[in]       SymbolPtr           symbol2     Symbol obsahující informace o porovnávané proměnné/konstantě
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_jumpifneq(InstructionListPtr l, char *label, SymbolPtr symbol1, SymbolPtr symbol2)
{
    char *s_symbol1;
    Instruction_getsymbol(symbol1, &s_symbol1);

    char *s_symbol2;
    Instruction_getsymbol(symbol2, &s_symbol2);

    char *instruction = String_printf("JUMPIFNEQ %s %s %s", label, s_symbol1, s_symbol2, NULL);
    if (instruction == NULL)
    {
        DEBUG_ERR("generator", "instruction failed to allocate");
        return INTERNAL_ERROR;
    }

    InstructionPtr ins_ptr = InstructionList_insertLast(l, instruction);
    if (ins_ptr == NULL)
    {
        DEBUG_ERR("generator", "instruction pointer failed to allocate");
        return INTERNAL_ERROR;
    }
    return NO_ERROR;
}

/**
 * Zapíše instrukce pro podmíněný skok na návěstí při rovnosti na stacku.
 *
 * @param[in,out]   InstructionlistPtr  l           Ukazatel na existující seznam instrukcí
 * @param[in]       char*           label    Řetězec obsahující název návěstí
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_jumpifeq_stack(InstructionListPtr l, char *label)
{
    char *instruction = String_concat("JUMPIFEQS", label, " ");
    if (instruction == NULL)
    {
        DEBUG_ERR("generator", "instruction failed to allocate");
        return INTERNAL_ERROR;
    }

    InstructionPtr ins_ptr = InstructionList_insertLast(l, instruction);
    if (ins_ptr == NULL)
    {
        DEBUG_ERR("generator", "instruction pointer failed to allocate");
        return INTERNAL_ERROR;
    }
    return NO_ERROR;
}

/**
 * Zapíše instrukce pro podmíněný skok na návěstí při nerovnosti na stacku.
 *
 * @param[in,out]   InstructionlistPtr  l           Ukazatel na existující seznam instrukcí
 * @param[in]       char*           label    Řetězec obsahující název návěstí
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_jumpifneq_stack(InstructionListPtr l, char *label)
{
    char *instruction = String_concat("JUMPIFNEQS", label, " ");
    if (instruction == NULL)
    {
        DEBUG_ERR("generator", "instruction failed to allocate");
        return INTERNAL_ERROR;
    }

    InstructionPtr ins_ptr = InstructionList_insertLast(l, instruction);
    if (ins_ptr == NULL)
    {
        DEBUG_ERR("generator", "instruction pointer failed to allocate");
        return INTERNAL_ERROR;
    }
    return NO_ERROR;
}

//-------------------------------------------------d-d-
//  Instructions: Other
//-----------------------------------------------------

/**
 * Zapíše instrukce pro výpis stavu interpretu na stderr.
 *
 * @param[in,out]   InstructionlistPtr  l           Ukazatel na existující seznam instrukcí
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_debug_break(InstructionListPtr l)
{
    char *instruction = String_create("BREAK");
    if (instruction == NULL)
    {
        DEBUG_ERR("generator", "instruction failed to allocate");
        return INTERNAL_ERROR;
    }

    InstructionPtr ins_ptr = InstructionList_insertLast(l, instruction);
    if (ins_ptr == NULL)
    {
        DEBUG_ERR("generator", "instruction pointer failed to allocate");
        return INTERNAL_ERROR;
    }
    return NO_ERROR;
}

/**
 * Zapíše instrukce pro výpis hodnoty na stderr.
 *
 * @param[in,out]   InstructionlistPtr  l           Ukazatel na existující seznam instrukcí
 * @param[in]       SymbolPtr           symbol     Symbol obsahující informace o vypisované proměnné/konstantě
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_debug_print(InstructionListPtr l, SymbolPtr symbol)
{
    char *s_symbol;
    Instruction_getsymbol(symbol, &s_symbol);

    char *instruction = String_concat("DPRINT", s_symbol, " ");
    if (instruction == NULL)
    {
        DEBUG_ERR("generator", "instruction failed to allocate");
        return INTERNAL_ERROR;
    }

    InstructionPtr ins_ptr = InstructionList_insertLast(l, instruction);
    if (ins_ptr == NULL)
    {
        DEBUG_ERR("generator", "instruction pointer failed to allocate");
        return INTERNAL_ERROR;
    }
    return NO_ERROR;
}

/**
 * Funkce pro zápis vlastní instrukce. Jendoduše uloží daný řetězec
 * do seznamu instrukcí.
 *
 * @param[in,out]   InstructionlistPtr  l       Ukazatel na existující seznam instrukcí
 * @param[in]       char                *data   Obsah instrukce
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_custom(InstructionListPtr l, const char *data)
{
    char *instruction = String_create(data);
    if (instruction == NULL)
    {
        DEBUG_ERR("generator", "instruction failed to allocate");
        return INTERNAL_ERROR;
    }

    InstructionPtr ins_ptr = InstructionList_insertLast(l, instruction);
    if (ins_ptr == NULL)
    {
        DEBUG_ERR("generator", "instruction pointer failed to allocate");
        return INTERNAL_ERROR;
    }
    return NO_ERROR;
}

/**
 * Závěrečná funkce volaná při úspěšném ukončování překladače.
 * Vypíše všechny instrukce programu, který byl přeložen na stdout.
 *
 * @param[in,out]   InstructionlistPtr  l       Ukazatel na existující seznam instrukcí
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_outputAll(InstructionListPtr l)
{
    while (l->first != NULL) {
        char *content = InstructionList_getFirstAndDelete(l);
        fprintf(stdout, "%s\n", content);
        //  InstructionList_getFirstAndDelete obsah neuvolňuje!
        String_destroy(&content);
    }
    return NO_ERROR;
}

#endif
