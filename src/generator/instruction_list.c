/**
 * Tento soubor obsahuje deklarace funkcí využité při implementaci
 * generátoru cílového kódu (generator).
 *
 * @author Lukáš Kulda (xkulda01)
 * @date 21.11.2017
 * @project IFJcode17Parser
 * @subject Formální jazyky a překladače (IFJ) - FIT VUT v Brně
 */

#include "instruction_list.h"
#include <stdio.h>

#ifndef _instruction_list_c
#define _instruction_list_c

#ifdef DEBUG_INCLUDE
#include "../support/strings.h"
#include "../support/error_codes.h"
#else
#include "strings.h"
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

//==================================================================d=d=
//  DEKLARACE A DEFINICE ENUMERÁTORŮ A STRUKTUR
//======================================================================

//==================================================================d=d=
//  DEKLARACE FUNKCÍ
//======================================================================

/**
 * Naalokuje a inicializuje řídící strukturu InstructionList.
 *
 * @retval  InstructionListPtr  Ukazatel na nově vytvořenou strukturu
 */
InstructionListPtr InstructionList_create()
{
    InstructionListPtr list = (InstructionListPtr) malloc(sizeof(InstructionList)); // Alokace
    if (list == NULL) { // Neúspěch alokace
        DEBUG_ERR("instruction_list", "failed to allocate List");
        return NULL;
    }
    // Inicializace
    list->active = NULL;
    list->first = NULL;
    list->last = NULL;
    return list; // Vrácení úspěšně inicializovaného listu
}

/**
 * Zruší a uvolní řídící strukturu InstructionList. Pokud je neprázdná
 * (obsahuje neuvolněné prvky) jsou tyto prvky zrušeny a uvolněny.
 *
 * @param[in,out]   InstructionListPtr  *l  Ukazatel na strukturu k zrušení
 */
void InstructionList_destroy(InstructionListPtr *l)
{
    InstructionListPtr list = *l;
    InstructionPtr destroyed; // Proměnná pro rušenou instrukci
    while (list->first != NULL) { // Dokud máme instrukce, které můžeme rušit
        destroyed = list->first; // Uložení rušené instrukce
        list->first = destroyed->next; // Posunutí na další instrukci
        free(destroyed); // Uvolnění
    }
    // Inicializace
    list->active = NULL;
    list->last = NULL;

    free(list);
    *l = NULL;
}

/**
 * Vloží novou instrukci na začátek seznamu.
 *
 * @param[in,out]   InstructionListPtr  l           Ukazatel řídící strukturu seznamu
 * @param[in]       char                *content    Samotný obsah instrukce - textový řetězec
 *
 * @retval  InstructionPtr  Ukazatel na nově vytvořenou strukturu
 */
InstructionPtr InstructionList_insertFirst(InstructionListPtr l, char *content)
{
    InstructionPtr inserted = malloc(sizeof(Instruction)); // Proměnná pro vkládanou instrukci a její alokace
    if (inserted == NULL) { // Neúspěch alokace
        DEBUG_ERR("instruction_list", "failed to allocate Element of List");
        return NULL;
    }
    // Vkládáme na začátek seznamu
    inserted->content = content; // Přiřazení obsahu instrukce vkládanému
    inserted->next = l->first; // Původní první bude následující
    inserted->prev = NULL; // Předchozí vkládaného bude NULL, protože vkládaný je první
    if (l->first != NULL) { // Seznam není prázdný
        l->first->prev = inserted; // Předchozí původního prvního bude ukazovat na vkládaný
    }
    else { // Seznam je prázdný
        l->last = inserted; // Vkládaný bude zároveň poslední
    }
    l->first = inserted; // Nastavení vkládaného na první
    return inserted; // Vrácení vkládaného prvku
}

/**
 * Vloží novou instrukci na konec seznamu.
 *
 * @param[in,out]   InstructionListPtr  l           Ukazatel řídící strukturu seznamu
 * @param[in]       char                *content    Samotný obsah instrukce - textový řetězec
 *
 * @retval  InstructionPtr  Ukazatel na nově vytvořenou strukturu
 */
InstructionPtr InstructionList_insertLast(InstructionListPtr l, char *content)
{
    InstructionPtr inserted = malloc(sizeof(Instruction)); // Proměnná pro vkládanou instrukci a její alokace
    if (inserted == NULL) { // Neúspěch alokace
        DEBUG_ERR("instruction_list", "failed to allocate Element of List");
        return NULL;
    }
    // Vkládáme na konec seznamu
    inserted->content = content; // Přiřazení obsahu instrukce vkládanému
    inserted->prev = l->last; // Původní poslední bude předchozí
    inserted->next = NULL; // Následující vkládaného bude NULL, protože vkládaný je poslední
    if (l->last != NULL) { // Seznam není prázdný
        l->last->next = inserted; // Předchozí původního posledního bude ukazovat na vkládaný
    }
    else { // Seznam je prázdný
        l->first = inserted; // Vkládaný bude zároveň první
    }
    l->last = inserted; // Nastavení vkládaného na poslední
    return inserted; // Vrácení vkládaného prvku
}

/**
 * Nastaví první instrukci na aktivní.
 *
 * @param[in,out]   InstructionListPtr  l   Ukazatel řídící strukturu seznamu
 */
void InstructionList_first(InstructionListPtr l)
{
    l->active = l->first;
}

