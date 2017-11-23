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

#ifndef _postfix2instructions_c
#define _postfix2instructions_c

#ifdef DEBUG_INCLUDE
#include "../generator/generator.h"
#include "../generator/instruction_list.h"
#else
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

//==================================================================d=d=
//  DEKLARACE A DEFINICE ENUMERÁTORŮ A STRUKTUR
//======================================================================


//==================================================================d=d=
//  DEKLARACE FUNKCÍ
//======================================================================

int postfix2instructions_mathematical(InstructionListPtr ilist, PostfixListPtr *postfixList)
{
    SymbolStackPtr s = SymbolStack_create();

    PostfixList_first(*postfixList);
    PostfixListItemPtr operand = PostfixList_get(*postfixList);
    do
    {
        if (operand->isOperator)
        {
            //  Operand je operátorem
            TokenPtr token = operand->token;
            if (token->type == PLUS)
            {
                //  Sčítání
                Instruction_add_stack(ilist);
            }
            else if (token->type == MINUS)
            {
                //  Odčítání
                Instruction_sub_stack(ilist);
            }
            else if (token->type == STAR)
            {
                //  Násobení
                Instruction_mul_stack(ilist);
            }
            else if (token->type == SLASH)
            {
                //  Dělení
                Instruction_div_stack(ilist);
            }
            else if (token->type == BACK_SLASH)
            {
                //  Celočíselné dělení
                Instruction_div_stack(ilist);
            }
            else
            {
                //  Neočekávaný token
                SymbolStack_destroy(&s);

                DEBUG_ERR("post2inst-math", "this type of Token was not expected!");
                Token_debugPrint(token);
                return INTERNAL_ERROR;
            }
        }
        else
        {
            //  Operand je identifikátorem/konstantou
            Instruction_stack_push(ilist, operand->symbol);
        }

        //  Načtení dalšího operandu
        operand = PostfixList_getNext(*postfixList);
    }
    while (operand != NULL);

    SymbolStack_destroy(&s);
    return NO_ERROR;
}

int postfix2instructions_logical(InstructionListPtr ilist, PostfixListPtr *postfixList)
{
    SymbolStackPtr s = SymbolStack_create();

    PostfixList_first(*postfixList);
    PostfixListItemPtr operand = PostfixList_get(*postfixList);
    do
    {
        if (operand->isOperator)
        {
            //  Operand je operátorem
            TokenPtr token = operand->token;
            if (token->type == EQ)
            {
                //  Porovnání (equals)
                Instruction_logic_eq_stack(ilist);
            }
            else if (token->type == LTGT)
            {
                //  Porovnání (not equals)
                Instruction_logic_not_stack(ilist);
                Instruction_logic_eq_stack(ilist);
            }
            else if (token->type == LT)
            {
                //  Porovnání (less than)
                Instruction_logic_lt_stack(ilist);
            }
            else if (token->type == GT)
            {
                //  Porovnání (greater than)
                Instruction_logic_gt_stack(ilist);
            }
            else if (token->type == AND)
            {
                //  Konjunkce
                Instruction_logic_and_stack(ilist);
            }
            else if (token->type == OR)
            {
                //  Disjunkce
                Instruction_logic_or_stack(ilist);
            }
            else if (token->type == NOT)
            {
                //  Negace
                Instruction_logic_not_stack(ilist);
            }
            else
            {
                //  Neočekávaný token
                SymbolStack_destroy(&s);

                DEBUG_ERR("post2inst-logic", "this type of Token was not expected!");
                Token_debugPrint(token);
                return INTERNAL_ERROR;
            }
        }
        else
        {
            //  Operand je identifikátorem/konstantou
            Instruction_stack_push(ilist, operand->symbol);
        }

        //  Načtení dalšího operandu
        operand = PostfixList_getNext(*postfixList);
    }
    while (operand != NULL);

    SymbolStack_destroy(&s);
    return NO_ERROR;
}

#endif
