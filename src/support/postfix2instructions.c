/**
 * Tento soubor obsahuje implementaci zpracování postfixových
 * výrazů (generování instrukcí).
 *
 * @author Daniel Dolejška (xdolej08)
 * @date 21.11.2017
 * @project IFJcode17Parser
 * @subject Formální jazyky a překladače (IFJ) - FIT VUT v Brně
 */

#include <stdio.h>

#include "postfix2instructions.h"
#include "postfix_list.h"
#include "symbol_stack.h"
#include "error_codes.h"
#include "strings.h"

#ifndef _postfix2instructions_c
#define _postfix2instructions_c

#ifdef DEBUG_INCLUDE
#include "../scanner/scanner.h"
#include "../parser/parser.h"
#include "../generator/generator.h"
#include "../generator/instruction_list.h"
#else
#include "scanner.h"
#include "parser.h"
#include "generator.h"
#include "instruction_list.h"
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

//==================================================================d=d=
//  DEKLARACE A DEFINICE ENUMERÁTORŮ A STRUKTUR
//======================================================================


//==================================================================d=d=
//  DEKLARACE FUNKCÍ
//======================================================================

int postfix2instructions(InputPtr input, InstructionListPtr ilist, SymbolTablePtr symtable, PostfixListPtr *postfixList, SymbolType result_dt, SymbolType *actual_dt)
{
    char *source = "post2inst";
    DEBUG_LOG(source, "converting postfix expression to instructions");
    PostfixList_debugPrint(*postfixList);

    int result = NO_ERROR;
    int parser_result;
    int instruction_result;

    SymbolStackPtr s = SymbolStack_create();

    PostfixList_first(*postfixList);
    PostfixListItemPtr operand = PostfixList_get(*postfixList);

    InstructionListPtr preprocess_ilist = InstructionList_create();

    DEBUG_LOG(source, "preprocessing expression");
    //  Preprocess instrukcí do speciálního listu instrukcí
    do
    {
        if (operand->isOperator)
        {
            //  Operand je operátorem
            TokenPtr token = operand->token;
            //
            //  MATEMATIKA:
            //
            if (token->type == PLUS)
            {
                //  Sčítání
                instruction_result = Instruction_add_stack(preprocess_ilist);
                if (instruction_result != NO_ERROR)
                {
                    return instruction_result;
                }

                InstructionPtr i = InstructionList_getLast(preprocess_ilist);
                i->isOperator = true;
            }
            else if (token->type == MINUS)
            {
                //  Odčítání
                instruction_result = Instruction_sub_stack(preprocess_ilist);
                if (instruction_result != NO_ERROR)
                {
                    return instruction_result;
                }

                InstructionPtr i = InstructionList_getLast(preprocess_ilist);
                i->isOperator = true;
            }
            else if (token->type == STAR)
            {
                //  Násobení
                instruction_result = Instruction_mul_stack(preprocess_ilist);
                if (instruction_result != NO_ERROR)
                {
                    return instruction_result;
                }

                InstructionPtr i = InstructionList_getLast(preprocess_ilist);
                i->isOperator = true;
            }
            else if (token->type == SLASH)
            {
                //  Dělení
                instruction_result = Instruction_div_stack(preprocess_ilist);
                if (instruction_result != NO_ERROR)
                {
                    return instruction_result;
                }

                InstructionPtr i = InstructionList_getLast(preprocess_ilist);
                i->isOperator = true;
            }
            else if (token->type == BACK_SLASH)
            {
                //  Celočíselné dělení
                instruction_result = Instruction_div_stack(preprocess_ilist);
                if (instruction_result != NO_ERROR)
                {
                    return instruction_result;
                }

                InstructionPtr i = InstructionList_getLast(preprocess_ilist);
                i->isOperator = true;
            }
            //
            //  LOGIKA:
            //
            else if (token->type == EQ)
            {
                //  Porovnání (equals)
                Instruction_logic_eq_stack(preprocess_ilist);

                InstructionPtr i = InstructionList_getLast(preprocess_ilist);
                i->isOperator = true;
            }
            else if (token->type == LTGT)
            {
                //  Porovnání (not equals)
                Instruction_logic_eq_stack(preprocess_ilist);
                Instruction_logic_not_stack(preprocess_ilist);

                InstructionPtr i = InstructionList_getLast(preprocess_ilist);
                i->isOperator = true;
            }
            else if (token->type == LT)
            {
                //  Porovnání (less than)
                Instruction_logic_lt_stack(preprocess_ilist);

                InstructionPtr i = InstructionList_getLast(preprocess_ilist);
                i->isOperator = true;
            }
            else if (token->type == GT)
            {
                //  Porovnání (greater than)
                Instruction_logic_gt_stack(preprocess_ilist);

                InstructionPtr i = InstructionList_getLast(preprocess_ilist);
                i->isOperator = true;
            }
            else if (token->type == AND)
            {
                //  Konjunkce
                Instruction_logic_and_stack(preprocess_ilist);

                InstructionPtr i = InstructionList_getLast(preprocess_ilist);
                i->isOperator = true;
            }
            else if (token->type == OR)
            {
                //  Disjunkce
                Instruction_logic_or_stack(preprocess_ilist);

                InstructionPtr i = InstructionList_getLast(preprocess_ilist);
                i->isOperator = true;
            }
            else if (token->type == NOT)
            {
                //  Negace
                Instruction_logic_not_stack(preprocess_ilist);

                InstructionPtr i = InstructionList_getLast(preprocess_ilist);
                i->isOperator = true;
            }
            else
            {
                //  Neočekávaný token
                SymbolStack_destroy(&s);

                DEBUG_ERR("post2instr-pre", "this type of token was not expected!");
                Token_debugPrint(token);
                return INTERNAL_ERROR;
            }
        }
        else
        {
            //  Operand je identifikátorem/konstantou
            InstructionPtr i;
            if (operand->symbol->type == ST_FUNCTION)
            {
                DEBUG_LOG(source, "operand is function identifier");
                DEBUG_LOG(source, "returning saved function params to scanner");
                //  Parametry funkce byly uloženy na stack do speciálního pole symbolu,
                //  je nutné je vrátit scanneru, protože volaná funkce si je bude ze scanneru tahat
                TokenStackPtr tStack = (TokenStackPtr) operand->info;
                TokenPtr t;
                if (TokenStack_isEmpty(tStack) == true)
                {
                    DEBUG_ERR(source, "nothing to return, token stack is empty!");
                }
                while (TokenStack_isEmpty(tStack) == false)
                {
                    t = TokenStack_top(tStack);
                    Scanner_UngetToken(NULL, &t);
                    TokenStack_pop(tStack);
                }

                //  Poslední instrukce před zpracováním volání funkce
                i = InstructionList_getLast(preprocess_ilist);

                DEBUG_LOG(source, "calling Parser_ParseFunctionCall");
                //  Parametry funkce byly vráceny scanneru,
                //  můžeme zavolat funkci parseru, která je zpracuje
                parser_result = Parser_ParseFunctionCall(input, preprocess_ilist, symtable, operand->symbol);
                if (parser_result != NO_ERROR)
                {
                    return parser_result;
                }
                DEBUG_LOG(source, "return from Parser_ParseFunctionCall");

                if (i == NULL)
                {
                    //  Před těmito instrukcemi nebyly v seznamu žádné instrukce
                    i = InstructionList_getFirst(preprocess_ilist);
                }
                else
                {
                    //  Před těmito instrukcemi byly instrukce, zvolí se první vložená instrukce
                    //  po poslední před voláním zpracování funkce
                    i = i->next;
                }
                SymbolInfo_FunctionPtr func_info = (SymbolInfo_FunctionPtr) operand->symbol->value;
                i->isVariable   = true;
                i->isBlockBegin = true;
                i->dataType     = func_info->dataType;

                i = InstructionList_getLast(preprocess_ilist);
                i->isBlockEnd = true;

                DEBUG_LOG(source, "function in expression successfully parsed");
                InstructionList_debugPrint(preprocess_ilist);
            }
            else
            {
                instruction_result = Instruction_stack_push(preprocess_ilist, operand->symbol);
                if (instruction_result != NO_ERROR)
                {
                    return instruction_result;
                }

                i = InstructionList_getLast(preprocess_ilist);
                i->isVariable = true;
                i->dataType   = operand->symbol->type;
            }
        }

        //  Načtení dalšího operandu
        operand = PostfixList_getNext(*postfixList);
    }
    while (operand != NULL);

    DEBUG_LOG(source, "preprocessing completed");
    InstructionList_debugPrint(preprocess_ilist);
    if (SymbolStack_isEmpty(s) == false)
    {
        //  Na stacku nějaké proměnné zůstaly, jedná se o syntaktickou chybu ve výrazu
        DEBUG_ERR(source, "symbol stack is not empty, probably invalid expression");
        return SYNTAX_ERROR;
    }
    SymbolStack_destroy(&s);

    DEBUG_LOG(source, "calling postfix2instructions_process");
    //  Zpracování preprocessed výrazu
    instruction_result = postfix2instructions_process(ilist, preprocess_ilist, postfixList, actual_dt);
    if (instruction_result != NO_ERROR
        && instruction_result != SEMANTICAL_DATATYPE_ERROR)
    {
        InstructionList_destroy(&preprocess_ilist);

        DEBUG_LOG(source, "expression failed to be processed");
        return instruction_result;
    }

    InstructionList_destroy(&preprocess_ilist);
    if (instruction_result == SEMANTICAL_DATATYPE_ERROR)
    {
        DEBUG_LOG(source, "expression processed successfully but implicit conversions were used");
        result = NO_ERROR;
    }
    else
    {
        DEBUG_LOG(source, "expression processed successfully");
    }

    if ((int) result_dt != NO_REQUIRED_TYPE && result_dt != *actual_dt)
    {
        DEBUG_LOG(source, "actual result dt doesnt match with expected result dt");
        DEBUG_PRINT("\texpected: %s (%i)\n\tactual: %s (%i)\n", SymbolType_toString(result_dt), result_dt, SymbolType_toString(*actual_dt), *actual_dt);
        /*
        if (SymbolType_canBeConvertedTo(result_dt, *actual_dt) == false)
        {
        }*/

        DEBUG_ERR(source, "actual result dt will be converted to expected dt");
        if (result_dt == ST_INTEGER && *actual_dt == ST_DOUBLE)
        {
            result = Instruction_float2int_stack(ilist);
            if (result != NO_ERROR)
            {
                return result;
            }
        }
        else if (result_dt == ST_DOUBLE && *actual_dt == ST_INTEGER)
        {
            result = Instruction_int2float_stack(ilist);
            if (result != NO_ERROR)
            {
                return result;
            }
        }
        else
        {
            DEBUG_ERR(source, "actual result dt cannot be implicitly converted to expected dt");
            return SEMANTICAL_DATATYPE_ERROR;
        }

        result = NO_ERROR;
    }

    return result;
}

