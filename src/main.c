/*
 ============================================================================
 Name        : LKelleyHtmlSrcList.c
 Author      : Lucas Kelley
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// defines functions for later declaration
void findAttribs(char file[]);
void tags(char file[]);
void urls(char file[]);
void freq(char file[]);

int total = 0;
int indexes[BUFSIZ];
char allTags[BUFSIZ][BUFSIZ];
char uniqueTags[BUFSIZ][BUFSIZ];
int numUniqueTags = 0;
int tagCounts[BUFSIZ];

int main(void) {
	setvbuf(stdout, NULL, _IONBF, 0);

	// creates variables for later use
	char url[BUFSIZ];
	char command[1] = "\0";
	char final[BUFSIZ] = "curl -s ";
	char fileStr[256000];
	char fileLine[BUFSIZ];

	// prompts the user for the url then opens it for reading
	printf("URL:\n");
	fgets(url, BUFSIZ, stdin);
	strcat(final, url);
	final[strlen(final) - 1] = '\0';
	FILE *htmlFile = popen(final, "r");

	// reads each line 1 by 1 and adds it to a string until the end of the file is reached
	while((fgets(fileLine, BUFSIZ, htmlFile)) != NULL) {
		strcat(fileStr, fileLine);
	}
	pclose(htmlFile);

	// calls the findAttribs function passing through the file string
	findAttribs(fileStr);

	// on a loop prompts the user for a command and if the command is 'q' the loop is exited
	while (command[0] != 'q') {
		printf("Ready\n");
		fgets(command, BUFSIZ, stdin);
		if (command[0] == 'c') {
			printf("%d\n", total);
		} else if (command[0] == 't') {
			tags(fileStr);
			for (int i = 0; i < total; i++) {
				printf("%s\n", allTags[i]);
			}
		} else if (command[0] == 'u') {
			urls(fileStr);
		} else if (command[0] == 'f') {
			freq(fileStr);
			for (int i = 0; i < numUniqueTags; i++) {
				printf("%s\t%d\n", uniqueTags[i], tagCounts[i]);
			}
		} else if (command[0] == 'q') {
			break;
		} else {
			printf("Not a valid command\n");
		}
	}
	printf("Complete\n");
	return EXIT_SUCCESS;
}

void findAttribs(char file[]) {
	// creates variables to store the length of the file, the string it is looking for and the length of the string it is looking for
	int stringLength = strlen(file);
	char sub[BUFSIZ] = "src=\"";
	int subLength = strlen(sub);
	// loop gets executed until it reaches the last character of the file
    for (int i = 0; i < stringLength;) {
        int j = 0;
        int count = 0;
        // while the file character of an index and substring character of a different index are the same it increments both indexes and the count
        while ((file[i] == sub[j])) {
        	if (file[i-1] == '.') {
        		break;
        	}
            count++, i++, j++;
        }
        // if the count is equal to the length of the substring then it found a match for the substring
        if (count == subLength) {
            indexes[total] = i;
            total++;
        }
        else
            i++;
    }
}

void tags(char file[]) {
	// for every index of known src=" locations
	for (int i = 0; i < total; i++) {
		int endIndex = indexes[i];
		int startIndex = indexes[i];
		char tag[BUFSIZ];
		char pointer = file[startIndex];
		// counts back until it finds <
		while (pointer != '<') {
			pointer = file[startIndex];
			if (pointer == '<'){
				startIndex++;
				break;
			}
			else {
				startIndex--;
			}
		}
		// adds every character after that to a string until a \n or blank space are encountered
		for (int n = 0; n < (endIndex-startIndex); n++) {
			if (file[startIndex] == '\n' || file[startIndex] == ' ') {
				break;
			} else {
				tag[n] = file[startIndex];
				startIndex++;
			}
			if (file[startIndex] == '\n') {
				tag[n] = '\0';
			}
		}
		strcpy(allTags[i], tag);
		memset(tag, 0, sizeof(tag));
	}
}

void urls(char file[]) {
	// executes the loop as many times as there are tags
	for (int i = 0; i < total; i++) {
		char url[BUFSIZ];
		int j = indexes[i];
		int n = 0;
		// adds each character after the index of the tag to an array until a " is encountered
		while (file[j] != '\"') {
			url[n] = file[j];
			n++, j++;
		}
		printf("%s\n", url);
		memset(url, 0, sizeof(url));
	}
}

void freq(char file[]) {
	// calls the tags function to find all tags
	tags(file);
	numUniqueTags = 0;
	// executes as many times as there are tags
	for (int j = 0; j < total; j++) {
		// if there are no unique tags increment unique tags by 1 and copy the tag to uniqueTags and increase its cound
		if (numUniqueTags == 0) {
			numUniqueTags++;
			strcpy(uniqueTags[0], allTags[j]);
			tagCounts[0] = 1;
		// else there is at least one unique tag
		} else {
			// executes as many times as there are unique tags
			for (int i = 0; i < numUniqueTags; i++) {
				// if the tag at the given index is the same as the unique tag at another index it increments that tags counter
				if (strcmp(uniqueTags[i], allTags[j]) == 0) {
					tagCounts[i]++;
					break;
				// else if it is the last execution of the loop copies the tag to uniqueTags of the index plus 1
				} else if (i == numUniqueTags - 1){
					strcpy(uniqueTags[i+1], allTags[j]);
					numUniqueTags++;
					tagCounts[i+1] = 1;
					break;
				}
			}
		}
	}
}
