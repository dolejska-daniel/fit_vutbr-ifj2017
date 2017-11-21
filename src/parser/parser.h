/**
 * Tento soubor obsahuje deklarace funkcí využité při implementaci
 * syntaktického analyzátoru (parseru).
 *
 * @author Daniel Dolejška (xdolej08)
 * @project IFJcode17Parser
 */

#ifndef _parser_h
#define _parser_h

#ifdef DEBUG_INCLUDE
#include "../scanner/input.h"
#else
#include "input.h"
#endif

//==================================================================d=d=
//  DEKLARACE A DEFINICE ENUMERÁTORŮ A STRUKTUR
//======================================================================

//  FIXME: Z nějakého důvodu tohle nefunguje
/*
const char *KEYWORDS[] = {
	"AS", "ASC", "DECLARE", "DIM", "DO", "DOUBLE", "ELSE", "END",
	"CHR", "FUNCTION", "IF", "INPUT", "INTEGER", "LENGTH", "LOOP",
	"PRINT", "RETURN", "SCOPE", "STRING", "SUBSTR", "THEN", "WHILE",
	"AND", "BOOLEAN", "CONTINUE", "ELSEIF", "EXIT", "FALSE", "FOR",
	"NEXT", "NOT", "OR", "SHARED", "STATIC", "TRUE"
}; ///< Seznam rezervovaných klíčových slov*/


//==================================================================d=d=
//  DEKLARACE FUNKCÍ
//======================================================================

/**
 * Hlavní funkce ovládající překladač.
 *
 * @param[in,out]   InputPtr    input   Ukazatel na strukturu se vstupními daty
 *
 * @retval	int	Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Parser_ParseCode(InputPtr input);

#endif
