tree grammar codegen;

options {
	tokenVocab=pil;
	ASTLabelType=pANTLR3_BASE_TREE;
	language=C;
}

@includes {
	#include "llvm/DerivedTypes.h"
	#include "llvm/ExecutionEngine/ExecutionEngine.h"
	#include "llvm/ExecutionEngine/JIT.h"
	#include "llvm/LLVMContext.h"
	#include "llvm/Linker.h"
	#include "llvm/Module.h"
	#include "llvm/Intrinsics.h"
	#include "llvm/PassManager.h"
	#include "llvm/Analysis/Verifier.h"
	#include "llvm/Analysis/Passes.h"
	#include "llvm/Target/TargetData.h"
	#include "llvm/Transforms/Scalar.h"
	#include "llvm/Support/IRBuilder.h"
	#include "llvm/Support/TargetSelect.h"
	#include <algorithm>
	#include <cstdio>
	#include <cstdlib>
	#include <vector>
	#include <map>
	#include <stack>
	#include "GroupType.h"
	using namespace std;
	using namespace llvm;
	
	struct Variable {
		bool variable;
		NumberT *type;
		Value *value;
	};
	
	struct Arg {
		const char *id;
		NumberT *type;
		Value *value;
		bool global_reference;
	};
}

@postinclude {
	struct cmp_str
	{
		bool operator()(const char *a, const char *b)
		{
			return strcmp(a, b) < 0;
		}
	};
	
	static Module *Common = new Module("Common", getGlobalContext());
	static Module *TheModule;
	static IRBuilder<> Builder(getGlobalContext());
	static stack< map<const char*, Variable, cmp_str>* > Vars;
	static map<const char*, NumberT*, cmp_str> Types;
				
	Value* operator_call(const char *name, Intrinsic::ID id, Value *a, Value *b, Value *mod)
	{
		vector<Type*> types;
		
		types.push_back(mod->getType());
		types.push_back(a->getType());
		types.push_back(b->getType());
		types.push_back(mod->getType());
	
		Function *function = Intrinsic::getDeclaration(TheModule, id, types);
		
		vector<Value*> args;
		
		args.push_back(a);
		args.push_back(b);
		args.push_back(mod);
		
		return Builder.CreateCall(function, args, name);
	}
	
	void init_common() 
	{	
		Function::Create(
			FunctionType::get(
				Type::getInt1Ty(getGlobalContext()),
				vector<Type *>(1, Type::getIntNTy(getGlobalContext(), 1024)),
				false),
			Function::ExternalLinkage, "CheckMembership", Common);
		
		Function::Create(
			FunctionType::get(
				Type::getInt1Ty(getGlobalContext()),
				vector<Type *>(1, Type::getInt1Ty(getGlobalContext())),
				false),
			Function::ExternalLinkage, "Verify", Common);
	}

	NumberT *NumberT::addWithSubFrom(const NumberT *first) const { return const_cast<NumberT*>(first); }
	NumberT *NumberT::addWithSubFrom(const GroupT *first) const { return const_cast<GroupT*>(first); }
	  
	NumberT *NumberT::mulWithExpOn(const NumberT *first) const { return const_cast<NumberT*>(first); }
	NumberT *NumberT::mulWithExpOn(const GroupT *first) const { return const_cast<GroupT*>(first); }
	
	Value * NumberT::createNeg(const char *id, Value *a) const  {
		return Builder.CreateNeg(a, id);
	}
	
	Value * NumberT::createAdd(const char *id, Value *a, Value *b) const {
		return Builder.CreateAdd(a, b, id);
	}
	
	Value * NumberT::createSub(const char *id, Value *a, Value *b) const {
		return Builder.CreateSub(a, b, id);
	}
	
	Value * NumberT::createMul(const char *id, Value *a, Value *b) const {
		return Builder.CreateMul(a, b, id);
	}
	
	Value * NumberT::createExp(const char *id, Value *a, Value *b) const {
		return Builder.CreateMul(a, b, id);
	}
	
	Value * GroupT::createNeg(const char *id, Value *a) const {
		return operator_call(id, Intrinsic::modsub, ConstantInt::get(getGlobalContext(), APInt(1024, 0)), a, this->getModulusConstant());
	}
	
	Value * GroupT::createAdd(const char *id, Value *a, Value *b) const {
		return operator_call(id, Intrinsic::modadd, a, b, this->getModulusConstant()); 	
	}
	
	Value * GroupT::createSub(const char *id, Value *a, Value *b) const {
		return operator_call(id, Intrinsic::modsub, a, b, this->getModulusConstant()); 		
	}
	
	Value * GroupT::createMul(const char *id, Value *a, Value *b) const {
		return operator_call(id, Intrinsic::modmul, a, b, this->getModulusConstant()); 	
	}
	
	Value * GroupT::createExp(const char *id, Value *a, Value *b) const {
		return operator_call(id, Intrinsic::modexp, a, b, this->getModulusConstant()); 	
	}
}

