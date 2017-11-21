/**
 * Tento soubor obsahuje deklarace funkcí využité při implementaci
 * generátoru cílového kódu (generator).
 *
 * @author Daniel Dolejška (xdolej08)
 * @date 21.11.2017
 * @project IFJcode17Parser
 * @subject Formální jazyky a překladače (IFJ) - FIT VUT v Brně
 */

#include "instruction_list.h"

#ifndef _generator_h
#define _generator_h

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
 * @param[in]   SymbolPtr   symbol  Symbol obsahující informace o funkci
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_function_begin(SymbolPtr symbol);

/**
 * Zapíše instrukce značící konec funkce. Postará se o uklizení rámce
 * po návratu z funkce.
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_function_end();

/**
 * Zapíše instrukce pro návrat hodnoty funkce.
 * Tato funkce musí být volána před samotným ukončením funkce
 * (Instruction_function_end)!
 *
 * @param[in]   SymbolPtr   symbol  Symbol obsahující informace o proměnné ve funkci jejíž hodnota bude navrácena
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_function_return(SymbolPtr symbol);

/**
 * Zapíše instrukce pro přečtení/uložení návratové hodnoty funkce.
 * Tato funkce musí být volána po funkci Instruction_function_return
 * a samotným ukončením funkce (Instruction_function_end)!
 *
 * @param[in]   SymbolPtr   symbol  Symbol obsahující informace o proměnné, do které bude návratová hodnota funkce uložena
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_function_return_read(SymbolPtr symbol);

/**
 * Zapíše instrukce pro volání funkce. Připraví rámce s parametry
 * volané funkce. Parametry jsou určeny předem funkcí
 * Instruction_function_call_addParameter.
 *
 * @param[in]   SymbolPtr   symbol  Symbol obsahující informace o volané funkci
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_function_call(SymbolPtr symbol);

/**
 * Zapíše instrukce pro přípravu rámce - parametrů před voláním funkce.
 * Za touto funkcí může následovat pouze volání Instruction_function_call
 * či příprava dalších parametrů.
 *
 * @param[in]   SymbolPtr   symbol  Symbol obsahující informace o proměnné/konstantě, sloužící jako parametr volané funkce
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_function_call_addParameter(SymbolPtr symbol);

/**
 * Zapíše instrukce pro deklaraci proměnné.
 *
 * @param[in]   SymbolPtr   symbol  Symbol obsahující informace o proměnné
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_variable_declare(SymbolPtr symbol);

/**
 * Zapíše instrukce pro definici proměnné.
 *
 * @param[in]   SymbolPtr   variable    Symbol obsahující informace o proměnné, jejíž hodnota bude definována
 * @param[in]   SymbolPtr   symbol      Symbol obsahující informace o proměnné/konstantě, sloužící jako hodnota proměnné
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_variable_assign(SymbolPtr variable, SymbolPtr symbol);

/**
 * Zapíše instrukce pro součet proměnných/konstant.
 * V případě nutnosti provádí požadované konverze datových typů.
 *
 * @param[in]   SymbolPtr   variable    Symbol obsahující informace o proměnné, kam bude hodnota výpočtu uložena
 * @param[in]   SymbolPtr   symbol1     Symbol obsahující informace o proměnné/konstantě, sloužící jako hodnota ve výpočtu
 * @param[in]   SymbolPtr   symbol2     Symbol obsahující informace o proměnné/konstantě, sloužící jako hodnota ve výpočtu
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_math_add(SymbolPtr variable, SymbolPtr symbol1, SymbolPtr symbol2);

/**
 * Zapíše instrukce pro rozdíl proměnných/konstant.
 * V případě nutnosti provádí požadované konverze datových typů.
 *
 * @param[in]   SymbolPtr   variable    Symbol obsahující informace o proměnné, kam bude hodnota výpočtu uložena
 * @param[in]   SymbolPtr   symbol1     Symbol obsahující informace o proměnné/konstantě, sloužící jako hodnota ve výpočtu
 * @param[in]   SymbolPtr   symbol2     Symbol obsahující informace o proměnné/konstantě, sloužící jako hodnota ve výpočtu
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_math_sub(SymbolPtr variable, SymbolPtr symbol1, SymbolPtr symbol2);

/**
 * Zapíše instrukce pro dělení proměnných/konstant.
 * V případě nutnosti provádí požadované konverze datových typů.
 *
 * @param[in]   SymbolPtr   variable    Symbol obsahující informace o proměnné, kam bude hodnota výpočtu uložena
 * @param[in]   SymbolPtr   symbol1     Symbol obsahující informace o proměnné/konstantě, sloužící jako hodnota ve výpočtu
 * @param[in]   SymbolPtr   symbol2     Symbol obsahující informace o proměnné/konstantě, sloužící jako hodnota ve výpočtu
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_math_div(SymbolPtr variable, SymbolPtr symbol1, SymbolPtr symbol2);

/**
 * Zapíše instrukce pro násobení proměnných/konstant.
 * V případě nutnosti provádí požadované konverze datových typů.
 *
 * @param[in]   SymbolPtr   variable    Symbol obsahující informace o proměnné, kam bude hodnota výpočtu uložena
 * @param[in]   SymbolPtr   symbol1     Symbol obsahující informace o proměnné/konstantě, sloužící jako hodnota ve výpočtu
 * @param[in]   SymbolPtr   symbol2     Symbol obsahující informace o proměnné/konstantě, sloužící jako hodnota ve výpočtu
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_math_mul(SymbolPtr variable, SymbolPtr symbol1, SymbolPtr symbol2);

int Instruction_logic_eq(SymbolPtr variable, SymbolPtr symbol1, SymbolPtr symbol2);

int Instruction_logic_lt(SymbolPtr variable, SymbolPtr symbol1, SymbolPtr symbol2);

int Instruction_logic_gt(SymbolPtr variable, SymbolPtr symbol1, SymbolPtr symbol2);

int Instruction_logic_and(SymbolPtr variable, SymbolPtr symbol1, SymbolPtr symbol2);

int Instruction_logic_or(SymbolPtr variable, SymbolPtr symbol1, SymbolPtr symbol2);

int Instruction_logic_not(SymbolPtr variable, SymbolPtr symbol1, SymbolPtr symbol2);

int Instruction_conversion_stri2int();

int Instruction_conversion_int2float(SymbolPtr symbol);

int Instruction_conversion_int2float(SymbolPtr symbol);

int Instruction_conversion_float2int(SymbolPtr symbol);

int Instruction_conversion_float2r2eint(SymbolPtr symbol);

int Instruction_conversion_float2r2oint(SymbolPtr symbol);

int Instruction_input(SymbolPtr symbol);

int Instruction_print(SymbolPtr symbol);

int Instruction_print_addParameter();

int Instruction_if_begin();

int Instruction_if_end();

int Instruction_if_else_begin();

int Instruction_if_else_end();

int Instruction_loop_begin();

int Instruction_loop_end();

int Instruction_custom(const char *data);

/**
 * Závěrečná funkce volaná při úspěšném ukončování překladače.
 * Vypíše všechny instrukce programu, který byl přeložen na stdout.
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_outputAll();

#endif
