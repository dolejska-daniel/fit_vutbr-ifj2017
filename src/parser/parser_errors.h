/**
 * Tento soubor obsahuje deklarace chybových kódů.
 *
 * @author Daniel Dolejška (xdolej08)
 * @project IFJcode17Parser
 */

#ifndef _parser_errors_h
#define _parser_errors_h

#ifdef DEBUG
#define DEBUG_PRINT(...) do{ fprintf( stderr, __VA_ARGS__ ); } while( 0 )
#else
#define DEBUG_PRINT(...) do{ } while ( 0 )
#endif

//==================================================================d=d=
//  DEKLARACE A DEFINICE ENUMERÁTORŮ A STRUKTUR
//======================================================================

typedef enum E_ParserError {
    NO_ERROR                    = 0,    ///< při překladu nedošlo k žádné chybě
    LEXICAL_ERROR               = 1,    ///< chyba v programu v rámci lexikální analýzy (chybná struktura aktuálního lexé-mu)
    SYNTAX_ERROR                = 2,    ///< chyba v programu v rámci syntaktické analýzy (chybná syntaxe programu)
    SEMANTICAL_DEFINITION_ERROR = 3,    ///< sémantická chyba v programu – nedefinovaná funkce/promenná, pokus o redefi-nici funkce/promenné, atd.
    SEMANTICAL_DATATYPE_ERROR   = 4,    ///< sémantická chyba typové kompatibility v aritmetických, řetezcových a relačních výrazech, příp. špatný počet či typ parametrů u volání funkce
    SEMANTICAL_OTHER_ERROR      = 6,    ///< ostatní sémantické chyby
    INTERNAL_ERROR              = 99,   ///< interní chyba překladace tj. neovlivněná vstupním programem (např. chyba alokace paměti, atd.)
} ParserError; ///< Chyby programu


//==================================================================d=d=
//  DEKLARACE FUNKCÍ
//======================================================================

#endif
