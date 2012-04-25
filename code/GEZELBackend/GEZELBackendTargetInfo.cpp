#include "GEZELTargetMachine.h"
#include "llvm/Module.h"
#include "llvm/Support/TargetRegistry.h"
using namespace llvm;

Target llvm::TheGEZELBackendTarget;

extern "C" void LLVMInitializeGEZELBackendTargetInfo() {
  RegisterTarget<> X(TheGEZELBackendTarget, "gezel", "GEZEL backend");
}

extern "C" void LLVMInitializeGEZELBackendTargetMC() {}
