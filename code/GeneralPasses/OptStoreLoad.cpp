#include "llvm/Pass.h"
#include "llvm/Function.h"
#include "llvm/Instructions.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;
using namespace std;

namespace {
  class OptStoreLoad : public FunctionPass {
  public:
    static char ID;

    OptStoreLoad() : FunctionPass(ID) {}

    virtual bool runOnFunction(Function &F) {
      BasicBlock &BB = F.getEntryBlock();

      bool changed = false;

      for(BasicBlock::iterator I = BB.begin(); I != BB.end(); I++) {
	if(StoreInst *SI = dyn_cast<StoreInst>(I)) {
	  Value *destination = SI->getPointerOperand();

	  BasicBlock::iterator J(I);

	  for(J++; J != BB.end(); J++) {
	    if(LoadInst *LI = dyn_cast<LoadInst>(J)) {
	      if(LI->getPointerOperand() == destination) {
		LI->replaceAllUsesWith(SI->getOperand(0));
		J--;
		LI->removeFromParent();
		changed = true;
	      }
	    }
	  }
	}
      }

      return changed;
    }
  };
}

char OptStoreLoad::ID = 0;

static RegisterPass<OptStoreLoad> X("optstoreload", "Optimizes out Store/Load pairs", false, false);
