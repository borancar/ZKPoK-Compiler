/*
 * Zero knowledge proof of knowledge C application framework
 *
 *  # Reporting and error handling module #
 *
 *
 * This modules is responsible to return information practical for debug
 * messages, store error codes and messages, print status reports and handle
 * signals.
 *
 * #######################################################################
 *
 * This file contains all functions needed in this module
 *
 * #######################################################################
 *
 * This is part of the CACE (www.cace-project.eu) C-back-end of their zero
 * knowledge proof of knowledge compiler framework
 *
 * License: GNU GPLv2 (http://www.gnu.org/licenses/gpl-2.0.html)
 *
 *  Created on: 03.10.2009
 *      Author: Andreas Gruenert
 */


#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

//TODO: describe / change to status instead of error
int error_count = 0;
char *cace_errmsg = NULL;
int cace_errno = 0;

/**
 * Array with all the standard dynamic types used for the supported protocols.
 * The types are stored in exactly the same order as the /enum types/.
 * To have the array index beginning at 1 (like the enum), the first element
 * is kept intentionally empty.
 */
char *arr_types[] = { "", "Zmod*","Zmod+", "Interval", "Prime", "Z" };
/**
 * Array with all the standard operations supported. The operations are stored
 * exactly in the same order as the /enum operations/.
 * To have the array index beginning at 1 (like the enum), the first element
 * is kept intentionally empty.
 */
char *arr_op[] = { "", "OP_CHECK_PARAMS", "IS_ORDER", "RANDOM", "POW", "MULT",
	"ADD", "IS_PRIME", "VERIFY_EQUALS", "CHECK_MEMB", "INV", "GET_BITS",
	"AND", "OR", "TH", "COMPLETE_OR", "HASH", "ISORDER", "ASSERT_TRUE",
	"ASSERT_FALSE", "ASSERT_EQ", "ASSERT_LE", "ASSERT_GE", "ASSERT_NE",
	"ASSERT_L", "ASSERT_G", "MPOW", "COMPLETE_SHAMIR", "COUNT" };

/**
 * Storing the address to access  global options and inputs
 */
struct input_var *inputs;
struct input_var *options;

/**
 * Initialization of this module by associating the beginning of the linked list
 * of the global options and inputs read from the configuration file. This
 * should be done before using the other functions since they might (in the
 * future) make use of global options or inputs.
 *
 * @return (int): always 0 (success)
 */
int init_reporting(struct input_var *file_inputs, struct input_var *file_options) {
	inputs = file_inputs;

	//printf("reporting inputs: %X\n",(unsigned long)inputs);
	options = file_options;
	return 0;
}

/**
 * Returns a string with the name corresponding the type id. If the type-id is
 * unknown it is searched if any extension exist with this type (read from
 * the global options). Else the integer value will be translated to a string
 * and returned.
 *
 * @param (int t): A type id
 * @return (char*): A string representation of the type id
 */
char *get_type(int t) {
	char *ret_constructed[32];
	char *option_value;
	char res = 0;

	if (t < ARRAY_SIZE(arr_types)) {
			return arr_types[t];
	} else if (t < 10000) {
		sprintf(*ret_constructed, "type:%d",t);
		if ((res = get_var(&option_value, options, *ret_constructed)) == 0) {
			return option_value;
		} else {
			sprintf(*ret_constructed, "ext-type: %d",t);
			return *ret_constructed;
		}
	} else {
		return "";
	}
}

/**
 * Returns a string with the name of the operation represented by the given
 * operation id, or unknown
 *
 * @param (int op): An operation id
 * @return (char*): A string representation of the operation id
 */
char *get_op(int op) {
	return (op < ARRAY_SIZE(arr_op))  ? arr_op[op] : "Unknown";
}

/**
 * Generic error setting function. Only the first stored error is stored,
 * subsequent ones are ignored.
 *
 * @param (int errno): The status or error number
 * @param (char *message): The status or error message
 */
void cace_set_error(int errno, char *message) {
	if (error_count == 0) {
		cace_errmsg = message;
		cace_errno = errno;
		error_count++;
	}
}

