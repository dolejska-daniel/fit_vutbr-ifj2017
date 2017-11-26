/**
 * Tento soubor obsahuje implementaci syntaktického analyzátoru (parser).
 *
 * @author Daniel Dolejška (xdolej08)
 * @date 21.11.2017
 * @project IFJcode17Parser
 * @subject Formální jazyky a překladače (IFJ) - FIT VUT v Brně
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "parser.h"
#include "nesting_list.h"

#ifndef _parser_c
#define _parser_c

#ifdef DEBUG_INCLUDE
#include "../generator/generator.h"
#include "../scanner/scanner.h"
#include "../scanner/token.h"
#include "../support/strings.h"
#include "../support/error_codes.h"
#include "../support/infix2postfix.h"
#include "../support/postfix2instructions.h"
#include "../support/token_list.h"
#include "../support/tokenType_list.h"
#else
#include "generator.h"
#include "scanner.h"
#include "token.h"
#include "strings.h"
#include "error_codes.h"
#include "infix2postfix.h"
#include "postfix2instructions.h"
#include "token_list.h"
#include "tokenType_list.h"
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

#define NO_REQUIRED_TYPE -1
#define LOOP_INTERNAL_NAME "__INTERNAL__LOOP"
#define COND_INTERNAL_NAME "__INTERNAL__COND"

extern int  error_char_index;
extern int  error_instruction_length;
extern char *error_description;

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
int Parser_ParseInitial(InputPtr input, InstructionListPtr ilist, SymbolTablePtr symtable)
{
    DEBUG_LOG("parser", "Starting initial parsing");
    DEBUG_LOG("parser", "Initializing variables");

    //  Inicializace proměnných
    char *source = "parser-init";
    int scanner_result;
    int parser_result;

    int result = NO_ERROR;
    TokenPtr token;

    NestingListPtr nlist = NestingList_create();
    NestingLevelPtr nlevel = NestingList_newLevel(nlist, NESTING_NONE, NULL);

    //  Prvotní instrukce
    DEBUG_LOG(source, "Writing initial instructions");
    Instruction_custom(ilist, ".IFJcode17");
    Instruction_jump(ilist, "main");

    while (true)
    {
        //  Získání tokenu
        scanner_result = Parser_getToken(source, input, nlevel, NO_REQUIRED_TYPE, &token);
        if (scanner_result != NO_ERROR)
        {
            DEBUG_ERR(source, "scanner returned error code");
            return scanner_result;
        }

        switch (token->type)
        {
            //-------------------------------------------------d-d-
            //  Funkce
            //-----------------------------------------------------
            case DECLARE:
            {
                //  Jedná se o posloupnost deklarace funkce

                #ifdef DEBUG_VERBOSE
                DEBUG_LOG(source, "calling Parser_ParseFunctionDeclaration");
                #endif

                parser_result = Parser_ParseFunctionDeclaration(input, ilist, symtable, nlist);
                if (parser_result != NO_ERROR)
                {
                    return parser_result;
                }
                break;
            }

            case FUNCTION:
            {
                //  Jedná se o posloupnost definice funkce
                //  (POUZE POKUD JE FUNKCE JIŽ DEKLAROVÁNA)

                #ifdef DEBUG_VERBOSE
                DEBUG_LOG(source, "calling Parser_ParseFunctionDefinition");
                #endif

                parser_result = Parser_ParseFunctionDefinition(input, ilist, symtable, nlist);
                if (parser_result != NO_ERROR)
                {
                    return parser_result;
                }
                break;
            }

            //-------------------------------------------------d-d-
            //  Ostatní
            //-----------------------------------------------------
            case SCOPE:
            {
                //  Hlavní tělo programu

                #ifdef DEBUG_VERBOSE
                DEBUG_LOG(source, "calling Parser_ParseScope");
                #endif

                parser_result = Parser_ParseScope(input, ilist, symtable, nlist);
                if (parser_result != NO_ERROR)
                {
                    return parser_result;
                }

                #ifdef DEBUG_VERBOSE
                //Instruction_outputAll(ilist);
                #endif

                break;
            }

            case LINE_END:
            {
                //  Konec řádku, prázdný řádek
                continue;
            }

            case FILE_END:
            {
                //  Konec vstupních dat

                #ifdef DEBUG_VERBOSE
                DEBUG_LOG(source, "received FILE_END, ending translation");
                #endif

                return result;
                break;
            }

            default:
            {
                //  Token není nic, co bychom očekáváli
                //  jedná se tedy o syntaktickou chybu
                DEBUG_ERR(source, "this type of token was not expected!");
                Token_debugPrint(token);

                if (token == NULL)
                {
                    Parser_setError_allocation();
                }
                else
                {
                    if (Parser_setError_statement("DECLARE, FUNCTION or SCOPE", token, input) != NO_ERROR)
                    {
                        Token_destroy(&token);
                        return INTERNAL_ERROR;
                    }
                    Token_destroy(&token);
                }
                return SYNTAX_ERROR;
            }
        }
    }
}

int Parser_ParseNestedCode(InputPtr input, InstructionListPtr ilist, SymbolTablePtr symtable, NestingListPtr nlist)
{
    //  Inicializace proměnných
    char *source = "parser-nested";
    int scanner_result;
    int parser_result;

    NestingLevelPtr nlevel = NestingList_active(nlist);
    TokenPtr token;

    while (true)
    {
        //  Získání tokenu
        scanner_result = Parser_getToken(source, input, nlevel, NO_REQUIRED_TYPE, &token);
        if (scanner_result != NO_ERROR)
        {
            DEBUG_ERR(source, "scanner returned error code");
            Token_destroy(&token);
            return scanner_result;
        }

        switch (token->type)
        {
            //-------------------------------------------------d-d-
            //  Funkce
            //-----------------------------------------------------
            case ASC:
            {
                //  Jedná se o posloupnost pro použití funkce ASC

                #ifdef DEBUG_VERBOSE
                DEBUG_LOG(source, "calling Parser_ParseBuiltinFunction_Asc");
                #endif

                parser_result = Parser_ParseBuiltinFunction_Asc(input, ilist, symtable);
                if (parser_result != NO_ERROR)
                {
                    return parser_result;
                }
                break;
            }

            case CHR:
            {
                //  Jedná se o posloupnost pro použití funkce CHR

                #ifdef DEBUG_VERBOSE
                DEBUG_LOG(source, "calling Parser_ParseBuiltinFunction_Chr");
                #endif

                parser_result = Parser_ParseBuiltinFunction_Chr(input, ilist, symtable);
                if (parser_result != NO_ERROR)
                {
                    return parser_result;
                }
                break;
            }

            case LENGTH:
            {
                //  Jedná se o posloupnost pro použití funkce LENGTH

                #ifdef DEBUG_VERBOSE
                DEBUG_LOG(source, "calling Parser_ParseBuiltinFunction_Length");
                #endif

                parser_result = Parser_ParseBuiltinFunction_Length(input, ilist, symtable);
                if (parser_result != NO_ERROR)
                {
                    return parser_result;
                }
                break;
            }

            case SUBSTR:
            {
                //  Jedná se o posloupnost pro použití funkce SUBSTR

                #ifdef DEBUG_VERBOSE
                DEBUG_LOG(source, "calling Parser_ParseBuiltinFunction_Substr");
                #endif

                parser_result = Parser_ParseBuiltinFunction_Substr(input, ilist, symtable);
                if (parser_result != NO_ERROR)
                {
                    return parser_result;
                }
                break;
            }


            //-------------------------------------------------d-d-
            //  Proměnné
            //-----------------------------------------------------
            case DIM:
            {
                //  Jedná se o posloupnost deklarace proměnné

                #ifdef DEBUG_VERBOSE
                DEBUG_LOG(source, "calling Parser_ParseVariableDeclaration");
                #endif

                parser_result = Parser_ParseVariableDeclaration(input, ilist, symtable, nlist);
                if (parser_result != NO_ERROR)
                {
                    return parser_result;
                }
                break;
            }

            case IDENTIFIER:
            {
                //  Jedná se o posloupnost pro příkaz přiřazení
                //  či volání funkce
                //  (POUZE POKUD JE FUNKCE ČI PROMĚNNÁ JIŽ DEKLAROVÁNA)

                SymbolPtr symbol = SymbolTable_getByToken(symtable, token);
                if (symbol == NULL)
                {
                    if (Parser_setError_undefined(token, input) != NO_ERROR)
                        return INTERNAL_ERROR;
                    return SEMANTICAL_DEFINITION_ERROR;
                }

                Token_destroy(&token);
                scanner_result = Parser_getToken(source, input, nlevel, EQ, &token);
                if (scanner_result != NO_ERROR)
                {
                    return scanner_result;
                }

                #ifdef DEBUG_VERBOSE
                DEBUG_LOG(source, "calling Parser_ParseVariableDefinition");
                #endif

                parser_result = Parser_ParseVariableDefinition(input, ilist, symtable, nlist, symbol);
                if (parser_result != NO_ERROR)
                {
                    return parser_result;
                }
                break;
            }


            //-------------------------------------------------d-d-
            //  IFy
            //-----------------------------------------------------
            case IF:
            {
                //  Jedná se o posloupnost pro použití podmínky

                #ifdef DEBUG_VERBOSE
                DEBUG_LOG(source, "calling Parser_ParseCondition");
                #endif

                parser_result = Parser_ParseCondition(input, ilist, symtable, nlist);
                if (parser_result != NO_ERROR)
                {
                    return parser_result;
                }
                break;
            }

            case ELSEIF:
            {
                //  Jedná se o posloupnost pro použití podmínky

                if (NestingList_isNestedIn(nlist, NESTING_CONDITION) == false)
                {
                    NestingList_debugPrint(nlist);
                    DEBUG_ERR(source, "ELSEIF statement not within condition block!");
                    if (Parser_setError_statement(NULL, token, input) != NO_ERROR)
                        return INTERNAL_ERROR;
                    return SYNTAX_ERROR;
                }

                #ifdef DEBUG_VERBOSE
                DEBUG_LOG(source, "received ELSEIF, returning to parent (ungetting token)");
                #endif

                Scanner_UngetToken(input, &token);
                return NO_ERROR;
            }

            case ELSE:
            {
                //  Jedná se o posloupnost pro použití podmínky

                if (NestingList_isNestedIn(nlist, NESTING_CONDITION) == false)
                {
                    NestingList_debugPrint(nlist);
                    DEBUG_ERR(source, "ELSE statement not within condition block!");
                    if (Parser_setError_statement(NULL, token, input) != NO_ERROR)
                        return INTERNAL_ERROR;
                    return SYNTAX_ERROR;
                }

                #ifdef DEBUG_VERBOSE
                DEBUG_LOG(source, "received ELSE, returning to parent (ungetting token)");
                #endif

                Scanner_UngetToken(input, &token);
                return NO_ERROR;
            }


            //-------------------------------------------------d-d-
            //  Cykly
            //-----------------------------------------------------
            case DO:
            {
                //  Jedná se o posloupnost pro použití příkazu DO WHILE

                #ifdef DEBUG_VERBOSE
                DEBUG_LOG(source, "calling Parser_ParseLoop_Do");
                #endif

                parser_result = Parser_ParseLoop_Do(input, ilist, symtable, nlist);
                if (parser_result != NO_ERROR)
                {
                    return parser_result;
                }
                break;
            }

            case FOR:
            {
                //  Jedná se o posloupnost pro použití příkazu DO WHILE
                return SYNTAX_ERROR;

                #ifdef DEBUG_VERBOSE
                DEBUG_LOG(source, "calling Parser_ParseLoop_For");
                #endif

                parser_result = Parser_ParseLoop_For(input, ilist, symtable, nlist);
                if (parser_result != NO_ERROR)
                {
                    return parser_result;
                }
                break;
            }


            //-------------------------------------------------d-d-
            //  Příkazy
            //-----------------------------------------------------
            case PRINT:
            {
                //  Jedná se o posloupnost pro použití příkazu PRINT

                #ifdef DEBUG_VERBOSE
                DEBUG_LOG(source, "calling Parser_ParseStatement_Print");
                #endif

                parser_result = Parser_ParseStatement_Print(input, ilist, symtable, nlist);
                if (parser_result != NO_ERROR)
                {
                    return parser_result;
                }
                break;
            }

            case INPUT:
            {
                //  Jedná se o posloupnost pro použití příkazu INPUT

                #ifdef DEBUG_VERBOSE
                DEBUG_LOG(source, "calling Parser_ParseStatement_Input");
                #endif

                parser_result = Parser_ParseStatement_Input(input, ilist, symtable, nlist);
                if (parser_result != NO_ERROR)
                {
                    return parser_result;
                }
                break;
            }

            case CONTINUE:
            {
                //  Jedná se o posloupnost pro použití příkazu CONTINUE

                if (NestingList_isNestedIn(nlist, NESTING_LOOP) == false)
                {
                    NestingList_debugPrint(nlist);
                    DEBUG_ERR(source, "continue statement not within loop!");
                    if (Parser_setError_statement(NULL, token, input) != NO_ERROR)
                        return INTERNAL_ERROR;
                    return SYNTAX_ERROR;
                }

                #ifdef DEBUG_VERBOSE
                DEBUG_LOG(source, "calling Parser_ParseStatement_Continue");
                #endif

                parser_result = Parser_ParseStatement_Continue(input, ilist, symtable, nlist);
                if (parser_result != NO_ERROR)
                {
                    return parser_result;
                }
                break;
            }

            case EXIT:
            {
                //  Jedná se o posloupnost pro použití příkazu EXIT

                if (NestingList_isNestedIn(nlist, NESTING_LOOP) == false)
                {
                    NestingList_debugPrint(nlist);
                    DEBUG_ERR(source, "exit statement not within loop!");
                    if (Parser_setError_statement(NULL, token, input) != NO_ERROR)
                        return INTERNAL_ERROR;
                    return SYNTAX_ERROR;
                }

                #ifdef DEBUG_VERBOSE
                DEBUG_LOG(source, "calling Parser_ParseStatement_Exit");
                #endif

                parser_result = Parser_ParseStatement_Exit(input, ilist, symtable, nlist);
                if (parser_result != NO_ERROR)
                {
                    return parser_result;
                }
                break;
            }

            case RETURN:
            {
                //  Jedná se o posloupnost pro použití příkazu RETURN

                if (NestingList_isNestedIn(nlist, NESTING_FUNCTION) == false)
                {
                    NestingList_debugPrint(nlist);
                    DEBUG_ERR(source, "return statement not within function!");
                    if (Parser_setError_statement(NULL, token, input) != NO_ERROR)
                        return INTERNAL_ERROR;
                    return SYNTAX_ERROR;
                }

                #ifdef DEBUG_VERBOSE
                DEBUG_LOG(source, "calling Parser_ParseStatement_Return");
                #endif

                parser_result = Parser_ParseStatement_Return(input, ilist, symtable, nlist);
                if (parser_result != NO_ERROR)
                {
                    return parser_result;
                }
                break;
            }

            //-------------------------------------------------d-d-
            //  Ostatní
            //-----------------------------------------------------
            case SCOPE:
            {
                #ifdef DEBUG_VERBOSE
                DEBUG_LOG(source, "calling Parser_ParseScope (NOT NOW! SYNTAX_ERR INTENTIONALLY)");
                #endif

                return SYNTAX_ERROR;

                //  Tělo programu
                parser_result = Parser_ParseScope(input, ilist, symtable, nlist);
                if (parser_result != NO_ERROR)
                {
                    return parser_result;
                }
                break;
            }

            case END:
            {
                //  Ukončení vnoření

                if (nlevel->type == NESTING_LOOP)
                {
                    DEBUG_ERR(source, "tying to return to parent, but still in loop");
                    if (Parser_setError_statement(NULL, token, input) != NO_ERROR)
                        return INTERNAL_ERROR;
                    return SYNTAX_ERROR;
                }

                #ifdef DEBUG_VERBOSE
                DEBUG_LOG(source, "received END, returning to parent (ungetting token)");
                #endif

                Scanner_UngetToken(input, &token);
                return NO_ERROR;
            }

            case LOOP:
            {
                //  Ukončení vnoření cyklu

                if (nlevel->type != NESTING_LOOP)
                {
                    DEBUG_ERR(source, "tying to return to parent from loop, but not in loop");
                    if (Parser_setError_statement(NULL, token, input) != NO_ERROR)
                        return INTERNAL_ERROR;
                    return SYNTAX_ERROR;
                }

                #ifdef DEBUG_VERBOSE
                DEBUG_LOG(source, "received LOOP, returning to parent (ungetting token)");
                #endif

                Scanner_UngetToken(input, &token);
                return NO_ERROR;
            }

            case LINE_END:
            {
                //  Konec řádku, prázdný řádek

                #ifdef DEBUG_VERBOSE
                DEBUG_LOG(source, "received LINE_END, continuing to next token");
                #endif

                continue;
            }

            case FILE_END:
            {
                //  Konec vstupních dat

                #ifdef DEBUG_VERBOSE
                DEBUG_LOG(source, "received LINE_END but nested, cannot end");
                #endif
            }
            default:
            {
                //  Token není nic, co bychom očekáváli
                //  jedná se tedy o syntaktickou chybu

                DEBUG_ERR(source, "this type of token was not expected!");
                Token_debugPrint(token);

                if (Parser_setError_statement(NULL, token, input) != NO_ERROR)
                    return INTERNAL_ERROR;

                Token_destroy(&token);
                return SYNTAX_ERROR;
            }
        }
    }
}


//-------------------------------------------------d-d-
//  Uživatelské funkce
//-----------------------------------------------------

int Parser_ParseFunctionDeclaration(InputPtr input, InstructionListPtr ilist, SymbolTablePtr symtable, NestingListPtr nlist)
{
    //            |->
    //  <DECLARE> | <FUNCTION> <IDENTIFIER> <OPEN_BRACKET> {[<COMMA>] <IDENTIFIER> <AS> <INTEGER|STRING|DOUBLE|BOOLEAN>}* <CLOSE_BRACKET> <AS> <INTEGER|STRING|DOUBLE|BOOLEAN> <LINE_END>
    //

    //-------------------------------------------------d-d-
    //  Inicializace
    //-----------------------------------------------------
    char *source = "parser-func_dec";
    int scanner_result;
    int symbol_result;

    bool firstParam = true;

    TokenPtr  token;
    TokenPtr  func_token;
    TokenPtr  func_type_token;
    TokenPtr  param_name_token;
    TokenPtr  param_type_token;
    SymbolPtr func_symbol;

    SymbolInfo_Function_ParameterListPtr paramList;

    NestingLevelPtr nlevel = NestingList_active(nlist);

    //-------------------------------------------------d-d-
    //  Zpracování tokenů
    //-----------------------------------------------------
    //  <FUNCTION>
    scanner_result = Parser_getToken(source, input, nlevel, FUNCTION, NULL);
    if (scanner_result != NO_ERROR)
    {
        return scanner_result;
    }

    //  <IDENTIFIER>
    scanner_result = Parser_getToken(source, input, nlevel, IDENTIFIER, &func_token);
    if (scanner_result != NO_ERROR)
    {
        return scanner_result;
    }
    func_symbol = SymbolTable_getByToken(symtable, func_token);
    if (func_symbol != NULL)
    {
        if (Parser_setError_alreadyDefined(func_token, input) != NO_ERROR)
        {
            Token_destroy(&func_token);
            return INTERNAL_ERROR;
        }
        Token_destroy(&func_token);
        return SEMANTICAL_DEFINITION_ERROR;
    }

    //  <OPEN_BRACKET>
    scanner_result = Parser_getToken(source, input, nlevel, OPEN_BRACKET, NULL);
    if (scanner_result != NO_ERROR)
    {
        Token_destroy(&func_token);
        return scanner_result;
    }

    DEBUG_LOG(source, "parsing parameters");
    paramList = SymbolInfo_Function_ParameterList_create();
    do
    {
        if (firstParam == false)
        {
            //  <COMMA> or <CLOSE_BRACKET>
            scanner_result = Parser_getToken(source, input, nlevel, NO_REQUIRED_TYPE, &token);
            if (scanner_result != NO_ERROR)
            {
                SymbolInfo_Function_ParameterList_destroy(&paramList);
                Token_destroy(&func_token);
                return scanner_result;
            }
            else if (token->type == CLOSE_BRACKET)
            {
                //  Načtený token je zavírací závorka
                DEBUG_LOG(source, "CLOSE BRACKET received, leaving param process loop");
                break;
            }
            else if (token->type != COMMA)
            {
                //  Načtený token není ani čárka, ani zavírací závorka
                if (Parser_setError_statement("COMMA or CLOSE_BRACKET", token, input) != NO_ERROR)
                {
                    Token_destroy(&token);
                    SymbolInfo_Function_ParameterList_destroy(&paramList);
                    Token_destroy(&func_token);
                    return INTERNAL_ERROR;
                }
                Token_destroy(&token);
                SymbolInfo_Function_ParameterList_destroy(&paramList);
                Token_destroy(&func_token);
                return SYNTAX_ERROR;
            }
        }
        else
        {
            firstParam = false;
        }

        //  <IDENTIFIER>
        scanner_result = Parser_getToken(source, input, nlevel, IDENTIFIER, &param_name_token);
        if (scanner_result != NO_ERROR)
        {
            SymbolInfo_Function_ParameterList_destroy(&paramList);
            Token_destroy(&func_token);
            return scanner_result;
        }
        if (SymbolInfo_Function_ParameterList_parameterExistsWithName(paramList, param_name_token->attr) == true)
        {
            DEBUG_ERR(source, "parameter with given name already exists");
            if (Parser_setError_alreadyDefined(param_name_token, input) != NO_ERROR)
            {
                Token_destroy(&param_name_token);
                return INTERNAL_ERROR;
            }
            Token_destroy(&param_name_token);
            return SEMANTICAL_DEFINITION_ERROR;
        }

        //  <AS>
        scanner_result = Parser_getToken(source, input, nlevel, AS, NULL);
        if (scanner_result != NO_ERROR)
        {
            SymbolInfo_Function_ParameterList_destroy(&paramList);
            Token_destroy(&func_token);
            return scanner_result;
        }

        //  <INTEGER|STRING|DOUBLE|BOOLEAN>
        scanner_result = Parser_getToken(source, input, nlevel, NO_REQUIRED_TYPE, &param_type_token);
        if (scanner_result != NO_ERROR)
        {
            SymbolInfo_Function_ParameterList_destroy(&paramList);
            Token_destroy(&func_token);
            return scanner_result;
        }

        if (Token_isDataType(param_type_token) == false)
        {
            if (Parser_setError_statement("INTEGER, STRING, DOUBLE or BOOLEAN", param_type_token, input))
            {
                SymbolInfo_Function_ParameterList_destroy(&paramList);
                Token_destroy(&func_token);
                Token_destroy(&param_name_token);
                Token_destroy(&param_type_token);
                return INTERNAL_ERROR;
            }
            SymbolInfo_Function_ParameterList_destroy(&paramList);
            Token_destroy(&func_token);
            Token_destroy(&param_name_token);
            Token_destroy(&param_type_token);
            return SYNTAX_ERROR;
        }

        DEBUG_LOG(source, "creating parameter");
        //  Vytvoření struktury
        SymbolInfo_Function_ParameterPtr param = SymbolInfo_Function_Parameter_create(param_name_token->attr, TokenType_Keyword_toSymbolType(param_type_token->type));
        if (param == NULL)
        {
            return INTERNAL_ERROR;
        }
        SymbolInfo_Function_Parameter_debugPrint(param);

        DEBUG_LOG(source, "inserting to param list");
        //  Vložení parametru do seznamu
        symbol_result = SymbolInfo_Function_ParameterList_insert(paramList, param);
        SymbolInfo_Function_ParameterList_debugPrint(paramList);
        if (symbol_result != NO_ERROR)
        {
            DEBUG_ERR(source, "failed to insert parameter to list");
            if (symbol_result == SEMANTICAL_DEFINITION_ERROR)
            {
                if (Parser_setError_alreadyDefined(param_name_token, input) != NO_ERROR)
                    return INTERNAL_ERROR;
                //  Param with this name already exists
            }
            return symbol_result;
        }

        Token_destroy(&param_name_token);
        Token_destroy(&param_type_token);
    }
    while(true);

    DEBUG_LOG(source, "parsing function return type");

    //  <AS>
    scanner_result = Parser_getToken(source, input, nlevel, AS, NULL);
    if (scanner_result != NO_ERROR)
    {
        SymbolInfo_Function_ParameterList_destroy(&paramList);
        Token_destroy(&func_token);
        Token_destroy(&param_name_token);
        Token_destroy(&param_type_token);
        return scanner_result;
    }

    //  <INTEGER|STRING|DOUBLE|BOOLEAN>
    scanner_result = Parser_getToken(source, input, nlevel, NO_REQUIRED_TYPE, &func_type_token);
    if (scanner_result != NO_ERROR)
    {
        SymbolInfo_Function_ParameterList_destroy(&paramList);
        Token_destroy(&func_token);
        Token_destroy(&param_name_token);
        Token_destroy(&param_type_token);
        return scanner_result;
    }

    if (Token_isDataType(func_type_token) == false)
    {
        if (Parser_setError_statement("INTEGER, STRING, DOUBLE or BOOLEAN", func_type_token, input))
        {
            SymbolInfo_Function_ParameterList_destroy(&paramList);
            Token_destroy(&func_token);
            Token_destroy(&param_name_token);
            Token_destroy(&param_type_token);
            Token_destroy(&func_type_token);
            return INTERNAL_ERROR;
        }
        SymbolInfo_Function_ParameterList_destroy(&paramList);
        Token_destroy(&func_token);
        Token_destroy(&param_name_token);
        Token_destroy(&param_type_token);
        Token_destroy(&func_type_token);
        return SYNTAX_ERROR;
    }

    DEBUG_LOG(source, "creating symbol information");
    SymbolInfo_FunctionPtr func_info = SymbolInfo_Function_create(TokenType_Keyword_toSymbolType(func_type_token->type), paramList);

    DEBUG_LOG(source, "inserting to symbol table");
    symbol_result = SymbolTable_insert(symtable, func_token->attr, ST_FUNCTION, CONSTANT, func_info, &func_symbol);

    Symbol_debugPrint(func_symbol);

    if (symbol_result != NO_ERROR)
    {
        if (symbol_result == SEMANTICAL_DEFINITION_ERROR)
        {
            DEBUG_ERR(source, "function with this name is already declared");
            if (Parser_setError_alreadyDefined(func_token, input) != NO_ERROR)
                return INTERNAL_ERROR;
        }
        Token_destroy(&func_token);
        Token_destroy(&param_name_token);
        Token_destroy(&param_type_token);
        Token_destroy(&func_type_token);
        SymbolInfo_Function_ParameterList_destroy(&paramList);
        return symbol_result;
    }

    SymbolTable_debugPrint(symtable);

    Token_destroy(&func_token);
    Token_destroy(&param_name_token);
    Token_destroy(&param_type_token);
    Token_destroy(&func_type_token);

    return NO_ERROR;
}

int Parser_ParseFunctionDefinition(InputPtr input, InstructionListPtr ilist, SymbolTablePtr symtable, NestingListPtr nlist)
{
    //             |->
    //  <FUNCTION> | <IDENTIFIER> <OPEN_BRACKET> {[<COMMA>] <IDENTIFIER> <AS> <INTEGER|STRING|DOUBLE|BOOLEAN>}* <CLOSE_BRACKET> <AS> <INTEGER|STRING|DOUBLE|BOOLEAN> <LINE_END>
    //  {_CODE_}*
    //  <END> <FUNCTION> <LINE_END>
    //

    //-------------------------------------------------d-d-
    //  Inicializace
    //-----------------------------------------------------
    char *source = "parser-func_def";
    int scanner_result;
    int parser_result;
    int symbol_result;
    int instruction_result;

    bool firstParam = true;

    TokenPtr  token;
    TokenPtr  func_token;
    TokenPtr  func_type_token;
    TokenPtr  param_name_token;
    TokenPtr  param_type_token;
    SymbolPtr symbol;
    SymbolPtr func_symbol;

    SymbolInfo_Function_ParameterListPtr paramList;
    SymbolInfo_FunctionPtr func_info;

    NestingLevelPtr nlevel = NestingList_active(nlist);

    //-------------------------------------------------d-d-
    //  Zpracování tokenů
    //-----------------------------------------------------

    //  <IDENTIFIER>
    scanner_result = Parser_getToken(source, input, nlevel, IDENTIFIER, &func_token);
    if (scanner_result != NO_ERROR)
    {
        return scanner_result;
    }
    DEBUG_LOG(source, "loading symbol information by token");

    func_symbol = SymbolTable_getByToken(symtable, func_token);
    if (func_symbol == NULL)
    {
        if (Parser_setError_undefined(func_token, input) != NO_ERROR)
        {
            Token_destroy(&func_token);
            return INTERNAL_ERROR;
        }
        Token_destroy(&func_token);
        return SEMANTICAL_DEFINITION_ERROR;
    }
    func_info = (SymbolInfo_FunctionPtr) func_symbol->value;
    if (func_info->isDefined == true)
    {
        if (Parser_setError_alreadyDefined(func_token, input) != NO_ERROR)
        {
            Token_destroy(&func_token);
            return INTERNAL_ERROR;
        }
        Token_destroy(&func_token);
        return SEMANTICAL_DEFINITION_ERROR;
    }

    //  <OPEN_BRACKET>
    scanner_result = Parser_getToken(source, input, nlevel, OPEN_BRACKET, NULL);
    if (scanner_result != NO_ERROR)
    {
        return scanner_result;
    }

    //  Provedeme změny v tabulce symbolů (je nutné "skrýt" proměnné hlavního programu či jiných funkcí)
    //  první pushframe pro parametry funkce
    SymbolTable_pushFrame(symtable);
    //  druhý pushframe kvůli samotnému volání funkce
    //  (instrukce je vytvořena později)
    SymbolTable_pushFrame(symtable);

    DEBUG_LOG(source, "parsing parameters");
    paramList = func_info->params;
    SymbolInfo_Function_ParameterList_first(paramList);
    do
    {
        //  TODO: musí podporovat i funkce bez parametrů
        if (firstParam == false)
        {
            //  <COMMA> or <CLOSE_BRACKET>
            scanner_result = Parser_getToken(source, input, nlevel, NO_REQUIRED_TYPE, &token);
            if (scanner_result != NO_ERROR)
            {
                SymbolInfo_Function_ParameterList_destroy(&paramList);
                return scanner_result;
            }
            else if (token->type == CLOSE_BRACKET)
            {
                //  Načtený token je zavírací závorka
                DEBUG_LOG(source, "CLOSE BRACKET received, leaving param process loop");
                break;
            }
            else if (token->type != COMMA)
            {
                //  Načtený token není ani čárka, ani zavírací závorka
                if (Parser_setError_statement("COMMA or CLOSE_BRACKET", token, input) != NO_ERROR)
                {
                    Token_destroy(&token);
                    SymbolInfo_Function_ParameterList_destroy(&paramList);
                    return INTERNAL_ERROR;
                }
                Token_destroy(&token);
                SymbolInfo_Function_ParameterList_destroy(&paramList);
                return SYNTAX_ERROR;
            }
        }
        else
        {
            firstParam = false;
        }

        //  <IDENTIFIER>
        scanner_result = Parser_getToken(source, input, nlevel, IDENTIFIER, &param_name_token);
        if (scanner_result != NO_ERROR)
        {
            return scanner_result;
        }

        if (SymbolInfo_Function_ParameterList_get(paramList) == NULL)
        {
            DEBUG_ERR(source, "there are no more parameters defined in function declaration");
            return SEMANTICAL_DATATYPE_ERROR;
        }

        //  <AS>
        scanner_result = Parser_getToken(source, input, nlevel, AS, NULL);
        if (scanner_result != NO_ERROR)
        {
            SymbolInfo_Function_ParameterList_destroy(&paramList);
            return scanner_result;
        }

        //  <INTEGER|STRING|DOUBLE|BOOLEAN>
        scanner_result = Parser_getToken(source, input, nlevel, NO_REQUIRED_TYPE, &param_type_token);
        if (scanner_result != NO_ERROR)
        {
            return scanner_result;
        }

        if (Token_isDataType(param_type_token) == false)
        {
            if (Parser_setError_statement("INTEGER, STRING, DOUBLE or BOOLEAN", param_type_token, input))
            {
                return INTERNAL_ERROR;
            }
            return SYNTAX_ERROR;
        }

        DEBUG_LOG(source, "validating parameter");
        if ((int) SymbolInfo_Function_ParameterList_get(paramList)->dataType != TokenType_Keyword_toSymbolType(param_type_token->type))
        {
            //  Datový typ aktuálního parametru se neshoduje s datovým typem
            //  uvedeným při deklaraci funkce
            DEBUG_ERR(source, "parameter datatypes do not match");

            if (Parser_setError_alreadyDefined(param_name_token, input) != NO_ERROR)
            {
                Token_destroy(&param_name_token);
                Token_destroy(&param_type_token);
                return INTERNAL_ERROR;
            }

            Token_destroy(&param_name_token);
            Token_destroy(&param_type_token);
            return SEMANTICAL_DATATYPE_ERROR;
        }

        //  Vytvoření symbolu pro daný parametr
        //  vytváří se na lokálním rámci, protože pushframe, který "instrukčně" příjde později
        //  jsme pro tabulku symbolů již udělali
        symbol_result = SymbolTable_insert(symtable, param_name_token->attr, TokenType_Keyword_toSymbolType(param_type_token->type), LOCAL_FRAME, param_name_token->attr, &symbol);
        if (symbol_result != NO_ERROR)
        {
            return symbol_result;
        }

        Token_destroy(&param_name_token);
        Token_destroy(&param_type_token);
        SymbolInfo_Function_ParameterList_next(paramList);
    }
    while(true);

    if (SymbolInfo_Function_ParameterList_get(paramList) != NULL)
    {
        DEBUG_ERR(source, "there are more parameters defined in function declaration");
        return SEMANTICAL_DEFINITION_ERROR;
    }

    DEBUG_LOG(source, "parsing function return type");

    //  <AS>
    scanner_result = Parser_getToken(source, input, nlevel, AS, NULL);
    if (scanner_result != NO_ERROR)
    {
        SymbolInfo_Function_ParameterList_destroy(&paramList);
        Token_destroy(&param_name_token);
        Token_destroy(&param_type_token);
        return scanner_result;
    }

    //  <INTEGER|STRING|DOUBLE|BOOLEAN>
    scanner_result = Parser_getToken(source, input, nlevel, NO_REQUIRED_TYPE, &func_type_token);
    if (scanner_result != NO_ERROR)
    {
        SymbolInfo_Function_ParameterList_destroy(&paramList);
        Token_destroy(&param_name_token);
        Token_destroy(&param_type_token);
        return scanner_result;
    }

    if (Token_isDataType(func_type_token) == false)
    {
        if (Parser_setError_statement("INTEGER, STRING, DOUBLE or BOOLEAN", func_type_token, input))
        {
            SymbolInfo_Function_ParameterList_destroy(&paramList);
            Token_destroy(&param_name_token);
            Token_destroy(&param_type_token);
            Token_destroy(&func_type_token);
            return INTERNAL_ERROR;
        }
        SymbolInfo_Function_ParameterList_destroy(&paramList);
        Token_destroy(&param_name_token);
        Token_destroy(&param_type_token);
        Token_destroy(&func_type_token);
        return SYNTAX_ERROR;
    }

    DEBUG_LOG(source, "validating function datatype");
    if ((int) func_info->dataType != TokenType_Keyword_toSymbolType(func_type_token->type))
    {
        DEBUG_ERR(source, "function return datatype does not match");
        SymbolInfo_Function_ParameterList_destroy(&paramList);
        Token_destroy(&param_name_token);
        Token_destroy(&param_type_token);
        Token_destroy(&func_type_token);

        return SEMANTICAL_DATATYPE_ERROR;
    }

    Token_destroy(&param_name_token);
    Token_destroy(&param_type_token);
    Token_destroy(&func_type_token);

    DEBUG_LOG(source, "creating nesting level");
    nlevel = NestingList_newLevel(nlist, NESTING_FUNCTION, func_symbol);
    NestingList_debugPrint(nlist);

    DEBUG_LOG(source, "creating initial instructions");
    Instruction_custom(ilist, "\n# FUNCTION");
    //  Vytvoření návěstí funkce
    instruction_result = Instruction_label(ilist, func_symbol->key);
    if (instruction_result != NO_ERROR)
    {
        return instruction_result;
    }

    //  Vytvoření nového rámce pro dočasné proměnné funkce
    instruction_result = Instruction_createframe(ilist);
    if (instruction_result != NO_ERROR)
    {
        return instruction_result;
    }

    //-------------------------------------------------d-d-
    //  Parsování vnitřního kódu funkce
    //-----------------------------------------------------
    DEBUG_LOG(source, "calling Parser_ParseNestedCode");
    parser_result = Parser_ParseNestedCode(input, ilist, symtable, nlist);
    if (parser_result != NO_ERROR)
    {
        return parser_result;
    }
    DEBUG_LOG(source, "return from Parser_ParseNestedCode");

    //-------------------------------------------------d-d-
    //  Závěrečné tokeny
    //-----------------------------------------------------
    DEBUG_LOG(source, "creating ending instructions");
    //  <END>
    scanner_result = Parser_getToken(source, input, nlevel, END, NULL);
    if (scanner_result != NO_ERROR)
    {
        return scanner_result;
    }

    //  <FUNCTION>
    scanner_result = Parser_getToken(source, input, nlevel, FUNCTION, NULL);
    if (scanner_result != NO_ERROR)
    {
        return scanner_result;
    }

    //  <LINE_END>
    scanner_result = Parser_getToken(source, input, nlevel, LINE_END, NULL);
    if (scanner_result != NO_ERROR)
    {
        return scanner_result;
    }

    //  Zrušení rámce pro dočasné proměnné funkce
    //  (smaže dočasné proměnné funkce)
    instruction_result = Instruction_popframe(ilist);
    if (instruction_result != NO_ERROR)
    {
        return instruction_result;
    }

    //  Instrukce pro návrat
    instruction_result = Instruction_return(ilist);
    if (instruction_result != NO_ERROR)
    {
        return instruction_result;
    }

    //  Provedeme změny i v tabulce symbolů
    //  nejdříve odstraní lokální proměnné funkce vytvořené
    //  v Parser_ParseNested
    SymbolTable_popFrame(symtable);
    //  Provedeme změny i v tabulce symbolů
    //  následně odstraní parametry funkce
    SymbolTable_popFrame(symtable);

    Instruction_custom(ilist, "# END FUNCTION");

    DEBUG_LOG(source, "removing current nesting level");
    NestingList_leaveCurrentLevel(nlist);
    NestingList_debugPrint(nlist);

    return NO_ERROR;
}

int Parser_ParseFunctionCall(InputPtr input, InstructionListPtr ilist, SymbolTablePtr symtable, SymbolPtr func_symbol)
{
    //               |->
    //  <IDENTIFIER> | <OPEN_BRACKET> {[<COMMA] <IDENTIFIER|CONSTANT>}* <CLOSE_BRACKET>
    //

    //-------------------------------------------------d-d-
    //  Inicializace
    //-----------------------------------------------------
    char *source = "parser-func_call";
    int scanner_result;
    int parser_result;
    int instruction_result;
    int symbol_result;

    TokenPtr  token;
    SymbolPtr symbol;

    PostfixListPtr postfix;

    SymbolInfo_FunctionPtr func_info = (SymbolInfo_FunctionPtr) func_symbol->value;
    SymbolInfo_Function_ParameterListPtr paramList = func_info->params;
    SymbolInfo_Function_ParameterPtr func_param = NULL;

    //-------------------------------------------------d-d-
    //  Zpracování tokenů
    //-----------------------------------------------------
    //  <OPEN_BRACKET>
    scanner_result = Parser_getToken(source, input, NULL, OPEN_BRACKET, NULL);
    if (scanner_result != NO_ERROR)
    {
        return scanner_result;
    }

    //  Posunutí aktuálního rámce výše
    instruction_result = Instruction_pushframe(ilist);
    if (instruction_result != NO_ERROR)
    {
        return instruction_result;
    }
    SymbolTable_pushFrame(symtable);

    //  Vytvoření nového rámce pro parametry funkce
    instruction_result = Instruction_createframe(ilist);
    if (instruction_result != NO_ERROR)
    {
        return instruction_result;
    }

    //  PARAMETERS
    DEBUG_LOG(source, "parsing parameters");
    SymbolInfo_Function_ParameterList_first(paramList);
    do
    {
        //  Načtení dalšího parametru z deklarace funkce
        func_param = SymbolInfo_Function_ParameterList_get(paramList);

        //  Vyhodnocení výrazu
        parser_result = Parser_ParseExpression(input, ilist, symtable, &postfix, true);
        if (parser_result != NO_ERROR)
        {
            DEBUG_ERR(source, "failed to parse expression");
            return parser_result;
        }
        DEBUG_LOG(source, "parameter expression successfully converted to postfix expression");

        SymbolType result_dt;
        parser_result = postfix2instructions(input, ilist, symtable, &postfix, func_param->dataType, &result_dt);
        if (parser_result != NO_ERROR)
        {
            DEBUG_ERR(source, "failed convert postfix to instructions");
            return parser_result;
        }
        DEBUG_LOG(source, "parameter expression successfully converted to instructions");

        //  Výsledek výrazu se aktuálně nachází na stacku,
        //  stačí ho jen přiřadit novému parametru
        DEBUG_LOG(source, "creating temporary symbol");

        //  Vytvoření dočasného symbolu pro práci s instrukcemi proměnných
        char *func_param_val = String_create(func_param->name);
        if (func_param_val == NULL)
        {
            return INTERNAL_ERROR;
        }
        symbol = Symbol_create(func_param->name, func_param->dataType, TEMPORARY_FRAME, func_param_val);
        if (symbol == NULL)
        {
            DEBUG_ERR(source, "failed to create temp symbol");
            return INTERNAL_ERROR;
        }
        Symbol_debugPrint(symbol);

        DEBUG_LOG(source, "creating param instructions");
        //  Vytvoření parametru
        instruction_result = Instruction_defvar(ilist, symbol);
        if (instruction_result != NO_ERROR)
        {
            Symbol_destroy(&symbol);
            return instruction_result;
        }

        //  Přiřazení hodnoty parametru
        instruction_result = Instruction_stack_pop(ilist, symbol);
        if (instruction_result != NO_ERROR)
        {
            Symbol_destroy(&symbol);
            return instruction_result;
        }
        Symbol_destroy(&symbol);

        DEBUG_LOG(source, "this parameter is completed, fetching next token");
        //  Načtení dalšího tokenu
        scanner_result = Parser_getToken(source, input, NULL, NO_REQUIRED_TYPE, &token);
        if (scanner_result != NO_ERROR)
        {
            return scanner_result;
        }

        if (token->type == COMMA)
        {
            DEBUG_LOG(source, "received comma, expecting more parameters, continuing");
            //  Měl by následovat další parametr
            Token_destroy(&token);

            //  Posunutí na další parametr v deklaraci funkce
            SymbolInfo_Function_ParameterList_next(paramList);
        }
        else
        {
            DEBUG_LOG(source, "received something not part of this expression, maybe close bracket");
            break;
        }
    }
    while(true);
    DEBUG_LOG(source, "parameter parsing completed");

    //  <CLOSE_BRACKET>
    if (token->type != CLOSE_BRACKET)
    {
        DEBUG_LOG(source, "its not close bracket :( syntax err");
        //  TODO: Error message
        return SYNTAX_ERROR;
    }
    DEBUG_LOG(source, "it was close bracket! :))");

    DEBUG_LOG(source, "creating instructions");

    //  Samotný skok na návěstí funkce a další pushframe
    //  který jsme ovšem v rámci práce s lokální tabulkou symbolů
    //  provedli už dříve
    instruction_result = Instruction_pushframe(ilist);
    if (instruction_result != NO_ERROR)
    {
        return instruction_result;
    }
    instruction_result = Instruction_call(ilist, func_symbol);
    if (instruction_result != NO_ERROR)
    {
        return instruction_result;
    }

    //  Toto je první instrukce po návratu z funkce
    //  smaže rámec s parametry funkce
    instruction_result = Instruction_popframe(ilist);
    if (instruction_result != NO_ERROR)
    {
        return instruction_result;
    }
    SymbolTable_popFrame(symtable);

    DEBUG_LOG(source, "function call successfully parsed");
    return NO_ERROR;
}


//-------------------------------------------------d-d-
//  Proměnné
//-----------------------------------------------------

int Parser_ParseVariableDeclaration(InputPtr input, InstructionListPtr ilist, SymbolTablePtr symtable, NestingListPtr nlist)
{
    //        |->
    //  <DIM> | <IDENTIFIER> <AS> <INTEGER|STRING|DOUBLE|BOOLEAN> <LINE_END>
    //

    //-------------------------------------------------d-d-
    //  Inicializace
    //-----------------------------------------------------
    char *source = "parser-var_dec";
    int scanner_result;
    int instruction_result;
    int symbol_result;

    TokenPtr token;
    NestingLevelPtr nlevel = NestingList_active(nlist);

    char        *var_name;
    SymbolType  var_type;
    SymbolPtr   var;

    //-------------------------------------------------d-d-
    //  Zpracování tokenů
    //-----------------------------------------------------
    //  <IDENTIFIER>
    scanner_result = Parser_getToken(source, input, nlevel, IDENTIFIER, &token);
    if (scanner_result != NO_ERROR)
    {
        Token_destroy(&token);
        return scanner_result;
    }

    var_name = token->attr;
    Token_destroy(&token);

    //  <AS>
    scanner_result = Parser_getToken(source, input, nlevel, AS, NULL);
    if (scanner_result != NO_ERROR)
    {
        return scanner_result;
    }

    //  <INTEGER|STRING|DOUBLE|BOOLEAN>
    scanner_result = Parser_getToken(source, input, nlevel, -1, &token);
    if (scanner_result != NO_ERROR)
    {
        return scanner_result;
    }

    //  Datový typ je v pořádku, zvolíme adekvátní typ symbolu
    switch (token->type)
    {
        case INTEGER:
            var_type = ST_INTEGER;
            break;
        case STRING:
            var_type = ST_STRING;
            break;
        case DOUBLE:
            var_type = ST_DOUBLE;
            break;
        case BOOLEAN:
            var_type = ST_BOOLEAN;
            break;
        default:
            //  Byl očekáván datový typ, který jsme ale nedostali
            DEBUG_ERR(source, "this type of token was not expected!");
            #ifdef DEBUG_VERBOSE
            Token_debugPrint(token);
            #endif

            if (Parser_setError_statement("INTEGER, STRING, DOUBLE or BOOLEAN", token, input) != NO_ERROR)
                return INTERNAL_ERROR;

            Token_destroy(&token);
            return SYNTAX_ERROR;
            break;
    }
    Token_destroy(&token);

    //  <LINE_END>
    scanner_result = Parser_getToken(source, input, nlevel, LINE_END, NULL);
    if (scanner_result != NO_ERROR)
    {
        return scanner_result;
    }

    //-------------------------------------------------d-d-
    //  Vytvoření symbolu
    //-----------------------------------------------------
    symbol_result = SymbolTable_insert(symtable, var_name, var_type, TEMPORARY_FRAME, var_name, &var);
    if (symbol_result != NO_ERROR)
    {
        //  Při vytváření symbolu došlo k chybě
        DEBUG_ERR(source, "failed to create symbol");

        return symbol_result;
    }
    else
    {
        //  Symbol byl úspěšně vytvořen
        DEBUG_LOG(source, "symbol successfully created");
        Token_debugPrint(token);
        SymbolTable_debugPrint(symtable);

        instruction_result = Instruction_variable_declare(ilist, var);
        if (instruction_result != NO_ERROR)
        {
            return instruction_result;
        }
    }

    return NO_ERROR;
}

int Parser_ParseVariableDefinition(InputPtr input, InstructionListPtr ilist, SymbolTablePtr symtable, NestingListPtr nlist, SymbolPtr variable)
{
    //                    |->
    //  <IDENTIFIER> <EQ> | _EXPRESSION_ <LINE_END>
    //

    //-------------------------------------------------d-d-
    //  Inicializace
    //-----------------------------------------------------
    char *source = "parser-var_def";
    int parser_result;
    int scanner_result;
    int instruction_result;

    TokenPtr last_token = NULL;

    PostfixListPtr postfix = NULL;

    NestingLevelPtr nlevel = NestingList_active(nlist);


    //-------------------------------------------------d-d-
    //  Zpracování tokenů
    //-----------------------------------------------------

    //  _EXPRESSION_
    DEBUG_LOG(source, "calling Parser_ParseExpression");
    parser_result = Parser_ParseExpression(input, ilist, symtable, &postfix, false);
    if (parser_result != NO_ERROR)
    {
        DEBUG_ERR(source, "expression failed to be parsed");
        PostfixList_destroy(&postfix);
        return parser_result;
    }

    DEBUG_LOG(source, "postfix conversion completed, calling postfix2instructions");
    SymbolType result_dt;
    parser_result = postfix2instructions(input, ilist, symtable, &postfix, variable->type, &result_dt);
    if (parser_result != NO_ERROR)
    {
        return parser_result;
    }

    DEBUG_LOG(source, "instruction conversion completed, cleaning up");
    PostfixList_destroy(&postfix);

    //  Načtení výsledku do proměnné
    instruction_result = Instruction_stack_pop(ilist, variable);
    if (instruction_result != NO_ERROR)
    {
        return instruction_result;
    }

    //  Vyčištění stacku
    instruction_result = Instruction_stack_clear(ilist);
    if (instruction_result != NO_ERROR)
    {
        return instruction_result;
    }

    //  <LINE_END>
    scanner_result = Parser_getToken(source, input, nlevel, LINE_END, NULL);
    if (scanner_result != NO_ERROR)
    {
        return scanner_result;
    }

    return NO_ERROR;
}


//-------------------------------------------------d-d-
//  IFy
//-----------------------------------------------------

int Parser_ParseCondition(InputPtr input, InstructionListPtr ilist, SymbolTablePtr symtable, NestingListPtr nlist)
{
    //       |->
    //  <IF> | _EXPRESSION_ <THEN> <LINE_END>
    //  {_CODE_ <LINE_END>}*
    //  <ELSE> <LINE_END>
    //  OR
    //  <ELSEIF> _CONDITION_
    //  {_CODE_ <LINE_END>}*
    //  <END> <IF> <LINE_END>
    //

    //-------------------------------------------------d-d-
    //  Inicializace
    //-----------------------------------------------------
    char *source = "parser-cond";
    int scanner_result;
    int parser_result;
    int instruction_result;

    TokenPtr  token;
    SymbolPtr cond_symbol;

    PostfixListPtr postfix;

    NestingLevelPtr nlevel = NestingList_active(nlist);


    //-------------------------------------------------d-d-
    //  Zpracování tokenů
    //-----------------------------------------------------
    Instruction_custom(ilist, "# IF BLOCK");

    //  _EXPRESSION_
    DEBUG_LOG(source, "calling Parser_ParseExpression");
    parser_result = Parser_ParseExpression(input, ilist, symtable, &postfix, false);
    if (parser_result != NO_ERROR)
    {
        DEBUG_ERR(source, "failed to parse expression");
        return parser_result;
    }

    Instruction_custom(ilist, "# IF");

    //  Zpracování podmínky
    DEBUG_LOG(source, "calling postfix2instructions");
    SymbolType result_dt;
    instruction_result = postfix2instructions(input, ilist, symtable, &postfix, ST_BOOLEAN, &result_dt);
    if (instruction_result != NO_ERROR)
    {
        return instruction_result;
    }

    //  <THEN>
    scanner_result = Parser_getToken(source, input, NULL, THEN, NULL);
    if (scanner_result != NO_ERROR)
    {
        return scanner_result;
    }

    //  <LINE_END>
    scanner_result = Parser_getToken(source, input, NULL, LINE_END, NULL);
    if (scanner_result != NO_ERROR)
    {
        return scanner_result;
    }

    DEBUG_LOG(source, "creating condition identifiers and symbols");
    char cond_symbol_id[5];
    snprintf(cond_symbol_id, 5, "%i", symtable->condCount++);

    //  Identifikátor podmínky
    char *cond_symbol_name = String_concat(COND_INTERNAL_NAME, cond_symbol_id, "_");
    if (cond_symbol_name == NULL)
    {
        return INTERNAL_ERROR;
    }
    //  Identifikátor konečného návěstí
    char *cond_end_label = String_concat(cond_symbol_name, "END", "_");
    char *cond_block_end_label = String_concat(cond_symbol_name, "BLOCK_END", "_");

    //  Symbol podmínky
    cond_symbol = Symbol_create(cond_symbol_name, ST_COND, CONSTANT, cond_block_end_label);
    if (cond_symbol == NULL)
    {
        DEBUG_ERR(source, "condition symbol failed to be allocated");
        Parser_setError_allocation();
        return INTERNAL_ERROR;
    }

    DEBUG_LOG(source, "creating new nesting level");
    nlevel = NestingList_newLevel(nlist, NESTING_CONDITION, cond_symbol);
    NestingList_debugPrint(nlist);

    instruction_result = Instruction_custom(ilist, "PUSHS bool@true");
    if (instruction_result != NO_ERROR)
    {
        return instruction_result;
    }

    //  Vyřešení podmínky
    instruction_result = Instruction_jumpifneq_stack(ilist, cond_end_label);
    if (instruction_result != NO_ERROR)
    {
        return instruction_result;
    }

    Instruction_custom(ilist, "# THEN");

    //  _CODE_
    DEBUG_LOG(source, "calling Parser_ParseNestedCode");
    parser_result = Parser_ParseNestedCode(input, ilist, symtable, nlist);
    if (parser_result != NO_ERROR)
    {
        return parser_result;
    }
    DEBUG_LOG(source, "return from Parser_ParseNestedCode");

    //  Po úspěšném opuštění vnořeného kódu skok na konec bloku podmínek
    instruction_result = Instruction_jump(ilist, cond_block_end_label);

    //  Vytvoření konečného návěstí podmínky
    instruction_result = Instruction_label(ilist, cond_end_label);
    if (instruction_result != NO_ERROR)
    {
        String_destroy(&cond_end_label);
        return instruction_result;
    }
    String_destroy(&cond_end_label);

    //  <ELSE|ELSEIF|END>
    scanner_result = Parser_getToken(source, input, NULL, NO_REQUIRED_TYPE, &token);
    if (scanner_result != NO_ERROR)
    {
        return scanner_result;
    }
    //  Blok podmínek pokračuje
    if (token->type == ELSEIF)
    {
        //  Následuje další podmínka
        DEBUG_LOG(source, "calling Parser_ParseSubCondition");

        parser_result = Parser_ParseSubCondition(input, ilist, symtable, nlist);
        if (parser_result != NO_ERROR)
        {
            Token_destroy(&token);
            return parser_result;
        }
    }
    else if (token->type == ELSE)
    {
        //  Následuje konečný blok else
        Instruction_custom(ilist, "# ELSE");

        //  _CODE_
        DEBUG_LOG(source, "calling Parser_ParseNestedCode");

        Token_destroy(&token);
        parser_result = Parser_ParseNestedCode(input, ilist, symtable, nlist);
        if (parser_result != NO_ERROR)
        {
            return parser_result;
        }
    }
    else if (token->type != END)
    {
        //  Token není to co potřebujeme, nastává syntax error
        //  TODO: Error message
        Token_destroy(&token);
        return SYNTAX_ERROR;
    }
    else
    {
        //  Token je typu END, ale máme ho příliš brzy
        Scanner_UngetToken(input, &token);
    }

    //  Následuje úplný konec bloku
    //  <END>
    scanner_result = Parser_getToken(source, input, NULL, END, NULL);
    if (scanner_result != NO_ERROR)
    {
        return scanner_result;
    }

    //  <IF>
    scanner_result = Parser_getToken(source, input, NULL, IF, NULL);
    if (scanner_result != NO_ERROR)
    {
        return scanner_result;
    }

    //  <LINE_END>
    scanner_result = Parser_getToken(source, input, NULL, LINE_END, NULL);
    if (scanner_result != NO_ERROR)
    {
        return scanner_result;
    }
    Instruction_custom(ilist, "# END IF");

    //  Vytvoření konečného návěstí bloku podmínek
    instruction_result = Instruction_label(ilist, cond_block_end_label);
    if (instruction_result != NO_ERROR)
    {
        String_destroy(&cond_block_end_label);
        return instruction_result;
    }
    String_destroy(&cond_block_end_label);

    //  Opuštění tohoto zanoření
    DEBUG_LOG(source, "leaving current nesting level");
    NestingList_leaveCurrentLevel(nlist);
    NestingList_debugPrint(nlist);

    Instruction_custom(ilist, "# END IF BLOCK");

    DEBUG_LOG(source, "successfully received END, terminating condition");
    return NO_ERROR;
}

int Parser_ParseSubCondition(InputPtr input, InstructionListPtr ilist, SymbolTablePtr symtable, NestingListPtr nlist)
{

    //-------------------------------------------------d-d-
    //  Inicializace
    //-----------------------------------------------------
    char *source = "parser-cond_sub";
    int scanner_result;
    int parser_result;
    int instruction_result;

    TokenPtr token;

    PostfixListPtr postfix;

    NestingLevelPtr nlevel = NestingList_active(nlist);


    //-------------------------------------------------d-d-
    //  Zpracování tokenů
    //-----------------------------------------------------
    Instruction_custom(ilist, "# ELSEIF");

    //  _EXPRESSION_
    DEBUG_LOG(source, "calling Parser_ParseExpression");
    parser_result = Parser_ParseExpression(input, ilist, symtable, &postfix, false);
    if (parser_result != NO_ERROR)
    {
        DEBUG_ERR(source, "failed to parse expression");
        return parser_result;
    }

    //  Zpracování podmínky
    SymbolType result_dt;
    instruction_result = postfix2instructions(input, ilist, symtable, &postfix, ST_BOOLEAN, &result_dt);
    if (instruction_result != NO_ERROR)
    {
        return instruction_result;
    }

    //  <THEN>
    scanner_result = Parser_getToken(source, input, NULL, THEN, NULL);
    if (scanner_result != NO_ERROR)
    {
        return scanner_result;
    }

    //  <LINE_END>
    scanner_result = Parser_getToken(source, input, NULL, LINE_END, NULL);
    if (scanner_result != NO_ERROR)
    {
        return scanner_result;
    }

    DEBUG_LOG(source, "creating sub-condition identifiers");
    char cond_symbol_id[5];
    snprintf(cond_symbol_id, 5, "%i", symtable->condCount++);

    //  Identifikátor podmínky
    char *cond_symbol_name = String_concat(COND_INTERNAL_NAME, cond_symbol_id, "_");
    if (cond_symbol_name == NULL)
    {
        return INTERNAL_ERROR;
    }
    //  Identifikátor konečného návěstí
    char *cond_end_label = String_concat(cond_symbol_name, "END", "_");
    char *cond_block_end_label = nlevel->symbol->value;

    instruction_result = Instruction_custom(ilist, "PUSHS bool@true");
    if (instruction_result != NO_ERROR)
    {
        return instruction_result;
    }

    //  Vyřešení podmínky
    instruction_result = Instruction_jumpifneq_stack(ilist, cond_end_label);
    if (instruction_result != NO_ERROR)
    {
        return instruction_result;
    }

    Instruction_custom(ilist, "# THEN");

    //  _CODE_
    DEBUG_LOG(source, "calling Parser_ParseNestedCode");
    parser_result = Parser_ParseNestedCode(input, ilist, symtable, nlist);
    if (parser_result != NO_ERROR)
    {
        return parser_result;
    }

    //  Po úspěšném opuštění vnořeného kódu skok na konec bloku podmínek
    instruction_result = Instruction_jump(ilist, cond_block_end_label);

    //  Vytvoření konečného návěstí podmínky
    instruction_result = Instruction_label(ilist, cond_end_label);
    if (instruction_result != NO_ERROR)
    {
        String_destroy(&cond_end_label);
        return instruction_result;
    }
    String_destroy(&cond_end_label);

    //  <ELSE|ELSEIF|END>
    scanner_result = Parser_getToken(source, input, NULL, NO_REQUIRED_TYPE, &token);
    if (scanner_result != NO_ERROR)
    {
        return scanner_result;
    }
    //  Blok podmínek pokračuje
    if (token->type == ELSEIF)
    {
        //  Následuje další podmínka
        DEBUG_LOG(source, "calling Parser_ParseSubCondition");
        parser_result = Parser_ParseSubCondition(input, ilist, symtable, nlist);
        if (parser_result != NO_ERROR)
        {
            Token_destroy(&token);
            return parser_result;
        }
    }
    else if (token->type == ELSE)
    {
        //  Následuje konečný blok else
        Instruction_custom(ilist, "# ELSE");

        //  _CODE_
        DEBUG_LOG(source, "calling Parser_ParseNestedCode");
        parser_result = Parser_ParseNestedCode(input, ilist, symtable, nlist);
        if (parser_result != NO_ERROR)
        {
            Token_destroy(&token);
            return parser_result;
        }
    }
    else if (token->type != END)
    {
        //  Token není to co potřebujeme, nastává syntax error
        //  TODO: Error message
        Token_destroy(&token);
        return SYNTAX_ERROR;
    }
    else
    {
        //  Následuje úplný konec bloku
        DEBUG_LOG(source, "received END, returning to parent condition");
        Scanner_UngetToken(input, &token);
        return NO_ERROR;
    }

    return SYNTAX_ERROR;
}


//-------------------------------------------------d-d-
//  Cykly
//-----------------------------------------------------

int Parser_ParseLoop_Do(InputPtr input, InstructionListPtr ilist, SymbolTablePtr symtable, NestingListPtr nlist)
{
    //       |->
    //  <DO> | <WHILE> _EXPRESSION_ <LINE_END>
    //  {_CODE_ <LINE_END>}*
    //  <LOOP> <LINE_END>
    //

    //-------------------------------------------------d-d-
    //  Inicializace
    //-----------------------------------------------------
    char *source = "parser-loop_do";
    int scanner_result;
    int parser_result;
    int symbol_result;
    int instruction_result;

    SymbolPtr loop_symbol;

    PostfixListPtr postfix;

    NestingLevelPtr nlevel;


    //-------------------------------------------------d-d-
    //  Zpracování tokenů
    //-----------------------------------------------------
    //  <WHILE>
    scanner_result = Parser_getToken(source, input, NestingList_active(nlist), WHILE, NULL);
    if (scanner_result != NO_ERROR)
    {
        return scanner_result;
    }

    //  _EXPRESSION_
    DEBUG_LOG(source, "calling Parser_ParseExpression");
    parser_result = Parser_ParseExpression(input, ilist, symtable, &postfix, false);
    if (parser_result != NO_ERROR)
    {
        DEBUG_ERR(source, "failed to parse expression");
        return parser_result;
    }

    DEBUG_LOG(source, "creating loop identifiers and symbols");
    char loop_symbol_id[5];
    snprintf(loop_symbol_id, 5, "%i", symtable->loopCount++);

    char *loop_symbol_name = String_concat(LOOP_INTERNAL_NAME, loop_symbol_id, "_");
    if (loop_symbol_name == NULL)
    {
        return INTERNAL_ERROR;
    }
    char *loop_begin_label = String_concat(loop_symbol_name, "BEGIN", "_");
    char *loop_end_label   = String_concat(loop_symbol_name, "END", "_");

    //  Vytvoření informací o cyklu
    SymbolInfo_LoopPtr loop_info = SymbolInfo_Loop_create(loop_begin_label, loop_end_label);
    if (loop_info == NULL)
    {
        Parser_setError_allocation();
        return INTERNAL_ERROR;
    }
    symbol_result = SymbolTable_insert(symtable, loop_symbol_name, ST_LOOP, CONSTANT, loop_info, &loop_symbol);
    if (symbol_result != NO_ERROR)
    {
        Parser_setError_allocation();
        return INTERNAL_ERROR;
    }

    nlevel = NestingList_newLevel(nlist, NESTING_LOOP, loop_symbol);

    //  TODO: Hrátky s CREATEFRAME atd.

    //  Vytvoření počátečního návěstí cyklu
    instruction_result = Instruction_label(ilist, loop_info->begin_label);
    if (instruction_result != NO_ERROR)
    {
        return instruction_result;
    }

    //  Zpracování podmínky cyklení
    DEBUG_LOG(source, "calling postfix2instructions");
    SymbolType result_dt;
    instruction_result = postfix2instructions(input, ilist, symtable, &postfix, ST_BOOLEAN, &result_dt);
    if (parser_result != NO_ERROR)
    {
        DEBUG_ERR(source, "failed to process expression");
        return parser_result;
    }

    //  Podmíněný skok na základě výsledku podmínky
    //  Na stack přidám TRUE
    instruction_result = Instruction_custom(ilist, "PUSHS bool@true");
    if (instruction_result != NO_ERROR)
    {
        return instruction_result;
    }
    //  Následně TRUE na stacku porovnávám s výsledkem podmínky
    //  pokud nejsou stejné, nastáva konec cyklu
    instruction_result = Instruction_jumpifneq_stack(ilist, loop_info->end_label);
    if (instruction_result != NO_ERROR)
    {
        return instruction_result;
    }

    //  <LINE_END>
    scanner_result = Parser_getToken(source, input, nlevel, LINE_END, NULL);
    if (scanner_result != NO_ERROR)
    {
        return scanner_result;
    }

    //  _CODE_
    DEBUG_LOG(source, "calling Parser_ParseNestedCode");
    parser_result = Parser_ParseNestedCode(input, ilist, symtable, nlist);
    if (parser_result != NO_ERROR)
    {
        DEBUG_ERR(source, "failed to parse nested code");
        return parser_result;
    }

    //  <LOOP>
    scanner_result = Parser_getToken(source, input, nlevel, LOOP, NULL);
    if (scanner_result != NO_ERROR)
    {
        return scanner_result;
    }

    //  <LINE_END>
    scanner_result = Parser_getToken(source, input, nlevel, LINE_END, NULL);
    if (scanner_result != NO_ERROR)
    {
        return scanner_result;
    }

    //  Skok zpět na začátek k znovuporovnání podmínky
    instruction_result = Instruction_jump(ilist, loop_info->begin_label);
    if (instruction_result != NO_ERROR)
    {
        return instruction_result;
    }

    //  Vytvoření konečného návěstí cyklu
    instruction_result = Instruction_label(ilist, loop_info->end_label);
    if (instruction_result != NO_ERROR)
    {
        return instruction_result;
    }

    NestingList_leaveCurrentLevel(nlist);

    return NO_ERROR;
}

int Parser_ParseLoop_For(InputPtr input, InstructionListPtr ilist, SymbolTablePtr symtable, NestingListPtr nlist)
{
    //        |->
    //  <FOR> | ??? <LINE_END>
    //  {_CODE_ <LINE_END>}*
    //  <LOOP> <LINE_END>
    //

    //-------------------------------------------------d-d-
    //  Inicializace
    //-----------------------------------------------------
    char *source = "parser-loop_for";
    int scanner_result;
    int parser_result;

    TokenPtr token;


    return INTERNAL_ERROR;
}


//-------------------------------------------------d-d-
//  Statementy
//-----------------------------------------------------

int Parser_ParseStatement_Print(InputPtr input, InstructionListPtr ilist, SymbolTablePtr symtable, NestingListPtr nlist)
{
    //          |->
    //  <PRINT> | {{<IDENTIFIER>|<CONSTANT>} <SEMICOLON>}* <LINE_END>
    //

    //-------------------------------------------------d-d-
    //  Inicializace
    //-----------------------------------------------------
    char *source = "parser-print";
    int scanner_result;
    int instruction_result;

    bool firstLoop = true;

    TokenPtr  token  = NULL;
    SymbolPtr symbol = NULL;

    NestingLevelPtr nlevel = NestingList_active(nlist);


    //-------------------------------------------------d-d-
    //  Zpracování tokenů
    //-----------------------------------------------------
    scanner_result = Parser_getToken(source, input, nlevel, NO_REQUIRED_TYPE, &token);
    if (scanner_result != NO_ERROR)
    {
        return scanner_result;
    }
    do
    {
        //  <IDENTIFIER>|<CONSTANT>
        if (token->type == IDENTIFIER)
        {
            //  Jedná se o identifikátor, podíváme se do tabulky symbolů
            symbol = SymbolTable_getByToken(symtable, token);
            if (symbol == NULL)
            {
                DEBUG_ERR(source, "trying to print undefined variable");
                if (Parser_setError_undefined(token, input) != NO_ERROR)
                    return INTERNAL_ERROR;
                return SEMANTICAL_DEFINITION_ERROR;
            }

            instruction_result = Instruction_write(ilist, symbol);
            if (instruction_result != NO_ERROR)
            {
                DEBUG_ERR(source, "failed to save instruction");
                return instruction_result;
            }
        }
        else if (Token_isConstant(token) == true)
        {
            //  Jedná se o konstantu
            symbol = SymbolTable_getByToken(symtable, token);
            instruction_result = Instruction_write(ilist, symbol);
            if (instruction_result != NO_ERROR)
            {
                DEBUG_ERR(source, "failed to save instruction");
                return instruction_result;
            }
        }
        else
        {
            //  Nejedná se o nic co bychom chtěli
            if (Parser_setError_statement(firstLoop ? "IDENTIFIER or CONSTANT" : "IDENTIFIER, CONSTANT or <EOL>", token, input) != NO_ERROR)
                return INTERNAL_ERROR;

            Token_destroy(&token);
            return SYNTAX_ERROR;
        }
        Token_destroy(&token);

        //  <SEMICOLON>
        scanner_result = Parser_getToken(source, input, nlevel, SEMICOLON, NULL);
        if (scanner_result != NO_ERROR)
        {
            return scanner_result;
        }

        //  <LINE_END> ?
        scanner_result = Parser_getToken(source, input, nlevel, NO_REQUIRED_TYPE, &token);
        if (scanner_result != NO_ERROR)
        {
            return scanner_result;
        }

        if (token->type == LINE_END)
        {
            //  Ukončení posloupnosti
            Token_destroy(&token);
            return NO_ERROR;
        }
    }
    while (token != NULL);

    return INTERNAL_ERROR;
}

int Parser_ParseStatement_Input(InputPtr input, InstructionListPtr ilist, SymbolTablePtr symtable, NestingListPtr nlist)
{
    //          |->
    //  <INPUT> | <IDENTIFIER> <LINE_END>
    //

    //-------------------------------------------------d-d-
    //  Inicializace
    //-----------------------------------------------------
    char *source = "parser-input";
    int scanner_result;
    int instruction_result;

    TokenPtr  token;
    SymbolPtr variable = NULL;

    NestingLevelPtr nlevel = NestingList_active(nlist);


    //-------------------------------------------------d-d-
    //  Zpracování tokenů
    //-----------------------------------------------------
    //  <IDENTIFIER>
    scanner_result = Parser_getToken(source, input, nlevel, IDENTIFIER, &token);
    if (scanner_result != NO_ERROR)
    {
        return scanner_result;
    }

    variable = SymbolTable_getByToken(symtable, token);
    if (variable == NULL)
    {
        if (Parser_setError_undefined(token, input) != NO_ERROR)
            return INTERNAL_ERROR;
        return SEMANTICAL_DEFINITION_ERROR;
    }

    instruction_result = Instruction_input(ilist, variable);
    if (instruction_result != NO_ERROR)
    {
        DEBUG_ERR(source, "failed to save instruction");
        return instruction_result;
    }

    //  <LINE_END>
    scanner_result = Parser_getToken(source, input, nlevel, LINE_END, NULL);
    if (scanner_result != NO_ERROR)
    {
        return scanner_result;
    }

    return NO_ERROR;
}

int Parser_ParseStatement_Continue(InputPtr input, InstructionListPtr ilist, SymbolTablePtr symtable, NestingListPtr nlist)
{
    //-------------------------------------------------d-d-
    //  Inicializace
    //-----------------------------------------------------
    char *source = "parser-continue";
    int instruction_result;

    NestingLevelPtr nlevel = NestingList_isNestedIn(nlist, NESTING_LOOP);

    SymbolPtr loop_symbol = nlevel->symbol;
    SymbolInfo_LoopPtr loop_info = (SymbolInfo_LoopPtr) loop_symbol->value;


    //-------------------------------------------------d-d-
    //  Zpracování
    //-----------------------------------------------------
    instruction_result = Instruction_jump(ilist, loop_info->begin_label);
    if (instruction_result != NO_ERROR)
    {
        return instruction_result;
    }

    return INTERNAL_ERROR;
}

int Parser_ParseStatement_Exit(InputPtr input, InstructionListPtr ilist, SymbolTablePtr symtable, NestingListPtr nlist)
{
    //-------------------------------------------------d-d-
    //  Inicializace
    //-----------------------------------------------------
    char *source = "parser-exit";
    int instruction_result;

    NestingLevelPtr nlevel = NestingList_isNestedIn(nlist, NESTING_LOOP);

    SymbolPtr loop_symbol = nlevel->symbol;
    SymbolInfo_LoopPtr loop_info = (SymbolInfo_LoopPtr) loop_symbol->value;


    //-------------------------------------------------d-d-
    //  Zpracování
    //-----------------------------------------------------
    DEBUG_LOG(source, "creating instruction");
    instruction_result = Instruction_jump(ilist, loop_info->end_label);
    if (instruction_result != NO_ERROR)
    {
        return instruction_result;
    }

    return INTERNAL_ERROR;
}

int Parser_ParseStatement_Return(InputPtr input, InstructionListPtr ilist, SymbolTablePtr symtable, NestingListPtr nlist)
{
    //           |->
    //  <RETURN> | _EXPRESSION_ <LINE_END>
    //

    //-------------------------------------------------d-d-
    //  Inicializace
    //-----------------------------------------------------
    char *source = "parser-return";
    int scanner_result;
    int parser_result;

    PostfixListPtr postfix;

    NestingLevelPtr nlevel = NestingList_isNestedIn(nlist, NESTING_FUNCTION);

    SymbolPtr func_symbol = nlevel->symbol;
    SymbolInfo_FunctionPtr func_info = (SymbolInfo_FunctionPtr) func_symbol->value;


    //-------------------------------------------------d-d-
    //  Zpracování tokenů
    //-----------------------------------------------------
    //  _EXPRESSION_
    DEBUG_LOG(source, "calling Parser_ParseExpression");
    parser_result = Parser_ParseExpression(input, ilist, symtable, &postfix, false);
    if (parser_result != NO_ERROR)
    {
        DEBUG_ERR(source, "failed to parse expression");
        return parser_result;
    }

    SymbolType result_dt;
    parser_result = postfix2instructions(input, ilist, symtable, &postfix, func_info->dataType, &result_dt);
    if (parser_result != NO_ERROR)
    {
        return parser_result;
    }

    //  <LINE_END>
    scanner_result = Parser_getToken(source, input, nlevel, LINE_END, NULL);
    if (scanner_result != NO_ERROR)
    {
        return scanner_result;
    }

    return NO_ERROR;
}


//-------------------------------------------------d-d-
//  Vestavěné funkce
//-----------------------------------------------------

int Parser_ParseBuiltinFunction_Asc(InputPtr input, InstructionListPtr ilist, SymbolTablePtr symtable)
{
    //        |->
    //  <ASC> | <OPEN_BRACKET> <IDENTIFIER:string> <COMMA> <IDENTIFIER:integer> <CLOSE_BRACKET> <LINE_END>
    //

    //-------------------------------------------------d-d-
    //  Inicializace
    //-----------------------------------------------------
    char *source = "parser-asc";
    int scanner_result;
    int parser_result;

    TokenPtr token;


    return INTERNAL_ERROR;
}

int Parser_ParseBuiltinFunction_Chr(InputPtr input, InstructionListPtr ilist, SymbolTablePtr symtable)
{
    //        |->
    //  <CHR> | <OPEN_BRACKET> <IDENTIFIER:integer> <CLOSE_BRACKET> <LINE_END>
    //

    //-------------------------------------------------d-d-
    //  Inicializace
    //-----------------------------------------------------
    char *source = "parser-chr";
    int scanner_result;
    int parser_result;

    TokenPtr token;


    return INTERNAL_ERROR;
}

int Parser_ParseBuiltinFunction_Length(InputPtr input, InstructionListPtr ilist, SymbolTablePtr symtable)
{
    //           |->
    //  <LENGTH> | <OPEN_BRACKET> <IDENTIFIER:string> <CLOSE_BRACKET> <LINE_END>
    //

    //-------------------------------------------------d-d-
    //  Inicializace
    //-----------------------------------------------------
    char *source = "parser-length";
    int scanner_result;
    int parser_result;

    TokenPtr token;


    return INTERNAL_ERROR;
}

int Parser_ParseBuiltinFunction_Substr(InputPtr input, InstructionListPtr ilist, SymbolTablePtr symtable)
{
    //        |->
    //  <ASC> | <OPEN_BRACKET> <IDENTIFIER:string> <COMMA> <IDENTIFIER:integer> <COMMA> <IDENTIFIER:integer> <CLOSE_BRACKET> <LINE_END>
    //

    //-------------------------------------------------d-d-
    //  Inicializace
    //-----------------------------------------------------
    char *source = "parser-substr";
    int scanner_result;
    int instruction_result;

    TokenPtr token;
    TokenPtr last_token = NULL;
    TokenListPtr tokens = TokenList_create();

    SymbolPtr symbol = NULL;

    //-------------------------------------------------d-d-
    //  Zpracování tokenů
    //-----------------------------------------------------
    TokenTypeListPtr expected_types = TokenTypeList_create();
    TokenTypeList_insert(expected_types, OPEN_BRACKET);
    TokenTypeList_insert(expected_types, IDENTIFIER);
    TokenTypeList_insert(expected_types, COMMA);
    TokenTypeList_insert(expected_types, IDENTIFIER);
    TokenTypeList_insert(expected_types, COMMA);
    TokenTypeList_insert(expected_types, IDENTIFIER);
    TokenTypeList_insert(expected_types, CLOSE_BRACKET);
    TokenTypeList_insert(expected_types, LINE_END);

    scanner_result = Parser_getTokens(source, input, NULL, expected_types, &tokens);
    if (scanner_result != NO_ERROR)
    {
        TokenTypeList_destroy(&expected_types);
        TokenList_destroy(&tokens);
        return scanner_result;
    }

    //-------------------------------------------------d-d-
    //  Zpracování prvního parametru funkce
    //-----------------------------------------------------
    //  Načte první identifikátor v definici
    token = TokenList_findFirstToken(tokens, IDENTIFIER);
    if (token == NULL)
    {
        return INTERNAL_ERROR;
    }

    //  Načte odpovídající symbol na základě tokenu
    symbol = SymbolTable_getByToken(symtable, token);
    if (symbol == NULL)
    {
        //  Použití nedeklarované proměnné
        return SEMANTICAL_DEFINITION_ERROR;
    }
    else if (symbol->type != ST_STRING)
    {
        //  Datové typy symbolu a tokenu se neshodují
        return SEMANTICAL_DATATYPE_ERROR;
    }
    instruction_result = Instruction_stack_push(ilist, symbol);
    if (instruction_result != NO_ERROR)
    {
        return instruction_result;
    }

    //-------------------------------------------------d-d-
    //  Zpracování druhého parametru funkce
    //-----------------------------------------------------
    //  Načte první identifikátor v definici
    token = TokenList_findFirstToken(tokens, IDENTIFIER);
    if (token == NULL)
    {
        return INTERNAL_ERROR;
    }

    //  Načte odpovídající symbol na základě tokenu
    symbol = SymbolTable_getByToken(symtable, token);
    if (symbol == NULL)
    {
        //  Použití nedeklarované proměnné
        return SEMANTICAL_DEFINITION_ERROR;
    }
    else if (symbol->type != ST_INTEGER)
    {
        //  Datové typy symbolu a tokenu se neshodují
        return SEMANTICAL_DATATYPE_ERROR;
    }
    instruction_result = Instruction_stack_push(ilist, symbol);
    if (instruction_result != NO_ERROR)
    {
        return instruction_result;
    }

    //-------------------------------------------------d-d-
    //  Zpracování třetího parametru funkce
    //-----------------------------------------------------
    //  Načte první identifikátor v definici
    token = TokenList_findFirstToken(tokens, IDENTIFIER);
    if (token == NULL)
    {
        return INTERNAL_ERROR;
    }

    //  Načte odpovídající symbol na základě tokenu
    symbol = SymbolTable_getByToken(symtable, token);
    if (symbol == NULL)
    {
        //  Použití nedeklarované proměnné
        return SEMANTICAL_DEFINITION_ERROR;
    }
    else if (symbol->type != ST_INTEGER)
    {
        //  Datové typy symbolu a tokenu se neshodují
        return SEMANTICAL_DATATYPE_ERROR;
    }
    instruction_result = Instruction_stack_push(ilist, symbol);
    if (instruction_result != NO_ERROR)
    {
        return instruction_result;
    }

    //  TODO: INSTRUCTIONS

    TokenTypeList_destroy(&expected_types);
    TokenList_destroy(&tokens);
    if (last_token != NULL)
        Token_destroy(&last_token);

    return NO_ERROR;
}


//-------------------------------------------------d-d-
//  Další
//-----------------------------------------------------

int Parser_ParseScope(InputPtr input, InstructionListPtr ilist, SymbolTablePtr symtable, NestingListPtr nlist)
{
    //          |->
    //  <SCOPE> | <LINE_END>
    //  {_CODE_}*
    //  <END> <SCOPE> <LINE_END|FILE_END>
    //

    //-------------------------------------------------d-d-
    //  Inicializace
    //-----------------------------------------------------
    char *source = "parser-scope";
    int scanner_result;
    int parser_result;

    TokenPtr token;

    DEBUG_LOG(source, "creating new nesting level");
    NestingLevelPtr nlevel = NestingList_newLevel(nlist, NESTING_SCOPE, NULL);
    NestingList_debugPrint(nlist);

    //-------------------------------------------------d-d-
    //  Úvodní tokeny
    //-----------------------------------------------------
    scanner_result = Parser_getToken(source, input, nlevel, LINE_END, NULL);
    if (scanner_result != NO_ERROR)
    {
        return scanner_result;
    }

    //-------------------------------------------------d-d-
    //  Zanořené instrukce
    //-----------------------------------------------------

    //  TODO: Main symbol
    Instruction_custom(ilist, "\n# MAIN");
    Instruction_label(ilist, "main");
    Instruction_createframe(ilist);

    DEBUG_LOG(source, "calling Parser_ParseNestedCode");
    parser_result = Parser_ParseNestedCode(input, ilist, symtable, nlist);
    if (parser_result != NO_ERROR)
    {
        return parser_result;
    }

    //-------------------------------------------------d-d-
    //  Závěrečné tokeny
    //-----------------------------------------------------
    DEBUG_LOG(source, "parsing closing tokens");

    //  <END>
    scanner_result = Parser_getToken(source, input, nlevel, END, NULL);
    if (scanner_result != NO_ERROR)
    {
        return scanner_result;
    }

    //  <SCOPE>
    scanner_result = Parser_getToken(source, input, nlevel, SCOPE, NULL);
    if (scanner_result != NO_ERROR)
    {
        return scanner_result;
    }

    //  <LINE_END|FILE_END>
    scanner_result = Parser_getToken(source, input, nlevel, NO_REQUIRED_TYPE, &token);
    if (scanner_result != NO_ERROR)
    {
        return scanner_result;
    }

    if (token->type == FILE_END)
    {
        Scanner_UngetToken(input, &token);
    }
    else if (token->type != LINE_END)
    {
        //  TODO: Error message
        return SYNTAX_ERROR;
    }

    DEBUG_LOG(source, "leaving current nesting level");
    NestingList_leaveCurrentLevel(nlist);
    NestingList_debugPrint(nlist);

    return NO_ERROR;
}

int Parser_ParseExpression(InputPtr input, InstructionListPtr ilist, SymbolTablePtr symtable, PostfixListPtr *postfix, bool isFunctionParam)
{
    //
    //  {<VARIABLE|CONSTANT> {+|-|*|/|\|and|or|>|>=|<|<=|<>} <VARIABLE|CONSTANT> [{+|-|*|/|\|and|or|>|>=|<|<=|<>}]}*
    //

    //-------------------------------------------------d-d-
    //  Inicializace
    //-----------------------------------------------------
    char *source = "parser-expr";
    int scanner_result;
    int parser_result;

    bool last_operand_was_variable = false;
    bool last_operand_was_operator = false; // Jiný než závorky

    TokenPtr  token;
    SymbolPtr symbol = NULL;

    TokenStackPtr tokenStack;


    //-------------------------------------------------d-d-
    //  Zpracování tokenů
    //-----------------------------------------------------
    parser_result = infix2postfix_init(&tokenStack, postfix);
    if (parser_result != NO_ERROR)
    {
        return parser_result;
    }

    scanner_result = Parser_getToken(source, input, NULL, NO_REQUIRED_TYPE, &token);
    do
    {
        if (scanner_result != NO_ERROR)
        {
            //  Při načítání [dalšího] operandu došlo k chybě
            DEBUG_ERR(source, "returning error code");
            return scanner_result;
        }

        if (token->type == IDENTIFIER)
        {
            //  Operand je identifikátor
            if (last_operand_was_variable == true)
            {
                if (Parser_setError_statement("OPERATOR or <EOL>", token, input) != NO_ERROR)
                    return INTERNAL_ERROR;
                return SYNTAX_ERROR;
            }
            last_operand_was_variable = true;
            last_operand_was_operator = false;

            symbol = SymbolTable_getByToken(symtable, token);
            if (symbol == NULL)
            {
                //  TODO: Undeclared error message
                DEBUG_ERR(source, "usage of undefined symbol");
                return SEMANTICAL_DEFINITION_ERROR;
            }
        }
        else if (Token_isConstant(token) == true)
        {
            //  Operand je konstanta
            if (last_operand_was_variable == true)
            {
                if (Parser_setError_statement("OPERATOR or <EOL>", token, input) != NO_ERROR)
                    return INTERNAL_ERROR;
                return SYNTAX_ERROR;
            }
            last_operand_was_variable = true;
            last_operand_was_operator = false;

            symbol = SymbolTable_getByToken(symtable, token);
        }
        else if (Token_isOperator(token) == true)
        {
            //  Operand je operátor
            if (token->type == OPEN_BRACKET)
            {
                if (last_operand_was_variable == true)
                {
                    //  Posledním operátorem byla konstanta nebo identifikátor
                    //  pokud je typ identifikátoru funkce, bude se volat funkce,
                    //  jinak se jedná o syntaktickou chybu
                    if (symbol->type != ST_FUNCTION)
                    {
                        DEBUG_ERR(source, "open bracket after variable (not function identifier), syntax error");
                        //  TODO: Error message
                        return SYNTAX_ERROR;
                    }

                    DEBUG_LOG(source, "open bracket after function identifier, extracting parameters");

                    //  TODO: uložit následující tokeny
                    TokenStackPtr tStack = TokenStack_create();
                    int bracketPairs = 0;
                    do
                    {
                        if (token->type == OPEN_BRACKET)
                        {
                            //  Je očekávána nová dvojice závorek
                            bracketPairs++;
                        }
                        else if (token->type == CLOSE_BRACKET)
                        {
                            //  Ukončení dvojice
                            bracketPairs--;
                        }
                        else if (token->type == LINE_END || token->type == FILE_END)
                        {
                            //  TOTO: Error message
                            return SYNTAX_ERROR;
                            break;
                        }

                        TokenStack_push(tStack, token);
                        if (bracketPairs == 0)
                        {
                            //  Všechny závorky byly uzavřeny
                            break;
                        }
                        scanner_result = Parser_getToken(source, input, NULL, NO_REQUIRED_TYPE, &token);
                        if (scanner_result != NO_ERROR)
                        {
                            return scanner_result;
                        }
                    }
                    while(true);

                    //  Uložení načtených parametrů k symbolu funkce
                    symbol->value2 = tStack;

                    DEBUG_LOG(source, "function params successfully extracted");
                    Symbol_debugPrint(symbol);
                    TokenStack_debugPrint(tStack);

                    DEBUG_LOG(source, "calling infix2postfix_addOperand");
                    infix2postfix_addOperand(&tokenStack, postfix, token, symbol);

                    scanner_result = Parser_getToken(source, input, NULL, NO_REQUIRED_TYPE, &token);

                    last_operand_was_variable = true;
                    last_operand_was_operator = false;
                    continue;
                }
                else
                {
                    //  Otevírací závorka znamená zanoření do podvýrazu
                    DEBUG_LOG(source, "calling infix2postfix_addOperand");
                    infix2postfix_addOperand(&tokenStack, postfix, token, NULL);

                    DEBUG_LOG(source, "calling Parser_ParseSubExpression");
                    parser_result = Parser_ParseSubExpression(input, ilist, symtable, &tokenStack, postfix);
                    if (parser_result != NO_ERROR)
                    {
                        infix2postfix_cleanup(&tokenStack, postfix);
                        return parser_result;
                    }

                    //  Posledním tokenem došlo k uzavření podvýrazu, načteme nový
                    //  jeho zpracování proběhne v následující iteraci
                    scanner_result = Parser_getToken(source, input, NULL, NO_REQUIRED_TYPE, &token);

                    last_operand_was_variable = true;
                    last_operand_was_operator = false;
                    continue;
                }
            }
            else if (token->type == CLOSE_BRACKET)
            {
                if (isFunctionParam == false)
                {
                    DEBUG_ERR(source, "close bracket in main expression is syntax error");
                    if (Parser_setError_statement("OPEN_BRACKET or OPERATOR MAIN", token, input) != NO_ERROR)
                        return INTERNAL_ERROR;
                    return SYNTAX_ERROR;
                }

                DEBUG_LOG(source, "close bracket in main expression for function parameter, ungetting token");
                Scanner_UngetToken(input, &token);
                DEBUG_LOG(source, "expression is complete, leaving loop");
                break;
            }
            else if (token->type == COMMA)
            {
                if (isFunctionParam == false)
                {
                    DEBUG_ERR(source, "comma in main expression is syntax error");
                    if (Parser_setError_statement("OPEN_BRACKET or OPERATOR MAIN", token, input) != NO_ERROR)
                        return INTERNAL_ERROR;
                    return SYNTAX_ERROR;
                }

                DEBUG_LOG(source, "comma in main expression for function parameter, ungetting token");
                Scanner_UngetToken(input, &token);
                DEBUG_LOG(source, "expression is complete, leaving loop");
                break;
            }
            else if (last_operand_was_operator == true)
            {
                DEBUG_ERR(source, "operator after operator is not allowed");
                //  Za operátorem může být jedině otevírací závorka
                if (Parser_setError_statement("OPEN_BRACKET, IDENTIFIER or CONSTANT", token, input) != NO_ERROR)
                    return INTERNAL_ERROR;
                return SYNTAX_ERROR;
            }
            last_operand_was_variable = false;
            last_operand_was_operator = true;

            symbol = NULL;
        }
        else
        {
            //  Neznámý typ tokenu - ani konstanta, ani identifikátor ani operátor
            if (last_operand_was_operator == true)
            {
                DEBUG_ERR(source, "unknown token after operator, syntax error");
                //  Pokud poslední operand byl operátor, máme chybný výraz, něco jako:
                //  var = var +
                if (Parser_setError_statement("IDENTIFIER or CONSTANT", token, input) != NO_ERROR)
                {
                    infix2postfix_cleanup(&tokenStack, postfix);
                    return INTERNAL_ERROR;
                }
                return SYNTAX_ERROR;
            }

            //  Poslední operand nebyl operátor, takže tu výraz ukončíme a daný token
            //  vrátíme pomocí scanneru parent funkci, která si s nim už nějak
            //  poradí
            DEBUG_LOG(source, "expression is complete, leaving loop");
            Scanner_UngetToken(input, &token);
            break;
        }

        DEBUG_LOG(source, "sending operand to infix2postfix function");
        Symbol_debugPrint(symbol);
        parser_result = infix2postfix_addOperand(&tokenStack, postfix, token, symbol);
        if (parser_result != NO_ERROR)
        {
            DEBUG_ERR(source, "failed to add operand, cleaning up");
            infix2postfix_cleanup(&tokenStack, postfix);
            return parser_result;
        }

        //  Načtení dalšího tokenu
        scanner_result = Parser_getToken(source, input, NULL, NO_REQUIRED_TYPE, &token);
    }
    while(true);

    DEBUG_LOG(source, "calling infix2postfix_process");
    TokenStack_debugPrint(tokenStack);
    PostfixList_debugPrint(*postfix);

    parser_result = infix2postfix_process(&tokenStack, postfix);
    if (parser_result != NO_ERROR)
    {
        DEBUG_ERR(source, "failed to process expression, cleaning up");
        infix2postfix_cleanup(&tokenStack, postfix);
        return parser_result;
    }

    DEBUG_LOG(source, "expression processed, returning to parent");
    return NO_ERROR;
}

int Parser_ParseSubExpression(InputPtr input, InstructionListPtr ilist, SymbolTablePtr symtable, TokenStackPtr *tokenStack, PostfixListPtr *postfix)
{
    //-------------------------------------------------d-d-
    //  Inicializace
    //-----------------------------------------------------
    char *source = "parser-expr_sub";
    int scanner_result;
    int parser_result;

    bool last_operand_was_variable = false;
    bool last_operand_was_operator = false; // Jiný než závorky

    TokenPtr  token;
    SymbolPtr symbol = NULL;


    //-------------------------------------------------d-d-
    //  Zpracování tokenů
    //-----------------------------------------------------
    scanner_result = Parser_getToken(source, input, NULL, NO_REQUIRED_TYPE, &token);
    do
    {
        if (scanner_result != NO_ERROR)
        {
            //  Při načítání [dalšího] operandu došlo k chybě
            return scanner_result;
        }

        if (token->type == LINE_END)
        {
            if (Parser_setError_statement(NULL, token, input) != NO_ERROR)
                return INTERNAL_ERROR;
            return SYNTAX_ERROR;
        }
        else if (token->type == IDENTIFIER)
        {
            //  Operand je identifikátor
            if (last_operand_was_variable == true)
            {
                if (Parser_setError_statement("OPERATOR or CLOSE_BRACKET", token, input) != NO_ERROR)
                    return INTERNAL_ERROR;
                return SYNTAX_ERROR;
            }
            last_operand_was_variable = true;
            last_operand_was_operator = false;

            symbol = SymbolTable_getByToken(symtable, token);
            if (symbol == NULL)
            {
                //  TODO: Undeclared error message
                return SEMANTICAL_DEFINITION_ERROR;
            }

            if (Symbol_isVariable(symbol) == false)
            {
                if (symbol->type == ST_FUNCTION)
                {
                    //parser_result = Parser_ParseFunctionCall();
                    return SYNTAX_ERROR;
                }
                else
                {
                    return SEMANTICAL_OTHER_ERROR;
                }
            }
        }
        else if (Token_isConstant(token) == true)
        {
            //  Operand je konstanta
            if (last_operand_was_variable == true)
            {
                if (Parser_setError_statement("OPERATOR or CLOSE_BRACKET", token, input) != NO_ERROR)
                    return INTERNAL_ERROR;
                return SYNTAX_ERROR;
            }
            last_operand_was_variable = true;
            last_operand_was_operator = false;

            symbol = SymbolTable_getByToken(symtable, token);
            if (symbol == NULL)
            {
                return INTERNAL_ERROR;
            }
        }
        else if (Token_isOperator(token) == true)
        {
            //  Operand je operátor
            if (token->type == CLOSE_BRACKET)
            {
                //  Jedná se o ukončení aktuálního podvýrazu, vracíme se o úroveň výš
                infix2postfix_addOperand(tokenStack, postfix, token, NULL);

                DEBUG_LOG(source, "subexpression is complete, returning to parent");
                return NO_ERROR;
            }
            else if (token->type == OPEN_BRACKET)
            {
                //  Otevírací závorka znamená zanoření do podvýrazu
                infix2postfix_addOperand(tokenStack, postfix, token, NULL);

                DEBUG_LOG(source, "calling Parser_ParseSubExpression");
                parser_result = Parser_ParseSubExpression(input, ilist, symtable, tokenStack, postfix);
                if (parser_result != NO_ERROR)
                {
                    return parser_result;
                }

                if (token->type == CLOSE_BRACKET)
                {
                    //  Posledním tokenem došlo k uzavření podvýrazu, načteme nový
                    //  jeho zpracování proběhne v následující iteraci
                    scanner_result = Parser_getToken(source, input, NULL, NO_REQUIRED_TYPE, &token);

                    last_operand_was_variable = true;
                    last_operand_was_operator = false;
                }
                continue;
            }
            else if (last_operand_was_operator == true)
            {
                //  Za operátorem může být jedině otevírací závorka
                if (Parser_setError_statement("OPEN_BRACKET, IDENTIFIER or CONSTANT", token, input) != NO_ERROR)
                    return INTERNAL_ERROR;
                return SYNTAX_ERROR;
            }
            last_operand_was_variable = false;
            last_operand_was_operator = true;

            symbol = NULL;
        }
        else
        {
            //  Token je něco co tu rozhodně neočekáváme,
            //  navíc se zde jedná o podvýraz, takže se zcela nepochybně
            //  jedná o syntaktickou chybu
            //  TODO: Error message
            return SYNTAX_ERROR;
        }

        parser_result = infix2postfix_addOperand(tokenStack, postfix, token, symbol);
        if (parser_result != NO_ERROR)
        {
            return parser_result;
        }

        //  Načtení dalšího tokenu
        scanner_result = Parser_getToken(source, input, NULL, NO_REQUIRED_TYPE, &token);
    }
    while(true);

    return INTERNAL_ERROR;
}

int Parser_getTokens(char *source, InputPtr input, NestingLevelPtr nlevel, TokenTypeListPtr expected_types, TokenListPtr *tokens)
{
    if (tokens != NULL)
    {
        *tokens = TokenList_create();
    }

    TokenPtr  token;
    TokenType expected_type;

    TokenTypeList_first(expected_types);
    TokenTypeListItemPtr type_item = TokenTypeList_get(expected_types);
    expected_type = type_item->type;

    int scanner_result = Parser_getToken(source, input, nlevel, expected_type, &token);

    do
    {
        type_item = TokenTypeList_getNext(expected_types);
        if (type_item == NULL)
        {
            return NO_ERROR;
        }
        expected_type = type_item->type;

        if (tokens != NULL)
        {
            TokenList_insert(*tokens, token);
        }
        else
        {
            Token_destroy(&token);
        }
        scanner_result = Parser_getToken(source, input, nlevel, expected_type, &token);
    }
    while(scanner_result == NO_ERROR);

    return scanner_result;
}

int Parser_getToken(char *source, InputPtr input, NestingLevelPtr nlevel, TokenType expected_type, TokenPtr *token)
{
    #ifdef DEBUG_VERBOSE
    DEBUG_LOG(source, "requesting new token");
    #endif

    TokenPtr t;

    //  Načtení tokenu ze scanneru
    int scanner_result = Scanner_GetToken(input, &t);
    if (token != NULL)
    {
        //  Token se má vrátit přes parametr
        *token = t;
    }

    if (t != NULL)
    {
        //  Převod klíčového slova true a false na booleanovské konstanty
        if (t->type == TRUE || t->type == FALSE)
        {
            t->type = CONSTANT_BOOLEAN;
        }
    }

    if (scanner_result != NO_ERROR || t == NULL || t->type == INVALID)
    {
        //  Scanner byl ukončen s chybovým stavem nebo vrátil neplatný token

        DEBUG_ERR(source, "scanner returned error code");
        if (nlevel)
        {
            DEBUG_PRINT("\tnlevel: %i\n", nlevel->type);
        }
        DEBUG_PRINT("\tcode: %i\n\treason: %s\n", scanner_result, t ? t->attr : "");
        if (t)
        {
            //  Token jsme dostali, získáme z něj chybovou zprávu
            if (Parser_setError_statement(expected_type ? TokenType_toString(expected_type) : NULL, t, input) != NO_ERROR)
            {
                if (token != NULL)
                {
                    *token = NULL;
                }
                Token_destroy(&t);
                return INTERNAL_ERROR;
            }
            Token_destroy(&t);
        }
        else
        {
            //  Token nemáme, předpokládáme problém s alokací
            Parser_setError_allocation();
        }

        if (token != NULL)
        {
            *token = NULL;
        }
        return scanner_result;
    }
    else if ((int) expected_type != -1 && t->type != expected_type)
    {
        //  Má se kontrolovat typ tokenu a token jsme buď nedostali, nebo daný typ nesouhlasí

        DEBUG_ERR(source, "this type of token was not expected!");
        DEBUG_PRINT("\texpected: %i (%s)\n\tgot: %i (%s)\n", expected_type, TokenType_toString(expected_type), t->type, TokenType_toString(t->type));
        #ifdef DEBUG_VERBOSE
        Token_debugPrint(t);
        #endif

        if (Parser_setError_statement(expected_type ? TokenType_toString(expected_type) : NULL, t, input) != NO_ERROR)
        {
            if (token != NULL)
            {
                *token = NULL;
            }
            Token_destroy(&t);
            return INTERNAL_ERROR;
        }

        if (token != NULL)
        {
            *token = NULL;
        }
        Token_destroy(&t);
        return SYNTAX_ERROR;
    }
    else
    {
        //  Získaný token je v pořádku

        #ifdef DEBUG_VERBOSE
        if ((int) expected_type != NO_REQUIRED_TYPE)
            DEBUG_LOG(source, "successfully received correct token");
        else
            DEBUG_LOG(source, "received token");

        if (token == NULL)
            DEBUG_LOG(source, "NOT sending it back");

        Token_debugPrint(t);
        #endif
    }

    /*
    if (token != NULL)
    {
        *token = NULL;
    }
    Token_destroy(&t);*/
    return scanner_result;
}

