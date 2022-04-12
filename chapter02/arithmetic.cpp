#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/GlobalValue.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/Value.h>
#include <llvm/Support/Alignment.h>


static llvm::LLVMContext TheContext;
static llvm::Module *ModuleOb = new llvm::Module("My Compiler", TheContext);
static std::vector<std::string> FuncArgs;


llvm::Function *createFunc(llvm::IRBuilder<> &Builder, std::string name)
{
    std::vector<llvm::Type *> Integers(FuncArgs.size(), llvm::Type::getInt32Ty(TheContext));
    llvm::FunctionType *funcType = llvm::FunctionType::get(Builder.getInt32Ty(), Integers, false);
    llvm::Function *fooFunc =
        llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, name, ModuleOb);

    return fooFunc;
}

llvm::BasicBlock *createBasicBlock(llvm::Function *fooFunc, std::string Name)
{
    return llvm::BasicBlock::Create(TheContext, Name, fooFunc);
}

llvm::GlobalVariable *createGlob(llvm::IRBuilder<> &Builder, std::string Name)
{
    ModuleOb->getOrInsertGlobal(Name, Builder.getInt32Ty());
    llvm::GlobalVariable *gVar = ModuleOb->getNamedGlobal(Name);
    gVar->setLinkage(llvm::GlobalValue::CommonLinkage);
    gVar->setAlignment(llvm::MaybeAlign(4));

    return gVar;
}

llvm::Value *createArith(llvm::IRBuilder<> &Builder, llvm::Value *L, llvm::Value *R)
{
    return Builder.CreateMul(L, R, "multmp");
}

void setFuncArgs(llvm::Function *fooFunc, std::vector<std::string> FuncArgs)
{
    unsigned Index = 0;
    llvm::Function::arg_iterator AI, AE;
    for (AI = fooFunc->arg_begin(), AE = fooFunc->arg_end(); AI != AE; ++AI, ++Index)
    {
        AI->setName(FuncArgs[Index]);
    }
}

int main()
{
    FuncArgs.push_back("a");
    FuncArgs.push_back("b");
    static llvm::IRBuilder<> Builder(TheContext);

    llvm::GlobalVariable *gVar = createGlob(Builder, "x");
    llvm::Function *fooFunc    = createFunc(Builder, "foo");
    setFuncArgs(fooFunc, FuncArgs);

    llvm::BasicBlock *entry = createBasicBlock(fooFunc, "entry");

    Builder.SetInsertPoint(entry);

    llvm::Value *Args1    = fooFunc->arg_begin();
    llvm::Value *constant = Builder.getInt32(32);
    llvm::Value *val      = createArith(Builder, Args1, constant);

    Builder.CreateRet(val);
    llvm::verifyFunction(*fooFunc);
    ModuleOb->print(llvm::errs(), nullptr);
    return 0;
}