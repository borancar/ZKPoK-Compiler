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
	#include "llvm/PassManager.h"
	#include "llvm/Analysis/Verifier.h"
	#include "llvm/Analysis/Passes.h"
	#include "llvm/Target/TargetData.h"
	#include "llvm/Transforms/Scalar.h"
	#include "llvm/Support/IRBuilder.h"
	#include "llvm/Support/TargetSelect.h"
	#include <cstdio>
	#include <cstdlib>
	#include <vector>
	#include <map>
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
	static map<const char*, Variable, cmp_str> Vars;
	static map<const char*, NumberT*, cmp_str> Types;
				
	Value* operator_call(const char *name, const char *fname, Value *a, Value *b, Value *mod)
	{
		Function *function = TheModule->getFunction(fname);
	
		vector<Value*> args;
		
		args.push_back(a);
		args.push_back(b);
		args.push_back(mod);
		
		return Builder.CreateCall(function, args, name);
	}
	
	NumberT *operator+(NumberT *lhs, NumberT &rhs) { return lhs; }
	NumberT *operator-(NumberT *lhs, NumberT &rhs) { return lhs; }
	NumberT *operator*(NumberT *lhs, NumberT &rhs) { return lhs; }
	NumberT *operator^(NumberT *lhs, NumberT &rhs) { return lhs; }	

	void init_common() 
	{
		Function::Create(
			FunctionType::get(
				Type::getIntNTy(getGlobalContext(), 1024),
				vector<Type *>(0, Type::getIntNTy(getGlobalContext(), 1024)),
				false),
			Function::ExternalLinkage, "Random", Common);		
		Function::Create(
			FunctionType::get(
				Type::getInt1Ty(getGlobalContext()),
				vector<Type *>(0, Type::getIntNTy(getGlobalContext(), 1024)),
				false),
			Function::ExternalLinkage, "CheckMembership", Common);
		
		Function::Create(
			FunctionType::get(
				Type::getIntNTy(getGlobalContext(), 1024),
				vector<Type *>(3, Type::getIntNTy(getGlobalContext(), 1024)),
				false),
			Function::ExternalLinkage, "modadd1024", Common);
	
		Function::Create(
			FunctionType::get(
				Type::getIntNTy(getGlobalContext(), 1024),
				vector<Type *>(3, Type::getIntNTy(getGlobalContext(), 1024)),
				false),
			Function::ExternalLinkage, "modsub1024", Common);
	
		Function::Create(
			FunctionType::get(
				Type::getIntNTy(getGlobalContext(), 1024),
				vector<Type *>(3, Type::getIntNTy(getGlobalContext(), 1024)),
				false),
			Function::ExternalLinkage, "modmul1024", Common);
	
		Function::Create(
			FunctionType::get(
				Type::getIntNTy(getGlobalContext(), 1024),
				vector<Type *>(3, Type::getIntNTy(getGlobalContext(), 1024)),
				false),
			Function::ExternalLinkage, "modexp1024", Common);
		
		Function::Create(
			FunctionType::get(
				Type::getInt1Ty(getGlobalContext()),
				vector<Type *>(1, Type::getInt1Ty(getGlobalContext())),
				false),
			Function::ExternalLinkage, "Verify", Common);
	}
}

proof[const char *part]
	: { init_common(); }^(PROOF execution_order common (block[$part])*)
	;

execution_order
	:	^(ORDER (step)*)
	;
	
step	:	^(ID ID)
	;

common	:	^(BLOCK ^(ID { TheModule = Common; } param? global? function?))
	;

block[const char *part]
	:	^(BLOCK ^(ID {
			Linker *linker = new Linker((const char*)$ID.text->chars, (const char*)$ID.text->chars, getGlobalContext());
			linker->LinkInModule(Common);
			TheModule = linker->getModule();
			} param? global? function?)) { TheModule->dump(); }
	;
	
param	:	^(PARAM (param_declaration)*)
	;

global	:	^(VARIABLE (global_declaration)*)
	;

function:	^(FUNCTION (function_declaration)*)
	;

