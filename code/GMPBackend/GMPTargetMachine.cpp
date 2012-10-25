#include "GMPTargetMachine.h"
#include "GMPWriter.h"

#include <llvm/PassManager.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/FormattedStream.h>

using namespace llvm;

namespace {
  Target TheGMPBackendTarget;

  static RegisterTargetMachine<GMPTargetMachine> X(TheGMPBackendTarget);

  static RegisterTarget<> Y(TheGMPBackendTarget, "gmp", "GMP backend");
}

bool GMPTargetMachine::addPassesToEmitFile(PassManagerBase &PM,
					   formatted_raw_ostream &Out,
					   CodeGenFileType FileType,
					   bool DisableVerify /* = true*/) {
  if(FileType != TargetMachine::CGFT_AssemblyFile) return true;

  PM.add(new GMPWriter(Out));

  return false;
}
