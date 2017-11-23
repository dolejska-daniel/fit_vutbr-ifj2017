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
    DEBUG_LOG("parser-init", "Initializing variables");

    //  Inicializace proměnných
    int scanner_result;
    int parser_result;

    int result = NO_ERROR;
    TokenPtr token;

    NestingListPtr nlist = NestingList_create();
    NestingList_newLevel(nlist, NESTING_NONE);

    //  Prvotní instrukce
    DEBUG_LOG("parser-init", "Writing initial instructions");
    Instruction_custom(ilist, ".IFJcode17");
    Instruction_custom(ilist, "JUMP main");

    DEBUG_LOG("parser-init", "Fetching first token");
    while (true)
    {
        //  Získání tokenu
        scanner_result = Scanner_GetToken(input, &token);
        if (scanner_result != NO_ERROR
            || token == NULL || token->type == INVALID)
        {
            //  Došlo k chybě při skenování
            DEBUG_ERR("parser-init", "scanner returned error code");
            DEBUG_PRINT("\tcode: %i\n\treason: %s\n", scanner_result, token ? token->attr : "failed to mallocate Token");

            if (token == NULL)
            {
                Parser_setError_allocation();
            }
            else
            {
                if (Parser_setError_statement(NULL, token, input) != NO_ERROR)
                {
                    Token_destroy(&token);
                    return INTERNAL_ERROR;
                }
                Token_destroy(&token);
            }
            return scanner_result;
        }

        #ifdef DEBUG_VERBOSE
        DEBUG_LOG("parser-init", "successfully received token");
        Token_debugPrint(token);
        #endif

        switch (token->type)
        {
            //-------------------------------------------------d-d-
            //  Funkce
            //-----------------------------------------------------
            case DECLARE:
            {
                //  Jedná se o posloupnost deklarace funkce

                #ifdef DEBUG_VERBOSE
                DEBUG_LOG("parser-init", "calling Parser_ParseFunctionDeclaration");
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
                DEBUG_LOG("parser-init", "calling Parser_ParseFunctionDefinition");
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
                DEBUG_LOG("parser-init", "calling Parser_ParseScope");
                #endif

                parser_result = Parser_ParseScope(input, ilist, symtable, nlist);
                if (parser_result != NO_ERROR)
                {
                    return parser_result;
                }
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
                DEBUG_ERR("parser-init", "this type of token was not expected!");
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
    DEBUG_LOG("parser-nested", "Initializing variables");
    //  Inicializace proměnných
    int scanner_result;
    int parser_result;

    NestingLevelPtr nlevel = NestingList_active(nlist);
    TokenPtr token;

    DEBUG_LOG("parser-nested", "Fetching first token");
    while (true)
    {
        //  Získání tokenu
        scanner_result = Scanner_GetToken(input, &token);
        if (scanner_result != NO_ERROR
            || token == NULL || token->type == INVALID)
        {
            //  Došlo k chybě při skenování
            *last_token = NULL;

            DEBUG_ERR("parser-nested", "Scanner returned error code");
            DEBUG_PRINT("\tntype: %i\n", nlevel->type);
            DEBUG_PRINT("\tcode: %i\n\treason: %s\n", scanner_result, token ? token->attr : "failed to mallocate Token");

            if (Parser_setError_statement(NULL, token, NULL) != NO_ERROR)
                return INTERNAL_ERROR;

            Token_destroy(&token);
            return scanner_result;
        }

        if (*last_token != NULL)
            Token_destroy(last_token);
        *last_token = Token_create(token->type, token->attr);

        #ifdef DEBUG_VERBOSE
        DEBUG_LOG("parser-nested", "successfully received token");
        Token_debugPrint(token);
        #endif

        switch (token->type)
        {
            //-------------------------------------------------d-d-
            //  Funkce
            //-----------------------------------------------------
            case ASC:
            {
                //  Jedná se o posloupnost pro použití funkce ASC

                #ifdef DEBUG_VERBOSE
                DEBUG_LOG("parser-nested", "calling Parser_ParseBuiltinFunction_Asc");
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
                DEBUG_LOG("parser-nested", "calling Parser_ParseBuiltinFunction_Chr");
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
                DEBUG_LOG("parser-nested", "calling Parser_ParseBuiltinFunction_Length");
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
                DEBUG_LOG("parser-nested", "calling Parser_ParseBuiltinFunction_Substr");
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
                DEBUG_LOG("parser-nested", "calling Parser_ParseVariableDeclaration");
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

                #ifdef DEBUG_VERBOSE
                DEBUG_LOG("parser-nested", "calling Parser_ParseVariableDefinition");
                #endif

                parser_result = Parser_ParseVariableDefinition(input, ilist, symtable, nlist);
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
                DEBUG_LOG("parser-nested", "calling Parser_ParseCondition");
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
                DEBUG_LOG("parser-nested", "calling Parser_ParseLoop_Do");
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
                DEBUG_LOG("parser-nested", "calling Parser_ParseLoop_For");
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
                DEBUG_LOG("parser-nested", "calling Parser_ParseStatement_Print");
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
                DEBUG_LOG("parser-nested", "calling Parser_ParseStatement_Input");
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
                    DEBUG_ERR("parser-nested", "Continue statement not within loop!");
                    if (Parser_setError_statement(NULL, token, input) != NO_ERROR)
                        return INTERNAL_ERROR;
                    return SYNTAX_ERROR;
                }

                #ifdef DEBUG_VERBOSE
                DEBUG_LOG("parser-nested", "calling Parser_ParseStatement_Continue");
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
                    DEBUG_ERR("parser-nested", "Exit statement not within loop!");
                    if (Parser_setError_statement(NULL, token, input) != NO_ERROR)
                        return INTERNAL_ERROR;
                    return SYNTAX_ERROR;
                }

                #ifdef DEBUG_VERBOSE
                DEBUG_LOG("parser-nested", "calling Parser_ParseStatement_Exit");
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
                    DEBUG_ERR("parser-nested", "Return statement not within function!");
                    if (Parser_setError_statement(NULL, token, input) != NO_ERROR)
                        return INTERNAL_ERROR;
                    return SYNTAX_ERROR;
                }

                #ifdef DEBUG_VERBOSE
                DEBUG_LOG("parser-nested", "calling Parser_ParseStatement_Return");
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
                DEBUG_LOG("parser-nested", "calling Parser_ParseScope (NOT NOW! SYNTAX_ERR INTENTIONALLY)");
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
                DEBUG_LOG("parser-nested", "received END, returning to parent");
                #endif

                return NO_ERROR;
                break;
            }

            case LINE_END:
            {
                //  Konec řádku, prázdný řádek

                #ifdef DEBUG_VERBOSE
                DEBUG_LOG("parser-nested", "received LINE_END, continuing to next token");
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

                DEBUG_ERR("parser-nested", "this type of token was not expected!");
                Token_debugPrint(token);
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

        DEBUG_ERR("parser-function-def", "this type of token was not expected!");
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
        /*
        instruction_result = Instruction_variable_declare(ilist, var);
        if (instruction_result != NO_ERROR)
        {
            return instruction_result;
        }*/
    }

    return NO_ERROR;
}

int Parser_ParseVariableDefinition(InputPtr input, InstructionListPtr ilist, SymbolTablePtr symtable, NestingListPtr nlist)
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

    TokenPtr token;
    NestingLevelPtr nlevel = NestingList_active(nlist);

    //-------------------------------------------------d-d-
    //  Zpracování tokenů
    //-----------------------------------------------------

    //  _EXPRESSION_
    DEBUG_LOG(source, "calling Parser_ParseExpression");
    parser_result = Parser_ParseExpression(input, ilist, symtable);
    if (parser_result != NO_ERROR)
    {
        return parser_result;
    }

    //  TODO: Process expression

    //  <LINE_END>
    scanner_result = Parser_getToken(source, input, nlevel, LINE_END, NULL);
    if (scanner_result != NO_ERROR)
    {
        return scanner_result;
    }

    return INTERNAL_ERROR;
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
        }
        else if (token->type >= CONSTANT_INTEGER && token->type <= CONSTANT_BOOLEAN)
        {
            //  Jedná se o konstantu
            //  TODO: vytvořit symbol pro konstantu
            instruction_result = Instruction_write(ilist, symbol);
        }
        else
        {
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

    TokenPtr token;


    return INTERNAL_ERROR;
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
        if (last_token != NULL)
            Token_destroy(&last_token);
        return parser_result;
    }

    //-------------------------------------------------d-d-
    //  Závěrečné tokeny
    //-----------------------------------------------------
    TokenTypeListPtr closing_tokens = TokenTypeList_create();
    TokenTypeList_insert(closing_tokens, END);
    TokenTypeList_insert(closing_tokens, SCOPE);
    TokenTypeList_insert(closing_tokens, LINE_END);

    scanner_result = Parser_getTokens(source, input, nlevel, closing_tokens, NULL);
    if (scanner_result != NO_ERROR)
    {
        TokenTypeList_destroy(&closing_tokens);
        return scanner_result;
    }

    TokenTypeList_destroy(&closing_tokens);
    if (last_token != NULL)
        Token_destroy(&last_token);
    return NO_ERROR;
}

int Parser_ParseExpression(InputPtr input, InstructionListPtr ilist, SymbolTablePtr symtable)
{

    //-------------------------------------------------d-d-
    //  Inicializace
    //-----------------------------------------------------
    int scanner_result;
    int parser_result;

    TokenPtr token;


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
        DEBUG_PRINT("\tnlevel: %i\n", nlevel->type);
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
    else if (expected_type >= 0 && t->type != expected_type)
    {
        //  Má se kontrolovat typ tokenu a token jsme buď nedostali, nebo daný typ nesouhlasí

        DEBUG_ERR(source, "this type of token was not expected!");
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
        if (expected_type >= 0)
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
