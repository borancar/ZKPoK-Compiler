#include "GEZELTargetMachine.h"
#include "GEZELWriter.h"

#include <llvm/PassManager.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/FormattedStream.h>

using namespace llvm;

namespace {
  Target TheGEZELBackendTarget;

  static RegisterTargetMachine<GEZELTargetMachine> X(TheGEZELBackendTarget);

  static RegisterTarget<> Y(TheGEZELBackendTarget, "gezel", "GEZEL backend");
}

bool GEZELTargetMachine::addPassesToEmitFile(PassManagerBase &PM,
					     formatted_raw_ostream &Out,
					     CodeGenFileType FileType,
					     bool DisableVerify /* = true*/) {
  if(FileType != TargetMachine::CGFT_AssemblyFile) return true;

  PM.add(new GEZELWriter(Out));

  return false;
}
