/**
 * Tento soubor obsahuje deklarace funkcí využité při implementaci
 * generátoru cílového kódu (generator).
 *
 * @author Daniel Dolejška (xdolej08)
 * @date 21.11.2017
 * @project IFJcode17Parser
 * @subject Formální jazyky a překladače (IFJ) - FIT VUT v Brně
 */

#ifndef _instruction_list_h
#define _instruction_list_h

#ifdef DEBUG_INCLUDE
#else
#endif

//==================================================================d=d=
//  DEKLARACE A DEFINICE ENUMERÁTORŮ A STRUKTUR
//======================================================================

typedef struct S_Instruction
    Instruction,
    *InstructionPtr;
struct S_Instruction {
    char            *content;   ///< Pole obsahující text instrukce
    InstructionPtr  prev;       ///< Ukazatel na předcházející instrukci
    InstructionPtr  next;       ///< Ukazatel na následující instrukci
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
 * @param[in,out]   InstructionListPtr  l           Ukazatel řídící strukturu seznamu
 * @param[in]       char                *content    Samotný obsah instrukce - textový řetězec
 *
 * @retval  InstructionPtr  Ukazatel na nově vytvořenou strukturu
 */
InstructionPtr InstructionList_insertFirst(InstructionListPtr l, char *content);

/**
 * Vloží novou instrukci na konec seznamu.
 *
 * @param[in,out]   InstructionListPtr  l           Ukazatel řídící strukturu seznamu
 * @param[in]       char                *content    Samotný obsah instrukce - textový řetězec
 *
 * @retval  InstructionPtr  Ukazatel na nově vytvořenou strukturu
 */
InstructionPtr InstructionList_insertLast(InstructionListPtr l, char *content);

/**
 * Nastaví první instrukci na aktivní.
 *
 * @param[in,out]   InstructionListPtr  l   Ukazatel řídící strukturu seznamu
 */
void InstructionList_first(InstructionListPtr l);

/**
 * Nastaví poslední instrukci na aktivní.
 *
 * @param[in,out]   InstructionListPtr  l   Ukazatel řídící strukturu seznamu
 */
void InstructionList_last(InstructionListPtr l);

/**
 * Nastaví následující instrukci na aktivní. Pokud žádná instrukce
 * není aktivní, neděje se nic.
 *
 * @param[in,out]   InstructionListPtr  l   Ukazatel řídící strukturu seznamu
 */
void InstructionList_next(InstructionListPtr l);

/**
 * Získá obsah (samotnou instrukci - textový řetězec) první instrukce
 * v seznamu.
 *
 * @param[in]   InstructionListPtr  l   Ukazatel řídící strukturu seznamu
 *
 * @retval  char*  Obsah instrukce
 */
char *InstructionList_getFirst(InstructionListPtr l);

/**
 * Získá obsah (samotnou instrukci - textový řetězec) poslední instrukce
 * v seznamu.
 *
 * @param[in]   InstructionListPtr  l   Ukazatel řídící strukturu seznamu
 *
 * @retval  char*  Obsah instrukce
 */
char *InstructionList_getLast(InstructionListPtr l);

/**
 * Získá obsah (samotnou instrukci - textový řetězec) aktivní instrukce
 * v seznamu.
 *
 * @param[in]   InstructionListPtr  l   Ukazatel řídící strukturu seznamu
 *
 * @retval  char*  Obsah instrukce
 */
char *InstructionList_getActive(InstructionListPtr l);

/**
 * Zruší a uvolní první instrukci v seznamu.
 *
 * @param[in,out]   InstructionListPtr  l   Ukazatel řídící strukturu seznamu
 */
void InstructionList_deleteFirst(InstructionListPtr l);

/**
 * Zruší a uvolní poslední instrukci v seznamu.
 *
 * @param[in,out]   InstructionListPtr  l   Ukazatel řídící strukturu seznamu
 */
void InstructionList_deleteLast(InstructionListPtr l);

/**
 * Zruší a uvolní aktivní instrukci v seznamu.
 *
 * @param[in,out]   InstructionListPtr  l   Ukazatel řídící strukturu seznamu
 */
void InstructionList_deleteActive(InstructionListPtr l);

/**
 * Získá obsah (samotnou instrukci - textový řetězec) aktivní instrukce
 * v seznamu a tu pak odstraní.
 *
 * @param[in,out]   InstructionListPtr  l   Ukazatel řídící strukturu seznamu
 */
char *InstructionList_getFirstAndDelete(InstructionListPtr l);


#endif
