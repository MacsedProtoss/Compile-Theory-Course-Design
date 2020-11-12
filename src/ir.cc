#include "swift-lite.h"
#include "iostream"
#include "memory"
#include "list"
#include "unordered_set"

using std::pair, std::tuple, std::list, std::make_unique, std::unordered_set;

LLVMContext TheContext;
Module TheModule("code", TheContext);
vector<BasicBlock *> block_stack;
vector<IRBuilder<>> builder_stack;
vector<Function *> function_stack;

unordered_map<string, pair<Function *, FunctionType *>> function_table;
unordered_map<string, pair<Value *, int>> val_table;
unordered_map<string, BasicBlock *> label_table;
unordered_set<BasicBlock *> finished_block;

typedef tuple<Instruction *, Value *, string, string> DeferredBrStatementType;

unordered_map<string, vector<Instruction *>> deferred_goto_statement;
list<DeferredBrStatementType> deferred_br_statement;
void buildVarDefine(Operation *opt);

auto create_goto = [&](const string &label) -> void {
    auto *current_basic_block = block_stack.back();
    if (finished_block.count(current_basic_block))
        return;
    finished_block.insert(current_basic_block);

    if (label_table.count(label))
    {
        builder_stack.back().CreateBr(label_table[label]);
    }
    else
    {
        Instruction *fake_node = builder_stack.back().CreateRetVoid();
        deferred_goto_statement[label].emplace_back(fake_node);
    }
};

ConstantInt* const_int_val = ConstantInt::get(TheContext, APInt(32,0));
ConstantInt* const_char_val = ConstantInt::get(TheContext, APInt(32,0));
ConstantFP* const_float_val = ConstantFP::get(TheContext, APFloat(0.0));

// extern vector<Symbol> symbol_table;

// pair<Value *, int> prepare_opn(LLVMContext &TheContext, unordered_map<string, pair<Value *, int>> &val_table, const Operation &operation)
// {
//     switch (operation.kind)
//     {
//     case INT:
//         return {ConstantInt::get(Type::getInt32Ty(TheContext), get<int>(operation.data)), INT};
//     case FLOAT:
//         return {ConstantFP::get(Type::getFloatTy(TheContext), get<float>(operation.data)), FLOAT};
//     case ID:
//         if (val_table.count(get<string>(operation.data)))
//             return val_table[get<string>(operation.data)];
//     }

//     return {nullptr, 0};
// }
void print_llvm_ir(Operation *head){
    builder_stack.emplace_back(IRBuilder<>(TheContext));

    Operation *currentOpt = head;
    while (currentOpt)
    {
        switch (currentOpt -> kind)
        {
        case ASSIGN:
            //must be var define assign

            break;
        case VAR_DEFINE:
            break;
        case FUNCTION:
            break;
        default:
            break;
        }
        currentOpt = currentOpt -> next;
    }
    
    if constexpr (PRINT_LLVM_IR == 1)
    {
        TheModule.print(errs(), nullptr);
    }
    verifyModule(TheModule, &(errs()));

    std::unique_ptr<Module> ptr(&TheModule);
    ExecutionEngine *engine = EngineBuilder(std::move(ptr)).create();
    engine->finalizeObject();
    engine->runFunction(function_table["main"].first, {});

}

void buildGVarDefine(Operation *opt){
    DefineOpt *defineOpt = (DefineOpt*)opt;
    for (int i = 0; i < defineOpt ->names.size(); i++)
    {
        string name = defineOpt -> names[i];
        switch (defineOpt ->type)
        {
        case Int:
            GlobalVariable *newGVar = new GlobalVariable(TheModule,Type::getInt32Ty(TheContext),false,GlobalVariable::ExternalLinkage,const_int_val,name);
            break;
        case Char:
            GlobalVariable *newGVar = new GlobalVariable(TheModule,Type::getInt32Ty(TheContext),false,GlobalVariable::ExternalLinkage,const_char_val,name);
            break;
        case Float:
            GlobalVariable *newGVar = new GlobalVariable(TheModule,Type::getFloatTy(TheContext),false,GlobalVariable::ExternalLinkage,const_float_val,name);
            break;
        default:
            break;
        }
    }
    
}

void buildVarDefine(Operation *opt){
    DefineOpt *defineOpt = (DefineOpt*)opt;
    for (int i = 0; i < defineOpt ->names.size(); i++)
    {
        string name = defineOpt -> names[i];
        switch (defineOpt ->type)
        {
        case Int:
            GlobalVariable *newGVar = new GlobalVariable(TheModule,Type::getInt32Ty(TheContext),false,GlobalVariable::ExternalLinkage,const_int_val,name);
            break;
        case Char:
            GlobalVariable *newGVar = new GlobalVariable(TheModule,Type::getInt32Ty(TheContext),false,GlobalVariable::ExternalLinkage,const_char_val,name);
            break;
        case Float:
            GlobalVariable *newGVar = new GlobalVariable(TheModule,Type::getFloatTy(TheContext),false,GlobalVariable::ExternalLinkage,const_float_val,name);
            break;
        default:
            break;
        }
    }
    
}

