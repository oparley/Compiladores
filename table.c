#include "table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Symbol* createSymbol(Symbol* prev, 
		char* type, char* name,
		char* value, char* returnedValue, int scope){
	
	Symbol* current = (Symbol *) calloc(1, sizeof(Symbol));
	current->type = type;
	current->name = name;
	current->value = value;
	current->returnedValue = returnedValue;
	current->scope = scope;
	current->prev = prev;

	return current;
}

Table* createTable(Symbol* head){
	Table* table = (Table *) calloc(1, sizeof(Table));
	table->head = head;
	table->tail = head;

	return table;
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

Symbol* findName (const Table* table, char* name) {
	if(!table){
		return NULL;
	}

	if(strcmp(name, "") == 0){
		return NULL;
	}

	Symbol* current;
	for(current = table->tail; current; current = current->prev){
		if(strcmp(current->name, name) == 0){
			return current;
		}
	}
	return current;
}

void printTable(const Table *table) {
	Symbol *prev;
	int i;
	
	if (table == NULL)
		return;
	
	for (prev = table->tail, i = 0; prev; prev = prev->prev, i++)
	{
		printf("No %d (%p):\ninfo = %s\n\
			%s\n\
			%s\n\
			%d\n\
			%s\n\
			prev " "= %p\n\n", i, (void *) prev, 
				prev->type,
				prev->name,
				prev->returnedValue,
				prev->scope,
				prev->value, (void *) prev->prev);
	}
}

void insertVariable(Table* table, char* type, char* name, 
		char* value, char* returnedValue, int scope){
	
	if (table == NULL){
		yyerror("Funcao nao declarada");	
	}

	Symbol* variable  = findName(table, scope);
	
	if (variable != NULL && scope == variable->scope) {
		yyerror("Variavel ja declarada");	
	} else {
		variable = createSymbol(table->tail, type, name, value, returnedValue, scope);
		if (variable == NULL){
			yyerror("Simbolo nao inicializado");
		}
		insertSymbol(table, variable);
	}
}
