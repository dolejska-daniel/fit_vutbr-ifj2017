/**
 * Tento soubor obsahuje deklarace funkcí využité při zpracování
 * postfixových výrazů (generování instrukcí).
 *
 * @author Daniel Dolejška (xdolej08)
 * @date 21.11.2017
 * @project IFJcode17Parser
 * @subject Formální jazyky a překladače (IFJ) - FIT VUT v Brně
 */

#include "postfix_list.h"

#ifndef _postfix2instructions_h
#define _postfix2instructions_h

#ifdef DEBUG_INCLUDE
#include "../generator/instruction_list.h"
#else
#include "instruction_list.h"
#endif

//==================================================================d=d=
//  DEKLARACE A DEFINICE ENUMERÁTORŮ A STRUKTUR
//======================================================================


//==================================================================d=d=
//  DEKLARACE FUNKCÍ
//======================================================================

/**
 * Tato funkce převede postfixovou posloupnost symbolů na instrukce.
 *
 * Před samotným převodem kontroluje výsledný datový typ, pokud
 * se neshoduje s result_dt a nelze zkonvertovat, dojde k chybě.
 *
 * Tato funkce sama zpracovává implicitní typové konverze mezi symboly.
 *
 * @param[in,out]   InstructionListPtr  ilist       Seznam instrukcí
 * @param[in,out]   PostfixListPtr      postfix     Postfixová posloupnost
 * @param[in]       SymbolType          result_dt   Předpokládaný datový typ výsledku výrazu
 *
 * @retval  int Stavový kód zpracování posloupnosti
 */
int postfix2instructions(InstructionListPtr ilist, PostfixListPtr *postfixList, SymbolType result_dt);

int postfix2instructions_preprocess(PostfixListPtr *postfixList);

#endif
