/**
 * Lexikální analyzátor (scanner).
 *
 * @author Petr Ullrich (xullri00)
 * @project IFJcode17Parser
 */

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#ifdef DEBUG_INCLUDE
#include "../support/error_codes.h"
#include "../support/strings.h"
#include "../support/token_stack.h"
#else
#include "error_codes.h"
#include "strings.h"
#include "token_stack.h"
#endif // DEBUG_INCLUDE




#include "input.h"
#include "scanner.h"
#include "token.h"


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

extern char *last_line;

TokenStackPtr token_stack = NULL;
bool charReturned = false;

//==================================================================d=d=
//  DEKLARACE FUNKCÍ
//======================================================================

/**
 * Hlavní funkce ovládající lexikální analyzátor.
 *
 * Pokud se na stacku tokenů nachází nějaký vrácený token,
 * je vrácen dříve, než je započato získávání tokenu nového.
 *
 * Pokud nebude token z nějakého důvodu získán, v parametru token vrací
 * hodnotu NULL a dále vrací adekvátní stavový kód.
 *
 * @param[in,out]   InputPtr    input           Ukazatel na strukturu se vstupními daty
 * @param[out]      TokenPtr    *token          Ukazatel na získaný token
 *
 * @retval int  Kód se kterým bylo získávání nového tokenu ukončeno
 */
