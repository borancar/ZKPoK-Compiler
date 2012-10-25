#ifndef HWDRAWERWRITER_H
#define HWDRAWERWRITER_H

#include <llvm/Pass.h>
#include <llvm/Support/FormattedStream.h>

class HWDrawerWriter : public llvm::FunctionPass {
  llvm::formatted_raw_ostream &Out;
  class Visitor;
  Visitor *v;

 public:
  static char ID;

  explicit HWDrawerWriter(llvm::formatted_raw_ostream &o) : llvm::FunctionPass(ID), Out(o) {}

  bool doInitialization(llvm::Module &M);

  bool runOnFunction(llvm::Function &F);

  bool doFinalization(llvm::Module &M);
};

#endif
