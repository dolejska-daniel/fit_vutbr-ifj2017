/**
 * Tento soubor obsahuje implementaci zpracování postfixových
 * výrazů (generování instrukcí).
 *
 * @author Daniel Dolejška (xdolej08)
 * @date 21.11.2017
 * @project IFJcode17Parser
 * @subject Formální jazyky a překladače (IFJ) - FIT VUT v Brně
 */

#include <stdio.h>

#include "postfix2instructions.h"
#include "postfix_list.h"

#ifndef _postfix2instructions_c
#define _postfix2instructions_c

#ifdef DEBUG_INCLUDE
#include "../generator/instruction_list.h"
#else
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
//  DEKLARACE A DEFINICE ENUMERÁTORŮ A STRUKTUR
//======================================================================


//==================================================================d=d=
//  DEKLARACE FUNKCÍ
//======================================================================

int postfix2instructions_mathematical(InstructionListPtr ilist, PostfixListPtr *postfixList)
{

}

int postfix2instructions_logical(InstructionListPtr ilist, PostfixListPtr *postfixList)
{

}

#endif
