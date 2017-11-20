/**
 * Tento soubor obsahuje deklarace funkcí využité při implementaci
 * lexikálního analyzátoru (scanner).
 *
 * @author Daniel Dolejška (xdolej08)
 * @project IFJcode17Parser
 */

#include "token.h"
#include "input.h"

#ifndef _scanner_h
#define _scanner_h

#ifdef DEBUG
#define DEBUG_PRINT(...) do{ fprintf( stderr, __VA_ARGS__ ); } while( 0 )
#else
#define DEBUG_PRINT(...) do{ } while ( 0 )
#endif

//==================================================================d=d=
//  DEKLARACE A DEFINICE ENUMERÁTORŮ A STRUKTUR
//======================================================================

typedef enum E_AutomataState {
	BEGIN,
	//  TODO: Další stavy automatu pro lexikální analýzu
} AutomataState; ///< Stavy automatu


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
int Scanner_GetToken(InputPtr input, TokenPtr *token);

#endif