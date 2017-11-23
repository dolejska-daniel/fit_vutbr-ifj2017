/**
 * Tento soubor obsahuje implementaci syntaktického analyzátoru (parser).
 *
 * @author Daniel Dolejška (xdolej08)
 * @date 21.11.2017
 * @project IFJcode17Parser
 * @subject Formální jazyky a překladače (IFJ) - FIT VUT v Brně
 */

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
#include "../support/token_list.h"
#include "../support/tokenType_list.h"
#else
#include "generator.h"
#include "scanner.h"
#include "token.h"
#include "strings.h"
#include "error_codes.h"
#include "infix2postfix.h"
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
    NestingLevelPtr nlevel = NestingList_newLevel(nlist, NESTING_NONE);

    //  Prvotní instrukce
    DEBUG_LOG(source, "Writing initial instructions");
    Instruction_custom(ilist, ".IFJcode17");
    Instruction_custom(ilist, "JUMP main");

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
                Instruction_outputAll(ilist);
                break;
            }

            case LINE_END:
            {
                //  Konec řádku, prázdný řádek

                continue;
                break;
            }

            case FILE_END:
            {
                //  Konec vstupních dat

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

int Parser_ParseNestedCode(InputPtr input, InstructionListPtr ilist, SymbolTablePtr symtable, NestingListPtr nlist, TokenPtr *last_token)
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

        if (*last_token != NULL)
            Token_destroy(last_token);
        *last_token = token;

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
                    //  TODO: Undefined error message
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

                if (NestingList_isNestedIn(nlist, NESTING_LOOP))
                {
                    DEBUG_ERR(source, "Continue statement not within loop!");
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

                if (NestingList_isNestedIn(nlist, NESTING_LOOP))
                {
                    DEBUG_ERR(source, "Exit statement not within loop!");
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

                if (NestingList_isNestedIn(nlist, NESTING_FUNCTION))
                {
                    DEBUG_ERR(source, "Return statement not within function!");
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

                #ifdef DEBUG_VERBOSE
                DEBUG_LOG(source, "received END, returning to parent");
                #endif

                return NO_ERROR;
                break;
            }

            case LINE_END:
            {
                //  Konec řádku, prázdný řádek

                #ifdef DEBUG_VERBOSE
                DEBUG_LOG(source, "received LINE_END, continuing to next token");
                #endif

                continue;
                break;
            }

            case FILE_END:
            {
                //  Konec vstupních dat

                #ifdef DEBUG_VERBOSE
                //DEBUG_PRINT("\ttype-str: FILE_END\n");
                #endif
            }
            default:
            {
                //  Token není nic, co bychom očekáváli
                //  jedná se tedy o syntaktickou chybu

                DEBUG_ERR(source, "this type of token was not expected!");
                Token_debugPrint(token);

                *last_token = NULL;
                Token_destroy(&token);

                if (Parser_setError_statement(NULL, token, input) != NO_ERROR)
                    return INTERNAL_ERROR;
                return SYNTAX_ERROR;
                break;
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
    int parser_result;

    TokenPtr token;

    //-------------------------------------------------d-d-
    //  Zpracování tokenů
    //-----------------------------------------------------


    return INTERNAL_ERROR;
}

int Parser_ParseFunctionDefinition(InputPtr input, InstructionListPtr ilist, SymbolTablePtr symtable, NestingListPtr nlist)
{
    //             |->
    //  <FUNCTION> | <IDENTIFIER> <OPEN_BRACKET> {[<COMMA>] <IDENTIFIER> <AS> <INTEGER|STRING|DOUBLE|BOOLEAN>}* <CLOSE_BRACKET> <AS> <INTEGER|STRING|DOUBLE|BOOLEAN> <LINE_END>
    //  {_CODE_}*
    //  <END> <FUNCTION> <LINE_END|FILE_END>
    //

    //-------------------------------------------------d-d-
    //  Inicializace
    //-----------------------------------------------------
    char *source = "parser-func_def";
    int scanner_result;
    int parser_result;

    TokenPtr token;
    TokenPtr last_token = NULL;

    NestingLevelPtr nlevel = NestingList_newLevel(nlist, NESTING_FUNCTION);

    //-------------------------------------------------d-d-
    //  Zpracování tokenů
    //-----------------------------------------------------


    //-------------------------------------------------d-d-
    //  Parsování vnitřního kódu funkce
    //-----------------------------------------------------
    parser_result = Parser_ParseNestedCode(input, ilist, symtable, nlist, &last_token);
    if (parser_result != NO_ERROR)
    {
        if (last_token != NULL)
            Token_destroy(&last_token);
        return parser_result;
    }

    //-------------------------------------------------d-d-
    //  Závěrečné tokeny
    //-----------------------------------------------------
    if (last_token != NULL && last_token->type == END)
    {

    }
    else
    {
        //  TODO: Error, invalid token

        DEBUG_ERR(source, "this type of token was not expected!");
        Token_debugPrint(last_token);

        if (Parser_setError_statement("END", last_token, input) != NO_ERROR)
            return INTERNAL_ERROR;

        if (last_token != NULL)
            Token_destroy(&last_token);
        return SYNTAX_ERROR;
    }

    if (last_token != NULL)
        Token_destroy(&last_token);
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
    int parser_result;
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
    int scanner_result;
    int parser_result;
    int instruction_result;

    TokenPtr token;
    TokenPtr last_token = NULL;

    PostfixListPtr postfix = NULL;

    NestingLevelPtr nlevel = NestingList_active(nlist);

    //-------------------------------------------------d-d-
    //  Zpracování tokenů
    //-----------------------------------------------------

    //  _EXPRESSION_
    DEBUG_LOG(source, "calling Parser_ParseExpression");
    parser_result = Parser_ParseExpression(input, ilist, symtable, &postfix, &last_token);
    if (parser_result != NO_ERROR)
    {
        DEBUG_ERR(source, "expression failed to be parsed");
        Token_destroy(&last_token);
        if (postfix != NULL)
            PostfixList_destroy(&postfix);
        return parser_result;
    }

    if (variable->type == ST_BOOLEAN)
    {
        //  Vyhodnocení logické hodnoty
        DEBUG_LOG(source, "postfix conversion completed, calling postfix2instructions_logical");
        parser_result = postfix2instructions_logical(ilist, &postfix);
        if (parser_result != NO_ERROR)
        {
            return parser_result;
        }
    }
    else
    {
        //  Vyhodnocení číselné hodnoty
        DEBUG_LOG(source, "postfix conversion completed, calling postfix2instructions_mathematical");
        parser_result = postfix2instructions_mathematical(ilist, &postfix);
        if (parser_result != NO_ERROR)
        {
            return parser_result;
        }
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
    if (last_token->type != LINE_END)
    {
        if (Parser_setError_statement("<EOL>", last_token, input))
        {
            Token_destroy(&last_token);
            return INTERNAL_ERROR;
        }

        Token_debugPrint(last_token);
        Token_destroy(&last_token);
        return SYNTAX_ERROR;
    }

    Token_destroy(&last_token);
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
    //  {_CODE_ <LINE_END>}*
    //  <END> <IF> <LINE_END>
    //

    //-------------------------------------------------d-d-
    //  Inicializace
    //-----------------------------------------------------
    char *source = "parser-cond";
    int scanner_result;
    int parser_result;

    TokenPtr token;


    return INTERNAL_ERROR;
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
    char *source = "parser-loop-do";
    int scanner_result;
    int parser_result;

    TokenPtr token;


    return INTERNAL_ERROR;
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
    char *source = "parser-loop-for";
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
    int parser_result;
    int instruction_result;

    bool firstLoop = true;

    TokenPtr  token  = NULL;
    SymbolPtr symbol = NULL;

    NestingLevelPtr nlevel = NestingList_active(nlist);

    //-------------------------------------------------d-d-
    //  Zpracování tokenů
    //-----------------------------------------------------

    scanner_result = Parser_getToken(source, input, nlevel, -1, &token);
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
            if (Parser_setError_statement(firstLoop ? "identifier or constant" : "identifier, constant or end of line", token, input) != NO_ERROR)
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
        scanner_result = Parser_getToken(source, input, nlevel, -1, &token);
        if (scanner_result != NO_ERROR)
        {
            return scanner_result;
        }

        if (token->type == LINE_END)
        {
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
    int parser_result;

    TokenPtr token;

    NestingLevelPtr nlevel = NestingList_active(nlist);


    return INTERNAL_ERROR;
}

int Parser_ParseStatement_Continue(InputPtr input, InstructionListPtr ilist, SymbolTablePtr symtable, NestingListPtr nlist)
{

    //-------------------------------------------------d-d-
    //  Inicializace
    //-----------------------------------------------------
    char *source = "parser-continue";
    int scanner_result;
    int parser_result;

    TokenPtr token;

    NestingLevelPtr nlevel = NestingList_active(nlist);


    return INTERNAL_ERROR;
}

int Parser_ParseStatement_Exit(InputPtr input, InstructionListPtr ilist, SymbolTablePtr symtable, NestingListPtr nlist)
{

    //-------------------------------------------------d-d-
    //  Inicializace
    //-----------------------------------------------------
    char *source = "parser-exit";
    int scanner_result;
    int parser_result;

    TokenPtr token;

    NestingLevelPtr nlevel = NestingList_active(nlist);


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

    TokenPtr token;

    NestingLevelPtr nlevel = NestingList_active(nlist);


    return INTERNAL_ERROR;
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
    int parser_result;
    int instruction_result;

    TokenPtr token;
    TokenPtr last_token = NULL;

    SymbolPtr symbol = NULL;

    //-------------------------------------------------d-d-
    //  Zpracování tokenů
    //-----------------------------------------------------
    TokenTypeListPtr tokens = TokenTypeList_create();
    TokenTypeList_insert(tokens, OPEN_BRACKET);
    TokenTypeList_insert(tokens, IDENTIFIER);
    TokenTypeList_insert(tokens, COMMA);
    TokenTypeList_insert(tokens, IDENTIFIER);
    TokenTypeList_insert(tokens, COMMA);
    TokenTypeList_insert(tokens, IDENTIFIER);
    TokenTypeList_insert(tokens, CLOSE_BRACKET);
    TokenTypeList_insert(tokens, LINE_END);

    scanner_result = Parser_getTokens(source, input, NULL, tokens, &last_token);
    if (scanner_result != NO_ERROR)
    {
        TokenTypeList_destroy(&tokens);
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

    TokenTypeList_destroy(&tokens);
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
    TokenPtr last_token = NULL;

    NestingLevelPtr nlevel = NestingList_newLevel(nlist, NESTING_SCOPE);

    //-------------------------------------------------d-d-
    //  Úvodní tokeny
    //-----------------------------------------------------
    scanner_result = Parser_getToken(source, input, nlevel, LINE_END, &token);
    if (scanner_result != NO_ERROR)
    {
        return scanner_result;
    }

    //-------------------------------------------------d-d-
    //  Zanořené instrukce
    //-----------------------------------------------------
    Instruction_label(ilist, "main");

    DEBUG_LOG(source, "calling Parser_ParseNestedCode");
    parser_result = Parser_ParseNestedCode(input, ilist, symtable, nlist, &last_token);
    if (parser_result != NO_ERROR)
    {
        Token_destroy(&last_token);
        return parser_result;
    }

    //-------------------------------------------------d-d-
    //  Závěrečné tokeny
    //-----------------------------------------------------
    DEBUG_LOG(source, "parsing closing tokens");
    if (last_token == NULL || last_token->type != END)
    {
        DEBUG_ERR(source, "last_token from nested code is incorrect");
        Token_debugPrint(last_token);
        Token_destroy(&last_token);
        return SYNTAX_ERROR;
    }
    Token_destroy(&last_token);

    TokenTypeListPtr closing_tokens = TokenTypeList_create();
    TokenTypeList_insert(closing_tokens, SCOPE);
    TokenTypeList_insert(closing_tokens, LINE_END);

    scanner_result = Parser_getTokens(source, input, nlevel, closing_tokens, NULL);
    if (scanner_result != NO_ERROR)
    {
        TokenTypeList_destroy(&closing_tokens);
        return scanner_result;
    }

    TokenTypeList_destroy(&closing_tokens);
    return NO_ERROR;
}

int Parser_ParseExpression(InputPtr input, InstructionListPtr ilist, SymbolTablePtr symtable, PostfixListPtr *postfix, TokenPtr *last_token)
{
    //
    //  {<VARIABLE|CONSTANT> {+|-|*|/|\|and|or|>|>=|<|<=|<>} <VARIABLE|CONSTANT> [{+|-|*|/|\|and|or|>|>=|<|<=|<>}]}*
    //

    //-------------------------------------------------d-d-
    //  Inicializace
    //-----------------------------------------------------
    char *source = "parse-expr";
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
    *last_token = token;
    do
    {
        if (scanner_result != NO_ERROR)
        {
            //  Při načítání [dalšího] operandu došlo k chybě
            return scanner_result;
        }

        if (token->type == LINE_END)
        {
            DEBUG_LOG(source, "expression is complete, leaving loop");
            break;
        }
        else if (token->type == IDENTIFIER)
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
                //  Otevírací závorka znamená zanoření do podvýrazu
                infix2postfix_addOperand(&tokenStack, postfix, token, NULL);

                DEBUG_LOG(source, "calling Parser_ParseSubExpression");
                parser_result = Parser_ParseSubExpression(input, ilist, symtable, &tokenStack, postfix, &token);
                *last_token = token;
                if (parser_result != NO_ERROR)
                {
                    infix2postfix_cleanup(&tokenStack, postfix);
                    return parser_result;
                }

                if (token->type == CLOSE_BRACKET)
                {
                    //  Posledním tokenem došlo k uzavření podvýrazu, načteme nový
                    //  jeho zpracování proběhne v následující iteraci
                    infix2postfix_addOperand(&tokenStack, postfix, token, NULL);

                    scanner_result = Parser_getToken(source, input, NULL, NO_REQUIRED_TYPE, &token);
                    *last_token = token;

                    last_operand_was_variable = true;
                    last_operand_was_operator = false;
                }
                continue;
            }
            else if (token->type == CLOSE_BRACKET)
            {
                if (Parser_setError_statement("OPEN_BRACKET or OPERATOR MAIN", token, input) != NO_ERROR)
                    return INTERNAL_ERROR;
                return SYNTAX_ERROR;
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
            if (Parser_setError_statement(NULL, token, input) != NO_ERROR)
            {
                infix2postfix_cleanup(&tokenStack, postfix);
                return INTERNAL_ERROR;
            }
            infix2postfix_cleanup(&tokenStack, postfix);
            return SYNTAX_ERROR;
        }

        DEBUG_LOG(source, "sending operand to infix2postfix function");
        Symbol_debugPrint(symbol);
        parser_result = infix2postfix_addOperand(&tokenStack, postfix, token, symbol);
        if (parser_result != NO_ERROR)
        {
            infix2postfix_cleanup(&tokenStack, postfix);
            return parser_result;
        }

        //  Načtení dalšího tokenu
        scanner_result = Parser_getToken(source, input, NULL, NO_REQUIRED_TYPE, &token);
        *last_token = token;
    }
    while(true);

    DEBUG_LOG(source, "calling infix2postfix_process");
    parser_result = infix2postfix_process(&tokenStack, postfix);
    if (parser_result != NO_ERROR)
    {
        infix2postfix_cleanup(&tokenStack, postfix);
        return parser_result;
    }

    return NO_ERROR;
}

int Parser_ParseSubExpression(InputPtr input, InstructionListPtr ilist, SymbolTablePtr symtable, TokenStackPtr *tokenStack, PostfixListPtr *postfix, TokenPtr *last_token)
{
    //-------------------------------------------------d-d-
    //  Inicializace
    //-----------------------------------------------------
    char *source = "parse-expr_sub";
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
    *last_token = token;
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
                parser_result = Parser_ParseSubExpression(input, ilist, symtable, tokenStack, postfix, &token);
                *last_token = token;
                if (parser_result != NO_ERROR)
                {
                    return parser_result;
                }

                if (token->type == CLOSE_BRACKET)
                {
                    //  Posledním tokenem došlo k uzavření podvýrazu, načteme nový
                    //  jeho zpracování proběhne v následující iteraci
                    scanner_result = Parser_getToken(source, input, NULL, NO_REQUIRED_TYPE, &token);
                    *last_token = token;

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
            if (Parser_setError_statement(source, token, input) != NO_ERROR)
                return INTERNAL_ERROR;
            return SYNTAX_ERROR;
        }

        parser_result = infix2postfix_addOperand(tokenStack, postfix, token, symbol);
        if (parser_result != NO_ERROR)
        {
            return parser_result;
        }

        //  Načtení dalšího tokenu
        scanner_result = Parser_getToken(source, input, NULL, NO_REQUIRED_TYPE, &token);
        *last_token = token;
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

int Parser_setError_undefinedSymbol(SymbolPtr symbol, TokenPtr token, InputPtr input)
{

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
        message = String_printf("Unexpected statement on line %i:%i. Expected %s got %s.", (char *) input->line, (char *) error_index, expected, TokenType_toString(token->type));
    }
    else
    {
        //  Buď jsme toho očekávali spoustu, jako vždy,
        //  nebo jsme toho naopak moc neočekávali
        message = String_printf("Unexpected %s ('%s') on line %i:%i.", TokenType_toString(token->type), token->attr, (char *) input->line, (char *) error_index);
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
