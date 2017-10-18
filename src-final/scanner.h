/**
 * Tento soubor obsahuje deklarace funkcí využité při implementaci
 * lexikálního analyzátoru (scanneru).
 *
 * @author Daniel Dolejška
 */

#include <token.h>

#ifndef _scanner_h
#define _scanner_h

//==================================================================d=d=
//  DEKLARACE A DEFINICE ENUMERÁTORŮ A STRUKTUR
//======================================================================

typedef enum E_AutomataState {
	BEGIN,
} AutomataState;


//==================================================================d=d=
//  DEKLARACE FUNKCÍ
//======================================================================

/**
 * Hlavní funkce ovládající překladač.
 *
 * @param	int		argc	Počet vstupních argumentů
 * @param	char**	argv	Pole vstupních argumentů
 */
Token *Scanner_GetToken();


#endif