proof[const char *part]
	: { init_common(); Vars.push(new map<const char*, Variable, cmp_str>()); }^(PROOF execution_order common (block[$part])*)
		{ delete Vars.top(); Vars.pop(); }
	;

execution_order
	:	^(ORDER (step)*)
	;
	
step	:	^(IDENT IDENT)
	;

common	:	^(BLOCK ^(IDENT { TheModule = Common; } param? global? function?))
	;

block[const char *part]
	:	^(BLOCK ^(IDENT {
			Linker *linker = new Linker((const char*)$IDENT.text->chars, (const char*)$IDENT.text->chars, getGlobalContext());
			linker->LinkInModule(Common);
			TheModule = linker->getModule();
			map<const char*, Variable, cmp_str> previous = (*(Vars.top()));
			Vars.push(new map<const char*, Variable, cmp_str>(previous));
			} param? global? function?)) { TheModule->dump(); delete Vars.top(); Vars.pop();}
	;
	
param	:	^(PARAM (param_declaration)*)
	;

global	:	^(VARIABLE (global_declaration)*)
	;

function:	^(FUNCTION (function_declaration)*)
	;

param_declaration returns [Value *value]
	:	^(par=IDENT type=type_declaration
			(NUMBER
			{$value = ConstantInt::get(getGlobalContext(), APInt($type.type->getBitWidth(), (const char*)$NUMBER.text->chars, 10));}
			| {$value = ConstantInt::get(getGlobalContext(), APInt($type.type->getBitWidth(), 0)); }))
	{
		Variable parx;
		parx.value = $value;
		parx.type = $type_declaration.type;
		parx.variable = false;

		(*(Vars.top()))[(const char*) $par.text->chars] = parx;
	}
	;

global_declaration returns [Value *value]
	: ^(var=IDENT type_declaration
	{ 	$value = new GlobalVariable(*TheModule, $type_declaration.type->getType(), false, GlobalVariable::ExternalLinkage, 0, (const char *) $var.text->chars, 0, true, 0);
	} (NUMBER)?)
	{
		Variable varx;
		varx.value = $value;
		varx.type = $type_declaration.type;
		varx.variable = true;

		(*(Vars.top()))[(const char*) $var.text->chars] = varx;
	}
	;

argument_declaration returns [Arg *arg]
	: (^(IDENT type_declaration))=> ^(var=IDENT type_declaration {
		arg = new Arg();
		$arg->id = (const char*)$IDENT.text->chars;
		$arg->type = $type_declaration.type;
		$arg->global_reference = false;
		})
	| (var=IDENT {
		arg = new Arg();
		$arg->id = (const char*)$IDENT.text->chars;
		$arg->type = (*(Vars.top()))[$arg->id].type;
		$arg->value = (*(Vars.top()))[$arg->id].value;
		$arg->global_reference = true;
		})
	;

type_declaration returns [NumberT *type]
	:	group {$type = $group.type; }
	|	interval {$type = $interval.type; }
	;

