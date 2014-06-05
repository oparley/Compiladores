%{
	#include "global.h"
	Table* table;
	int debugOption;
	int lineNumber = 1;
	int scope = 0;
	int has_error = 0;
	int second_parse = 0;
%}

%token PLUS
%token MINUS
%token TIMES
%token DIVIDE
%token POWER

%token LESS_THAN
%token GREATER_THAN
%token LESS_OR_EQUAL
%token GREATER_OR_EQUAL 

%token OPEN_PARENTHESIS
%token CLOSE_PARENTHESIS

%token DIFFERENT
%token EQUAL
%token RECEIVES 

%token AND
%token OR
%token NOT

%token END_LINE
%token START
%token END

%token IF
%token THEN
%token ELSE
%token FOR
%token WHILE
%token FUNCTION

%token FROM
%token TO
%token STEP

%token PRINT
%token SCAN

%token INT
%token FLOAT
%token CHAR
%token STRING_TYPE
%token BOOL

%token IDENTIFIER
%token NUMBER
%token STRING_VALUE

%token END_IF
%token END_FOR
%token END_WHILE
%token END_FUNCTION

%token TRUE
%token FALSE

%start StartExpression

%%

StartExpression:
	START {
		Symbol* main = createSymbol(NULL, "void",
				"main", "", "int", scope);
		table = createTable(main, debugOption);

		printCode("#!/usr/bin/env python\n\n", second_parse);
	} Input
	;

Input:
	/* Empty Line */
	| Input Line
	;

Line:
	END_LINE {
		lineNumber++;
	}
	| Expression {
		printTable(table);
		printCode("\n", second_parse);
		indent(scope, second_parse);
	}
	| END { 
		if(table != NULL){
			deleteTable(table, scope);
			printTable(table);
		} else {
			printTable(table);
		}
		if (table->head != NULL){
			printf("Error: Finalize uma das estruturas\n");
			scope--;
			deleteTable(table, scope);
			printTable(table);
			has_error = 1;
		}
	}
	;

Expression:
	PrintExpression
	| IfExpression
	| WhileExpression
	| ForExpression
	| AttribuitionExpression
	| DeclarationExpression
	| error END_LINE{
		lineNumber++;
		yyerrok;
	}
	;

PrintExpression:
	PRINT STRING_VALUE {
		char *buffer = (char*) malloc(sizeof(char));
		sprintf(buffer,"print %s",$2);
		printCode(buffer, second_parse);	
	}
	| PRINT IDENTIFIER {
		Symbol* variable = findName(table, $2);
		if(variable == NULL){
			printf("Error: Variavel %s nao declarada\n", $2);
			has_error = 1;
		}
		char *buffer = (char*) malloc(sizeof(char));
		sprintf(buffer,"print %s",$2);
		printCode(buffer, second_parse);
	}
	;

IfExpression:
	IF BoolComparasion THEN {
		char *buffer = (char*) malloc(sizeof(char));
		sprintf(buffer,"if  %s:", $2);
		printCode(buffer, second_parse);
		scope++; 
		insertVariable(table, "function", "if", NULL, NULL, scope);
	}
	| ELSE {
		printCode("else:", second_parse);			
	}
	| END_IF {
		deleteTable(table, scope);
		scope--;
		printTable(table);
	}
	;

BoolComparasion:
	 BoolExpression BinaryOperator BoolComparasion {
		char *str2 = (char *) malloc (sizeof(char));
		strcpy(str2, $1);
		strcat(str2, " ");
		strcat(str2, $2);
		strcat(str2, " ");
		strcat(str2, $3);
		$$ = str2;
	}
	| BoolExpression {
		$$ = $1;
	}
	| TRUE {
		$$ = $1;	
	}
	| FALSE {
		$$ = $1;	
	}
	| IDENTIFIER {
		Symbol* variable = findName(table, $1);
		if(variable == NULL){
			printf("Error: Variavel %s nao declarada\n", $1);
			has_error = 1;
		}

		if ( strcmp(variable->type, "bool") != 0) {
			printf("Error: A variavel %s precisa ser booleana\n", $1);
			has_error = 1;			
		} else if (variable->value == NULL) {
			printf("Error: A variavel %s precisa ser inicializada!\n", $1);
			has_error = 1;		
		} else {
			$$ = variable->name;
		}
	}
	;
	
BoolExpression:
	NumberOrIdentifier LogicalComparer NumberOrIdentifier {
		char *str = (char *) malloc (sizeof(char));
		strcpy(str, $1);
		strcat(str, " ");
		strcat(str, $2);
		strcat(str, " ");
		strcat(str, $3);
		$$ = str;
	}
	;


LogicalComparer:
	LESS_THAN {
		$$ = "<";
	}
	| GREATER_THAN {
		$$ = ">";
	}
	| LESS_OR_EQUAL {
		$$ = "<=";
	}
	| GREATER_OR_EQUAL {
		$$ = ">=";
	}
	| DIFFERENT {
		$$ = "!=";
	}
	| EQUAL {
		$$ = "==";
	}
	;

