/** \file
 *  This C header file was generated by $ANTLR version 3.2 Fedora release 15 (Rawhide) Tue Feb  8 02:02:23 UTC 2011
 *
 *     -  From the grammar source file : pil.g
 *     -                            On : 2012-02-20 02:54:37
 *     -                for the parser : pilParserParser *
 * Editing it, at least manually, is not wise. 
 *
 * C language generator and runtime by Jim Idle, jimi|hereisanat|idle|dotgoeshere|ws.
 *
 *
 * The parser pilParser has the callable functions (rules) shown below,
 * which will invoke the code for the associated rule in the source grammar
 * assuming that the input stream is pointing to a token/text stream that could begin
 * this rule.
 * 
 * For instance if you call the first (topmost) rule in a parser grammar, you will
 * get the results of a full parse, but calling a rule half way through the grammar will
 * allow you to pass part of a full token stream to the parser, such as for syntax checking
 * in editors and so on.
 *
 * The parser entry points are called indirectly (by function pointer to function) via
 * a parser context typedef ppilParser, which is returned from a call to pilParserNew().
 *
 * The methods in ppilParser are  as follows:
 *
 *  - pilParser_proof_return      ppilParser->proof(ppilParser)
 *  - pilParser_execution_order_return      ppilParser->execution_order(ppilParser)
 *  - pilParser_step_return      ppilParser->step(ppilParser)
 *  - pilParser_block_return      ppilParser->block(ppilParser)
 *  - pilParser_function_declaration_return      ppilParser->function_declaration(ppilParser)
 *  - pilParser_function_params_return      ppilParser->function_params(ppilParser)
 *  - pilParser_argument_declaration_return      ppilParser->argument_declaration(ppilParser)
 *  - pilParser_body_return      ppilParser->body(ppilParser)
 *  - pilParser_statement_return      ppilParser->statement(ppilParser)
 *  - pilParser_ifknown_return      ppilParser->ifknown(ppilParser)
 *  - pilParser_topExpr_return      ppilParser->topExpr(ppilParser)
 *  - pilParser_expr_return      ppilParser->expr(ppilParser)
 *  - pilParser_priExpr_return      ppilParser->priExpr(ppilParser)
 *  - pilParser_terminal_return      ppilParser->terminal(ppilParser)
 *  - pilParser_function_call_return      ppilParser->function_call(ppilParser)
 *  - pilParser_function_arguments_return      ppilParser->function_arguments(ppilParser)
 *  - pilParser_variable_declaration_return      ppilParser->variable_declaration(ppilParser)
 *  - pilParser_variable_init_return      ppilParser->variable_init(ppilParser)
 *  - pilParser_type_return      ppilParser->type(ppilParser)
 *  - pilParser_alias_return      ppilParser->alias(ppilParser)
 *  - pilParser_group_return      ppilParser->group(ppilParser)
 *  - pilParser_interval_return      ppilParser->interval(ppilParser)
 *  - pilParser_bits_return      ppilParser->bits(ppilParser)
 *
 * The return type for any particular rule is of course determined by the source
 * grammar file.
 */
// [The "BSD licence"]
// Copyright (c) 2005-2009 Jim Idle, Temporal Wave LLC
// http://www.temporal-wave.com
// http://www.linkedin.com/in/jimidle
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. The name of the author may not be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
// NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef	_pilParser_H
#define _pilParser_H
/* =============================================================================
 * Standard antlr3 C runtime definitions
 */
#include    <antlr3.h>

/* End of standard antlr 3 runtime definitions
 * =============================================================================
 */

	#include <map>
	using namespace std;

 
