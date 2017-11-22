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

#ifdef DEBUG_INCLUDE
#include "../support/error_codes.h"
#include "../support/strings.h"
#else
#include "error_codes.h"
#include "error_codes.h"
#endif

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


//==================================================================d=d=
//  DEKLARACE FUNKCÍ
//======================================================================

/**
 * Hlavní funkce ovládající lexikální analyzátor.
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
    /** HOW TO

        char ch = fgetc(input->source);
        ungetc(ch, input->source);

        token = Token_create(IF, NULL);
        token = Token_create(CONSTANT_INTEGER, "25");
        if(token == NULL)
        {
            return INTERNAL_ERROR;
        }

        pokud dojde k lexikální chybě (přijde neočekávaný znak -> LEXICAL_ERROR)

        String_destroy(&final_string); //neposilame final_string v tokenu -> musime uvolnit
    */
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
                String_addChar(&final_string, ch);
                *token = Token_create(STAR, final_string);
                if(*token == NULL)
                {
                    return INTERNAL_ERROR;
                }
                return NO_ERROR;
            }
            else if(ch == '+')
            {
                String_addChar(&final_string, ch);
                *token = Token_create(PLUS, final_string);
                if(*token == NULL)
                {
                    return INTERNAL_ERROR;
                }
                return NO_ERROR;
            }
            else if(ch == '-')
            {
                String_addChar(&final_string, ch);
                *token = Token_create(MINUS, final_string);
                if(*token == NULL)
                {
                    return INTERNAL_ERROR;
                }
                return NO_ERROR;
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
                String_addChar(&final_string, ch);
                *token = Token_create(LT, final_string);
                if(*token == NULL)
                {
                    return INTERNAL_ERROR;
                }
                return NO_ERROR;
            }
            else if(ch == '>')
            {
                String_addChar(&final_string, ch);
                *token = Token_create(GT, final_string);
                if(*token == NULL)
                {
                    return INTERNAL_ERROR;
                }
                return NO_ERROR;
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
                String_addChar(&final_string, ch);
                *token = Token_create(BACK_SLASH, final_string);
                if(*token == NULL)
                {
                    return INTERNAL_ERROR;
                }
                return NO_ERROR;
            }
            else if(ch == '\n')
            {
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
                char* reason = String_printf("Unexpected character '%c' on line %i:%i.", ch, input->line, input->character, NULL);
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
                //Porovnavani retezce, zda se nejedna o keyword
                if(strcmp(final_string, "as") == 0)
                {
                    String_destroy(&final_string); //neposilame final_string v tokenu -> musime uvolnit
                    *token = Token_create(AS, NULL);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "asc") == 0)
                {
                    String_destroy(&final_string); //neposilame final_string v tokenu -> musime uvolnit
                    *token = Token_create(ASC, NULL);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "declare") == 0)
                {
                    String_destroy(&final_string); //neposilame final_string v tokenu -> musime uvolnit
                    *token = Token_create(DECLARE, NULL);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "dim") == 0)
                {
                    String_destroy(&final_string); //neposilame final_string v tokenu -> musime uvolnit
                    *token = Token_create(DIM, NULL);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "do") == 0)
                {
                    String_destroy(&final_string); //neposilame final_string v tokenu -> musime uvolnit
                    *token = Token_create(DO, NULL);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "double") == 0)
                {
                    String_destroy(&final_string); //neposilame final_string v tokenu -> musime uvolnit
                    *token = Token_create(DOUBLE, NULL);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "else") == 0)
                {
                    String_destroy(&final_string); //neposilame final_string v tokenu -> musime uvolnit
                    *token = Token_create(ELSE, NULL);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "end") == 0)
                {
                    String_destroy(&final_string); //neposilame final_string v tokenu -> musime uvolnit
                    *token = Token_create(END, NULL);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "chr") == 0)
                {
                    String_destroy(&final_string); //neposilame final_string v tokenu -> musime uvolnit
                    *token = Token_create(CHR, NULL);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "function") == 0)
                {
                    String_destroy(&final_string); //neposilame final_string v tokenu -> musime uvolnit
                    *token = Token_create(FUNCTION, NULL);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "if") == 0)
                {
                    String_destroy(&final_string); //neposilame final_string v tokenu -> musime uvolnit
                    *token = Token_create(IF, NULL);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "input") == 0)
                {
                    String_destroy(&final_string); //neposilame final_string v tokenu -> musime uvolnit
                    *token = Token_create(INPUT, NULL);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "integer") == 0)
                {
                    String_destroy(&final_string); //neposilame final_string v tokenu -> musime uvolnit
                    *token = Token_create(INTEGER, NULL);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "length") == 0)
                {
                    String_destroy(&final_string); //neposilame final_string v tokenu -> musime uvolnit
                    *token = Token_create(LENGTH, NULL);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "loop") == 0)
                {
                    String_destroy(&final_string); //neposilame final_string v tokenu -> musime uvolnit
                    *token = Token_create(LOOP, NULL);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "print") == 0)
                {
                    String_destroy(&final_string); //neposilame final_string v tokenu -> musime uvolnit
                    *token = Token_create(PRINT, NULL);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "return") == 0)
                {
                    String_destroy(&final_string); //neposilame final_string v tokenu -> musime uvolnit
                    *token = Token_create(RETURN, NULL);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "scope") == 0)
                {
                    String_destroy(&final_string); //neposilame final_string v tokenu -> musime uvolnit
                    *token = Token_create(SCOPE, NULL);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "string") == 0)
                {
                    String_destroy(&final_string); //neposilame final_string v tokenu -> musime uvolnit
                    *token = Token_create(STRING, NULL);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "substr") == 0)
                {
                    String_destroy(&final_string); //neposilame final_string v tokenu -> musime uvolnit
                    *token = Token_create(SUBSTR, NULL);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "then") == 0)
                {
                    String_destroy(&final_string); //neposilame final_string v tokenu -> musime uvolnit
                    *token = Token_create(THEN, NULL);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "while") == 0)
                {
                    String_destroy(&final_string); //neposilame final_string v tokenu -> musime uvolnit
                    *token = Token_create(WHILE, NULL);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "and") == 0)
                {
                    String_destroy(&final_string); //neposilame final_string v tokenu -> musime uvolnit
                    *token = Token_create(AND, NULL);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "boolean") == 0)
                {
                    String_destroy(&final_string); //neposilame final_string v tokenu -> musime uvolnit
                    *token = Token_create(BOOLEAN, NULL);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "continue") == 0)
                {
                    String_destroy(&final_string); //neposilame final_string v tokenu -> musime uvolnit
                    *token = Token_create(CONTINUE, NULL);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "elseif") == 0)
                {
                    String_destroy(&final_string); //neposilame final_string v tokenu -> musime uvolnit
                    *token = Token_create(ELSEIF, NULL);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "exit") == 0)
                {
                    String_destroy(&final_string); //neposilame final_string v tokenu -> musime uvolnit
                    *token = Token_create(EXIT, NULL);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "false") == 0)
                {
                    String_destroy(&final_string); //neposilame final_string v tokenu -> musime uvolnit
                    *token = Token_create(FALSE, NULL);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "for") == 0)
                {
                    String_destroy(&final_string); //neposilame final_string v tokenu -> musime uvolnit
                    *token = Token_create(FOR, NULL);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "next") == 0)
                {
                    String_destroy(&final_string); //neposilame final_string v tokenu -> musime uvolnit
                    *token = Token_create(NEXT, NULL);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "not") == 0)
                {
                    String_destroy(&final_string); //neposilame final_string v tokenu -> musime uvolnit
                    *token = Token_create(NOT, NULL);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "or") == 0)
                {
                    String_destroy(&final_string); //neposilame final_string v tokenu -> musime uvolnit
                    *token = Token_create(OR, NULL);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "shared") == 0)
                {
                    String_destroy(&final_string); //neposilame final_string v tokenu -> musime uvolnit
                    *token = Token_create(SHARED, NULL);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "static") == 0)
                {
                    String_destroy(&final_string); //neposilame final_string v tokenu -> musime uvolnit
                    *token = Token_create(STATIC, NULL);
                    if(*token == NULL)
                    {
                        return INTERNAL_ERROR;
                    }
                    return NO_ERROR;
                }
                else if(strcmp(final_string, "true") == 0)
                {
                    String_destroy(&final_string); //neposilame final_string v tokenu -> musime uvolnit
                    *token = Token_create(TRUE, NULL);
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
                String_destroy(&final_string); //neposilame final_string v tokenu -> musime uvolnit
                char* reason = String_printf("Unexpected character '%c' on line %i:%i. Expected character \" ", ch, input->line, input->character, NULL);
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
            else if(ch == '\n')
            {
                String_destroy(&final_string); //neposilame final_string v tokenu -> musime uvolnit
                char* reason = String_printf("Unexpected character '%c' on line %i:%i. (EOL)", ch, input->line, input->character, NULL);
                *token = Token_create(INVALID, reason);
                if(*token == NULL)
                {
                    return INTERNAL_ERROR;
                }
                return LEXICAL_ERROR;
            }
            else
            {
                if(ch == ' ') //nahrazeni mezery
                {
                    final_string = String_concat(final_string, "\032", NULL);
                }
                else if(isupper(ch))
                {
                    ch = ch + 32;
                    String_addChar(&final_string, ch);
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
                if(ch == '0')
                {
                    esc1 = ch;
                }

                state = STATE_INC_ESC1;
                String_addChar(&final_string, ch);
            }
            else if(ch == '"' || ch == 'n' || ch == 't' || ch == '\\')
            {
                state = STATE_INC_STRING_BEGIN;
                String_addChar(&final_string, ch);
            }
            else
            {
                String_destroy(&final_string); //neposilame final_string v tokenu -> musime uvolnit
                char* reason = String_printf("Unexpected character '%c' on line %i:%i. Expected characters: \", n, t, \\ or <0,2>", ch, input->line, input->character, NULL);
                *token = Token_create(INVALID, reason);
                if(*token == NULL)
                {
                    return INTERNAL_ERROR;
                }
                return LEXICAL_ERROR;
            }
            break;

        case STATE_INC_ESC1:
            if(ch >= '0' && ch <= '5')
            {
                if(ch == '0')
                {
                    esc2 = ch;
                }
                state = STATE_INC_ESC2;
                String_addChar(&final_string, ch);
            }
            else
            {
                String_destroy(&final_string); //neposilame final_string v tokenu -> musime uvolnit
                char* reason = String_printf("Unexpected character '%c' on line %i:%i. Expected characters: <0,5>", ch, input->line, input->character, NULL);
                *token = Token_create(INVALID, reason);
                if(*token == NULL)
                {
                    return INTERNAL_ERROR;
                }
                return LEXICAL_ERROR;
            }
            break;

        case STATE_INC_ESC2:
            if(ch >= '0' && ch <= '5')
            {
                if(esc1 == '0' && esc2 == '0' && ch == '0')
                {
                    String_destroy(&final_string); //neposilame final_string v tokenu -> musime uvolnit
                    char* reason = String_printf("Unexpected character '%c' on line %i:%i. Expected characters: <1,5>", ch, input->line, input->character, NULL);
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
                String_destroy(&final_string); //neposilame final_string v tokenu -> musime uvolnit
                char* reason = String_printf("Unexpected character '%c' on line %i:%i. Expected characters: <0,5>", ch, input->line, input->character, NULL);
                *token = Token_create(INVALID, reason);
                if(*token == NULL)
                {
                    return INTERNAL_ERROR;
                }
                return LEXICAL_ERROR;
            }
            break;

        case STATE_STRING:
            *token = Token_create(STRING, final_string);
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
                if(strcmp(final_string,"") == 0)
                {
                    String_addChar(&final_string, '0');
                }
                state = STATE_INC_EXP;
                String_addChar(&final_string, ch);
            }
            else
            {
                if(strcmp(final_string,"") == 0)
                {
                    String_addChar(&final_string, '0');
                }
                *token = Token_create(INTEGER, final_string);
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
                char* reason = String_printf("Unexpected character '%c' on line %i:%i. Expected characters: <0,9>", ch, input->line, input->character, NULL);
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
                *token = Token_create(DOUBLE, final_string);
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
                char* reason = String_printf("Unexpected character '%c' on line %i:%i. Expected characters: <0,9> or +, -", ch, input->line, input->character, NULL);
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
                char* reason = String_printf("Unexpected character '%c' on line %i:%i. Expected characters: <0,9> or +, -", ch, input->line, input->character, NULL);
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
                if(first_exp_number == '0')
                {
                    String_addChar(&final_string, '0');
                }
                *token = Token_create(INTEGER, final_string);
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

            break;
        // Cases for DIV/BLOCK COMMENT end
        case STATE_INC_AMPERSAND:

            break;
        }

    }
}
