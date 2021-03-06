/**
 * Tento soubor obsahuje deklarace funkcí využité při implementaci
 * generátoru cílového kódu (generator).
 *
 * @author Daniel Dolejška (xdolej08)
 * @date 21.11.2017
 * @project IFJcode17Parser
 * @subject Formální jazyky a překladače (IFJ) - FIT VUT v Brně
 */

#include <stdbool.h>

#ifndef _instruction_list_h
#define _instruction_list_h

#ifdef DEBUG_INCLUDE
#include "../parser/symtable.h"
#else
#include "symtable.h"
#endif

//==================================================================d=d=
//  DEKLARACE A DEFINICE ENUMERÁTORŮ A STRUKTUR
//======================================================================

typedef struct S_Instruction
    Instruction,
    *InstructionPtr;
struct S_Instruction {
    char            *content;     ///< Pole obsahující text instrukce
    bool            isVariable;   ///< (pro postfix2isntruction) Je tato instrukce proměnná
    bool            isOperator;   ///< (pro postfix2isntruction) Je tato instrukce operace
    bool            isBlockBegin; ///< (pro postfix2isntruction) Je tato instrukce počátkem zpracovaného bloku
    bool            isBlockEnd;   ///< (pro postfix2isntruction) Je tato instrukce koncem zpracovaného bloku
    SymbolType      dataType;     ///< Datový typ tohoto bloku (proměnné)
    InstructionPtr  prev;         ///< Ukazatel na předcházející instrukci
    InstructionPtr  next;         ///< Ukazatel na následující instrukci
}; ///< Struktura pro ukládání jednotlivých instrukcí

typedef struct S_InstructionList
    InstructionList,
    *InstructionListPtr;
struct S_InstructionList {
    InstructionPtr  first;  ///< Ukazatel na první instrukci v seznamu
    InstructionPtr  last;   ///< Ukazatel na poslední instrukci v seznamu
    InstructionPtr  active; ///< Ukazatel na aktivní instrukci v seznamu
}; ///< Struktura pro ukládání instrukcí před výpisem


//==================================================================d=d=
//  DEKLARACE FUNKCÍ
//======================================================================

/**
 * Naalokuje a inicializuje řídící strukturu InstructionList.
 *
 * @retval  InstructionListPtr  Ukazatel na nově vytvořenou strukturu
 */
InstructionListPtr InstructionList_create();

/**
 * Zruší a uvolní řídící strukturu InstructionList. Pokud je neprázdná
 * (obsahuje neuvolněné prvky) jsou tyto prvky zrušeny a uvolněny.
 *
 * @param[in,out]   InstructionListPtr  *l  Ukazatel na strukturu k zrušení
 */
void InstructionList_destroy(InstructionListPtr *l);

/**
 * Vloží novou instrukci na začátek seznamu.
 *
 * @param[in,out]   InstructionListPtr  l           Ukazatel na řídící strukturu seznamu
 * @param[in]       char                *content    Samotný obsah instrukce - textový řetězec
 *
 * @retval  InstructionPtr  Ukazatel na nově vytvořenou strukturu
 */
InstructionPtr InstructionList_insertFirst(InstructionListPtr l, char *content);

/**
 * Vloží novou instrukci na konec seznamu.
 *
 * @param[in,out]   InstructionListPtr  l           Ukazatel na řídící strukturu seznamu
 * @param[in]       char                *content    Samotný obsah instrukce - textový řetězec
 *
 * @retval  InstructionPtr  Ukazatel na nově vytvořenou strukturu
 */
InstructionPtr InstructionList_insertLast(InstructionListPtr l, char *content);

/**
 * Vloží novou instrukci před danou instrukci v seznamu.
 *
 * @param[in,out]   InstructionListPtr  l           Ukazatel na řídící strukturu seznamu
 * @param[in,out]   InstructionPtr      active      Ukazatel na instrukci, za kterou bude nová instrukce vložena
 * @param[in]       char                *content    Samotný obsah instrukce - textový řetězec
 *
 * @retval  InstructionPtr  Ukazatel na nově vytvořenou strukturu
 */
InstructionPtr InstructionList_insertBefore(InstructionListPtr l, InstructionPtr active, char *content);

