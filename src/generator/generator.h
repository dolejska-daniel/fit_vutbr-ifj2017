/**
 * Tento soubor obsahuje deklarace funkcí využité při implementaci
 * generátoru cílového kódu (generator).
 *
 * @author Daniel Dolejška (xdolej08)
 * @project IFJcode17Parser
 */

#include "instruction_stack.h"

#ifndef _generator_h
#define _generator_h

#ifdef DEBUG
#include "../parser/symtable.h"
#include "../scanner/token.h"
#define DEBUG_PRINT(...) do{ fprintf( stderr, __VA_ARGS__ ); } while( 0 )
#else
#include "symtable.h"
#include "token.h"
#define DEBUG_PRINT(...) do{ } while ( 0 )
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
 * @param[in]   SymbolPtr   symbol  Symbol obsahující informace o funkci
 */
void Instruction_function_begin(SymbolPtr symbol);

/**
 * Zapíše instrukce značící konec funkce. Postará se o uklizení rámce
 * po návratu z funkce.
 */
void Instruction_function_end();

/**
 * Zapíše instrukce pro návrat hodnoty funkce.
 * Tato funkce musí být volána před samotným ukončením funkce
 * (Instruction_function_end)!
 *
 * @param[in]   SymbolPtr   symbol  Symbol obsahující informace o proměnné ve funkci jejíž hodnota bude navrácena
 */
void Instruction_function_return(SymbolPtr symbol);

/**
 * Zapíše instrukce pro přečtení/uložení návratové hodnoty funkce.
 * Tato funkce musí být volána po funkci Instruction_function_return
 * a samotným ukončením funkce (Instruction_function_end)!
 *
 * @param[in]   SymbolPtr   symbol  Symbol obsahující informace o proměnné, do které bude návratová hodnota funkce uložena
 */
void Instruction_function_return_read(SymbolPtr symbol);

/**
 * Zapíše instrukce pro volání funkce. Připraví rámce s parametry
 * volané funkce. Parametry jsou určeny předem funkcí
 * Instruction_function_call_addParameter.
 *
 * @param[in]   SymbolPtr   symbol  Symbol obsahující informace o volané funkci
 */
void Instruction_function_call(SymbolPtr symbol);

/**
 * Zapíše instrukce pro přípravu rámce - parametrů před voláním funkce.
 * Za touto funkcí může následovat pouze volání Instruction_function_call
 * či příprava dalších parametrů.
 *
 * @param[in]   SymbolPtr   symbol  Symbol obsahující informace o proměnné/konstantě, sloužící jako parametr volané funkce
 */
void Instruction_function_call_addParameter(SymbolPtr symbol);

/**
 * Zapíše instrukce pro deklaraci proměnné.
 *
 * @param[in]   SymbolPtr   symbol  Symbol obsahující informace o proměnné
 */
void Instruction_variable_declare(SymbolPtr symbol);

/**
 * Zapíše instrukce pro definici proměnné.
 *
 * @param[in]   SymbolPtr   variable    Symbol obsahující informace o proměnné, jejíž hodnota bude definována
 * @param[in]   SymbolPtr   symbol      Symbol obsahující informace o proměnné/konstantě, sloužící jako hodnota proměnné
 */
void Instruction_variable_assign(SymbolPtr variable, SymbolPtr symbol);

/**
 * Zapíše instrukce pro součet proměnných/konstant.
 * V případě nutnosti provádí požadované konverze datových typů.
 *
 * @param[in]   SymbolPtr   variable    Symbol obsahující informace o proměnné, kam bude hodnota výpočtu uložena
 * @param[in]   SymbolPtr   symbol1     Symbol obsahující informace o proměnné/konstantě, sloužící jako hodnota ve výpočtu
 * @param[in]   SymbolPtr   symbol2     Symbol obsahující informace o proměnné/konstantě, sloužící jako hodnota ve výpočtu
 */
