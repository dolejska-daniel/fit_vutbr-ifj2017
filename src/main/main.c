/**
 * Tento soubor obsahuje implementaci hlavní øídící funkce pøekladaèe.
 *
 * @author Daniel Dolejška (xdolej08)
 * @date 21.11.2017
 * @project IFJcode17Parser
 * @subject Formální jazyky a pøekladaèe (IFJ) - FIT VUT v Brnì
 */

#include <stdio.h>
#include <stdlib.h>

#ifdef DEBUG_INCLUDE
#include "../generator/generator.h"
#include "../parser/parser.h"
#include "../scanner/scanner.h"
#include "../scanner/input.h"
#include "../support/strings.h"
#include "../support/error_codes.h"
#include "../generator/instruction_list.h"
#else
#include "generator.h"
#include "parser.h"
#include "parser_errors.h"
#include "scanner.h"
#include "input.h"
#include "strings.h"
#include "instruction_list.h"
#endif

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
//  DEKLARACE FUNKCÍ
//======================================================================
/**
 * Hlavní funkce ovládající pøekladaè.
 *
 * @param	int		argc	Poèet vstupních argumentù
 * @param	char    **argv	Pole vstupních argumentù
 * @param	char    **argv	Pole vstupních argumentù
 */
int main(int argc, char **argv)
{
    DEBUG_LOG("main", "Starting program");

    //  Inicializace potøebných promìnných
    DEBUG_LOG("main", "Initializing variables");

    int result = NO_ERROR;

    InputPtr input = Input_create(stdin);
    if (input == NULL)
    {
        DEBUG_ERR("main", "failed to mallocate Input");
        return INTERNAL_ERROR;
    }

    DEBUG_LOG("main", "Starting parser");
    int parser_result = Parser_ParseCode(input);
    if (parser_result != 0)
    {
        DEBUG_PRINT("[%s] %3s Parser returner error code %i\n", "main", "ERR", parser_result);
        //  TODO: Output errors
        //  TODO: Cleanup & exit
    }
    else
    {
        DEBUG_LOG("main", "Outputting instructions");
        //Instruction_outputAll();
    }
    result = parser_result;


    //  Zrušení promìnných
    DEBUG_LOG("main", "Destroying allocated variables");
    Input_destroy(&input);

    DEBUG_LOG("main", "Exiting program");
    return result;
}