int Parser_setError_allocation()
{
    error_description = "Failed to allocate memory.";
    return INTERNAL_ERROR;
}

int Parser_setError_undefined(TokenPtr token, InputPtr input)
{
    int  instruction_length;
    if (token->type == INVALID)
    {
        instruction_length = 1;
    }
    else
    {
        instruction_length = strlen(token ? token->attr : "");
    }

    int  error_index = input->character - instruction_length;
    char *message;

    if (token->type == INVALID)
    {
        //  Speciální typ tokenu - obsahuje chybovou zprávu
        message = token->attr;
    }
    else
    {
        message = String_printf("Usage of undefined reference %s on line %i:%i.", token->attr, (char *) input->line, (char *) (error_index + 1), NULL);
    }

    if (message == NULL)
    {
        return Parser_setError_allocation();
    }

    error_description        = message;
    error_char_index         = error_index;
    error_instruction_length = instruction_length;

    return NO_ERROR;
}

int Parser_setError_alreadyDefined(TokenPtr token, InputPtr input)
{
    int  instruction_length;
    if (token->type == INVALID)
    {
        instruction_length = 1;
    }
    else
    {
        instruction_length = strlen(token ? token->attr : "");
    }

    int  error_index = input->character - instruction_length;
    char *message;

    if (token->type == INVALID)
    {
        //  Speciální typ tokenu - obsahuje chybovou zprávu
        message = token->attr;
    }
    else
    {
        message = String_printf("Cannot redeclare/redefine symbol %s on line %i:%i.", token->attr, (char *) input->line, (char *) (error_index + 1), NULL);
    }

    if (message == NULL)
    {
        return Parser_setError_allocation();
    }

    error_description        = message;
    error_char_index         = error_index;
    error_instruction_length = instruction_length;

    return NO_ERROR;
}

