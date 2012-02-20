
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
 *      Author: Endre Bangerter, Peter Linder
 *
 *  History: 27.10.2009: Included into the CACE C Zk-PoK framework.
 *  		 Andreas Grünert
 */


#include <stdlib.h>
#include <stdio.h>
#include <gmp.h>

void m_exp(mpz_t *bases, mpz_t *exponents, mpz_t *modulus, mpz_t *product, int numbers);