/**
 * Nastaví poslední instrukci na aktivní.
 *
 * @param[in,out]   InstructionListPtr  l   Ukazatel řídící strukturu seznamu
 */
void InstructionList_last(InstructionListPtr l)
{
    l->active = l->last;
}

/**
 * Nastaví následující instrukci na aktivní. Pokud žádná instrukce
 * není aktivní, neděje se nic.
 *
 * @param[in,out]   InstructionListPtr  l   Ukazatel řídící strukturu seznamu
 */
void InstructionList_next(InstructionListPtr l)
{
    if (l->active != NULL) { // Pokud seznam má nějakou instrukci, která je aktivní
        l->active = l->active->next; // Posunutí aktivity na následující instrukci
    }
}

/**
 * Získá obsah (samotnou instrukci - textový řetězec) první instrukce
 * v seznamu.
 *
 * @param[in]   InstructionListPtr  l   Ukazatel řídící strukturu seznamu
 *
 * @retval  char*  Obsah instrukce
 */
char *InstructionList_getFirst(InstructionListPtr l)
{
    return l->first->content;
}

/**
 * Získá obsah (samotnou instrukci - textový řetězec) poslední instrukce
 * v seznamu.
 *
 * @param[in]   InstructionListPtr  l   Ukazatel řídící strukturu seznamu
 *
 * @retval  char*  Obsah instrukce
 */
char *InstructionList_getLast(InstructionListPtr l)
{
    return  l->last->content;
}

/**
 * Získá obsah (samotnou instrukci - textový řetězec) aktivní instrukce
 * v seznamu.
 *
 * @param[in]   InstructionListPtr  l   Ukazatel řídící strukturu seznamu
 *
 * @retval  char*  Obsah instrukce
 */
char *InstructionList_getActive(InstructionListPtr l)
{
    if (l->active == NULL) { // Pokud seznam nemá instrukci, která je aktivní
        return NULL; // Vrácení prázdného obsahu
    }
    else { // Pokud seznam má nějakou instrukci, která je aktivní
        return l->active->content; // Vrácení obsahu aktivní instrukce
    }
}

/**
 * Zruší a uvolní první instrukci v seznamu.
 *
 * @param[in,out]   InstructionListPtr  l   Ukazatel řídící strukturu seznamu
 */
void InstructionList_deleteFirst(InstructionListPtr l)
{
    InstructionPtr destroyed; // Proměnná pro rušenou instrukci
    if (l->first != NULL) { // Seznam není prázdný
        destroyed = l->first; // Uložení rušené instrukce
        if (l->active == l->first) { // První instrukce je aktivní
            l->active = NULL; // Zrušení aktivity prvního
        }
        if (l->first == l->last) { // První instrukce je zároveň poslední (jedinná instrukce v seznamu)
            l->last = NULL; // Poslední bude NULL
        }
        l->first = l->first->next; // Aktualizace
        if (l->first != NULL) {
            l->first->prev = NULL; // Předchozí bude NULL
        }
        free(destroyed); // Uvolnění
    }
}

/**
 * Zruší a uvolní poslední instrukci v seznamu.
 *
 * @param[in,out]   InstructionListPtr  l   Ukazatel řídící strukturu seznamu
 */
void InstructionList_deleteLast(InstructionListPtr l)
{
    InstructionPtr destroyed; // Proměnná pro rušenou instrukci
    if (l->last != NULL) { // Seznam není prázdný
        destroyed = l->last; // Uložení rušené instrukce
        if (l->active == l->last) { // První instrukce je aktivní
            l->active = NULL; // Zrušení aktivity prvního
        }
        if (l->first == l->last) { // První instrukce je zároveň poslední (jedinná instrukce v seznamu)
            l->first = NULL; // První bude NULL
        }
        l->last = l->last->prev; // Aktualizace
        if (l->last != NULL) {
            l->last->next = NULL; // Následující bude NULL
        }
        free(destroyed); // Uvolnění
    }
}

/**
 * Zruší a uvolní aktivní instrukci v seznamu.
 *
 * @param[in,out]   InstructionListPtr  l   Ukazatel řídící strukturu seznamu
 */
void InstructionList_deleteActive(InstructionListPtr l)
{
    InstructionPtr destroyed;
    if(l->active != NULL) { // Seznam má aktivní instrukci
        destroyed = l->active; // Uložení rušené instrukce
        if (l->active == l->first) { // Aktivní je zároveň první
            l->first = l->first->next;
        }
        if (l->active == l->last) { // Aktivní je zároveň poslední
            l->last = l->last->prev;
        }
        if (l->active->next != NULL) // Aktivní má následující
        {
            l->active->next->prev = l->active->prev; // Předchozí ukazatel následujícího aktivního bude ukazovat na předchozí aktivního
        }
        if (l->active->prev != NULL) // Aktivní má předchozí
        {
            l->active->prev->next = l->active->next; // Následující ukazatel předchozího aktivního bude ukazovat na následující aktivního
        }

        free(destroyed); // Uvolnění
        l->active = NULL; // Aktualizace aktivního
    }
}

/**
 * Získá obsah (samotnou instrukci - textový řetězec) první instrukce
 * v seznamu a tu pak odstraní.
 *
 * @param[in,out]   InstructionListPtr  l   Ukazatel řídící strukturu seznamu
 */
char *InstructionList_getFirstAndDelete(InstructionListPtr l)
{
    char *content = InstructionList_getFirst(l);
    InstructionList_deleteFirst(l);
    return content; // Vrácení obsahu
}


#endif
