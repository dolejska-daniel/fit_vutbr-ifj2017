/**
 * Tento soubor obsahuje deklarace funkcí využité při konverzi
 * infixových výrazů na postfixové.
 *
 * @author Daniel Dolejška (xdolej08)
 * @date 21.11.2017
 * @project IFJcode17Parser
 * @subject Formální jazyky a překladače (IFJ) - FIT VUT v Brně
 */

#include "token_stack.h"
#include "postfix_list.h"

#ifndef _infix2postfix_h
#define _infix2postfix_h

#ifdef DEBUG_INCLUDE
#else
#endif

//==================================================================d=d=
//  DEKLARACE A DEFINICE ENUMERÁTORŮ A STRUKTUR
//======================================================================


//==================================================================d=d=
//  DEKLARACE FUNKCÍ
//======================================================================

int untilLeftPar(TokenStackPtr s, PostfixListPtr postfixList);

int doOperation(TokenStackPtr s, PostfixListPtr postfixList, TokenPtr token);

int getTokenPriority(TokenPtr token);

int infix2postfix_init(TokenStackPtr *s, PostfixListPtr *postfixList);

int infix2postfix_addOperand(TokenStackPtr *s, PostfixListPtr *postfixList, TokenPtr token, SymbolPtr symbol, void *info);

int infix2postfix_process(TokenStackPtr *s, PostfixListPtr *postfixList);

void infix2postfix_cleanup(TokenStackPtr *s, PostfixListPtr *postfixList);

#endif
