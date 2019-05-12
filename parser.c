#include "parser.h"
#include <stdlib.h>
token tk;
int depth;//Will increment this and save it in node structures

Node* parser(){
	Node* root;
	tk = scan();
	
	if(tk.ID == EOF_TK){
		printf("You've passed an empty file\n");
		exit(1);
	}

	root = program();
	
	
	if(tk.ID == EOF_TK){
		return root;
	}else{
		token needed[1];
		needed[0].name = "EOF_TK";
		error(needed,1,tk);
	}
}

//<program> -> <vars><block>
Node* program(){
	int depth = 0;
	Node *node = getNode("<program>",depth);
	node->child1 = vars(depth);
	node->child2 = block(depth);
	return node;
}

//<vars> -> empty | var Identifier : Integer <vars>
Node* vars(int depth){
	depth++;	
	Node *node = getNode("<vars>",depth);

	//Start by checking for var token	
	if(tk.ID == VAR_TK){
		tk = scan();
		if(tk.ID == ID_TK){
			node->tk1 = tk;//Save id	
			tk = scan();
			if(tk.ID == COLON_TK){
				tk = scan();
				if(tk.ID == INT_TK){
					node->tk2 = tk;//store int		
					tk = scan();
					node->child1 = vars(depth);
					return node;
				} else{
					token needed[1];
					needed[0].name = "Integer";
					error(needed,1,tk);
				}	
			}else{
				token needed[1];
				needed[0].name = ":";
				error(needed,1,tk);
			}
		}else{
			token needed[1];
			needed[0].name = "Identifier";
			error(needed,1,tk);
		}
	}else{
		return NULL;
	}
}

//Block -> void <vars><stats> return
Node* block(int depth){
	depth++;	
	Node *node = getNode("<block>",depth);
	
	if(tk.ID == VOID_TK){
		tk = scan();
		node->child1 = vars(depth);
		node->child2 = stats(depth);
		if(tk.ID == RETURN_TK){
			tk = scan();
			return node;
		}else{
			token needed[1];
			needed[0].name = "return";
			error(needed,1,tk);
		}
	}else{
		token needed[1];
		needed[0].name = "void";
		error(needed,1,tk);
	}
}

//<expr>-><A>+<expr>|<A>-<expr>|<A>
Node*  expr(int depth){
	depth++;
	Node *node = getNode("<expr>",depth);
	node->child1 = A(depth);
	
	if(tk.ID == PLUS_TK){
		node->tk1 = tk;
		tk = scan();
		node->child2 = expr(depth);
		return node;
	}else if(tk.ID == MINUS_TK){
		node->tk1 = tk;
		tk = scan();
		node->child2 = expr(depth);
		return node;
	}
	return node;
}

//<A>-><N>/<A>|<N>
Node *A(int depth){
	depth++;
	Node *node = getNode("<A>",depth);
	node->child1 = N(depth);
	if(tk.ID == SLASH_TK){
		node->tk1 = tk;
		tk = scan();
		node->child2 = A(depth);
		return node;
	}else
		return node;
}

//<N>-><M>*<N>|<M>
Node *N(int depth){
	depth++;
	Node *node = getNode("<N>",depth);
	node->child1 = M(depth);
	if(tk.ID == ASTERIK_TK){
		node->tk1 = tk;
		tk = scan();
		node->child2 = N(depth);
		return node;
	}else
		return node;
}

//<M>->%<M>|<R>
Node *M(int depth){
	depth++;
	Node *node = getNode("<M>",depth);
	if(tk.ID == PERCENT_TK){
		node->tk1 = tk;
		tk = scan();
		node->child1 = M(depth);
		return node;
	}else{
		node->child1 = R(depth);
		return node;
	}
}

//<R>->(<expr>) | Identifier | Integer
Node *R(int depth){
	depth++;
	Node *node = getNode("<R>",depth);
	if(tk.ID == LEFT_PARENTHESIS_TK){
		tk = scan();
		node->child1 = expr(depth);
		if(tk.ID == RIGHT_PARENTHESIS_TK){
			tk = scan();
			return node;
		}else{
			token needed[1];
			needed[0].name = "(";
			error(needed,1,tk);
		}
	}else if(tk.ID == ID_TK){
		node->tk1 = tk;
		tk = scan();
		return node;
	}else if(tk.ID == INT_TK){
		node->tk1 = tk;
		tk = scan();
		return node;
	}else{
		token needed[3];
		needed[0].name = "(";
		needed[1].name = "Identifier";
		needed[2].name = "Integer";
		error(needed,3,tk);
	}
}

//<stats>-><stat>;<mStat>
Node *stats(int depth){
	depth++;
	Node *node = getNode("<stats>",depth);
	
	node->child1=stat(depth);
	if(tk.ID == SEMI_COLON_TK){
		tk = scan();
		node->child2 = mStat(depth);
		return node;
	}else{
		token needed[1];
		needed[0].name = ";";
		error(needed,1,tk);
	}
}

//<mStat> -> empty | <stat> ; <mStat>
Node *mStat(int depth){
	depth++;
	Node *node = getNode("<mStat>",depth);
	if(tk.ID == SCAN_TK || tk.ID == PRINT_TK || tk.ID == VOID_TK || tk.ID == COND_TK || tk.ID == ITER_TK || tk.ID == ID_TK){
		node->child1=stat(depth);
		if(tk.ID == SEMI_COLON_TK){
			tk = scan();
			node->child2 = mStat(depth);
			return node;
		}else{	
			token needed[1];
			needed[0].name = ";";
			error(needed,1,tk);
		}
	}else
		return NULL;
} 

