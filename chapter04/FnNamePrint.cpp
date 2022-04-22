#include "llvm/Pass.h"
#include <llvm/IR/Function.h>
#include <llvm/Support/raw_ostream.h>

#define DEBUG_TYPE "FnNamePrint"

namespace
{
    struct FnNamePrint : public llvm::FunctionPass
    {
        static char ID;
        FnNamePrint() : llvm::FunctionPass(ID) {}

        bool runOnFunction(llvm::Function &F) override
        {
            llvm::errs() << "Function " << F.getName() << '\n';
            return false;
        }
    };

}    // namespace


char FnNamePrint::ID = 0;

static llvm::RegisterPass<FnNamePrint> X("fnnameprint", "Function Name Print");
