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
 * This file contains all operations needed for calculations in GF(2).
 *
 * #######################################################################
 *
 * This is part of the CACE (www.cace-project.eu) C-back-end of their zero
 * knowledge proof of knowledge compiler framework
 *
 * Notice: Ideas and part of the code are borrowed from the GNU GPL
 * project ssss (http://point-at-infinity.org/ssss/).
 *
 * License: GNU GPLv2 (http://www.gnu.org/licenses/gpl-2.0.html)
 *
 *  Created on: 03.03.2009
 *      Author: Andreas Gruenert
 */

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <time.h>

#include <gmp.h>

#include "types.h"

#define mpz_lshift(A, B, l) mpz_mul_2exp(A, B, l)
#define mpz_sizeinbits(A) (mpz_cmp_ui(A, 0) ? mpz_sizeinbase(A, 2) : 0)


unsigned int degree;
mpz_t poly;

void field_add(mpz_t z, const mpz_t x, const mpz_t y) {
  mpz_xor(z, x, y);
}

int max(int l, int r) {
    return l > r ? l:r;
}

int min(int l, int r) {
    return l < r ? l:r;
}

void field_mult(mpz_t z, const mpz_t x, const mpz_t y) {
	mpz_t b;
	unsigned int i,j;
	mpz_init_set_ui(b,0);

	for (i = 0; i < (2*degree)-1; i++) {
		for (j = max(0,i-degree-1); j < min(degree,i+1); j++) {
			if ((mpz_tstbit(b,i) ^ (mpz_tstbit(x,j) && mpz_tstbit(y,i-j))) == 1) {
				mpz_setbit(b,i);
			} else {
				mpz_clrbit(b,i);
			}
		}
	}
	for (i = (2*degree), j = degree; i >= degree; i--,j--) {
		if ((mpz_tstbit(b,i) ^ mpz_tstbit(b,j)) == 1) {
			mpz_setbit(b,j);
		} else {
			mpz_clrbit(b,j);
		}
		if ((mpz_tstbit(b,i) ^ mpz_tstbit(b,j+1)) == 1) {
		   mpz_setbit(b,j+1);
		} else {
			mpz_clrbit(b,j+1);
		}
		mpz_clrbit(b,i);
	}

	mpz_set(z,b);
	mpz_clear(b);
}

void field_mult_ssss(mpz_t z, const mpz_t x, const mpz_t y) {
	mpz_t b;
	unsigned int i;
	assert(z != y);
	mpz_init_set(b, x);

	if (mpz_tstbit(y, 0)) {
		mpz_set(z, b);
	} else {
		mpz_set_ui(z, 0);
	}
	for(i = 1; i < degree; i++) {
		mpz_lshift(b, b, 1);
		if (mpz_tstbit(b, degree)) {
			mpz_xor(b, b, poly);
		}
		if (mpz_tstbit(y, i)) {
			mpz_xor(z, z, b);
		}
	}
	mpz_clear(b);
}


void field_init(int deg) {
  mpz_init_set_ui(poly, 0);
  mpz_setbit(poly, deg);
  mpz_setbit(poly, 1);
  mpz_setbit(poly, 0);
  degree = deg;
}

void field_deinit(void) {
  mpz_clear(poly);
}

void field_invert(mpz_t z, const mpz_t x) {
  mpz_t u, v, g, h;
  int i;
  assert(mpz_cmp_ui(x, 0));
  mpz_init_set(u, x);
  mpz_init_set(v, poly);
  mpz_init_set_ui(g, 0);
  mpz_set_ui(z, 1);
  mpz_init(h);
  while (mpz_cmp_ui(u, 1) == 1) { // && j > 0) {
//	--j;
    i = mpz_sizeinbits(u) - mpz_sizeinbits(v);
    //gmp_printf("\t\t u=%Zd, %d\n",u,mpz_cmp_ui(u, 1));
    if (i < 0) {
      mpz_swap(u, v);
      mpz_swap(z, g);
      i = -i;
    }
    mpz_lshift(h, v, i);
    mpz_xor(u, u, h);
    mpz_lshift(h, g, i);
    mpz_xor(z, z, h);
  }
  //gmp_printf("\t\t end: u=%Zd, %d\n",u,mpz_cmp_ui(u, 1));
  mpz_clear(u); mpz_clear(v); mpz_clear(g); mpz_clear(h);
}


void lagrange(mpz_t z, int size, struct point p[size], mpz_t request, mpz_t secret, int degree) {
    int i,j;
    mpz_t b1,b2,b3;
    mpz_init(b1);
    mpz_init(b2);
    mpz_init(b3);
    mpz_set_ui(z,0);
    field_init(degree);
   for (i = 0; i < size; ++i) {
      mpz_set_ui(b3,1);
      for (j = 0; j < size; ++j) {
         // The i-th term has to be skipped
         if (j != i) {
        	field_add(b1,p[i].x,p[j].x);
            field_invert(b1,b1);
            field_add(b2,request,p[j].x);
            field_mult(b2,b2,b1);
            field_mult(b3,b3,b2);
         }
      }
      field_mult(b3,b3,p[i].y);
      field_add(z,z,b3);
   }
   //+secret value
   field_add(z,z,secret);

   field_deinit();
   //gmp_printf("\tl: result = %Zd\n",z);
}