int Parser_setError_statement(char *expected, TokenPtr token, InputPtr input)
{
    int  instruction_length;
    if (token->type == INVALID)
    {
        instruction_length = 1;
    }
    else
    {
        instruction_length = strlen(token ? token->attr : "");
    }

    int  error_index = input->character - instruction_length;
    char *message;

    if (token->type == INVALID)
    {
        //  Speciální typ tokenu - obsahuje chybovou zprávu
        message = token->attr;
    }
    else if (expected != NULL)
    {
        //  Očekávali jsme něco konkrétního
        message = String_printf("Unexpected statement on line %i:%i. Expected %s got %s.", (char *) input->line, (char *) (error_index + 1), expected, TokenType_toString(token->type));
    }
    else
    {
        //  Buď jsme toho očekávali spoustu, jako vždy,
        //  nebo jsme toho naopak moc neočekávali
        message = String_printf("Unexpected %s on line %i:%i.", TokenType_toString(token->type), (char *) input->line, (char *) (error_index + 1), NULL);
    }

    if (message == NULL)
    {
        return Parser_setError_allocation();
    }

    error_description        = message;
    error_char_index         = error_index;
    error_instruction_length = instruction_length;

    return NO_ERROR;
}

int Parser_setError_custom(char *content)
{
    error_description        = content;
    error_char_index         = -1;
    error_instruction_length = -1;
    return NO_ERROR;
}

#endif
