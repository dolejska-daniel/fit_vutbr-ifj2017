/**
 * Tento soubor obsahuje implementaci "zanořovacího seznamu" pomocí
 * dvousměrně vázaného seznamu.
 *
 * @author Daniel Dolejška (xdolej08)
 * @date 21.11.2017
 * @project IFJcode17Parser
 * @subject Formální jazyky a překladače (IFJ) - FIT VUT v Brně
 */

#include <stdio.h>

#include "nesting_list.h"

#ifndef _nesting_list_c
#define _nesting_list_c

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
//  DEKLARACE A DEFINICE ENUMERÁTORÙ A STRUKTUR
//======================================================================


//==================================================================d=d=
//  DEKLARACE FUNKCÍ
//======================================================================

//-------------------------------------------------d-d-
//  List operations
//-----------------------------------------------------

/**
 * Funkce vytvoří (alokuje) novou řídící strukturu pro NestingList.
 *
 * @retval  NestingListPtr  Ukazatel na nově vytvořenou strukturu
 */
NestingListPtr NestingList_create()
{
    NestingListPtr l = (NestingListPtr) malloc(sizeof(NestingList));
    if (l == NULL)
    {
        DEBUG_ERR("nesting_list", "failed to mallocate NestingList");
        return NULL;
    }

    l->first    = NULL;
    l->last     = NULL;
    l->active   = NULL;

    return l;
}

/**
 * Funkce zruší a uvolní existující řídící strukturu pro NestingList.
 * Pokud je neprázdná (obsahuje neuvolněné prvky) jsou tyto prvky zrušeny
 * a uvolněny.
 *
 * @param[in,out]   NestingListPtr  l   Ukazatel na existující řídící strukturu seznamu
 */
void NestingList_destroy(NestingListPtr *l)
{
    NestingListPtr list = *l;
    while (list->first != NULL)
        NestingList_deleteFirst(list);

    free(list);
    *l = NULL;
}

/**
 * Vloží novou úroveň na začátek seznamu.
 *
 * @param[in,out]   NestingListPtr  l       Ukazatel na řídící strukturu seznamu
 * @param[in]       NestingType     type    Samotný obsah úrovně - typ zanoření
 *
 * @retval  NestingLevelPtr Ukazatel na nově vytvořenou strukturu
 */
NestingLevelPtr NestingList_insertFirst(NestingListPtr l, NestingType type)
{
    //  Vytvoření nové struktury úrovně
    NestingLevelPtr level = NestingLevel_create(type);
    NestingLevelPtr first = l->first;

    if (first != NULL)
    {
        //  V seznamu již je první položka
        first->prev = level;
        level->next = first;
    }

    //  Navázání ukazatelů
    l->first = level;

    return l;
}

/**
 * Vloží novou úroveň na konec seznamu.
 *
 * @param[in,out]   NestingListPtr  l       Ukazatel na řídící strukturu seznamu
 * @param[in]       NestingType     type    Samotný obsah úrovně - typ zanoření
 *
 * @retval  NestingLevelPtr Ukazatel na nově vytvořenou strukturu
 */
NestingLevelPtr NestingList_insertLast(NestingListPtr l, NestingType type)
{
    //  Vytvoření nové struktury úrovně
    NestingLevelPtr level = NestingLevel_create(type);
    NestingLevelPtr last = l->last;

    if (last != NULL)
    {
        //  V seznamu již je poslední položka
        last->next  = level;
        level->prev = last;
    }

    //  Navázání ukazatelů
    l->last = level;

    return l;
}

/**
 * Nastaví první úroveň na aktivní.
 *
 * @param[in,out]   NestingListPtr  l   Ukazatel na řídící strukturu seznamu
 *
 * @retval  NestingLevelPtr|NULL    Ukazatel na právě aktivní strukturu
 */
NestingLevelPtr NestingList_first(NestingListPtr l)
{
    l->active = l->first;
    return l->active;
}

/**
 * Nastaví poslední úroveň na aktivní.
 *
 * @param[in,out]   NestingListPtr  l   Ukazatel na řídící strukturu seznamu
 *
 * @retval  NestingLevelPtr|NULL    Ukazatel na právě aktivní strukturu
 */
NestingLevelPtr NestingList_last(NestingListPtr l)
{
    l->active = l->last;
    return l->active;
}

/**
 * Vrátí ukazatel na právě aktivní úroveň seznamu.
 *
 * @param[in,out]   NestingListPtr  l   Ukazatel na řídící strukturu seznamu
 *
 * @retval  NestingLevelPtr|NULL    Ukazatel na právě aktivní strukturu
 */
NestingLevelPtr NestingList_active(NestingListPtr l)
{
    return l->active;
}

/**
 * Zruší a uvolní úroveň v seznamu na základě ukazatele na danou
 * úroveň.
 *
 * @param[in,out]   NestingListPtr  l       Ukazatel na řídící strukturu seznamu
 * @param[in,out]   NestingLevelPtr level   Ukazatel na strukturu úrovně v seznamu
 */
