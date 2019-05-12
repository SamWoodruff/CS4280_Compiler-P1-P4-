/*
 * 	Samuel Woodruff
 * 	CS4280 P1
 * 	2/6/19
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "statSem.h"
int main(int argc, char *argv[]){
	char *buff;
	//Store name of input file	
	char *inputFile = NULL;
	if(argc > 2)//too mnay arguments
	{
		printf("Command line arguments are not correct. Too many!\n");
		exit(0);
	}
	if(argc == 2){//file name given
		//Get file name add .sp19 extension and open
		inputFile = argv[1];
		strcpy(outputFile,inputFile);
		strcat(inputFile,".sp19");
		strcat(outputFile,".asm");
		fp = fopen(inputFile, "r");
		if(fp == NULL){
			fprintf(stderr, "%s: Error in %s: ", argv[0], inputFile);
			perror("");
			exit(0);
		}
	}
	if(argc == 1){//user input
		char buffer[125];
		strcpy(outputFile,"out.asm");
		fp = fopen("temp", "w");
		if(fp == NULL){
			fprintf(stderr, "%s: Error in %s: ", argv[0], inputFile);
			perror("");
			exit(0);
		}
		fgets(buffer,25,stdin);
		while(!feof(stdin)){	
			fputs(buffer, fp);
			fgets(buffer,125,stdin);	
		}
		fclose(fp);	
		fp = fopen("temp","r");
		if(fp == NULL){
			fprintf(stderr, "%s: Error reading %s: ", argv[0], inputFile);
			perror("");
			exit(1);
		}
	}
	output = fopen(outputFile,"w");	
	//Call parser and retrieve constructed tree
	Node *root = parser();
	//Do semantics check here
	statSem(root,0);
	fclose(fp);
	fclose(output);
	remove("temp");
	printf("%s\n",outputFile);
	return 0;
}
