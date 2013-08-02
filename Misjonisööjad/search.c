/* search.c :: Reaalne tuum heuristilsie otsingu sooritamiseks.
 * Autor: Kristjan Kaitsa 
 * Tehisintellekt I (MTAT.06.008) @ 2012 A.D. */
#include "search.h"

inputValues *taskSpecs;
static worldStatus *initStatus;
static dArray *open;
static hashtable *closed;
unsigned int cycles;

// Genereerib kasutaja sisendi järgi loodud algandmete andmestruktuuri järgi esialgse seisundi.
__inline void generateInitStatus(inputValues *initValues) {
	taskSpecs = initValues;
	initStatus = (worldStatus*) malloc(sizeof(worldStatus));

	initStatus->cannibalsOnLeft = initValues->cannibals;
	initStatus->missionariesOnLeft = initValues->missionaries;
	initStatus->parent = NULL;
	initStatus->moves = 0;
	initStatus->boatLocation = LEFT_SHORE;
}

// Kontrollib, kas seisund on lahend.
__inline unsigned char isSolved(worldStatus *currentStatus) {
	if ((currentStatus->cannibalsOnLeft == 0) &&
		(currentStatus->missionariesOnLeft == 0))
		return TRUE;
	else
		return FALSE;
}

// Kontrollib, kas antud paat on lubatud: kannibalid ei söö misjonäre jne.
__inline unsigned char isLegalBoat(unsigned short missionaries, unsigned short cannibals) {
	if ((missionaries != 0) && (cannibals > missionaries)) return FALSE;
	if (missionaries+cannibals > taskSpecs->boatCapacity) return FALSE;
	if (missionaries+cannibals < taskSpecs->hasToRow) return FALSE;
	return TRUE;
}

// Kontrollib, kas kallas on lubatud: inimsööjaid ei ole misjonäridest rohkem.
__inline unsigned char isLegalShore(unsigned short missionaries, unsigned short cannibals) {
	if ((missionaries != 0) && (cannibals > missionaries)) return FALSE;
	else return TRUE;
}

// Genereerib järgmise seisundi vastavalt eelmisele seisundile ning paati paigutatud inimeste järgi.
__inline worldStatus* move(worldStatus *currentStatus, unsigned short missionaries, unsigned short cannibals) {
	worldStatus *newStatus = (worldStatus*) malloc(sizeof(worldStatus));

	memcpy(newStatus, currentStatus, sizeof(worldStatus));
	if (currentStatus->boatLocation == LEFT_SHORE) {
		newStatus->cannibalsOnLeft -= cannibals;
		newStatus->missionariesOnLeft -= missionaries;
		newStatus->boatLocation = RIGHT_SHORE;
	}
	else {
		newStatus->cannibalsOnLeft += cannibals;
		newStatus->missionariesOnLeft += missionaries;
		newStatus->boatLocation = LEFT_SHORE;
	}
	newStatus->moves++;
	newStatus->parent = currentStatus;

	return newStatus;
}

/* Hinnangufunktsioon: f(n)=g(n)+h(n)
 * g(n) - tehtud liigutuste (seisundite) arv
 * h(n) - ebatäpne (väiksem) hinnang minimaalsete vajalike käikude (seisundite) arvu jaoks  */
__inline unsigned short calculatePriority(worldStatus *statusToCalc) {
	unsigned short estimate;

	estimate = 2 * (statusToCalc->cannibalsOnLeft + statusToCalc->missionariesOnLeft) / (taskSpecs->boatCapacity);
	return (statusToCalc->moves) + estimate;
}

