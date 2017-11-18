/**
 * Tento soubor obsahuje deklarace funkcí využité při implementaci
 * a práci s tokeny v lexikálním analyzátoru (scanneru).
 *
 * @author Daniel Dolejška (xdolej08)
 * @project IFJcode17Parser
 */

#ifndef _token_h
#define _token_h

//==================================================================d=d=
//  DEKLARACE A DEFINICE ENUMERÁTORŮ A STRUKTUR
//======================================================================

typedef enum E_TokenType {
    KEYWORD,
	//	TODO: Typy tokenů, vyblité scannerem
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