/**
 * Getter function for cace_errno
 */
int cace_get_errno() {
	return cace_errno;
}

/**
 * Getter function for cace_errmsg
 */
char *cace_get_errmsg() {
	if (cace_errmsg == NULL)
		return "";
	return cace_errmsg;
}

/**
 * Prints status reports according to the stored error number and message.
 * The errors are divided into
 * - Verification failures
 * - Protocol errors (e.g. segfaults)
 * - Communication errors (e.g. broken connection)
 * - Internal errors (e.g. out of memory)
 */
void cace_print_error() {
	const char *line_msg = "\n-----------------------------------------------------------------------------\n";
	const char *report_msg = "\nIf you think this is a bug please don't "
			"hesitate to send\nthis case to the developers. Please include the "
			"detailed log output\n(at level 6), verifier.c, prover.c and if "
			"available the *.pil or *.psl\nfiles used to generate the protocol. "
			"Send all this to: 'andreas.gruenert@gmx.ch'\n\n";
	const char *raise_msg = "You can raise the log level with the option "
			"'main:verbose'\nin file 'test.config')\n";
	int have_message = 1;

	if (cace_errmsg == NULL) {
		cace_errmsg = "(no message)";
		have_message = 0;
	}

	if (cace_errno >= 0) {
        printf("\n\n>>  Application run successful  <<\n");
        if (have_message == 1) printf("Code: %d%s%s%s",cace_get_errno(), line_msg, cace_get_errmsg(), line_msg);
	} else if (cace_errno > -100) {
        printf("\n\n>>  Knowledge Verification failed  <<\n");
        printf("Code: %d%s%s%s",cace_get_errno(), line_msg, cace_get_errmsg(), line_msg);
		printf(report_msg);
		printf(raise_msg);
	} else if (cace_errno <= -100 && cace_errno > -200) {
        printf("\n\n>>  Protocol Error Occurred  <<\n");
        printf("Code: %d%s%s%s",cace_get_errno(), line_msg, cace_get_errmsg(), line_msg);
        printf("Something is wrong with the protocol. This should not happen. \n");
        printf(report_msg);
        printf(raise_msg);
	} else if (cace_errno <= -200 && cace_errno > -300) {
        printf("\n\n>>  Communication Error Occurred  <<\n");
        printf("Code: %d%s%s%s",cace_get_errno(), line_msg, cace_get_errmsg(), line_msg);
        printf(report_msg);
        printf(raise_msg);

	} else if (cace_errno <= -300 && cace_errno > -400) {
        printf("\n\n>>  Internal Error Occurred  <<\n");
        printf("Code: %d%s%s%s",cace_get_errno(), line_msg, cace_get_errmsg(), line_msg);
        printf("The program aborted due to an illegal state. This should not happen. \n");
        printf(report_msg);
        printf(raise_msg);
	}
}
/**
 * Reentrant signal handler. Called nondeterministically via lngjmp if any of
 * the defined signals occur:
 * - SIGSEGV: Segmentation fault: print error report and exit application. It is
 *            impossible to recover securely from this condition. It is also
 *            impossible to free the memory since we do not know were the
 *            application was left when entering this fault. It though should be
 *            traceable with the debug output.
 * - SIGPIPE: Broken pipe: This occurs when the peer client or server aborts the
 *            connection. The error can only occur when sending or receiving
 *            a message. Handing back control is therefore without risk, since
 *            the communication interface is designed to abort in this condition
 *            anyway. Thus the signal is ignored.
 */
void signal_handler(int signum) {
	if (signum == SIGSEGV) {
		cace_set_error(ERROR_SEGMENTATION_FAULT, "The execution was stopped by a segmentation fault");
		cace_print_error();
		exit(ERROR_SEGMENTATION_FAULT);
	} else if (signum == SIGPIPE) {
		cace_set_error(ERROR_BROKEN_CONNECTION, "Connection broke by termination of peer (SIGPIPE)");
	}
}
