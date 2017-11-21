/**
 * Lexikální analyzátor (scanner).
 *
 * @author Petr Ullrich (xullri00)
 * @project IFJcode17Parser
 */

#include <stdio.h>
#ifdef DEBUG_INCLUDE
#include "../support/error_codes.h"
#else
#include "error_codes.h"
#endif // DEBUG_INCLUDE


#include "input.h"
#include "scanner.h"
#include "token.h"


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
//  DEKLARACE FUNKCÍ
//======================================================================

/**
 * Hlavní funkce ovládající lexikální analyzátor.
 *
 * Pokud nebude token z nějakého důvodu získán, v parametru token vrací
 * hodnotu NULL a dále vrací adekvátní stavový kód.
 *
 * @param[in,out]   InputPtr    input           Ukazatel na strukturu se vstupními daty
 * @param[out]      TokenPtr    *token          Ukazatel na získaný token
 *
 * @retval int  Kód se kterým bylo získávání nového tokenu ukončeno
 */
int Scanner_GetToken(InputPtr input, TokenPtr *token)
{
/** HOW TO

    char ch = fgetc(input->source);
    ungetc(ch, input->source);

    token = Token_create(IF, NULL);
    if(token == NULL)
    {
        return LEXICAL_ERROR;
    }
*/



}
