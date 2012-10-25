#include "HWDrawerTargetMachine.h"
#include "HWDrawerWriter.h"

#include <llvm/PassManager.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/FormattedStream.h>

using namespace llvm;

namespace {
  Target TheHWDrawerBackendTarget;

  static RegisterTargetMachine<HWDrawerTargetMachine> X(TheHWDrawerBackendTarget);

  static RegisterTarget<> Y(TheHWDrawerBackendTarget, "hwdraw", "Draw HW backend");
}

bool HWDrawerTargetMachine::addPassesToEmitFile(PassManagerBase &PM,
						formatted_raw_ostream &Out,
						CodeGenFileType FileType,
						bool DisableVerify /* = true*/) {
  if(FileType != TargetMachine::CGFT_AssemblyFile) return true;

  PM.add(new HWDrawerWriter(Out));

  return false;
}
