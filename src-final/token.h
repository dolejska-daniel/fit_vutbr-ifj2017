/**
 * Tento soubor obsahuje deklarace funkcí využité při implementaci
 * a práci s tokeny v lexikálním analyzátoru (scanneru).
 *
 * @author Daniel Dolejška
 */

#ifndef _token_h
#define _token_h

//==================================================================d=d=
//  DEKLARACE A DEFINICE ENUMERÁTORŮ A STRUKTUR
//======================================================================

typedef enum E_TokenType {
	//	TODO: Typy tokenů, vyblité scannerem
} TokenType;

typedef struct S_Token
    Token,
   *TokenPtr;
struct S_Token {
	TokenType	type;
	char		*attr;
	//	TODO: Další vlastnosti, které token musí mít
};


//==================================================================d=d=
//  DEKLARACE FUNKCÍ
//======================================================================

//	TODO: Deklarovat funkce používané s tokeny, jestli jsou nějaké

#endif