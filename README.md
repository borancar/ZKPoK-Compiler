Implementation and Evaluation of Zero-Knowledge Proofs of Knowledge
===================================================================

This is the source code for my Master Thesis titled Implementation and
Evaluation of Zero Knowledge Proofs of Knowledge with the accompanying
source code for the Zero Knowledge Proofs of Knowledge compiler.

Outline of work:
----------------

This thesis aimed to improve on the CACE ZKC by allowing HW-SW
co-design and HW implementations of the protocols. This is achieved by
using LLVM as an intermediate format. Its SSA form allowed an easier
DDG extraction from the code and thanks to its modular design it was
possible to independenty develop front-ends and back-ends at the same
time provided that the specification of the intermediate language was
kept.

Prerequisites:
--------------

ANTLR 3.4 is needed. This can be installed from you package manager.

LLVM 3.1 is needed, configured with --enable-shared and built with
REQUIRES_RTTI=1. This is not usually provided by your package manager
so the recommended way is to build it. The following steps outline the
build procedure:

 1. download the LLVM package
   (e.g. wget http://llvm.org/releases/3.1/llvm-3.1.src.tar.gz)
 2. unpack the package (e.g. tar xfz llvm-3.1.src.tar.gz)
 3. cd into tools directory (e.g. cd llvm-3.1.src/tools)
 4. download the clang package 
    (e.g. wget http://llvm.org/releases/3.1/clang-3.1.src.tar.gz)
 5. unpack clang to llvm-3.1.src/tools, (e.g. tar xfz clang-3.1.src.tar.gz)
 6. mv clang-3.1.src clang
 7. cd ..
 8. ./configure --enable-shared --prefix=YOUR_INSTALLATION_PREFIX_HERE
 9. make REQUIRES_RTTI=1
10. sudo make install

Compilation:
------------

Issue make in each subdirectory of interest. This will build the
module and place the executable in the same directory.

Directory structure:
--------------------

* ImplementationZKPK
  * article - the article that was submitted to the ESORICS 2012
              conference
  
  * code - code for the Zero Knowledge Proofs of Knowledge compiler
  
  * final - the final presentation of the Master's Thesis
  
  * interim - the interim presentation of the Master's Thesis
  
  * paper - paper as required by KULeuven
  
  * planning - a document detailing the planning the work
  
  * reading - repository of articles
  
  * text - text of the Master's Thesis
