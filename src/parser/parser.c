/**
 * Tento soubor obsahuje implementaci syntaktického analyzátoru (parser).
 *
 * @author Daniel Dolejška (xdolej08)
 * @date 21.11.2017
 * @project IFJcode17Parser
 * @subject Formální jazyky a překladače (IFJ) - FIT VUT v Brně
 */

#include <stdbool.h>

#include "parser.h"
#include "nesting_list.h"

#ifndef _parser_c
#define _parser_c

#ifdef DEBUG_INCLUDE
#include "../generator/generator.h"
#include "../scanner/token.h"
#include "../support/strings.h"
#include "../support/error_codes.h"
#include "../support/infix2postfix.h"
#include "../support/token_list.h"
#include "../support/tokenType_list.h"
#else
#include "generator.h"
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
            char *scanner_result_reason = "failed to mallocate Token";
            if (token != NULL)
            {
                scanner_result_reason = token->attr;
            }

            DEBUG_ERR("parser-init", "Scanner returned error code");
            DEBUG_PRINT("\tcode: %i\n\treason: %s\n", scanner_result, scanner_result_reason);

            if (token == NULL)
            {
                Parser_setError_allocation();
            }
            else
            {
                if (Parser_setError_statement(token->attr, NULL) != NO_ERROR)
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
        DEBUG_PRINT("\ttype: %i\n\tattr: %s\n", token->type, token->attr);
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
                DEBUG_PRINT("\ttype-str: DECLARE\n");
                DEBUG_LOG("parser-init", "calling Parser_ParseFunctionDeclaration");
                #endif
                parser_result = Parser_ParseFunctionDeclaration(input, ilist, symtable, nlist);
                if (parser_result != NO_ERROR)
                {
                    DEBUG_ERR("parser-init", "called parser function returned error code");
                    DEBUG_PRINT("\tcode: %i\n", parser_result);
                    return parser_result;
                }
                break;
            }

            case FUNCTION:
            {
                //  Jedná se o posloupnost definice funkce
                //  (POUZE POKUD JE FUNKCE JIŽ DEKLAROVÁNA)
                #ifdef DEBUG_VERBOSE
                DEBUG_PRINT("\ttype-str: FUNCTION\n");
                DEBUG_LOG("parser-init", "calling Parser_ParseFunctionDefinition");
                #endif
                parser_result = Parser_ParseFunctionDefinition(input, ilist, symtable, nlist);
                if (parser_result != NO_ERROR)
                {
                    DEBUG_ERR("parser-init", "called parser function returned error code");
                    DEBUG_PRINT("\tcode: %i\n", parser_result);
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
                DEBUG_PRINT("\ttype-str: SCOPE\n");
                DEBUG_LOG("parser-init", "calling Parser_ParseFunctionDefinition");
                #endif
                parser_result = Parser_ParseScope(input, ilist, symtable, nlist);
                if (parser_result != NO_ERROR)
                {
                    DEBUG_ERR("parser-init", "called parser function returned error code");
                    DEBUG_PRINT("\tcode: %i\n", parser_result);
                    return parser_result;
                }
                break;
            }

            case LINE_END:
            {
                //  Konec řádku, prázdný řádek
                #ifdef DEBUG_VERBOSE
                DEBUG_PRINT("\ttype-str: LINE_END\n");
                #endif
                continue;
                break;
            }

            case FILE_END:
            {
                //  Konec vstupních dat
                #ifdef DEBUG_VERBOSE
                DEBUG_PRINT("\ttype-str: FILE_END\n");
                #endif
                return result;
                break;
            }

            default:
            {
                //  Token není nic, co bychom očekáváli
                //  jedná se tedy o syntaktickou chybu
                DEBUG_ERR("parser-init", "this type of token was not expected!");
                DEBUG_PRINT("\ttype: %i\n\tattr: %s\n", token ? token->type : -1, NULL ? token->attr : NULL);

                if (token == NULL)
                {
                    Parser_setError_allocation();
                }
                else
                {
                    if (Parser_setError_statement(token->attr, NULL) != NO_ERROR)
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
            DEBUG_PRINT("onload token");
            //  Došlo k chybě při skenování
            char *scanner_result_reason = "failed to mallocate Token";
            if (token != NULL)
            {
                scanner_result_reason = token->attr;
            }
            *last_token = NULL;

            DEBUG_ERR("parser-nested", "Scanner returned error code");
            DEBUG_PRINT("\tntype: %i\n", nlevel ? nlevel->type : -1);
            DEBUG_PRINT("\tcode: %i\n\treason: %s\n", scanner_result, scanner_result_reason);

            if (Parser_setError_statement(scanner_result_reason, NULL) != NO_ERROR)
                return INTERNAL_ERROR;

            Token_destroy(&token);
            return scanner_result;
        }

        if (*last_token != NULL)
            Token_destroy(last_token);
        *last_token = Token_create(token->type, token->attr);

        #ifdef DEBUG_VERBOSE
        DEBUG_LOG("parser-nested", "successfully received token");
        DEBUG_PRINT("\tntype: %i\n", nlevel ? nlevel->type : -1);
        DEBUG_PRINT("\ttype: %i\n\tattr: %s\n", token->type, token->attr);
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
                DEBUG_PRINT("\ttype-str: ASC\n");
                DEBUG_LOG("parser-nested", "calling Parser_ParseBuiltinFunction_Asc");
                #endif
                parser_result = Parser_ParseBuiltinFunction_Asc(input, ilist, symtable);
                if (parser_result != NO_ERROR)
                {
                    DEBUG_ERR("parser-nested", "called parser function returned error code");
                    DEBUG_PRINT("\tcode: %i\n", parser_result);
                    return parser_result;
                }
                break;
            }

            case CHR:
            {
                //  Jedná se o posloupnost pro použití funkce CHR
                #ifdef DEBUG_VERBOSE
                DEBUG_PRINT("\ttype-str: CHR\n");
                DEBUG_LOG("parser-nested", "calling Parser_ParseBuiltinFunction_Chr");
                #endif
                parser_result = Parser_ParseBuiltinFunction_Chr(input, ilist, symtable);
                if (parser_result != NO_ERROR)
                {
                    DEBUG_ERR("parser-nested", "called parser function returned error code");
                    DEBUG_PRINT("\tcode: %i\n", parser_result);
                    return parser_result;
                }
                break;
            }

            case LENGTH:
            {
                //  Jedná se o posloupnost pro použití funkce LENGTH
                #ifdef DEBUG_VERBOSE
                DEBUG_PRINT("\ttype-str: LENGTH\n");
                DEBUG_LOG("parser-nested", "calling Parser_ParseBuiltinFunction_Length");
                #endif
                parser_result = Parser_ParseBuiltinFunction_Length(input, ilist, symtable);
                if (parser_result != NO_ERROR)
                {
                    DEBUG_ERR("parser-nested", "called parser function returned error code");
                    DEBUG_PRINT("\tcode: %i\n", parser_result);
                    return parser_result;
                }
                break;
            }

            case SUBSTR:
            {
                //  Jedná se o posloupnost pro použití funkce SUBSTR
                #ifdef DEBUG_VERBOSE
                DEBUG_PRINT("\ttype-str: SUBSTR\n");
                DEBUG_LOG("parser-nested", "calling Parser_ParseBuiltinFunction_Substr");
                #endif
                parser_result = Parser_ParseBuiltinFunction_Substr(input, ilist, symtable);
                if (parser_result != NO_ERROR)
                {
                    DEBUG_ERR("parser-nested", "called parser function returned error code");
                    DEBUG_PRINT("\tcode: %i\n", parser_result);
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
                DEBUG_PRINT("\ttype-str: DIM\n");
                DEBUG_LOG("parser-nested", "calling Parser_ParseVariableDeclaration");
                #endif
                parser_result = Parser_ParseVariableDeclaration(input, ilist, symtable, nlist);
                if (parser_result != NO_ERROR)
                {
                    DEBUG_ERR("parser-nested", "called parser function returned error code");
                    DEBUG_PRINT("\tcode: %i\n", parser_result);
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
                DEBUG_PRINT("\ttype-str: IDENTIFIER\n");
                DEBUG_LOG("parser-nested", "calling Parser_ParseVariableDefinition");
                #endif
                parser_result = Parser_ParseVariableDefinition(input, ilist, symtable, nlist);
                if (parser_result != NO_ERROR)
                {
                    DEBUG_ERR("parser-nested", "called parser function returned error code");
                    DEBUG_PRINT("\tcode: %i\n", parser_result);
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
                DEBUG_PRINT("\ttype-str: IF\n");
                DEBUG_LOG("parser-nested", "calling Parser_ParseCondition");
                #endif
                parser_result = Parser_ParseCondition(input, ilist, symtable, nlist);
                if (parser_result != NO_ERROR)
                {
                    DEBUG_ERR("parser-nested", "called parser function returned error code");
                    DEBUG_PRINT("\tcode: %i\n", parser_result);
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
                DEBUG_PRINT("\ttype-str: DO\n");
                DEBUG_LOG("parser-nested", "calling Parser_ParseLoop_Do");
                #endif
                parser_result = Parser_ParseLoop_Do(input, ilist, symtable, nlist);
                if (parser_result != NO_ERROR)
                {
                    DEBUG_ERR("parser-nested", "called parser function returned error code");
                    DEBUG_PRINT("\tcode: %i\n", parser_result);
                    return parser_result;
                }
                break;
            }

            case FOR:
            {
                //  Jedná se o posloupnost pro použití příkazu DO WHILE
                #ifdef DEBUG_VERBOSE
                DEBUG_PRINT("\ttype-str: FOR\n");
                DEBUG_LOG("parser-nested", "calling Parser_ParseLoop_For");
                #endif
                parser_result = Parser_ParseLoop_For(input, ilist, symtable, nlist);
                if (parser_result != NO_ERROR)
                {
                    DEBUG_ERR("parser-nested", "called parser function returned error code");
                    DEBUG_PRINT("\tcode: %i\n", parser_result);
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
                DEBUG_PRINT("\ttype-str: PRINT\n");
                DEBUG_LOG("parser-nested", "calling Parser_ParseStatement_Print");
                #endif
                parser_result = Parser_ParseStatement_Print(input, ilist, symtable);
                if (parser_result != NO_ERROR)
                {
                    DEBUG_ERR("parser-nested", "called parser function returned error code");
                    DEBUG_PRINT("\tcode: %i\n", parser_result);
                    return parser_result;
                }
                break;
            }

            case INPUT:
            {
                //  Jedná se o posloupnost pro použití příkazu INPUT
                #ifdef DEBUG_VERBOSE
                DEBUG_PRINT("\ttype-str: INPUT\n");
                DEBUG_LOG("parser-nested", "calling Parser_ParseStatement_Input");
                #endif
                parser_result = Parser_ParseStatement_Input(input, ilist, symtable);
                if (parser_result != NO_ERROR)
                {
                    DEBUG_ERR("parser-nested", "called parser function returned error code");
                    DEBUG_PRINT("\tcode: %i\n", parser_result);
                    return parser_result;
                }
                break;
            }

            case CONTINUE:
            {
                //  Jedná se o posloupnost pro použití příkazu CONTINUE
                #ifdef DEBUG_VERBOSE
                DEBUG_PRINT("\ttype-str: CONTINUE\n");
                #endif
                if (NestingList_isNestedIn(nlist, NESTING_LOOP))
                {
                    DEBUG_ERR("parser-nested", "Continue statement not within loop!");
                    if (Parser_setError_statement(token->attr, input) != NO_ERROR)
                        return INTERNAL_ERROR;
                    return SYNTAX_ERROR;
                }
                #ifdef DEBUG_VERBOSE
                DEBUG_LOG("parser-nested", "calling Parser_ParseStatement_Continue");
                #endif
                parser_result = Parser_ParseStatement_Continue(input, ilist, symtable);
                if (parser_result != NO_ERROR)
                {
                    DEBUG_ERR("parser-nested", "called parser function returned error code");
                    DEBUG_PRINT("\tcode: %i\n", parser_result);
                    return parser_result;
                }
                break;
            }

            case EXIT:
            {
                //  Jedná se o posloupnost pro použití příkazu EXIT
                #ifdef DEBUG_VERBOSE
                DEBUG_PRINT("\ttype-str: EXIT\n");
                #endif
                if (NestingList_isNestedIn(nlist, NESTING_LOOP))
                {
                    DEBUG_ERR("parser-nested", "Exit statement not within loop!");
                    if (Parser_setError_statement(token->attr, input) != NO_ERROR)
                        return INTERNAL_ERROR;
                    return SYNTAX_ERROR;
                }
                #ifdef DEBUG_VERBOSE
                DEBUG_LOG("parser-nested", "calling Parser_ParseStatement_Exit");
                #endif
                parser_result = Parser_ParseStatement_Exit(input, ilist, symtable);
                if (parser_result != NO_ERROR)
                {
                    DEBUG_ERR("parser-nested", "called parser function returned error code");
                    DEBUG_PRINT("\tcode: %i\n", parser_result);
                    return parser_result;
                }
                break;
            }

            case RETURN:
            {
                //  Jedná se o posloupnost pro použití příkazu RETURN
                #ifdef DEBUG_VERBOSE
                DEBUG_PRINT("\ttype-str: RETURN\n");
                #endif
                if (NestingList_isNestedIn(nlist, NESTING_FUNCTION))
                {
                    DEBUG_ERR("parser-nested", "Return statement not within function!");
                    if (Parser_setError_statement(token->attr, input) != NO_ERROR)
                        return INTERNAL_ERROR;
                    return SYNTAX_ERROR;
                }
                #ifdef DEBUG_VERBOSE
                DEBUG_LOG("parser-nested", "calling Parser_ParseStatement_Return");
                #endif
                parser_result = Parser_ParseStatement_Return(input, ilist, symtable);
                if (parser_result != NO_ERROR)
                {
                    DEBUG_ERR("parser-nested", "called parser function returned error code");
                    DEBUG_PRINT("\tcode: %i\n", parser_result);
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
                DEBUG_PRINT("\ttype-str: SCOPE\n");
                DEBUG_LOG("parser-nested", "calling Parser_ParseScope (NOT NOW! SYNTAX_ERR INTENTIONALLY)");
                #endif
                return SYNTAX_ERROR;

                //  Tělo programu
                parser_result = Parser_ParseScope(input, ilist, symtable, nlist);
                if (parser_result != NO_ERROR)
                {
                    DEBUG_ERR("parser-nested", "called parser function returned error code");
                    DEBUG_PRINT("\tcode: %i\n", parser_result);
                    return parser_result;
                }
                break;
            }

            case END:
            {
                //  Ukončení vnoření
                #ifdef DEBUG_VERBOSE
                DEBUG_PRINT("\ttype-str: END\n");
                DEBUG_LOG("parser-nested", "returning to parent");
                #endif
                return NO_ERROR;
                break;
            }

            case LINE_END:
            {
                //  Konec řádku, prázdný řádek
                #ifdef DEBUG_VERBOSE
                DEBUG_PRINT("\ttype-str: LINE_END\n");
                DEBUG_LOG("parser-nested", "continuing to next token");
                #endif
                continue;
                break;
            }

            case FILE_END:
            {
                //  Konec vstupních dat
                #ifdef DEBUG_VERBOSE
                DEBUG_PRINT("\ttype-str: FILE_END\n");
                #endif
            }
            default:
            {
                //  Token není nic, co bychom očekáváli
                //  jedná se tedy o syntaktickou chybu
                #ifdef DEBUG_VERBOSE
                DEBUG_PRINT("\ttype-str: _UNKNOWN_\n");
                #endif
                DEBUG_ERR("parser-nested", "this type of token was not expected!");
                DEBUG_PRINT("\tntype: %i\n", nlevel ? nlevel->type : -1);
                DEBUG_PRINT("\ttype: %i\n\tattr: %s\n", token ? token->type : -1, token ? token->attr : NULL);
                Token_destroy(&token);

                if (Parser_setError_statement(token->attr, input) != NO_ERROR)
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
    return INTERNAL_ERROR;
}

int Parser_ParseFunctionDefinition(InputPtr input, InstructionListPtr ilist, SymbolTablePtr symtable, NestingListPtr nlist)
{
    //             |->
    //  <FUNCTION> | <IDENTIFIER> <OPEN_BRACKET> {[<COMMA>] <IDENTIFIER> <AS> <INTEGER|STRING|DOUBLE|BOOLEAN>}* <CLOSE_BRACKET> <AS> <INTEGER|STRING|DOUBLE|BOOLEAN> <LINE_END>
    //  {_CODE_}*
    //  <END> <FUNCTION> <LINE_END|FILE_END>
    //

    //  Inicializace
    int scanner_result;
    int parser_result;

    TokenPtr token;
    TokenPtr last_token = NULL;

    NestingLevelPtr nlevel = NestingList_newLevel(nlist, NESTING_FUNCTION);

    //  Parsování hlavních tokenů funkce


    //  Parsování vnitřního kódu funkce
    parser_result = Parser_ParseNestedCode(input, ilist, symtable, nlist, &last_token);
    if (parser_result != NO_ERROR)
    {
        if (last_token != NULL)
            Token_destroy(&last_token);
        return parser_result;
    }


    //  Parsování ukončovacích tokenů funkce
    if (last_token->type == END)
    {

    }
    else
    {
        //  TODO: Error, invalid token

        DEBUG_ERR("parser-function-def", "this type of token was not expected!");
        DEBUG_PRINT("\tntype: %i\n", nlevel ? nlevel->type : -1);
        DEBUG_PRINT("\ttype: %i\n\tattr: %s\n", last_token ? last_token->type : -1, last_token ? last_token->attr : NULL);
        if (last_token != NULL)
            Token_destroy(&last_token);

        if (Parser_setError_statement(token->attr, input) != NO_ERROR)
            return INTERNAL_ERROR;
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
    //  <DIM> | <IDENTIFIER> <AS> <INTEGER|STRING|DOUBLE|BOOLEAN>
    //
    return INTERNAL_ERROR;
}

int Parser_ParseVariableDefinition(InputPtr input, InstructionListPtr ilist, SymbolTablePtr symtable, NestingListPtr nlist)
{
    //                    |->
    //  <IDENTIFIER> <EQ> | _EXPRESSION_ <LINE_END>
    //
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
    return INTERNAL_ERROR;
}

int Parser_ParseLoop_For(InputPtr input, InstructionListPtr ilist, SymbolTablePtr symtable, NestingListPtr nlist)
{
    return INTERNAL_ERROR;
}


//-------------------------------------------------d-d-
//  Statementy
//-----------------------------------------------------

int Parser_ParseStatement_Print(InputPtr input, InstructionListPtr ilist, SymbolTablePtr symtable)
{
    //          |->
    //  <PRINT> | {<IDENTIFIER> <SEMICOLON>}* <LINE_END>
    //
    return INTERNAL_ERROR;
}

int Parser_ParseStatement_Input(InputPtr input, InstructionListPtr ilist, SymbolTablePtr symtable)
{
    //          |->
    //  <INPUT> | <IDENTIFIER> <LINE_END>
    //
    return INTERNAL_ERROR;
}

int Parser_ParseStatement_Continue(InputPtr input, InstructionListPtr ilist, SymbolTablePtr symtable)
{
    return INTERNAL_ERROR;
}

int Parser_ParseStatement_Exit(InputPtr input, InstructionListPtr ilist, SymbolTablePtr symtable)
{
    return INTERNAL_ERROR;
}

int Parser_ParseStatement_Return(InputPtr input, InstructionListPtr ilist, SymbolTablePtr symtable)
{
    //           |->
    //  <RETURN> | _EXPRESSION_ <LINE_END>
    //
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
    return INTERNAL_ERROR;
}

int Parser_ParseBuiltinFunction_Chr(InputPtr input, InstructionListPtr ilist, SymbolTablePtr symtable)
{
    //        |->
    //  <CHR> | <OPEN_BRACKET> <IDENTIFIER:integer> <CLOSE_BRACKET> <LINE_END>
    //
    return INTERNAL_ERROR;
}

int Parser_ParseBuiltinFunction_Length(InputPtr input, InstructionListPtr ilist, SymbolTablePtr symtable)
{
    //           |->
    //  <LENGTH> | <OPEN_BRACKET> <IDENTIFIER:string> <CLOSE_BRACKET> <LINE_END>
    //
    return INTERNAL_ERROR;
}

int Parser_ParseBuiltinFunction_Substr(InputPtr input, InstructionListPtr ilist, SymbolTablePtr symtable)
{
    //        |->
    //  <ASC> | <OPEN_BRACKET> <IDENTIFIER:string> <COMMA> <IDENTIFIER:integer> <COMMA> <IDENTIFIER:integer> <CLOSE_BRACKET> <LINE_END>
    //
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

    int scanner_result;
    int parser_result;

    TokenPtr token;
    TokenPtr last_token = NULL;

    //-------------------------------------------------d-d-
    //  Úvodní tokeny
    //-----------------------------------------------------
    NestingLevelPtr nlevel = NestingList_newLevel(nlist, NESTING_SCOPE);

    scanner_result = Parser_getToken("parser-scope", input, nlevel, LINE_END, &token);
    if (scanner_result != NO_ERROR)
    {
        return scanner_result;
    }

    //-------------------------------------------------d-d-
    //  Zanořené instrukce
    //-----------------------------------------------------
    Instruction_label(ilist, "main");

    DEBUG_LOG("parser-scope", "calling Parser_ParseNestedCode");
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

    scanner_result = Parser_getTokens("parser-scope", input, nlevel, closing_tokens, NULL);
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

    int scanner_result = Parser_getToken(source, nlevel, expected_type, &token);

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
            TokenList_insert(tokens, token);
        }
        else
        {
            Token_destroy(&token);
        }
        scanner_result = Parser_getToken(source, nlevel, expected_type, &token);
    }
    while(scanner_result == NO_ERROR);

    return scanner_result;
}

int Parser_getToken(char *source, InputPtr input, NestingLevelPtr nlevel, TokenType expected_type, TokenPtr *token)
{
    #ifdef DEBUG_VERBOSE
    DEBUG_LOG(source, "requesting new token");
    #endif

    TokenPtr t = *token;

    int scanner_result = Scanner_GetToken(input, &t);

    #ifdef DEBUG_VERBOSE
    DEBUG_LOG(source, "successfully received token");
    DEBUG_PRINT("\ttype: %i\n\tattr: %s\n", t->type, t->attr);
    #endif

    if (t->type != expected_type)
    {
        DEBUG_ERR(source, "this type of token was not expected!");
        #ifdef DEBUG_VERBOSE
        DEBUG_PRINT("\tntype: %i\n", nlevel ? nlevel->type : -1);
        DEBUG_PRINT("\ttype: %i\n\tattr: %s\n", t ? t->type : -1, t ? t->attr : NULL);
        #endif

        if (Parser_setError_statement(t->attr, input) != NO_ERROR)
            return INTERNAL_ERROR;

        *token = NULL;
        Token_destroy(&t);
        return SYNTAX_ERROR;
    }
    else if (scanner_result != NO_ERROR)
    {

    }
    else;
    {
        #ifdef DEBUG_VERBOSE
        DEBUG_PRINT("\ttype-str: %i (OK!)\n", expected_type);
        #endif
    }

    return scanner_result;
}

int Parser_setError_allocation()
{
    error_description = "Failed to allocate memory.";
    return INTERNAL_ERROR;
}

int Parser_setError_statement(char *instruction, InputPtr input)
{
    char *message = String_printf("Unexpected statement on line %i:%i. Expected 'EOL' got '%s'.", input ? input->line : 0, input ? input->character - strlen(instruction) : 0, instruction, NULL);
    if (message == NULL)
    {
        return Parser_setError_allocation();
    }

    error_description = message;
    return NO_ERROR;
}

#endif
