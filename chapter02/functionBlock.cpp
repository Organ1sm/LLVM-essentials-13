#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>


static llvm::LLVMContext TheContext;
static llvm::Module *ModuleOb = new llvm::Module("My Compiler", TheContext);

llvm::Function *createFunc(llvm::IRBuilder<> &Builder, std::string name)
{
    llvm::FunctionType *funcType = llvm::FunctionType::get(Builder.getInt32Ty(), false);
    llvm::Function *fooFunc =
        llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, name, ModuleOb);

    return fooFunc;
}

llvm::BasicBlock *createBasicBlock(llvm::Function *fooFunc, std::string Name)
{
    return llvm::BasicBlock::Create(TheContext, Name, fooFunc);
}

int main()
{
    static llvm::IRBuilder<> Builder(TheContext);
    llvm::Function *fooFunc = createFunc(Builder, "foo");
    llvm::BasicBlock *entry = createBasicBlock(fooFunc, "entry");

    Builder.SetInsertPoint(entry);
    llvm::verifyFunction(*fooFunc);
    ModuleOb->print(llvm::errs(), nullptr);
    return 0;
}