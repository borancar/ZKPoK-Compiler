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
#include <gmpxx.h>
using namespace llvm;
using namespace std;

class MontgomeryDomain : public ModulePass, public InstVisitor<MontgomeryDomain> {
  Module *Mod;
  
public:
  static char ID;

  explicit MontgomeryDomain() : ModulePass(ID) {}
  
  CallInst *enterMontgomery(Value *val, Value *mod, Instruction *insertBefore) {
    vector<Type*> types;
    types.push_back(mod->getType());
    types.push_back(val->getType());
    types.push_back(Type::getIntNTy(Mod->getContext(), 1024));
    types.push_back(mod->getType());

    Function *mpro = Intrinsic::getDeclaration(Mod, Intrinsic::montpro, types);  

    mpz_class R("2", 10);
    mpz_pow_ui(R.get_mpz_t(), R.get_mpz_t(), 1024);

    mpz_class modul(dyn_cast<ConstantInt>(mod)->getValue().toString(10, true), 10);
    mpz_powm_ui(R.get_mpz_t(), R.get_mpz_t(), 2, modul.get_mpz_t());

    vector<Value*> args;
    args.push_back(val);
    args.push_back(ConstantInt::get(Type::getIntNTy(Mod->getContext(),1024), R.get_str(), 10));
    args.push_back(mod);
    
    return CallInst::Create(mpro, args, val->getName().str() + "t", insertBefore);
  }

  CallInst *mulMontgomery(string name, Value *a, Value *b, Value *mod, Instruction *insertBefore) {
    vector<Type*> types;
    types.push_back(mod->getType());
    types.push_back(a->getType());
    types.push_back(b->getType());
    types.push_back(mod->getType());

    Function *mpro = Intrinsic::getDeclaration(Mod, Intrinsic::montpro, types);  

    vector<Value*> args;
    args.push_back(a);
    args.push_back(b);
    args.push_back(mod);
    
    return CallInst::Create(mpro, args, name, insertBefore);
  }

  CallInst *expMontgomery(string name, Value *a, Value *b, Value *mod, Instruction *insertBefore) {
    vector<Type*> types;
    types.push_back(mod->getType());
    types.push_back(a->getType());
    types.push_back(b->getType());
    types.push_back(mod->getType());

    mpz_class R("2", 10);
    mpz_pow_ui(R.get_mpz_t(), R.get_mpz_t(), 1024);

    mpz_class modul(dyn_cast<ConstantInt>(mod)->getValue().toString(10, true), 10);
    mpz_powm_ui(R.get_mpz_t(), R.get_mpz_t(), 2, modul.get_mpz_t());

    Function *mpro = Intrinsic::getDeclaration(Mod, Intrinsic::montexp, types);  

    vector<Value*> args;
    args.push_back(a);
    args.push_back(b);
    args.push_back(mod);
    
    return CallInst::Create(mpro, args, name, insertBefore);    
  }
  
  CallInst *leaveMontgomery(Value *val, Value *mod, Instruction *insertBefore) {
    vector<Type*> types;
    types.push_back(mod->getType());
    types.push_back(val->getType());
    types.push_back(Type::getIntNTy(Mod->getContext(), 1024));
    types.push_back(mod->getType());

    Function *mpro = Intrinsic::getDeclaration(Mod, Intrinsic::montpro, types);  

    vector<Value*> args;
    
    args.push_back(val);
    args.push_back(ConstantInt::get(Mod->getContext(), APInt(1024, 1)));
    args.push_back(mod);
    
    return CallInst::Create(mpro, args, val->getName().str(), insertBefore);
  }

  void visitCallInst(CallInst &I) {
    string intrinsic = I.getCalledFunction()->getName().str();

    if(intrinsic.find("modmul") != -1) {

      CallInst *enterMontpro1 = enterMontgomery(I.getOperand(0), I.getOperand(2), &I);
      CallInst *enterMontpro2 = enterMontgomery(I.getOperand(1), I.getOperand(2), &I);
      CallInst *mulMontpro = mulMontgomery(I.getName().str(), enterMontpro1, enterMontpro2, I.getOperand(2), &I);
      CallInst *exitMontpro = leaveMontgomery(mulMontpro, I.getOperand(2), &I);

      I.replaceAllUsesWith(exitMontpro);

      I.removeFromParent();
    } else if(intrinsic.find("modexp") != -1) {
      
      CallInst *enterMontpro1 = enterMontgomery(I.getOperand(0), I.getOperand(2), &I);
      CallInst *expMontpro = expMontgomery(I.getName().str(), enterMontpro1, I.getOperand(1), I.getOperand(2), &I);
      CallInst *exitMontpro = leaveMontgomery(expMontpro, I.getOperand(2), &I);

      I.replaceAllUsesWith(exitMontpro);

      I.eraseFromParent();
    }

  }

  bool runOnModule(Module &M) {
    Mod = &M;

    visit(M);

    return true;
  }
};

char MontgomeryDomain::ID = 1;

static RegisterPass<MontgomeryDomain> X("montgomery", "Implements modular arithmetic using Montgomery multiplication", false, false);
