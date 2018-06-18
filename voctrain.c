#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
typedef struct translation {
	char* voc;
	char* trans;
} Translation;

typedef struct translationReturn {
	Translation* vocs;
	unsigned int size;
} TransReturn;

unsigned int numberOfLines(FILE* fd) {
	unsigned int i = 0;
	char dump[250];

	while(fgets(dump, 250, fd) != NULL) {
		i++;
	}

	rewind(fd);

	return i / 2; 
}

void deleteNewLine(char* string) {
	unsigned int size = strlen(string);
	if(strlen > 0 && string[size-1] == '\n') {
		string[size-1] = '\0';
	}
}

TransReturn* readFile(char* fileName) {
	FILE *fd = fopen(fileName, "r");
	
	if(fd == NULL) {
		return NULL;
	}
	
	unsigned int size = numberOfLines(fd);

	Translation* vocs = (Translation*) malloc(size*sizeof(Translation));

	for(unsigned int i = 0; i < size; i++) {
		vocs[i].voc = (char*) malloc(250*sizeof(char));
		fgets(vocs[i].voc, 999, fd);
		deleteNewLine(vocs[i].voc);
		vocs[i].trans = (char*) malloc(250*sizeof(char));
	   	fgets(vocs[i].trans, 999, fd);	
		deleteNewLine(vocs[i].trans);
	}

	TransReturn *tReturn = (TransReturn*) malloc(sizeof(TransReturn));

	tReturn->size = size;
	tReturn->vocs = vocs;

	return tReturn;
}

void printCommendation() {
	printf("Beebo Beebo\n");
}

void printInsult(char* rightAnswer) {
	printf("ITS %s YOU DUMB PIECE OF SHIT\n", rightAnswer);
}

int main(int argc, char **argv) {
	TransReturn *tReturn;
	if(argc < 2) {
		tReturn = readFile("./japvoc.txt");
	} else {
		tReturn = readFile(*(++argv));
	}	

	if(tReturn == NULL) {
		exit(1);
	}

	Translation* vocs = tReturn->vocs;
	unsigned int size = tReturn->size;
	char* question; 
	char* answer = (char*) malloc(250*sizeof(char));
	unsigned int offset;
	time_t t;
	srand((unsigned) time(&t));

	while(1) {
		offset = rand() % size;
		question = vocs[offset].voc;

		printf("%s\n", question);

		fgets(answer, 250, stdin);
		deleteNewLine(answer);

		if(strcmp(answer, vocs[offset].trans) != 0) {
			printInsult(vocs[offset].trans);
		} else {
			printCommendation();
		}
	}
}
