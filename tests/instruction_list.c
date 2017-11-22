#include <stdio.h>
#include <stdlib.h>

#ifdef DEBUG_INCLUDE
#include "../generator/instruction_list.h"
#else
#include "instruction_list.h"
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
//  DEKLARACE FUNKCÍ
//======================================================================
void PrintInstructionList (InstructionListPtr list)
{
    InstructionPtr i = list->first;
    int index = 0;
    DEBUG_PRINT("[%s] -------------------", "main");
    while (i != NULL) {
        DEBUG_PRINT("\n[%s] Instruction %d: %s", "main", index, i->content);
        if (i == list->active && list->active != NULL) {
            DEBUG_PRINT("\t<< Active instruction");
        }
        i = i->next;
        index++;
    }
    DEBUG_PRINT("\n[%s] -------------------\n\n", "main");
}
/**
 * Hlavní funkce ovládající pøekladaè.
 *
 * @param	int		argc	Poèet vstupních argumentù
 * @param	char    **argv	Pole vstupních argumentù
 * @param	char    **argv	Pole vstupních argumentù
 */
int main(int argc, char **argv)
{
    DEBUG_LOG("main", "Starting program");
    DEBUG_LOG("main", "List testing:");
    InstructionListPtr List;

    DEBUG_LOG("main", "Creating list:");
    List = InstructionList_create();

    DEBUG_LOG("main", "Inserting as first SUB then ADD into list:");
    char *test_string1 = "SUB";
    char *test_string2 = "ADD";

    InstructionList_insertFirst(List, test_string1);
    InstructionList_insertFirst(List, test_string2);
    PrintInstructionList(List);

    DEBUG_LOG("main", "Destroying list:");
    InstructionList_destroy(&List);
    DEBUG_PRINT("[%s] List: %s\n\n", "main", List);

    DEBUG_LOG("main", "Creating list and inserting as last DIV then MUL into list:");
    char *test_string3 = "DIV";
    char *test_string4 = "MUL";

    List = InstructionList_create();
    InstructionList_insertLast(List, test_string3);
    InstructionList_insertLast(List, test_string4);
    PrintInstructionList(List);

    DEBUG_LOG("main", "Inserting as first VAR and as last MOD into list:");
    char *test_string5 = "VAR";
    char *test_string6 = "MOD";

    InstructionList_insertFirst(List, test_string5);
    InstructionList_insertLast(List, test_string6);
    PrintInstructionList(List);

    DEBUG_LOG("main", "Inserting MOVE, CALL, DEFVAR, CREATEFRAME as first into list:");
    char *test_string7 = "MOVE";
    char *test_string8 = "CALL";
    char *test_string9 = "DEFVAR";
    char *test_string10 = "CREATEFRAME";

    InstructionList_insertFirst(List, test_string7);
    InstructionList_insertFirst(List, test_string8);
    InstructionList_insertFirst(List, test_string9);
    InstructionList_insertFirst(List, test_string10);
    PrintInstructionList(List);

    DEBUG_LOG("main", "Setting first as active in list:");
    InstructionList_first(List);
    PrintInstructionList(List);

    DEBUG_LOG("main", "Deleting active in list:");
    InstructionList_deleteActive(List);
    PrintInstructionList(List);

    DEBUG_LOG("main", "Setting next as active in list:");
    InstructionList_first(List);
    InstructionList_next(List);
    PrintInstructionList(List);

    DEBUG_LOG("main", "Getting content of active in list:");
    char *content_active;
    content_active = InstructionList_getActive(List);
    DEBUG_PRINT("[%s] Content of active: %s\n\n", "main", content_active);

    DEBUG_LOG("main", "Deleting active in list:");
    InstructionList_deleteActive(List);
    PrintInstructionList(List);

    DEBUG_LOG("main", "Setting last as active in list:");
    InstructionList_last(List);
    PrintInstructionList(List);

    DEBUG_LOG("main", "Deleting active in list:");
    InstructionList_deleteActive(List);
    PrintInstructionList(List);

    DEBUG_LOG("main", "Setting next as active in list:");
    InstructionList_last(List);
    InstructionList_next(List);
    PrintInstructionList(List);

    DEBUG_LOG("main", "Getting content of first in list:");
    char *content_first;
    content_first = InstructionList_getFirst(List);
    DEBUG_PRINT("[%s] Content of first: %s\n\n", "main", content_first);

    DEBUG_LOG("main", "Getting content of last in list:");
    char *content_last;
    content_last = InstructionList_getLast(List);
    DEBUG_PRINT("[%s] Content of last: %s\n\n", "main", content_last);

    DEBUG_LOG("main", "Getting content of active in list:");
    content_active = InstructionList_getActive(List);
    DEBUG_PRINT("[%s] Content of active: %s\n\n", "main", content_active);

    DEBUG_LOG("main", "Deleting first in list:");
    InstructionList_deleteFirst(List);
    PrintInstructionList(List);

    DEBUG_LOG("main", "Deleting last in list:");
    InstructionList_deleteLast(List);
    PrintInstructionList(List);

    DEBUG_LOG("main", "Deleting active in list:");
    InstructionList_deleteActive(List);
    PrintInstructionList(List);

    DEBUG_LOG("main", "Getting first and then deleting first in list:");
    DEBUG_PRINT("[%s] First: %s\n", "main", InstructionList_getFirstAndDelete(List));
    PrintInstructionList(List);

    return 0;
}
