/**
 * Tento soubor obsahuje deklarace funkcí využité při implementaci
 * generátoru cílového kódu (generator).
 *
 * @author Lukáš Kulda (xkulda01)
 * @date 21.11.2017
 * @project IFJcode17Parser
 * @subject Formální jazyky a překladače (IFJ) - FIT VUT v Brně
 */

#include "instruction_list.h"
#include <stdio.h>

#ifndef _generator_c
#define _generator_c

#ifdef DEBUG_INCLUDE
#include "../parser/symtable.h"
#include "../scanner/token.h"
#include "../support/error_codes.h"
#else
#include "symtable.h"
#include "token.h"
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

//==================================================================d=d=
//  DEKLARACE A DEFINICE ENUMERÁTORŮ A STRUKTUR
//======================================================================


//==================================================================d=d=
//  DEKLARACE FUNKCÍ
//======================================================================

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

int Instruction_move(InstructionListPtr l, SymbolPtr variable, SymbolPtr symbol);

int Instruction_createframe(InstructionListPtr l);

int Instruction_pushframe(InstructionListPtr l);

int Instruction_popframe(InstructionListPtr l);

int Instruction_defvar(InstructionListPtr l, SymbolPtr symbol);

int Instruction_call(InstructionListPtr l, SymbolPtr symbol);

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
int Instruction_variable_declare(InstructionListPtr l, SymbolPtr symbol);

/**
 * Zapíše instrukce pro definici proměnné.
 *
 * @param[in,out]   InstructionlistPtr  l           Ukazatel na existující seznam instrukcí
 * @param[in]       SymbolPtr           variable    Symbol obsahující informace o proměnné, jejíž hodnota bude definována
 * @param[in]       SymbolPtr           symbol      Symbol obsahující informace o proměnné/konstantě, sloužící jako hodnota proměnné
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_variable_assign(InstructionListPtr l, SymbolPtr variable, SymbolPtr symbol);


//-------------------------------------------------d-d-
//  Instructions: Stack
//-----------------------------------------------------

int Instruction_stack_push(InstructionListPtr l, SymbolPtr symbol);

int Instruction_stack_pop(InstructionListPtr l, SymbolPtr symbol);

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
int Instruction_add(InstructionListPtr l, SymbolPtr variable, SymbolPtr symbol1, SymbolPtr symbol2);

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
int Instruction_sub(InstructionListPtr l, SymbolPtr variable, SymbolPtr symbol1, SymbolPtr symbol2);

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
int Instruction_div(InstructionListPtr l, SymbolPtr variable, SymbolPtr symbol1, SymbolPtr symbol2);

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
int Instruction_mul(InstructionListPtr l, SymbolPtr variable, SymbolPtr symbol1, SymbolPtr symbol2);


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

int Instruction_logic_eq(InstructionListPtr l, SymbolPtr variable, SymbolPtr symbol1, SymbolPtr symbol2)

int Instruction_logic_lt(InstructionListPtr l, SymbolPtr variable, SymbolPtr symbol1, SymbolPtr symbol2)

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

int Instruction_logic_not(InstructionListPtr l, SymbolPtr variable, SymbolPtr symbol1)
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

int Instruction_input(InstructionListPtr l, SymbolPtr symbol) /// ZKONROLOVAT!!
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

int Instruction_concat(InstructionListPtr l, SymbolPtr variable, SymbolPtr symbol1, SymbolPtr symbol2);
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
int Instruction_jumpifeq_stack(InstructionListPtr l, char *label)
{
    char *instruction = String_concat("JUMPIFEQ", label, " ");
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

int Instruction_jumpifneq_stack(InstructionListPtr l, char *label)
{
    char *instruction = String_concat("JUMPIFNEQ", label, " ");
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
        printf("%s\n", content);
        String_destroy(&content);
    }
    return NO_ERROR;
}

#endif
