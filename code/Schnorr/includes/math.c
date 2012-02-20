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
 * This file contains all math operations (except for operations in GF(2))
 * and access methods math numbers, dynamically typed (see documentation,
 * to understand how this framework is designed).
 *
 * #######################################################################
 *
 * This is part of the CACE (www.cace-project.eu) C-back-end of their zero
 * knowledge proof of knowledge compiler framework
 *
 * License: GNU GPLv2 (http://www.gnu.org/licenses/gpl-2.0.html)
 *
 *  Created on: 03.03.2009
 *      Author: Andreas Gruenert
 */
#include <stdlib.h>
#include <search.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <gmp.h>
//#include <tomcrypt.h>

#include "types.h"

/**
 * Every operation function has the same function signature. To not rewrite
 * the parameters to the function every time, a macro is used:
 */
#define PARAMS struct cace_int *result, struct cace_int *var,  \
	struct cace_int *params
#define ULONG (unsigned int)

#ifndef VERBOSE
#define VERBOSE(p,q) if (p < get_verbose()) { q }
#define MSG_ERROR(p)  if (ERR < get_verbose()) { p }
#define MSG_WARNING(p)  if (WRN < get_verbose()) { p }
#define MSG_INFO(p)  if (INF < get_verbose()) { p }
#define MSG_DEBUG(q,p)  if (q < get_verbose()) { p }

#endif
/**
 * To implement complicated operation constructs without the need of
 * wrapping them into each other, losening the ability for proper exception
 * handling, a stack is needed. The size is currently limited.
 */
#define MAX_STACK_SIZE 20
struct cace_int *_stack[MAX_STACK_SIZE];
int _stacksize = -1;
int _stackmax = 0;

/*********************************************************************
 *  The following functions are used for dynamic type checking
 *  This type checking is not yet implemented for every operation
 *********************************************************************/

/**
 * Return -1 (error) if the given element is not initialized.
 */
/*
int check_op_params_not_null(struct cace_int *element) {
	if (element != NULL && element->value1 != NULL && element->initialized == 1) {
		return 0;
	}
	cace_set_error(ERROR_ELEMENT_NOT_INITIALIZED, "Parameter verification failed:\n Parameter not initialized.");
	return -1;
}
*/
/**
 * Return -1 (error) if the given element is not initialized.
 */
/*
int check_op_params_group_init(struct cace_type *type, int param_count) {
	struct cace_type_param *tmp;
	int count = 0;

	if (type != NULL) {
		tmp = type->params;
		while (tmp != NULL && tmp->param != NULL && tmp->param->initialized == 1) {
			count++;
			tmp = tmp->next;
		}
		if (count != param_count) {
			cace_set_error(ERROR_GROUP_WRONGLY_INITIALIZED,
					"Group initialization verification failed:\n Wrong amount of parameters.");
			return -1;
		}
		return 0;
	}
	cace_set_error(ERROR_GROUP_NOT_INITIALIZED,
			"Group initialization verification failed:\n Group not initialized.");
	return -1;
}
*/
/**
 * Return -1 (error) if the given element is _not_ of the requested group
 */
int check_cace_element_type(struct cace_int *element, int type) {
	if (element != NULL && element->initialized == 0) {
		if (element->group->type == type) {
			return 0;
		}
		MSG_ERROR(fprintf(stderr, "math: check_cace_element_type(): dynamic type checking failed: %s != %s \n", get_type(type), get_type(element->group->type));)
		cace_set_error(ERROR_WRONG_TYPE,
					"Type verification failed: wrong type:\n param due count != 0.");
		return -1;
	}
	MSG_ERROR(fprintf(stderr, "math: check_cace_element_type(): dynamic type checking failed: element is null pointer\n");)
	cace_set_error(ERROR_NULL_POINTER, "Null pointer given as parameter to check_cace_element_type()");
	return -1;
}

int check_cace_type_init(struct cace_type *type) {
	if (type != NULL) {
		if (type->param_due_count == 0) {
			return 0;
		}
		MSG_ERROR(fprintf(stderr, "math: check_cace_type_init(): dynamic type checking failed: param due count is %d not 0\n", type->param_due_count);)
		cace_set_error(ERROR_GROUP_NOT_INITIALIZED,
					"Type initialization verification failed:\n param due count != 0.");
		return -1;
	}
	MSG_ERROR(fprintf(stderr, "math: check_cace_type_init(): dynamic type checking failed: type is null pointer\n");)
	cace_set_error(ERROR_NULL_POINTER, "Null pointer given as parameter to check_cace_type_init()");
	return -1;
}

int check_cace_int_init(struct cace_int *element) {
	if (element != NULL) {
		if (element->initialized != 0) {
			return check_cace_type_init(element->group);
		}
		MSG_ERROR(fprintf(stderr, "math: check_cace_int_init(): dynamic type checking failed: element not initialized\n");)
		cace_set_error(ERROR_VAR_NOT_INITIALIZED,
					"Variable initialization verification failed:\n Variable not initialized.");
		return -1;
	}
	MSG_ERROR(fprintf(stderr, "math: check_cace_type_init(): dynamic type checking failed: type is null pointer\n");)
	cace_set_error(ERROR_NULL_POINTER, "Null pointer given as parameter to check_cace_type_init()");
	return -1;
}

/*
 * Return -1 (error) if the given singly linked list of elements has not
 * requested length.
 */
/*
int check_op_params_list_length(struct cace_int *element, int length) {
	struct cace_int *tmp;
	int count = 0;

	tmp = element;
	while (tmp != NULL) {
		count++;
		tmp = tmp->next;
	}
	if (count != length) {
		return -1;
	}
	return 0;
}
*/

/*********************************************************************
 *  The following functions define the mathematical operations
 *********************************************************************/

/*
 * Writes the upper bound (inclusive) of an interval to result
 *
 * @type: interval
 * @result: Stores result
 * @var: An element of type interval
 * @params: not used
 */
int interv_max(PARAMS) {
	int res = 0;
	MSG_DEBUG(5,fprintf(stderr, "math:op: interv_max(): execute\n");)

	IFNOERROR(check_cace_int_init(result););
	IFNOERROR(check_cace_int_init(var););

	IFNOERRORX(mpz_set(*result->value1,*var->group->params->next->param->value1););
	return res;
}

/*
 * Writes the lower bound (inclusive) of an interval to result
 *
 * @type: interval
 * @result: Stores result
 * @var: An element of type interval
 * @params: not used
 */
int interv_min(PARAMS) {
	int res = 0;
	MSG_DEBUG(5,fprintf(stderr, "math:op: interv_min(): execute\n");)

	IFNOERROR(check_cace_int_init(result););
	IFNOERROR(check_cace_int_init(var););

	IFNOERRORX(mpz_set(*result->value1,*var->group->params->param->value1););
	return res;
}

/*
 * Writes the smallest value of a list of elements to result
 *
 * @type: Z
 * @result: Stores result
 * @var: not used
 * @params: Linked list of elements
 */
int z_min(PARAMS) {
	struct cace_int *tmp;
	int res = 0;

	MSG_DEBUG(5,fprintf(stderr, "math:op: z_min(): execute\n");)

	IFNOERROR(check_cace_int_init(result););
	IFNOERROR(check_cace_int_init(params););

	if (res >= 0) {
		mpz_set(*result->value1,*params->value1);
		tmp = params;
		while (tmp != NULL) {
			if (mpz_cmp(*result->value1, *tmp->value1) < 0) {
				mpz_set(*result->value1,*tmp->value1);
			}
			params = params->next;
		}
	}
	return res;
}


/*
 * Writes the biggest value of a list of elements to result
 *
 * @type: Z
 * @result: Stores result
 * @var: not used
 * @params: Linked list of elements
 */
int z_max(PARAMS) {
	struct cace_int *tmp;
	int res = 0;

	MSG_DEBUG(5,fprintf(stderr, "math:op: z_max(): execute\n");)

	IFNOERROR(check_cace_int_init(result););
	IFNOERROR(check_cace_int_init(params););

	if (res >= 0) {
		mpz_set(*result->value1,*params->value1);
		tmp = params;
		while (tmp != NULL) {
			if (mpz_cmp(*result->value1, *tmp->value1) > 0) {
				mpz_set(*result->value1,*tmp->value1);
			}
			params = params->next;
		}
	}
	return res;
}

/*
 * Returns 0 if given var value equals Zero, -35 otherwise
 *
 * @type: any
 * @result: not used
 * @var: Element to test for equality to 0
 * @params: not used
 */
int all_assert_true(PARAMS) {
	int res = 0;
	MSG_DEBUG(5,fprintf(stderr, "math:op: all_assert_true(): execute\n");)

	IFNOERROR(check_cace_int_init(var););

	if (res >= 0) {
		if (mpz_cmp_ui(*var->value1,0) == 0) {
			res = 0;
		} else {
			MSG_ERROR(fprintf(stderr, "math:op: all_assert_true(): Assertion failed\n");)
			cace_set_error(ERROR_ASSERTION_FAILED, "Assertion to true failed");
			res = -1;
		}
	}
	return res;
}

/*
 * Returns 0 if given var value does not equal, -35 otherwise
 *
 * @type: any
 * @result: not used
 * @var: Element to test for not equlity to 0
 * @params: not used
 */
int all_assert_false(PARAMS) {
	int res = 0;

	MSG_DEBUG(5,fprintf(stderr, "math:op: all_assert_false(): execute\n");)

	IFNOERROR(check_cace_int_init(var););
	if (res >= 0) {
		if (mpz_cmp_ui(*var->value1,0) != 0) {
			res = 0;
		} else {
			MSG_ERROR(fprintf(stderr, "math:op: all_assert_false(): Assertion failed\n");)
			cace_set_error(ERROR_ASSERTION_FAILED, "Assertion to false failed");
			res = -1;
		}
	}
	return res;
}

/*
 * Returns 0 if two given elements have equal value, -35 otherwise
 *
 * @type: any
 * @result: not used
 * @var: Left hand element to test for equality
 * @params: Right hand element to test for equality
 */
int all_assert_eq(PARAMS) {
	int res = 0;
	MSG_DEBUG(5,fprintf(stderr, "math:op: all_assert_eq(): execute\n");)

	IFNOERROR(check_cace_int_init(var););
	IFNOERROR(check_cace_int_init(params););

	if (res >= 0) {
		if (mpz_cmp(*var->value1,*params->value1) == 0) {
			res = 0;
		} else {
			MSG_ERROR(fprintf(stderr, "math:op: all_assert_eq(): Assertion failed\n");)
			cace_set_error(ERROR_ASSERTION_FAILED, "Assertion to equal failed");
			res = -1;
		}
	}
	return res;
}

/*
 * Returns 0 if an element is less or equal in value to another, -35 otherwise
 *
 * @type: any
 * @result: not used
 * @var: Element that is tested to be less or equal
 * @params: Element that is tested to be greater
 */
int all_assert_le(PARAMS) {
	int res = 0;
	MSG_DEBUG(5,fprintf(stderr, "math:op: all_assert_le(): execute\n");)

	IFNOERROR(check_cace_int_init(var););
	IFNOERROR(check_cace_int_init(params););

	if (res >= 0) {
		if (mpz_cmp(*var->value1,*params->value1) <= 0) {
			res = 0;
		} else {
			MSG_ERROR(fprintf(stderr, "math:op: all_assert_le(): Assertion failed\n");)
			cace_set_error(ERROR_ASSERTION_FAILED, "Assertion to less or equal failed");
			res =  -1;
		}
	}
	return res;
}

/*
 * Returns 0 if an element is greater or equal in value to another, -35 otherwise
 *
 * @type: any
 * @result: not used
 * @var: Element that is tested to be greater or equal
 * @params: Element that is tested to be less
 */
int all_assert_ge(PARAMS) {
	int res = 0;
	MSG_DEBUG(5,fprintf(stderr, "math:op: all_assert_ge(): execute\n");)

	IFNOERROR(check_cace_int_init(var););
	IFNOERROR(check_cace_int_init(params););

	if (res >= 0) {
		if (mpz_cmp(*var->value1,*params->value1) >= 0) {
			res =  0;
		} else {
			MSG_ERROR(fprintf(stderr, "math:op: all_assert_ge(): Assertion failed\n");)
			cace_set_error(ERROR_ASSERTION_FAILED, "Assertion to greater or equal failed");
			res = -1;
		}
	}
	return res;
}

/*
 * Returns 0 if an element is not equal in value to another, -35 otherwise
 *
 * @type: any
 * @result: not used
 * @var: Left hand element tested for not equality
 * @params: Right hand element tested for not equality
 */
int all_assert_ne(PARAMS) {
	int res = 0;
	MSG_DEBUG(5,fprintf(stderr, "math:op: all_assert_ne(): execute\n");)
	IFNOERROR(check_cace_int_init(var););
	IFNOERROR(check_cace_int_init(params););

	if (res >= 0) {
		if (mpz_cmp(*var->value1,*params->value1) != 0) {
			res =  0;
		} else {
			MSG_ERROR(fprintf(stderr, "math:op: all_assert_ne(): Assertion failed\n");)
			cace_set_error(ERROR_ASSERTION_FAILED, "Assertion to not equal failed");
			res = -1;
		}

	}
	return res;
}

/*
 * Returns 0 if an element is less in value to another, -35 otherwise
 *
 * @type: any
 * @result: not used
 * @var: Element that is tested to be less
 * @params: Element that is tested to be greater or equal
 */
int all_assert_l(PARAMS) {
	int res = 0;
	MSG_DEBUG(5,fprintf(stderr, "math:op: all_assert_l(): execute\n");)
	IFNOERROR(check_cace_int_init(var););
	IFNOERROR(check_cace_int_init(params););

	if (res >= 0) {
		if (mpz_cmp(*var->value1,*params->value1) < 0) {
			res =  0;
		} else {
			MSG_ERROR(fprintf(stderr, "math:op: all_assert_l(): Assertion failed\n");)
			cace_set_error(ERROR_ASSERTION_FAILED, "Assertion to less than failed");
			res = -1;
		}
	}
	return res;
}

/*
 * Returns 0 if an element is greater in value to another, -35 otherwise.
 *
 * @type: any
 * @result: not used
 * @var: Element that is tested to be greater
 * @params: Element that is tested to be less or equal
 */
int all_assert_g(PARAMS) {
	int res = 0;
	MSG_DEBUG(5,fprintf(stderr, "math:op: all_assert_g(): execute\n");)
	IFNOERROR(check_cace_int_init(var););
	IFNOERROR(check_cace_int_init(params););

	if (res >= 0) {
		if (mpz_cmp(*var->value1,*params->value1) > 0) {
			res =  0;
		} else {
			MSG_ERROR(fprintf(stderr, "math:op: all_assert_g(): Assertion failed\n");)
			cace_set_error(ERROR_ASSERTION_FAILED, "Assertion to greater than failed");
			res = -1;
		}
	}
	return res;
}

/*
 * Returns 1 if an element is equal in value and type to another, -57 if value
 * is unequal, -56 if type is unequal.
 *
 * @type: any
 * @result: not used
 * @var: Left hand element tested for equality
 * @params: Right hand element tested for equality
 */
int all_verifyequals(PARAMS) {
	int res = 0;
	MSG_DEBUG(5,fprintf(stderr, "math:op all_verifyequals(): execute\n");)
	IFNOERROR(check_cace_int_init(var););
	IFNOERROR(check_cace_int_init(params););

	//check if group is the same
	if (var->group == params->group) {
		//check if value is the same
		if (mpz_cmp(*var->value1,*params->value1) != 0) {
			MSG_ERROR(
					fprintf(stderr, "math:op: all_verifyequals(): elements not the same: ");
					cace_int_print_err(var);
					fprintf(stderr, " != ");
					cace_int_print_err(params);
					fprintf(stderr, "\n");
					)
			cace_set_error(ERROR_VERIFICATION_FAILED, "Verification failed: Element not equal");
			res = -1;
		} else {
			//TODO: Good status message
			res = 2;
		}
	} else {
		MSG_ERROR(
				fprintf(stderr, "math:op: all_verifyequals(): "
						"types not the same: %s != %s\n",
						get_type(var->group->type),
						get_type(params->group->type));)
		cace_set_error(ERROR_VERIFICATION_FAILED, "Verification failed: Type not equal");
		res = -1;
	}
	return res;
}


/*
 * Writes a block of bits of the binary representation of a value to result
 *
 * @type: any
 * @result: Stores result
 * @var: Elements where bits are taken from
 * @params: Linked list with two values: howmany bits -> which block of bits
 */
int all_getbits(PARAMS) {
	int res = 0;
	int i;
	unsigned long tmp, howmany, which;

	MSG_DEBUG(5,fprintf(stderr, "math:op: all_getbits(): execute");)

	IFNOERROR(check_cace_int_init(var););
	IFNOERROR(check_cace_int_init(params););
	IFNOERROR(check_cace_int_init(result););

	if (res >= 0) {
		howmany = mpz_get_ui(*params->value1);
		which = mpz_get_ui(*params->next->value1) * howmany;
		mpz_set_ui(*result->value1,0);
		for(i = 0; i < howmany; i++) {
			tmp = mpz_tstbit(*var->value1,which+i);
			//TODO: store in temp variable, such that var == result is allowed
			if (tmp == 1) {
				mpz_setbit(*result->value1,i);
			}
		}
	}
	return res;
}

/*
 * Writes the logical AND of two values to result
 *
 * Returns 0 if no error happens, -45 otherwise
 *
 * @type: any
 * @result: Stores result
 * @var: Left hand element of AND operation
 * @params: Right hand element of AND operation
 */
int all_and(PARAMS) {
	int res = 0;

	MSG_DEBUG(5,fprintf(stderr, "math:op: all_and(): execute\n");)


	if (result != NULL && var != NULL && params != NULL) {
		if (params->next == NULL) {
			// Only test base and params elements
			res = var->known & params->known;
			result->known = res;
		} else {
			//params might represent a linked list -> go through list
			//TODO: find a better solution!
			MSG_ERROR(fprintf(stderr, "math:op all_and(): linked lists not supported yet");)
			cace_set_error(ERROR_NOT_SUPPORTED, "Binary operations do not support parsing of linked lists yet");
			res = -1;
		}
	} else {
		MSG_ERROR(fprintf(stderr, "math:op all_and(): var or result or prams is null pointer\n");)
		cace_set_error(ERROR_NULL_POINTER, "Null pointer given as parameter to all_and()");
		res = -1;
	}
	return res;
}


/*
 * Writes the logical OR of two values to result
 *
 * Returns 0 if no error happens, -45 otherwise
 *
 * @type: any
 * @result: Stores result
 * @var: Left hand element of OR operation
 * @params: Right hand element of OR operation
 * TODO: logical or does not work if either var or params is not \in {0,1} -> check membership
 */
int all_or(PARAMS) {
	int res = 0;

	MSG_DEBUG(5,fprintf(stderr, "math:op: all_or(): execute\n");)

	if (result != NULL && var != NULL && params != NULL) {
		if (params->next == NULL) {
			//only test base and params elements
			res = var->known | params->known;
			result->known = res;
		} else {
			//params represent a linked list -> go through list
			//TODO: find a better solution!
			MSG_ERROR(fprintf(stderr, "math:op all_or(): linked lists not supported yet");)
			cace_set_error(ERROR_NOT_SUPPORTED, "Binary operations do not support parsing of linked lists yet");
			res = -1;
		}
	} else {
		MSG_ERROR(fprintf(stderr, "math:op all_or(): var or result or prams is null pointer\n");)
		cace_set_error(ERROR_NULL_POINTER, "Null pointer given as parameter to all_or()");
		res = -1;
	}
	return res;
}


/*
 * Writes number of known elements (not null) to known field in result.
 *
 * Return the number of known elements if no error occurs, -45 otherwise
 *
 * @type: any
 * @result: Stores result
 * @var: Left hand element of Count operation
 * @params: Right hand element of Count operation
 */
int all_count(PARAMS) {
	int res = 0;
	struct cace_int *tmp;
	MSG_DEBUG(5,fprintf(stderr, "math:op: all_count(): execute\n");)

	if (result != NULL && var != NULL && params != NULL) {
		if (params->next == NULL) {
			//only test base and params elements
			res = var->known + params->known;
			result->known = res;
		}
		tmp = params->next;
		while (tmp != NULL) {
			res += tmp->known;
			tmp = tmp->next;
		}
	} else {
		MSG_ERROR(fprintf(stderr, "math:op all_count(): var or result or prams is null pointer\n");)
		cace_set_error(ERROR_NULL_POINTER, "Null pointer given as parameter to all_count()");
		res = -1;
	}
	return res;

}


/*
 * Writes 0 to known field of result if known field of var is higher than a
 * given threshold.
 *
 * Returns 0 if no error happens, -47 if value is below threshold
 *
 * @type: any
 * @result: Stores result
 * @var: Element representing known elements
 * @params: Threshold of known elements
 */
int all_th(PARAMS) {
	int res = 0;
	unsigned long threshold;

	MSG_DEBUG(5,fprintf(stderr, "math:op: all_th(): execute\n");)

	if (result != NULL && var != NULL && params != NULL) {
		threshold = mpz_get_ui(*params->value1);
		if (var->known < threshold) {
			MSG_ERROR(fprintf(stderr, "math:op: all_th(): known elements %d below threshold %lu\n",var->known, threshold);)
			cace_set_error(ERROR_KNOWLEDGE_BELOW_THRESHOLD, "Knowledge below threshold");
			res = -1;
			result->known = 0;
		} else {
			result->known = 1;
		}
	} else {
		MSG_ERROR(fprintf(stderr, "math:op all_and(): var or result or prams is null pointer\n");)
		cace_set_error(ERROR_NULL_POINTER, "Null pointer given as parameter to all_and()");
		res = -1;
	}
	return res;

}

/*
 * Calculating multi-exponentiation using ?? Algorithm (library: mexp.a,
 * written by Endre Bangerter & Peter Linder, BFH-TI)
 *
 * Returns 0 if no error happens, -42 if calculation preparation was unsuccessful
 * or any other error raised by type checking or memory protection.
 *
 * @type: Multiplicative group
 * @result: Stores result
 * @var: not used
 * @params: Linked list of even elements represented in the form
 *    base 1 -> exponent 1 -> base 2 -> exponent 2 -> .. -> base n -> exponent n
 */
int zmult_mpow(PARAMS) {
	mpz_t *bases;
	mpz_t *exp;
	struct cace_int *tmp;
	int i = 0, j = 0, count = 0;
	int res = 0;

	MSG_DEBUG(5,fprintf(stderr, "math:op: zmult_mpow(): execute\n");)

	IFNOERROR(check_cace_int_init(params););
	IFNOERROR(check_cace_int_init(result););
	if (res < 0) {
		return res;
	}
	// Calculate number of exponents and bases, to allocate memory
	tmp = params;
	i = 0;
	while (tmp != NULL) {
		if (i++ % 2 == 1) {
			count++;
		}
		tmp = tmp->next;
	}
	if (i % 2 == 1) {
		// We do not have the same amount of bases and exponents: Abort
		MSG_ERROR(fprintf(stderr, "math:op: zmult_mpow(): number of exponents != number of bases\n");)
		cace_set_error(ERROR_DYNAMIC_TYPECHECKING, "MultiExp not having the same amount of bases and exponents");
		res = -1;
	}
	// Allocate the memory for bases (count elements) and exponents (count
	// elements) in one chunk:
	IFNOERRORX(exp = (mpz_t *)calloc(count*2,sizeof(mpz_t)););
	if (res >= 0 && exp == NULL) {
		cace_set_error(ERROR_OUT_OF_MEMORY, "Out of memory in zmult_mpow()");
		res = -1;
	}

	if (res >= 0) {
		bases = &exp[count];

		//add bases and exponents to the arrays for mexp
		i = 1;
		j = 0;
		tmp = params;
		while (tmp != NULL) {
			if (i % 2 == 1) {
				mpz_init_set(bases[j],*tmp->value1);
			} else {
				mpz_init_set(exp[j], *tmp->value1);
				if (mpz_cmp_ui(exp[j],0) < 0) {
					mpz_invert(bases[j], bases[j],
							*params->group->params->param->value1);
					mpz_neg(exp[j], exp[j]);
				}
				j++;
			}
			i++;
			tmp = tmp->next;
		}

		MSG_DEBUG(5,
			fprintf(stderr, "math:op: zmult_mpow(): bases and exponents:\n");
			for (i = 0; i < count; i++) {
				gmp_fprintf(stderr, "\tbase[%d] = %Zd\n\texp[%d] = %Zd\n",i,bases[i],i,exp[i]);
			}
		)

		// Using the algorithm to calculate multi exponentiation
		mexp(count, bases, exp, params->group->params->param->value1, result->value1);


		MSG_DEBUG(5, gmp_fprintf(stderr, "\tmath:op: zmult_mpow(): result: %Zd\n",*result->value1);)
	}
	if (exp != NULL) {
		// Cleaning up
		free(exp);
		exp = NULL;
	}

	return res;
}

/**
 * Checks if a given element is in Z
 *
 * @type: Z
 * @result: not used
 * @var: Element whose value is tested to be within its defined type
 *
 */
int z_checkmemb(PARAMS) {
	MSG_DEBUG(5,fprintf(stderr, "math:op: z_checkmemb(): execute\n");)
	// Every element that can be reprsented in mpz_t is in Z
	return 0;
}

/**
 * Returns 0 if a given element is elemnt of an associated additive group.
 * If this is not true and the result element pointer is not NULL, the
 * result element is set to -1 and 0 returned. In case of result == NULL -3
 * is returned.
 *
 * @type: Additive group
 * @result: Optional, to store result
 * @var: Element whose value is tested to be within its defined type
 * @param: not used
 *
 */
int zplus_checkmemb(PARAMS) {
	int res = 0;
	MSG_DEBUG(5,fprintf(stderr, "math:op: zplus_checkmemb(): execute\n");)

	IFNOERROR(check_cace_int_init(var););

	if (res >= 0) {
		MSG_DEBUG(5, gmp_fprintf(stderr, "\tvar: %Zd\n",*var->value1););
		MSG_DEBUG(5, gmp_fprintf(stderr, "\tmodul: %Zd\n",*var->group->params->param->value1););
	}

	if (res >= 0 && mpz_cmp(*var->value1,*var->group->params->param->value1) < 0) {
		if (result != NULL)  {
			mpz_set_ui(*result->value1,0);
		}
		res = 0;
	} else if (res >= 0) {
		if (result != NULL)  {
			mpz_set_ui(*result->value1,-1);
		}
		res = -1;
		MSG_ERROR(fprintf(stderr, "math:op: zplus_checkmemb(): verification failed\n");)
		cace_set_error(ERROR_CHECK_MEMBERSHIP, "Zmod+() check membership verification failed");
	}

	return res;
}

/**
 * Returns 0 if a given element is element of an associated multiplicative group.
 * If this is not true and the result element pointer is not NULL, the
 * result element is set to -1 and 0 returned. In case of result == NULL -3
 * is returned.
 *
 * @type: Multiplicative Group
 * @result: Optional, to store result
 * @var: Element whose value is tested to be within its defined type
 * @param: not used
 *
 */
int zmult_checkmemb(PARAMS) {
	int res = 0;
	struct cace_int tmp;

	MSG_DEBUG(5,fprintf(stderr, "math:op: zmult_checkmemb(): execute\n");)

	IFNOERROR(check_cace_int_init(var););
	if (res < 0) return res;

	tmp.value1 = malloc(sizeof(mpz_t));
    if (tmp.value1 == NULL) {
		cace_set_error(ERROR_OUT_OF_MEMORY, "Out of memory in zmult_checkmemb()");
		res = -1;
		return res;
    }

	mpz_init_set_ui(*tmp.value1,0);

	if (mpz_invert(*tmp.value1, *var->value1,
			*var->group->params->param->value1) != 0) {
		if (result != NULL)  {
			mpz_set_ui(*result->value1,0);
		}
	} else {
		MSG_ERROR(fprintf(stderr, "math:op: zmult_checkmemb(): verification failed\n");)
		cace_set_error(ERROR_CHECK_MEMBERSHIP, "Zmod*() check membership verficiation failed");
		res = -1;
	}
	if (res != 0 && result != NULL) {
		mpz_set_ui(*result->value1,-1);
		res = 0;
	}
	mpz_clear(*tmp.value1);
	free(tmp.value1);
	tmp.value1 = NULL;

	return res;
}

/**
 * Returns 0 if a given element is probable prime. If this is not true and the
 * result element pointer is not NULL, the result element is set to -1
 * and 0 is returned. In case of result == NULL -3 is returned.
 *
 * @type: Prime
 * @result: Optional, to store result
 * @var: Element whose value is tested to be within its defined type
 * @param: not used
 *
 */
int prime_checkmemb(PARAMS) {
	int res = 0;
	MSG_DEBUG(5,fprintf(stderr, "math:op: prime_checkmemb(): execute\n");)

	IFNOERROR(check_cace_int_init(var););

	IFNOERROR(mpz_probab_prime_p(*var->value1, 5););
	if (res == 0) {
		res = -1;
		MSG_ERROR(fprintf(stderr, "math:op: prime_checkmemb(): verification failed\n");)
		cace_set_error(ERROR_CHECK_MEMBERSHIP, "Prime() check membership verficiation failed");
	}
	if (result != NULL)  {
		mpz_set_ui(*result->value1,res);
		res = 0;
	}
	return res;
}

/**
 * Returns 0 if a given element is within the inclusive bounds of the associated
 * interval. If this is not true and the result element pointer is not NULL, the
 * result element is set to -1 and 0 returned. In case of result == NULL -3
 * is returned.
 *
 * @type: Interval
 * @result: Optional, to store result
 * @var: Element whose value is tested to be within its defined type
 * @param: not used
 *
 */
int interv_checkmemb(PARAMS) {
	//op.param1 <= op.value  <= op.param2u
	int res = 0;
	int val1 = 0;
	int val2 = 0;
	struct cace_int *op1,*op2;

	MSG_DEBUG(5,fprintf(stderr, "math:op: interv_checkmemb(): execute\n");)

	IFNOERROR(check_cace_int_init(var););
	if (res < 0) return res;

	op1 = var->group->params->param;
	op2 = var->group->params->next->param;

	val1 = mpz_cmp(*var->value1,*op1->value1);
	val2 = mpz_cmp(*var->value1,*op2->value1);
	MSG_DEBUG(5,
		fprintf(stderr, "math:op: interv_checkmemb(): bounds:\n");
		fprintf(stderr, "\tleft bound check: %d, right bound check: %d\n",val1,val2);
		gmp_fprintf(stderr, "\tvalue: %Zd\n",*var->value1);
		gmp_fprintf(stderr, "\tbound1: %Zd\n",*op1->value1);
		gmp_fprintf(stderr, "\tbound2: %Zd\n",*op2->value1);
	);
	if (val1 >= 0 && val2 <= 0) {
		res = 0;
	} else {
		res = -1;
		MSG_ERROR(fprintf(stderr, "math:op: interv_checkmemb(): verification failed\n");)
		cace_set_error(ERROR_CHECK_MEMBERSHIP, "Zmod*() check membership verficiation failed");
	}

	if (result != NULL)  {
		mpz_set_ui(*result->value1,res);
		res = 0;
	}
	return res;
}



/**
 * Writes a random value within a given interval (inclusive) to result
 *
 * @type: Interval
 * @result: Stores result
 * @var: Element of type Interval
 * @param: not used
 *
 */
int interv_random(PARAMS) {
	int res = 0;
	mpz_t bound;
	gmp_randstate_t gmpRandState;

	MSG_DEBUG(5, fprintf(stderr, "math:op: interv_random(): executing\n"););

	IFNOERROR(check_cace_int_init(var););
	IFNOERROR(check_cace_int_init(result););

	if (res >= 0) {
		MSG_DEBUG(5,
			gmp_fprintf(stderr, "\tbound1: %Zd\n",*var->group->params->param->value1);
			gmp_fprintf(stderr, "\tbound2: %Zd\n",*var->group->params->next->param->value1);
		)

		mpz_init_set_ui(*result->value1,0);

		// Calculate total bound (upper bound - lower bound)
		mpz_init_set(bound,*var->group->params->next->param->value1);
		mpz_sub(bound,bound,*var->group->params->param->value1);
		MSG_DEBUG(5, gmp_fprintf(stderr, "\tbound total: %Zd\n",bound);)

		// Initialize the random state
		gmp_randinit_mt(gmpRandState);
		gmp_randseed_ui(gmpRandState, rand());

		// Create random value between 0 .. bound -1
		mpz_urandomm(*result->value1, gmpRandState, bound);

		// Add lower bound of inerval to result, to shift it back into it.
		mpz_add(*result->value1,*result->value1,*var->group->params->param->value1);
	}
	mpz_clear(bound);
	return res;
}

/**
 * Writes a random value element of a multiplicative group to result.
 *
 * Returns 0 on success, -40 if no member of given group could be found
 *
 * @type: Multiplicative group
 * @result: Stores result
 * @var: Element of a Multiplicative group
 * @param: not used
 *
 */
int zmult_random(PARAMS) {
	int res = 0;
	int tries = 3;
	gmp_randstate_t gmpRandState;

	MSG_DEBUG(5, fprintf(stderr, "math:op: zmult_random(): executing\n"););

	IFNOERROR(check_cace_int_init(var););
	IFNOERROR(check_cace_int_init(result););

	if (res >= 0) {
		// Initialize the random state with default algorithm...
		gmp_randinit_mt(gmpRandState);
		gmp_randseed_ui(gmpRandState, rand());

		// Find a random value, that is member of the group number of /tries/
		// times. If unsuccessful abort and return error code.
		do {
			// Create random between 0 .. randBound -1
			mpz_urandomm(*result->value1, gmpRandState,
					*var->group->params->param->value1);
			res = 0;

		} while (--tries > 0 && zmult_checkmemb(NULL, result, NULL));
		if (tries <= 0) {
			res = -1;
			MSG_ERROR(fprintf(stderr, "math:op: zmult_random(): No random element found\n");)
			cace_set_error(ERROR_NO_RANDOM_FOUND, "No random element for Zmod*() found");
		}
	}
	return res;
}

/**
 * Writes a random value element of an additive group to result.
 *
 * Returns 0 on success, -40 if no member of given group could be found
 *
 * @type: Additive group
 * @result: Stores result
 * @var: Element of a Multiplicative group
 * @param: not used
 *
 */

int zplus_random(PARAMS) {
	int res = 0;
	int tries = 3;
	gmp_randstate_t gmpRandState;

	MSG_DEBUG(5, fprintf(stderr, "math:op: zplus_random(): executing\n"););

	IFNOERROR(check_cace_int_init(var););
	IFNOERROR(check_cace_int_init(result););

	if (res >= 0) {
		// Initialize the random state with default algorithm...
		gmp_randinit_mt(gmpRandState);
		gmp_randseed_ui(gmpRandState, rand());

		// Find a random value, that is member of the group number of /tries/
		// times. If unsuccessful abort and return error code.
		do {
			// Create random between 0 .. randBound -1
			mpz_urandomm(*result->value1, gmpRandState,
					*var->group->params->param->value1);
			res = 0;

		} while (--tries > 0 && zplus_checkmemb(NULL, result, NULL));
		if (tries <= 0) {
			res = -1;
			MSG_ERROR(fprintf(stderr, "math:op: zplus_random(): No random element found\n");)
			cace_set_error(ERROR_NO_RANDOM_FOUND, "No random element for Zmod*() found");
		}
	}
	return res;
}

/**
 * Writes a random value element smaller than a given bit length to result
 *
 * @type: Z
 * @result: Stores result
 * @var: Element of Z with a defined bit length
 * @param: not used
 *
 */
int z_random(PARAMS) {
	int res = 0;
	gmp_randstate_t gmpRandState;
	unsigned long bound;

	MSG_DEBUG(5, fprintf(stderr, "math:op: z_random(): executing\n"););

	IFNOERROR(check_cace_int_init(var););
	IFNOERROR(check_cace_int_init(result););

	if (res >= 0) {
		bound = var->group->bitlen;
		mpz_init_set_ui(*result->value1,0);
		bound = (bound * log(2)/log(10))+1;

		// Initialize the random state with default algorithm...
		gmp_randinit_mt(gmpRandState);
		gmp_randseed_ui(gmpRandState, rand());

		// Create random between 0 .. randBound 2 ^ randbound
		mpz_urandomb(*result->value1, gmpRandState, bound);
	}
	return res;
}


/**
 * Writes a hash over a list of concatenated elements (by max bit length) to
 * result. This function uses the hash algorithms given in libTomCrypt, a public
 * domain crypto library
 *
 * @type: any
 * @result: Stores result
 * @var: not used
 * @param: Linked list of element to be concatenated and hashed
 *
 */
/*
int all_hash(PARAMS) {
    int res = 0;
    int numb, size, count, bcount, max, i;
    int idx, x;
    struct cace_int *tmp;
    unsigned char hash_buffer[MAXBLOCKSIZE];
    char retstr[MAXBLOCKSIZE];
    unsigned char *p;
    hash_state md;
	VERBOSE(5, printf("Entering all_hash()\n"););

    // Registering the hashes we need (libTomCrypt)
    register_hash (&sha512_desc);

	// Select the hash, exit if not found (libTomCrypt
    idx = find_hash("sha512");
    if (idx == -1) {
    	VERBOSE(4, printf("\tInvalid hash specified.\n"););
    	return -1;
    }

    // Calculates the buffer size to store the greatest number according to its
    // bitlength
    size = MAXBLOCKSIZE;
    max = 0;
	tmp = params;
    numb = 8 * size;
	while (tmp != NULL) {
		// Calculate how many blocks this element needs
	    bcount = (((tmp->group->bitlen * log(2)/log(10))+1) / size) + 1;
	    if (max < bcount) {
	    	max = bcount;
	    }
		tmp = tmp->next;
	}
	// Allocate the memory. calloc ensure that the memory area is nulled
	// which is necessary for this application
    p = calloc (max,size);
    if (p == NULL) {
    	return -23;
    }
    // Initialize hash_descripter (libTomCrypt)
    hash_descriptor[idx].init(&md);
    tmp = params;
	while (tmp != NULL) {
		bcount = (((tmp->group->bitlen * log(2)/log(10))+1) / size) + 1;
	    count = (mpz_sizeinbase (*tmp->value1, 2) + numb-1) / numb;
	    if (count > bcount) {
	        // The number is greater than the defined max bit length of the
	    	// number
	        res = -34;
	        break;
	    }
	    // Binary export the number to the beginning of the buffer for the
	    // current bit length (we wan't to hash the binary representation,
	    // not the structure of the element.
	    mpz_export(p+((bcount-count)*size),NULL,-1,128,1,0,*tmp->value1);
	    // Block-wise add and compute the hash (libTomCrypt)
	    for (i = 0; i < bcount; i++) {
			hash_descriptor[idx].process(&md, p+(i*size), size);
		}
		VERBOSE(5,
			printf("\tcount: %d, bcount: %d, size: %d\n",count, bcount, size););

		tmp = tmp->next;
	}
	// Finalize the computation, and store the result in a buffer (libTomCrypt)
	IFNOERROR(hash_descriptor[idx].done(&md, hash_buffer));
	if (res >= 0) {
		// Write the hash as hexadecimal value to a return string
		for (x = 0; x < (int)hash_descriptor[idx].hashsize; x++) {
		  sprintf(retstr+(2*x),"%02x",hash_buffer[x]);
		}
	}
	// End the resultstring with \0
	IFNOERROR(retstr[2*x] = '\0');
	VERBOSE(5, printf("\tHash: %s\n",retstr););

	// Add the hash to the return value element
    IFNOERROR(mpz_init_set_str(*result->value1,retstr,16));
    free(p);
    p = NULL;

    return res;
}
*/

/*
 * Not implemented
 */
int all_or_isconsistant(PARAMS) {
	MSG_ERROR(fprintf(stderr, "math:all_or_isconsistant(): not implemented");)
	cace_set_error(ERROR_NOT_SUPPORTED, "OR Isconsistant checks checks not supported yet");
	return -1;
}

/*
 * Not implemented
 */
int all_shamir_isconsistant(PARAMS) {
	MSG_ERROR(fprintf(stderr, "math:all_shamir_isconsistant(): not implemented");)
	cace_set_error(ERROR_NOT_SUPPORTED, "Shamir Isconsistant checks checks not supported yet");
	return -1;
}

/**
 * Set all unkown partitions of a secret value c (in GF(2) such that they
 * satisfy the interpolation polynom over irreducible polynom x^(n-k)+x+1.
 * (k = threshold, n = number of elements).
 *
 * @type: any
 * @result: not used
 * @var: c
 * @param: Linked list of element c(1) ... c(n), as partition of c in GF(2) and
 * 		k == threshold as last element of the list
 *
 */
int all_shamir_complete(PARAMS) {
	unsigned int n = 0, known = 0, cuknown = 0, i = 0, j = 0;
	unsigned long k = 0,l;
	int res = 0;
	struct cace_int *tmp;
	struct point *p;
	mpz_t t1;


	MSG_DEBUG(5, fprintf(stderr, "math:op: all_shamir_complete(): executing\n"););

	IFNOERROR(check_cace_int_init(var););
	IFNOERROR(check_cace_int_init(params););
	IFNOERROR(check_cace_int_init(params->next););
	if (res < 0) return res;

	// Get total number of elements n and number of unknown elements
	tmp = params;
	while (tmp != NULL) {
		if (!tmp->known) {
			cuknown++;
		}
		n++;
		// The last element of the list is the threshold k, save the number
		// and set the detach it from the list
		/* Uncommented out since impossible to hapen...
		 * if (tmp->next == NULL) {
			return -32;
		} else*/
		if (tmp->next->next == NULL) {
			k = mpz_get_ui(*tmp->next->value1);
			tmp->next = NULL;
		}
		tmp = tmp->next;
	}

	// Extract k and check that known <= n-k
	// l = n-k == threshold
	l = n-k;
	known = n-cuknown;
	MSG_DEBUG(5,
			fprintf(stderr, "\tl: %lu n: %d k: %lu known: %d, unknown: %d\n",l,n,k,known,cuknown);)

	if (known > l) {
		cace_set_error(ERROR_ATTRIBUTE_ERROR, "Shamir: Number of known elements greater than threshold n-k");
		MSG_ERROR(fprintf(stderr, "math:op: all_shamir_complete(): Number of known elements greater than threshold n-k");)
		res = -1;
		//return -31;
	}
	if (res < 0) return res;

	// Allocate memory to point structure for l elements
	p = (struct point*)calloc(l, sizeof(struct point));
	if (p == NULL) {
		cace_set_error(ERROR_OUT_OF_MEMORY, "Out of memory in all_shamir_complete()");
		res = -1;
	}
	if (res < 0) return res;

	// Randomly associate numbers to n-k - known elements beginning with the
	// first unknown
	tmp = params;
	i = 1; j = 0;
	while (tmp != NULL) {
		MSG_DEBUG(5, fprintf(stderr, "\tAnalysing position %d, known %d\n",i,tmp->known););
		if (!tmp->known && known < l) {
			MSG_DEBUG(5, fprintf(stderr, "\tAdd random to shamir at pos %d...\n",i););

			z_random(tmp, var, NULL);
			tmp->known = 1;
			known++;
		}
		if (tmp->known) {
			MSG_DEBUG(5, fprintf(stderr, "\tadd point pos %d and elem %d\n",i,j););

			mpz_init_set(p[j].x,*tmp->value1);
			mpz_init_set_ui(p[j].y,i);
			j++;
		}
		i++;
		tmp = tmp->next;
	}

	// Calculate the residual not know values with lagrange, the function needed
	// are extern in galois.c
	i = 1;
	mpz_init(t1);
	tmp = params;
	while (tmp != NULL) {
		MSG_DEBUG(5,
			fprintf(stderr,"\tlagrange for pos %d if value %d != 1\n",i,tmp->known);
		);

		if (!tmp->known) {
			mpz_set_ui(t1,i);
			lagrange(*tmp->value1, l, p, t1 , *var->value1,var->group->bitlen);
		}
		MSG_DEBUG(5, gmp_fprintf(stderr, "\tvalue = %Zd\n",*tmp->value1););

		i++;
		tmp = tmp->next;
	}
	// Clear up
	mpz_clear(t1);
	free(p);
	p = NULL;

	return res;
}


/**
 * var = c
 * params (c1,c2)
 */
/**
 * Set all unknown elements c1 .. cn that to XOR to c.
 *
 * @type: any
 * @result: not used
 * @var: c
 * @param: Linked list of element c(1) ... c(n)
 *
 */
int z_or_complete(PARAMS) {
	int res = 0;
	int count = 1, cuknown = 0, i = 0, x = 0;
	struct cace_int *tmp, *saved;
	mpz_t xortmp;

	MSG_DEBUG(5, fprintf(stderr, "math:op: z_or_complete(): executing\n"););

	IFNOERROR(check_cace_int_init(var););
	IFNOERROR(check_cace_int_init(params););
	IFNOERROR(check_cace_int_init(params->next););
	if (res < 0) return res;

	tmp = params;
	while (tmp != NULL) {
		if (!tmp->known) {
			cuknown++;
		}
		count++;
		tmp = tmp->next;
	}

	tmp = params;
	if (tmp != NULL && cuknown > 0) {
		i = 0;
		while (tmp != NULL) {
			if (!tmp->known && cuknown > 1) {
				z_random(tmp, var, NULL);
				cuknown--;
			} else if (!tmp->known && cuknown == 1) {
				saved = tmp;
				x = i;
			}
			tmp = tmp->next;
			i++;
		}

		tmp = params;
		mpz_init_set_ui(xortmp,0);
		i = 0;
		while (tmp != NULL) {
			if (i != x) {
				mpz_xor(xortmp,*tmp->value1, xortmp);
			}
			tmp->known = 1;
			i++;
			tmp = tmp->next;
		}
		mpz_xor(*saved->value1,*var->value1,xortmp);
	} else {
		res = -1;
		cace_set_error(ERROR_ATTRIBUTE_ERROR, "OR complete: All values already known, therefore every value guessed, nothing to proof");
		MSG_ERROR(fprintf(stderr, "math:op: z_or_complete(): all values known, every value guesssed, nothing to proof");)
	}

	return res;
}

/**
 * Calculates the power of a given base to its exponent in a multiplicative
 * group
 *
 * This method may through an GMP arithmetic exception if the given element is
 * not element of the associated multiplicative group.
 *
 * @type: Multiplicative group
 * @result: Stores result
 * @var: Base
 * @param: Exponent
 *
 */
int zmult_pow(PARAMS) {
	int res = 0;
	MSG_DEBUG(5, fprintf(stderr, "math:op: zmult_pow(): executing\n"););

	IFNOERROR(check_cace_int_init(result););
	IFNOERROR(check_cace_int_init(var););
	IFNOERROR(check_cace_int_init(params););

	if (res >= 0) {
		mpz_powm(*result->value1, *var->value1, *params->value1,
				*var->group->params->param->value1);
	}

	return res;
}

/**
 * Calculates the power of a given base to its exponent.
 *
 * @type: Z
 * @result: Stores result
 * @var: Base
 * @param: Exponent
 *
 */
int z_pow(PARAMS) {
	int res = 0;
	unsigned long exp;
	MSG_DEBUG(5, fprintf(stderr, "math:op: z_pow(): executing\n"););

	IFNOERROR(check_cace_int_init(result););
	IFNOERROR(check_cace_int_init(var););
	IFNOERROR(check_cace_int_init(params););

	if (mpz_fits_ulong_p(*params->value1)) {
		exp = mpz_get_ui(*params->value1);
		mpz_pow_ui(*result->value1, *var->value1, exp);
		res = 0;
	} else {
		cace_set_error(ERROR_NUMBER_OUT_OF_RANGE, "Exponent of power operation not representable with unsigned long");
		MSG_ERROR(fprintf(stderr, "math:op: zmult_pow(): exponent out of range\n");)
		res = -1;
	}
	return res;
}


/**
 * Calculates the multiplication of two element in Z
 *
 * @type: Z
 * @result: Stores result
 * @var: Left hand element of multiplication
 * @param: Right hand element of multiplication
 *
 */
int z_mult(PARAMS) {
	int res = 0;
	MSG_DEBUG(5, fprintf(stderr, "math:op: z_mult(): executing\n"););

	IFNOERROR(check_cace_int_init(result););
	IFNOERROR(check_cace_int_init(var););
	IFNOERROR(check_cace_int_init(params););

	if (res >= 0) {
		mpz_mul(*result->value1, *var->value1, *params->value1);
	}
	return res;
}

/**
 * Calculates the multiplication of two element in an interval
 *
 * @type: Interval
 * @result: Stores result
 * @var: Left hand element of multiplication
 * @param: Right hand element of multiplication
 *
 */
int interv_mult(PARAMS) {
	int res = 0;
	MSG_DEBUG(5, fprintf(stderr,"math:op: interv_mult(): executing\n"););
	IFNOERROR(check_cace_int_init(result););
	IFNOERROR(check_cace_int_init(var););
	IFNOERROR(check_cace_int_init(params););

	//TODO: everything alright here? no bounds checking?
	if (res >= 0) {
		mpz_mul(*result->value1, *var->value1, *params->value1);
	}
	return res;
}

/**
 * Calculates the multiplication of two element in a multiplicative group
 *
 * @type: Multiplicative group
 * @result: Stores result
 * @var: Left hand element of multiplication
 * @param: Right hand element of multiplication
 *
 */
int zmult_mult(PARAMS) {
	int res = 0;
	mpz_t tmp;
	MSG_DEBUG(5, fprintf(stderr,"math:op: interv_mult(): executing\n"););

	IFNOERROR(check_cace_int_init(result););
	IFNOERROR(check_cace_int_init(var););
	IFNOERROR(check_cace_int_init(params););

	if (res >= 0) {
		mpz_init_set_ui(tmp,0);
		mpz_mul(tmp, *var->value1, *params->value1);
		mpz_mod(*result->value1, tmp, *var->group->params->param->value1);
		mpz_clear(tmp);
	}

	return res;
}

/**
 * Calculates the multiplication of two element in an additive group
 *
 * @type: Z
 * @result: Stores result
 * @var: Left hand element of multiplication
 * @param: Right hand element of multiplication
 *
 */
int zplus_mult(PARAMS) {
	int res = 0;
	mpz_t tmp;
	MSG_DEBUG(5, fprintf(stderr, "math:op: zplus_mult(): executing\n"););

	IFNOERROR(check_cace_int_init(result););
	IFNOERROR(check_cace_int_init(var););
	IFNOERROR(check_cace_int_init(params););
	if (res >= 0) {
		mpz_init_set_ui(tmp,0);
		mpz_mul(tmp, *var->value1, *params->value1);
		mpz_mod(*result->value1, tmp, *var->group->params->param->value1);
		mpz_clear(tmp);
	}
	return 0;
}

/**
 * Writes the result of adding one element to the another
 *
 * @type: Z
 * @result: Stores result
 * @var: Left hand element of addition
 * @param: Right hand element of addition
 *
 */
int z_add(PARAMS) {
	int res = 0;
		MSG_DEBUG(5, fprintf(stderr, "math:op: z_add(): executing\n"););

		IFNOERROR(check_cace_int_init(result););
		IFNOERROR(check_cace_int_init(var););
		IFNOERROR(check_cace_int_init(params););

	if (res >= 0) {
		mpz_add(*result->value1, *var->value1, *params->value1);
	}
	return res;
}

/**
 * Write the result of adding one element to another in an interval
 *
 * @type: Interval
 * @result: Stores result
 * @var: Left hand element of addition
 * @param: Right hand element of addition
 *
 */
int interv_add(PARAMS) {
	int res = 0;
	MSG_DEBUG(5, fprintf(stderr, "math:op: interv_add(): executing\n"););

	IFNOERROR(check_cace_int_init(result););
	IFNOERROR(check_cace_int_init(var););
	IFNOERROR(check_cace_int_init(params););

	//TODO: no bounds checking?
	if (res >= 0) {
		mpz_add(*result->value1, *var->value1, *params->value1);
	}
	return res;
}

/**
 * Write the result of adding one element to another in an additive group
 *
 * @type: Additive group
 * @result: Stores result
 * @var: Left hand element of addition
 * @param: Right hand element of addition
 */

int zplus_add(PARAMS) {
	mpz_t tmp;
	int res = 0;
	MSG_DEBUG(5, fprintf(stderr, "math:op: zplus_add(): executing\n"););

	IFNOERROR(check_cace_int_init(result););
	IFNOERROR(check_cace_int_init(var););
	IFNOERROR(check_cace_int_init(params););

	if (res >= 0) {
		mpz_init_set_ui(tmp,0);
		mpz_add(tmp, *var->value1, *params->value1);
		mpz_mod(*result->value1, tmp, *var->group->params->param->value1);
		mpz_clear(tmp);
	}
	return res;
}



/**
 * (result) = |(var)|
 */
int z_abs(PARAMS) {
	int res = 0;
	MSG_DEBUG(5, fprintf(stderr,"math:op: z_abs(): executing\n"););

	IFNOERROR(check_cace_int_init(result););
	IFNOERROR(check_cace_int_init(var););
	IFNOERROR(check_cace_int_init(params););

	if (res >= 0) {
		mpz_abs(*result->value1, *var->value1);
	}
	return res;
}
/**
 * (result) = sqrt (var)
 */
int z_sqrt(PARAMS) {
	int res = 0;
	MSG_DEBUG(5, fprintf(stderr, "math:op: z_sqrt(): executing\n"););

	IFNOERROR(check_cace_int_init(result););
	IFNOERROR(check_cace_int_init(var););

	if (res >= 0) {
		mpz_sqrt(*result->value1, *var->value1);
	}
	return res;
}


/**
 * Write the inverse of a given element to result
 *
 * @type: Additive group
 * @result: Stores result
 * @var: the element to invert
 * @param: not used
 *
 */
int zplus_inv(PARAMS) {
	int res = 0;
	MSG_DEBUG(5, fprintf(stderr, "math:op: zplus_inv(): executing\n"););

	IFNOERROR(check_cace_int_init(result););
	IFNOERROR(check_cace_int_init(var););

	if (res >= 0) {
		mpz_neg(*result->value1, *var->value1);
		mpz_mod(*result->value1, *result->value1,
				*var->group->params->param->value1);
	}
	return res;
}

/**
 * Write the inverse of a given element to result.
 *
 * @type: Multiplicative group
 * @result: Stores result
 * @var: the element to invert
 * @param: not used
 *
 */
int zmult_inv(PARAMS) {
	int res = 0;
	MSG_DEBUG(5, fprintf(stderr, "math:op: zmult_inv(): executing\n"););

	IFNOERROR(check_cace_int_init(result););
	IFNOERROR(check_cace_int_init(var););

	if (res >= 0) {
		mpz_invert(*result->value1, *var->value1,
				*var->group->params->param->value1);
	}
	return res;
}

/**
 * Write the inverse of a given element to result
 *
 * @type: Z
 * @result: Stores result
 * @var: the element to invert
 * @param: not used
 *
 */
int z_inv(PARAMS) {
	int res = 0;
	MSG_DEBUG(5, fprintf(stderr, "math:op: z_inv(): executing\n"););

	IFNOERROR(check_cace_int_init(result););
	IFNOERROR(check_cace_int_init(var););

	if (res >= 0) {
		mpz_neg(*result->value1, *var->value1);
	}
	return res;
}


/**
 * Returns -99. Used to catch calls to unknown operations.
 *
 * @type: any
 * @result: not used
 * @var: not used
 * @param: not used
 *
 */
int unknown(PARAMS) {
	MSG_DEBUG(5, fprintf(stderr, "math:op: unknown(): executing\n"););
	MSG_ERROR(fprintf(stderr, "math:op: unknown(): Unknown operation called\n");)
	cace_set_error(ERROR_UNKNOWN_OPERATION, "The requested operation has no implementation assigned for the given type");
	return -1;
}



/**
 * Returns 0. Used to catch calls to operations not implemented, but not used
 * for proper function
 *
 * @type: any
 * @result: not used
 * @var: not used
 * @param: not used
 *
 */
int generic(PARAMS) {
	MSG_DEBUG(5, fprintf(stderr, "math:op: generic(): executing\n"););
	MSG_WARNING(fprintf(stderr, "math:op: generic(): The empty catch all function was called\n");)
	return 0;
}



/**
 * The static table, mapping groups and operation id's to their implementation
 * (function pointer)
 */
struct cace_op prime_operations[] =
{
		{OP_CHECK_PARAMS, generic },
		{CHECK_MEMB, prime_checkmemb},
		{ADD, z_add },
		{MULT, z_mult },
		{POW, z_pow },
		{INV, z_inv},
		{VERIFY_EQUALS, all_verifyequals},
		{GET_BITS, all_getbits},
		{COUNT, all_count},
};
struct cace_op interv_operations[] =
{
		{OP_CHECK_PARAMS, generic },
		{CHECK_MEMB, interv_checkmemb },
		{ADD, interv_add },
		{MULT, interv_mult },
		{POW, z_pow },
		{RANDOM, interv_random },
		{VERIFY_EQUALS, all_verifyequals},
		{GET_BITS, all_getbits},
		{COUNT, all_count},
};


struct cace_op z_operations[] =
{
		{OP_CHECK_PARAMS, generic },
		{CHECK_MEMB, z_checkmemb},
		{ADD, z_add },
		{MULT, z_mult },
		{POW, z_pow },
		{INV, z_inv },
		{RANDOM, z_random },
		{VERIFY_EQUALS, all_verifyequals},
		{GET_BITS, all_getbits},
		{COUNT, all_count},
		{COMPLETE_OR, z_or_complete},
		{COMPLETE_SHAMIR, all_shamir_complete},

};

struct cace_op zplus_operations[] =
{
		{OP_CHECK_PARAMS, generic },
		{CHECK_MEMB, zplus_checkmemb},
		{ADD, zplus_add },
		{MULT, zplus_mult },
		{INV, zplus_inv},
		{RANDOM, zplus_random },
		{VERIFY_EQUALS, all_verifyequals},
		{GET_BITS, all_getbits},
		{GET_BITS, all_getbits},
		{COUNT, all_count},
		{AND, all_and},
		{OR, all_or},
		{TH, all_th},
};

struct cace_op zmult_operations[] =
{
		{OP_CHECK_PARAMS, generic },
		{CHECK_MEMB, zmult_checkmemb },
		{POW, zmult_pow},
		{MULT, zmult_mult },
		{INV, zmult_inv },
		{RANDOM, zmult_random },
		{VERIFY_EQUALS, all_verifyequals},
		{GET_BITS, all_getbits},
		{COUNT, all_count},
		{MPOW, zmult_mpow},
};

struct cace_type_op prime_global = { PRIME, ARRAY_SIZE(prime_operations), prime_operations, NULL };
struct cace_type_op interv_global = { INTERV, ARRAY_SIZE(interv_operations), interv_operations, &prime_global };
struct cace_type_op zmult_global = { ZMULT, ARRAY_SIZE(zmult_operations), zmult_operations, &interv_global };
struct cace_type_op zplus_global = { ZPLUS, ARRAY_SIZE(zplus_operations), zplus_operations, &zmult_global };
//ROOT OF STRUCTURE:
struct cace_type_op z_global = { Z, ARRAY_SIZE(z_operations), z_operations, &zplus_global };
//struct global_op operations = { EC, ec_operations, &zmult_global };


/*
 * UNTESTED
 * MEMORY MANAGEMENT (create type structure)
 * ERROR MANAGEMENT
 */
int cace_register_type(struct cace_type_op *type_op) {
	struct cace_type_op *tmp;
	tmp = &z_global;
	while (tmp->next != NULL) {
		tmp = tmp->next;
	}
	tmp->next = type_op;
	return 0;
}

/*********************************************************************
 *  The following functions all care about handling cace_int numbers
 *  - printing
 *  - serializing
 *  - assigning
 *  - size calculation
 *  - calculation
 *
 *  This is the interface for the autmatically generated application flow
 *  to the math library, such that no explicit call to the library is necessary.
 *
 *********************************************************************/


/**
 * Method to print a value from a group element, used for debugging
 */
int cace_int_print(struct cace_int *element) {
	gmp_fprintf(stderr, "%Zd",*element->value1);
	return 0;
}

/**
 * Method to print a value from a group element, used for debugging
 */
int cace_int_print_err(struct cace_int *element) {
	gmp_fprintf(stderr,"%Zd",*element->value1);
	return 0;
}

/**
 * compare a sortkey with a group_operation, sorting depending on
 * group as well as operation
 */
/*
int op_cmp (const int *key, const struct cace_type_op *group_op){
  return (*key - (group_op->operation + group_op->group));
}
*/
int op_cmp (const int *key, const struct cace_op *type_ops){
	return (*key - (type_ops->operation));
}



/**
 * Lookup an operation implementation from the static table by a group
 * and operation id
 */
int (*lookup_operation(int op_code, struct cace_type *target_type))
	(struct cace_int *result, struct cace_int *var, struct cace_int *params) {
	struct cace_type_op *current_type;
	struct cace_op *opp = NULL;
	size_t op_size = sizeof(struct cace_op);
	size_t count;

	current_type = &z_global;
	while (current_type != NULL && current_type->type != target_type->type) {
		current_type = current_type->next;
	}

	if (current_type != NULL) {
		count = current_type->op_len;
		opp=lfind(&op_code, current_type->operations, &count, op_size,
			  (int (*)(const void *, const void *)) op_cmp);
		//TODO: can target_type->type be of other than allowed type?
		MSG_DEBUG(4,fprintf(stderr, "math: lookup_op(): type: %s, operation: %s\n", get_type(target_type->type),get_op(op_code));)
	}
	if (opp == NULL) {
		MSG_ERROR(fprintf(stderr, "math: lookup_op(): requested operation %d of type %d not found\n", op_code, target_type->type);)

		// Operation was not found, return catch unknown operation
		return unknown;
  }

  return opp->operate;
}


int cace_calc(struct cace_int *result, struct cace_type *group, int op_code,
		struct cace_int *op_params) {
	int res = 0;
	int set_params = 0;
	struct cace_int *tmp;
	int (*operation)(PARAMS);

	MSG_DEBUG(6, fprintf(stderr,"math: cace_calc(): executing:\n");)

	if (group == NULL) {
		cace_set_error(ERROR_NULL_POINTER, "Given group for calculation is a null pointer");
		MSG_ERROR(fprintf(stderr, "math: cace_calc(): group parameter is null");)
		res = -1;
	}
	if (res >= 0 && op_params == NULL) {
		MSG_DEBUG(6, fprintf(stderr, "math: cace_calc(): reset op_params\n"););
		tmp = malloc(sizeof(struct cace_int));
		if (tmp == NULL) {
			cace_set_error(ERROR_OUT_OF_MEMORY, "Out of memory in cace_calc()");
			// Return here, since further computation makes no sense and
			// no ressource needs to be freed in this function.
			return -1;
		}
		tmp->initialized = 0;
		cace_int_init(tmp,group);
		set_params = 1;
	} else if (res >= 0) {
		tmp = op_params;
	}


	IFNOERRORX(operation = lookup_operation(op_code, group););
	if (res == 0 && operation == NULL) {
		cace_set_error(ERROR_NULL_POINTER, "Received null pointer when looking up operation");
		MSG_ERROR(fprintf(stderr, "math: cace_calc(): operation received is null");)
		res = -1;
	}

	IFNOERROR(operation(result, tmp, NULL););
	result->known = 1;
	MSG_DEBUG(5,
		fprintf(stderr, "math: cace_calc(): operation resulted in code %d: \n",res);
		if (result != NULL) {
			gmp_fprintf (stderr, "\tresult (%d) = %Zd; \n",result->group->type,*result->value1);
		};
		)

	if (res < 0) {
		MSG_WARNING(
			if (group != NULL) {
				fprintf(stderr, "math: cace_calc(): operation %s of type %s failed with code %d: \n",get_op(op_code), get_type(group->type), res);
				if (result != NULL) {
					gmp_fprintf (stderr, "\tresult (%d) = %Zd; \n",result->group->type,*result->value1);
				}
				gmp_fprintf (stderr, "\tparam[1] (%d) = %Zd\n; ",tmp->group->type,*tmp->value1);
			}
		)
	}

	// Clearing up
	if (set_params == 1) {
		mpz_clear(*tmp->value1);
		tmp->value1 = NULL;
		free(tmp);
	}
	return res;
}


/**
 * Execute an operation on an element given by its code, the input, output and parameter element
 */
int cace_calc_element(struct cace_int *result, struct cace_int *var, int op_code, struct cace_int *op_params) {
	int res = 0;
	struct cace_type *g = NULL;
	int (*operation)(PARAMS);

	MSG_DEBUG(6, fprintf(stderr, "math: cace_calc_element(): executing:\n");)

	if (var != NULL) {
		g = var->group;
	} else if (op_params != NULL) {
		g = op_params->group;
	} else {
		cace_set_error(ERROR_NULL_POINTER, "Both group defining parameters of cace_calc_element() are null pointers. Cannot derive group");
		MSG_ERROR(fprintf(stderr, "math: cace_calc_element(): op_params & var is null. cannot derive group\n");)
		res = -1;
	}
	MSG_DEBUG(6,
		if (var != NULL) {
			gmp_printf ("\tvar (%s) = %Zd; \n",get_type(var->group->type),*var->value1);
		}
		if (op_params != NULL) {
			gmp_printf ("\tparam[0] (%s) = %Zd; \n",get_type(op_params->group->type),*op_params->value1);
		}
	);
	//call operation
	if (res == 0) {
		operation = lookup_operation(op_code, g);
		if (operation == NULL) {
			res = -96;
		}
	}
	if (res == 0 && result != NULL && g != NULL) {
		cace_int_init(result, g);
		result->known = 1;
	}

	IFNOERROR(operation(result, var, op_params););
	MSG_DEBUG(5,
		fprintf(stderr, "math: cace_calc_element(): operation resulted in code %d: \n",res);
		if (result != NULL) {
			gmp_fprintf (stderr,"\tresult (%d) = %Zd; \n",result->group->type,*result->value1);
		}
	);
	if (res < 0) {
		MSG_WARNING(
			if (g != NULL) {
				fprintf(stderr, "math: cace_calc_element() operation %s of type %s failed with code %d: \n",get_op(op_code), get_type(g->type), res);
			}
			if (var != NULL) {
				gmp_fprintf (stderr, "\tvar (%d) = %Zd; \n",var->group->type,*var->value1);
			}
			if (op_params != NULL) {
				gmp_fprintf (stderr, "\tparam[1] (%d) = %Zd\n",op_params->group->type,*op_params->value1);
			}
			if (result != NULL) {
				gmp_fprintf (stderr, "\tresult (%d) = %Zd;\n",result->group->type,*result->value1);
			}
		)
	}
	if (op_params != NULL) {
		cace_int_detach(op_params);
	}

	return res;
}

/**
 * Assign a value-param to a type
 */
int cace_type_assign_param_value(struct cace_type *group, char * value) {
	int res = 0;
	int (*operation)(PARAMS);
	struct cace_type_param *tmp;
	struct cace_int *element;
	int pos = -1;
	MSG_DEBUG(5,fprintf(stderr, "math: cace_type_assign_param_value(): assigning value %s\n",value);)

	if (group->param_due_count == 0) {
		res = -1;
		MSG_ERROR(fprintf(stderr, "math: cace_type_assign_param_value(): param due count for group %s reached,"
				"still trying to add additional parameter.\n", get_type(group->type)););
		cace_set_error(ERROR_ATTRIBUTE_ERROR,"Maximum attribute count of a group reached while still adding more attribues");
	} else {
		if (group->params != NULL) {
			MSG_DEBUG(6, fprintf(stderr, "math: cace_type_assign_param_value(): add param to end of linked list\n"););
			group->param_due_count--;

			tmp = group->params;
			while (tmp->next != NULL && pos++ >= 0) {
				tmp = tmp->next;
			}
			tmp->next = malloc(sizeof(struct cace_type_param));
			if (tmp->next == NULL) {
				cace_set_error(ERROR_OUT_OF_MEMORY, "Out of memory in cace_type_assign_param_value()");
				return -1;
			}
			tmp = tmp->next;

			element = malloc(sizeof(struct cace_int));
			if (element == NULL) {
				free (tmp);
				cace_set_error(ERROR_OUT_OF_MEMORY, "Out of memory in cace_type_assign_param_value()");
				return -1;
			}

			element->value1 = malloc(sizeof(mpz_t));
			if (element->value1 == NULL) {
				free(tmp);
				free(element);
				cace_set_error(ERROR_OUT_OF_MEMORY, "Out of memory in cace_type_assign_param_value()");
				return -1;
			}
			element->value2 = NULL;
			mpz_init_set_str(*element->value1,value,10);

			operation = lookup_operation(OP_CHECK_PARAMS, group);
			if (operation == NULL || operation(NULL, NULL, element) != 0) {
				MSG_ERROR(fprintf(stderr, "math: cace_type_assign_param_value(): param check failed\n"););
				cace_set_error(ERROR_DYNAMIC_TYPECHECKING, "Parameter check failed while assigning to a group");
				res = -1;
			}
			tmp->param = element;
			tmp->pos = pos;
			tmp->next = NULL;

		} else {
			MSG_DEBUG(6, fprintf(stderr, "math: cace_type_assign_param_value(): add param to beginning of linked list"););
			group->param_due_count--;
			element = malloc(sizeof(struct cace_int));
			if (element == NULL) {
				cace_set_error(ERROR_OUT_OF_MEMORY, "Out of memory in cace_type_assign_param_value()");
				return -1;
			}
			element->value1 = malloc(sizeof(mpz_t));
			if (element->value1 == NULL) {
				free(element);
				cace_set_error(ERROR_OUT_OF_MEMORY, "Out of memory in cace_type_assign_param_value()");
				return -1;
			}
			element->value2 = NULL;

			mpz_init_set_str(*element->value1,value,10);
			group->params = malloc(sizeof(struct cace_type_param));
			if (group->params == NULL) {
				free(element->value1);
				free(element);
				cace_set_error(ERROR_OUT_OF_MEMORY, "Out of memory in cace_type_assign_param_value()");
				return -1;
			}
			operation = lookup_operation(OP_CHECK_PARAMS, group);
			if (operation == NULL || operation(NULL, NULL, element) != 0) {
				MSG_ERROR(fprintf(stderr, "math: cace_type_assign_param_value(): param check failed\n"););
				cace_set_error(ERROR_DYNAMIC_TYPECHECKING, "Parameter check failed while assigning to a group");
				res = -1;
			}
			group->params->param = element;
			group->params->pos = ++pos;
			group->params->next = NULL;
		}
	}
	return res;
}

/**
 * Assign a pre set group element as a param to a group
 */
int cace_type_assign_param(struct cace_type *group, struct cace_int *element) {
	int res = 0;
	int (*operation)(PARAMS);
	struct cace_type_param *tmp;
	int pos = -1;

	MSG_DEBUG(5,gmp_fprintf(stderr, "math: cace_type_assign_param(): assigning value %Zd\n",element->value1);)

	// Check if we got all parameters
	if (group->param_due_count == 0) {
		res = -1;
		MSG_ERROR(fprintf(stderr, "math: cace_type_assign_param(): param due count for group %s reached,"
				"still trying to add additional parameter. ", get_type(group->type)););
		cace_set_error(ERROR_ATTRIBUTE_ERROR,"Maximum attribute count of a group reached while still adding more attribues");
	} else {
		operation = lookup_operation(OP_CHECK_PARAMS, group);
		if (operation == NULL || operation(NULL, NULL, element) != 0) {
			MSG_ERROR(fprintf(stderr, "math: cace_type_assign_param(): param check failed\n"););
			cace_set_error(ERROR_DYNAMIC_TYPECHECKING, "Parameter check failed while assigning to a group");
			res = -1;
		} else 	if (group->params != NULL) {
			MSG_DEBUG(6, fprintf(stderr, "math: cace_type_assign_param(): add param to end of linked list"););

			group->param_due_count--;
			tmp = group->params;
			while (tmp->next != NULL && pos++ >= 0) {
				tmp = tmp->next;
			}
			tmp->next = malloc(sizeof(struct cace_type_param));
			if (tmp->next == NULL) {
				cace_set_error(ERROR_OUT_OF_MEMORY, "Out of memory in cace_type_assign_param()");
				return -1;
			}
			tmp = tmp->next;

			tmp->param = element;
			tmp->pos = pos;
			tmp->next = NULL;

		} else {
			MSG_DEBUG(6, fprintf(stderr, "math: cace_type_assign_param(): add param to beginning of linked list"););
			group->param_due_count--;
			group->params = malloc(sizeof(struct cace_type_param));
			if (group->params == NULL) {
				cace_set_error(ERROR_OUT_OF_MEMORY, "Out of memory in cace_type_assign_param()");
				return -1;
			}
			group->params->param = element;
			group->params->pos = ++pos;
			group->params->next = NULL;
		}
	}
	return res;

}

/**
 * Initilize a type
 */
int cace_type_init(struct cace_type* type_out, int type, int bitlen, int param_count) {
	int res = 0;
	MSG_DEBUG(5, fprintf(stderr, "math: cace_type_int(): init type %s\n",get_type(type)););

	type_out->type = type;
	type_out->bitlen = bitlen;
	type_out->param_due_count = param_count;
	type_out->params = NULL;

	return res;
}

/**
 * Initilize an element
 */
int cace_int_init_multiple(struct cace_int *element, struct cace_type *type, int count_values) {
	int res = 0;
	MSG_DEBUG(5, fprintf(stderr, "math: cace_int_init(): init cace_int of type %s\n",get_type(type->type)););

	if (element->initialized != 1) {
		element->value1 = malloc(sizeof(mpz_t));
		if (element->value1 == NULL) {
			cace_set_error(ERROR_OUT_OF_MEMORY, "Out of memory in cace_int_init()");
			return -1;
		} else {
			mpz_init_set_ui(*element->value1,0);
			element->next = NULL;
			element->initialized = 1;
			element->known = 0;
			element->value2 = NULL;
		}
		if (count_values == 2) {
			element->value2 = malloc(sizeof(mpz_t));
			if (element->value1 == NULL) {
				cace_set_error(ERROR_OUT_OF_MEMORY, "Out of memory in cace_int_init()");
				return -1;
			} else  {
				mpz_init_set_ui(*element->value2,0);
			}
		} else if (count_values > 2) {
			cace_set_error(ERROR_TOO_MANY_VALUES, "Too many values. Cannot initialize cace_int with more than two values");
			MSG_ERROR(fprintf(stderr, "Too many values. Cannot initialize cace_int with more than two values. Chosen number is %d\n",count_values);)
			res = -1;
		}
	}
	element->group = type;
	return res;
}

int cace_int_init(struct cace_int *element, struct cace_type *type) {
	return cace_int_init_multiple(element, type, 1);
}



/**
 * Assign a value to an element.
 */
int cace_int_assign_base_pos(struct cace_int *element, char* value, int not_null, int base, int pos) {
	int res = 0;
	MSG_DEBUG(5, fprintf(stderr, "math: cace_int_assign(): value %s\n",value););

	if (value[0] != 'N') {
		if (pos == 2) {
			mpz_set_str(*element->value2, value, base);
		} else {
			mpz_set_str(*element->value1, value, base);
		}
		res = 0;
		element->known = 1;
	} else if (not_null == 1) {
		// Only at initialization time, null values are allowed, in any other
		// situation this shall abort the protocol
		cace_set_error(ERROR_ASSIGNING_ZERO, "Trying to assign a zero value to an elment not allowing zero values");
		MSG_ERROR(fprintf(stderr, "math: cace_int_assign(): illegal try to asssign a zero value\n");)
		res = -1;
	}
	return res;
}

int cace_int_assign(struct cace_int *element, char* value, int not_null) {
	return cace_int_assign_base_pos(element,value,not_null, 10, 1);
}

/**
 * Assign an integer to an element.
 */
int cace_int_assign_int(struct cace_int *element, int value) {
	MSG_DEBUG(5, fprintf(stderr, "math: cace_int_assign_int() with value %d\n",value););

	mpz_set_ui(*element->value1, value);
	element->known = 1;
	return 0;
}

/**
 * Attach a element to another element, making them a singly linked list.
 */
int cace_int_attach(struct cace_int *out, struct cace_int *in) {
	MSG_DEBUG(6, fprintf(stderr, "math: cace_int_attach() execute\n"););
	out->next = in;
	return 0;
}

/**
 * Detach all following elements from each other (resolvig singly linked list
 * to single elements).
 */
int cace_int_detach(struct cace_int *element) {
	MSG_DEBUG(5, fprintf(stderr, "math: cace_int_detach(): execute\n"););
	if (element->next != NULL) {
		cace_int_detach(element->next);
	}
	element->next = NULL;
	return 0;
}

/**
 * Serialize element and store in given string (send), the string needs to be
 * allocated the a right sized buffer.
 */
int cace_int_serialize(char *send, struct cace_int *element) {
	int tmp;
	char *tmpc;
	MSG_DEBUG(6,fprintf(stderr, "math: cace_int_serialize(): execute\n");)

	if (element->value2 == NULL) {
		//printf("serialize 1\n");
		send = mpz_get_str(send, 10, *element->value1);
	} else {
		//printf("serialize 1+2\n");
		send = mpz_get_str(send, 10, *element->value1);
		tmp = mpz_sizeinbase(*element->value1,10)-1;
		send[tmp] = ':';
		tmpc = send+tmp+1;
		tmpc = mpz_get_str(tmpc, 10, *element->value2);
	}

	return 0;
}

/**
 * Calculate the decimal size of a element
 */
int cace_int_size(struct cace_int *element) {
	int res = 0;
	MSG_DEBUG(7,fprintf(stderr, "math: cace_int_size(): execute\n");)

	//fprintf(stderr,"!!!!!!!!!value2: %X %X\n", element->value2, *element->value2);
	if (element->value2 == NULL) {
		res = mpz_sizeinbase(*element->value1,10);
	} else {
		res = mpz_sizeinbase(*element->value1,10);
		res += mpz_sizeinbase(*element->value2,10);
		// Add a character for the delimiter
		res += 1;
	}
	return res;
}

/**
 * To allow binary operations with the return value given as parameter like
 * w = (x+y) * (z+w), the intermediate results have to be stored efficiently
 * For this a stack is used:
 * 	t = x+y; push(t);
 * 	t = z+w; push(t);
 * 	pop(t2); pop(t1);
 * 	w = t1 * t2
 */

/**
 * Push an element onto the stack, return -3 on stack overrun
 */
int cace_int_push(struct cace_int *element) {
	int res = 0;
	MSG_DEBUG(6, fprintf(stderr, "math: cace_int_push(): execute\n"););

	struct cace_int *tmp;
	if (_stacksize == MAX_STACK_SIZE) {
		res = -3;
	} else if (_stackmax-1 <= _stacksize) {
		tmp = malloc(sizeof(struct cace_int));
		if (tmp == NULL) {
			cace_set_error(ERROR_OUT_OF_MEMORY, "Out of memory in config:read_inputs()");
			res = -1;
		} else {
			tmp->initialized = 0;
			_stackmax++;
		}
	} else {
		tmp = _stack[_stacksize + 1];
	}
	IFNOERROR(cace_int_init(tmp,element->group););
	IFNOERRORX(
			mpz_set(*tmp->value1,*element->value1);
			tmp->group = element->group;
			tmp->initialized = element->initialized;
			_stack[++_stacksize] = tmp;
	);
	return res;
}
/**
 * Pop an element from the stack, return -3 if stack is empty
 */
int cace_int_pop(struct cace_int *result) {
	MSG_DEBUG(6, fprintf(stderr, "math: cace_int_pop(): execute\n"););

	struct cace_int *tmp;
	if (_stacksize >= 0) {
		tmp = _stack[_stacksize--];
		mpz_set(*result->value1,*tmp->value1);
		result->group = tmp->group;
		result->initialized = tmp->initialized;
		return 0;
	}
	return -3;
}

int cace_int_stack_deinit() {
	MSG_DEBUG(6, fprintf(stderr, "math: cace_int_stack_deinit(): execute\n");)
	int i;
	for (i = _stackmax-1; i >= 0; i--) {
		mpz_clear(*_stack[i]->value1);
		free(_stack[i]->value1);
		_stack[i]->value1 = NULL;
		free(_stack[i]);
		_stack[i] = NULL;
	}
	return 0;
}

/**
 * Calculate the maximal decimal size an element of the passed array of elements
 * need. This function is necessary to know how much memory have to be
 * allocated for the send string when serializing a set of elements.
 */
int cace_int_max_size(struct cace_int *common_var[], int size) {
	int res = 0;
	int tmp;
	int i;
	for (i = 0; i < size; i++) {
		MSG_DEBUG(7, fprintf(stderr, "math: cace_int_max_size(): element max size: %d (res: %d)\n",size, res););

		if (common_var[i]->initialized == 1) {
			if ((tmp = cace_int_size(common_var[i])) > res) {
				res = tmp;
			}
			if (common_var[i]->group->params != NULL &&
					(tmp = cace_int_size(common_var[i]->group->params->param)) > res) {
				res = tmp;
			}
		}
	}
	if (res > 0) {
		// Since the result is used as buffer initialization size, add one
		// for the null terminator
		res++;
	}
	return res;
}
