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
 * This is the description of the communication interface (I/0)
 *
 * #######################################################################
 *
 * This is part of the CACE (www.cace-project.eu) C-back-end of their zero
 * knowledge proof of knowledge compiler framework.
 *
 * Licence: GNU GPLv2 (http://www.gnu.org/licenses/gpl-2.0.html)
 *
 *
 *  Created on: 04.03.2009
 *      Author: Andreas Gruenert
 */


#ifndef COMM_H_
#define COMM_H_

#include <gmp.h>
#include "types.h"
#include "config.h"

// Define the possible implementations
#define COMM_SOCKET 2
#define COMM_TERM   3

// Select the socket interface, if implementation is not chosen by compiler
#ifndef COMM
#define COMM COMM_TERM
#endif



struct input_request {
	char *name;
	struct cace_int *result;
};

extern int request_comm(struct input_request request[], int count, int force_com);
extern int send_comm(char *value, int size);
extern int init_comm(struct input_var *file_inputs, struct input_var *file_options);
extern int stop_comm();

extern int send_through_pipe(char *value, int size);
extern int request(struct input_request request[], int count);
extern int init_read_pipe();
extern int stop_read_pipe();

extern int send_through_socket(char *value, int size);
//extern int request_through_socket(char *line, struct input_request request, int inputsize);
extern int socket_stop();
extern int socket_init(struct input_var *file_inputs, struct input_var *file_options);
extern int request_socket(struct input_request request[], int count, int force_comm);

extern int request_term(struct input_request request[], int count, int force_comm);
extern int send_through_term(char *value, int size);
extern int term_init(struct input_var *file_inputs, struct input_var *file_options);
extern int term_stop();

#endif /* COMM_H_ */
