/**
 * Tento soubor obsahuje deklarace funkcí využité při implementaci
 * syntaktického analyzátoru (parseru).
 *
 * @author Daniel Dolejška
 */
 
#ifndef _parser_h
#define _parser_h
#define KEYWORD_ARRAY_SIZE 35

//==================================================================d=d=
//  DEKLARACE A DEFINICE ENUMERÁTORŮ A STRUKTUR
//======================================================================

char *KEYWORDS[KEYWORD_ARRAY_SIZE] = {
	"AS", "ASC", "DECLARE", "DIM", "DO", "DOUBLE", "ELSE", "END",
	"CHR", "FUNCTION", "IF", "INPUT", "INTEGER", "LENGTH", "LOOP",
	"PRINT", "RETURN", "SCOPE", "STRING", "SUBSTR", "THEN", "WHILE",
	"AND", "BOOLEAN", "CONTINUE", "ELSEIF", "EXIT", "FALSE", "FOR",
	"NEXT", "NOT", "OR", "SHARED", "STATIC", "TRUE"
}; ///< Seznam rezervovaných klíčových slov


//==================================================================d=d=
//  DEKLARACE FUNKCÍ
//======================================================================

/**
 * Hlavní funkce ovládající překladač.
 *
 * @param	
 *
 * @retval	int	Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int Parser_ParseCode();

#endif