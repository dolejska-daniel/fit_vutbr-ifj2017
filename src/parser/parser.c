/**
 * Tento soubor obsahuje implementaci syntaktického analyzátoru (parser).
 *
 * @author Daniel Dolejška (xdolej08)
 * @project IFJcode17Parser
 */

#include "parser.h"
#include "parser_errors.h"

#ifndef _parser_c
#define _parser_c

#ifdef DEBUG
#include "../scanner/scanner.h"
#include "../scanner/input.h"
#define DEBUG_PRINT(...) do{ fprintf( stderr, __VA_ARGS__ ); } while( 0 )
#else
#include "scanner.h"
#include "input.h"
#define DEBUG_PRINT(...) do{ } while ( 0 )
#endif

//==================================================================d=d=
//  DEKLARACE A DEFINICE ENUMERÁTORÙ A STRUKTUR
//======================================================================


//==================================================================d=d=
//  DEKLARACE FUNKCÍ
//======================================================================

/**
 * Hlavní funkce ovládající pøekladaè.
 *
 * @param
 *
 * @retval	int	Návratový kód popisující situaci (chyba, úspìch, ...)
 */
int Parser_ParseCode()
{

}

#endif
