grammar pil;

options {
	output=AST;
	//ASTLabelType=CommonTree;
	ASTLabelType=pANTLR3_BASE_TREE;
	language=C;
}

tokens {
	INTERVAL; PARAM; PROOF; ORDER; BLOCK; INP; OUTPUT; BODY; VARIABLE; FUNCTION;
}

@includes {
	#include <map>
	using namespace std;
}

@postinclude {
	struct cmp_str
	{
		bool operator()(const char *a, const char *b)
		{
			return strcmp(a, b) < 0;
		}
	};

	map<const char *, pANTLR3_BASE_TREE, cmp_str> aliases;
}

proof	:	execution_order (block)* -> ^(PROOF ^(ORDER execution_order) ^(BLOCK block)*)
	;

execution_order 
	:	'ExecutionOrder' ':=' '(' step (',' step)* ')' ';' -> (step)*
	;

step	:	ID'.'ID -> ^(ID ID)
	;

block 	:	ID '(' (params+=variable_declaration)? (';' params+=variable_declaration)* ')'
		'{' (vars+=variable_declaration ';')* (function_declaration)* '}'
			-> ^(ID ^(PARAM $params*)? ^(VARIABLE $vars*)? ^(FUNCTION function_declaration*)?)
	;

function_declaration
	:	'Def' '(' function_params ')' ':' ID '(' function_params ')' '{' body '}' 
			-> ^(ID ^(OUTPUT function_params) ^(INP function_params) ^(BODY body)?)
	;
	
function_params
	:	'Void'
	|	argument_declaration (';'! argument_declaration)*
	;
	
argument_declaration
	:	type!? variable_init[$type.tree] (','! variable_init[$type.tree])*
	;

body
	:	(statement)*
	;
	
statement
	:	ID ':=' expr ';' -> ^(':=' ID expr)
	|	function_call ';'!
	|	ifknown
	;
	
ifknown	:	'IfKnown' '(' topExpr ')' '{'
		body
		'}' ('Else' '{'
		body
		'}')?
	;	

topExpr	:	expr (('=='^|'!='^) expr)?
	;

expr	:	('+'^|'-')? priExpr (('+'^|'-'^) priExpr)*
	;
	
priExpr	:	(('('! expr ')'!) | terminal) (('^'^|'*'^) priExpr)?
	;
	
terminal
	:	NUMBER
	|	function_call
	|	ID
	;
	
function_call
	:	'Random'^ '('! type ')'!
	|	'Verify'^ '('! topExpr ')'!
	|	'CheckMembership'^ '('! ID ','! type ')'!
	;

variable_declaration
	:	type! variable_init[$type.tree] (','! variable_init[$type.tree])*
	;
	
variable_init[pANTLR3_BASE_TREE t]
	:	ID ('=' NUMBER)? -> ^(ID {$t} NUMBER?)
	;

type	: 	alias
	|	group
	|	interval
	;
	
alias	:	(ID '=' )=> ID! '='! (group { aliases[(const char*)$ID.text->chars] = $group.tree; } | interval { aliases[(const char*)$ID.text->chars] = $interval.tree; })
	|	ID -> { aliases[(const char*)$ID.text->chars] }
	;

group	:	('Zmod+'|'Zmod*')^ ('('! expr ')'!) 
	|	'Prime'^ ('('! expr ')'!)
	|	'Int'^ ('('! expr ')'!)
	|	'Z'
	;

interval:	'[' from=expr ',' to=expr ']' -> ^(INTERVAL $from $to)
	;

ID  :	('a'..'z'|'A'..'Z'|'_') ('a'..'z'|'A'..'Z'|'0'..'9'|'_')*
    ;

NUMBER	:	('0'..'9'|'a'..'z'|'A'..'Z')+;

COMMENT
    :   '//' ~('\n'|'\r')* '\r'? '\n' { $channel=HIDDEN; }
    |   '/*' ( options { greedy=false; } : . )* '*/' { $channel=HIDDEN; }
    ;
    
NEWLINE	: '\r'? '\n' { $channel=HIDDEN; } ;

WS  :   (' '|'\t')+ { $channel=HIDDEN; } ;