param_declaration returns [Value *value]
	:	^(par=ID type=type_declaration
			(NUMBER
			{$value = ConstantInt::get(getGlobalContext(), APInt($type.type->getBitWidth(), (const char*)$NUMBER.text->chars, 10));}
			| {$value = ConstantInt::get(getGlobalContext(), APInt($type.type->getBitWidth(), 0)); }))
	{
		Variable parx;
		parx.value = $value;
		parx.type = $type_declaration.type;
		parx.variable = false;

		Vars[(const char*) $par.text->chars] = parx;
	}
	;

global_declaration returns [Value *value]
	: ^(var=ID type_declaration
	{ 	$value = new GlobalVariable(*TheModule, $type_declaration.type->getType(), false, GlobalVariable::ExternalLinkage, 0, (const char *) $var.text->chars);
	} (NUMBER)?)
	{
		Variable varx;
		varx.value = $value;
		varx.type = $type_declaration.type;
		varx.variable = true;

		Vars[(const char*) $var.text->chars] = varx;
	}
	;

argument_declaration returns [Arg *arg]
	: (^(ID type_declaration))=> ^(var=ID type_declaration {
		arg = new Arg();
		$arg->id = (const char*)$ID.text->chars;
		$arg->type = $type_declaration.type;
		$arg->global_reference = false;
		})
	| (var=ID {
		arg = new Arg();
		$arg->id = (const char*)$ID.text->chars;
		$arg->type = Vars[$arg->id].type;
		$arg->value = Vars[$arg->id].value;
		$arg->global_reference = true;
		})
	;

type_declaration returns [NumberT *type]
	:	group {$type = $group.type; }
	|	interval {$type = $interval.type; }
	;

function_declaration returns [Function *func, vector<Arg *> outs, vector<Arg *> inps]
	:	^(ID ^(OUTPUT ('Void'|(out=argument_declaration { $outs.push_back($out.arg); })*)) 
			^(INP ('Void'|(inp=argument_declaration { $inps.push_back($inp.arg); })*)) 
		{
			vector<Type *> Inps;
			
			for(vector<Arg *>::iterator i = $inps.begin(); i != $inps.end(); i++) {
				Inps.push_back((*i)->type->getType());
			}

			FunctionType *FT;
			
			if($outs.size() == 0) {
				FT = FunctionType::get(Type::getVoidTy(getGlobalContext()), Inps, false);
			} else {
				FT = FunctionType::get($outs[0]->type->getType(), Inps, false);
			}
		
			$func = Function::Create(FT, Function::ExternalLinkage, (const char*) $ID.text->chars, TheModule);
			
			BasicBlock *entry_block = BasicBlock::Create(getGlobalContext(), "entry", $func);
			
			Builder.SetInsertPoint(entry_block);
			
			for(vector<Arg*>::iterator def = $outs.begin(); def != $outs.end(); def++) {
				if(!(*def)->global_reference) {
					Variable varx;
					varx.type = (*def)->type;
					varx.variable = false;
				
					Vars[(*def)->id] = varx;
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
				
					Vars[(*def)->id] = varx;
				} else {
					arg->setName((*def)->id);
				
					Builder.CreateStore(arg, (*def)->value);
				}
			}			
		}	
		body?)
		{	
			if($outs.size() > 0) {
				Variable varx = Vars[$outs[0]->id];
			
				if(varx.variable) {
					Builder.CreateRet(Builder.CreateLoad(varx.value, $outs[0]->id));
				} else {
					Builder.CreateRet(varx.value);
				}
			} else {
				Builder.CreateRetVoid();
			}
		}
	;

body	:	^(BODY statement*)
	;
	
statement
	:	assignment
	|	function_call
	;

assignment returns [Value *value]
	:	^(':=' ID expr[(const char*)$ID.text->chars])
		{
			const char *id = (const char*) $ID.text->chars;
		
			Variable dest = Vars[id];
			
			if(!dest.variable) {
				dest.value = $expr.value;
				
				Vars[id] = dest;
			} else {
				$value = Builder.CreateStore(Builder.CreateIntCast($expr.value, dest.type->getType(), false), dest.value);			
			}
		}
	;
	
