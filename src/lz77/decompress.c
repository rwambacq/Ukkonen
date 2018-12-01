#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "ukkonen.h"
#include "compress.h"

int decompress() {

	DecompInput* di = malloc(sizeof(DecompInput));

	uint32_t buffersize = 64;

	char* buffer = malloc(buffersize * sizeof(char));
	uint32_t pos = 0;

	while (fread(di, 9, 1, stdin)) {

		if (di->l== 0) {
			if (pos == buffersize) {
				buffer = realloc(buffer, buffersize * sizeof(char) * 2);
				buffersize *= 2;
			}
			buffer[pos] = (char)di->karakter;
			pos++;
		}
		else {
			for (uint32_t i = 0; i < di->l; i++) {
				if (pos == buffersize) {
					buffer = realloc(buffer, buffersize * sizeof(char) * 2);
					buffersize *= 2;
				}
				buffer[pos] = buffer[di->p + i];
				pos++;
			}
			if (pos == buffersize) {
				buffer = realloc(buffer, buffersize * sizeof(char) * 2);
				buffersize *= 2;
			}
			buffer[pos] = (char)di->karakter;
			pos++;
		}


	}

	if (pos == buffersize) {
		buffer = realloc(buffer, (buffersize + 1) * sizeof(char));
		buffersize += 1;
	}

	buffer[pos] = '\0';

	printf("%s", buffer);
	return 0;
}

int decompress_optimized(){

	uint32_t pos = 0;
	bool nogtegaan = true;
	DecompInput *di;

	while(nogtegaan) {

		nogtegaan = false;

		uint32_t buffersize = 64;

		char *buffer = malloc(buffersize * sizeof(char));

		di = malloc(sizeof(DecompInput));

		while (pos < MAX_BUFFER && fread(di, 9, 1, stdin)) {

			if (di->l == 0) {
				if (pos == buffersize) {
					buffer = realloc(buffer, buffersize * sizeof(char) * 2);
					buffersize *= 2;
				}
				buffer[pos] = (char) di->karakter;
				pos++;
			} else {
				for (uint32_t i = 0; i < di->l; i++) {
					if (pos == buffersize) {
						buffer = realloc(buffer, buffersize * sizeof(char) * 2);
						buffersize *= 2;
					}
					buffer[pos] = buffer[di->p + i];
					pos++;
				}
				if (pos == buffersize) {
					buffer = realloc(buffer, buffersize * sizeof(char) * 2);
					buffersize *= 2;
				}
				buffer[pos] = (char) di->karakter;
				pos++;
			}
		}

		if (pos == buffersize) {
			buffer = realloc(buffer, (buffersize + 1) * sizeof(char));
			buffersize += 1;
		}

		if(pos >= MAX_BUFFER){
			nogtegaan = true;
		}

		buffer[pos] = '\0';

		printf("%s", buffer);
		free(buffer);

	}
	return 0;
}