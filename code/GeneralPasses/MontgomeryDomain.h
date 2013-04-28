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

    bool runOnModule(Module &M);
  };
}

#endif
