/* misjonisööjad.c :: Leiab heuristilist otsingut kasutades sammud, 
 * kuidas lahendada sisendiks antud misjonäride ja inimsööjate ülesannet.
 * Autor: Kristjan Kaitsa 
 * Tehisintellekt I (MTAT.06.008) @ 2012 A.D. */
#include "misjonisööjad.h"

void describePath(worldStatus *finalStatus);

int main(void) {
	inputValues *input;
	worldStatus *finalStatus;
	clock_t start, end;
	char newTask;

	// Üritame täpitähtede kuvamiseks Eesti kooditabelit määrata
	if (!setlocale(LC_CTYPE, "est")) {
      fprintf(stderr,	"VIGA! Teie systeemis ei 6nnestunud lokalisatsiooni seadistada. "
						"T2pit2htede kuvamine v6ib eba6nnestuda.\n\n");
    }

	printf(	"Misjonisööjad v0.1\n"
			"Autor: Kristjan Kaitsa\n");

	while (TRUE) {
		newTask = '\0';
		printf("\n");
		input = gatherInputValues();
		if (input != NULL) {
			printf("Palun oodake, otsin lahendust...\n");
			start = clock();
			finalStatus = solve(input);
			end = clock();
			describePath(finalStatus);

			printf("\n\nTulemuseni jõuti pärast %.3f sekundit ning %u seisundi läbivaatamist.\n", 
				(double)(end-start)/CLOCKS_PER_SEC, cycles);
			cleanUp();
		}

		while ((newTask != 'j') && (newTask != 'e')) {
			printf("Kas soovite sisestada uut ülesannet? (j/e): ");
			newTask = tolower(getchar());
			fflush(stdin);
		}
		if (newTask == 'e') break;
	}
	printf("\nSulgemiseks vajutage ENTERit.");
	getchar();
	return 0;
}

/* Liigub etteantud viida järgi (lõpp)seisundist tagasi algseisundisse
 * moodustades ning tagastades seisundite viitade vektori. */
__inline worldStatus** backTracePath(worldStatus *currentStatus) {
	int i;
	worldStatus *status;
	worldStatus **vector = (worldStatus**) malloc(sizeof(worldStatus*) * (currentStatus->moves + 1));

	status = currentStatus;
	for (i = currentStatus->moves; i >= 0; i--) {
		vector[i] = status;
		status = status->parent;
	}
	
	return vector;
}

// Kirjutab standardväljundisse etteantud seisundite viidavektori järgi vajalike sammude kirjeldused.
#ifndef SOLUTION_AS_TABLE
void describePath(worldStatus *finalStatus) {
	int i;
	worldStatus **vector;

	if (finalStatus == NULL) {
		printf("Ülesannet ei õnnestunud lahendada!\n");
		return;
	}
	printf("... lahendus leitud!\n\n");
	vector = backTracePath(finalStatus);

	for (i = 1; i <= finalStatus->moves; i++) {
		switch (vector[i]->boatLocation) {
		case LEFT_SHORE:
			printf("%d. Sõidutage vasakule kaldale: %d misjonär(i) ja %d inimsööja(t).\n",
				i,
				vector[i]->missionariesOnLeft - vector[i-1]->missionariesOnLeft,
				vector[i]->cannibalsOnLeft - vector[i-1]->cannibalsOnLeft
			);
			break;
		case RIGHT_SHORE:
			printf("%d. Sõidutage paremale kaldale: %d misjonär(i) ja %d inimsööja(t).\n",
				i,
				vector[i-1]->missionariesOnLeft - vector[i]->missionariesOnLeft,
				vector[i-1]->cannibalsOnLeft - vector[i]->cannibalsOnLeft
			);
			break;
		}
	}
	printf("\nVasakul kaldal %d misjonäri ja %d inimsööja(t).\n", 
		vector[i-1]->missionariesOnLeft, vector[i-1]->cannibalsOnLeft);
	printf("Paremal kaldal %d misjonäri ja %d inimsööja(t).\n", 
		taskSpecs->missionaries - vector[i-1]->missionariesOnLeft,
		taskSpecs->cannibals - vector[i-1]->cannibalsOnLeft);

	free(vector);
}
#endif

// Alternatiivne kirjeldusmeetod tabelina, ei ole alati ilusti joondatud.
#ifdef SOLUTION_AS_TABLE
void describePath(worldStatus *finalStatus) {
	int i;
	worldStatus **vector;

	if (finalStatus == NULL) {
		printf("Ülesannet ei õnnestunud lahendada!\n");
		return;
	}
	printf("... lahendus leitud!\n\n");
	vector = backTracePath(finalStatus);

	printf("Nr.\tVasak kallas\tPaat\t\t\tParem kallas\n");
	for (i = 1; i <= finalStatus->moves; i++) {
		switch (vector[i]->boatLocation) {
		case LEFT_SHORE:
			printf("%d.\tM:%d K:%d\t\t<== M:%d K:%d\t\tM:%d K:%d\n",
				i,
				vector[i-1]->missionariesOnLeft,
				vector[i-1]->cannibalsOnLeft,
				vector[i]->missionariesOnLeft - vector[i-1]->missionariesOnLeft,
				vector[i]->cannibalsOnLeft - vector[i-1]->cannibalsOnLeft,
				taskSpecs->missionaries - vector[i-1]->missionariesOnLeft - (vector[i]->missionariesOnLeft - vector[i-1]->missionariesOnLeft),
				taskSpecs->cannibals - vector[i-1]->cannibalsOnLeft - (vector[i]->cannibalsOnLeft - vector[i-1]->cannibalsOnLeft)
			);
			break;
		case RIGHT_SHORE:
			printf("%d.\tM:%d K:%d\t\tM:%d K:%d==>\t\tM:%d K:%d\n",
				i,
				vector[i-1]->missionariesOnLeft - (vector[i-1]->missionariesOnLeft - vector[i]->missionariesOnLeft),
				vector[i-1]->cannibalsOnLeft - (vector[i-1]->cannibalsOnLeft - vector[i]->cannibalsOnLeft),
				vector[i-1]->missionariesOnLeft - vector[i]->missionariesOnLeft,
				vector[i-1]->cannibalsOnLeft - vector[i]->cannibalsOnLeft,
				taskSpecs->missionaries - vector[i-1]->missionariesOnLeft,
				taskSpecs->cannibals - vector[i-1]->cannibalsOnLeft
			);
			break;
		}
	}
	printf("\nVasakul kaldal %d misjonäri ja %d inimsööja(t).\n", 
		vector[i-1]->missionariesOnLeft, vector[i-1]->cannibalsOnLeft);
	printf("Paremal kaldal %d misjonäri ja %d inimsööja(t).\n", 
		taskSpecs->missionaries - vector[i-1]->missionariesOnLeft,
		taskSpecs->cannibals - vector[i-1]->cannibalsOnLeft);

	free(vector);
}
#endif
