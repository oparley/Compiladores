#include "table.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Table* createTable(Symbol* head, int debugOption){
	Table* table = calloc(1, sizeof(Table));
	table->head = head;
	//table->tail = head;
	if(debugOption > 0){
		table->debugOption = debugOption;
	}else {
		table->debugOption = 0;
	}
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
	debugMessages(table, "	FindName {");
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
	debugMessages(table, "	} FindName OK.\n");
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
	debugMessages(table, "PrintTable {\n");
	if (table == NULL){
		return;
	}
	if(table->debugOption > 0){
		for (current = table->tail, i = 0; current; current = current->prev, i++) {
			printSymbol(current, i);
			//free(current);
		}
		debugMessages(table, "} PrintTable \n");
	}
}

int insertVariable(Table* table, char* type, char* name,
		char* value, char* returnedValue, int scope){

	debugMessages(table, "InsertVariable {\n");
	if (table == NULL){
		debugMessages(table, "Funcao nao declarada\n");
		return UNDECLARED_FUNCTION;
	}

	Symbol* variable = findName(table, name);

	if (variable != NULL && scope == variable->scope) {
		debugMessages(table, "Variavel ja declarada\n");
		return VARIABLE_ALREADY_DECLARED;
	} else {
		variable = createSymbol(table->tail, type, name, value, returnedValue, scope);
		if (variable == NULL){
			debugMessages(table, "Simbolo nao inicializado\n");
			return UNITIALIZED_SYMBOL;
		}
		insertSymbol(table, variable);
		findName(table, name);

	}
	debugMessages(table, "} InsertVariable OK\n");
}

int setVariable(const Table* table, const char* name, char* value){
	Symbol* variable = findName(table, name);
	if(variable == NULL){
		debugMessages(table, "Variavel nao declarada\n");
		return UNDECLARED_VARIABLE;
	}

	variable->value = value;
}

void debugMessages(const Table* table, const char* message){
	if (table == NULL){
		return;
	}

	if (table->debugOption > 0){
		printf("%s\n", message);
	} else {
		// Nothing to do.
	}
}

void indent(int scope){
	int i;
	for(i=0;i<scope;i++){
		printf("\t");
	}
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
