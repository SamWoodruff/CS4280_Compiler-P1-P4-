#include <stdlib.h>
#include <stdio.h>
#include "statSem.h"
#include "scanner.h"

#define STACKSIZE 100
int varCount = 0;
int scope = 0;
int v = 0;
int l = 0;
int loopCount = 0;

struct 
{ token s[STACKSIZE];
  int tos;
} stack={{0},-1};


char* tempVars[STACKSIZE];

char* addTempVar(){
	char *temp;
	asprintf(&temp,"V%d",v);
	tempVars[v] = temp;
	v++;
	return temp;
}
void push(token tk){
	if(varCount >= STACKSIZE){
		printf("Stack overflow\n");
		exit(1);
	}else{
		//printf("Scope: %d, Variable %s\n",scope,tk.name);
		stack.s[varCount] = tk;
		varCount++;
		stack.tos++;

		//Code-Gen
		fprintf(output,"\tPUSH\n");
	}
}

void pop(){
	if(stack.tos < 0){
		printf("Stack underflow\n");
		exit(0);
	}else{
		int i;
		for(i = varCount; i > scope; i--){
			varCount--;
			stack.tos--;
			stack.s[i].name = "";
			fprintf(output,"\tPOP\n");
		}
	}	
}

int find(token tk){
	int i;

	for(i = stack.tos; i >= scope; i--){
		if(strcmp(stack.s[i].name, tk.name) == 0){
			return stack.tos  - i;
		}
	}
	return -1;
}

