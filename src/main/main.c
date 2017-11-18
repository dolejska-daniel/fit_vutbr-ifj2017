/**
 * Tento soubor obsahuje implementaci hlavn� ��d�c� funkce p�eklada�e.
 *
 * @author Daniel Dolej�ka (xdolej08)
 * @project IFJcode17Parser
 */

#include <stdio.h>
#include <stdlib.h>

#ifdef DEBUG
#include "../parser/parser.h"
#include "../scanner/scanner.h"
#define DEBUG_PRINT(...) do{ fprintf( stderr, __VA_ARGS__ ); } while( 0 )
#else
#include "parser.h"
#include "scanner.h"
#define DEBUG_PRINT(...) do{ } while ( 0 )
#endif

//==================================================================d=d=
//  DEKLARACE FUNKC�
//======================================================================

/**
 * Hlavn� funkce ovl�daj�c� p�eklada�.
 *
 * @param	int		argc	Po�et vstupn�ch argument�
 * @param	char    **argv	Pole vstupn�ch argument�
 */
int main(int argc, char **argv)
{
    return 99;
}
