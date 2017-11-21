/**
 * Tento soubor obsahuje deklarace funkcí využité při implementaci
 * pomocných funkcí pro práci s textovými řetězci.
 *
 * @author Daniel Dolejška (xdolej08)
 * @date 21.11.2017
 * @project IFJcode17Parser
 * @subject Formální jazyky a překladače (IFJ) - FIT VUT v Brně
 */

#include <string.h>
#include <malloc.h>

#ifndef _strings_h
#define _strings_h

#ifdef DEBUG_INCLUDE
#else
#endif

//==================================================================d=d=
//  DEKLARACE A DEFINICE ENUMERÁTORŮ A STRUKTUR
//======================================================================


//==================================================================d=d=
//  DEKLARACE FUNKCÍ
//======================================================================

/**
 * Alokuje textový řetězec.
 * Výsledný řetězec je nutné později uvolnit!
 *
 * @param[in]   const char  *str    Řetězec
 *
 * @retval char*    Alokovaný textový řetězec
 */
char *String_create(const char *str);

/**
 * Spojí dva textové řetězce do jednoho.
 * Výsledný řetězec je nutné později uvolnit!
 *
 * @param[in]   char        *str1       Vstupní textový řetězec 1
 * @param[in]   char        *str2       Vstupní textový řetězec 2
 * @param[in]   const char  *strBetween Lepidlo, řetězec, který bude vložen mezi spojené textové řetězce
 *
 * @retval char*    Výsledný konkatenovaný textový řetězec
 */
char *String_concat(char *str1, char *str2, const char *strBetween);

/**
 * "printf do stringu". Možnost uložit výstup printf do textového řetězce.
 * Výsledný řetězec je nutné později uvolnit!
 *
 * @param[in]   char    *subject    Hlavní textový řetězec pro printf
 * @param[in]   char    *arg1       Vedlejší textový řetězec, slouží k nahrazení proměnných v hlavním řetězci
 * @param[in]   char    *arg2       Vedlejší textový řetězec, slouží k nahrazení proměnných v hlavním řetězci
 * @param[in]   char    *arg3       Vedlejší textový řetězec, slouží k nahrazení proměnných v hlavním řetězci
 * @param[in]   char    *arg4       Vedlejší textový řetězec, slouží k nahrazení proměnných v hlavním řetězci
 *
 * @retval char*    Výsledný textový řetězec
 */
char *String_printf(char *subject, char *arg1, char *arg2, char *arg3, char *arg4);

/**
 * Uvolní alokovaný textový řetězec.
 *
 * @param[in,out]   char    **s Ukazatel na textový řetězec k uvolnění
 */
void String_destroy(char **s);

#endif
