#ifndef ASS4_15CS10053_TRANSLATOR_H
#define ASS4_15CS10053_TRANSLATOR_H

#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;
extern int size_int;
extern int size_double;
extern int size_pointer;
extern int size_char;
extern int size_bool;

class node_type;	// to store a type and its list
class quad;		//to define quad so that we can further use it to make array
class symdata;	//to enter data into symbol table
class expr;		// to be used for truelist,falselist and nextlist
class symtab;	//usual symbol table
class quad_array;	//it stores the list of quads for the expression and also emits the required emits
class funct;
class matrix;
extern symtab *global_ST; 	//Global symbol table pointer
extern symtab *curr_ST	;	//Current symbol table pointer
extern node_type *global_type; 	//to store the global type for inherited attributes
extern quad_array global_quad;	//Store all quads generated by the grammar
extern int global_width; // to store the global width for inherited attributes
extern int temp_count; // count of the temporary varibles to name the new temporary variable
extern int nxt_instr;		//next instr for use in quads and in different function like backpatch

struct declaration_str;			// struct for declaration grammar
struct identifier_str;			//struct for argument grammar
struct expr;			//struct for storing expressions
struct list;
struct arglistStr;		//struct for argument grammar

//it is the basic type that an element can have
union basic_val{
	int int_val;
	double double_val;
	char char_val;
	char mat_val[100];
};

enum types{
	tp_void=0,tp_bool,tp_char,tp_int,tp_double,tp_ptr,tp_mat,tp_func
};

enum opcode{
	//binary assignment operator
	Q_PLUS=1,Q_MINUS,Q_MULT,Q_DIVIDE,Q_MODULO,Q_LEFT_OP,Q_RIGHT_OP,
	Q_XOR,Q_AND,Q_OR,Q_LOG_AND,Q_LOG_OR,Q_LESS,Q_LESS_OR_EQUAL,
	Q_GREATER_OR_EQUAL,Q_GREATER,Q_EQUAL,Q_NOT_EQUAL,
	
	//unary assignment operator
	Q_UNARY_MINUS,Q_UNARY_PLUS,Q_COMPLEMENT,Q_NOT,Q_TRANS,

	//Copy Assignment
	Q_ASSIGN,

	//Unconditional Jump
	Q_GOTO,

	//Conditional Jump
	Q_IF_EQUAL,Q_IF_NOT_EQUAL,Q_IF_EXPRESSION,Q_IF_NOT_EXPRESSION,
	Q_IF_LESS,Q_IF_GREATER,Q_IF_LESS_OR_EQUAL,Q_IF_GREATER_OR_EQUAL,

	//Type Conversions
	Q_CHAR2INT,Q_CHAR2DOUBLE,Q_INT2CHAR,Q_DOUBLE2CHAR,Q_INT2DOUBLE,Q_DOUBLE2INT,

	//Procedure Call
	Q_PARAM,Q_CALL,Q_RETURN,

	//Pointer Assignment Operator
	Q_LDEREF,Q_RDEREF,
	Q_ADDR,

	//Array Indexing
	Q_RINDEX,
	Q_LINDEX
};

class node_type{
	public:
		int size;    // to save the size of the type
		types basetp; // to save the basic type of the elemnt
		node_type *next; // to save next node_type type for arrays
		node_type(types t,int sz=1, node_type *n=NULL); //constuctor
		int getSize(); //returns the size
		int getSize2();
		types getBasetp(); //return Base type
		void printSize(); //to print the size
		void print(); 
};

node_type *CopyType(node_type *t);

//class which will be used as building element for symbol table
class symdata{
	public:
		string name;//to store the value of a varaible stored in symbol table
		int size; //to stoe the size of an element in a symbol table
		int offset;//to maintain the offset at this variable to know the position at symbol table
		basic_val i_val;//to store the initialized value for an element stored at symbol table
		node_type *tp_n;//for storing the type of element
		symtab *nest_tab; //to store the pointer to the symbol table to which the current element belongs to
	 	matrix *mat;//to store the pointer to a matrix if its a matrix type
		funct *fun;//to store the pointer to a function if its an function
	//void createarray();
		string var_type;//to store whether the varaible is "null=0" "local=1" "param=2" "func=3" "ret=4" "temporary=5"
		bool isInitialized; //If the value of element is initialized or not
		bool isFunction; //to know whether the current element is function like function pointer
		bool isMatrix; // to know whether the current element is a matrix it helps if we have been in grammar tree
		symdata(string n=""); //name is initialized to null that will be used for naming temporary variables
		//void createMatrix();
};
	
class symtab{
public:
	string name;			// name of the symbol
	int offset;				// final offset of this symbol table that will be used in the update function
	vector<symdata*> symbol_tab; //maintaining a list of symbol tables
	symtab();  //constructor
	~symtab(); //destructor
	symdata* lookup(string n,bool search=false);// Lookup function searches the variable with name. If the variable is present then returns its pointer location else creates a new entry with its name and returns that pointer
	 //it can also searche for the variable and returns the poiter to it if present
	
	symdata* gentemp(node_type *type); //gentemp creates a new element in the symbol table with the type provided at the time of constructing
	void update(symdata *loc,node_type *type,basic_val initval, symtab *next = NULL);//
	void print();
};

/* To store details of functions, its parameters and return type */
class funct{
	public:
		funct(vector<types> tpls);
		/* Parameter type list */
		vector<types> typelist;

		void print();		/* Prints details in suitable format */
		
		/* Return type */
		node_type *rettype;
};

class matrix{
	public:
	// Stores the array base and the variable containing array offset 
	string base_mat;
	types tp;
	// Initiates array name, offset and array type 
	matrix(string s,int sz,types t);
	// Stores array dimensions 
	vector<int> dims;
	// Size of base type 
	int bsize;
	// Number of dimensions 
	int dimension_size;
	// Adds an array index for array accessing 
	void addindex(int i);
};

void propagate(list *L);

struct expr{
	symdata* loc;
	node_type* type;
	list* truelist;
	list* falselist;
	bool isPointer;
	bool isMatrix;
	symdata *mat;	
};

typedef struct list{
	int index;
	struct list *next;
}list;

list* makelist(int i);  //creates a list have only one element as i
list* merge(list *l1,list *l2); //merges the the two list and return a merged list
void backpatch(list *l,int i);  //to fill the dangling list of goto's l1 to i

// struct for identifier grammar
struct identifier_str{
	symdata *loc;					// pointer to the symboltable
	string *name;					// name of the identifier
};

// struct for declaration grammar
struct declaration_str{
	node_type *type;					// type of the current declaration
	int width;					// width of the variable
};

// struct for argument grammar
struct arglistStr{
	vector<expr*> *arguments;		// A simple vector is used to store the locations of all seen arguments
};

class quad{
	public:
		string arg1,result,arg2;	//consist of three elements 
		opcode op;
		
		quad(opcode op, string arg1="", string arg2="", string result="");  //quad used for operations with 3 inputs
		quad(opcode op, int val, string result="");				// quad for assignment of integer
		quad(opcode op, double val, string result="");			// quad for assignment of double
		quad(opcode op, char val, string result="");			// quad for assignment of char
		void print_arg();
};

class quad_array{
	public:
		vector<quad> arr;	//store quads
		quad_array();		//define constructor
		
		void emit(quad q); //emit a TAC using quad
		void print();	
};
void conv2Bool(expr *e);	//to convert the given exprssion type to bool mostly used in relational operator
void typecheck(expr *e1, expr *e2, bool isAss=false);
#endif
