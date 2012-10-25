#ifndef MONTGOMERYDOMAIN_H
#define MONTGOMERYDOMAIN_H

#include <llvm/Pass.h>

namespace llvm {
  class MontgomeryDomain : public ModulePass {
    class Visitor;
    Visitor *v;

  public:
    static char ID;

    explicit MontgomeryDomain() : ModulePass(ID) {}

    bool doInitialization(Module &M);

    bool runOnModule(Module &M);

    bool doFinalization(Module &M);
  };
}

#endif
