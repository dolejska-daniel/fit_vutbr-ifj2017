/**
 * Tento soubor obsahuje deklarace funkcí využité při implementaci
 * a práci s tokeny v lexikálním analyzátoru (scanneru).
 *
 * @author Daniel Dolejška (xdolej08)
 * @date 21.11.2017
 * @project IFJcode17Parser
 * @subject Formální jazyky a překladače (IFJ) - FIT VUT v Brně
 */

#include <stdio.h>
#include <malloc.h>

#include "token.h"

#ifndef _token_c
#define _token_c

#ifdef DEBUG_INCLUDE
#include "../parser/symtable.h"
#else
#include "symtable.h"
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

/**
 * Funkce pro vytvoření struktury tokenu.
 *
 * @param[in]   TokenType   type    Typ vytvářeného tokenu
 * @param[in]   char        *attr   Vlastnost tokenu
 *
 * @retval  TokenPtr    Ukazatel na nově vytvořenou strukturu
 */
TokenPtr Token_create(TokenType type, char *attr)
{
    TokenPtr t = (TokenPtr) malloc(sizeof(Token));
    if (t == NULL)
    {
        DEBUG_ERR("token", "failed to mallocate Token");
        return NULL;
    }

    t->type = type;
    t->attr = attr;

    return t;
}

/**
 * Funkce pro zrušení struktury tokenu.
 *
 * @param[in,out]   TokenPtr    *t   Ukazatel na ukazatel na existující strukturu tokenu
 */
void Token_destroy(TokenPtr *t)
{
    free(*t);
    *t = NULL;
}

/**
 * Funkce zobrazí obsah tokenu na stderr.
 *
 * @param[in,out]	TokenPtr    token   Ukazatel na existující token
 */
void Token_debugPrint(TokenPtr token)
{
    #ifdef DEBUG_PRINT_TOKEN
    if (token == NULL)
    {
        fprintf(stderr, "DEBUG | TOKEN: NULL\n");
    }
    else
    {
        fprintf(stderr, "DEBUG | TOKEN (%p): {\n", token);
        fprintf(stderr, "\ttype: %s,\n", TokenType_toString(token->type));
        fprintf(stderr, "\tattr: %s,\n", token->attr);
        fprintf(stderr, "}\n");
    }
    #endif
}

bool Token_isConstant(TokenPtr token)
{
    switch (token->type)
    {
        case CONSTANT_BINARY:
        case CONSTANT_BOOLEAN:
        case CONSTANT_DOUBLE:
        case CONSTANT_HEXA:
        case CONSTANT_STRING:
        case CONSTANT_INTEGER:
        case CONSTANT_OCTAL:
            return true;
        default:
            return false;
    }
}

bool Token_isOperator(TokenPtr token)
{
    switch (token->type)
    {
        case EQ:
        case LT:
        case LTEQ:
        case LTGT:
        case GT:
        case GTEQ:
        case PLUS:
        case MINUS:
        case SLASH:
        case BACK_SLASH:
        case STAR:
        case AND:
        case OR:
        case OPEN_BRACKET:
        case CLOSE_BRACKET:
            return true;
        default:
            return false;
    }
}

int TokenType_toSymbolType(TokenType type)
{
    switch (type)
    {
        case CONSTANT_BINARY:
        case CONSTANT_HEXA:
        case CONSTANT_OCTAL:
        case CONSTANT_INTEGER:
            return ST_INTEGER;
        case CONSTANT_BOOLEAN:
            return ST_BOOLEAN;
        case CONSTANT_DOUBLE:
            return ST_DOUBLE;
        case CONSTANT_STRING:
            return ST_STRING;
        default:
            return -1;
    }
}

/**
 * Funkce vrátí textový řetězec zastupující typ tokenu.
 *
 * @retval  char*   Textový řetězec popisující typ tokenu
 */
char *TokenType_toString(TokenType type)
{
    if (type < 35)
        return "KEYWORD";

    switch (type)
    {
        case IDENTIFIER:
            return "IDENTIFIER";
        case CONSTANT_INTEGER:
            return "CONSTANT_INTEGER";
        case CONSTANT_BINARY:
            return "CONSTANT_BINARY";
        case CONSTANT_OCTAL:
            return "CONSTANT_OCTAL";
        case CONSTANT_HEXA:
            return "CONSTANT_HEXA";
        case CONSTANT_DOUBLE:
            return "CONSTANT_DOUBLE";
        case CONSTANT_STRING:
            return "CONSTANT_STRING";
        case CONSTANT_BOOLEAN:
            return "CONSTANT_BOOLEAN";
        case COMMA:
            return "COMMA";
        case SEMICOLON:
            return "SEMICOLON";
        case OPEN_BRACKET:
            return "OPEN_BRACKET";
        case CLOSE_BRACKET:
            return "CLOSE_BRACKET";
        case EQ:
            return "EQUALS";
        case GT:
            return "GREATER_THAN";
        case GTEQ:
            return "GREATER_THAN_EQUALS";
        case LT:
            return "LESS_THAN";
        case LTEQ:
            return "LESS_THAN_EQUALS";
        case LTGT:
            return "NOT_EQUALS";
        case MINUS:
            return "MINUS";
        case MINUSEQ:
            return "-=";
        case PLUS:
            return "PLUS";
        case PLUSEQ:
            return "+=";
        case SLASH:
            return "SLASH";
        case SLASHEQ:
            return "/=";
        case BACK_SLASH:
            return "BACK_SLASH";
        case BACK_SLASHEQ:
            return "\\=";
        case STAR:
            return "STAR";
        case STAREQ:
            return "*=";
        case LINE_END:
            return "<EOL>";
        case FILE_END:
            return "<EOF>";
        default:
            return "_UNKNOWN_";
    }
}

#endif
