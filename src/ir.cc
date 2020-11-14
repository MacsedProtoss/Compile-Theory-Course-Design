#include "swift-lite.h"
#include "iostream"
#include "memory"
#include "list"
#include "unordered_set"

using std::pair, std::tuple, std::list, std::make_unique, std::unordered_set;

unsigned long int tempIndex = 0;

LLVMContext TheContext;
Module TheModule("app", TheContext);
vector<BasicBlock *> block_stack;
vector<IRBuilder<>> builder_stack;
vector<Function *> function_stack;

extern VariableList *globalVars;

unordered_map<string, pair<Function *, FunctionType *>> function_table;
unordered_map<string, pair<Value *, int>> val_table;
unordered_map<string, BasicBlock *> label_table;
unordered_set<BasicBlock *> finished_block;

typedef tuple<Instruction *, Value *, string, string> DeferredBrStatementType;

unordered_map<string, vector<Instruction *>> deferred_goto_statement;
list<DeferredBrStatementType> deferred_br_statement;
vector<GlobalVariable*> buildGVarDefine(Operation *opt);
void buildVarDefine(Operation *opt,BasicBlock *block);
Value* buildSimpleOpts(Operation *opt,VariableList *list,BasicBlock *block);

ConstantInt* const_int_val = ConstantInt::get(TheContext, APInt(32,0));
ConstantInt* const_char_val = ConstantInt::get(TheContext, APInt(32,0));
ConstantFP* const_float_val = ConstantFP::get(TheContext, APFloat(0.0));

// auto create_goto = [&](const string &label) -> void {
//     auto *current_basic_block = block_stack.back();
//     if (finished_block.count(current_basic_block))
//         return;
//     finished_block.insert(current_basic_block);