//<stat>-><in>|<out>|<block>|<if>|<loop>|<assign>
Node *stat(int depth){
	depth++;
	Node *node = getNode("<stat>",depth);
	if(tk.ID == SCAN_TK){
		node->child1 = in(depth);
		return node;
	}else if(tk.ID == PRINT_TK){
		node->child1 = out(depth);
		return node;
	}else if(tk.ID == VOID_TK){
		node->child1 = block(depth);
		return node;
	}else if(tk.ID == COND_TK){
		node->child1 = If(depth);
		return node;
	}else if(tk.ID == ITER_TK){
		node->child1 = loop(depth);
		return node;
	}else if(tk.ID == ID_TK){
		node->child1 = assign(depth);
		return node;
	}else{
		token needed[6];
		needed[0].name = "scan";
		needed[1].name = "print";
		needed[2].name = "void";
		needed[3].name = "cond";
		needed[4].name = "iter";
		needed[5].name = "Identifier";
		error(needed,6,tk);
	}
}

//<in> -> scan Identifier
Node* in(int depth){
	depth++;
	Node* node = getNode("<in>", depth);
	if(tk.ID == SCAN_TK){
		tk = scan();
		if(tk.ID == ID_TK){
			node->tk1 = tk;
			tk = scan();
			return node;
		}else{
			token needed[1];
			needed[0].name = "Identifier";
			error(needed,1,tk);
		}
	}else{
		token needed[1];
		needed[0].name = "scan";
		error(needed,1,tk);
	}
}

//<out>->print<expr>
Node* out(int depth){
	depth++;
	Node* node = getNode("<out>",depth);
	if(tk.ID == PRINT_TK){
		tk = scan();
		node->child1 = expr(depth);
		return node;
	}else{
		token needed[1];
		needed[0].name = "print";
		error(needed,1,tk);
	}
}

//<if>->cond[<expr><RO><expr>]<stat>
Node* If(int depth){
	depth++;
	Node* node = getNode("<if>",depth);
	if(tk.ID == COND_TK){
		tk = scan();
		if(tk.ID == LEFT_BRACKET_TK){
			tk = scan();
			node->child1 = expr(depth);
			node->child2 = RO(depth);
			node->child3 = expr(depth);
			if(tk.ID == RIGHT_BRACKET_TK){
				tk = scan();
				node->child4 = stat(depth);
				return node;
			}else{
				token needed[1];
				needed[0].name = "]";
				error(needed,1,tk);
			}
		}else{
			token needed[1];
			needed[0].name = "[";
			error(needed,1,tk);
		}
	}else{
		token needed[1];
		needed[0].name = "cond";
		error(needed,1,tk);
	}
}

//<loop>->iter[<expr><RO><expr]<stat>
Node* loop(int depth){
	depth++;
	Node* node = getNode("<loop>",depth);
	if(tk.ID == ITER_TK){
		tk = scan();
		if(tk.ID == LEFT_BRACKET_TK){
			tk = scan();
			node->child1 = expr(depth);
			node->child2 = RO(depth);
			node->child3 = expr(depth);
			if(tk.ID == RIGHT_BRACKET_TK){
				tk = scan();
				node->child4 = stat(depth);
				return node;
			}else{
				token needed[1];
				needed[0].name = "[";
				error(needed,1,tk);
			}	
		}else{
			token needed[1];
			needed[0].name = "]";
			error(needed,1,tk);
		}
	}else{
		token needed[1];
		needed[0].name = "iter";
		error(needed,1,tk);
	}
}

//<assign>->Identifier=<expr>
Node* assign(int depth){
	depth++;
	Node* node = getNode("<assign>",depth);
	if(tk.ID == ID_TK){
		node->tk1 = tk;
		tk = scan();
		if(tk.ID == ASSIGN_TK){
			tk = scan();
			node->child1 = expr(depth);
			return node;
		}else{
			token needed[1];
			needed[0].name = "=";
			error(needed,1,tk);
		}
	}else{
		token needed[1];
		needed[0].name = "Identifier";
		error(needed,1,tk);
	}
}

//<RO>-> < | =< | > | => | <>| =
Node* RO(int depth){
	depth++;
	Node *node = getNode("<RO>",depth);
	if(tk.ID == ASSIGN_TK){
		node->tk1 = tk;
		tk = scan();
		if(tk.ID == LESS_THAN_TK){
			node->tk2 = tk;
			tk = scan();
			return node;	
		}else if(tk.ID == GREATER_THAN_TK){
			node->tk2 = tk;
			tk = scan();
			return node;
		}else{
			return node;
		}
	}else if(tk.ID == LESS_THAN_TK){
		node->tk1 = tk;
		tk = scan();
		if(tk.ID == GREATER_THAN_TK){
			node->tk2 = tk;
			tk = scan();
			return node;
		}else{
			return node;
		}
	}else if(tk.ID == GREATER_THAN_TK){
		node->tk1 = tk;
		tk = scan();
		return node;
	}else{
		token needed[3];
		needed[0].name = "<";
		needed[1].name = "=";
		needed[2].name = ">";
		error(needed,3,tk);
	}
}

Node* getNode(char* name, int depth){
	Node *newNode = (Node *)malloc(sizeof(Node));
	
	newNode->depth = depth;
	newNode->name = name;
	newNode->child1 = NULL;
	newNode->child2 = NULL;
	newNode->child3 = NULL;
	newNode->child4 = NULL;
	return newNode;	
}
void error(token needed[],int size ,token recieved){
	int i;
	remove(outputFile);
	printf("Error found in parsing--\n");
	printf("Expected: ");
	for(i = 0; i < size; i++){
		printf("%s ",needed[i].name);
	}
	printf("\n");
	printf("Recieved: %s, Line: %d\n", recieved.name, recieved.lineNum);
	exit(0);
}