// Genereerib seisundile võimalikud järglased.
__inline void generateAndPushChildren(worldStatus *currentStatus) {
	unsigned short missionariesOnShore;
	unsigned short cannibalsOnShore;
	unsigned short missionariesInBoat;
	unsigned short cannibalsInBoat;
	unsigned short maxMissionariesInBoat;
	unsigned short maxCannibalsInBoat;
	worldStatus *newStatus;

	if (currentStatus->boatLocation == LEFT_SHORE) {
		missionariesOnShore = currentStatus->missionariesOnLeft;
		cannibalsOnShore = currentStatus->cannibalsOnLeft;
	}
	else {
		missionariesOnShore = taskSpecs->missionaries - currentStatus->missionariesOnLeft;
		cannibalsOnShore = taskSpecs->cannibals - currentStatus->cannibalsOnLeft;
	}
	
	// Optimeerimines
	maxMissionariesInBoat = (missionariesOnShore < taskSpecs->boatCapacity) ? missionariesOnShore : taskSpecs->boatCapacity;
	maxCannibalsInBoat = (cannibalsOnShore < taskSpecs->boatCapacity) ? cannibalsOnShore : taskSpecs->boatCapacity;
	// Genereerime paadile sisu
	for (missionariesInBoat = 0; missionariesInBoat <= maxMissionariesInBoat; missionariesInBoat++) {
		for (cannibalsInBoat = 0; cannibalsInBoat <= maxCannibalsInBoat; cannibalsInBoat++) {
			// Kontrollime, kas paat ja kaldad on legaalsed
			if ((isLegalBoat(missionariesInBoat, cannibalsInBoat)) &&
				(isLegalShore(missionariesOnShore - missionariesInBoat, cannibalsOnShore - cannibalsInBoat)) &&
				(isLegalShore(taskSpecs->missionaries - missionariesOnShore + missionariesInBoat, taskSpecs->cannibals - cannibalsOnShore + cannibalsInBoat))) {
					// Lisame kuhja
					newStatus = move(currentStatus, missionariesInBoat, cannibalsInBoat);
					heap_push(
						open,
						calculatePriority(newStatus),
						newStatus
					);			
			}
		}
	}
}

// Asub ülesannet lahendama vastavalt etteantud väärtustele.
worldStatus* solve(inputValues *initValues) {
	worldStatus *currentStatus;
	worldStatus *closedStatus;

	cycles = 0;
	open = heap_new();
	closed = hashtable_new(&hashFunction, 1000, &compareFunction);

	generateInitStatus(initValues);
	heap_push(open, calculatePriority(initStatus), initStatus);						// Lisame algoleku kuhja (avatud nimekiri)

	while (!heap_isEmpty(open)) {													// Tsüklis kuni kuhjas on elemente
		cycles++;																	// Statistilise informatsiooni jaoks
		currentStatus = (worldStatus*) heap_pop(open);								// Võtame kuhjast pealmise elemendi
		if (isSolved(currentStatus))												// Kontrollime, kas see olek on lahendus												
			return currentStatus;
		if (hashtable_contains(closed, currentStatus)) {							// Kontrollime, kas paisktabel (suletud nimekiri) sisaldab juba seda olekut
			closedStatus = (worldStatus*) hashtable_get(closed, currentStatus);
			if (currentStatus->moves >= closedStatus->moves) {							// Kui sisaldab, siis kontrollime, kas uude olekusse jõuti vähemate käikudega
				free(currentStatus);														// Kui ei, siis viskame uue oleku minema
				continue;
			}
			hashtable_remove(closed, closedStatus);										// Kui jah, siis eemaldame vana oleku
			free(closedStatus);
		}
		hashtable_put(closed, currentStatus);										// Lisame oleku paisktabelisse

		generateAndPushChildren(currentStatus);										// Genereerime järglased
	}
	return NULL;
}

// Räsifunktsioon olekutega paisktabelis manipuleerimiseks.
unsigned int hashFunction(void *currentStatus) {
	unsigned int hash;

	hash = 0;
	hash ^= ((worldStatus*)currentStatus)->cannibalsOnLeft;
	hash ^= ((worldStatus*)currentStatus)->missionariesOnLeft;
	hash ^= ((worldStatus*)currentStatus)->boatLocation;
	hash %= 1000;

	return hash;
}

// Kahe oleku võrdlemine.
unsigned char compareFunction(void *item1, void *item2) {
	worldStatus *status1;
	worldStatus *status2;

	status1 = (worldStatus*)item1;
	status2 = (worldStatus*)item2;
	if ((status1->cannibalsOnLeft == status2->cannibalsOnLeft) &&
		(status1->missionariesOnLeft == status2->missionariesOnLeft) &&
		(status1->boatLocation == status2->boatLocation))
		return TRUE;
	else return FALSE;
}

// Vabastab kõik ülesande lahendamiseks eraldatud mälu.
void cleanUp(void) {
	hashtable_destroyWithContent(closed);
	heap_destroyWithContent(open);
	free(taskSpecs);
}