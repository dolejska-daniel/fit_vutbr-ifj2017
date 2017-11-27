/**
 * Tento soubor obsahuje deklarace funkcí využité při implementaci
 * generátoru cílového kódu (generator).
 *
 * @author Lukáš Kulda (xkulda01), Daniel Dolejška (xdolej08)
 * @date 21.11.2017
 * @project IFJcode17Parser
 * @subject Formální jazyky a překladače (IFJ) - FIT VUT v Brně
 */

#include <stdio.h>
#include <stdbool.h>

#include "instruction_list.h"

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
        String_destroy(&(destroyed->content)); // Uvolnění textu instrukce (je alokovaná)
        free(destroyed); // Uvolnění
    }

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
 * Vloží novou instrukci před danou instrukci v seznamu.
 *
 * @param[in,out]   InstructionListPtr  l           Ukazatel na řídící strukturu seznamu
 * @param[in,out]   InstructionPtr      active      Ukazatel na instrukci, za kterou bude nová instrukce vložena
 * @param[in]       char                *content    Samotný obsah instrukce - textový řetězec
 *
 * @retval  InstructionPtr  Ukazatel na nově vytvořenou strukturu
 */
InstructionPtr InstructionList_insertBefore(InstructionListPtr l, InstructionPtr active, char *content)
{
    if (active == NULL)
    {
        DEBUG_ERR("instruction_list-insertBefore", "cannot insert before active, when none active!");
        return NULL;
    }

    InstructionPtr i = malloc(sizeof(Instruction));
    if (i == NULL)
    {
        DEBUG_ERR("instruction_list", "failed to allocate Element of List");
        return NULL;
    }

    i->content = content;
    if (active->prev)
    {
        //  Existuje předchozí prvek
        active->prev->next = i;
        i->prev = active->prev;
    }
    else
    {
        //  Neexistuje předchozí prvek, instrukce bude tedy první v seznamu
        l->first = i;
        i->prev = NULL;
    }

    i->next = active;
    active->prev = i;

    return i;
}

/**
 * Vloží novou instrukci za danou instrukci v seznamu.
 *
 * @param[in,out]   InstructionListPtr  l           Ukazatel na řídící strukturu seznamu
 * @param[in,out]   InstructionPtr      active      Ukazatel na instrukci, za kterou bude nová instrukce vložena
 * @param[in]       char                *content    Samotný obsah instrukce - textový řetězec
 *
 * @retval  InstructionPtr  Ukazatel na nově vytvořenou strukturu
 */
InstructionPtr InstructionList_insertAfter(InstructionListPtr l, InstructionPtr active, char *content)
{
    if (active == NULL)
    {
        DEBUG_ERR("instruction_list-insertAfter", "cannot insert after active, when none active!");
        return NULL;
    }

    InstructionPtr i = malloc(sizeof(Instruction));
    if (i == NULL)
    {
        DEBUG_ERR("instruction_list", "failed to allocate Element of List");
        return NULL;
    }

    i->content = content;
    if (active->next)
    {
        //  Existuje následující prvek
        active->next->prev = i;
        i->next = active->next;
    }
    else
    {
        //  Neexistuje následující prvek, instrukce bude tedy poslední v seznamu
        l->last = i;
        i->next = NULL;
    }

    i->prev = active;
    active->next = i;

    return i;
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
    inserted->isBlockBegin = false;
    inserted->isBlockEnd = false;
    inserted->isVariable = false;
    inserted->isOperator = false;
    inserted->dataType = -1;
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
 * Získá první instrukci v seznamu.
 *
 * @param[in]   InstructionListPtr  l   Ukazatel na řídící strukturu seznamu
 *
 * @retval  InstructionPtr|NULL Instrukce
 */
InstructionPtr InstructionList_getFirst(InstructionListPtr l)
{
    return l->first;
}

/**
 * Získá poslední instrukci v seznamu.
 *
 * @param[in]   InstructionListPtr  l   Ukazatel na řídící strukturu seznamu
 *
 * @retval  InstructionPtr|NULL Instrukce
 */
InstructionPtr InstructionList_getLast(InstructionListPtr l)
{
    return  l->last;
}

/**
 * Získá aktivní instrukci v seznamu.
 *
 * @param[in]   InstructionListPtr  l   Ukazatel na řídící strukturu seznamu
 *
 * @retval  InstructionPtr|NULL Instrukce
 */
InstructionPtr InstructionList_getActive(InstructionListPtr l)
{
    return l->active;
}

/**
 * Získá obsah (samotnou instrukci - textový řetězec) první instrukce
 * v seznamu.
 *
 * @param[in]   InstructionListPtr  l   Ukazatel řídící strukturu seznamu
 *
 * @retval  char*  Obsah instrukce
 */
char *InstructionList_getFirstContent(InstructionListPtr l)
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
char *InstructionList_getLastContent(InstructionListPtr l)
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
char *InstructionList_getActiveContent(InstructionListPtr l)
{
    if (l->active == NULL) { // Pokud seznam nemá instrukci, která je aktivní
        return NULL; // Vrácení prázdného obsahu
    }
    else { // Pokud seznam má nějakou instrukci, která je aktivní
        return l->active->content; // Vrácení obsahu aktivní instrukce
    }
}

void InstructionList_debugPrint(InstructionListPtr l)
{
    #ifdef DEBUG_PRINT_INSTRUCTIONS
    InstructionPtr active = l->active;
    InstructionPtr i;
    fprintf(stderr, "DEBUG | InstructionList (%p): {\n", l);
    InstructionList_first(l);
    i = l->active;
    while (i != NULL)
    {
        fprintf(stderr, "\t%s (%p): {\n", i->content, i);
        fprintf(stderr, "\t\ttype: %s\n", SymbolType_toString(i->dataType));
        fprintf(stderr, "\t\tisBlockBegin: %i\n", i->isBlockBegin);
        fprintf(stderr, "\t\tisBlockEnd: %i\n", i->isBlockEnd);
        fprintf(stderr, "\t\tisOperator: %i\n", i->isOperator);
        fprintf(stderr, "\t\tisVariable: %i\n", i->isVariable);
        fprintf(stderr, "\t\tnext: %p\n", i->next);
        fprintf(stderr, "\t\tprev: %p\n", i->prev);
        fprintf(stderr, "\t}\n", i->content);
        InstructionList_next(l);
        i = InstructionList_getActive(l);
    }
    fprintf(stderr, "}\n");
    l->active = active;
    #endif
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
    char *content = InstructionList_getFirstContent(l);
    InstructionList_deleteFirst(l);
    return content; // Vrácení obsahu
}


#endif