/**
 * Vloží novou instrukci za danou instrukci v seznamu.
 *
 * @param[in,out]   InstructionListPtr  l           Ukazatel na řídící strukturu seznamu
 * @param[in,out]   InstructionPtr      active      Ukazatel na instrukci, za kterou bude nová instrukce vložena
 * @param[in]       char                *content    Samotný obsah instrukce - textový řetězec
 *
 * @retval  InstructionPtr  Ukazatel na nově vytvořenou strukturu
 */
InstructionPtr InstructionList_insertAfter(InstructionListPtr l, InstructionPtr active, char *content);

/**
 * Nastaví první instrukci na aktivní.
 *
 * @param[in,out]   InstructionListPtr  l   Ukazatel na řídící strukturu seznamu
 */
void InstructionList_first(InstructionListPtr l);

/**
 * Nastaví poslední instrukci na aktivní.
 *
 * @param[in,out]   InstructionListPtr  l   Ukazatel na řídící strukturu seznamu
 */
void InstructionList_last(InstructionListPtr l);

/**
 * Nastaví následující instrukci na aktivní. Pokud žádná instrukce
 * není aktivní, neděje se nic.
 *
 * @param[in,out]   InstructionListPtr  l   Ukazatel na řídící strukturu seznamu
 */
void InstructionList_next(InstructionListPtr l);

/**
 * Získá první instrukci v seznamu.
 *
 * @param[in]   InstructionListPtr  l   Ukazatel na řídící strukturu seznamu
 *
 * @retval  InstructionPtr|NULL Instrukce
 */
InstructionPtr InstructionList_getFirst(InstructionListPtr l);

/**
 * Získá poslední instrukci v seznamu.
 *
 * @param[in]   InstructionListPtr  l   Ukazatel na řídící strukturu seznamu
 *
 * @retval  InstructionPtr|NULL Instrukce
 */
InstructionPtr InstructionList_getLast(InstructionListPtr l);

/**
 * Získá aktivní instrukci v seznamu.
 *
 * @param[in]   InstructionListPtr  l   Ukazatel na řídící strukturu seznamu
 *
 * @retval  InstructionPtr|NULL Instrukce
 */
InstructionPtr InstructionList_getActive(InstructionListPtr l);

/**
 * Získá obsah (samotnou instrukci - textový řetězec) první instrukce
 * v seznamu.
 *
 * @param[in]   InstructionListPtr  l   Ukazatel na řídící strukturu seznamu
 *
 * @retval  char*  Obsah instrukce
 */
char *InstructionList_getFirstContent(InstructionListPtr l);

/**
 * Získá obsah (samotnou instrukci - textový řetězec) poslední instrukce
 * v seznamu.
 *
 * @param[in]   InstructionListPtr  l   Ukazatel na řídící strukturu seznamu
 *
 * @retval  char*  Obsah instrukce
 */
char *InstructionList_getLastContent(InstructionListPtr l);

/**
 * Získá obsah (samotnou instrukci - textový řetězec) aktivní instrukce
 * v seznamu.
 *
 * @param[in]   InstructionListPtr  l   Ukazatel na řídící strukturu seznamu
 *
 * @retval  char*  Obsah instrukce
 */
char *InstructionList_getActiveContent(InstructionListPtr l);

void InstructionList_debugPrint(InstructionListPtr l);

/**
 * Zruší a uvolní první instrukci v seznamu.
 *
 * @param[in,out]   InstructionListPtr  l   Ukazatel na řídící strukturu seznamu
 */
void InstructionList_deleteFirst(InstructionListPtr l);

/**
 * Zruší a uvolní poslední instrukci v seznamu.
 *
 * @param[in,out]   InstructionListPtr  l   Ukazatel na řídící strukturu seznamu
 */
void InstructionList_deleteLast(InstructionListPtr l);

/**
 * Zruší a uvolní aktivní instrukci v seznamu.
 *
 * @param[in,out]   InstructionListPtr  l   Ukazatel na řídící strukturu seznamu
 */
void InstructionList_deleteActive(InstructionListPtr l);

/**
 * Získá obsah (samotnou instrukci - textový řetězec) první instrukce
 * v seznamu a tu pak odstraní.
 *
 * @param[in,out]   InstructionListPtr  l   Ukazatel na řídící strukturu seznamu
 */
char *InstructionList_getFirstAndDelete(InstructionListPtr l);


#endif
