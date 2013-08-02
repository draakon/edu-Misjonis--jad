/* setup.c :: Kogub kasutajalt �lesande p�stituse v��rtused ning sooritab esmase kontrolli.
 * Autor: Kristjan Kaitsa 
 * Tehisintellekt I (MTAT.06.008) @ 2012 A.D. */
#include "setup.h"

// Funktsioon konkreetsele muutujale kasutajalt v��rtuse saamiseks.
unsigned short getIntFromUser(char* desc, char letter) {
	unsigned short value;

	printf(	"%s\n"
			"%c=", desc, letter);
	scanf_s("%hu", &value);
	fflush(stdin);

	return value;
}

// Kasutajalt sisendv��rtuse saamine ning nende esmane kontrollimine.
inputValues* gatherInputValues(void) {
	inputValues *values = (inputValues*) malloc(sizeof(inputValues));

	printf("Palun sisestage ainult positiivseid t�isarve!\n");

	values->missionaries = 
		getIntFromUser("1) palun sisestage misjon�ride arv", 'm');
	values->cannibals =
		getIntFromUser("2) palun sisestage inims��jate arv", 'n');

	if (values->cannibals > values->missionaries) {
		fprintf(stderr, "VIGA! Inims��jaid ei tohi misjon�ridest rohkem olla! (Tingimus: m>=n)");
		goto cleanUpOnError;
	}
	if (values->cannibals + values->missionaries == 0) {
		fprintf(stderr, "VIGA! Vasakul kaldal ei ole kedagi: seega �lesanne juba lahendatud.");
		goto cleanUpOnError;
	}

	values->boatCapacity =
		getIntFromUser("3) palun sisestage palju inimesi paati mahub", 'p');

	if (values->boatCapacity == 0) {
		fprintf(stderr, "VIGA! Sellise paadiga ei �nnestu kedagi �le viia.");
		goto cleanUpOnError;
	}

	values->hasToRow =
		getIntFromUser("4) palun sisestage mitut isikut on s�udmiseks vaja", 'r');

	if (values->hasToRow > values->boatCapacity) {
		fprintf(stderr, "VIGA! Minimaalne vajalik s�udjate arv ei mahu paati! (Tingimus: r<=p)");
		goto cleanUpOnError;
	}
	printf("\n");
	return values;

cleanUpOnError:
	fflush(stdin);
	free(values);
	printf("\n\n");
	return NULL;
}
