#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include "scanner.h"
token FADriver();
token getToken(enum tokenID ID, char* name);
char* append(char *str, char c);
char *keywords[]={"iter","void","var","return","scan","print","program","cond","then","let","end"};
char *delimiters[]={".","(",")",",","{","}",";","[","]"};
char *operators[]={"<",">","=",":","+","-","*","/","%"};
int fa_table[5][23] = {/* a-z|  0-9|    <|   >|     =|    :|    +|    -|    *|    /|    %|    .|    (|    )|    ,|    {|    }|    ;|    [|    ]|  A-Z,  EOF,   WS*/
			 {   1,    2,    3,    3,    3,    3,    3,    3,    3,    3,    3,    4,    4,    4,    4,    4,    4,    4,    4,    4,   -1,   -2,    0},
			 {   1,    1, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000},//ID_TK
			 {1001,    2, 1001, 1001, 1001, 1001, 1001, 1001, 1001, 1001, 1001, 1001, 1001, 1001, 1001, 1001, 1001, 1001, 1001, 1001,    1, 1001, 1001},//INT_TK
			 {1002, 1002, 1002, 1002, 1002, 1002, 1002, 1002, 1002, 1002, 1002, 1002, 1002, 1002, 1002, 1002, 1002, 1002, 1002, 1002, 1002, 1002, 1002},//LESS_THAN_TK
			 {1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003},//GREATER_THAN_TK
		};

int lineNumber = 1;
char ch =' ';
int nextChar;
token tok;

token scan(FILE *fp){
	token tok;
	if(isblank(ch)){
		filter(fp);
	}
	tok = FADriver(fp);
		if(tok.name[0] == '\0'){
		tok =FADriver(fp);
	}
	return tok;	
}

token FADriver(){
	token tok;
	int kwType;
	int state = 0;
	int nextState;
	char *S = "";
	
	while(state < 1000 && state > -1){//While state is less than the least greatest final state val
		nextState = fa_table[state][nextChar];
		if(nextState < 0){//is Error
			if(nextState == -2){
				tok = getToken(EOF_TK,"EOF");
				return tok;
			}
			if(nextState == -1){
				tok = getToken(ERROR_TK,"Identier must start with lowercase: ");
				tok.name = append(tok.name,ch);
				while(isalpha(ch) || isdigit(ch) && ch != EOF){
					filter(fp);
				}
				return tok;
			}
		}
		if(nextState >= 1000){//is final state
			if(nextState == 1000){
				kwType = isKeyword(S);
				if(kwType != -1){
					//is keyword
					tok = getToken(kwType,S);
					return tok;
				}
				else if(kwType == -1){
					//else return idTok
					tok = getToken(ID_TK, S);
					return tok;
				}	
			}
			else if(nextState == 1001){
				//return int tok
				tok = getToken(INT_TK, S);
				return tok;
			}
			else if(nextState == 1002 || nextState <= 1003){
				//retur opertok
				int k;
				if((k = isDelim(S)) != -1){
					tok = getToken(k,S);
				}
				else if((k = isOperator(S)) != -1){
					tok = getToken(k,S);
				}
				
				return tok;
			}
		}
		else{
			//Longer than 8 chars
			if(strlen(S) > 8){
				while(ch != ' ' && ch != '\n' && ch != '\t' && ch != EOF){
					S = append(S,ch);
					ch = fgetc(fp);
				}
				printf("%s: ", S);
				tok = getToken(ERROR_TK, "No number or identier can be longer than eigth characters");
				return tok;
			}
			state = nextState;
			if(!isspace(ch)){
				S = append(S, ch);
			}
			
			filter(fp);
		}
	}
}

int isOperator(char *S){
	int i;
	for(i = 0; i < 9; i++){
		if(strcmp(S, operators[i]) == 0){
			return i + 13;		
		}
	}
	return -1;
}

int isDelim(char *S){
	int i;
	for(i = 0; i < 9; i++){
		if(strcmp(S, delimiters[i]) == 0){
			return i + 22;		
		}
	}
	return -1;
}

int isKeyword(char *S){
	int i;	
	for(i = 0; i < 11; i++){
		if(strcmp(S, keywords[i]) == 0){
			return i;
		}
	}
	return -1;
}


void filter(){
	if(ch == '\n'){
		lineNumber++;
	}
	ch = fgetc(fp);
	if(ch == 38){//if charaster is &
		while(ch != '\n' && ch != EOF){
			ch = fgetc(fp);
		}	
	}
	nextChar = getRowIndex(ch);	
}

int getRowIndex(char c){
	if(isalpha(c)&& (c >= 'a' && c <= 'z')){
		return 0;
	}
	if(isalpha(c)&& (c >= 'A' && c <= 'Z')){
		return 20;
	}
	else if(isdigit(c) && (c >='0' && c <='9')){
		return 1;
	}
	else if(c == '<'){return 2;}
	else if(c == '>'){return 3;}
	else if(c == '='){return 4;}
	else if(c == ':'){return 5;}
	else if(c == '+'){return 6;}
	else if(c == '-'){return 7;}
	else if(c == '*'){return 8;}
	else if(c == '/'){return 9;}
	else if(c == '%'){return 10;}
	else if(c == '.'){return 11;}
	else if(c == '('){return 12;}
	else if(c == ')'){return 13;}
	else if(c == ','){return 14;}
	else if(c == '{'){return 15;}
	else if(c == '}'){return 16;}
	else if(c == ';'){return 17;}
	else if(c == '['){return 18;}
	else if(c == ']'){return 19;}
	else if(c == EOF){return 21;}
	else if(c == ' '){return 22;}
	else if(c == '\n'){return 22;}
	else if(c == '\t'){ return 22;}
}

token getToken(	enum tokenID ID,char *name){
	token tok;
	tok.ID = ID;
	tok.name = name;
	tok.lineNum = lineNumber;
	return tok;
}

char* append(char *str, char c){
	size_t len = strlen(str);
	char *str2 = malloc(len + 1 + 1);
	strcpy(str2,str);
	str2[len] = c;
	str2[len + 1] = '\0';
	return str2;
}
