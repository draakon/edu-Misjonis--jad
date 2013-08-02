/* misjonisööjad.h :: Leiab heuristilist otsingut kasutades sammud, 
 * kuidas lahendada sisendiks antud misjonäride ja inimsööjate ülesannet.
 * Autor: Kristjan Kaitsa 
 * Tehisintellekt I (MTAT.06.008) @ 2012 A.D. */
#include <stdio.h>
#include <locale.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include "setup.h"
#include "Heap/heap.h"
#include "Hashtable/hashTable.h"
#include "search.h"
//#define SOLUTION_AS_TABLE