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

#ifndef _generator_c
#define _generator_c

#ifdef DEBUG_INCLUDE
#include "../parser/symtable.h"
#include "../scanner/token.h"
#else
#include "symtable.h"
#include "token.h"
#endif

//==================================================================d=d=
//  DEKLARACE A DEFINICE ENUMERÁTORŮ A STRUKTUR
//======================================================================


//==================================================================d=d=
//  DEKLARACE FUNKCÍ
//======================================================================

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
int Instruction_function_begin(InstructionListPtr l, SymbolPtr symbol)
{

}

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
 * Zapíše instrukce pro deklaraci proměnné.
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
int Instruction_math_add(InstructionListPtr l, SymbolPtr variable, SymbolPtr symbol1, SymbolPtr symbol2);

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
int Instruction_math_sub(InstructionListPtr l, SymbolPtr variable, SymbolPtr symbol1, SymbolPtr symbol2);

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
int Instruction_math_div(InstructionListPtr l, SymbolPtr variable, SymbolPtr symbol1, SymbolPtr symbol2);

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
int Instruction_math_mul(InstructionListPtr l, SymbolPtr variable, SymbolPtr symbol1, SymbolPtr symbol2);

int Instruction_logic_eq(InstructionListPtr l, SymbolPtr variable, SymbolPtr symbol1, SymbolPtr symbol2);

int Instruction_logic_lt(InstructionListPtr l, SymbolPtr variable, SymbolPtr symbol1, SymbolPtr symbol2);

int Instruction_logic_gt(InstructionListPtr l, SymbolPtr variable, SymbolPtr symbol1, SymbolPtr symbol2);

int Instruction_logic_and(InstructionListPtr l, SymbolPtr variable, SymbolPtr symbol1, SymbolPtr symbol2);

int Instruction_logic_or(InstructionListPtr l, SymbolPtr variable, SymbolPtr symbol1, SymbolPtr symbol2);

int Instruction_logic_not(InstructionListPtr l, SymbolPtr variable, SymbolPtr symbol1, SymbolPtr symbol2);

int Instruction_conversion_stri2int(InstructionListPtr l);

int Instruction_conversion_int2float(InstructionListPtr l, SymbolPtr symbol);

int Instruction_conversion_int2float(InstructionListPtr l, SymbolPtr symbol);

int Instruction_conversion_float2int(InstructionListPtr l, SymbolPtr symbol);

int Instruction_conversion_float2r2eint(InstructionListPtr l, SymbolPtr symbol);

int Instruction_conversion_float2r2oint(InstructionListPtr l, SymbolPtr symbol);

int Instruction_input(InstructionListPtr l, SymbolPtr symbol);

int Instruction_print(InstructionListPtr l, SymbolPtr symbol);

int Instruction_print_addParameter(InstructionListPtr l);

int Instruction_if_begin(InstructionListPtr l);

int Instruction_if_end(InstructionListPtr l);

int Instruction_if_else_begin(InstructionListPtr l);

int Instruction_if_else_end(InstructionListPtr l);

int Instruction_loop_begin(InstructionListPtr l);

int Instruction_loop_end(InstructionListPtr l);

int Instruction_custom(InstructionListPtr l, const char *data)
{
    char *instruction = String_create(data);
    if (instruction == NULL)
    {
        DEBUG_ERR("generator", "instruction failed to allocate");
        return INTERNAL_ERROR;
    }

    InstructionList_insertFirst(l, instruction);
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
