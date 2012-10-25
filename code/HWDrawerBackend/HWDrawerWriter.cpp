#include "HWDrawerWriter.h"

#include <llvm/Instructions.h>
#include <llvm/Intrinsics.h>
#include <llvm/Constants.h>
#include <llvm/Support/InstVisitor.h>

#include <string>
#include <set>
#include <map>
#include <vector>

#include <boost/lexical_cast.hpp>

using namespace llvm;
using namespace std;

char HWDrawerWriter::ID = 0;

bool HWDrawerWriter::runOnFunction(Function &F) {
  return false;
}

bool HWDrawerWriter::doFinalization(Module &M) {
  return false;
}

bool HWDrawerWriter::doInitialization(Module &M) {
  return false;
}
