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
    llvm::Type *vecTy = llvm::VectorType::get(u32Ty, 4, false);    // Vec is of vector type <4 x i32>

    llvm::Function *fooFunc = createFunc(Builder->getInt32Ty(), vecTy, funcName);

    setFuncArgs(fooFunc, FuncArgs);

    return fooFunc;
}

llvm::Value *getExtractElement(llvm::Value *vec, llvm::Value *idx)
{
    return Builder->CreateExtractElement(vec, idx);
}


int main()
{
    Init();

    llvm::Function *fooFunc = createFuncProto("Foo");
    auto *entry             = createBasicBlock(fooFunc, "entry");
    Builder->SetInsertPoint(entry);

    llvm::Value *Vec = fooFunc->arg_begin();

    llvm::SmallVector<llvm::Value *, 4> V(4);
    for (std::size_t i = 0; i < 4; i++) { V[i] = std::move(getExtractElement(Vec, Builder->getInt32(i))); }

    auto add1 = Builder->CreateAdd(V[0], V[1]);
    auto add2 = Builder->CreateAdd(add1, V[2]);
    auto add  = Builder->CreateAdd(add2, V[3]);

    Builder->CreateRet(add);

    llvm::verifyFunction(*fooFunc);

    TheModule->print(llvm::errs(), nullptr);

    return 0;
}
