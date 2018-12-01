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
#include "compress.h"

void print_wrong_input(){
    printf("Wrong number of arguments were given.\n");
    printf("Use flag -c to compress or flag -d to decompress.");
    printf("The -o flag can be added to either to optimize memory usage.");
}

int main(int argc, char* argv[]) {

    bool c = false;
    bool d = false;
    bool opt = false;

    if(argc == 1){
        print_wrong_input();
        return -1;
    }
    for(int i = 1; i < argc; i++){
        if(strcmp(argv[i], "-c") == 0){
            c = true;
        } else if(strcmp(argv[i], "-d") == 0){
            d = true;
        } else if(strcmp(argv[i], "-o") == 0){
            opt = true;
        } else {
            print_wrong_input();
            return -1;
        }
    }

    if(c && d){
        print_wrong_input();
        return -1;
    }

	if(c) {
	    if(opt){
	        compress_optimized();
	    } else {
            compress();
        }
    } else if(d) {
	    if(opt){
            decompress_optimized();
	    } else {
            decompress();
        }
    }
	return 0;

}
