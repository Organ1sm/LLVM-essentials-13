#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/GlobalValue.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/Support/Alignment.h>


static std::unique_ptr<llvm::LLVMContext> TheContext;
static std::unique_ptr<llvm::Module> TheModule;
static std::unique_ptr<llvm::IRBuilder<>> Builder;

void Init()
{
    TheContext = std::make_unique<llvm::LLVMContext>();
    TheModule  = std::make_unique<llvm::Module>("My Compiler", *TheContext);
    Builder    = std::make_unique<llvm::IRBuilder<>>(*TheContext);
}

llvm::Function *createFunc(std::string name)
{
    llvm::FunctionType *funcType = llvm::FunctionType::get(Builder->getInt32Ty(), false);
    llvm::Function *fooFunc =
        llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, name, *TheModule);

    return fooFunc;
}

llvm::BasicBlock *createBasicBlock(llvm::Function *fooFunc, std::string Name)
{
    return llvm::BasicBlock::Create(*TheContext, Name, fooFunc);
}

llvm::GlobalVariable *createGlob(std::string Name)
{
    TheModule->getOrInsertGlobal(Name, Builder->getInt32Ty());
    llvm::GlobalVariable *gVar = TheModule->getNamedGlobal(Name);
    gVar->setLinkage(llvm::GlobalValue::CommonLinkage);
    gVar->setAlignment(llvm::MaybeAlign(4));

    return gVar;
}

int main()
{
    Init();
    llvm::GlobalVariable *gVar = createGlob("x");
    llvm::Function *fooFunc    = createFunc("foo");
    llvm::BasicBlock *entry    = createBasicBlock(fooFunc, "entry");

    Builder->SetInsertPoint(entry);
    llvm::verifyFunction(*fooFunc);
    TheModule->print(llvm::errs(), nullptr);

    return 0;
}