/**
 * Tento soubor obsahuje implementaci hlavní øídící funkce pøekladaèe.
 *
 * @author Daniel Dolejška (xdolej08)
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
//  DEKLARACE FUNKCÍ
//======================================================================

/**
 * Hlavní funkce ovládající pøekladaè.
 *
 * @param	int		argc	Poèet vstupních argumentù
 * @param	char    **argv	Pole vstupních argumentù
 */
int main(int argc, char **argv)
{
    DEBUG_PRINT("[%s] Starting program", "main");
    DEBUG_PRINT("[%s] Initializing variables", "main");

    //  Inicializace potøebných promìnných
    InputPtr input = Input_create(stdin);


    DEBUG_PRINT("[%s] Starting parser", "main");
    Parser_ParseCode(input);

    DEBUG_PRINT("[%s] Outputting instructions", "main");
    Instruction_outputAll();


    DEBUG_PRINT("[%s] Destroying allocated variables", "main");

    //  Zrušení promìnných
    Input_destroy(input);

    DEBUG_PRINT("[%s] Closing program", "main");
    return NO_ERROR;
}
