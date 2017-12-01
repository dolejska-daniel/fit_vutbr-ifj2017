/**
 * Tento soubor obsahuje deklarace funkcí využité při implementaci
 * lexikálního analyzátoru (scanner).
 *
 * @author Daniel Dolejška (xdolej08)
 * @author Petr Ullrich (xullri00)
 * @date 22.11.2017
 * @project IFJcode17Parser
 * @subject Formální jazyky a překladače (IFJ) - FIT VUT v Brně
 */

#include "token.h"
#include "input.h"

#ifndef _scanner_h
#define _scanner_h

#ifdef DEBUG_INCLUDE
#else
#endif



//==================================================================d=d=
//  DEKLARACE A DEFINICE ENUMERÁTORŮ A STRUKTUR
//======================================================================

typedef enum E_AutomataState {
    // Final states begin
	STATE_BEGIN,
    STATE_INTEGER,
    STATE_DOUBLE,
    STATE_DOUBLE_EXP,
    STATE_IDENTIFIER,
    STATE_STRING,
    STATE_DIV,
    STATE_BIN,
    STATE_OCT,
    STATE_HEX,
    // Final states end
    // States for INT begin
    STATE_INC_EXP,
    STATE_INC_DOT,
    STATE_INC_SIGN,
    // States for INT end
    // States for STRING begin
    STATE_INC_EXC,
    STATE_INC_STRING_BEGIN,
    STATE_INC_BACKSLASH,
    STATE_INC_ESC1,
    STATE_INC_ESC2,
    //States for STRING end
    //States for COMMENTS starts
    STATE_INC_COMMENT,
    STATE_INC_BLOCK_COMMENT,
    STATE_INC_BLOCK_COMMENT_APOS,
    //States for COMMENTS end
    STATE_INC_AMPERSAND,
    STATE_INC_B,
    STATE_INC_O,
    STATE_INC_H,
	//  TODO: Další stavy automatu pro lexikální analýzu
} AutomataState; ///< Stavy automatu


//==================================================================d=d=
//  DEKLARACE FUNKCÍ
//======================================================================

/**
 * Hlavní funkce ovládající lexikální analyzátor.
 *
 * Pokud se na stacku tokenů nachází nějaký vrácený token,
 * je vrácen dříve, než je započato získávání tokenu nového.
 *
 * Pokud nebude token z nějakého důvodu získán, v parametru token vrací
 * hodnotu NULL a dále vrací adekvátní stavový kód.
 *
 * @param[in,out]   InputPtr    input           Ukazatel na strukturu se vstupními daty
 * @param[out]      TokenPtr    *token          Ukazatel na získaný token
 *
 * @retval int  Kód se kterým bylo získávání nového tokenu ukončeno
 */
int Scanner_GetToken(InputPtr input, TokenPtr *token);

/**
 * Tato funkce "vrátí" získaný token. Uloží jej na stack
 * tokenů a při dalším volání funkce Scanner_GetToken vrátí
 * tento, ne nový token.
 *
 * @param[in,out]   InputPtr    input           Ukazatel na strukturu se vstupními daty
 * @param[out]      TokenPtr    *token          Ukazatel na získaný token
 *
 * @retval int  Kód se kterým bylo získávání nového tokenu ukončeno
 */
int Scanner_UngetToken(InputPtr input, TokenPtr *token);

#endif
