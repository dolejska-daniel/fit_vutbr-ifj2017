/**
 * Tento soubor obsahuje implementaci syntaktick�ho analyz�toru (parser).
 *
 * @author Daniel Dolej�ka (xdolej08)
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
//  DEKLARACE A DEFINICE ENUMER�TOR� A STRUKTUR
//======================================================================


//==================================================================d=d=
//  DEKLARACE FUNKC�
//======================================================================

/**
 * Hlavn� funkce ovl�daj�c� p�eklada�.
 *
 * @param
 *
 * @retval	int	N�vratov� k�d popisuj�c� situaci (chyba, �sp�ch, ...)
 */
int Parser_ParseCode()
{

}

#endif
