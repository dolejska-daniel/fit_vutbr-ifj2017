/**
 * Tento soubor obsahuje deklarace funkcí využité při implementaci
 * generátoru cílového kódu (generator).
 *
 * @author Daniel Dolejška (xdolej08)
 * @project IFJcode17Parser
 */

#ifndef _generator_h
#define _generator_h

#ifdef DEBUG
#define DEBUG_PRINT(...) do{ fprintf( stderr, __VA_ARGS__ ); } while( 0 )
#else
#define DEBUG_PRINT(...) do{ } while ( 0 )
#endif

//==================================================================d=d=
//  DEKLARACE A DEFINICE ENUMERÁTORŮ A STRUKTUR
//======================================================================

typedef struct S_InstructionStack
    InstructionStack,
    *InstructionStackPtr;
struct S_InstructionStack {
    char        **array;    ///< Pole obsahující jednotlivé instrukce
    unsigned    count;      ///< Aktuální počet instrukcí
    unsigned    size;       ///< Velikost stacku
    unsigned    sizeStep;   ///< Přidaná velikost při dosažení aktuální maximální velikosti
}; ///< Struktura pro ukládání instrukcí před výpisem


//==================================================================d=d=
//  DEKLARACE FUNKCÍ
//======================================================================

/**
 * Funkce pro vytvoření struktury pro uchovávání instrukcí.
 *
 * @param[in]   unsigned    size        Velikost alokované paměti (počet míst pro instrukce)
 * @param[in]   unsigned    sizeStep    Velikost přidané paměti při dosažení velikosti aktuálně alokované paměti
 *
 * @retval  InstructionStackPtr Ukazatel na nově vytvořenou strukturu
 */
InstructionStackPtr InstructionStack_create(unsigned size, unsigned sizeStep);

/**
 * Funkce pro zrušení struktury pro uchovávání instrukcí.
 *
 * @param[in,out]   InstructionStackPtr stack   Ukazatel na existující strukturu
 */
void InstructionStack_destroy(InstructionStackPtr stack);

/**
 * Přidá novou instrukci na specifikovaný stack instrukcí.
 *
 * Pokud by mělo dojít k přetečení, rozšíří velikost stacku o sizeStep.
 *
 * @param[in]   InstructionStackPtr stack   Ukazatel na existující strukturu uchovávání instrukcí
 * @param[in]   char                *data   Samotná instrukce k uložení
 */
void InstructionStack_addInstruction(InstructionStackPtr stack, char *data);

/**
 * Získá hodnotu nejspodnějšího prvku na stacku.
 *
 * @param[in]   InstructionStackPtr stack   Ukazatel na existující strukturu uchovávání instrukcí
 *
 * @retval char*    Uložená instrukce
 */
char *InstructionStack_getBottom(InstructionStackPtr stack);

/**
 * Odstraní nejspodnější prvek ve stacku.
 *
 * @param[in]   InstructionStackPtr stack   Ukazatel na existující strukturu uchovávání instrukcí
 */
void InstructionStack_popBottom(InstructionStackPtr stack);

#endif
