/**
 * Tento soubor obsahuje implementaci funkcí pro konverzi
 * infixových výrazů na postfixové.
 *
 * @author Daniel Dolejška (xdolej08)
 * @date 21.11.2017
 * @project IFJcode17Parser
 * @subject Formální jazyky a překladače (IFJ) - FIT VUT v Brně
 */

#include <stdio.h>
#include <stdbool.h>

#include "infix2postfix.h"
#include "symbol_stack.h"
#include "token_stack.h"
#include "error_codes.h"

#ifndef _infix2postfix_c
#define _infix2postfix_c

#ifdef DEBUG_INCLUDE
#else
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

int untilLeftPar(TokenStackPtr s, PostfixListPtr postfixList)
{
    TokenPtr stack_token;
    int      stack_result;

    while(TokenStack_isEmpty(s) == false)
    {
        stack_token  = TokenStack_top(s);
        stack_result = TokenStack_pop(s);
        if (stack_result != NO_ERROR)
        {
            return INTERNAL_ERROR;
        }

        if (stack_token->type == OPEN_BRACKET)
        {
            break;
        }
        PostfixList_insertOperator(postfixList, stack_token);
    }

    return NO_ERROR;
}

int doOperation(TokenStackPtr s, PostfixListPtr postfixList, TokenPtr token)
{
    //	Kontrola typu operátoru a jeho další zpracování
    if (token == NULL)
    {
        //	Hack pro jednoduché vyprázdnění zásobníku
        return untilLeftPar(s, postfixList);
    }
    else if (token->type == OPEN_BRACKET)
    {
        //	Jedná se o otevírací závorku, provedeme uložení na zásobník
        return TokenStack_push(s, token);
    }
    else if (token->type == CLOSE_BRACKET)
    {
        //	Jedná se o zavírací závorku, provedeme volání funkce pro
        // 	zpracování této situace
        return untilLeftPar(s, postfixList);
    }
    else
    {
        //	Jedná se o některý z operátorů, provedeme jeho zpracování
        TokenPtr stack_token;
        int      stack_result;
        int      stack_priority;

        int token_priority = getTokenPriority(token);

        while(TokenStack_isEmpty(s) == false)
        {
            //	Získání operátoru na vrcholu zásobníku
            stack_token = TokenStack_top(s);

            //	Přiřazení priorit daným operátorům
            stack_priority = getTokenPriority(stack_token);

            //	Pokud je operátorem na vrcholu zásobníku otevírací závorka
            //	pak není problém pokračovat
            if (stack_priority == -1)
            {
                break;
            }
            //	Pokud je operátorem na vrcholu zásobníku operátor stejné
            //	nebo vyšší priority, je třeba jej přepsat do řetězce
            else if (stack_priority >= token_priority)
            {
                //	Operátor z vrcholu zásobníku musí být vypsán
                PostfixList_insertOperator(postfixList, stack_token);
            }
            else
            {
                //  Operátor na vrcholu zásobníku má nižší prioritu, než
                //  náš operátor, můžeme jej dát na stack
                break;
            }

            stack_result = TokenStack_pop(s);
            if (stack_result != NO_ERROR)
            {
                return INTERNAL_ERROR;
            }
        }

        stack_result = TokenStack_push(s, token);
        if (stack_result != NO_ERROR)
        {
            return INTERNAL_ERROR;
        }
    }

    return NO_ERROR;
}

int getTokenPriority(TokenPtr token)
{
    switch (token->type)
    {
        //-------------------------------------------------d-d-
        //  Matematické
        //-----------------------------------------------------
        case STAR:
        case SLASH:
            return 8;

        case BACK_SLASH:
            return 6;

        case PLUS:
        case MINUS:
            return 4;

        //-------------------------------------------------d-d-
        //  Logické
        //-----------------------------------------------------
        case LT:    //  less than
        case LTEQ:  //  less than or equal
        case LTGT:  //  not equal
        case GT:    //  greater than
        case GTEQ:  //  greater than or equal
        case EQ:    //  equal
            return 2;

        case AND:
        case OR:
        case NOT:
            return 1;

        //-------------------------------------------------d-d-
        //  Obecné
        //-----------------------------------------------------
        case OPEN_BRACKET:
            return -1;

        default:
            return INTERNAL_ERROR;
    }
}

int infix2postfix_init(TokenStackPtr *s, PostfixListPtr *postfixList)
{
    //	Alokace paměti pro zásobník operátorů
    *s = TokenStack_create();
    if (*s == NULL)
    {
        return INTERNAL_ERROR;
    }

    //	Inicializace pomocných proměnných
    *postfixList = PostfixList_create();
    if (*postfixList == NULL)
    {
        return INTERNAL_ERROR;
    }

    return NO_ERROR;
}

int infix2postfix_addOperand(TokenStackPtr *s, PostfixListPtr *postfixList, TokenPtr token, SymbolPtr symbol)
{
    if (token == NULL)
    {
        DEBUG_ERR("inf2post-addOperand", "token is NULL!");
        return INTERNAL_ERROR;
    }

    if (token->type == IDENTIFIER)
    {
        if (symbol == NULL)
        {
            DEBUG_ERR("inf2post-addOperand", "token type is IDENTIFIER, but symbol is NULL!");
            return INTERNAL_ERROR;
        }
        return PostfixList_insertSymbol(*postfixList, symbol);
    }
    else
    {
        if (token->type == PLUS || token->type == MINUS || token->type == STAR
            || token->type == SLASH || token->type == BACK_SLASH || token->type == OPEN_BRACKET
            || token->type == CLOSE_BRACKET)
        {
            //	Znak je operátorem
            return doOperation(*s, *postfixList, token);
        }
        else
        {
            //  Znak není operátorem ani identifikátorem
            infix2postfix_cleanup(s, postfixList);
            return INTERNAL_ERROR;
        }
    }

    return NO_ERROR;
}

int infix2postfix_process(TokenStackPtr *s, PostfixListPtr *postfixList)
{
    return doOperation(*s, *postfixList, NULL);
}

void infix2postfix_cleanup(TokenStackPtr *s, PostfixListPtr *postfixList)
{
    TokenStack_destroy(s);
    PostfixList_destroy(postfixList);
}

#endif
