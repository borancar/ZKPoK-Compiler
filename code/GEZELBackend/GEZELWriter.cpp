#include "GEZELWriter.h"

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

char GEZELWriter::ID = 0;

class GEZELWriter::Visitor : public InstVisitor<Visitor> {
  friend class GEZELWriter;
  friend class InstVisitor;

  set<string> globals;
  map<string, int> widths;
  map<string, bool> used;
  vector<string> assign;
  set<string> signals;
  vector<string> blocks;
  set<string> used_blocks;
  map<string, string> block_dp;
  map<string, int> nblocks;

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
};

bool GEZELWriter::runOnFunction(Function &F) {
  v->used.clear();
  v->assign.clear();
  v->signals.clear();
  v->blocks.clear();
  v->used_blocks.clear();
  v->block_dp.clear();

  v->visit(F);

  for(set<string>::iterator block = v->used_blocks.begin(); block != v->used_blocks.end(); block++) {
    Out << "dp " + *block + " : " + v->block_dp[*block] + "\n";
  }

  Out << "\n";

  Out << "dp " << F.getName() << "(\n";

  bool has_global = false;

  set<string>::iterator glob = v->globals.begin();
  Function::const_arg_iterator arg = F.arg_begin();

  if(glob != v->globals.end()) {
    Out << "  in r" << *glob << "_i: ns(" << v->widths[*glob] << "); " << "out r" << *glob << "_o: ns(" << v->widths[*glob] << ")";      

    has_global = true;

    for(glob++; glob != v->globals.end(); glob++) {
      Out << ";\n";
      Out << "  in r" << *glob << "_i: ns(" << v->widths[*glob] << "); " << "out r" << *glob << "_o: ns(" << v->widths[*glob] << ")";      
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

    v->assign.push_back("s" + arg->getName().str() + "=" + arg->getName().str() + ";");

    v->signals.insert("s" + arg->getName().str());
    v->widths["s" + arg->getName().str()] = arg->getType()->getPrimitiveSizeInBits();
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

  for(set<string>::iterator sig = v->signals.begin(); sig != v->signals.end(); sig++) {
    Out << "  sig " << *sig << " : ns(" << v->widths[*sig] << ");\n";
  }

  Out << "\n";

  for(vector<string>::iterator block = v->blocks.begin(); block != v->blocks.end(); block++) {
    Out << "  " << *block << "\n";
  }

  Out << "\n  always {\n";

  for(set<string>::iterator check_glob = v->globals.begin(); check_glob != v->globals.end(); check_glob++) {
    if(!v->used[*check_glob])
      Out << "    r" << *check_glob << "_o=r" << *check_glob << "_i;\n";
  }

  for(vector<string>::iterator conns = v->assign.begin(); conns != v->assign.end(); conns++) {
    Out << "    " << *conns << "\n";
  }

  Out << "  }\n";

  Out << "}\n\n";

  return false;
}

bool GEZELWriter::doFinalization(Module &M) {
  delete v;

  return false;
}

bool GEZELWriter::doInitialization(Module &M) {
  v = new Visitor();

  for(Module::const_global_iterator glob = M.global_begin(); glob != M.global_end(); glob++) {
    string name = glob->getName().str();
	
    v->globals.insert(name);
    v->widths[name] = glob->getType()->getElementType()->getPrimitiveSizeInBits();
    v->used[name] = false;
  }

  return false;
}

