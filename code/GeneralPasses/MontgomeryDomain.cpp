#include "MontgomeryDomain.h"

#include <llvm/Instructions.h>
#include <llvm/Intrinsics.h>
#include <llvm/Constants.h>
#include <llvm/Support/InstVisitor.h>

#include <string>
#include <set>
#include <map>
#include <vector>

#include <boost/lexical_cast.hpp>
#include <gmpxx.h>

using namespace llvm;
using namespace std;

char MontgomeryDomain::ID = 1;

class MontgomeryDomain::Visitor : public InstVisitor<Visitor> {
  friend class MontgomeryDomain;
  friend class InstVisitor;

  Module *Mod;

  CallInst *enterMontgomery(Value *val, Value *mod, Instruction *insertBefore) {
    vector<Type*> types;
    types.push_back(val->getType());
    types.push_back(Type::getIntNTy(Mod->getContext(), 1024));
    types.push_back(mod->getType());

    FunctionType *ftype = FunctionType::get(mod->getType(), types, false);
    Function *mpro = Function::Create(ftype, Function::ExternalLinkage, "montpro", Mod);  

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
    types.push_back(a->getType());
    types.push_back(b->getType());
    types.push_back(mod->getType());

    FunctionType *ftype = FunctionType::get(mod->getType(), types, false);
    Function *mpro = Function::Create(ftype, Function::ExternalLinkage, "montpro", Mod);  

    vector<Value*> args;
    args.push_back(a);
    args.push_back(b);
    args.push_back(mod);
    
    return CallInst::Create(mpro, args, name, insertBefore);
  }

  CallInst *expMontgomery(string name, Value *a, Value *b, Value *mod, Instruction *insertBefore) {
    vector<Type*> types;
    types.push_back(a->getType());
    types.push_back(b->getType());
    types.push_back(mod->getType());

    mpz_class R("2", 10);
    mpz_pow_ui(R.get_mpz_t(), R.get_mpz_t(), 1024);

    mpz_class modul(dyn_cast<ConstantInt>(mod)->getValue().toString(10, true), 10);
    mpz_powm_ui(R.get_mpz_t(), R.get_mpz_t(), 2, modul.get_mpz_t());

    FunctionType *ftype = FunctionType::get(mod->getType(), types, false);
    Function *mpro = Function::Create(ftype, Function::ExternalLinkage, "montpro", Mod);  

    vector<Value*> args;
    args.push_back(a);
    args.push_back(b);
    args.push_back(mod);
    
    return CallInst::Create(mpro, args, name, insertBefore);    
  }
  
  CallInst *leaveMontgomery(Value *val, Value *mod, Instruction *insertBefore) {
    vector<Type*> types;
    types.push_back(val->getType());
    types.push_back(Type::getIntNTy(Mod->getContext(), 1024));
    types.push_back(mod->getType());

    FunctionType *ftype = FunctionType::get(mod->getType(), types, false);
    Function *mpro = Function::Create(ftype, Function::ExternalLinkage, "montpro", Mod);  

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
};

bool MontgomeryDomain::runOnModule(Module &M) {
  v = new Visitor();

  v->Mod = &M;

  v->visit(M);

  delete v;

  return true;
}

static RegisterPass<MontgomeryDomain> X("montgomery", "Implements modular arithmetic using Montgomery multiplication", false, false);
