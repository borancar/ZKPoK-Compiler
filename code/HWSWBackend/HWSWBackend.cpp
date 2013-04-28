#include "HWSWTargetMachine.h"
#include "llvm/Pass.h"
#include "llvm/Module.h"
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
using namespace llvm;
using namespace std;

namespace {
  Target TheHWSWBackendTarget;

  static RegisterTargetMachine<HWSWTargetMachine> X(TheHWSWBackendTarget);

  static RegisterTarget<> Y(TheHWSWBackendTarget, "hwsw", "HW/SW backend");


  class HWSWWriter : public FunctionPass, public InstVisitor<HWSWWriter> {
    raw_ostream &Out;

  public:
    static char ID;

    map<string, bool> initialized;
    vector<string> const_inits;
    vector<string> operations;
    set<string> vars;

    explicit HWSWWriter(formatted_raw_ostream &o) : FunctionPass(ID), Out(o) {}

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
	  string tmp = "_const_" + val;
	  
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
	operations.push_back("random(" + resolveValue(&I) + ", " + resolveValue(I.getOperand(0)) + ");");
      } else if(intrinsic.find("montexp") != -1) {
	operations.push_back("montexp(" + resolveValue(I.getOperand(0)) + ", " + resolveValue(I.getOperand(1)) + ", " + resolveValue(&I) + ", " + resolveValue(I.getOperand(2)) + ");");
      } else if(intrinsic.find("montpro") != -1) {
	operations.push_back("montpro(" + resolveValue(I.getOperand(0)) + ", " + resolveValue(I.getOperand(1)) + ", " + resolveValue(&I) + ", " + resolveValue(I.getOperand(2)) + ");");
      } else if(intrinsic.find("modadd") != -1) {
	operations.push_back("if(add1024(" + resolveValue(I.getOperand(0)) + ", " + resolveValue(I.getOperand(1)) + ", " + resolveValue(&I) + "))");
	operations.push_back("  subtract1024(" + resolveValue(&I) + ", " + resolveValue(I.getOperand(2)) + ", " + resolveValue(&I) + ");");
	operations.push_back("else if(larger_or_equal(" + resolveValue(&I) + ", " + resolveValue(I.getOperand(2)) + "))");
	operations.push_back("  subtract1024(" + resolveValue(&I) + ", " + resolveValue(I.getOperand(2)) + ", " + resolveValue(&I) + ");");
      }
    }

    void visitLoadInst(LoadInst &I) {
      operations.push_back("copy(" + resolveValue(&I) + ", " + resolveValue(I.getPointerOperand()) + ");");
    }

    void visitStoreInst(StoreInst &I) {
      operations.push_back("copy(" + resolveValue(I.getPointerOperand()) + ", " + resolveValue(I.getValueOperand()) + ");");
    }

    void visitInsertValueInst(InsertValueInst &I) {
      operations.push_back("copy(Out" + boost::lexical_cast<string>(I.getIndices()[0]) + ", " + resolveValue(I.getInsertedValueOperand()) + ");");
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

	  Out << "__xdata unsigned char *Out" << n++;

	  for(el++; el != sttype->element_end(); el++) {
	    Out << ", __xdata unsigned char *Out" << n++;
	  }
	} else {
	  Out << "__xdata unsigned char *Out";
	}
      }

      Function::const_arg_iterator inp = F.arg_begin();

      if(inp != F.arg_end()) {

	if(F.getReturnType()->isVoidTy()) {
	  Out << "__xdata unsigned char *" << inp->getName();
	  inp++;
	}

	while(inp != F.arg_end()) {
	  Out << ", __xdata unsigned char *" << inp->getName();
	  inp++;
	}
      } else {
	if(F.getReturnType()->isVoidTy()) {
	  Out << "void";
	}
      }

      Out << ") {\n";

      set<string>::iterator var = vars.begin();

      for(set<string>::iterator var = vars.begin(); var != vars.end(); var++) {
	Out << "  __xdata unsigned char " << *var << "[SIZE];\n";
      }

      Out << "\n";

      for(vector<string>::iterator op = operations.begin(); op != operations.end(); op++) {
	Out << "  " << *op << "\n";
      }

      Out << "}\n\n";

      return false;
    }

    bool doFinalization(Module &M) {
      return false;
    }
  };
}

char HWSWWriter::ID = 0;

bool HWSWTargetMachine::addPassesToEmitFile(PassManagerBase &PM,
					   formatted_raw_ostream &Out,
					   CodeGenFileType FileType,
					   bool DisableVerify) {
  if(FileType != TargetMachine::CGFT_AssemblyFile) return true;

  PM.add(new HWSWWriter(Out));

  return false;
}