function_call returns [Value *value, NumberT *type]
	:	^(ID argument*)
		{
			Function *CalleeF = TheModule->getFunction((const char*) $ID.text->chars);
			vector<Value*> ArgsV;
			
			unsigned int idx = 0;
			for(Function::arg_iterator AI = CalleeF->arg_begin(); AI != CalleeF->arg_end(); AI++, idx++) {
				ArgsV.push_back($argument.value);
			}
			
			$value = Builder.CreateCall(CalleeF, ArgsV, "calltmp");
			
			$type = new GroupT(APInt(1024,0));
		}
		;
	
argument returns [Value *value]
	:	expr["arg"] { $value = $expr.value; }
	|	group
	;

group returns [NumberT *type]
	:	^(GROUP	{$type = new GroupT(APInt(32, 0));} (ID    
			{
				Variable var = Vars[(const char*)$ID.text->chars];
				
				$type = new GroupT(static_cast<ConstantInt*>(var.value)->getValue());
			}
			| NUMBER
			{
				unsigned int bits = atoi((const char*)$NUMBER.text->chars);
									
				$type = new GroupT(APInt(bits, 0));
				//$type = new NumberT(bits);
			}
			)? )
	;

alias returns [NumberT *type]
	:	^('=' ID (group {$type=$group.type; } | interval {$type=$interval.type; }))
		{
			Types[(const char*)$ID.text->chars] = $type;
		}
	;
	
interval returns [NumberT *type]
	:^(INTERVAL expr["lhs"] expr["rhs"]) {$type = new GroupT(APInt(32, 0)); }
	;
	
expr[const char *id] returns [Value *value, NumberT *type]
	:	^('+' lhs=expr[$id] rhs=expr[$id]) {$type = $lhs.type + *$rhs.type; $value = operator_call($id, "modadd1024", $lhs.value, $rhs.value, 
			Builder.CreateIntCast(dynamic_cast<GroupT*>($type)->getModulusConstant(), IntegerType::get(getGlobalContext(), 1024), false)); }
	|	^('-' lhs=expr[$id] rhs=expr[$id]) {$type = $lhs.type - *$rhs.type; $value = operator_call($id, "modsub1024", $lhs.value, $rhs.value,
			Builder.CreateIntCast(dynamic_cast<GroupT*>($type)->getModulusConstant(), IntegerType::get(getGlobalContext(), 1024), false)); }
	|	^('*' lhs=expr[$id] rhs=expr[$id]) {$type = $lhs.type * *$rhs.type; $value = operator_call($id, "modmul1024", $lhs.value, $rhs.value,
			Builder.CreateIntCast(dynamic_cast<GroupT*>($type)->getModulusConstant(), IntegerType::get(getGlobalContext(), 1024), false)); }
	|	^('^' lhs=expr[$id] rhs=expr[$id]) {$type = $lhs.type ^ *$rhs.type; $value = operator_call($id, "modexp1024", $lhs.value, $rhs.value,
			Builder.CreateIntCast(dynamic_cast<GroupT*>($type)->getModulusConstant(), IntegerType::get(getGlobalContext(), 1024), false)); }
	|	^('==' lhs=expr[$id] rhs=expr[$id]) {$value = Builder.CreateICmpEQ($lhs.value, $rhs.value, $id);}
	|	^('!=' lhs=expr[$id] rhs=expr[$id]) {$value = Builder.CreateICmpNE($lhs.value, $rhs.value, $id);}
	|	function_call {$type = $function_call.type; $value = $function_call.value;}
	|	ID
		{
			Variable varx = Vars[(const char *) $ID.text->chars];
			$type = varx.type;
			if(!varx.variable) {
				$value = Builder.CreateIntCast(varx.value, IntegerType::get(getGlobalContext(), 1024), false, (const char*)$ID.text->chars);
			} else {
				$value = Builder.CreateIntCast(Builder.CreateLoad(varx.value, (const char*) $ID.text->chars), IntegerType::get(getGlobalContext(), 1024), false, (const char*)$ID.text->chars);
			}
		}
	|	NUMBER {$value = ConstantInt::get(getGlobalContext(), APInt(1024, (const char*) $NUMBER.text->chars, 10));}
	;