#ifdef __cplusplus
extern "C" {
#endif

// Forward declare the context typedef so that we can use it before it is
// properly defined. Delegators and delegates (from import statements) are
// interdependent and their context structures contain pointers to each other
// C only allows such things to be declared if you pre-declare the typedef.
//
typedef struct pilParser_Ctx_struct pilParser, * ppilParser;



#ifdef	ANTLR3_WINDOWS
// Disable: Unreferenced parameter,							- Rules with parameters that are not used
//          constant conditional,							- ANTLR realizes that a prediction is always true (synpred usually)
//          initialized but unused variable					- tree rewrite variables declared but not needed
//          Unreferenced local variable						- lexer rule declares but does not always use _type
//          potentially unitialized variable used			- retval always returned from a rule 
//			unreferenced local function has been removed	- susually getTokenNames or freeScope, they can go without warnigns
//
// These are only really displayed at warning level /W4 but that is the code ideal I am aiming at
// and the codegen must generate some of these warnings by necessity, apart from 4100, which is
// usually generated when a parser rule is given a parameter that it does not use. Mostly though
// this is a matter of orthogonality hence I disable that one.
//
#pragma warning( disable : 4100 )
#pragma warning( disable : 4101 )
#pragma warning( disable : 4127 )
#pragma warning( disable : 4189 )
#pragma warning( disable : 4505 )
#pragma warning( disable : 4701 )
#endif

/* ========================
 * BACKTRACKING IS ENABLED
 * ========================
 */
typedef struct pilParser_proof_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;
   
}
    pilParser_proof_return;

typedef struct pilParser_execution_order_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;
   
}
    pilParser_execution_order_return;

typedef struct pilParser_step_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;
   
}
    pilParser_step_return;

typedef struct pilParser_block_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;
   
}
    pilParser_block_return;

typedef struct pilParser_function_declaration_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;
   
}
    pilParser_function_declaration_return;

typedef struct pilParser_function_params_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;
   
}
    pilParser_function_params_return;

typedef struct pilParser_argument_declaration_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;
   
}
    pilParser_argument_declaration_return;

typedef struct pilParser_body_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;
   
}
    pilParser_body_return;

typedef struct pilParser_statement_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;
   
}
    pilParser_statement_return;

typedef struct pilParser_ifknown_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;
   
}
    pilParser_ifknown_return;

typedef struct pilParser_topExpr_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;
   
}
    pilParser_topExpr_return;

typedef struct pilParser_expr_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;
   
}
    pilParser_expr_return;

typedef struct pilParser_priExpr_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;
   
}
    pilParser_priExpr_return;

typedef struct pilParser_terminal_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;
   
}
    pilParser_terminal_return;

typedef struct pilParser_function_call_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;
   
}
    pilParser_function_call_return;

typedef struct pilParser_function_arguments_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;
   
}
    pilParser_function_arguments_return;

typedef struct pilParser_variable_declaration_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;
   
}
    pilParser_variable_declaration_return;

typedef struct pilParser_variable_init_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;
   
}
    pilParser_variable_init_return;

typedef struct pilParser_type_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;
   
}
    pilParser_type_return;

typedef struct pilParser_alias_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;
   
}
    pilParser_alias_return;

typedef struct pilParser_group_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;
   
}
    pilParser_group_return;

typedef struct pilParser_interval_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;
   
}
    pilParser_interval_return;

typedef struct pilParser_bits_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;
   
}
    pilParser_bits_return;



/** Context tracking structure for pilParser
 */
struct pilParser_Ctx_struct
{
    /** Built in ANTLR3 context tracker contains all the generic elements
     *  required for context tracking.
     */
    pANTLR3_PARSER   pParser;


