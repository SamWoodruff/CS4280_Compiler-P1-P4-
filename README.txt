Storage = Local
(Variables outside of a block are scoped in the entire program,
variable in a block are scoped in that block)

There is a directory of 13 test files included in the test_files directory
Also includes source code of Janikow's ACC assembly interpreter in asm.c

Samuel Woodruff
5/12/19
P4
To compile:
	Type make
To execute:
	./comp <optionalFile>

Output file will match the name of the file, or
will be out.asm if taking input from keyboard.


P1 Scanner details:
	Suggestions for the FA option
	File can be opened and lookahead character can be set explicitly before the first call to the scanner for the first token
	Have the scanner not read directly from the file but from a filter. The filter would count lines, skip over spaces and comments, construct string of characters for the current token, and return the column number in the table corresponding to the character
	Represent the 2-d array for the FSA as array of integers
	0, 1, etc would be states/rows
	-1, -2, etc could be different errors
	1001, 1002, etc could be final states recognizing different tokens
	Recognize keywords as identifiers in the automaton, then do table lookup
	To print tokens I would suggest an array of strings describing the tokens, listed in the same order as the tokenID enumeration. For example:

  	enum tokenID {IDENT_tk, NUM_tk, KW_tk, etc};
 	string tokenNames[] ={"Identifier", "Number", "Keyword", etc};
 	struct token { tokenID, string, int};
	Then printing tokenNames[tokenID] will print the token description.

P2 Parser:
	Build tree of nodes and stores tokens from scanner in them
	Bnf:
	<program>  ->     <vars> <block>
	<block>    ->     void <vars> <stats> return
	<vars>     ->     empty | var Identifier : Integer <vars> 
	<expr>     ->     <A> + <expr> | <A> - <expr> | <A>
	<A>        ->     <N> / <A> | <N>
	<N>        ->     <M> * <N> | <M>
	<M>        ->     % <M> |  <R>
	<R>        ->     ( <expr> ) | Identifier | Integer
	<stats>    ->     <stat> ; <mStat>
	<mStat>    ->     empty |  <stat>  ;  <mStat>
	<stat>     ->     <in> | <out> | <block> | <if> | <loop> | <assign>
	<in>       ->     scan  Identifier 
	<out>      ->     print <expr>
	<if>       ->     cond [ <expr> <RO> <expr> ] <stat>
	<loop>     ->     iter [ <expr> <RO> <expr> ] <stat>
	<assign>   ->     Identifier  = <expr>  
	<RO>       ->     < | = <  | >  | = > | < >   |   = 

p3 Static Semantics:
Local support:
 
	Implement a stack adapter according to the following

	Stack item type is String or whatever was your identifier token instance - the stack will process identifiers. 
	You may also store line number or the entire token for more detailed error messaging
	You can assume no more than 100 items in a program and generate stack overflow if more

Interface
----------
void push(String);
	just push the argument on the stack
void pop(void);
	just remove
int find(String);
	the exact interface may change, see below
	find the first occurrence of the argument on the stack, starting from the top and going down to the bottom of the stack
	return the distance from the TOS (top of stack) where the item was found (0 if at TOS) or -1 if not found
	
	Static semantics
	Perform left to right traversal, and perform different actions depending on subtree and node visited
	when working in <vars> outside of the first block, process the identifiers there as in the  global option (or process as local if desired)
	when working in a <block>
	set varCount=0 for this block
	in <vars> of this <block> take the identifier v
	when varCount>0 call find(v) and error/exit if it returns non-negative number < varCount (means that multiple definition in this block)
	push(v)  and varCount++
	otherwise when identifier token found (variable use)
	find(v), if -1 try STV.verify(v) (if STV used for the global variables) and error if still not found
	call pop() varCount times when leaving a block (note that varCount must be specific to each block)


P4 Code Gen:
	Local Storage allocation used for P4

	The program is to parse the input, generate a parse tree, perform static semantics, and then generate a target file. Any error should display detailed message, including line number if available (depending on scanner).

	The target name should be

	out.asm if keyboard input
	file.asm if file input. The base name matches that of the input file
	The program has 2 parts to be properly generated:

	Code generation
	120
	Storage allocation global option 30 (+20 for local)

	Local - variables outside of a block are scoped in the entire program, variables in a block are scoped in that block
	Note that this does not have to match your static semantics but local storage will use the same mechanisms as local semantics.

	An included README.txt file must say Storage=Global or Local on the first line. Otherwise global will be assumed.

	Temporary variables can processed as global scope even when doing local scoping for user variables.