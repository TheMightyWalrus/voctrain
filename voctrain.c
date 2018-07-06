#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#define MAX_WORD_LENGTH 250

typedef struct vocable {
	char *original;
	char *translation;
	struct vocable* next;
	struct vocable* prev;
} Vocable;

typedef struct vocableList {
	Vocable* head;
	Vocable* tail;
	unsigned int size;
} VocableList;

Vocable* createVocable(char* original, char* translation) {
	printf("creating vocable %s -> %s\n",original,translation);
	Vocable *voc = (Vocable*) malloc(sizeof(Vocable));
	if(!voc) return NULL;

	voc->original = (char*) malloc(strlen(original) * sizeof(char));
	if(!voc->original) return NULL;

	voc->translation = (char*) malloc(strlen(translation) * sizeof(char));
	if(!voc->translation) return NULL;

	strcpy(voc->original,original);
	strcpy(voc->translation,translation);
	voc->next = NULL;
	voc->prev = NULL;

	printf("returning voc\n");
	return voc;
}

void printList(VocableList* list) {
	printf("printing %p\n",list);
	Vocable *voc = list->head;
		
	for(unsigned int i = 0; i < list->size; i++) {
		printf("%s -> %s\n",voc->original,voc->translation);
		voc = voc->next;
	}
}
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

unsigned int numberOfLines(FILE* fd) {
	unsigned int i = 0;
	char dump[250];

	while(fgets(dump, 250, fd) != NULL) {
		i++;
	}

	rewind(fd);

	return i; 
}

void deleteNewLine(char* string) {
	unsigned int size = strlen(string);
	if(strlen > 0 && string[size-1] == '\n') {
		string[size-1] = '\0';
	}
}

void removeFromList(VocableList* list, Vocable *voc) {
	printf("removing %s from list %p\n",voc->original,list);
	if(list->size > 1) {
		printf("size greater 1\n");
		voc->prev->next = voc->next;
		voc->next->prev = voc->prev;
	
		if(voc == list->head) {
			list->head = voc->next;
		}
	} else {
		list->head = NULL;
		list->tail = NULL;
	}
	
	voc->next = NULL;
	voc->prev = NULL;

	--list;
	printf("returning from removing\n");
}

void addVocToList(VocableList* list, Vocable* voc) {
	printf("adding %s to list %p\n",voc->original,list);
	if(!list->head) {
		printf("head was NULL\n");
		list->head = voc;
		list->tail = voc;
	} else {
		printf("head was not NULL\n");
		list->tail->next = voc;
		voc->prev = list->tail;
		voc->next = NULL;
		list->tail = voc;	
	}

	list->size++;
}

VocableList readAsList(char* filename) {
	printf("read file %s\n",filename);
	FILE *fd;
	unsigned int nol;
	unsigned int nov;
	char translation[MAX_WORD_LENGTH];
	char original[MAX_WORD_LENGTH];
	VocableList list;
	Vocable *vocable;
	
	list.size = 0;
	list.head = NULL;
	list.tail = NULL;

	fd = fopen(filename, "r");
	if(!fd) return list;

	nol = numberOfLines(fd);
	printf("found %i lines\n",nol);

	if(nol < 2 || nol % 2 != 0) {
		fclose(fd);
		return list;
	}	

	nov = nol / 2;

	for(unsigned int i = 0; i < nov; i++) {
		fgets(original, MAX_WORD_LENGTH, fd);
		fgets(translation, MAX_WORD_LENGTH, fd);
		deleteNewLine(original);
		deleteNewLine(translation);	
		vocable = createVocable(original, translation);

		if(!vocable) {
			list.size = 0;
			fclose(fd);
			return list;
		}

		addVocToList(&list, vocable);
	}

	printList(&list);	
	fclose(fd);
	printf("returning from reading %s\n",filename);
	return list;
}

void makeCircularList(VocableList* list) {
	list->tail->next = list->head;
	list->head->prev = list->tail;
}

void catenateLists(VocableList* list, VocableList* listToCat) {
	if(!list->head) {
		list->head = listToCat->head;
		list->tail = listToCat->tail;
		list->size = listToCat->size;
	} else {
		list->tail->next = listToCat->head;
		listToCat->head->prev = list->tail;
		list->size += listToCat->size;
	}
} 

VocableList readVocableFiles(unsigned int argc, char **argv) {
	VocableList list;
	VocableList current;

	list.head = NULL;
	list.tail = NULL;
	list.size = 0;

	for(unsigned int i = 0; i < argc; i++) {
		current = readAsList(*argv);
		
		if(current.size > 0) {
			catenateLists(&list, &current);
		} 

		argv++;
	}
	printList(&list);
	return list;
}

void printCircularList(Vocable* list) {
	Vocable* head = list;
	Vocable* this = head;
	
	while(this->next != head) {
		printf("%s\n",this->original);
		printf("%s\n",this->translation);
		this = this->next;
	}

	printf("%s\n",this->original);
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

int main(int argc, char **argv) {
	VocableList vocableList;
	VocableList correctList;
	Vocable* current = NULL;
	unsigned int amountOfVocs = 0;
	unsigned int randomIndex = 0;
	unsigned int falseAnswers = 0;
	char* answer;
	char* question;
	time_t t;

	if(argc < 2) {
			puts("No files to read");
			exit(1);
	} else {
		vocableList = readVocableFiles(argc - 1, ++argv);
	}	

	if(vocableList.size == 0) {
		puts("One or more vocable files could not be read");
		exit(1);
	}

	correctList.head = NULL;
	correctList.tail = NULL;
	correctList.size = 0;

	makeCircularList(&vocableList);
	printCircularList(vocableList.head);
	
	answer = (char*) malloc(250*sizeof(char));
	srand((unsigned) time(&t));

	while(1) {
		printf("vocablehead: %s\n",vocableList.head->original);
		randomIndex = rand() % vocableList.size;
		current = vocableByIndex(vocableList.head,randomIndex);
		question = current->original;
		printf("%s\n", question);

		fgets(answer, 250, stdin);
		deleteNewLine(answer);

		if(strcmp(answer, current->translation) != 0) {
			falseAnswers++;
			printInsult(current->translation);
		} else {
			removeFromList(&vocableList, current);
			addVocToList(&correctList, current);
			printCommendation();
		}

		if(vocableList.size < 1) {
			printf("Round ended. You tested %i vocables and gave %i wrong answers\n", amountOfVocs, falseAnswers);

			puts("Start another Round? [Y/n]");
			fgets(answer, 250, stdin);

			deleteNewLine(answer);

			if(strcmp(answer, "") != 0 && strcmp(answer, "Y") != 0 && strcmp(answer, "y") != 0) {
				return 0;
			} else {
				falseAnswers = 0;
				
				vocableList.head = correctList.head;
				vocableList.tail = correctList.tail;
				vocableList.size = correctList.size;
				correctList.head = NULL;
				correctList.tail = NULL;
				correctList.size = 0;
				makeCircularList(&vocableList);
			}
		}
	}
}