function_declaration returns [Function *func, vector<Arg *> outs, vector<Arg *> inps]
	:	^(IDENT ^(OUTPUT ('Void'|(out=argument_declaration { $outs.push_back($out.arg); })*)) 
			^(INP ('Void'|(inp=argument_declaration { $inps.push_back($inp.arg); })*)) 
		{
			vector<Type *> Inps;
			
			for(vector<Arg *>::iterator i = $inps.begin(); i != $inps.end(); i++) {
				Inps.push_back((*i)->type->getType());
			}
			
			vector<Type *> Outs;
			
			for(vector<Arg *>::iterator i = $outs.begin(); i != $outs.end(); i++) {
				Outs.push_back((*i)->type->getType());
			}

			FunctionType *FT;
			
			if($outs.size() == 0) {
				FT = FunctionType::get(Type::getVoidTy(getGlobalContext()), Inps, false);
			} else {
				FT = FunctionType::get(StructType::create(getGlobalContext(), Outs, "rty"), Inps, false);
			}
		
			$func = Function::Create(FT, Function::ExternalLinkage, (const char*) $IDENT.text->chars, TheModule);
			
			BasicBlock *entry_block = BasicBlock::Create(getGlobalContext(), "entry", $func);
			
			Builder.SetInsertPoint(entry_block);
			
			for(vector<Arg*>::iterator def = $outs.begin(); def != $outs.end(); def++) {
				if(!(*def)->global_reference) {
					Variable varx;
					varx.type = (*def)->type;
					varx.variable = false;
				
					(*(Vars.top()))[(*def)->id] = varx;
				}
			}
			
			vector<Arg*>::iterator def = $inps.begin();
			
			for(Function::arg_iterator arg = $func->arg_begin(); arg != $func->arg_end(); arg++, def++) {
				if(!(*def)->global_reference)
				{
					arg->setName((*def)->id);
				
					Variable varx;
					
					varx.value = arg;
					varx.type = (*def)->type;
					varx.variable = false;
				
					(*(Vars.top()))[(*def)->id] = varx;
				} else {
					arg->setName((*def)->id);
				
					Builder.CreateStore(arg, (*def)->value);
				}
			}
		}	
		body?)
		{	
			Value **values = new Value*[$outs.size()];
			
			int pos = 0;
		
			for(vector<Arg *>::iterator i = $outs.begin(); i != $outs.end(); i++) {
				Variable varx = (*(Vars.top()))[(*i)->id];				
				
				if(varx.variable)
					values[pos++] = Builder.CreateLoad(varx.value, $outs[0]->id);
				else
					values[pos++] = varx.value;
			}
			
			if($outs.size() > 0) {
				Builder.CreateAggregateRet(values, $outs.size());
			} else {
				Builder.CreateRetVoid();			
			}
			
			delete[] values;
		}
	;

body	:	^(BODY statement*)
	;
	
statement
	:	assignment
	|	function_call["call"]
	;

assignment returns [Value *value]
	:	^(':=' IDENT expr[(const char*)$IDENT.text->chars])
		{			
			const char *id = (const char*) $IDENT.text->chars;
		
			if((*(Vars.top())).find(id) == (*(Vars.top())).end()) {
				Variable varx;
				varx.variable = false;
				varx.type = $expr.type;
				(*(Vars.top()))[id] = varx;
			}
		
			Variable dest = (*(Vars.top()))[id];
			
			if(!dest.variable) {
				dest.value = $expr.value;
				
				(*(Vars.top()))[id] = dest;
			} else {
				$value = Builder.CreateStore($expr.value, dest.value);			
			}
		}
	;
	
concat [const char *id] returns [Value *value]	
	:	IDENT
	|	hash[$id]
	|	^('||' concat[$id] concat[$id])
	;
	
hash [const char *id] returns [Value *value, NumberT *type]
	:	^('SHA256' concat[$id])
		{
			$value = ConstantInt::get(getGlobalContext(), APInt(256, 0));
			$type = new NumberT(256);
		}
	;
	
