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

int postfix2instructions_mathematical(InstructionListPtr ilist, PostfixListPtr *postfixList);

int postfix2instructions_logical(InstructionListPtr ilist, PostfixListPtr *postfixList);

#endif
