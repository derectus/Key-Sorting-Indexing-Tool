/*
 ========================================================================================
 Name        : keySorting.c
 Author      : Nuri Çilengir
 Version     : 0.1
 Copyright   : GPLv3
 Description : Key Sort Indexing of given files according to settings file (json format)
 ========================================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <json-c/json.h>

struct JSON_Conf {			// It keeps Configration variables get from json file.
	char *dataFileName;		// datafileName represent Input Data File
	char *indexFileName;	// IndexFileName represent Output Data File
	char *keyEncoding;		// Key Encoding represent Output style
	char *order;			// Ascending or decsending Indexes
	int recordLenght;		// Lenht of record
	int keyStart;			// Key start byte
	int keyEnd;				// Jey End byte
} JSON_Conf;

typedef struct JSON_Conf Config;
Config config;

struct _Student {			// It Keeps Input data Struct
	int rowID;
	int studentID;
	char firstName[20];
	char lastName[20];
	char phone[10];
};
typedef struct _Student Student;
Student *students;

struct _Index {
	int key;				// key offset pairs
	int offset;
};
typedef struct _Index Index;
Index *index;

/*Declarations Main Functions*/
int binarySearch(void * key, void *data, int firstIdx, int lastIdx);		// It performs classical binary Search.
int comparator_ascending(const void *a, const void *b);									// ASC sorting comparator for QuickSort
int comparator_descending(const void *a, const void *b);								// DEC sorting comparator for QuickSort
int writeIndexToIndexFile();																						// It write index objects to indexFile from memory
void printIndexFile();																									// It prints indexes from created IndexFile before.
void printDataFile();																										// It prints Input File.
void searchKey();																												// This is helper function of binarySearch function
void closeFiles();																											// This function free to files from the memory
void config_parser(json_object * jsonObject);														// It reads json config file
void Selection(json_object *jobj);																			// Menu Main function
void printMenu(int* answer);
void openDataFile(char *dataFileName);																	// It Open Data file and get Indexes to memory

/*Main Variables of Program*/
static FILE *data_file = NULL;																				 // Input file (Data File)
static FILE *output_index_file = NULL;																 // OutptFile (Index File)
int recordRange = 0, orderSize = 0, nIdx = -1;												 // recordRange = keyEnd - keyStart, orderSize = count of data file orders(students)

int main(){
	struct json_object* jobj;
	jobj = json_object_from_file("config.json");
	Selection(jobj);
 return 0;
}

int comparator_ascending(const void *a, const void *b) {
	return ((((Index*) a)->key - ((Index*) b)->key));
}						// It compare A key and B key and then return 0 or 1
int comparator_descending(const void *a, const void *b) {
	return ((((Index*) b)->key - ((Index*) a)->key));
}						// reverse of Ascending


/* This function tracks the supplied json file and adds it to the configuration structure. The program must be read when it is opened. */
void config_parser(json_object * jsonObject) {
	json_object_object_foreach(jsonObject, key, value) {
		if (strcmp(key, "dataFileName") == 0) {
			config.dataFileName = (char *) malloc(
					strlen(json_object_get_string(value)) + 1);
			strcpy(config.dataFileName, json_object_get_string(value));
		} else if (strcmp(key, "indexFileName") == 0) {
			config.indexFileName = (char *) malloc(
					strlen(json_object_get_string(value)) + 1);
			strcpy(config.indexFileName, json_object_get_string(value));
		} else if (strcmp(key, "keyEncoding") == 0) {
			config.keyEncoding = (char *) malloc(
					strlen(json_object_get_string(value)) + 1);
			strcpy(config.keyEncoding, json_object_get_string(value));
		} else if (strcmp(key, "order") == 0) {
			config.order = (char *) malloc(
					strlen(json_object_get_string(value)) + 1);
			strcpy(config.order, json_object_get_string(value));
		} else if (strcmp(key, "recordLenght") == 0) {
			config.recordLenght = json_object_get_int(value);
		} else if (strcmp(key, "keyStart") == 0) {
			config.keyStart = json_object_get_int(value);
		} else if (strcmp(key, "keyEnd") == 0) {
			config.keyEnd = json_object_get_int(value);
		} else {
			printf("\n\nInvalid Configration Arguments");
		}
	}
	recordRange = config.keyEnd - config.keyStart;	//Calculate byte Range
}
void openDataFile(char *dataFileName) {
	FILE *fp;
	fp = fopen(dataFileName, "rb");	//first we read the data file and if is NULL function will be stop.
	if (!fp)
		return;
	fseek(fp, 0L, SEEK_END);				//	We are going to the end of the file. Then we find the number of orders in the file.
	orderSize = (ftell(fp) / config.recordLenght);
	fseek(fp, 0, SEEK_SET);					// file pointer to the beginning of the file.
	fread(&recordRange, sizeof(int), 1, fp);  // and we need to calculate the distance from the file we are reading.
	index = malloc(orderSize * sizeof(Index));
	int i;
	for (i = 0; i < orderSize; i++) {
		fread(&index[i].key, sizeof(index[i].key), 1, fp); // we put the bytes into the corresponding places of the index structure.
		index[i].offset = i;
	}																									//Finally, according to the order value in our setup file, we sort the index in memory.
	if (strcmp(config.order, "ASC") == 0) {
		qsort(index, orderSize, sizeof(Index), comparator_ascending);
	} else if (strcmp(config.order, "DEC") == 0) {
		qsort(index, orderSize, sizeof(Index), comparator_descending);
	}
}
int writeIndexToIndexFile() {
	output_index_file = fopen(config.indexFileName, "a"); //we are opening our file with "a" so that each incoming data is added to the end.
	fclose(fopen(config.indexFileName, "w"));
	int i;
	if (strcmp(config.keyEncoding, "BIN")) { 			// If the Encoding value in our setting file is Binary,
		for (i = 0; i < orderSize; i++) {
		fwrite(&index[i], sizeof(Index), 1, output_index_file);
		}
	}else if (strcmp(config.keyEncoding, "UTF")){		//	If the Encoding value in our setting file is UTF,
		for (i = 0; i < orderSize; i++) {
		fprintf(output_index_file,"%s%d",index[i].key,index[i].offset);
		}
	}else{ 																	//	If the Encoding value in our setting file is Character,
		for (i = 0; i < orderSize; i++) {
		//	char *s = malloc(sizeof(char) * sizeof(index[i].key));
		//	char *b = malloc(sizeof(char) * sizeof(index[i].offset));
		//	fputs(sprintf(s, "%d", index[i].key),output_index_file);
		//	fputs(sprintf(b, "%d", index[i].offset),output_index_file);
			fwrite(&index[i], sizeof(Index), 1, output_index_file);
		}
	}
	return 1;
}

