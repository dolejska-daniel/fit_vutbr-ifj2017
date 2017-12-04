/**
 * Tento soubor obsahuje deklarace funkcí pro převod z binární, osmičkové čí
 * hexadecimální soustavy do decimální.
 * @author Lukáš Kulda (xkulda01)
 * @date 21.11.2017
 * @project IFJcode17Parser
 * @subject Formální jazyky a překladače (IFJ) - FIT VUT v Brně
 */

#include <string.h>
#include <stdio.h>
#include <math.h>

#ifdef DEBUG_INCLUDE
#include "../support/error_codes.h"
#else
#include "error_codes.h"
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

 /**
 * Převede hodnotu z binární soustavy do decimální.
 *
 * @param[in]   int value   vstupní hodnota
 *
 * @retval int  převedená hodnota
 */
int binary2decimal(int value)
{
     int decimal = 0;
     int remainder = 0;
     int exponent = 0;

     while (value != 0) {
        remainder = value % 10;
        value = value / 10;
        decimal = decimal + remainder * pow(2, exponent);
        exponent++;
     }
     return decimal;
}

  /**
 * Převede hodnotu z osmičkové soustavy do decimální.
 *
 * @param[in]   int value   vstupní hodnota
 *
 * @retval int  převedená hodnota
 */
int octa2decimal(int value)
{
     int decimal = 0;
     int remainder = 0;
     int exponent = 0;

     while (value != 0) {
        remainder = value % 10;
        value = value / 10;
        decimal = decimal + remainder * pow(8, exponent);
        exponent++;
     }
     return decimal;
}

  /**
 * Převede hodnotu z hexadecimální soustavy do decimální.
 *
 * @param[in]   int value   vstupní hodnota
 *
 * @retval int  převedená hodnota
 */
int hexa2decimal(char *value)
{
    int decimal = 0;
    int current = 0;
    int index = strlen(value) - 1;
    int exponent = 0;

    while (index >= 0) {
        if (value[index] >= '0' && value[index] <= '9') {
            current = value[index] - 48;
        }
        else if (value[index] >= 'A' && value[index] <= 'F') {
            current = value[index] - 55;
        }
        else if (value[index] >= 'a' && value[index] <= 'f') {
            current = value[index] - 87;
        }

        decimal = decimal + current * pow(16, exponent);
        exponent++;
        index--;
    }
    return decimal;
}