void NestingList_delete(NestingListPtr l, NestingLevelPtr level)
{
    if (level != NULL)
    {
        //  Položka existuje, je co smazat
        if (level == l->first && level == l->last)
        {
            //  Položka je první a zároveň poslední
            l->first = NULL;
            l->last  = NULL;
        }
        else
        {
            //  Položka není v seznamu sama
            if (level == l->first)
            {
                //  Položka je první
                l->first = level->next;
            }
            else if (level->prev != NULL)
            {
                //  Existuje předchozí položka
                level->prev->next = level->next;
                level->next->prev = level->prev;
            }

            if (level == l->last)
            {
                //  Položka je poslední
                l->last = level->prev;
            }
            else if (level->next != NULL)
            {
                //  Existuje další položka
                level->next->prev = level->prev;
                level->prev->next = level->next;
            }
        }

        //  Pokud je ostraňovaná položka aktivní, aktivita je zrušena
        if (l->active == level)
            l->active = NULL;

        //  Samotné uvolnění úrovně
        NestingLevel_destroy(&level);
    }
}

/**
 * Zruší a uvolní první úroveň v seznamu.
 *
 * @param[in,out]   NestingListPtr  l   Ukazatel na řídící strukturu seznamu
 */
void NestingList_deleteFirst(NestingListPtr l)
{
    NestingList_delete(l, l->first);
}

/**
 * Zruší a uvolní poslední úroveň v seznamu.
 *
 * @param[in,out]   NestingListPtr  l   Ukazatel na řídící strukturu seznamu
 */
void NestingList_deleteLast(NestingListPtr l)
{
    NestingList_delete(l, l->last);
}


//-------------------------------------------------d-d-
//  Level operations
//-----------------------------------------------------

/**
 * Funkce vytvoří (alokuje) novou úroveň seznamu.
 *
 * @param[in]   NestingType type    Typ zanoření úrovně
 *
 * @retval  NestingLevelPtr Ukazatel na nově vytvořenou strukturu
 */
NestingLevelPtr NestingLevel_create(NestingType type)
{
    NestingLevelPtr l = (NestingLevelPtr) malloc(sizeof(NestingLevel));
    if (l == NULL)
    {
        DEBUG_ERR("nesting_list", "failed to mallocate NestingLevel");
        return NULL;
    }

    l->type = type;
    l->next = NULL;
    l->prev = NULL;

    return l;
}

/**
 * Funkce zruší (uvolní) existující úroveň seznamu.
 *
 * @param[in,out]   NestingLevelPtr *l  Ukazatel na existující strukturu úrovně seznamu
 */
void NestingLevel_destroy(NestingLevelPtr *l)
{
    free(*l);
    *l = NULL;
}


//-------------------------------------------------d-d-
//  Special operations / Aliases
//-----------------------------------------------------

/**
 * Prohledá předcházející úrovně seznamu a zjistí, zda se zde nachází
 * alespoň jedna úroveň s daným typem.
 *
 * @param[in,out]   NestingListPtr  l       Ukazatel na řídící strukturu seznamu
 * @param[in]       NestingType     type    Samotný obsah úrovně - typ zanoření
 *
 * @retval  NestingLevelPtr|NULL    Ukazatel na nejbližší vyhledanou úroveň s daným typem
 */
NestingLevelPtr NestingList_isNestedIn(NestingListPtr l, NestingType type)
{
    NestingLevelPtr active = NestingList_active(l);
    NestingLevelPtr level;
    if (active != NULL)
    {
        if (active->type == type)
            return active;

        level = active->prev;
        while (level != NULL && level->type != type)
            level = level->prev;

        return level;
    }
    else
    {
        //  V seznamu nebyla nalezena aktivní úroveň, nelze vyhledávat
        return NULL;
    }
}

/**
 * Vytvoří novou úroveň v seznamu (automaticky jako poslední) a nastaví
 * ji jako aktivní.
 *
 * @param[in,out]   NestingListPtr  l       Ukazatel na řídící strukturu seznamu
 * @param[in]       NestingType     type    Samotný obsah úrovně - typ zanoření
 *
 * @retval  NestingLevelPtr|NULL    Ukazatel na právě aktivní úroveň v seznamu
 */
NestingLevelPtr NestingList_newLevel(NestingListPtr l, NestingType type)
{
    NestingList_insertLast(l, type);
    return NestingList_last(l);
}

/**
 * Odstraní poslední úroveň v seznamu a nastaví aktivitu na předcházející
 * úroveň.
 *
 * @param[in,out]   NestingListPtr  l       Ukazatel na řídící strukturu seznamu
 * @param[in]       NestingType     type    Samotný obsah úrovně - typ zanoření
 *
 * @retval  NestingLevelPtr|NULL    Ukazatel na právě aktivní úroveň v seznamu
 */
NestingLevelPtr NestingList_leaveCurrentLevel(NestingListPtr l)
{
    NestingList_delete(l, l->active);
    return NestingList_last(l);
}

#endif
