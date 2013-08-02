/* misjonis��jad.c :: Leiab heuristilist otsingut kasutades sammud, 
 * kuidas lahendada sisendiks antud misjon�ride ja inims��jate �lesannet.
 * Autor: Kristjan Kaitsa 
 * Tehisintellekt I (MTAT.06.008) @ 2012 A.D. */
#include "misjonis��jad.h"

void describePath(worldStatus *finalStatus);

int main(void) {
	inputValues *input;
	worldStatus *finalStatus;
	clock_t start, end;
	char newTask;

	// �ritame t�pit�htede kuvamiseks Eesti kooditabelit m��rata
	if (!setlocale(LC_CTYPE, "est")) {
      fprintf(stderr,	"VIGA! Teie systeemis ei 6nnestunud lokalisatsiooni seadistada. "
						"T2pit2htede kuvamine v6ib eba6nnestuda.\n\n");
    }

	printf(	"Misjonis��jad v0.1\n"
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

			printf("\n\nTulemuseni j�uti p�rast %.3f sekundit ning %u seisundi l�bivaatamist.\n", 
				(double)(end-start)/CLOCKS_PER_SEC, cycles);
			cleanUp();
		}

		while ((newTask != 'j') && (newTask != 'e')) {
			printf("Kas soovite sisestada uut �lesannet? (j/e): ");
			newTask = tolower(getchar());
			fflush(stdin);
		}
		if (newTask == 'e') break;
	}
	printf("\nSulgemiseks vajutage ENTERit.");
	getchar();
	return 0;
}

/* Liigub etteantud viida j�rgi (l�pp)seisundist tagasi algseisundisse
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

// Kirjutab standardv�ljundisse etteantud seisundite viidavektori j�rgi vajalike sammude kirjeldused.
#ifndef SOLUTION_AS_TABLE
void describePath(worldStatus *finalStatus) {
	int i;
	worldStatus **vector;

	if (finalStatus == NULL) {
		printf("�lesannet ei �nnestunud lahendada!\n");
		return;
	}
	printf("... lahendus leitud!\n\n");
	vector = backTracePath(finalStatus);

	for (i = 1; i <= finalStatus->moves; i++) {
		switch (vector[i]->boatLocation) {
		case LEFT_SHORE:
			printf("%d. S�idutage vasakule kaldale: %d misjon�r(i) ja %d inims��ja(t).\n",
				i,
				vector[i]->missionariesOnLeft - vector[i-1]->missionariesOnLeft,
				vector[i]->cannibalsOnLeft - vector[i-1]->cannibalsOnLeft
			);
			break;
		case RIGHT_SHORE:
			printf("%d. S�idutage paremale kaldale: %d misjon�r(i) ja %d inims��ja(t).\n",
				i,
				vector[i-1]->missionariesOnLeft - vector[i]->missionariesOnLeft,
				vector[i-1]->cannibalsOnLeft - vector[i]->cannibalsOnLeft
			);
			break;
		}
	}
	printf("\nVasakul kaldal %d misjon�ri ja %d inims��ja(t).\n", 
		vector[i-1]->missionariesOnLeft, vector[i-1]->cannibalsOnLeft);
	printf("Paremal kaldal %d misjon�ri ja %d inims��ja(t).\n", 
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
		printf("�lesannet ei �nnestunud lahendada!\n");
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
	printf("\nVasakul kaldal %d misjon�ri ja %d inims��ja(t).\n", 
		vector[i-1]->missionariesOnLeft, vector[i-1]->cannibalsOnLeft);
	printf("Paremal kaldal %d misjon�ri ja %d inims��ja(t).\n", 
		taskSpecs->missionaries - vector[i-1]->missionariesOnLeft,
		taskSpecs->cannibals - vector[i-1]->cannibalsOnLeft);

	free(vector);
}
#endif
