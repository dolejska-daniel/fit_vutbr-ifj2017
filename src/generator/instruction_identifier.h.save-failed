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

typedef enum E_InstructionType {
    IF,
    LOOP,
} InstructionType;

typedef struct S_InstructionID
    InstructionID,
    *InstructionIDPtr;
struct S_InstructionID {
    InstructionType type;
    unsigned        ID;
};

//==================================================================d=d=
//  DEKLARACE FUNKCÍ
//======================================================================

InstructionIDPtr InstructionID_create(InstructionType type, unsigned ID);

void InstructionID_destroy(InstructionIDPtr i);

#endif