     pilParser_proof_return (*proof)	(struct pilParser_Ctx_struct * ctx);
     pilParser_execution_order_return (*execution_order)	(struct pilParser_Ctx_struct * ctx);
     pilParser_step_return (*step)	(struct pilParser_Ctx_struct * ctx);
     pilParser_block_return (*block)	(struct pilParser_Ctx_struct * ctx);
     pilParser_function_declaration_return (*function_declaration)	(struct pilParser_Ctx_struct * ctx);
     pilParser_function_params_return (*function_params)	(struct pilParser_Ctx_struct * ctx);
     pilParser_argument_declaration_return (*argument_declaration)	(struct pilParser_Ctx_struct * ctx);
     pilParser_body_return (*body)	(struct pilParser_Ctx_struct * ctx);
     pilParser_statement_return (*statement)	(struct pilParser_Ctx_struct * ctx);
     pilParser_ifknown_return (*ifknown)	(struct pilParser_Ctx_struct * ctx);
     pilParser_topExpr_return (*topExpr)	(struct pilParser_Ctx_struct * ctx);
     pilParser_expr_return (*expr)	(struct pilParser_Ctx_struct * ctx);
     pilParser_priExpr_return (*priExpr)	(struct pilParser_Ctx_struct * ctx);
     pilParser_terminal_return (*terminal)	(struct pilParser_Ctx_struct * ctx);
     pilParser_function_call_return (*function_call)	(struct pilParser_Ctx_struct * ctx);
     pilParser_function_arguments_return (*function_arguments)	(struct pilParser_Ctx_struct * ctx);
     pilParser_variable_declaration_return (*variable_declaration)	(struct pilParser_Ctx_struct * ctx);
     pilParser_variable_init_return (*variable_init)	(struct pilParser_Ctx_struct * ctx, pANTLR3_BASE_TREE t);
     pilParser_type_return (*type)	(struct pilParser_Ctx_struct * ctx);
     pilParser_alias_return (*alias)	(struct pilParser_Ctx_struct * ctx);
     pilParser_group_return (*group)	(struct pilParser_Ctx_struct * ctx);
     pilParser_interval_return (*interval)	(struct pilParser_Ctx_struct * ctx);
     pilParser_bits_return (*bits)	(struct pilParser_Ctx_struct * ctx);
     ANTLR3_BOOLEAN (*synpred1_pil)	(struct pilParser_Ctx_struct * ctx);
     ANTLR3_BOOLEAN (*synpred2_pil)	(struct pilParser_Ctx_struct * ctx);
    // Delegated rules
    const char * (*getGrammarFileName)();
    void	    (*free)   (struct pilParser_Ctx_struct * ctx);
    /* @headerFile.members() */
    pANTLR3_BASE_TREE_ADAPTOR	adaptor;
    pANTLR3_VECTOR_FACTORY		vectors;
    /* End @headerFile.members() */
};

// Function protoypes for the constructor functions that external translation units
// such as delegators and delegates may wish to call.
//
ANTLR3_API ppilParser pilParserNew         (pANTLR3_COMMON_TOKEN_STREAM instream);
ANTLR3_API ppilParser pilParserNewSSD      (pANTLR3_COMMON_TOKEN_STREAM instream, pANTLR3_RECOGNIZER_SHARED_STATE state);

/** Symbolic definitions of all the tokens that the parser will work with.
 * \{
 *
 * Antlr will define EOF, but we can't use that as it it is too common in
 * in C header files and that would be confusing. There is no way to filter this out at the moment
 * so we just undef it here for now. That isn't the value we get back from C recognizers
 * anyway. We are looking for ANTLR3_TOKEN_EOF.
 */
#ifdef	EOF
#undef	EOF
#endif
#ifdef	Tokens
#undef	Tokens
#endif 
#define FUNCTION      13
#define T__42      42
#define T__40      40
#define T__41      41
#define T__29      29
#define T__28      28
#define T__27      27
#define T__26      26
#define T__25      25
#define ORDER      7
#define T__24      24
#define T__23      23
#define T__22      22
#define T__21      21
#define T__20      20
#define NUMBER      15
#define INP      9
#define PARAM      5
#define ID      14
#define PROOF      6
#define EOF      -1
#define INTERVAL      4
#define T__30      30
#define GROUP      16
#define T__31      31
#define T__32      32
#define WS      19
#define T__33      33
#define T__34      34
#define NEWLINE      18
#define VARIABLE      12
#define T__35      35
#define T__36      36
#define T__37      37
#define T__38      38
#define T__39      39
#define BLOCK      8
#define BODY      11
#define COMMENT      17
#define OUTPUT      10
#ifdef	EOF
#undef	EOF
#define	EOF	ANTLR3_TOKEN_EOF
#endif

#ifndef TOKENSOURCE
#define TOKENSOURCE(lxr) lxr->pLexer->rec->state->tokSource
#endif

/* End of token definitions for pilParser
 * =============================================================================
 */
/** \} */

#ifdef __cplusplus
}
#endif

#endif

/* END - Note:Keep extra line feed to satisfy UNIX systems */