function_call [const char *id] returns [Value *value, NumberT *type]
	:	^('Random' type_declaration)
		{
			$type = $type_declaration.type;			
		
			vector<Value*> args;
			vector<Type*> types;
			
			types.push_back($type->getType());
			types.push_back($type->getType());

			Function *function = Intrinsic::getDeclaration(TheModule, Intrinsic::random, types);
			
			if(GroupT *group_t = dynamic_cast<GroupT*>($type))
				args.push_back(ConstantInt::get(getGlobalContext(), group_t->getModulus()));
			else
				args.push_back(ConstantInt::get(getGlobalContext(), APInt($type->getBitWidth(), 0)));

			$value = Builder.CreateCall(function, args, id);			
		}
	|	hash[$id]
		{
			$value = $hash.value;
			$type = $hash.type;
		}
	|	^('CheckMembership' argument type_declaration)
		{
			Function *CalleeF = TheModule->getFunction("CheckMembership");

			vector<Value *> ArgsV;
			
			ArgsV.push_back($argument.value);
			
			$value = Builder.CreateCall(CalleeF, ArgsV, id);
			
			$type = new GroupT(APInt(1024,0));			
		}
	|	^('Verify' expr["verify"])
		{
			Function *CalleeF = TheModule->getFunction("Verify");
			
			vector<Value *> ArgsV;
			
			ArgsV.push_back($expr.value);
			
			$value = Builder.CreateCall(CalleeF, ArgsV, id);
			
			$type = new GroupT(APInt(1024,0));			
		}
		;
	
argument returns [Value *value]
	:	expr["arg"] { $value = $expr.value; }
	|	group
	;

group returns [NumberT *type]
	:	'Z' { $type = new NumberT(32); }
	|	^('Prime' expr["group"]) { $type = new NumberT(static_cast<ConstantInt*>($expr.value)->getValue().getLimitedValue(1024)); }
	|	^('Int' expr["group"]) { $type = new NumberT(static_cast<ConstantInt*>($expr.value)->getValue().getLimitedValue(1024)); }
	|	^('Zmod+' IDENT) { $type = new GroupT(static_cast<ConstantInt*>((*Vars.top())[(const char*)$IDENT.text->chars].value)->getValue()); }
	|	^('Zmod*' IDENT) { $type = new GroupT(static_cast<ConstantInt*>((*Vars.top())[(const char*)$IDENT.text->chars].value)->getValue()); }
	;

alias returns [NumberT *type]
	:	^('=' IDENT (group {$type=$group.type; } | interval {$type=$interval.type; }))
		{
			Types[(const char*)$IDENT.text->chars] = $type;
		}
	;
	
interval returns [NumberT *type]
	:^(INTERVAL expr["lhs"] expr["rhs"]) {$type = new GroupT(APInt(32, 0)); }
	;
	
expr[const char *id] returns [Value *value, NumberT *type]
	:	('-' val=expr[$id]) {$type = $val.type; $value = $type->createNeg($id, $val.value); }
	|	^('+' lhs=expr[$id] rhs=expr[$id]) {$type = *$lhs.type + *$rhs.type; $value = $type->createAdd($id, $lhs.value, $rhs.value); }
	|	^('-' lhs=expr[$id] rhs=expr[$id]) {$type = *$lhs.type - *$rhs.type; $value = $type->createSub($id, $lhs.value, $rhs.value); }
	|	^('*' lhs=expr[$id] rhs=expr[$id]) {$type = *$lhs.type * *$rhs.type; $value = $type->createMul($id, $lhs.value, $rhs.value); }
	|	^('^' lhs=expr[$id] rhs=expr[$id]) {$type = *$lhs.type ^ *$rhs.type; $value = $type->createExp($id, $lhs.value, $rhs.value); }
	|	^('==' lhs=expr[$id] rhs=expr[$id]) {$value = Builder.CreateICmpEQ($lhs.value, $rhs.value, $id);}
	|	^('!=' lhs=expr[$id] rhs=expr[$id]) {$value = Builder.CreateICmpNE($lhs.value, $rhs.value, $id);}
	|	function_call[$id] {$type = $function_call.type; $value = $function_call.value;}
	|	IDENT
		{
			Variable varx = (*(Vars.top()))[(const char *) $IDENT.text->chars];
			$type = varx.type;
			if(!varx.variable) {
				$value = varx.value;
			} else {
				$value = Builder.CreateLoad(varx.value, (const char*) $IDENT.text->chars);
			}
		}
	|	NUMBER {$type = new NumberT(1024); $value = ConstantInt::get(getGlobalContext(), APInt(1024, (const char*) $NUMBER.text->chars, 10));}
	;