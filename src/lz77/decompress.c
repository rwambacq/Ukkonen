#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "ukkonen.h"
#include "compress.h"

void decompress() {
	
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

	printf(buffer);
}