void Instruction_math_add(SymbolPtr variable, SymbolPtr symbol1, SymbolPtr symbol2);

/**
 * Zapíše instrukce pro rozdíl proměnných/konstant.
 * V případě nutnosti provádí požadované konverze datových typů.
 *
 * @param[in]   SymbolPtr   variable    Symbol obsahující informace o proměnné, kam bude hodnota výpočtu uložena
 * @param[in]   SymbolPtr   symbol1     Symbol obsahující informace o proměnné/konstantě, sloužící jako hodnota ve výpočtu
 * @param[in]   SymbolPtr   symbol2     Symbol obsahující informace o proměnné/konstantě, sloužící jako hodnota ve výpočtu
 */
void Instruction_math_sub(SymbolPtr variable, SymbolPtr symbol1, SymbolPtr symbol2);

/**
 * Zapíše instrukce pro dělení proměnných/konstant.
 * V případě nutnosti provádí požadované konverze datových typů.
 *
 * @param[in]   SymbolPtr   variable    Symbol obsahující informace o proměnné, kam bude hodnota výpočtu uložena
 * @param[in]   SymbolPtr   symbol1     Symbol obsahující informace o proměnné/konstantě, sloužící jako hodnota ve výpočtu
 * @param[in]   SymbolPtr   symbol2     Symbol obsahující informace o proměnné/konstantě, sloužící jako hodnota ve výpočtu
 */
void Instruction_math_div(SymbolPtr variable, SymbolPtr symbol1, SymbolPtr symbol2);

/**
 * Zapíše instrukce pro násobení proměnných/konstant.
 * V případě nutnosti provádí požadované konverze datových typů.
 *
 * @param[in]   SymbolPtr   variable    Symbol obsahující informace o proměnné, kam bude hodnota výpočtu uložena
 * @param[in]   SymbolPtr   symbol1     Symbol obsahující informace o proměnné/konstantě, sloužící jako hodnota ve výpočtu
 * @param[in]   SymbolPtr   symbol2     Symbol obsahující informace o proměnné/konstantě, sloužící jako hodnota ve výpočtu
 */
void Instruction_math_mul(SymbolPtr variable, SymbolPtr symbol1, SymbolPtr symbol2);

void Instruction_logic_eq(SymbolPtr variable, SymbolPtr symbol1, SymbolPtr symbol2);

void Instruction_logic_lt(SymbolPtr variable, SymbolPtr symbol1, SymbolPtr symbol2);

void Instruction_logic_gt(SymbolPtr variable, SymbolPtr symbol1, SymbolPtr symbol2);

void Instruction_logic_and(SymbolPtr variable, SymbolPtr symbol1, SymbolPtr symbol2);

void Instruction_logic_or(SymbolPtr variable, SymbolPtr symbol1, SymbolPtr symbol2);

void Instruction_logic_not(SymbolPtr variable, SymbolPtr symbol1, SymbolPtr symbol2);

void Instruction_conversion_stri2int();

void Instruction_conversion_int2float(SymbolPtr symbol);

void Instruction_conversion_int2float(SymbolPtr symbol);

void Instruction_conversion_float2int(SymbolPtr symbol);

void Instruction_conversion_float2r2eint(SymbolPtr symbol);

void Instruction_conversion_float2r2oint(SymbolPtr symbol);

void Instruction_input(SymbolPtr symbol);

void Instruction_print(SymbolPtr symbol);

void Instruction_print_addParameter();

void Instruction_if_begin();

void Instruction_if_end();

void Instruction_if_else_begin();

void Instruction_if_else_end();

void Instruction_loop_begin();

void Instruction_loop_end();

void Instruction_custom(const char *data);

/**
 * Závěrečná funkce volaná při úspěšném ukončování překladače.
 * Vypíše všechny instrukce programu, který byl přeložen na stdout.
 */
void Instruction_outputAll();

#endif