int Scanner_GetToken(InputPtr input, TokenPtr *token)
{
    if (token_stack == NULL)
    {
        token_stack = TokenStack_create();
    }
    else if (TokenStack_isEmpty(token_stack) == false)
    {
        DEBUG_LOG("scanner", "returning token from stack, rather than creating new!");
        *token = TokenStack_top(token_stack);
        if (TokenStack_pop(token_stack) != NO_ERROR)
        {
            return INTERNAL_ERROR;
        }
        return NO_ERROR;
    }

    AutomataState state = STATE_BEGIN;
    char* final_string = String_create(NULL);

    //pomocne promenne pro kontrolu, zda escape sekvence splnuje interval <001,255>
    char esc1 = '1';
    char esc2 = '1';

    //pomocne promenne pro vynechavani nul na zacatku cisla
    char first_number;
    char first_exp_number;

    while(1)
    {
        char ch = fgetc(input->source);
        if (charReturned == false)
        {
            //  Posledni znak nebyl vracen
            String_addChar(&last_line, ch);
        }
        else
        {
            //  Posledni znak byl vracen
            charReturned = false;
        }

        input->character++;

        switch(state)
        {
        case STATE_BEGIN:
            if(isalpha(ch) || ch == '_')
            {
                state = STATE_IDENTIFIER;

                if(isupper(ch))
                {
                    ch = ch + 32;
                }
                String_addChar(&final_string, ch);
            }
            else if(ch == ' ' || ch == '\t')
            {

            }
            else if(isdigit(ch))
            {
                first_number = ch;
                state = STATE_INTEGER;
                if(first_number != '0')
                {
                    String_addChar(&final_string, ch);
                }
            }
            else if(ch == '!')
            {
                state = STATE_INC_EXC;
            }
            else if(ch == '/')
            {
                state = STATE_DIV;
                String_addChar(&final_string, ch);
            }
            else if(ch == '&')
            {
                state = STATE_INC_AMPERSAND;
            }
            else if(ch == '\'')
            {
                state = STATE_INC_COMMENT;
            }
            else if(ch == '*')
            {
                state = STATE_MUL;
                String_addChar(&final_string, ch);
            }
            else if(ch == '+')
            {
                state = STATE_ADD;
                String_addChar(&final_string, ch);
            }
            else if(ch == '-')
            {
                state = STATE_SUB;
                String_addChar(&final_string, ch);
            }
            else if(ch == '(')
            {
                String_addChar(&final_string, ch);
                *token = Token_create(OPEN_BRACKET, final_string);
                if(*token == NULL)
                {
                    return INTERNAL_ERROR;
                }
                return NO_ERROR;
            }
            else if(ch == ')')
            {
                String_addChar(&final_string, ch);
                *token = Token_create(CLOSE_BRACKET, final_string);
                if(*token == NULL)
                {
                    return INTERNAL_ERROR;
                }
                return NO_ERROR;
            }
            else if(ch == ';')
            {
                String_addChar(&final_string, ch);
                *token = Token_create(SEMICOLON, final_string);
                if(*token == NULL)
                {
                    return INTERNAL_ERROR;
                }
                return NO_ERROR;
            }
            else if(ch == ',')
            {
                String_addChar(&final_string, ch);
                *token = Token_create(COMMA, final_string);
                if(*token == NULL)
                {
                    return INTERNAL_ERROR;
                }
                return NO_ERROR;
            }
            else if(ch == '<')
            {
                state = STATE_LESS;
                String_addChar(&final_string, ch);
            }
            else if(ch == '>')
            {
                state = STATE_GREATER;
                String_addChar(&final_string, ch);
            }
            else if(ch == '=')
            {
                String_addChar(&final_string, ch);
                *token = Token_create(EQ, final_string);
                if(*token == NULL)
                {
                    return INTERNAL_ERROR;
                }
                return NO_ERROR;
            }
            else if(ch == '\\')
            {
                state = STATE_B_SLASH;
                String_addChar(&final_string, ch);
            }
            else if(ch == '\n')
            {
                String_destroy(&last_line);
                last_line = String_create(NULL);
                input->line++;
                input->character = 0;

                *token = Token_create(LINE_END, NULL);
                if(*token == NULL)
                {
                    return INTERNAL_ERROR;
                }
                return NO_ERROR;
            }
            else if(ch == EOF)
            {
                *token = Token_create(FILE_END, NULL);
                if(*token == NULL)
                {
                    return INTERNAL_ERROR;
                }
                return NO_ERROR;
            }
            else
            {
                String_destroy(&final_string); //neposilame final_string v tokenu -> musime uvolnit

                final_string = String_create(NULL); //alokace, za ucelem vytvoreni stringu, ktery posleme, jako parametr znaku, ktery je chybny.
                String_addChar(&final_string, ch);  //znak pridame do retezce -> budeme mit retezec s jednim znakem

                char* reason = String_printf("Unexpected character '%c' on line %i:%i.", final_string, (char *) input->line, (char *) input->character, NULL);
                String_destroy(&final_string); //nasledne uvolneni
                *token = Token_create(INVALID, reason);
                if(*token == NULL)
                {
                    return INTERNAL_ERROR;
                }
                return LEXICAL_ERROR;
            }
            break;

        case STATE_IDENTIFIER:
            if(isalnum(ch) || ch == '_')
            {
                if(isupper(ch))
                {
                    ch = ch + 32;
                }
                String_addChar(&final_string, ch);
            }
            else
            {
                charReturned = true;
                ungetc(ch, input->source);
                input->character--;
                //Porovnavani retezce, zda se nejedna o keyword
                if(strcmp(final_string, "as") == 0)
                {
                    *token = Token_create(AS, final_string);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "asc") == 0)
                {
                    *token = Token_create(ASC, final_string);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "declare") == 0)
                {
                    *token = Token_create(DECLARE, final_string);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "dim") == 0)
                {
                    *token = Token_create(DIM, final_string);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "do") == 0)
                {
                    *token = Token_create(DO, final_string);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "double") == 0)
                {
                    *token = Token_create(DOUBLE, final_string);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "else") == 0)
                {
                    *token = Token_create(ELSE, final_string);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "end") == 0)
                {
                    *token = Token_create(END, final_string);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "chr") == 0)
                {
                    *token = Token_create(CHR, final_string);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "function") == 0)
                {
                    *token = Token_create(FUNCTION, final_string);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "if") == 0)
                {
                    *token = Token_create(IF, final_string);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "input") == 0)
                {
                    *token = Token_create(INPUT, final_string);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "integer") == 0)
                {
                    *token = Token_create(INTEGER, final_string);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "length") == 0)
                {
                    *token = Token_create(LENGTH, final_string);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "loop") == 0)
                {
                    *token = Token_create(LOOP, final_string);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "print") == 0)
                {
                    *token = Token_create(PRINT, final_string);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "return") == 0)
                {
                    *token = Token_create(RETURN, final_string);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "scope") == 0)
                {
                    *token = Token_create(SCOPE, final_string);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "string") == 0)
                {
                    *token = Token_create(STRING, final_string);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "substr") == 0)
                {
                    *token = Token_create(SUBSTR, final_string);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "then") == 0)
                {
                    *token = Token_create(THEN, final_string);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "while") == 0)
                {
                    *token = Token_create(WHILE, final_string);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "and") == 0)
                {
                    *token = Token_create(AND, final_string);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "boolean") == 0)
                {
                    *token = Token_create(BOOLEAN, final_string);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "continue") == 0)
                {
                    *token = Token_create(CONTINUE, final_string);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "elseif") == 0)
                {
                    *token = Token_create(ELSEIF, final_string);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "exit") == 0)
                {
                    *token = Token_create(EXIT, final_string);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "false") == 0)
                {
                    *token = Token_create(FALSE, final_string);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "for") == 0)
                {
                    *token = Token_create(FOR, final_string);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "next") == 0)
                {
                    *token = Token_create(NEXT, final_string);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "not") == 0)
                {
                    *token = Token_create(NOT, final_string);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "or") == 0)
                {
                    *token = Token_create(OR, final_string);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "shared") == 0)
                {
                    *token = Token_create(SHARED, final_string);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "static") == 0)
                {
                    *token = Token_create(STATIC, final_string);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "true") == 0)
                {
                    *token = Token_create(TRUE, final_string);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                // Retezec neni keyword, tudiz je IDENTIFIER
                else
                {
                    *token = Token_create(IDENTIFIER, final_string);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
            }
            break;

        //cases for STRING begin
        case STATE_INC_EXC:
            if(ch == '"')
            {
                state = STATE_INC_STRING_BEGIN;
            }
            else
            {
                charReturned = true;
                ungetc(ch, input->source);
                input->character--;
                String_destroy(&final_string); //neposilame final_string v tokenu -> musime uvolnit

                final_string = String_create(NULL); //alokace, za ucelem vytvoreni stringu, ktery posleme, jako parametr znaku, ktery je chybny.
                String_addChar(&final_string, ch);  //znak pridame do retezce -> budeme mit retezec s jednim znakem

                char* reason = String_printf("Unexpected character '%c' on line %i:%i. Expected character \" ", final_string, (char *) input->line, (char *) input->character, NULL);
                String_destroy(&final_string); //uvolneni
                *token = Token_create(INVALID, reason);
                if(*token == NULL)
                {
                    return INTERNAL_ERROR;
                }
                return LEXICAL_ERROR;
            }
            break;

        case STATE_INC_STRING_BEGIN:
            if(ch == '"')
            {
                state = STATE_STRING;
            }
            else if(ch == '\\')
            {
                state = STATE_INC_BACKSLASH;
                String_addChar(&final_string, ch);
            }
            else if(ch == '\n' || ch == EOF)
            {
                String_destroy(&final_string); //neposilame final_string v tokenu -> musime uvolnit

                final_string = String_create(NULL); //alokace, za ucelem vytvoreni stringu, ktery posleme, jako parametr znaku, ktery je chybny.
                String_addChar(&final_string, ch);  //znak pridame do retezce -> budeme mit retezec s jednim znakem

                char* reason = String_printf("Unexpected character '%c' on line %i:%i. (EOL)", final_string, (char *) input->line, (char *) input->character, NULL);
                String_destroy(&final_string);

                *token = Token_create(INVALID, reason);
                if(*token == NULL)
                {
                    return INTERNAL_ERROR;
                }
                return LEXICAL_ERROR;
            }
            else
            {
                if(ch <= 32 || ch == 35)
                {
                    char str_num[3];
                    snprintf(str_num, 3, "%d", ch);

                    if (strlen(str_num) == 1)
                    {
                        final_string = String_concat(final_string, str_num, "\\00");
                    }
                    else
                    {
                        final_string = String_concat(final_string, str_num, "\\0");
                    }
                }
                else
                {
                    String_addChar(&final_string, ch);
                }
            }
            break;

        case STATE_INC_BACKSLASH:
            if(ch >= '0' && ch <= '2')
            {
                esc1 = ch; //dosazeni do promenne, ktera bude pro dalsi kontrolu, zda cislo odpovida intervalu <0,255>
                state = STATE_INC_ESC1;
                String_addChar(&final_string, ch);
            }
            else if(ch == '"')
            {
                final_string = String_concat(final_string, "034", NULL);
                state = STATE_INC_STRING_BEGIN;
            }
            else if(ch == 'n')
            {
                final_string = String_concat(final_string, "010", NULL);
                state = STATE_INC_STRING_BEGIN;
            }
            else if(ch == 't')
            {
                final_string = String_concat(final_string, "009", NULL);
                state = STATE_INC_STRING_BEGIN;
            }
            else if(ch == '\\')
            {
                final_string = String_concat(final_string, "092", NULL);
                state = STATE_INC_STRING_BEGIN;
            }
            else
            {
                String_destroy(&final_string); //neposilame final_string v tokenu -> musime uvolnit

                final_string = String_create(NULL); //alokace, za ucelem vytvoreni stringu, ktery posleme, jako parametr znaku, ktery je chybny.
                String_addChar(&final_string, ch);  //znak pridame do retezce -> budeme mit retezec s jednim znakem

                char* reason = String_printf("Unexpected character '%c' on line %i:%i. Expected characters: \", n, t, \\ or <0,2>", final_string, (char *) input->line, (char *) input->character, NULL);
                String_destroy(&final_string);

                *token = Token_create(INVALID, reason);
                if(*token == NULL)
                {
                    return INTERNAL_ERROR;
                }
                return LEXICAL_ERROR;
            }
            break;

        case STATE_INC_ESC1:
            if(isdigit(ch))
            {
                esc2 = ch; //dosazeni do promenne, ktera bude pro dalsi kontrolu, zda cislo odpovida intervalu <0,255>
                state = STATE_INC_ESC2;
                String_addChar(&final_string, ch);
            }
            else
            {
                String_destroy(&final_string); //neposilame final_string v tokenu -> musime uvolnit

                final_string = String_create(NULL); //alokace, za ucelem vytvoreni stringu, ktery posleme, jako parametr znaku, ktery je chybny.
                String_addChar(&final_string, ch);  //znak pridame do retezce -> budeme mit retezec s jednim znakem

                char* reason = String_printf("Unexpected character '%c' on line %i:%i. Expected characters: <0,5>", final_string, (char *) input->line, (char *) input->character, NULL);
                String_destroy(&final_string);

                *token = Token_create(INVALID, reason);
                if(*token == NULL)
                {
                    return INTERNAL_ERROR;
                }
                return LEXICAL_ERROR;
            }
            break;

        case STATE_INC_ESC2:
            if(isdigit(ch))
            {
                if((esc1 == '0' && esc2 == '0' && ch == '0') || (esc1 == '2' && esc2 > '5') || (esc1 == '2' && esc2 >= '5' && ch > '5'))
                {
                    charReturned = true;
                    ungetc(ch, input->source);
                    input->character--;
                    String_destroy(&final_string); //neposilame final_string v tokenu -> musime uvolnit

                    final_string = String_create(NULL); //alokace, za ucelem vytvoreni stringu, ktery posleme, jako parametr znaku, ktery je chybny.
                    String_addChar(&final_string, ch);  //znak pridame do retezce -> budeme mit retezec s jednim znakem

                    char* reason = String_printf("Unexpected character '%c' on line %i:%i. Expected characters: <1,5>", final_string, (char *) input->line, (char *) input->character, NULL);
                    String_destroy(&final_string);

                    *token = Token_create(INVALID, reason);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return LEXICAL_ERROR;
                }
                else
                {
                    state = STATE_INC_STRING_BEGIN;
                    String_addChar(&final_string, ch);
                }
            }
            else
            {
                first_number = '0';
                String_destroy(&final_string); //neposilame final_string v tokenu -> musime uvolnit

                final_string = String_create(NULL); //alokace, za ucelem vytvoreni stringu, ktery posleme, jako parametr znaku, ktery je chybny.
                String_addChar(&final_string, ch);  //znak pridame do retezce -> budeme mit retezec s jednim znakem


                char* reason = String_printf("Unexpected character '%c' on line %i:%i. Expected characters: <0,5>", final_string, (char *) input->line, (char *) input->character, NULL);
                String_destroy(&final_string);

                *token = Token_create(INVALID, reason);
                if(*token == NULL)
                {
                    return INTERNAL_ERROR;
                }
                return LEXICAL_ERROR;
            }
            break;

        case STATE_STRING:
            //  Došlo ke změně stavu načetl se nepotřebný znak, musíme ho vrátit
            charReturned = true;
            ungetc(ch, input->source);
            input->character--;

            *token = Token_create(CONSTANT_STRING, final_string);
            if(*token == NULL)
            {
                return INTERNAL_ERROR;
            }
            return NO_ERROR;

            break;
        //cases for STRING end

        //case for comment begin
        case STATE_INC_COMMENT:
            if(ch == '\n')
            {
                charReturned = true;
                ungetc(ch, input->source);
                input->character--;
                state = STATE_BEGIN;
            }
            else if(ch == EOF)
            {
                *token = Token_create(FILE_END, NULL);
                if(*token == NULL)
                {
                    return INTERNAL_ERROR;
                }
                return NO_ERROR;
            }
            break;
        //case for comment end

        // Cases for INTEGER and DOUBLE begin
        case STATE_INTEGER:
            if(isdigit(ch))
            {
                if(first_number == '0')
                {
                    if(ch != '0')
                    {
                        first_number = '1';
                        String_addChar(&final_string, ch);
                    }
                }
                else
                {
                    String_addChar(&final_string, ch);
                }
            }
            else if(ch == '.')
            {
                if(first_number == '0')
                {
                    String_addChar(&final_string, '0');
                }
                state = STATE_INC_DOT;
                String_addChar(&final_string, ch);
            }
            else if(ch == 'e' || ch == 'E')
            {
                if(first_number == '0')
                {
                    String_addChar(&final_string, '0');
                }
                state = STATE_INC_EXP;
                String_addChar(&final_string, ch);
            }
            else
            {
                charReturned = true;
                ungetc(ch, input->source);
                input->character--;
                if(first_number == '0')
                {
                    String_addChar(&final_string, '0');
                }
                *token = Token_create(CONSTANT_INTEGER, final_string);
                if(*token == NULL)
                {
                    return INTERNAL_ERROR;
                }
                return NO_ERROR;
            }
            break;

        case STATE_INC_DOT:
            if(isdigit(ch))
            {
                state = STATE_DOUBLE;
                String_addChar(&final_string, ch);
            }
            else
            {
                String_destroy(&final_string); //neposilame final_string v tokenu -> musime uvolnit

                final_string = String_create(NULL); //alokace, za ucelem vytvoreni stringu, ktery posleme, jako parametr znaku, ktery je chybny.
                String_addChar(&final_string, ch);  //znak pridame do retezce -> budeme mit retezec s jednim znakem

                char* reason = String_printf("Unexpected character '%c' on line %i:%i. Expected characters: <0,9>", final_string, (char *) input->line, (char *) input->character, NULL);
                String_destroy(&final_string);

                *token = Token_create(INVALID, reason);
                if(*token == NULL)
                {
                    return INTERNAL_ERROR;
                }
                return LEXICAL_ERROR;
            }
            break;


        case STATE_DOUBLE:
            if(isdigit(ch))
            {
                String_addChar(&final_string, ch);
            }
            else if(ch == 'e' || ch == 'E')
            {
                state = STATE_INC_EXP;
                String_addChar(&final_string, ch);
            }
            else
            {
                ungetc(ch, input->source);
                input->character--;
                *token = Token_create(CONSTANT_DOUBLE, final_string);
                if(*token == NULL)
                {
                    return INTERNAL_ERROR;
                }
                return NO_ERROR;
            }
            break;

        case STATE_INC_EXP:
            if(ch == '+' || ch == '-')
            {
                state = STATE_INC_SIGN;
                String_addChar(&final_string, ch);
            }
            else if(isdigit(ch))
            {
                first_exp_number = ch;
                state = STATE_DOUBLE_EXP;
                if(first_exp_number != '0')
                {
                    String_addChar(&final_string, ch);
                }
            }
            else
            {
                String_destroy(&final_string); //neposilame final_string v tokenu -> musime uvolnit

                final_string = String_create(NULL); //alokace, za ucelem vytvoreni stringu, ktery posleme, jako parametr znaku, ktery je chybny.
                String_addChar(&final_string, ch);  //znak pridame do retezce -> budeme mit retezec s jednim znakem

                char* reason = String_printf("Unexpected character '%c' on line %i:%i. Expected characters: <0,9> or +, -", final_string, (char *) input->line, (char *) input->character, NULL);
                String_destroy(&final_string);

                *token = Token_create(INVALID, reason);
                if(*token == NULL)
                {
                    return INTERNAL_ERROR;
                }
                return LEXICAL_ERROR;
            }
            break;

        case STATE_INC_SIGN:
            if(isdigit(ch))
            {
                first_exp_number = ch;
                state = STATE_DOUBLE_EXP;
                if(first_exp_number != '0')
                {
                    String_addChar(&final_string, ch);
                }
            }
            else
            {
                String_destroy(&final_string); //neposilame final_string v tokenu -> musime uvolnit

                final_string = String_create(NULL); //alokace, za ucelem vytvoreni stringu, ktery posleme, jako parametr znaku, ktery je chybny.
                String_addChar(&final_string, ch);  //znak pridame do retezce -> budeme mit retezec s jednim znakem

                char* reason = String_printf("Unexpected character '%c' on line %i:%i. Expected characters: <0,9> or +, -", final_string, (char *) input->line, (char *) input->character, NULL);
                String_destroy(&final_string);

                *token = Token_create(INVALID, reason);
                if(*token == NULL)
                {
                    return INTERNAL_ERROR;
                }
                return LEXICAL_ERROR;
            }
            break;

        case STATE_DOUBLE_EXP:
            if(isdigit(ch))
            {
                if(first_exp_number == '0')
                {
                    if(ch != '0')
                    {
                        first_exp_number = '1';
                        String_addChar(&final_string, ch);
                    }

                }
                else
                {
                    String_addChar(&final_string, ch);
                }
            }
            else
            {
                charReturned = true;
                ungetc(ch, input->source);
                input->character--;
                if(first_exp_number == '0')
                {
                    String_addChar(&final_string, '0');
                }
                *token = Token_create(CONSTANT_DOUBLE, final_string);
                if(*token == NULL)
                {
                    return INTERNAL_ERROR;
                }
                return NO_ERROR;
            }
            break;
        // Cases for INTEGER AND DOUBLE end
        // Cases for DIV/BLOCK COMMENT begin
        case STATE_DIV:
            if(ch == '\'')
            {
                String_destroy(&final_string); //neposilame final_string v tokenu -> musime uvolnit
                final_string = String_create(NULL);
                state = STATE_INC_BLOCK_COMMENT;
            }
            else if(ch == '=')
            {
                String_addChar(&final_string, ch);
                *token = Token_create(SLASHEQ, final_string);
                if(*token == NULL)
                {
                    return INTERNAL_ERROR;
                }
                return NO_ERROR;
            }
            else
            {
                charReturned = true;
                ungetc(ch, input->source);
                input->character--;

                *token = Token_create(SLASH, final_string);
                if(*token == NULL)
                {
                    return INTERNAL_ERROR;
                }
                return NO_ERROR;
            }
            break;

        case STATE_INC_BLOCK_COMMENT:
            if(ch == '\'')
            {
                state = STATE_INC_BLOCK_COMMENT_APOS;
            }
            else if(ch == '\n')
            {
                input->line++;
            }
            else if(ch == EOF)
            {
                *token = Token_create(FILE_END, NULL);
                if(*token == NULL)
                {
                    return INTERNAL_ERROR;
                }
                return NO_ERROR;
            }
            break;

        case STATE_INC_BLOCK_COMMENT_APOS:
            if(ch == '/')
            {
                state = STATE_BEGIN;
            }
            else if(ch == '\n')
            {
                String_destroy(&last_line);
                last_line = String_create(NULL);
                input->line++;
            }
            else if(ch == EOF)
            {
                String_destroy(&final_string); //neposilame final_string v tokenu -> musime uvolnit
                char* reason = String_printf("Unexpected EOF on line %i:%i.", NULL, (char *) input->line, (char *) input->character, NULL);

                *token = Token_create(INVALID, reason);
                if(*token == NULL)
                {
                    return INTERNAL_ERROR;
                }
                return LEXICAL_ERROR;
            }
            break;
        // Cases for DIV/BLOCK COMMENT end

        // Cases for extended number input begin
        case STATE_INC_AMPERSAND:
            if(ch == 'B')
            {
                state = STATE_INC_B;
            }
            else if(ch == 'O')
            {
                state = STATE_INC_O;
            }
            else if(ch == 'H')
            {
                state = STATE_INC_H;
            }
            else
            {
                String_destroy(&final_string); //neposilame final_string v tokenu -> musime uvolnit

                final_string = String_create(NULL); //alokace, za ucelem vytvoreni stringu, ktery posleme, jako parametr znaku, ktery je chybny.
                String_addChar(&final_string, ch);  //znak pridame do retezce -> budeme mit retezec s jednim znakem

                char* reason = String_printf("Unexpected character '%c' on line %i:%i. Expected characters: B, O or H", final_string, (char *) input->line, (char *) input->character, NULL);
                String_destroy(&final_string); //uvolneni pred ukoncenim funkce

                *token = Token_create(INVALID, reason);
                if(*token == NULL)
                {
                    return INTERNAL_ERROR;
                }
                return LEXICAL_ERROR;
            }
            break;

        case STATE_INC_B:
            if(ch == '1' || ch == '0')
            {
                first_number = ch;

                state = STATE_BIN;
                if(first_number != '0') //pokud bude prvni cislo nula, tak zatim nepridavame do retezce (muze nasledovat dalsi nula)
                {
                    String_addChar(&final_string, ch);
                }

            }
            else
            {
                String_destroy(&final_string); //neposilame final_string v tokenu -> musime uvolnit

                final_string = String_create(NULL); //alokace, za ucelem vytvoreni stringu, ktery posleme, jako parametr znaku, ktery je chybny.
                String_addChar(&final_string, ch);  //znak pridame do retezce -> budeme mit retezec s jednim znakem

                char* reason = String_printf("Unexpected character '%c' on line %i:%i. Expected characters: 1 or 0", final_string, (char *) input->line, (char *) input->character, NULL);
                String_destroy(&final_string);

                *token = Token_create(INVALID, reason);
                if(*token == NULL)
                {
                    return INTERNAL_ERROR;
                }
                return LEXICAL_ERROR;
            }
            break;

        case STATE_INC_O:
            if(ch >= '0' && ch <= '7')
            {
                first_number = ch;
                state = STATE_OCT;
                if(first_number != '0') //pokud bude prvni cislo nula, tak zatim nepridavame do retezce (muze nasledovat dalsi nula)
                {
                    String_addChar(&final_string, ch);
                }
            }
            else
            {
                String_destroy(&final_string); //neposilame final_string v tokenu -> musime uvolnit

                final_string = String_create(NULL); //alokace, za ucelem vytvoreni stringu, ktery posleme, jako parametr znaku, ktery je chybny.
                String_addChar(&final_string, ch);  //znak pridame do retezce -> budeme mit retezec s jednim znakem

                char* reason = String_printf("Unexpected character '%c' on line %i:%i. Expected characters: <0,7>", final_string, (char *) input->line, (char *) input->character, NULL);
                String_destroy(&final_string);

                *token = Token_create(INVALID, reason);
                if(*token == NULL)
                {
                    return INTERNAL_ERROR;
                }
                return LEXICAL_ERROR;
            }
            break;

        case STATE_INC_H:
            if(isdigit(ch) || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F'))
            {
                first_number = ch;
                state = STATE_HEX;
                if(first_number != '0') //pokud bude prvni cislo nula, tak zatim nepridavame do retezce (muze nasledovat dalsi nula)
                {
                    String_addChar(&final_string, ch);
                }
            }
            else
            {
                String_destroy(&final_string); //neposilame final_string v tokenu -> musime uvolnit

                final_string = String_create(NULL); //alokace, za ucelem vytvoreni stringu, ktery posleme, jako parametr znaku, ktery je chybny.
                String_addChar(&final_string, ch);  //znak pridame do retezce -> budeme mit retezec s jednim znakem

                char* reason = String_printf("Unexpected character '%c' on line %i:%i. Expected characters: alphanumeric", final_string, (char *) input->line, (char *) input->character, NULL);
                String_destroy(&final_string);

                *token = Token_create(INVALID, reason);
                if(*token == NULL)
                {
                    return INTERNAL_ERROR;
                }
                return LEXICAL_ERROR;
            }
            break;
        case STATE_BIN:
            if(ch == '1' || ch == '0')
            {
                //testovani, zda i dalsi cislo neni nulove, pokud ano, prejdeme na dalsi cislo a do retezce nic nezapisujeme
                if(first_number == '0')
                {
                    if(ch != '0')
                    {
                        first_number = '1';
                        String_addChar(&final_string, ch);
                    }
                }
                else
                {
                    String_addChar(&final_string, ch);
                }

            }
            else
            {
                charReturned = true;
                ungetc(ch, input->source);
                input->character--;
                if(first_number == '0')
                {
                    String_addChar(&final_string, '0');
                }
                *token = Token_create(CONSTANT_BINARY, final_string);
                if(*token == NULL)
                {
                    return INTERNAL_ERROR;
                }
                return NO_ERROR;
            }
            break;

        case STATE_OCT:
            if(ch >= '0' && ch <= '7')
            {
                //testovani, zda i dalsi cislo neni nulove, pokud ano, prejdeme na dalsi cislo a do retezce nic nezapisujeme
                if(first_number == '0')
                {
                    if(ch != '0')
                    {
                        first_number = '1';
                        String_addChar(&final_string, ch);
                    }
                }
                else
                {
                    String_addChar(&final_string, ch);
                }
            }
            else
            {
                charReturned = true;
                ungetc(ch, input->source);
                input->character--;
                if(first_number == '0')
                {
                    String_addChar(&final_string, '0');
                }
                *token = Token_create(CONSTANT_OCTAL, final_string);
                if(*token == NULL)
                {
                    return INTERNAL_ERROR;
                }
                return NO_ERROR;
            }
            break;

        case STATE_HEX:
            if(isdigit(ch) || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F'))
            {
                //testovani, zda i dalsi cislo neni nulove, pokud ano, prejdeme na dalsi cislo a do retezce nic nezapisujeme
                if(first_number == '0')
                {
                    if(ch != '0')
                    {
                        first_number = '1';
                        String_addChar(&final_string, ch);
                    }
                }
                else
                {
                    String_addChar(&final_string, ch);
                }
            }
            else
            {
                charReturned = true;
                ungetc(ch, input->source);
                input->character--;
                if(first_number == '0')
                {
                    String_addChar(&final_string, '0');
                }
                *token = Token_create(CONSTANT_HEXA, final_string);
                if(*token == NULL)
                {
                    return INTERNAL_ERROR;
                }
                return NO_ERROR;
            }
            break;
        // Cases for extended number input end
        // Cases for comparison begin
        case STATE_LESS:
            if(ch == '=')
            {
                String_addChar(&final_string, ch);
                *token = Token_create(LTEQ, final_string);
                if(*token == NULL)
                {
                    return INTERNAL_ERROR;
                }
                return NO_ERROR;
            }
            else if(ch == '>')
            {
                String_addChar(&final_string, ch);
                *token = Token_create(LTGT, final_string);
                if(*token == NULL)
                {
                    return INTERNAL_ERROR;
                }
                return NO_ERROR;
            }
            else
            {
                charReturned = true;
                ungetc(ch, input->source);
                input->character--;

                *token = Token_create(LT, final_string);
                if(*token == NULL)
                {
                    return INTERNAL_ERROR;
                }
                return NO_ERROR;
            }
            break;

        case STATE_GREATER:
            if(ch == '=')
            {
                String_addChar(&final_string, ch);
                *token = Token_create(GTEQ, final_string);
                if(*token == NULL)
                {
                    return INTERNAL_ERROR;
                }
                return NO_ERROR;
            }
            else
            {
                charReturned = true;
                ungetc(ch, input->source);
                input->character--;

                *token = Token_create(GT, final_string);
                if(*token == NULL)
                {
                    return INTERNAL_ERROR;
                }
                return NO_ERROR;
            }
            break;
        // Cases for comparison end
        // Cases for unary begin
        case STATE_MUL:
            if(ch == '=')
            {
                String_addChar(&final_string, ch);
                *token = Token_create(STAREQ, final_string);
                if(*token == NULL)
                {
                    return INTERNAL_ERROR;
                }
                return NO_ERROR;
            }
            else
            {
                charReturned = true;
                ungetc(ch, input->source);
                input->character--;

                *token = Token_create(STAR, final_string);
                if(*token == NULL)
                {
                    return INTERNAL_ERROR;
                }
                return NO_ERROR;
            }
            break;

        case STATE_ADD:
            if(ch == '=')
            {
                String_addChar(&final_string, ch);
                *token = Token_create(PLUSEQ, final_string);
                if(*token == NULL)
                {
                    return INTERNAL_ERROR;
                }
                return NO_ERROR;
            }
            else
            {
                charReturned = true;
                ungetc(ch, input->source);
                input->character--;

                *token = Token_create(PLUS, final_string);
                if(*token == NULL)
                {
                    return INTERNAL_ERROR;
                }
                return NO_ERROR;
            }
            break;

        case STATE_SUB:
            if(ch == '=')
            {
                String_addChar(&final_string, ch);
                *token = Token_create(MINUSEQ, final_string);
                if(*token == NULL)
                {
                    return INTERNAL_ERROR;
                }
                return NO_ERROR;
            }
            else
            {
                charReturned = true;
                ungetc(ch, input->source);
                input->character--;

                *token = Token_create(MINUS, final_string);
                if(*token == NULL)
                {
                    return INTERNAL_ERROR;
                }
                return NO_ERROR;

            }
            break;

        case STATE_B_SLASH:
            if(ch == '=')
            {
                String_addChar(&final_string, ch);
                *token = Token_create(BACK_SLASHEQ, final_string);
                if(*token == NULL)
                {
                    return INTERNAL_ERROR;
                }
                return NO_ERROR;
            }
            else
            {
                charReturned = true;
                ungetc(ch, input->source);
                input->character--;

                *token = Token_create(BACK_SLASH, final_string);
                if(*token == NULL)
                {
                    return INTERNAL_ERROR;
                }
                return NO_ERROR;

            }
            break;
            // Cases for unary end
        }
    }

}
/**
 * Tato funkce "vrátí" získaný token. Uloží jej na stack
 * tokenů a při dalším volání funkce Scanner_GetToken vrátí
 * tento, ne nový token.
 *
 * @param[in,out]   InputPtr    input           Ukazatel na strukturu se vstupními daty
 * @param[out]      TokenPtr    *token          Ukazatel na získaný token
 *
 * @retval int  Kód se kterým bylo vrácení získaného tokenu ukončeno
 */
int Scanner_UngetToken(InputPtr input, TokenPtr *token)
{
    if (input && input->source == NULL)
        return INTERNAL_ERROR;

    DEBUG_LOG("scanner-unget", "ungetting token");
    Token_debugPrint(*token);

    if (token_stack == NULL)
    {
        token_stack = TokenStack_create();
    }

    if (TokenStack_push(token_stack, *token) != NO_ERROR)
    {
        *token = NULL;
        return INTERNAL_ERROR;
    }
    *token = NULL;

    DEBUG_LOG("scanner-unget", "ok");
    TokenStack_debugPrint(token_stack);
    return NO_ERROR;
}
