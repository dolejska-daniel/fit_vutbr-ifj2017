/**
 * Tento soubor obsahuje implementaci hlavn� ��d�c� funkce p�eklada�e.
 *
 * @author Daniel Dolej�ka (xdolej08)
 * @date 21.11.2017
 * @project IFJcode17Parser
 * @subject Form�ln� jazyky a p�eklada�e (IFJ) - FIT VUT v Brn�
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
//  DEKLARACE FUNKC�
//======================================================================
/**
 * Hlavn� funkce ovl�daj�c� p�eklada�.
 *
 * @param	int		argc	Po�et vstupn�ch argument�
 * @param	char    **argv	Pole vstupn�ch argument�
 * @param	char    **argv	Pole vstupn�ch argument�
 */
int main(int argc, char **argv)
{
    DEBUG_LOG("main", "Starting program");

    //  Inicializace pot�ebn�ch prom�nn�ch
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


    //  Zru�en� prom�nn�ch
    DEBUG_LOG("main", "Destroying allocated variables");
    Input_destroy(&input);

    DEBUG_LOG("main", "Exiting program");
    return result;
}
