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


static llvm::LLVMContext TheContext;
static llvm::Module *ModuleOb = new llvm::Module("My Compiler", TheContext);
static std::vector<std::string> FuncArgs;

using BBList  = llvm::SmallVector<llvm::BasicBlock *, 16>;
using ValList = llvm::SmallVector<llvm::Value *, 16>;


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

llvm::Value *createIfElse(llvm::IRBuilder<> &Builder, BBList List, ValList VL)
{
    llvm::Value *CondTn = VL[0];
    llvm::Value *Arg1   = VL[1];

    llvm::BasicBlock *ThenBB  = List[0];
    llvm::BasicBlock *ElseBB  = List[1];
    llvm::BasicBlock *MergeBB = List[2];

    Builder.CreateCondBr(CondTn, ThenBB, ElseBB);

    // then
    Builder.SetInsertPoint(ThenBB);
    llvm::Value *ThenVal = Builder.CreateAdd(Arg1, Builder.getInt32(1), "thenAddTmp");
    Builder.CreateBr(MergeBB);

    // else
    Builder.SetInsertPoint(ElseBB);
    llvm::Value *ElseVal = Builder.CreateAdd(Arg1, Builder.getInt32(2), "elseAddTmp");
    Builder.CreateBr(MergeBB);

    unsigned PhiBBSize = List.size() - 1;
    Builder.SetInsertPoint(MergeBB);
    llvm::PHINode *Phi = Builder.CreatePHI(llvm::Type::getInt32Ty(TheContext), PhiBBSize, "iftmp");

    Phi->addIncoming(ThenVal, ThenBB);
    Phi->addIncoming(ElseVal, ElseBB);

    return Phi;
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

    llvm::Value *Arg1     = fooFunc->arg_begin();                    // use a
    llvm::Value *constant = Builder.getInt32(32);                    // constant = 32
    llvm::Value *val      = createArith(Builder, Arg1, constant);    // val = a  * constant

    llvm::Value *val2    = Builder.getInt32(100);                         // val2 = 100
    llvm::Value *Compare = Builder.CreateICmpULT(val, val2, "cmptmp");    //
    llvm::Value *CondTn  = Builder.CreateICmpNE(Compare, Builder.getInt1(0), "ifcond");


    ValList VL;
    VL.push_back(CondTn);
    VL.push_back(Arg1);

    llvm::BasicBlock *ThenBB  = createBasicBlock(fooFunc, "then");
    llvm::BasicBlock *ElseBB  = createBasicBlock(fooFunc, "else");
    llvm::BasicBlock *MergeBB = createBasicBlock(fooFunc, "ifcont");

    BBList List;
    List.push_back(ThenBB);
    List.push_back(ElseBB);
    List.push_back(MergeBB);

    llvm::Value *v = createIfElse(Builder, List, VL);

    Builder.CreateRet(v);
    llvm::verifyFunction(*fooFunc);
    ModuleOb->print(llvm::errs(), nullptr);

    return 0;
}