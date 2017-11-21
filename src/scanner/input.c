/**
 * Tento soubor obsahuje implementaci funkcí pro práci se strukturou
 * vstupních dat.
 *
 * @author Daniel Dolejška (xdolej08)
 * @date 21.11.2017
 * @project IFJcode17Parser
 * @subject Formální jazyky a překladače (IFJ) - FIT VUT v Brně
 */

#include <stdio.h>
#include <malloc.h>

#include "input.h"

#ifndef _input_c
#define _input_c

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
//  DEKLARACE A DEFINICE ENUMERÁTORŮ A STRUKTUR
//======================================================================


//==================================================================d=d=
//  DEKLARACE FUNKCÍ
//======================================================================

/**
 * Funkce pro vytvoření struktury vstupních dat.
 *
 * @param[in,out]	FILE    *source Ukazatel na resource vstupních dat
 *
 * @retval InputPtr Ukazatel na nově vytvořenou strukturu
 */
InputPtr Input_create(FILE *source)
{
    InputPtr input = (InputPtr) malloc(sizeof(Input));
    input->source = source;
    return input;
}

/**
 * Funkce pro zrušení struktury vstupních dat.
 *
 * @param[in,out]	InputPtr    *i  Ukazatel na existující strukturu
 */
void Input_destroy(InputPtr *i)
{
    free(i);
    *i = NULL;
}

#endif
