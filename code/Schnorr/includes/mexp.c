
/*
 * Zero knowledge proof of knowledge C application framework
 *
 *  # Type utility module #
 *
 *
 * This modules is responsible for all type related functions and therefore
 * all math operations
 *
 * #######################################################################
 *
 * This file contains the algorithms to efficiently calculate multi-
 * exponentiation.
 *
 * #######################################################################
 *
 * This is part of the CACE (www.cace-project.eu) C-back-end of their zero
 * knowledge proof of knowledge compiler framework
 *
 * The algorithm was originally not developed for the this framework and has
 * therefore another structure concerning error, memory management and
 * commenting. It is aimed to change the structure accordingly in the future.
 *
 * License: GNU GPLv2 (http://www.gnu.org/licenses/gpl-2.0.html)
 *
 *  Created on: 02.2009
 *      Author: Endre Bangerter
 *
 *  History: 27.10.2009: Included into the CACE C Zk-PoK framework.
 *  		 Andreas Grünert
 */

#include <math.h>
#include "mexp.h"
#include <stdio.h>
#include <stdlib.h>

static int winSize;
static int numberOfFactors;
static unsigned preCompTableSize;

static mpz_t *preCompTable;
static mpz_t *bases;
static mpz_t *modulus;



void precomputation() {
    preCompTableSize = 1;
    preCompTableSize <<= ((numberOfFactors * winSize));
    preCompTable = (mpz_t *) malloc(sizeof(mpz_t) * preCompTableSize);

    int i, j, slot, h , factor, mask,tmpmask;
    unsigned mod_length;

    if (preCompTable == NULL) {
    	exit(255);
    }

    mod_length = 2 * mpz_sizeinbase(*modulus, 2);

    mpz_init_set_ui(preCompTable[0], 1);


    mask = 1;
    mask <<= winSize;
    mask--;
    for (i = 1; i < preCompTableSize; i++) {
      mpz_init2(preCompTable[i], mod_length);

      // Choose index of base element which is going to be multiplicand
      tmpmask = mask;
      factor = 0;
      for (j = 0; j < numberOfFactors; j++) {
	if ((i & tmpmask) != 0) {
	  factor = j;
	  break;
	}
	tmpmask <<= winSize;
      }
      h = 1;
      slot = i;
      if (factor != 0) {
	slot -= ((h <<= (factor * winSize)));
      } else slot -= 1;

      mpz_mul(preCompTable[i], bases[factor], preCompTable[slot]);
      mpz_mod(preCompTable[i], preCompTable[i], *modulus);
    }
}

void freeMemory() {
  int i;
  if (preCompTable != NULL)
  for (i = 0; i < preCompTableSize; i++) {
	  mpz_clear(preCompTable[i]);
  }
  free(preCompTable);
}

void mexp(int k, mpz_t b[k], mpz_t e[k], mpz_t *m, mpz_t *r) {

  // Main multiexp alg from paper
  int i, j, jnew, J, l, precompindex, offset,km;
  unsigned det;
  int msb_set;
  unsigned bitlen, maxbitlen = 0;

  // Setup and precomputation
  bases = b;
  modulus = m;
  numberOfFactors = k;

  // Compute window size
  for (i = 0; i < k; i++) {
    bitlen = mpz_sizeinbase(e[i], 2);
    if (bitlen > maxbitlen) maxbitlen = bitlen;
  }
  // TODO: Make this more elegant
  double lmb = log(maxbitlen)/log(2);
  winSize = (int) ceil((lmb - 2*(log(lmb)/log(2))) / k);
  if(winSize <= 0) winSize =1;


  // Build up precomputation table
  precomputation();

  km = k -1;
  mpz_set_ui(*r, 1);

  // Find maximal bit length of all exponents

  // Type conversion from unsigned to int. Only a problem for exponents
  // that have more binary digits than max(int). This is unlikely to occur in practice.
  // Maybe add check on arg to m_exp() to test for this possible problem?
  j = maxbitlen - 1;
  // A-while
  while (j >= 0) {
    // check if bit #maxbitlen is zero for all exponents
    // TODO: in the first iteraration this is the case by def of maxbitlen
    // -> can we skip first iteration to improve efficiency?
    msb_set = 0;

    for(i=0; i< k && msb_set == 0; i++) msb_set = mpz_tstbit(e[i], j);
    // B-if
    if (msb_set == 0) {
      mpz_powm_ui(*r, *r, 2, *m);
      j--;
    }
    // C-else
    else {
      if ( (j-winSize) > -1) jnew = j-winSize; else jnew = -1;
      J = jnew + 1;
      // D-while (is implemented as do-while)
      for(;;) {
	i = km;
	msb_set = 0;
	for(i=0; i< k && msb_set == 0; i++) msb_set = mpz_tstbit(e[i], j);
	if (msb_set == 0)
	  J++;
	else
	  break;
      }
      // E-for
      precompindex = 0;
      offset = winSize - (j - J) -1;
      for (i = km; i  >= 0; i--) {
	// Compute access index into pre-computation table
	precompindex <<= offset;
	for (l = j; l >= J; l--) {
	  precompindex <<= 1;
	  precompindex = precompindex | mpz_tstbit(e[i], l);
	}
      }
      // F-while
      det = 1;
      det <<= ((j-J)+1);
      mpz_powm_ui(*r, *r,det, *m);
      j = J-1;
      mpz_mul(*r, *r, preCompTable[precompindex]);
      mpz_mod(*r, *r, *m);

      // G-while
      if (j > jnew) {
	det =1;
	det <<= (j-jnew);
	mpz_powm_ui(*r, *r,det, *m);
	j = jnew + 1;
      }
    }
  }
  freeMemory();
}

