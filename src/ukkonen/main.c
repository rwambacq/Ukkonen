/**GIVEN*CODE*******************************************************************
 *
 * DA3 - Ukkonen voorbeeldcode
 *
 * Je mag deze code gebruiken binnen jouw project en zoveel aanpassen als je
 * zelf wilt. Bij vragen of problemen kun je de auteur contacteren.
 *
 * Door: Robbert Gurdeep Singh <Robbert.GurdeepSingh@UGent.be>
 */

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ukkonen.h"

#include "ukkonen.h"
int main(int argc, char* argv[]) {

	int buffersize = 64;
	int size = 0;
	char* stringsje = (char*)malloc(buffersize * sizeof(char));
	char lees;
	UkkonenTree* tree = ukkonen_make();


	while ((lees = (char)fgetc(stdin)) != EOF) {
		if (size == buffersize) {
			stringsje = realloc(stringsje, sizeof(char) * (buffersize * 2));
			buffersize *= 2;
		}
		stringsje[size] = lees;
		size++;
	}	
	if (size == buffersize) {
		stringsje = realloc(stringsje, sizeof(char) * (buffersize + 1));
		buffersize += 1;
	}
	printf("%c\n", stringsje[size - 1]);
	stringsje[size] = '\0';

	tree->data = stringsje;

	while (tree->data[tree->leaf_it + 1] != '\0') {
		ukkonen_ingest_next(tree);
	}

	ukkonen_print(tree);
	ukkonen_free(tree);
	return 0;

}