int postfix2instructions_process(InstructionListPtr ilist, InstructionListPtr preprocessed_ilist, PostfixListPtr *postfixList, SymbolType *result_dt)
{
    char *source = "post2inst-process";

    int result = NO_ERROR;

    //  Nastavíme se na začátek seznamu instrukcí
    InstructionList_first(preprocessed_ilist);
    InstructionPtr i = InstructionList_getActive(preprocessed_ilist);
    InstructionPtr iX = NULL;
    InstructionPtr iX_end = NULL;
    InstructionPtr iY = NULL;
    InstructionPtr iY_end = NULL;

    //  Nastavíme se na začátek postfixového výrazu
    PostfixList_first(*postfixList);
    PostfixListItemPtr operand = PostfixList_get(*postfixList);

    DEBUG_LOG(source, "starting real expression processing");
    do
    {
        if (operand->isOperator)
        {
            //  Operand je operátorem
            TokenPtr token = operand->token;

            #ifdef DEBUG_VERBOSE
            //DEBUG_LOG(source, "searching operands");
            //DEBUG_LOG(source, "searching Y (second operator)");
            #endif

            iY = i->prev;
            iY_end = NULL;
            if (iY->isBlockEnd == true)
            {
                iY_end = iY;
                while (iY->isBlockBegin != true)
                {
                    iY = iY->prev;
                    if (iY == NULL)
                    {
                        //  Or syntax err?
                        return INTERNAL_ERROR;
                    }
                }
            }

            #ifdef DEBUG_VERBOSE
            //DEBUG_LOG(source, "searching X (first operator)");
            #endif

            //  Operand 2 této operace (předchozí blok předchozího bloku instrukcí)
            iX = iY->prev;
            iX_end = NULL;
            if (iX->isBlockEnd == true)
            {
                iX_end = iX;
                while (iX->isBlockBegin != true)
                {
                    iX = iX->prev;
                    if (iX == NULL)
                    {
                        //  Or syntax err?
                        return INTERNAL_ERROR;
                    }
                }
            }

            #ifdef DEBUG_VERBOSE
            DEBUG_LOG(source, "validating operation");
            DEBUG_PRINT("\toperator: %s,\n\tx = %p\n\tdt x = %s,\n\ty = %p\n\tdt y = %s\n", TokenType_toString(token->type), iX, SymbolType_toString(iX ? (int) iX->dataType : -1), iY, SymbolType_toString(iY ? (int) iY->dataType : -1));
            #endif

            switch (token->type)
            {
                //
                //  Binární operace
                //
                case PLUS:
                case MINUS:
                case SLASH:
                case STAR:
                case BACK_SLASH:
                case EQ:
                case LT:
                case LTGT:
                case GT:
                case AND:
                case OR:
                    if (iX == NULL || iY == NULL || SymbolType_isBinaryOperationOk(iX->dataType, iY->dataType, token) == false)
                    {
                        //  TODO: Error message
                        DEBUG_ERR(source, "these operands cannot be used in this binary operation");
                        return SEMANTICAL_DATATYPE_ERROR;
                    }

                    #ifdef DEBUG_VERBOSE
                    DEBUG_LOG(source, "this binary operation is valid, validating implicit datatype conversions for operand 1");
                    #endif

                    SymbolType resultDataType;
                    char *i_content;

                    //  Kontrola datového typu prvního operandu
                    if (SymbolType_hasToConvertOperator1(iX->dataType, iY->dataType, &resultDataType) == true)
                    {
                        if (iX->dataType == ST_DOUBLE && resultDataType == ST_INTEGER)
                        {
                            #ifdef DEBUG_VERBOSE
                            DEBUG_LOG(source, "first operand needs to be converted from float to integer");
                            #endif
                            i_content = String_create("FLOAT2INTS");
                            if (i_content == NULL)
                            {
                                return INTERNAL_ERROR;
                            }
                        }
                        else if (iX->dataType == ST_INTEGER && resultDataType == ST_DOUBLE)
                        {
                            #ifdef DEBUG_VERBOSE
                            DEBUG_LOG(source, "first operand needs to be converted from integer to float");
                            #endif
                            i_content = String_create("INT2FLOATS");
                            if (i_content == NULL)
                            {
                                return INTERNAL_ERROR;
                            }
                        }
                        else
                        {
                            DEBUG_ERR(source, "first operand needs to be converted, but we do not know how");
                            DEBUG_PRINT("\ttarget data type is %s\n", SymbolType_toString(resultDataType));
                            return INTERNAL_ERROR;
                        }
                        InstructionList_insertAfter(preprocessed_ilist, iX, i_content);
                        result = SEMANTICAL_DATATYPE_ERROR;
                    }

                    #ifdef DEBUG_VERBOSE
                    DEBUG_LOG(source, "this binary operation is valid, validating implicit datatype conversions for operand 2");
                    #endif

                    //  Kontrola datového typu druhého operandu
                    resultDataType = -1;
                    if (SymbolType_hasToConvertOperator2(iX->dataType, iY->dataType, &resultDataType) == true)
                    {
                        if (iY->dataType == ST_DOUBLE && resultDataType == ST_INTEGER)
                        {
                            #ifdef DEBUG_VERBOSE
                            DEBUG_LOG(source, "second operand needs to be converted from float to integer");
                            #endif
                            i_content = String_create("FLOAT2INTS");
                            if (i_content == NULL)
                            {
                                return INTERNAL_ERROR;
                            }
                        }
                        else if (iY->dataType == ST_INTEGER && resultDataType == ST_DOUBLE)
                        {
                            #ifdef DEBUG_VERBOSE
                            DEBUG_LOG(source, "second operand needs to be converted from integer to float");
                            #endif
                            i_content = String_create("INT2FLOATS");
                            if (i_content == NULL)
                            {
                                return INTERNAL_ERROR;
                            }
                        }
                        else
                        {
                            DEBUG_ERR(source, "second operand needs to be converted, but we do not know how");
                            DEBUG_PRINT("\ttarget data type is %s\n", SymbolType_toString(resultDataType));
                            return INTERNAL_ERROR;
                        }
                        InstructionList_insertAfter(preprocessed_ilist, iY, i_content);
                        result = SEMANTICAL_DATATYPE_ERROR;
                    }

                    //  Opravení výsledného datového typu na základě operace
                    switch (token->type)
                    {
                        case SLASH:
                            resultDataType = ST_DOUBLE;
                            break;
                        case EQ:
                        case LT:
                        case LTGT:
                        case GT:
                        case AND:
                        case OR:
                        case NOT:
                            resultDataType = ST_BOOLEAN;
                            break;
                        default:
                            break;
                    }

                    //  Vytvoření nového bloku z použitých instrukcí
                    iX->isBlockBegin = true;
                    iX->isVariable = false;
                    iX->dataType = resultDataType;
                    if (iX_end != NULL)
                        iX_end->isBlockEnd = false;

                    iY->isBlockBegin = false;
                    iY->isVariable = false;
                    if (iY_end != NULL)
                        iY_end->isBlockEnd = false;

                    i->isOperator = false;
                    i->isBlockEnd = true;

                    DEBUG_LOG(source, "operation processed");
                    break;
                //
                //  Unární operace
                //
                case NOT:
                    if (iY == NULL || SymbolType_isOperationOk(iY->dataType, token) == false)
                    {
                        //  TODO: Error message
                        DEBUG_ERR(source, "this operand cannot be used in this unary operation");
                        return SEMANTICAL_DATATYPE_ERROR;
                    }

                    #ifdef DEBUG_VERBOSE
                    DEBUG_LOG(source, "this unary operation is valid, validating implicit datatype conversions");
                    #endif

                    iY->isBlockBegin = true;
                    iY->dataType     = resultDataType;
                    i->isBlockEnd    = true;
                    if (iY_end != NULL)
                        iY_end->isBlockEnd = false;
                    break;
                default:
                    //  Neočekávaný token
                    DEBUG_ERR(source, "this type of token was not expected!");
                    Token_debugPrint(token);
                    return INTERNAL_ERROR;
            }
        }
        else
        {
            //  Operand je identifikátorem/konstantou
        }

        //  Načtení dalšího operandu
        operand = PostfixList_getNext(*postfixList);

        //  Načtení další instrukce
        i = i->next;
        while (i != NULL && i->isOperator == false && i->isVariable == false && i->isBlockBegin == false)
            //  Instrukce není ani operátor andi proměnná ani začátek bloku zpracovaného operandu
            i = i->next;

        if (operand != NULL)
        {
            DEBUG_ERR(source, "operand is not NULL but instruction is!!");
        }
    }
    while (operand != NULL && i != NULL);
    DEBUG_LOG(source, "real expression processing completed");
    InstructionList_debugPrint(preprocessed_ilist);

    DEBUG_LOG(source, "copying instructions to instruction list");
    //  Nastavíme se na začátek seznamu instrukcí
    InstructionList_first(preprocessed_ilist);
    i = InstructionList_getActive(preprocessed_ilist);
    *result_dt = i->dataType;
    while (i != NULL)
    {
        Instruction_custom(ilist, i->content);
        InstructionList_next(preprocessed_ilist);
        i = InstructionList_getActive(preprocessed_ilist);
    }

    return result;
}

#endif
