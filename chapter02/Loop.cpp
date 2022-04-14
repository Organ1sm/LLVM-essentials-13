#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include <llvm/ADT/SmallVector.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/GlobalValue.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Value.h>
#include <llvm/Support/Alignment.h>

static std::unique_ptr<llvm::LLVMContext> TheContext;
static std::unique_ptr<llvm::Module> TheModule;
static std::unique_ptr<llvm::IRBuilder<>> Builder;
static std::vector<std::string> FuncArgs;

void Init()
{
    TheContext = std::make_unique<llvm::LLVMContext>();
    TheModule  = std::make_unique<llvm::Module>("My Compiler", *TheContext);
    Builder    = std::make_unique<llvm::IRBuilder<>>(*TheContext);
    FuncArgs.push_back("a");
    FuncArgs.push_back("b");
}

using BBList  = llvm::SmallVector<llvm::BasicBlock *, 16>;
using ValList = llvm::SmallVector<llvm::Value *, 16>;

llvm::Function *createFunc(std::string name)
{
    std::vector<llvm::Type *> Integers(FuncArgs.size(), llvm::Type::getInt32Ty(*TheContext));
    llvm::FunctionType *funcType = llvm::FunctionType::get(Builder->getInt32Ty(), Integers, false);
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

llvm::Value *createArith(llvm::Value *L, llvm::Value *R) { return Builder->CreateMul(L, R, "multmp"); }


void setFuncArgs(llvm::Function *fooFunc, std::vector<std::string> FuncArgs)
{
    unsigned Index = 0;
    llvm::Function::arg_iterator AI, AE;
    for (AI = fooFunc->arg_begin(), AE = fooFunc->arg_end(); AI != AE; ++AI, ++Index)
    {
        AI->setName(FuncArgs[Index]);
    }
}

llvm::Value *createLoop(BBList List, ValList VL, llvm::Value *StartVal, llvm::Value *EndVal)
{
    llvm::BasicBlock *PreheaderBB = Builder->GetInsertBlock();
    llvm::Value *val              = VL[0];
    llvm::BasicBlock *LoopBB      = List[0];

    Builder->CreateBr(LoopBB);
    Builder->SetInsertPoint(LoopBB);

    llvm::PHINode *IndVar = Builder->CreatePHI(llvm::Type::getInt32Ty(*TheContext), 2, "i");
    IndVar->addIncoming(StartVal, PreheaderBB);

    llvm::Value *Add     = Builder->CreateAdd(val, Builder->getInt32(5), "addtmp");
    llvm::Value *StepVal = Builder->getInt32(1);
    llvm::Value *NextVal = Builder->CreateAdd(IndVar, StepVal, "nextval");
    llvm::Value *EndCond = Builder->CreateICmpULT(IndVar, EndVal, "endcond");
    EndCond              = Builder->CreateICmpNE(EndCond, Builder->getInt1(false), "loopcond");

    llvm::BasicBlock *LoopEndBB = Builder->GetInsertBlock();
    llvm::BasicBlock *AfterBB   = List[1];

    Builder->CreateCondBr(EndCond, LoopBB, AfterBB);
    Builder->SetInsertPoint(AfterBB);
    IndVar->addIncoming(NextVal, LoopEndBB);

    return Add;
}

int main()
{
    Init();

    // Global Variable
    llvm::GlobalVariable *gVar = createGlob("x");

    llvm::Function *fooFunc = createFunc("foo");
    setFuncArgs(fooFunc, FuncArgs);

    llvm::BasicBlock *entry = createBasicBlock(fooFunc, "entry");
    Builder->SetInsertPoint(entry);

    llvm::Function::arg_iterator AI = fooFunc->arg_begin();
    llvm::Value *Arg1               = AI++;                           // use b
    llvm::Value *Arg2               = AI;                             // use a
    llvm::Value *constant           = Builder->getInt32(32);          // constant = 32
    llvm::Value *val                = createArith(Arg1, constant);    // val = a  * constant

    ValList VL;
    VL.push_back(Arg1);

    BBList List;
    llvm::BasicBlock *LoopBB  = createBasicBlock(fooFunc, "loop");
    llvm::BasicBlock *AfterBB = createBasicBlock(fooFunc, "afterloop");
    List.push_back(LoopBB);
    List.push_back(AfterBB);

    llvm::Value *StartVal = Builder->getInt32(1);
    llvm::Value *Res      = createLoop(List, VL, StartVal, Arg2);

    Builder->CreateRet(Res);
    llvm::verifyFunction(*fooFunc);
    TheModule->print(llvm::errs(), nullptr);

    return 0;
}