/* search.h :: Reaalne tuum heuristilsie otsingu sooritamiseks.
 * Autor: Kristjan Kaitsa 
 * Tehisintellekt I (MTAT.06.008) @ 2012 A.D. */
#pragma once
#include <string.h>
#include "Hashtable/hashTable.h"
#include "Heap/heap.h"
#include "setup.h"
#define FALSE 0
#define TRUE 1

typedef enum tagLocation {
	LEFT_SHORE,
	RIGHT_SHORE
} location;

typedef struct tagWorldStatus {
	struct tagWorldStatus *parent;		// Viit eelmisele seisundile
	unsigned short moves;				// Käikude arv siia seisundisse jõudmiseks

	unsigned short missionariesOnLeft;	// Misjonäre vasakul kaldal
	unsigned short cannibalsOnLeft;		// Inimsööjaid vasakul kaldal

	enum tagLocation boatLocation;		// Paadi asukoht
} worldStatus;

worldStatus* solve(inputValues *initValues);
__inline unsigned char isSolved(worldStatus *currentStatus);
unsigned int hashFunction(void *currentStatus);
unsigned char compareFunction(void *item1, void *item2);
void cleanUp(void);

extern inputValues *taskSpecs;
extern unsigned int cycles;