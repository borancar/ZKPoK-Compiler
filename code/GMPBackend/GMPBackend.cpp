#include "GMPTargetMachine.h"
#include "llvm/Pass.h"
#include "llvm/Instructions.h"
#include "llvm/PassManager.h"
#include "llvm/Intrinsics.h"
#include "llvm/DerivedTypes.h"
#include "llvm/ADT/APInt.h"
#include "llvm/Constants.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/InstVisitor.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FormattedStream.h"
#include <string>
#include <set>
#include <map>
#include <vector>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include <gmpxx.h>
using namespace llvm;
using namespace std;

namespace {
  Target TheGMPBackendTarget;

  static RegisterTargetMachine<GMPTargetMachine> X(TheGMPBackendTarget);

  static RegisterTarget<> Y(TheGMPBackendTarget, "gmp", "GMP backend");

  class GMPWriter : public FunctionPass, public InstVisitor<GMPWriter> {
    raw_ostream &Out;

  public:
    static char ID;

    map<string, bool> initialized;
    vector<string> const_inits;
    vector<string> operations;
    set<string> vars;

    explicit GMPWriter(formatted_raw_ostream &o) : FunctionPass(ID), Out(o) {}

    bool doInitialization(Module &M) {
      return false;
    }

    string resolveValue(Value *V) {
      if(!V) return "";

      if(V->hasName()) {
	string name = V->getName().str();

	if(dyn_cast<GlobalValue>(V)) {
	  name = "g" + name;
	} else if(dyn_cast<Argument>(V)) {

	} else {
	  vars.insert(name);
	}

	return name;
      } else {
	if (ConstantInt *x = dyn_cast<ConstantInt>(V)) {
	  string val = x->getValue().toString(10, true);
	  string tmp = "_mpz_" + val;
	  
	  if(!initialized[tmp]) {
	    const_inits.push_back("mpz_set_str(" + tmp + ", \""  + val + "\", 10);\n");
	    initialized[tmp] = true;
	  }
	  
	  return tmp;
	}
      }

      return "";
    }

    void visitCallInst(CallInst &I) {
      string intrinsic = I.getCalledFunction()->getName().str();

      if(intrinsic.find("random") != -1) {
	operations.push_back("mpz_urandomm(" + resolveValue(&I) + ", state, " + resolveValue(I.getOperand(0)) + ");");
      } else if(intrinsic.find("modexp") != -1) {
	operations.push_back("mpz_powm(" + resolveValue(&I) + ", " + resolveValue(I.getOperand(0)) + ", " + resolveValue(I.getOperand(1)) + ", " + resolveValue(I.getOperand(2)) + ");");
      } else if(intrinsic.find("modmul") != -1) {
	operations.push_back("mpz_mul(" + resolveValue(&I) + ", " + resolveValue(I.getOperand(0)) + ", " + resolveValue(I.getOperand(1)) + ");");
	operations.push_back("mpz_mod(" + resolveValue(&I) + ", " + resolveValue(&I) + ", " + resolveValue(I.getOperand(2)) + ");");
      } else if(intrinsic.find("modadd") != -1) {
	operations.push_back("mpz_add(" + resolveValue(&I) + ", " + resolveValue(I.getOperand(0)) + ", " + resolveValue(I.getOperand(1)) + ");");
	operations.push_back("mpz_mod(" + resolveValue(&I) + ", " + resolveValue(&I) + ", " + resolveValue(I.getOperand(2)) + ");");
      }
    }

    void visitLoadInst(LoadInst &I) {
      operations.push_back("mpz_set(" + resolveValue(&I) + ", " + resolveValue(I.getPointerOperand()) + ");");
    }

    void visitStoreInst(StoreInst &I) {
      operations.push_back("mpz_set(" + resolveValue(I.getPointerOperand()) + ", " + resolveValue(I.getValueOperand()) + ");");
    }

    void visitInsertValueInst(InsertValueInst &I) {
      operations.push_back("mpz_set(Out" + boost::lexical_cast<string>(I.getIndices()[0]) + ", " + resolveValue(I.getInsertedValueOperand()) + ");");
    }

    void visitReturnInst(ReturnInst &I) {
      /*if(I.getReturnValue()) {
	operations.push_back("mpz_set(Out, " + resolveValue(I.getReturnValue()) + ");");
      }*/
    }

    bool runOnFunction(Function &F) {
      operations.clear();
      vars.clear();

      visit(F);

      Out << "void ";

      Out << F.getName().str() + "(";

      Type *ret;

      if(!(ret = F.getReturnType())->isVoidTy()) {
	if(ret->isStructTy()) {
	  StructType *sttype = dyn_cast<StructType>(ret);

	  int n = 0;

	  StructType::element_iterator el = sttype->element_begin();

	  Out << "mpz_t Out" << n++;

	  for(el++; el != sttype->element_end(); el++) {
	    Out << ", mpz_t Out" << n++;
	  }
	} else {
	  Out << "mpz_t Out";
	}
      }

      Function::const_arg_iterator inp = F.arg_begin();

      if(inp != F.arg_end()) {

	if(F.getReturnType()->isVoidTy()) {
	  Out << "mpz_t " << inp->getName();
	  inp++;
	}

	while(inp != F.arg_end()) {
	  Out << ", mpz_t " << inp->getName();
	  inp++;
	}
      } else {
	if(F.getReturnType()->isVoidTy()) {
	  Out << "void";
	}
      }

      Out << ") {\n";

      set<string>::iterator var = vars.begin();

      if(var != vars.end()) {
	Out << "  mpz_t " << *var;
	var++;

	while(var != vars.end()) {
	  Out << ", " << *var;
	  var++;
	}

	Out << ";\n";

	for(set<string>::iterator init = vars.begin(); init != vars.end(); init++) {
	  Out << "  mpz_init(" + *init + ");\n";
	}
      }

      Out << "\n";

      for(vector<string>::iterator op = operations.begin(); op != operations.end(); op++) {
	Out << "  " << *op << "\n";
      }

      Out << "\n";

      for(set<string>::iterator clear = vars.begin(); clear != vars.end(); clear++) {
	Out << "  mpz_clear(" + *clear + ");\n";
      }

      Out << "}\n\n";

      return false;
    }

    bool doFinalization(Module &M) {
      return false;
    }
  };
}

char GMPWriter::ID = 0;

bool GMPTargetMachine::addPassesToEmitFile(PassManagerBase &PM,
					   formatted_raw_ostream &Out,
					   CodeGenFileType FileType,
					   CodeGenOpt::Level OptLevel,
					   bool DisableVerify) {
  if(FileType != TargetMachine::CGFT_AssemblyFile) return true;

  PM.add(new GMPWriter(Out));

  return false;
}
