/**
 * Tento soubor obsahuje deklarace funkcí využité při implementaci
 * syntaktického analyzátoru (parseru).
 *
 * @author Daniel Dolejška (xdolej08)
 * @project IFJcode17Parser
 */

#include "symtable.h"
#include "nesting_list.h"

#ifndef _parser_h
#define _parser_h

#ifdef DEBUG_INCLUDE
#include "../scanner/input.h"
#include "../scanner/token.h"
#include "../generator/instruction_list.h"
#include "../support/tokentype_list.h"
#include "../support/token_list.h"
#include "../support/token_stack.h"
#include "../support/postfix_list.h"
#else
#include "input.h"
#include "token.h"
#include "instruction_list.h"
#include "tokentype_list.h"
#include "token_list.h"
#include "token_stack.h"
#include "postfix_list.h"
#endif

//==================================================================d=d=
//  DEKLARACE A DEFINICE ENUMERÁTORŮ A STRUKTUR
//======================================================================

//==================================================================d=d=
//  DEKLARACE FUNKCÍ
//======================================================================

/**
 * Hlavní funkce ovládající překladač.
 *
 * @param[in,out]   InputPtr            input       Ukazatel na strukturu se vstupními daty
 * @param[in,out]   InstructionListPtr  ilist       Ukazatel na strukturu s instrukcemi
 * @param[in,out]   SymbolTablePtr      symtable    Ukazatel na strukturu se symboly
 *
 * @retval	int	Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Parser_ParseInitial(InputPtr input, InstructionListPtr ilist, SymbolTablePtr symtable);

/**
 * Sekundární funkce pro zpracování zanořeného kódu (rekurzivní zanořování).
 *
 * @param[in,out]   InputPtr            input       Ukazatel na strukturu se vstupními daty
 * @param[in,out]   InstructionListPtr  ilist       Ukazatel na strukturu s instrukcemi
 * @param[in,out]   SymbolTablePtr      symtable    Ukazatel na strukturu se symboly
 * @param[in,out]   NestingListPtr      nlist       Ukazatel na strukturu s úrovněmi zanořování
 *
 * @retval	int	Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Parser_ParseNestedCode(InputPtr input, InstructionListPtr ilist, SymbolTablePtr symtable, NestingListPtr nlist);


//-------------------------------------------------d-d-
//  Uživatelské funkce
//-----------------------------------------------------

int Parser_ParseFunctionDeclaration(InputPtr input/*, InstructionListPtr ilist*/, SymbolTablePtr symtable, NestingListPtr nlist);

int Parser_ParseFunctionDefinition(InputPtr input, InstructionListPtr ilist, SymbolTablePtr symtable, NestingListPtr nlist);

int Parser_ParseFunctionCall(InputPtr input, InstructionListPtr ilist, SymbolTablePtr symtable, SymbolPtr func_symbol);

//-------------------------------------------------d-d-
//  Proměnné
//-----------------------------------------------------

int Parser_ParseVariableDeclaration(InputPtr input, InstructionListPtr ilist, SymbolTablePtr symtable, NestingListPtr nlist);

int Parser_ParseVariableDefinition(InputPtr input, InstructionListPtr ilist, SymbolTablePtr symtable, NestingListPtr nlist, SymbolPtr variable);

int Parser_ParseCondition(InputPtr input, InstructionListPtr ilist, SymbolTablePtr symtable, NestingListPtr nlist);

int Parser_ParseSubCondition(InputPtr input, InstructionListPtr ilist, SymbolTablePtr symtable, NestingListPtr nlist);


//-------------------------------------------------d-d-
//  Cykly
//-----------------------------------------------------

int Parser_ParseLoop_Do(InputPtr input, InstructionListPtr ilist, SymbolTablePtr symtable, NestingListPtr nlist);

int Parser_ParseLoop_For(InputPtr input, InstructionListPtr ilist, SymbolTablePtr symtable, NestingListPtr nlist);


//-------------------------------------------------d-d-
//  Statementy
//-----------------------------------------------------

int Parser_ParseStatement_Print(InputPtr input, InstructionListPtr ilist, SymbolTablePtr symtable, NestingListPtr nlist);

int Parser_ParseStatement_Input(InputPtr input, InstructionListPtr ilist, SymbolTablePtr symtable, NestingListPtr nlist);

int Parser_ParseStatement_Continue(/*InputPtr input, */InstructionListPtr ilist/*, SymbolTablePtr symtable*/, NestingListPtr nlist);

int Parser_ParseStatement_Exit(/*InputPtr input, */InstructionListPtr ilist/*, SymbolTablePtr symtable*/, NestingListPtr nlist);

int Parser_ParseStatement_Return(InputPtr input, InstructionListPtr ilist, SymbolTablePtr symtable, NestingListPtr nlist);


//-------------------------------------------------d-d-
//  Vestavěné funkce
//-----------------------------------------------------

int Parser_ParseBuiltinFunction_Asc(InputPtr input, InstructionListPtr ilist, SymbolTablePtr symtable);

int Parser_ParseBuiltinFunction_Chr(InputPtr input, InstructionListPtr ilist, SymbolTablePtr symtable);

int Parser_ParseBuiltinFunction_Length(InputPtr input, InstructionListPtr ilist, SymbolTablePtr symtable);

int Parser_ParseBuiltinFunction_Substr(InputPtr input, InstructionListPtr ilist, SymbolTablePtr symtable);


//-------------------------------------------------d-d-
//  Další
//-----------------------------------------------------

int Parser_ParseScope(InputPtr input, InstructionListPtr ilist, SymbolTablePtr symtable, NestingListPtr nlist);

int Parser_ParseExpression(InputPtr input, InstructionListPtr ilist, SymbolTablePtr symtable, PostfixListPtr *postfix, bool isFunctionParam);

int Parser_ParseSubExpression(InputPtr input, InstructionListPtr ilist, SymbolTablePtr symtable, TokenStackPtr *tokenStack, PostfixListPtr *postfix);


//-------------------------------------------------d-d-
//  Pomocné funkce
//-----------------------------------------------------

int Parser_DeclareBuiltinFunctions(/*InputPtr input, InstructionListPtr ilist, */SymbolTablePtr symtable);

int Parser_DefineBuiltinFunctions(/*InputPtr input, */InstructionListPtr ilist, SymbolTablePtr symtable);

int Parser_getTokens(char *source, InputPtr input, NestingLevelPtr nlevel, TokenTypeListPtr expected_types, TokenListPtr *tokens);

int Parser_getToken(char *source, InputPtr input, NestingLevelPtr nlevel, TokenType expected_type, TokenPtr *token);

int Parser_setError_allocation();

int Parser_setError_statement(char *expected, TokenPtr token, InputPtr input);

int Parser_setError_undefined(TokenPtr token, InputPtr input);

int Parser_setError_alreadyDefined(TokenPtr token, InputPtr input);

int Parser_setError_custom(char *content);

#endif
