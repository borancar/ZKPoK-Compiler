#include "GEZELTargetMachine.h"
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
using namespace llvm;
using namespace std;

namespace {
  Target TheGEZELBackendTarget;

  static RegisterTargetMachine<GEZELTargetMachine> X(TheGEZELBackendTarget);

  static RegisterTarget<> Y(TheGEZELBackendTarget, "gezel", "GEZEL backend");

  class GEZELWriter : public FunctionPass, public InstVisitor<GEZELWriter> {
    raw_ostream &Out;

  public:
    static char ID;

    set<string> globals;
    map<string, int> widths;
    map<string, bool> used;
    vector<string> assign;
    set<string> signals;
    vector<string> blocks;
    set<string> used_blocks;
    map<string, string> block_dp;
    map<string, int> nblocks;

    explicit GEZELWriter(formatted_raw_ostream &o) : FunctionPass(ID), Out(o) {}

    virtual const char *getPassName() const { return "GEZEL backend"; }

    virtual bool doInitialization(Module &M) {
      for(Module::const_global_iterator glob = M.global_begin(); glob != M.global_end(); glob++) {
	string name = glob->getName().str();
	
	globals.insert(name);
	widths[name] = glob->getType()->getElementType()->getPrimitiveSizeInBits();
	used[name] = false;
      }

      return false;
    }

    string resolveValue(Value *V) {
      if(V->hasName()) {
	return "s" + V->getName().str();
      } else {
	if (ConstantInt *x = dyn_cast<ConstantInt>(V)) {
	  return x->getValue().toString(10,true);
	}
      }

      return "";
    }

    void visitLoadInst(LoadInst &I) {
      assign.push_back("s" + I.getName().str() + "=r" + I.getPointerOperand()->getName().str() + "_i; ");

      signals.insert("s" + I.getName().str());
      widths["s" + I.getName().str()] = I.getType()->getPrimitiveSizeInBits();
    }

    void visitStoreInst(StoreInst &I) {
      assign.push_back("r" + I.getPointerOperand()->getName().str() + "_o=s" + I.getValueOperand()->getName().str() + ";");
      used[I.getPointerOperand()->getName().str()] = true;
    }

    void visitCallInst(CallInst &I) {
      string intrinsic = I.getCalledFunction()->getName().str();

      if(intrinsic.find("random") != -1) {
	intrinsic = "random";
      } else if(intrinsic.find("modexp") != -1) {
	intrinsic = "modexp";
      } else if(intrinsic.find("modmul") != -1) {
	intrinsic = "modmul";
      } else if(intrinsic.find("modadd") != -1) {
	intrinsic = "modadd";
      }

      string block_name = intrinsic + boost::lexical_cast<string>(nblocks[intrinsic]++);

      used_blocks.insert(block_name);
      block_dp[block_name] = intrinsic;

      string block = "use " + block_name + "(";

      for(int i = 0; i < I.getNumArgOperands(); i++) {
	block = block + resolveValue(I.getArgOperand(i)) + ", ";
      }

      string output_name = "s" + I.getName().str();

      block = block + output_name + ");";

      blocks.push_back(block);
      signals.insert(output_name);
      widths[output_name] = I.getType()->getPrimitiveSizeInBits();
    }

    void visitTruncInst(TruncInst &I) {
      assign.push_back("s" + I.getName().str() + "=s" + I.getOperand(0)->getName().str() + ";");

      signals.insert("s" + I.getName().str());
      widths["s" + I.getName().str()] = I.getType()->getPrimitiveSizeInBits();
    }

    void visitReturnInst(ReturnInst &I) {
      //if(I.getNumOperands() > 0)
      //assign.push_back("_out=s" + I.getOperand(0)->getName().str() + ";");
    }

    void visitZExtInst(ZExtInst &I) {
      assign.push_back("s" + I.getName().str() + "=s" + I.getOperand(0)->getName().str() + ";");

      signals.insert("s" + I.getName().str());
      widths["s" + I.getName().str()] = I.getType()->getPrimitiveSizeInBits();
    }

