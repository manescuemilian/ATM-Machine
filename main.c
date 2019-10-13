#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "functii.h"

/* MANESCU Emilian-Claudiu - 313 CB */

int main() {
	LC *lista = NULL;
	char buf[1000], fisier[1000][1000];
	int linii_fisier = 0, nr_carduri;
	FILE *input, *output;
	input = fopen ("input.in", "r");
	if (input == NULL) {
		return -1;
	}

	fscanf(input, "%d", &nr_carduri);

	while (fgets (buf, 1000, input)) {
		strcpy(fisier[linii_fisier], buf);
		linii_fisier++;
	}
	fclose(input);

	output = fopen ("output.out", "w");
	if (output == NULL) {
		return -1;
	}

	executare_comenzi(fisier, linii_fisier, &nr_carduri, &lista, output);
	fclose (output);
	eliberareLC (&lista);
	return 0;
}