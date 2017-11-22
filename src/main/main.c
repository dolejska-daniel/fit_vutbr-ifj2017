/**
 * Tento soubor obsahuje implementaci hlavní řídící funkce překladače.
 *
 * @author Daniel Dolejška (xdolej08)
 * @date 21.11.2017
 * @project IFJcode17Parser
 * @subjec Formální jazyky a překladače (IFJ) - FIT VUT v Brně
 */

#include <stdio.h>
#include <stdlib.h>

#ifdef DEBUG_INCLUDE
#include "../generator/generator.h"
#include "../generator/instruction_list.h"
#include "../parser/parser.h"
#include "../scanner/scanner.h"
#include "../scanner/input.h"
#include "../support/strings.h"
#include "../support/error_codes.h"
#include "../generator/instruction_list.h"
#else
#include "generator.h"
#include "instruction_list.h"
#include "parser.h"
#include "scanner.h"
#include "input.h"
#include "strings.h"
<<<<<<< HEAD
#include "error_codes.h"
=======
#include "instruction_list.h"
>>>>>>> dolejska/lk
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

char *error_description;

//==================================================================d=d=
//  DEKLARACE FUNKCÍ
//======================================================================
/**
 * Hlavní funkce ovládající překladač.
 *
 * @param	int		argc	Počet vstupních argumentů
 * @param	char    **argv	Pole vstupních argumentů
 */
int main(/*int argc, char **argv*/)
{
    DEBUG_LOG("main", "Starting program");
    InstructionListPtr List;

    DEBUG_LOG("main", "Creating list:");
    List = InstructionList_create();

<<<<<<< HEAD

    //-------------------------------------------------d-d-
    //  Inicializace potřebných proměnných
    //-----------------------------------------------------
=======
    DEBUG_LOG("main", "Creating symbol:");
    SymbolPtr s1 = Symbol_create("var", ST_STRING, GLOBAL_FRAME, "variable");
    SymbolPtr s2 = Symbol_create("pom", ST_STRING, LOCAL_FRAME, "pomocna");
    int i = Instruction_jumpifneq(List, "navesti", s1, s2);
    InstructionList_destroy(&List);
    return i;
    //  Inicializace pot�ebn�ch prom�nn�ch
>>>>>>> dolejska/lk
    DEBUG_LOG("main", "Initializing variables");

    int result = NO_ERROR;

    InputPtr input = Input_create(stdin);
    if (input == NULL)
    {
        DEBUG_ERR("main", "failed to mallocate Input");
        result = INTERNAL_ERROR;
        goto program_exit;
    }

    InstructionListPtr ilist = InstructionList_create();
    if (ilist == NULL)
    {
        DEBUG_ERR("main", "failed to mallocate InstructionList");
        result = INTERNAL_ERROR;
        goto program_exit;
    }

    SymbolTablePtr symtable = SymbolTable_create();
    if (symtable == NULL)
    {
        DEBUG_ERR("main", "failed to mallocate SymbolTable");
        result = INTERNAL_ERROR;
        goto program_exit;
    }


    //-------------------------------------------------d-d-
    //  Hlavní logika programu
    //-----------------------------------------------------
    DEBUG_LOG("main", "Starting parser");
    int parser_result = Parser_ParseInitial(input, ilist, symtable);
    if (parser_result != 0)
    {
        DEBUG_PRINT("[%s] ERR Parser returned error code %i\n", "main", parser_result);
        DEBUG_PRINT("[%s] ERR \tMessage: %s", "main", error_description);

        String_destroy(&error_description);
        //  TODO: Output errors
        //  TODO: Cleanup & exit
    }
    else
    {
        DEBUG_LOG("main", "Outputting instructions");
        //Instruction_outputAll(ilist);
    }
    result = parser_result;


program_exit:
    //-------------------------------------------------d-d-
    //  Zrušení proměnných a ukončení programu
    //-----------------------------------------------------
    DEBUG_LOG("main", "Destroying allocated variables");
    if (input != NULL)
        Input_destroy(&input);

    if (ilist != NULL)
        InstructionList_destroy(&ilist);

    if (symtable != NULL)
        SymbolTable_destroy(&symtable);


    DEBUG_LOG("main", "Exiting program");
    return result;
}
