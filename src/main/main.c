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
#include "error_codes.h"
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

int  error_char_index;
int  error_instruction_length;
char *error_description;
char *last_line;


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
    //-------------------------------------------------d-d-
    //  Inicializace potřebných proměnných
    //-----------------------------------------------------
    DEBUG_LOG("main", "Initializing variables");

    int result = NO_ERROR;

    last_line = String_create(NULL);
    if (last_line == NULL)
    {
        DEBUG_ERR("main", "failed to mallocate last_line");
        result = INTERNAL_ERROR;
        goto program_exit;
    }

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
        DEBUG_PRINT("[%s]   | Message: %s\n", "main", error_description);

        //  TODO: Output errors
        //  TODO: Cleanup & exit
        Instruction_outputAll(ilist);
    }
    else
    {
        DEBUG_LOG("main", "Outputting instructions");
        Instruction_outputAll(ilist);
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
    if (result != NO_ERROR)
    {
        //  Došlo k ukončení s chybami
        fprintf(stderr, "\nAn error occured during program execution!\n==========================================\n\nYou will find more detailed information below.\n\n");
        fprintf(stderr, "%p\n", last_line);
        if (last_line != NULL && strlen(last_line) > 0)
        {
            //  Máme k dispozici obsah posledního řádku
            fprintf(stderr, "Error line:\n\t");
            fprintf(stderr, last_line);
            if (last_line[strlen(last_line) - 1] != '\n')
                fprintf(stderr, "\n");

            fprintf(stderr, "\t");
            for (int char_index = 0; char_index < error_char_index; char_index++)
                fprintf(stderr, " ");
            fprintf(stderr, "^");
            for (int char_index = 0; char_index < error_instruction_length - 1; char_index++)
                fprintf(stderr, "~");
            fprintf(stderr, "\n\n");

            String_destroy(&last_line);
        }

        fprintf(stderr, "Exit code:  %i\n", result);
        fprintf(stderr, "Error:      ");
        if (error_description)
        {
            fprintf(stderr, error_description);
            fprintf(stderr, "\n");
            String_destroy(&error_description);
        }
        else
        {
            fprintf(stderr, "Unknown error occured during parsing.\n");
        }
    }
    return result;
}