//     if (label_table.count(label))
//     {
//         builder_stack.back().CreateBr(label_table[label]);
//     }
//     else
//     {
//         Instruction *fake_node = builder_stack.back().CreateRetVoid();
//         deferred_goto_statement[label].emplace_back(fake_node);
//     }
// };



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

    Type *preAssignreturnType = Type::getVoidTy(TheContext);
    FunctionType *reAssignfunctionType = FunctionType::get(preAssignreturnType,false);
    Function *fpreAssignunctionValue = Function::Create(reAssignfunctionType,Function::ExternalLinkage,"preservedFunction_Global_var_assign_init",TheModule);
    BasicBlock *preAssignBlock = BasicBlock::Create(TheContext,"entry",fpreAssignunctionValue);
    IRBuilder<> preAssignBuilder(preAssignBlock);

    Operation *currentOpt = head;
    while (currentOpt)
    {
        switch (currentOpt -> kind)
        {
        case ASSIGN:
            {
                //must be var define assign

                NormalOpt *opt = (NormalOpt*) currentOpt;
                auto gvars = buildGVarDefine(opt -> left);
                buildSimpleOpts(opt -> right,globalVars,preAssignBlock);
                for (int i = 0; i < gvars.size(); i++)
                {
                    auto var = gvars[i];
                    auto l = preAssignBuilder.CreateLoad(var);
                    // auto l = var;
                    auto v = ConstantInt::get(Type::getInt32Ty(TheContext), 1);
                    preAssignBuilder.CreateStore(l,v);
                }
                
                
            }
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

    preAssignBuilder.CreateRetVoid();
    
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

vector<GlobalVariable*> buildGVarDefine(Operation *opt){
    DefineOpt *defineOpt = (DefineOpt*)opt;
    vector<GlobalVariable*> vars;
    for (int i = 0; i < defineOpt ->names.size(); i++)
    {
        string name = defineOpt -> names[i];
        switch (defineOpt ->type)
        {
        case Int:
            {
                GlobalVariable *newGVar = new GlobalVariable(TheModule,Type::getInt32Ty(TheContext),false,GlobalVariable::ExternalLinkage,const_int_val,name);
                vars.push_back(newGVar);
            }
            break;
        case Char:
            {
                GlobalVariable *newGVar = new GlobalVariable(TheModule,Type::getInt32Ty(TheContext),false,GlobalVariable::ExternalLinkage,const_char_val,name);
                vars.push_back(newGVar);
            }
            break;
        case Float:
            {
                GlobalVariable *newGVar = new GlobalVariable(TheModule,Type::getFloatTy(TheContext),false,GlobalVariable::ExternalLinkage,const_float_val,name);
                vars.push_back(newGVar);
            }
            break;
        default:
            break;
        }
    }
    
    return vars;
}

void buildVarDefine(Operation *opt,BasicBlock *block){
    DefineOpt *defineOpt = (DefineOpt*)opt;
    for (int i = 0; i < defineOpt ->names.size(); i++)
    {
        string name = defineOpt -> names[i];
        switch (defineOpt ->type)
        {
        case Int:
            {
                auto AI = new AllocaInst(Type::getInt32Ty(TheContext),0,name,block);
            }
            
            break;
        case Char:
            {
                auto AI = new AllocaInst(Type::getInt32Ty(TheContext),0,name,block);
            }
            break;
        case Float:
            {
                auto AI = new AllocaInst(Type::getFloatTy(TheContext),0,name,block);
            }
            break;
        default:
            break;
        }
    }
    
}

Value* buildSimpleOpts(Operation *opt,VariableList *list,BasicBlock *block){
    // vector<Operation *> opts;
    // if (opt->kind != ID && opt->kind != INTEGER && opt->kind != FLOAT && opt->kind != CHAR && opt->kind != FUNC_CALL)
    // {
    //     Operation *temp = opt;
    //     while (temp)
    //     {
    //         opts.push_back(temp);
    //         RightOpt *rOpt = (RightOpt*)temp;
    //         temp = rOpt -> right;
    //     }

    //     while (opts.size())
    //     {
    //         Operation *opt = opts.back();
    //         opts.pop_back();

            

    //         default:
    //             break;
    //         }
    //     }
        
        
    // }
    

    
    switch (opt->kind)
    {
        case ID:
            {
                VarUseOpt *newOp = (VarUseOpt*) opt;
                auto var = search_variable_symbol_llvm(newOp->name,list);
                auto ai = var->llvmAI;
                return ai;
            }
            break;
        case INTEGER :case FLOAT: case CHAR:
            {
                StaticValueOpt *newOp = (StaticValueOpt*)opt;
                if (newOp -> kind == INTEGER || newOp -> kind == CHAR)
                {
                    if (newOp -> kind == INTEGER)
                    {
                        int value = get<int>(newOp -> data);
                        if (newOp -> return_type == Float){
                            auto v = ConstantInt::get(Type::getFloatTy(TheContext), (float)value);
                            return v;
                        }else{
                            auto v = ConstantInt::get(Type::getInt32Ty(TheContext), value);
                            return v;
                        }
                        
                    }else{
                        int value = (int)get<char>(newOp -> data);
                        auto v = ConstantInt::get(Type::getInt32Ty(TheContext), value);
                        return v;
                    }
                    // std::string tempName("PreservedTempNameWithIndex" + std::to_string(tempIndex));
                }else{
                    float value = get<float>(newOp -> data);
                    auto v = ConstantInt::get(Type::getFloatTy(TheContext), value);
                    return v;
                }
                
            }
            break;
        case NOT: 
            {
                RightOpt *rOpt = (RightOpt*)opt;
                auto result = buildSimpleOpts(rOpt -> right,list,block);
                std::string tempName("PreservedTempNameWithIndex" + std::to_string(tempIndex));
                tempIndex ++;
                auto returnValue = builder_stack.back().CreateNot(result,tempName);
                return returnValue;
            }
            break;
        case INCREASE: 
            {
                RightOpt *rOpt = (RightOpt*)opt;
                auto l = buildSimpleOpts(rOpt -> right,list,block);
                auto r = ConstantInt::get(Type::getInt32Ty(TheContext), 1);
                auto returnValue = builder_stack.back().CreateAdd(l,r,l->getName());
                return returnValue;
            }
            break;
        case DECREASE:
            {
                RightOpt *rOpt = (RightOpt*)opt;
                auto l = buildSimpleOpts(rOpt -> right,list,block);
                auto r = ConstantInt::get(Type::getInt32Ty(TheContext), 1);
                auto returnValue = builder_stack.back().CreateSub(l,r,l->getName());
                return returnValue;
            }
            break;
        case COMPARE:
            {
                CompareOpt *cOpt = (CompareOpt *)opt;
                bool useFloat = false;
                if (cOpt -> left -> return_type == Float || cOpt -> right -> return_type == Float)
                {
                    useFloat = true;
                }
                switch (cOpt ->type)
                {
                case Equal:
                    {
                        auto l = buildSimpleOpts(cOpt -> left,list,block);
                        auto r = buildSimpleOpts(cOpt -> right,list,block);
                        std::string tempName("PreservedTempNameWithIndex" + std::to_string(tempIndex));
                        tempIndex ++;
                        if (useFloat)
                        {
                            auto result = builder_stack.back().CreateFCmpOEQ(l,r,tempName);
                            return result;
                        }else{
                            auto result = builder_stack.back().CreateICmpEQ(l,r,tempName);
                            return result;
                        }
                    }
                    break;
                case NEqual:
                    {
                        auto l = buildSimpleOpts(cOpt -> left,list,block);
                        auto r = buildSimpleOpts(cOpt -> right,list,block);
                        std::string tempName("PreservedTempNameWithIndex" + std::to_string(tempIndex));
                        tempIndex ++;
                        if (useFloat)
                        {
                            auto result = builder_stack.back().CreateFCmpONE(l,r,tempName);
                            return result;
                        }else{
                            auto result = builder_stack.back().CreateICmpNE(l,r,tempName);
                            return result;
                        }
                    }
                    break;
                case Large:
                    {
                        auto l = buildSimpleOpts(cOpt -> left,list,block);
                        auto r = buildSimpleOpts(cOpt -> right,list,block);
                        std::string tempName("PreservedTempNameWithIndex" + std::to_string(tempIndex));
                        tempIndex ++;
                        if (useFloat)
                        {
                            auto result = builder_stack.back().CreateFCmpOGT(l,r,tempName);
                            return result;
                        }else{
                            auto result = builder_stack.back().CreateICmpSGT(l,r,tempName);
                            return result;
                        }
                    }
                    break;
                case Small:
                    {
                        auto l = buildSimpleOpts(cOpt -> left,list,block);
                        auto r = buildSimpleOpts(cOpt -> right,list,block);
                        std::string tempName("PreservedTempNameWithIndex" + std::to_string(tempIndex));
                        tempIndex ++;
                        if (useFloat)
                        {
                            auto result = builder_stack.back().CreateFCmpOLT(l,r,tempName);
                            return result;
                        }else{
                            auto result = builder_stack.back().CreateICmpSLT(l,r,tempName);
                            return result;
                        }
                    }
                    break;
                case LargeEqual:
                    {
                        auto l = buildSimpleOpts(cOpt -> left,list,block);
                        auto r = buildSimpleOpts(cOpt -> right,list,block);
                        std::string tempName("PreservedTempNameWithIndex" + std::to_string(tempIndex));
                        tempIndex ++;
                        if (useFloat)
                        {
                            auto result = builder_stack.back().CreateFCmpOGE(l,r,tempName);
                            return result;
                        }else{
                            auto result = builder_stack.back().CreateICmpSGE(l,r,tempName);
                            return result;
                        }
                    }
                    break;
                case SmallEqual:
                    {
                        auto l = buildSimpleOpts(cOpt -> left,list,block);
                        auto r = buildSimpleOpts(cOpt -> right,list,block);
                        std::string tempName("PreservedTempNameWithIndex" + std::to_string(tempIndex));
                        tempIndex ++;
                        if (useFloat)
                        {
                            auto result = builder_stack.back().CreateFCmpOLE(l,r,tempName);
                            return result;
                        }else{
                            auto result = builder_stack.back().CreateICmpSLE(l,r,tempName);
                            return result;
                        }
                    }
                    break;
                default:
                    break;
                }
            }
        case AND: case OR: 
            {
                NormalOpt *nOpt = (NormalOpt *)opt;
                auto l = buildSimpleOpts(nOpt -> left,list,block);
                auto r = buildSimpleOpts(nOpt -> right,list,block);
                std::string tempName("PreservedTempNameWithIndex" + std::to_string(tempIndex));
                tempIndex ++;
                if (opt -> kind == AND)
                {
                    auto result = builder_stack.back().CreateAnd(l,r,tempName);
                return result;
                }else{
                    auto result = builder_stack.back().CreateOr(l,r,tempName);
                    return result;
                }
                
            }
            break;
        case PLUS: 
        case MINUS: 
        case MULTI: 
        case DIVID:
            {
                NormalOpt *nOpt = (NormalOpt *)opt;
                auto l = buildSimpleOpts(nOpt -> left,list,block);
                auto r = buildSimpleOpts(nOpt -> right,list,block);
                bool useFloat = false;
                if (nOpt -> left -> return_type == Float || nOpt -> right -> return_type == Float)
                {
                    useFloat = true;
                }
                switch (opt -> kind)
                {
                case PLUS:
                    if (useFloat)
                    {
                        auto result = builder_stack.back().CreateFAdd(l,r,l->getName());
                        return result;
                    }else{
                        auto result = builder_stack.back().CreateAdd(l,r,l->getName());
                        return result;
                    }
                    break;
                case MINUS:
                    if (useFloat)
                    {
                        auto result = builder_stack.back().CreateFSub(l,r,l->getName());
                        return result;
                    }else{
                        auto result = builder_stack.back().CreateSub(l,r,l->getName());
                        return result;
                    }
                case MULTI:
                    if (useFloat)
                    {
                        auto result = builder_stack.back().CreateFMul(l,r,l->getName());
                        return result;
                    }else{
                        auto result = builder_stack.back().CreateMul(l,r,l->getName());
                        return result;
                    }
                case DIVID:
                    if (useFloat)
                    {
                        auto result = builder_stack.back().CreateFDiv(l,r,l->getName());
                        return result;
                    }else{
                        auto result = builder_stack.back().CreateSDiv(l,r,l->getName());
                        return result;
                    }
                    break;
                default:
                    break;
                }
            }

        case FUNC_CALL:
            //MARK:TODO
            break;
        default:
            break;
    }
}

// variant<char,int,float> 