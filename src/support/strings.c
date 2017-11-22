/**
 * Tento soubor obsahuje deklarace funkcí využité při implementaci
 * pomocných funkcí pro práci s textovými řetězci.
 *
 * @author Daniel Dolejška (xdolej08)
 * @date 21.11.2017
 * @project IFJcode17Parser
 * @subject Formální jazyky a překladače (IFJ) - FIT VUT v Brně
 */

#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "error_codes.h"

#include "strings.h"

#ifndef _strings_c
#define _strings_c

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
 * Alokuje textový řetězec.
 * Výsledný řetězec je nutné později uvolnit!
 *
 * @param[in]   const char  *str    Řetězec
 *
 * @retval char*    Alokovaný textový řetězec
 */
char *String_create(const char *str)
{
    char *output = malloc(sizeof(char) * ((str ? strlen(str) : 0) + 1));
    if (output == NULL)
    {
        DEBUG_ERR("string-create", "failed to mallocate string");
        return NULL;
    }

    if (str)
        strcpy(output, str);
    else
        output[0] = '\0';

    return output;
}

/**
 * K textovému řetězci přidá znak.
 * Funkce předpokládá, že vstupní řetězec ke konkatenaci je
 * alokovaný - provádí realokaci.
 *
 * @param[in,out]   char    **string    Textový řetězec ke konkatenaci
 * @param[in]       char    ch          Znak ke konkatenaci
 *
 * @retval  int Návratový kód popisující situaci (chyba, úspěch, ...)
 */
int String_addChar(char **string, char ch)
{
    char *output;
    if (*string == NULL)
        return;

    unsigned length = strlen(*string);
    output = malloc(sizeof(char) * (length + 2));
    if (*string == NULL)
    {
        return INTERNAL_ERROR;
    }

    if (length)
        strcpy(output, *string);
    String_destroy(string);

    output[length] = ch;
    output[length + 1] = '\0';

    *string = output;
}

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
char *String_concat(char *str1, char *str2, const char *strBetween)
{
    unsigned extra_size = 1;
    if (strBetween != NULL)
        extra_size+= strlen(strBetween);

    char *output = (char *) malloc(sizeof(char) * (strlen(str1) + strlen(str2) + extra_size));
    if (output == NULL)
        return NULL;

    strcpy(output, str1);
    if (strBetween != NULL)
        strcat(output, strBetween);
    strcat(output, str2);

    return output;
}

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
char *String_printf(char *subject, char *arg1, char *arg2, char *arg3, char *arg4)
{
    size_t sz = snprintf(NULL, 0, subject, arg1, arg2, arg3, arg4);
    char *output = (char *) malloc(sizeof(char) * (sz + 1));
    if (output == NULL)
        return NULL;

    snprintf(output, sz + 1, subject, arg1, arg2, arg3, arg4);
    return output;
}

/**
 * Uvolní alokovaný textový řetězec.
 *
 * @param[in,out]   char    **s Ukazatel na textový řetězec k uvolnění
 */
void String_destroy(char **s)
{
    free(*s);
    *s = NULL;
}

#endif

