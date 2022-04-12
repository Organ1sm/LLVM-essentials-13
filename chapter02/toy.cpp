#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include <llvm/Support/raw_ostream.h>

static llvm::LLVMContext TheContext;
static llvm::Module *ModuleOb = new llvm::Module("My Compiler", TheContext);

int main()
{
    ModuleOb->print(llvm::errs(), nullptr);
    return 0;
}