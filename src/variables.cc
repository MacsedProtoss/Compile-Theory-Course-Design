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

optional<Variable*> search_variable_symbol(const string &name,int line,VariableList *list){
    int index = 0;
    while (index < line)
    {
        std::unordered_map<int, Variable*>::iterator it;
        if ((it = list->variables.find(index)) != list->variables.end()) {
            Variable *var = it->second;
            if (var->name == name){
                return var;
            }
        }
        
        index++;
    }
    
    if (list->father != nullptr){
        return search_variable_symbol(name,list->father->variables.size(),list->father);
    }
    
    return nullopt;

}