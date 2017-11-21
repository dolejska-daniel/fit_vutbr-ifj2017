/**
 * Tento soubor obsahuje implementaci syntaktick�ho analyz�toru (parser).
 *
 * @author Daniel Dolej�ka (xdolej08)
 * @date 21.11.2017
 * @project IFJcode17Parser
 * @subject Form�ln� jazyky a p�eklada�e (IFJ) - FIT VUT v Brn�
 */

#include "parser.h"
#include "../support/error_codes.h"

#ifndef _parser_c
#define _parser_c

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
//  DEKLARACE A DEFINICE ENUMER�TOR� A STRUKTUR
//======================================================================


//==================================================================d=d=
//  DEKLARACE FUNKC�
//======================================================================

/**
 * Hlavn� funkce ovl�daj�c� p�eklada�.
 *
 * @param[in,out]   InputPtr    input   Ukazatel na strukturu se vstupn�mi daty
 *
 * @retval	int	N�vratov� k�d popisuj�c� situaci (chyba, �sp�ch, ...)
 */
int Parser_ParseCode(InputPtr input)
{
    return INTERNAL_ERROR;
}

#endif