BinaryOperator:
	AND {
		$$ = " and ";	
	}
	| OR {
		$$ = " or ";	
	}
	;

WhileExpression:
	WHILE BoolComparasion {
		char *buffer = (char*) malloc(sizeof(char));
		sprintf(buffer,"while  %s:", $2);
		printCode(buffer, second_parse);
		scope++;
		insertVariable(table, "function", "while", NULL, NULL, scope);
	}
	| END_WHILE {
		deleteTable(table, scope);
		scope--;
		printTable(table);
	}
	;
	
ForExpression:
	FOR IDENTIFIER FROM NumberOrIdentifier TO NumberOrIdentifier STEP NUMBER {
		char *buffer = (char*) malloc(sizeof(char));
		sprintf(buffer,"for %s in range(%s , %s, %s):", $2, $4, $6, $8);
		printCode(buffer, second_parse);
		scope++;
		insertVariable(table, "function", "for", NULL, NULL, scope);
	}
	| FOR IDENTIFIER FROM NumberOrIdentifier TO NumberOrIdentifier {
		char *buffer = (char*) malloc(sizeof(char));
		sprintf(buffer,"for %s in range(%s , %s):", $2, $4, $6);
		printCode(buffer, second_parse);
		scope++;
		insertVariable(table, "function", "for", NULL, NULL, scope);
	}
	| END_FOR {
		deleteTable(table, scope);
		scope--;
		printTable(table);
	}
	;

NumberOrIdentifier:
	IDENTIFIER {
		$$ = $1;
	}
	| NUMBER {
		$$ = $1;
	}
	;

AttribuitionExpression:
	IDENTIFIER RECEIVES AttribuitionValue {
		setVariable(table, $1, $3);
		char *buffer = (char*) malloc(sizeof(char));
		sprintf(buffer,"%s = %s", $1, $3);
		printCode(buffer, second_parse);
	}
	;

AttribuitionValue:
	NUMBER {
		$$ = $1;
	}
	| STRING_VALUE {
		$$ = $1;
	}
	| TRUE {
		$$ = "true";
	}
	| FALSE {
		$$ = "false";
	}
	| MathExpression {
		$$ = $1;
	}
	;

DeclarationExpression:
	Type IDENTIFIER {
		insertVariable(table, $1, $2, NULL, NULL, scope);
	}
	| Type IDENTIFIER RECEIVES AttribuitionValue {
		insertVariable(table, $1, $2, $4, NULL, scope);	
		char *buffer = (char*) malloc(sizeof(char));
		sprintf(buffer,"%s = %s", $2, $4);
		printCode(buffer, second_parse);
	}
	;

Type:
	INT {
		$$ = "int";
	}
	| FLOAT {
		$$ = "float";
	}
	| CHAR {
		$$ = "char";
	}
	| STRING_TYPE {
		$$ = "string";
	}
	| BOOL {
		$$ = "bool";
	}
	;

MathExpression:
	MathParam
	| MathExpression Operator MathExpression {
		char *str = (char *) malloc (sizeof(char));
		strcpy(str, $1);
		strcat(str, " ");
		strcat(str, $2);
		strcat(str, " ");
		strcat(str, $3);
		$$ = str;
	}
	| OPEN_PARENTHESIS MathExpression CLOSE_PARENTHESIS {
		char *str = (char *) malloc (sizeof(char));
		strcpy(str, "(");
		strcat(str, $2);
		strcat(str, ")");
		$$ = str;
	}
	;

MathParam:
	IDENTIFIER {
		Symbol* val = findName(table,$1);
		if(val!=NULL && val->value!=NULL){
			if(strcmp(val->type,"int")==0){
				$$ = $1;
			} else {
				printf("Error: Variavel inteira");
			}
		} else {
			printf("Error: Variavel não declarada ou valor não declarado");
		}
	}
	| NUMBER {
		$$ = $1;
	}
	;

Operator:
	PLUS {
		$$ = "+";
	}
	| MINUS {
		$$ = "-";
	}
	| DIVIDE {
		$$ = "/";
	}
	| TIMES {
		$$ = "*";
	}
	;
%%

int yyerror(char *s) {
	has_error = 1;
	printf("Error: %s Line %d\n", s, lineNumber);
}

int main(int argc, char* argv[]) {
	if(argv[1] != NULL){
		if(strcmp(argv[1], "-debug") == 0){ 
			debugOption = 1;
		} else {
			debugOption = 0;
		}
	}
	table = calloc(1, sizeof(Table));
	yyparse();	

	if (table->head != NULL){
		printTable(table);
		printf("Error: Linha: %d, comando \"fim\" esperado. \n", lineNumber);
		has_error = 1;
		deleteTable(table, 0);
		printTable(table);
	}
	else{
		printTable(table);
	}
	
	if (has_error == 0) {
		second_parse = 1;
		fseek(stdin, 0, SEEK_SET);
		yyparse();
	} 

	exit(EXIT_SUCCESS); 
}

