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
        case LINE_END:
            return "<EOL>";
        case FILE_END:
            return "<EOF>";
        default:
            return "_UNKNOWN_";
    }
}

#endif
