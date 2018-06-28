#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

typedef struct vocNode {
	char *foreignWord;
	char *translation;
	struct vocNode* next;
	struct vocNode* prev;
} Vocable;

typedef struct translationReturn {
	Vocable* vocableList;
	unsigned int size;
} TransReturn;

void clearList(Vocable* start) {
	Vocable *node = start;
	Vocable *nextNode = NULL;

	while(node->next) {
		nextNode = node->next;
		free(node);
		node = nextNode;
	}

	free(node);
}

unsigned int numberOfVocables(FILE* fd) {
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
	unsigned int size = 0;
	Vocable* headVocable = NULL;
	Vocable* freshVocable = NULL; 
	Vocable* prevVocable = NULL;

	if(fd == NULL) {
		return NULL;
	}
	
	size = numberOfVocables(fd);

	if(size < 2) {
		return NULL;
	}

	headVocable = (Vocable*) malloc(sizeof(Vocable));

	if (!headVocable) return NULL;

	prevVocable = headVocable;

	for(unsigned int i = 0; i < size; i++) {
		freshVocable = (Vocable*) malloc(sizeof(Vocable));

		freshVocable->foreignWord = (char*) malloc(250*sizeof(char));
		fgets(freshVocable->foreignWord, 999, fd);
		deleteNewLine(freshVocable->foreignWord);
		freshVocable->translation = (char*) malloc(250*sizeof(char));
	   	fgets(freshVocable->translation, 999, fd);	
		deleteNewLine(freshVocable->translation);

		freshVocable->next = headVocable;
		freshVocable->prev = prevVocable;
		prevVocable->next = freshVocable;
		prevVocable = freshVocable;
	}

	freshVocable->next = headVocable->next;
	headVocable->next->prev = freshVocable;

	TransReturn *tReturn = (TransReturn*) malloc(sizeof(TransReturn));

	tReturn->size = size;
	tReturn->vocableList = freshVocable;

	return tReturn;
}

void printList(Vocable* list) {
	Vocable* head = list;
	Vocable* this = head;
	
	while(this->next != head) {
		printf("%s\n",this->foreignWord);
		printf("%s\n",this->translation);
		this = this->next;
	}

	printf("%s\n",this->foreignWord);
	printf("%s\n",this->translation);
}

void printCommendation() {
	printf("Beebo Beebo\n");
}

void printInsult(char* rightAnswer) {
	printf("ITS %s YOU DUMB PIECE OF SHIT\n", rightAnswer);
}

Vocable* vocableByIndex(Vocable* list, unsigned int index) {
	Vocable* voc = list;
	
	for(unsigned int i = 0; i < index; i++) {
		voc = voc->next;
	}

	return voc;
}

Vocable* getHead(Vocable* list) {
	Vocable* voc = list;
	while(voc->prev) voc = voc->prev;
	return voc;
}

Vocable* addToList(Vocable* list, Vocable* voc) {
	if(!list) return voc;

	list->next = voc;
	voc->prev = list;

	return voc;
}

int main(int argc, char **argv) {
	TransReturn *tReturn;
	Vocable* voclist = NULL;
	Vocable* correct = NULL;
	Vocable* current = NULL;
	unsigned int amountOfVocs = 0;
	unsigned int randomIndex = 0;
	unsigned int vocsLeft = 0;
	unsigned int falseAnswers = 0;
	char* answer;
	char* question;
	time_t t;

	if(argc < 2) {
		tReturn = readFile("./japvoc.txt");
	} else {
		tReturn = readFile(*(++argv));
	}	

	if(tReturn == NULL) {
		exit(1);
	}

	voclist = tReturn->vocableList;
	
	amountOfVocs = tReturn->size;
	vocsLeft = amountOfVocs;
	answer = (char*) malloc(250*sizeof(char));
	srand((unsigned) time(&t));

	while(1) {
		randomIndex = rand() % vocsLeft;
		current = vocableByIndex(voclist,randomIndex);
		question = current->foreignWord;
		printf("%s\n", question);

		fgets(answer, 250, stdin);
		deleteNewLine(answer);

		if(strcmp(answer, current->translation) != 0) {
			falseAnswers++;
			printInsult(current->translation);
		} else {
			voclist = current->next;
			current->prev->next = current->next;
			current->next->prev = current->prev;

			current->prev = NULL;
			current->next = NULL;

			correct = addToList(correct, current);
			vocsLeft--;

			printCommendation();
		}

		if(vocsLeft < 1) {
			printf("Round ended. You tested %i vocables and gave %i wrong answers\n", amountOfVocs, falseAnswers);
			Vocable* newHead = getHead(correct);
			newHead->prev = correct;
			correct->next = newHead;

			puts("Start another Round? [Y/n]");
			fgets(answer, 250, stdin);

			deleteNewLine(answer);

			if(strcmp(answer, "") != 0 && strcmp(answer, "Y") != 0 && strcmp(answer, "y") != 0) {
				return 0;
			} else {
				vocsLeft = amountOfVocs;
				falseAnswers = 0;
				voclist = correct;
				correct = NULL;
			}
		}
	}
}
