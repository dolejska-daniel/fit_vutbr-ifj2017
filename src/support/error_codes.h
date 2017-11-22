/**
 * Tento soubor obsahuje deklarace chybových kódů.
 *
 * @author Daniel Dolejška (xdolej08)
 * @date 21.11.2017
 * @project IFJcode17Parser
 * @subject Formální jazyky a překladače (IFJ) - FIT VUT v Brně
 */

#ifndef _error_codes_h
#define _error_codes_h

#ifdef DEBUG_INCLUDE
#else
#endif

//==================================================================d=d=
//  DEKLARACE A DEFINICE ENUMERÁTORŮ A STRUKTUR
//======================================================================

typedef enum E_ErrorCodes {
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
