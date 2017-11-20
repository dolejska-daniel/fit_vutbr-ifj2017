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
#include "../parser/parser_errors.h"
#include "../scanner/scanner.h"
#include "../scanner/input.h"
#define DEBUG_PRINT(...) do{ fprintf( stderr, __VA_ARGS__ ); } while( 0 )
#else
#include "parser.h"
#include "parser_errors.h"
#include "scanner.h"
#include "input.h"
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
    DEBUG_PRINT("[%s] Starting program", "main");
    DEBUG_PRINT("[%s] Initializing variables", "main");

    //  Inicializace pot�ebn�ch prom�nn�ch
    InputPtr input = Input_create(stdin);


    DEBUG_PRINT("[%s] Starting parser", "main");
    Parser_ParseCode(input);

    DEBUG_PRINT("[%s] Outputting instructions", "main");
    Instruction_outputAll();


    DEBUG_PRINT("[%s] Destroying allocated variables", "main");

    //  Zru�en� prom�nn�ch
    Input_destroy(input);

    DEBUG_PRINT("[%s] Closing program", "main");
    return NO_ERROR;
}
