#ifndef HWSWTARGETMACHINE_H
#define HWSWTARGETMACHINE_H

#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetData.h"

namespace llvm {

  class HWSWTargetMachine : public TargetMachine {
  public:
    HWSWTargetMachine(const Target &T, StringRef TT,
		      StringRef CPU, StringRef FS,
		      const TargetOptions &Options,
		      Reloc::Model RM, CodeModel::Model CM,
		      CodeGenOpt::Level OL) 
      : TargetMachine(T, TT, CPU, FS, Options) {}

    virtual bool addPassesToEmitFile(PassManagerBase &PM,
				     formatted_raw_ostream &Out,
				     CodeGenFileType FileType,
				     bool DisableVerify = true);

    virtual const TargetData *getTargetData() const { return 0; }
  };
}

#endif
