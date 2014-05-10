#include "table.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define UNDECLARED_FUNCTION -1
#define VARIABLE_ALREADY_DECLARED -2
#define UNITIALIZED_SYMBOL -3
#define UNDECLARED_VARIABLE -4

Table* createTable(Symbol* head){
	Table* table = calloc(1, sizeof(Table));
	table->head = head;
	//table->tail = head;

	return table;
}

Symbol* createSymbol(Symbol* prev, 
		char* type, char* name,
		char* value, char* returnedValue, int scope){
	
	Symbol* current = calloc(1, sizeof(Symbol));
	current->type = type;
	current->name = name;
	current->value = value;
	current->returnedValue = returnedValue;
	current->scope = scope;
	current->prev = prev;

	return current;
}

void insertSymbol(Table* table, Symbol* symbol){
	if(table == NULL || symbol == NULL){
		return;
	}

	if(table->tail == NULL){
		table->tail = symbol;
		return;
	}
	
	symbol->prev = table->tail;
	table->tail = symbol;
}

void deleteSymbol(Symbol* symbol){
	free(symbol->name);
	free(symbol->value);
	free(symbol->returnedValue);
	free(symbol);
}

void deleteTable(Table* table){

	if(!table){
		return;
	}

	Symbol* current;
	Symbol* temp = table->tail;
	for(current = table->tail; current; current = temp->prev){
		temp = temp->prev;		
		deleteSymbol(current);
	}
	free(table->head);
	free(table->tail);
	free(temp);
	free(table);
}

Symbol* findName(const Table* table, const char* name) {
	printf("	FindName {");
	if(!table){
		return NULL;
	}

	if(strcmp(name, "") == 0){
		return NULL;
	}

	Symbol* current = NULL;
	for(current = table->tail; current; current = current->prev){
		if(strcmp(current->name, name) == 0){
			break;
		}
	}
	printf("	} FindName OK, (%p) \n", current);
	return current;
}

void printSymbol(const Symbol* current, int position){
	printf("	Posicao %d (%p):\n\
	type = %s\n\
	name = %s\n\
	returnedValue = %s\n\
	scope = %d\n\
	value = %s\n\
	prev = %p\n\n", position, (void *) current, 
				current->type,
				current->name,
				current->returnedValue,
				current->scope,
				current->value, (void *) current->prev);
}

void printTable(const Table *table) {
	Symbol *current;
	int i;
	printf("PrintTable {\n");
	if (table == NULL){
		return;
	}
	
	for (current = table->tail, i = 0; current; current = current->prev, i++) {
		printSymbol(current, i);
		//free(current);
	}
	printf("} PrintTable \n");
}

int insertVariable(Table* table, char* type, char* name, 
		char* value, char* returnedValue, int scope){
	
	printf("InsertVariable {\n");	
	if (table == NULL){
		printf("Funcao nao declarada\n");
		return UNDECLARED_FUNCTION;	
	}

	Symbol* variable = findName(table, name);
	
	if (variable != NULL && scope == variable->scope) {
		printf("Variavel ja declarada\n");	
		return VARIABLE_ALREADY_DECLARED;
	} else {
		variable = createSymbol(table->tail, type, name, value, returnedValue, scope);
		if (variable == NULL){
			printf("Simbolo nao inicializado\n");
			return UNITIALIZED_SYMBOL;
		}
		insertSymbol(table, variable);
		findName(table, name);
		
	}
	printf("} InsertVariable OK\n");
}

int setVariable(const Table* table, const char* name, char* value){
	Symbol* variable = findName(table, name);
	if(variable == NULL){
		printf("Variavel nao declarada\n");
		return UNDECLARED_VARIABLE;
	}

	variable->value = value;
}

/*void checkError(int code, char* identifier){

	char errorLog = (char *) malloc (sizeof(char));
	lineString = lineNumber + '0';
	strcat(errorLog, "Na linha ");
	strcat(errorLog, lineString);
	
	switch(code){		
		case UNDECLARED_FUNCTION:
			strcat(errorLog, ". Funcao nao declarada.\n");
		break;	
		case VARIABLE_ALREADY_DECLARED:
			strcat(errorLog, ". Variavel ");
			strcat(errorLog, identifier);
			strcat(errorLog, " ja declarada anteriormente.\n");
		break;
		case UNITIALIZED_SYMBOL:
			strcat(errorLog, ". Funcao nao declarada.\n");
		break;
		default:
			free(errorLog);
	}
}*/
