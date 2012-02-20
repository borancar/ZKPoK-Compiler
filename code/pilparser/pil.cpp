#include <cstdio>
#include "codegen.h"
#include "pilParser.h"
#include "pilLexer.h"
using namespace std;

int main(int argc, char *argv[])
{
  pANTLR3_UINT8 fName;

  pANTLR3_INPUT_STREAM input;

  ppilLexer lxr;

  pANTLR3_COMMON_TOKEN_STREAM tstream;

  ppilParser psr;

  pilParser_proof_return treeAST;

  pANTLR3_COMMON_TREE_NODE_STREAM nodes;

  pcodegen treePsr;

  if(argc < 2) {
    ANTLR3_FPRINTF(stderr, "Usage: pil <input-file>\n");

    return 1;
  }

  input = antlr3AsciiFileStreamNew((pANTLR3_UINT8)argv[1]);

  if(!input) {
    ANTLR3_FPRINTF(stderr, "Unable to open input file\n");

    return 1;
  }

  lxr = pilLexerNew(input);

  tstream = antlr3CommonTokenStreamSourceNew(ANTLR3_SIZE_HINT, TOKENSOURCE(lxr));

  psr = pilParserNew(tstream);

  treeAST = psr->proof(psr);

  if(psr->pParser->rec->getNumberOfSyntaxErrors(psr->pParser->rec) > 0) {
    ANTLR3_FPRINTF(stderr, "The parser returned %d error, tree walking aborted.\n", psr->pParser->rec->getNumberOfSyntaxErrors(psr->pParser->rec));
  } else {
    nodes = antlr3CommonTreeNodeStreamNewTree(treeAST.tree, ANTLR3_SIZE_HINT);
    
    treePsr = codegenNew(nodes);

    treePsr->proof(treePsr, "");

    nodes->free(nodes);
    treePsr->free(treePsr);
  }

  psr->free(psr);
  tstream->free(tstream);
  lxr->free(lxr);
  input->close(input);

  return 0;
}
