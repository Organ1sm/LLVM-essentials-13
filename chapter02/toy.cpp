#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include <llvm/IR/IRBuilder.h>
#include <llvm/Support/raw_ostream.h>
#include <memory>

static std::unique_ptr<llvm::LLVMContext> TheContext;
static std::unique_ptr<llvm::Module> TheModule;

void Init()
{
    TheContext = std::make_unique<llvm::LLVMContext>();
    TheModule = std::make_unique<llvm::Module>("My Compiler", *TheContext);
}

int main()
{
    Init();
    TheModule->print(llvm::errs(), nullptr);
    return 0;
}