    void visitInsertValueInst(InsertValueInst &I) {
      assign.push_back("_out" + boost::lexical_cast<string>(I.getIndices()[0]) + "=s" + I.getInsertedValueOperand()->getName().str() + ";");
    }

    void outputGlobal(string name) {
	Out << "  in r" << name << "_i: ns(" << widths[name] << "); " << "out r" << name << "_o: ns(" << widths[name] << ")";      
    }

    bool runOnFunction(Function &F) {
      used.clear();
      assign.clear();
      signals.clear();
      blocks.clear();
      used_blocks.clear();
      block_dp.clear();

      visit(F);

      for(set<string>::iterator block = used_blocks.begin(); block != used_blocks.end(); block++) {
	Out << "dp " + *block + " : " + block_dp[*block] + "\n";
      }

      Out << "\n";

      Out << "dp " << F.getName() << "(\n";

      bool has_global = false;

      set<string>::iterator glob = globals.begin();
      Function::const_arg_iterator arg = F.arg_begin();

      if(glob != globals.end()) {
	outputGlobal(*glob);

	has_global = true;

	for(glob++; glob != globals.end(); glob++) {
	  Out << ";\n";
	  outputGlobal(*glob);
	}

	if(arg != F.arg_end()) {
	  Out << ";\n";
	}
      }

      if(arg != F.arg_end()) {
	Out << "  in " << arg->getName() << ": ns(" << arg->getType()->getPrimitiveSizeInBits() << ")";
      }

      for(arg++; arg != F.arg_end(); arg++) {
	Out << ";\n  in " << arg->getName() << ": ns(" << arg->getType()->getPrimitiveSizeInBits() << ")";

	assign.push_back("s" + arg->getName().str() + "=" + arg->getName().str() + ";");

	signals.insert("s" + arg->getName().str());
	widths["s" + arg->getName().str()] = arg->getType()->getPrimitiveSizeInBits();
      }

      Type *ret;

      if(!(ret = F.getReturnType())->isVoidTy())
	if(ret->isStructTy()) {
	  StructType *sttype = dyn_cast<StructType>(ret);

	  int n = 0;

	  StructType::element_iterator el = sttype->element_begin();

	  Out << ";\n  out _out" << n << ": ns(" << sttype->getElementType(n)->getPrimitiveSizeInBits() << ")";

	  n++;

	  for(el++; el != sttype->element_end(); el++) {
	    Out << ";\n  out _out" << n << ": ns(" << sttype->getElementType(n)->getPrimitiveSizeInBits() << ")";

	    n++;
	  }
	}

      Out << ") {\n";

      Out << "\n";

      for(set<string>::iterator sig = signals.begin(); sig != signals.end(); sig++) {
	Out << "  sig " << *sig << " : ns(" << widths[*sig] << ");\n";
      }

      Out << "\n";

      for(vector<string>::iterator block = blocks.begin(); block != blocks.end(); block++) {
	Out << "  " << *block << "\n";
      }

      Out << "\n  always {\n";

      for(set<string>::iterator check_glob = globals.begin(); check_glob != globals.end(); check_glob++) {
	if(!used[*check_glob])
	  Out << "    r" << *check_glob << "_o=r" << *check_glob << "_i;\n";
      }

      for(vector<string>::iterator conns = assign.begin(); conns != assign.end(); conns++) {
	Out << "    " << *conns << "\n";
      }

      Out << "  }\n";

      Out << "}\n\n";

      return false;
    }

    virtual bool doFinalization(Module &M) {
      return false;
    }
  };
}

char GEZELWriter::ID = 0;

bool GEZELTargetMachine::addPassesToEmitFile(PassManagerBase &PM,
					     formatted_raw_ostream &Out,
					     CodeGenFileType FileType,
					     CodeGenOpt::Level OptLevel,
					     bool DisableVerify) {
  if(FileType != TargetMachine::CGFT_AssemblyFile) return true;

  PM.add(new GEZELWriter(Out));

  return false;
}
