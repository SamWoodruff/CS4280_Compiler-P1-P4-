#ifndef PARSER_H
#define PARSER_H
#include "scanner.h"

char outputFile[20];

typedef struct Node {	
	//For storing tokens from scanner
	token tk1;
	token tk2;

	//3-4 children
	struct Node* child1;
	struct Node* child2;
	struct Node* child3;
	struct Node* child4;
	
	char *name;
	int depth;
} Node;

	Node*  parser();
	Node* program();
	Node* block(int depth);
	Node* vars(int depth);
	Node* expr(int depth);
	Node* A(int depth);
	Node* N(int depth);
	Node* M(int depth);
	Node* R(int depth);
	Node*  stats(int depth);
	Node*  mStat(int depth);
	Node* stat(int depth);
	Node* in(int depth);
	Node* out(int depth);
	Node* If(int depth);
	Node* loop(int depth);
	Node* assign(int depth);
	Node* RO(int depth);

	void error(token needed[],int size ,token recieved);

	struct Node* getNode(char *name, int depth);

#endif
