#include "scanner.h"
#include "parser.h"
#ifndef STATSEM_H
#define STATSEM_H

	FILE *output;
	
	void push(token tk);
	void pop();
	int find(token tk);//Return position on stack(Distance from TOS)
	int verify(token tk);
	void statSem(Node* node,int count);	
	void codeGen(Node* root, char* fileName);
#endif

