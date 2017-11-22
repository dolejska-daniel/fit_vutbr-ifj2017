/**
 * Tento soubor obsahuje deklarace funkcí využité při implementaci
 * generátoru cílového kódu (generator).
 *
 * @author Daniel Dolejška (xdolej08)
 * @author Lukáš Kulda (xkulda01)
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

//-------------------------------------------------d-d-
//  Instructions: Functions
//-----------------------------------------------------

int Instruction_getsymbol(SymbolPtr symbol, char **output);

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

int Instruction_return(InstructionListPtr l);


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

/**
 * Zapíše instrukce pro definici proměnné pomocí hodnoty ze stacku.
 *
 * @param[in,out]   InstructionlistPtr  l           Ukazatel na existující seznam instrukcí
 * @param[in]       SymbolPtr           variable    Symbol obsahující informace o proměnné, jejíž hodnota bude definována
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_variable_assignFromStack(InstructionListPtr l, SymbolPtr variable);


//-------------------------------------------------d-d-
//  Instructions: Stack
//-----------------------------------------------------

int Instruction_stack_push(InstructionListPtr l, SymbolPtr symbol);

int Instruction_stack_pop(InstructionListPtr l, SymbolPtr symbol);

int Instruction_stack_clear(InstructionListPtr l);


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
int Instruction_add_stack(InstructionListPtr l);

/**
 * Zapíše instrukce pro rozdíl proměnných/konstant na stacku.
 *
 * @param[in,out]   InstructionlistPtr  l   Ukazatel na existující seznam instrukcí
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_sub_stack(InstructionListPtr l);

/**
 * Zapíše instrukce pro dělení proměnných/konstant na stacku.
 *
 * @param[in,out]   InstructionlistPtr  l   Ukazatel na existující seznam instrukcí
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_div_stack(InstructionListPtr l);

/**
 * Zapíše instrukce pro násobení proměnných/konstant na stacku.
 *
 * @param[in,out]   InstructionlistPtr  l   Ukazatel na existující seznam instrukcí
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_mul_stack(InstructionListPtr l);


//-------------------------------------------------d-d-
//  Instructions: Logic
//-----------------------------------------------------

int Instruction_logic_eq(InstructionListPtr l, SymbolPtr variable, SymbolPtr symbol1, SymbolPtr symbol2);

int Instruction_logic_lt(InstructionListPtr l, SymbolPtr variable, SymbolPtr symbol1, SymbolPtr symbol2);

int Instruction_logic_gt(InstructionListPtr l, SymbolPtr variable, SymbolPtr symbol1, SymbolPtr symbol2);

int Instruction_logic_and(InstructionListPtr l, SymbolPtr variable, SymbolPtr symbol1, SymbolPtr symbol2);

int Instruction_logic_or(InstructionListPtr l, SymbolPtr variable, SymbolPtr symbol1, SymbolPtr symbol2);

int Instruction_logic_not(InstructionListPtr l, SymbolPtr variable, SymbolPtr symbol1);


//-------------------------------------------------d-d-
//  Instructions: Logic for stack
//-----------------------------------------------------

int Instruction_logic_eq_stack(InstructionListPtr l);

int Instruction_logic_lt_stack(InstructionListPtr l);

int Instruction_logic_gt_stack(InstructionListPtr l);

int Instruction_logic_and_stack(InstructionListPtr l);

int Instruction_logic_or_stack(InstructionListPtr l);

int Instruction_logic_not_stack(InstructionListPtr l);


//-------------------------------------------------d-d-
//  Instructions: Conversions
//-----------------------------------------------------

int Instruction_stri2int(InstructionListPtr l, SymbolPtr variable, SymbolPtr symbol1, SymbolPtr symbol2);

int Instruction_int2char(InstructionListPtr l, SymbolPtr variable, SymbolPtr symbol);

int Instruction_float2int(InstructionListPtr l, SymbolPtr variable, SymbolPtr symbol);

int Instruction_float2r2eint(InstructionListPtr l, SymbolPtr variable, SymbolPtr symbol);

int Instruction_float2r2oint(InstructionListPtr l, SymbolPtr variable, SymbolPtr symbol);

int Instruction_stri2int_stack(InstructionListPtr l);

int Instruction_int2char_stack(InstructionListPtr l);

int Instruction_float2int_stack(InstructionListPtr l);

int Instruction_float2r2eint_stack(InstructionListPtr l);

int Instruction_float2r2oint_stack(InstructionListPtr l);


//-------------------------------------------------d-d-
//  Instructions: IO
//-----------------------------------------------------

int Instruction_input(InstructionListPtr l, SymbolPtr symbol);

int Instruction_write(InstructionListPtr l, SymbolPtr symbol);


//-------------------------------------------------d-d-
//  Instructions: Strings
//-----------------------------------------------------

int Instruction_concat(InstructionListPtr l, SymbolPtr variable, SymbolPtr symbol1, SymbolPtr symbol2);

int Instruction_strlen(InstructionListPtr l, SymbolPtr variable, SymbolPtr symbol);

int Instruction_getchar(InstructionListPtr l, SymbolPtr variable, SymbolPtr symbol1, SymbolPtr symbol2);

int Instruction_setchar(InstructionListPtr l, SymbolPtr variable, SymbolPtr symbol1, SymbolPtr symbol2);


//-------------------------------------------------d-d-
//  Instructions: Types
//-----------------------------------------------------

int Instruction_type(InstructionListPtr l, SymbolPtr variable, SymbolPtr symbol);


//-------------------------------------------------d-d-
//  Instructions: Jumps
//-----------------------------------------------------

int Instruction_label(InstructionListPtr l, char *label);

int Instruction_jump(InstructionListPtr l, char *label);

int Instruction_jumpifeq(InstructionListPtr l, char *label, SymbolPtr symbol1, SymbolPtr symbol2);

int Instruction_jumpifneq(InstructionListPtr l, char *label, SymbolPtr symbol1, SymbolPtr symbol2);

int Instruction_jumpifeq_stack(InstructionListPtr l, char *label);

int Instruction_jumpifneq_stack(InstructionListPtr l, char *label);


//-------------------------------------------------d-d-
//  Instructions: Other
//-----------------------------------------------------

int Instruction_debug_break(InstructionListPtr l);

int Instruction_debug_print(InstructionListPtr l, SymbolPtr symbol);

/**
 * Funkce pro zápis vlastní instrukce. Jendoduše uloží daný řetězec
 * do seznamu instrukcí.
 *
 * @param[in,out]   InstructionlistPtr  l       Ukazatel na existující seznam instrukcí
 * @param[in]       char                *data   Obsah instrukce
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_custom(InstructionListPtr l, const char *data);

/**
 * Závěrečná funkce volaná při úspěšném ukončování překladače.
 * Vypíše všechny instrukce programu, který byl přeložen na stdout.
 *
 * @param[in,out]   InstructionlistPtr  l       Ukazatel na existující seznam instrukcí
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Instruction_outputAll(InstructionListPtr l);

#endif
