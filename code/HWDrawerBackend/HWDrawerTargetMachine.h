#ifndef HWDRAWERTARGETMACHINE_H
#define HWDRAWERTARGETMACHINE_H

#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetData.h"

namespace llvm {
  class HWDrawerTargetMachine : public TargetMachine {
  public:
  HWDrawerTargetMachine(const Target &T, StringRef TT,
			StringRef CPU, StringRef FS,
			const TargetOptions &Options,
			Reloc::Model RM, CodeModel::Model CM,
			CodeGenOpt::Level OL)
    : TargetMachine(T, TT, CPU, FS, Options) {}

    bool addPassesToEmitFile(PassManagerBase &PM,
			     formatted_raw_ostream &Out,
			     CodeGenFileType FileType,
			     bool DisableVerify = true);

    const TargetData *getTargetData() const { return 0; }
  };
}

#endif
