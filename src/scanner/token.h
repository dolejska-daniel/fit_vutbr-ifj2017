/**
 * Tento soubor obsahuje deklarace funkcí využité při implementaci
 * a práci s tokeny v lexikálním analyzátoru (scanneru).
 *
 * @author Daniel Dolejška (xdolej08)
 * @date 21.11.2017
 * @project IFJcode17Parser
 * @subject Formální jazyky a překladače (IFJ) - FIT VUT v Brně
 */

#ifndef _token_h
#define _token_h

#ifdef DEBUG_INCLUDE
#else
#endif

//==================================================================d=d=
//  DEKLARACE A DEFINICE ENUMERÁTORŮ A STRUKTUR
//======================================================================

typedef enum E_TokenType {
    //  Keywords begin
    AS, ASC, DECLARE, DIM, DO, DOUBLE, ELSE, END, CHR, FUNCTION, IF,
    INPUT, INTEGER, LENGTH, LOOP, PRINT, RETURN, SCOPE, STRING, SUBSTR,
    THEN, WHILE, AND, BOOLEAN, CONTINUE, ELSEIF, EXIT, FALSE, FOR, NEXT,
    NOT, OR, SHARED, STATIC, TRUE,
    //  Keywords end
    IDENTIFIER,         ///< variable_name
    CONSTANT_INTEGER,   ///< 25
    CONSTANT_DOUBLE,    ///< 0.25
    CONSTANT_STRING,    ///< "string"
    CONSTANT_BOOLEAN,   ///< true, false
    COMMA,              ///< ,
    SEMICOLON,          ///< ;
    OPEN_BRACKET,       ///< (
    CLOSE_BRACKET,      ///< )
    EQ,                 ///< =
    GT,                 ///< >
    LT,                 ///< <
    MINUS,              ///< -
    PLUS,               ///< +
    SLASH,              ///< /
    STAR,               ///< *
    LINE_END,           ///< EOL
    FILE_END,           ///< EOF
    INVALID,            ///< Chyba scanneru
	//	TODO: Další typy tokenů, vyblité scannerem
} TokenType; ///< Typy tokenů

typedef struct S_Token
    Token,
   *TokenPtr;
struct S_Token {
	TokenType	type;
	char		*attr;
	//	TODO: Další vlastnosti, které token musí mít
}; ///< Struktura tokenu


//==================================================================d=d=
//  DEKLARACE FUNKCÍ
//======================================================================

/**
 * Funkce pro vytvoření struktury tokenu.
 *
 * @param[in]   TokenType   type    Typ vytvářeného tokenu
 * @param[in]   char        *attr   Vlastnost tokenu
 */
TokenPtr Token_create(TokenType type, char *attr);

/**
 * Funkce pro zrušení struktury tokenu.
 *
 * @param[in,out]   TokenPtr    t   Ukazatel na existující strukturu tokenu
 */
void Token_destroy(TokenPtr t);

//	TODO: Deklarovat další funkce používané s tokeny, jestli jsou nějaké

#endif
