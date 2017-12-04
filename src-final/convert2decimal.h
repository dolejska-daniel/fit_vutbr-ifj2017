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

 /**
 * Převede hodnotu z binární soustavy do decimální.
 *
 * @param[in]   int value   vstupní hodnota
 *
 * @retval int  převedená hodnota
 */
 int binary2decimal(int value);


  /**
 * Převede hodnotu z osmičkové soustavy do decimální.
 *
 * @param[in]   int value   vstupní hodnota
 *
 * @retval int  převedená hodnota
 */
 int octa2decimal(int value);

  /**
 * Převede hodnotu z hexadecimální soustavy do decimální.
 *
 * @param[in]   int value   vstupní hodnota
 *
 * @retval int  převedená hodnota
 */
 int hexa2decimal(string value);
