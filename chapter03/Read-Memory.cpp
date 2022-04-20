#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Value.h>
#include <llvm/Support/Alignment.h>
#include <llvm/Support/raw_ostream.h>

static std::unique_ptr<llvm::LLVMContext> TheContext;
static std::unique_ptr<llvm::Module> TheModule;
static std::unique_ptr<llvm::IRBuilder<>> Builder;

void Init()
{
    TheContext = std::make_unique<llvm::LLVMContext>();
    TheModule  = std::make_unique<llvm::Module>("My Compiler", *TheContext);
    Builder    = std::make_unique<llvm::IRBuilder<>>(*TheContext);
}

llvm::Function *createFunc(llvm::Type *RetTy,
                           llvm::ArrayRef<llvm::Type *> Params,
                           std::string Name,
                           bool isVarArg = false)
{
    llvm::FunctionType *funcType = llvm::FunctionType::get(RetTy, Params, isVarArg);
    llvm::Function *fooFunc =
        llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, Name, *TheModule);

    return fooFunc;
}

llvm::BasicBlock *createBasicBlock(llvm::Function *fooFunc, std::string Name)
{
    return llvm::BasicBlock::Create(*TheContext, Name, fooFunc);
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

llvm::Function *createFuncProto(std::string funcName)
{
    std::vector<std::string> FuncArgs;
    FuncArgs.push_back("a");

    llvm::Type *u32Ty = llvm::Type::getInt32Ty(*TheContext);
    llvm::Type *vecTy = llvm::VectorType::get(u32Ty, 2, false);
    llvm::Type *ptrTy = vecTy->getPointerTo(0);

    llvm::Function *fooFunc = createFunc(Builder->getInt32Ty(), {ptrTy}, funcName);

    setFuncArgs(fooFunc, FuncArgs);

    return fooFunc;
}

llvm::Value *getGEP(llvm::Value *base, llvm::ArrayRef<llvm::Value *> offset)
{
    base->getType()->print(llvm::errs());
    llvm::errs() << base->getType()->getTypeID() << '\n';

    return Builder->CreateGEP(base->getType()->getPointerElementType(), base, offset, "a1");
}

llvm::Value *getLoad(llvm::Value *address)
{
    return Builder->CreateLoad(address->getType()->getPointerElementType(), address, "load");
}

// %val = load i32, i32* a1
int main()
{
    Init();

    llvm::Function *fooFunc = createFuncProto("Foo");
    llvm::Value *base       = fooFunc->arg_begin();

    auto *entry = createBasicBlock(fooFunc, "entry");
    Builder->SetInsertPoint(entry);

    llvm::Value *gep  = getGEP(base, {Builder->getInt32(0), Builder->getInt32(1)});
    llvm::Value *load = getLoad(gep);


    Builder->CreateRet(load);

    llvm::verifyFunction(*fooFunc);

    TheModule->print(llvm::errs(), nullptr);

    return 0;
}
