#include "swift-lite.h"
#include <stdio.h>

Variable::Variable() : hasValue(false) {}
VariableList::VariableList() : father(nullptr) {}

VariableList* createSubList(VariableList *father,string name){
    VariableList *son = new VariableList();
    son->father = father;
    son->namespacing = name;
    return son;
}

void insertVariable(Variable* var,VariableList *list,int line){
    (list->variables).insert(std::make_pair(line,var));
    printf("inserted var %s into list %s with line %d",var->name.c_str(),list->namespacing.c_str(),line);
}