/*This is a classic binary search process. we do the query process via index key and input key.*/
int binarySearch(void * key, void *data, int firstIdx, int lastIdx) {
	int middleIdx = (firstIdx + lastIdx) / 2;
	if (index[firstIdx].key == *(int*) key) {
		nIdx = firstIdx + 1;
		return index[firstIdx].offset;

	}
	if (index[lastIdx].key == *(int*) key) {
		nIdx = lastIdx + 1;
		return index[lastIdx].offset;

	}
	if (middleIdx == firstIdx || middleIdx == lastIdx) {
		printf("Record has not been found!");
		return -1;
	}
	if (index[middleIdx].key == *(int*) key) {
		nIdx = middleIdx + 1;
		return index[middleIdx].offset;

	}
	if (index[middleIdx].key > *(int*) key) {
		binarySearch(key, data, firstIdx + 1, middleIdx - 1);
	} else {
		binarySearch(key, data, middleIdx + 1, lastIdx - 1);
	}
}

void printDataFile() {
	FILE* inputFile;
	inputFile = fopen(config.dataFileName, "rb");	//The input file is read.
	fseek(inputFile, 0, SEEK_SET);			//then the pointer is taken to the beginning of the file.
	int i;
	for (i = 0; i < orderSize; i++) {	//The file is scanned up to the order size.
		Student bufferPerson;						//The student is created a student object for each reading.
		fread(&bufferPerson, sizeof(Student), 1, inputFile);
		printf(
				"\nrowID : %d\tStudent ID : %d\tName : %s\tLastname : %s\t Phone : %s",
				bufferPerson.rowID, bufferPerson.studentID,
				bufferPerson.firstName, bufferPerson.lastName,
				bufferPerson.phone);				//and then printed on the screen.
	}
	fclose(inputFile);
}
void printIndexFile() { //this function behaves the same as printDataFile.
	int i;
	Index *object = malloc(sizeof(Index));
	FILE * file = fopen(config.indexFileName, "r");
	if (file != NULL) {
		for ( i = 0; i < orderSize; i++){
		  fread(object, sizeof(Index), 1, file);
		  printf("Key : %d , Offset : %d\n",object->key,object->offset);
		}
	fclose(file);
	}
}

/*

This is a help function. read the corresponding data file. The value is separated by the amount of data to be queried from memory. and then output according to the value returned by the binarySearch operation.
*/
void searchKey() {
	FILE *foundRec = fopen(config.dataFileName, "r");
	int key, place;
	void *data;
	data = malloc(sizeof(config.recordLenght));
	printf("Enter Key : ");
	scanf("%d", &key);
	place = binarySearch(&key, data, 0, orderSize);
	if (place != -1) {
		Student *bufferPerson;
		fseek(foundRec, place, SEEK_SET);
		fread(data, config.recordLenght, 1, foundRec);
		bufferPerson = (Student *) data;
		printf("Record has been found successfully!\n");
		printf("\n\nrowID : %d\tStudent ID : %d\tName : %s\tLastname : %s\t Phone : %s",
				bufferPerson->rowID, bufferPerson->studentID,
				bufferPerson->firstName, bufferPerson->lastName,
				bufferPerson->phone);
	}
}
void printMenu(int* answer) {
	//print the user menu
	printf("You can perform the following tasks: \n");
	printf("-----------------------------------------------\n");
	printf("(1) Open a Json and Data Files\n");
	printf("(2) Create the index file \n");
	printf("(3) Print  Data File\n");
	printf("(4) Print Index File\n");
	printf("(5) Search a record for the given key\n");
	printf("(6)Close the used files and free the memory\n");
	printf("-----------------------------------------------\n");
	printf("7) Quit \n");
	printf("Please Select one... \n");
	scanf("%d", answer);
}
void closeFiles(){
	free(data_file);
	free(output_index_file);
}
void Selection(json_object *jobj) {
	int answer;
	int loop = 1;
	while (1) {
		printMenu(&answer);
		while (answer > 8 || answer < 1) {
			printf("\nEnter a valid choice by pressing ENTER key again");
			printMenu(&answer);
		}
		switch (answer) {
		printf("%d", answer);
			case 1:					//1) Open a Json and Data Files
				config_parser(jobj);
				openDataFile(config.dataFileName);
				break;
			case 2:				//(2) Create the index file
				writeIndexToIndexFile();
				break;
			case 3:			//Print  Data File
				printDataFile();
				break;
			case 4:		//Print  Index File
				printIndexFile();
				break;
			case 5:		//Search a record for the given ke
				searchKey();
				break;
			case 6:		//Close the used files and free the memory
				closeFiles();
				break;
			case 7:		//7) Quit
				exit(0);
				break;
		}
		if (loop == 0)
			break;
	}
}
