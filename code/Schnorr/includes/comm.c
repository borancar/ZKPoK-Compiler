/*
 * Zero knowledge proof of knowledge C application framework
 *
 *  # Communication utility module #
 *
 *
 * This modules is responsible for all interprocess communication
 * related functions.
 *
 * #######################################################################
 *
 * This file resembles is the abstract implementation of the
 * communication interface. It does not handle any request itself, but
 * delegates them to the chosen specific implementation.
 *
 * The communication interface is selected at compile time, only the requested
 * code is compiled into the application. Therefore the choice of implementation
 * is done through selective compile statements.
 *
 * #######################################################################
 *
 * This is part of the CACE (www.cace-project.eu) C-back-end of their zero
 * knowledge proof of knowledge compiler framework.
 *
 * Licence: GNU GPLv2 (http://www.gnu.org/licenses/gpl-2.0.html)
 *
 *
 *  Created on: 06.03.2009
 *      Author: Andreas Gruenert
 */

#include <stdio.h>

#include "comm.h"

int request_comm(struct input_request req[], int count, int force_comm) {
#if COMM == COMM_SOCKET
	return request_socket(req, count, force_comm);
#elif COMM == COMM_TERM
	return request_term(req, count, force_comm);
#endif
	return -3;
}

int send_comm(char *value,int size) {
#if COMM == COMM_SOCKET
	return send_through_socket(value,size);
#elif COMM == COMM_TERM
	return send_through_term(value,size);
#endif
	return -3;
}

int init_comm(struct input_var *file_inputs, struct input_var *file_options) {
#if COMM == COMM_SOCKET
	return socket_init(file_inputs, file_options);
#elif COMM == COMM_TERM
	return term_init(file_inputs, file_options);
#endif
	return -3;

}

int stop_comm() {
#if COMM == COMM_SOCKET
	return socket_stop();
#elif COMM == COMM_TERM
	return term_stop();
#endif
	return -3;
}