int verify(token tk){
	int i;
	for(i = stack.tos; i >= 0; i--){
		if(strcmp(stack.s[i].name,tk.name) == 0){
			return stack.tos - i;
		}
	}
	return -1;
}
void statSem(Node* node,int count){
	if(node == NULL){
		return;
	}
	if(strcmp(node->name,"<program>") == 0){
		int numVars = 0;
		if(node->child1 != NULL){
			statSem(node->child1,numVars);
		}
		if(node->child2 != NULL){
			statSem(node->child2,numVars);
		}
		int i;
		for(i = 0; i < v; i++){
			fprintf(output,"\t%s 0\n",tempVars[i]);
		}
		fprintf(output,"\tSTOP\n");
	}else if(strcmp(node->name,"<vars>") == 0){
		int distanceFromTos = find(node->tk1);
		
		if(distanceFromTos == -1 || distanceFromTos > count){
			push(node->tk1);	
			count++;
		}else if(distanceFromTos < count){
			printf("Redeclaration of variable in scope: %s on line %d\n",node->tk1.name,node->tk1.lineNum);
			remove(outputFile);
			exit(1);
		}
		if(node->child1 != NULL){
			statSem(node->child1,count);
		}
		
	}else if(strcmp(node->name,"<block>") == 0){
		int numVars = 0;
		scope = varCount;
		if(node->child1 != NULL){
			statSem(node->child1, numVars);
		}
		if(node->child2 != NULL){
			statSem(node->child2, numVars);
		}
		//Dont pop if no vars
		if(stack.tos > -1){
			pop();
		}
	}else if(strcmp(node->name,"<expr>") == 0){
		char *var = addTempVar();
		if(node->tk1.ID == PLUS_TK ){
			if(node->child2 != NULL){
				statSem(node->child2,count);
			}
			fprintf(output,"\tSTORE %s\n",var);
			if(node->child1 != NULL){
				statSem(node->child1,count);
			}
			fprintf(output,"\tADD %s\n",var);
		}else if(node->tk1.ID == MINUS_TK){
			if(node->child2 != NULL){
				statSem(node->child2,count);
			}
			fprintf(output,"\tSTORE %s\n",var);
			if(node->child1 != NULL){
				statSem(node->child1,count);
			}
			fprintf(output,"\tSUB %s\n",var);
		}else if(node->child1 != NULL){
			statSem(node->child1,count);
		}

	}else if(strcmp(node->name,"<A>") == 0){
		if(node->child2 != NULL){
			char * var = addTempVar();

			statSem(node->child2,count);
			fprintf(output,"\tSTORE %s\n",var);
			statSem(node->child1,count);
			fprintf(output,"\tDIV %s\n",var);
		}else{
			statSem(node->child1,count);
		}
	}else if(strcmp(node->name,"<N>") == 0){
		if(node->child2 != NULL){
			char * var = addTempVar();

			statSem(node->child1,count);
			fprintf(output,"\tSTORE %s\n",var);
			statSem(node->child2,count);
			fprintf(output,"\tMULT %s\n",var);
		}else{
			statSem(node->child1,count);
		}
	}else if(strcmp(node->name,"<M>") == 0){
		if(node->tk1.ID == PERCENT_TK){
			if(node->child1 != NULL){
				statSem(node->child1,count);
				fprintf(output,"MULT -1\n");
			}
		char *var = addTempVar();
		fprintf(output,"STORE %s\n",var);
		}else if(node->child1 != NULL){
			statSem(node->child1,count);
		}

	}else if(strcmp(node->name,"<R>") == 0){
		if(node->tk1.ID == ID_TK){
			int verified = verify(node->tk1);
			if(verified == -1){
				printf("Identifier: %s is not declared on line %d\n",node->tk1.name,node->tk1.lineNum);
				remove(outputFile);
				exit(0);
			}		
			fprintf(output,"\tSTACKR %d\n",verified);
		}else if(node->tk1.ID == INT_TK){
			fprintf(output,"\tLOAD %s\n",node->tk1.name);
		}else if(node->child1 != NULL){
			statSem(node->child1,count);
		}
	}else if(strcmp(node->name,"<in>") == 0){	
		int verified = verify(node->tk1);
		
		if(verified == -1){
			printf("Identifier: %s is not declared on line %d\n",node->tk1.name,node->tk1.lineNum);
			remove(outputFile);
			exit(0);
		}
		char *var = addTempVar();
		fprintf(output,"\tREAD %s\n",var);
		fprintf(output,"\tLOAD %s\n",var);
		fprintf(output,"\tSTACKW %d\n",verified);
	}else if(strcmp(node->name,"<out>") == 0){	
		if(node->child1 != NULL){
			statSem(node->child1,count);
		}
		char *var = addTempVar();
		fprintf(output,"\tSTORE %s\n",var);
		fprintf(output,"\tWRITE %s\n",var);
	}else if(strcmp(node->name,"<if>")==0){
		char *var = addTempVar();
		loopCount++;
		int Br = loopCount;
		fprintf(output,"loop%d:",loopCount);
		//<expr>	
		statSem(node->child1,count);
		fprintf(output,"\tSTORE %s\n",var);
		//<expr>
		statSem(node->child3,count);
		fprintf(output,"\tSUB %s\n", var);
		//<RO>
		statSem(node->child2,count);
		//stat
		statSem(node->child4,count);
		
		fprintf(output,"end%d:\tNOOP\n",Br);
	}else if(strcmp(node->name,"<loop>") == 0){
		loopCount++;
		int Br = loopCount;
		char *var = addTempVar();
		fprintf(output,"loop%d:", loopCount);
		//<expr>
		statSem(node->child1,count);
		fprintf(output,"\tSTORE %s\n",var);
		//<expr>
		statSem(node->child3,count);
		fprintf(output,"\tSUB %s\n",var);
		//<RO>
		statSem(node->child2, count);
		//<Stat>
		statSem(node->child4, count);
		fprintf(output,"\tBR loop%d\n",Br);
		fprintf(output,"end%d:\tNOOP\n",Br);
	}else if(strcmp(node->name,"<RO>") == 0){
		enum tokenID tkID1 = node->tk1.ID;
		enum tokenID tkID2 = node->tk2.ID;	
		if(tkID1 == LESS_THAN_TK){
			if(tkID2 == GREATER_THAN_TK){
				fprintf(output,"\tBRZERO end%d\n",loopCount);
			}else{
				fprintf(output,"\tBRZNEG end%d\n",loopCount);
			}
		}else if(tkID1 == GREATER_THAN_TK){
			fprintf(output,"\tBRZPOS end%d\n",loopCount);
		}else if(tkID1 == ASSIGN_TK){
			if(tkID2 == LESS_THAN_TK){
				fprintf(output,"\tBRNEG end%d\n",loopCount);
			}else if(tkID2 == GREATER_THAN_TK){
				fprintf(output,"\tBRPOS end%d\n",loopCount);
			}else{
				fprintf(output,"\tBRPOS end%d\n",loopCount);
				fprintf(output,"\tBRNEG end%d\n",loopCount);
			}
		}
	}else if(strcmp(node->name,"<assign>") == 0){
		if(node->child1 != NULL){
			statSem(node->child1,count);
		}
		int verified = verify(node->tk1);
		if(verified == -1){
			printf("Identifier: %s is not declared on line %d\n",node->tk1.name,node->tk1.lineNum);
			remove(outputFile);
			exit(0);
		}
		
		fprintf(output,"\tSTACKW %d\n", verified);
	}
	else
	{
		if(node->child1 != NULL){
			statSem(node->child1, count);
		}
		if(node->child2 != NULL){
			statSem(node->child2, count);
		}
		if(node->child3 != NULL){
			statSem(node->child3, count);
		}
		if(node->child4 != NULL){
			statSem(node->child4, count);
		}
	}
}


