#include "swift-lite.h"
#include <stdio.h>

Variable::Variable() : hasValue(false),llvmAI(nullptr) {}
VariableList::VariableList() : father(nullptr),enterLine(1){}

VariableList* createSubList(VariableList *father,string name){
    VariableList *son = new VariableList();
    son->father = father;
    son->namespacing = name;
    return son;
}

void insertVariable(vector<Variable*> var,VariableList *list,int line){
    (list->variables).insert(std::make_pair(line,var));
    if(PRINT_SYMBOL_TABLE){
        for (int i = 0; i < var.size(); i++)
        {
            printf("inserted var %s into list %s with line %d\n",var[i]->name.c_str(),list->namespacing.c_str(),line);
        }
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
        return search_variable_symbol(name,list->enterLine,list->father);
    }
    
    return nullopt;

}

Variable* search_variable_symbol_llvm(const string &name,VariableList *list){
    return search_variable_symbol(name,999,list).value();
}