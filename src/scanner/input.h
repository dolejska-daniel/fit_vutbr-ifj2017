/**
 * Tento soubor obsahuje deklarace funkcí využité při implementaci
 * lexikálního analyzátoru (scanner).
 *
 * @author Daniel Dolejška (xdolej08)
 * @project IFJcode17Parser
 */

#include <stdio.h>

#ifndef _input_h
#define _input_h

//==================================================================d=d=
//  DEKLARACE A DEFINICE ENUMERÁTORŮ A STRUKTUR
//======================================================================

typedef struct S_Input
    Input,
   *InputPtr;
struct S_Input {
    FILE    *source;
}; ///< Struktura se vstupními daty


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
InputPtr Input_create(FILE *source);

/**
 * Funkce pro zrušení struktury vstupních dat.
 *
 * @param[in,out]	InputPtr    *i  Ukazatel na existující strukturu
 *
 * @retval void
 */
void Input_destroy(InputPtr i);

#endif
