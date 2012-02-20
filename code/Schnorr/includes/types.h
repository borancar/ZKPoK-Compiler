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
 * This file contains all type and operation definitions, data structures and
 * the function signatures of the math utility module interface.
 *
 * #######################################################################
 *
 * This is part of the CACE (www.cace-project.eu) C-back-end of their zero
 * knowledge proof of knowledge compiler framework
 *
 * License: GNU GPLv2 (http://www.gnu.org/licenses/gpl-2.0.html)
 *
 *  Created on: 01.03.2009
 *      Author: Andreas Gruenert
 */

#ifndef TYPES_H_
#define TYPES_H_

#include "comm.h"
#include "config.h"

#define NEXTTYPENUM 6
enum types { ZMULT=1, ZPLUS=2, INTERV=3, PRIME=4, Z=5 };
//extern enum addtype addtypes;
extern char *get_type(int t);
extern char *get_op(int op);


enum operations { OP_CHECK_PARAMS=1, IS_ORDER=2, RANDOM=3, POW=4, MULT=5,
	ADD=6, IS_PRIME=7, VERIFY_EQUALS=8, CHECK_MEMB=9, INV=10, GET_BITS=11,
	AND=12, OR=13, TH=14, COMPLETE_OR=15, HASH=16, ISORDER=17, ASSERT_TRUE=18,
	ASSERT_FALSE=19, ASSERT_EQ=20, ASSERT_LE=21, ASSERT_GE=22, ASSERT_NE=23,
	ASSERT_L=24, ASSERT_G=25, MPOW=26, COMPLETE_SHAMIR=27, COUNT=28 };



// Error message codes
/**
 * Verification / status messages (-1..-99)
 */
#define ERROR_VERIFICATION_FAILED -57
#define ERROR_KNOWLEDGE_BELOW_THRESHOLD -58
#define ERROR_NO_RANDOM_FOUND -59
/*

#define ERROR_WRONG_GROUP_FOR_OP -14
#define ERROR_NO_PARAMS -15
#define ERROR_WRONG_GROUP_FOR_PARAM -16
#define ERROR_WRONG_INPUT_VAR -17
#define ERROR_PIPE_NOT_EXIST -18
#define ERROR_OPEN_TIMOUT_EXCEEDED -19
*/
/**
 * Protocol errors (-100..-199)
 */
#define ERROR_CHECK_MEMBERSHIP -101
#define ERROR_DYNAMIC_TYPECHECKING -102
#define ERROR_GROUP_NOT_INITIALIZED -103
#define ERROR_GROUP_WRONGLY_INITIALIZED -104
#define ERROR_ELEMENT_NOT_INITIALIZED -105
#define ERROR_VAR_NOT_INITIALIZED -106
#define ERROR_ASSERTION_FAILED -107
#define ERROR_WRONG_TYPE -108
#define ERROR_ATTRIBUTE_ERROR -109
#define ERROR_UNKNOWN_OPERATION -110
#define ERROR_TOO_MANY_VALUES -111

/**
 * Communication errors (-200..-299)
 */
#define ERROR_OVERLONG_VALUE_IN_CONFIGFILE -201
#define ERROR_BROKEN_CONNECTION -202
#define ERROR_REQUESTED_INPUT_TOO_SHORT -211
#define ERROR_NO_INPUT -212
#define ERROR_REQUESTED_INPUT_TOO_LONG -213
#define ERROR_SERVER_CANNOT_BIND -214
#define ERROR_CONNECTION_TIMEOUT -215
#define ERROR_SERVER_CONNECTION_SETUP -216
#define ERROR_ASSIGNING_ZERO -217

/**
 * Internal errors (-300..-399)
 */
#define ERROR_OUT_OF_MEMORY -301
#define ERROR_SEGMENTATION_FAULT -302
#define ERROR_NULL_POINTER -303
#define ERROR_NOT_SUPPORTED -304
#define ERROR_NUMBER_OUT_OF_RANGE -305



// Math library types
#define MATH_GMP 1
#define MATH_MIRACL 2

/**
 * Macros
 */
//#define IFNOERROR(p) if (res >= 0)  res = p
//#define IFNOERRORX(p) if (res >= 0)  p
#define IFNOERROR(p) if (res >= 0)  { res = p }
#define IFNOERRORX(p) if (res >= 0)  { p }

#define ARRAY_SIZE(p) sizeof(p) / sizeof(p[0])


enum message_types { ERR=0, WRN=1, INF=2, DBG=3 };
#define QUIET 0
#if QUIET == 1
#define VERBOSE(p,q)

#define MSG_ERROR(p)
#define MSG_WARNING(p)
#define MSG_INFO(p)
#define MSG_DEBUG(q,p)

#else
#define VERBOSE(p,q) if (p < get_verbose()) { q }

#define MSG_ERROR(p)  if (ERR < get_verbose()) { fprintf(stderr, "(EE) "); p }
#define MSG_WARNING(p)  if (WRN < get_verbose()) { fprintf(stderr, "(WW) "); p }
#define MSG_INFO(p)  if (INF < get_verbose()) { fprintf(stderr, "(II) "); p }
#define MSG_DEBUG(q,p)  if (q < get_verbose()) { fprintf(stderr, "(DD) "); p }

#endif


#ifndef DEBUG
#define DEBUG 1
#endif

#define CHECK_PARAMS 1


struct cace_type_param {
	int pos;
	char name;
	struct cace_int *param;
	struct cace_type_param *next;
};


struct cace_type {
	char *name;
	int type;
	int bitlen;
	int param_due_count;
	struct cace_type_param *params;
};

struct point {
    mpz_t x;
    mpz_t y;
};

typedef mpz_t math_int;

struct cace_int {
	mpz_t *value1;
	mpz_t *value2;
	int initialized;
	int known;
	struct cace_type *group;
	struct cace_int *next;
};


struct cace_op {
  const int operation; //opcode
  int (* const operate)(struct cace_int *result, struct cace_int *var, struct cace_int *params);
};

struct cace_type_op {
  const int type; //opcode
  int op_len;
  struct cace_op *operations;
  struct cace_type_op *next;
};


// Function for finite field arithmetic:
extern void lagrange(mpz_t z, int size, struct point p[size], mpz_t request, mpz_t secret, int degree);

// Function for multiple exponentiation
extern void mexp(int k, mpz_t b[k], mpz_t e[k], mpz_t *m, mpz_t *r);

// Interace functions to the math utility library
extern int cace_type_init(struct cace_type* group_out, int type, int bitlen, int param_count);
extern int cace_calc_element(struct cace_int * result, struct cace_int *var, int op_code, struct cace_int *op_params);
extern int cace_calc(struct cace_int *result, struct cace_type *group, int op_code, struct cace_int *op_params);
extern int cace_type_assign_param(struct cace_type *group, struct cace_int *element);
extern int cace_type_assign_param_value(struct cace_type *group, char * value);
extern int cace_int_init(struct cace_int *element, struct cace_type *group);
extern int cace_int_init_multiple(struct cace_int *element, struct cace_type *type, int count_values);
extern int cace_int_serialize(char *send, struct cace_int *element);
extern int cace_int_assign(struct cace_int *element, char* value, int not_null);
extern int cace_int_assign_int(struct cace_int *element, int value);
extern int cace_int_assign_base_pos(struct cace_int *element, char* value, int not_null, int base, int pos);
extern int cace_int_size(struct cace_int *element);
extern int cace_int_push(struct cace_int *element);
extern int cace_int_pop(struct cace_int *result);
extern int cace_int_max_size(struct cace_int *common_var[], int size);
extern int cace_int_attach(struct cace_int *out, struct cace_int *in);
extern int cace_int_detach(struct cace_int *element);
extern int cace_int_print_err(struct cace_int *element);
extern int cace_int_print(struct cace_int *element);
extern int cace_int_stack_deinit();
extern int cace_register_type(struct cace_type_op *type_op);

extern int read_config_file();

extern void cace_set_error(int errno, char *message);
extern int cace_get_errno();
extern char *cace_get_errmsg();
extern void cace_print_error();
extern void signal_handler(int signum);
extern int init_reporting(struct input_var *file_inputs, struct input_var *file_options);
#endif /* TYPES_H_ */
