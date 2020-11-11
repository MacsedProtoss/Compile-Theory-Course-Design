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

void insertVariable(vector<Variable*> var,VariableList *list,int line){
    (list->variables).insert(std::make_pair(line,var));
    for (int i = 0; i < var.size(); i++)
    {
        printf("inserted var %s into list %s with line %d",var[i]->name.c_str(),list->namespacing.c_str(),line);
    }
}

optional<Variable*> search_variable_symbol(const string &name,int line,VariableList *list){
    int index = -1;
    while (index < line)
    {
        std::unordered_map<int, vector<Variable*>>::iterator it;
        if ((it = list->variables.find(index)) != list->variables.end()) {
            vector<Variable *>vars = it->second;
            for (int i = 0; i < vars.size(); i++)
            {
                Variable *var = vars[i];
                if (var->name == name){
                    return var;
                }
            }
            
            
            
        }
        
        index++;
    }
    
    if (list->father != nullptr){
        return search_variable_symbol(name,list->father->variables.size(),list->father);
    }
    
    return nullopt;

}