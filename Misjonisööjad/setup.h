/* setup.h :: Kogub kasutajalt ülesande püstituse väärtused ning sooritab esmase kontrolli.
 * Autor: Kristjan Kaitsa 
 * Tehisintellekt I (MTAT.06.008) @ 2012 A.D. */
#pragma once
#include <stdio.h>
#include <stdlib.h>

typedef struct tagInputValues {
	unsigned short cannibals;
	unsigned short missionaries;
	unsigned short boatCapacity;
	unsigned short hasToRow;
} inputValues;

inputValues* gatherInputValues(void);
