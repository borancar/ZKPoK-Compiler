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
 * Implementation of the communication interface via socket. Socket
 * communication is always of type client/server. Since neither of the
 * parties/processes communicating are client or server but just peers, the
 * association has to be dynamic.
 *
 * This implementation first tries to connect (via TCP) to a server on a
 * defined host with a defined IP. If no connection can be established, the
 * application will open the given on the own machine taking the role of the
 * server.
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
/**
 * Open Issues:
 * - Received characters are not tested to be a valid numeric value
 * - Only decimal values are correctly assigned, sending hexadecimal is
 *   not supported yet.
 */

#include <netinet/in.h>
#include <sys/socket.h>	/* for socket(), bind(), and connect() */
#include <arpa/inet.h>	/* for sockaddr_in and inet_ntoa() */
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <fcntl.h>

#include "comm.h"
#include "types.h"

// The port to connect to
#define PORT "31415"

// Max connections, this should be 1, since only two parties are talking
// with each other
#define MAXPENDING 1
// The IP of the machine to connect to. In the case of both processes
// running on the same machine, this is localhost.
#define HOSTNAME "127.0.0.1"
// When receiving a message there is only a maximum size of the value given
// in most cases though the value received will be much smaller.
// The STANDARD_LINESIZE defines the default size to be allocated to the storing
// variable. If the size exceeds it will the allocation will be extended by
// STANDARD_LINESIZE^2
#define STANDARD_LINESIZE 30

// We need to measure time to stop the server if no client tries to connecgt
struct timeval tv;
// Client address
struct sockaddr_storage remoteaddr;
struct addrinfo hints, *ai, *p;
char remoteIP[INET6_ADDRSTRLEN];

// Master file descriptor list
fd_set master;
// Temporary file descriptor list for select()
fd_set read_fds;

// Newly accept()ed socket descriptor
int newfd;
// Listening socket descriptor
int listener;
int nbytes;
int count = 0;
socklen_t addrlen;

// Storing the address to access  global options and inputs
struct input_var *inputs;
struct input_var *options;

// Get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*) sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*) sa)->sin6_addr);
}

/*  The kernel may decide not to send all the data out in one chunk.
 *  This function takes care that really everything is sent before continuing.
 */
int sendall(int s, char *buf, int len) {
	// How many bytes we've sent
	int total = 0;
	// How many we have left to send
	int bytesleft = len;
	int n = -1;

	while (total < len) {
		n = send(s, buf + total, bytesleft, 0);
		if (n == -1) {
			break;
		}
		total += n;
		bytesleft -= n;
	}

	// Return -1 on failure, 0 on success
	return (n == -1) ? -1 : 0;
}

/**
 * Send a given value through the open connection.
 */
int send_through_socket(char *value, int size) {
	int res = 0;
	MSG_DEBUG(4, fprintf(stderr,"comm: send %s of length %d through socket (%d)\n",value, (int)strlen(value), newfd););

	// Send the message
	if (sendall(newfd, value, strlen(value)) == -1) {
		MSG_ERROR(fprintf(stderr, "comm: error while sending value %s\n", value););
		res = -1;
		cace_set_error(ERROR_BROKEN_CONNECTION, "Error while sending data");
	}
	// Send a newline, to finish the message
	if (res == 0 && sendall(newfd, "\n", 1) == -1) {
		MSG_ERROR(fprintf(stderr, "comm: error while sending newline %s\n", value););
		res = -1;
		cace_set_error(ERROR_BROKEN_CONNECTION, "Error while sending newline");
	}
    return res;
}

/**
 * Request a value from the connected party, the value has to satisfy the
 * given (request) specification and may not except the (inputsize). The read
 * command is blocking and waits until it receives something, or the connection
 * is closed.
 *
 * The (line) buffer needs to be preallocated to the maximum length of the request
 */
int request_through_socket(struct input_request request, int inputsize) {
	int res = 0;
    int numbytes;
    int i = 0;
    int line_size = STANDARD_LINESIZE;
    char *tmp_line, *tmpx_line;

    tmp_line = malloc(line_size);
    if (tmp_line == NULL) {
    	res = -1;
    	cace_set_error(ERROR_OUT_OF_MEMORY,"Out of memory");
    	return res;
    }

    MSG_DEBUG(4,fprintf(stderr, "comm: trying to read (socket) %s (%d)\n", request.name, inputsize););

    i = 0;

    // Block-read one character, a time until a \n is received
    // or sent input is too long.
    do {

    	if (i >= line_size) {
			line_size *= 2;
			tmpx_line = realloc(tmp_line, line_size > inputsize ? inputsize : line_size);
			if (tmpx_line == NULL) {
		    	res = -1;
		    	cace_set_error(ERROR_OUT_OF_MEMORY,"Out of memory");
			} else {
				// This guarantees that the original block was freed if reallocation failed.
				tmp_line = tmpx_line;
				tmpx_line = NULL;
			}
		}

        if (res >= 0 && (numbytes = recv(newfd, &tmp_line[i], 1, 0)) == -1) {
			MSG_ERROR(fprintf(stderr, "comm: error while receiving a message\n"););
            res = -1;
        }

    } while(res >= 0 && tmp_line[i] != '\n' && i++ < inputsize);

    // Analyzing the result. Was the received value good or did an error occurr?
    if (res < 0) {
    	res = -1;
        cace_set_error(ERROR_BROKEN_CONNECTION, "Connection broke while receiving input");
    } else if (i == 0) {
    	MSG_ERROR(fprintf(stderr, "comm: no input for variable %s received\n",request.name););
    	res = -1;
    	cace_set_error(ERROR_NO_INPUT,"No input received for a variable.");
    } else if (inputsize <= i) {
    	MSG_ERROR(fprintf(stderr, "comm: request too long: size: %d/%d, last: %c\n",i, inputsize,tmp_line[i]););
		res = -1;
		//TODO...
		cace_set_error(ERROR_REQUESTED_INPUT_TOO_LONG,"An input received was too long");
    }

	// Add the string terminator
    if (tmp_line != NULL) {
    	tmp_line[i] = '\0';
    }

	if (res >= 0) {
		MSG_DEBUG(5, fprintf(stderr,"comm: received %s\n",tmp_line););
		cace_int_assign(request.result, tmp_line, 1);
	}

	// Cleaning up
	if (tmp_line != NULL) {
	  free(tmp_line);
	}
	tmp_line = NULL;

    return res;
}

/**
 * Close the connection
 */
int socket_stop() {
    close(newfd);
    return 0;
}


/**
 * Try to connect to a server
 */
int socket_init_client() {
    int rv;
    char *option_value;
    char s[INET6_ADDRSTRLEN];
    char *port = PORT, *hostname=HOSTNAME, res = 0;

 	if ((res = get_var(&option_value, options, "comm:port")) == 0) {
     	port = option_value;
    }

	if ((res = get_var(&option_value, options, "comm:target")) == 0) {
		hostname = option_value;
	}
	MSG_INFO(fprintf(stderr, "comm: selected port: %s\n",port););
	MSG_INFO(fprintf(stderr, "comm: selected host: %s\n",hostname););

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;



    if ((rv = getaddrinfo(hostname, port, &hints, &ai)) != 0) {
    	// No server found to connect -> return from function and let the
    	// init handler open a server socket him/herself instead.
    	// (Hybrid client/server model)
    	MSG_WARNING(fprintf(stderr,"comm: client peer could not connect: %s\n", gai_strerror(rv)););
        return -1;
    }

    // Loop through all the results and bind to the first available server
    for(p = ai; p != NULL; p = p->ai_next) {
        if ((newfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            continue;
        }

        if (connect(newfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(newfd);
            continue;
        }

        break;
    }

    // Check if process was able to connect to a server
    if (p == NULL) {
    	// All the found servers somehow do not allow us to connect.
    	MSG_WARNING(fprintf(stderr, "comm: client peer: failed to connect\n"););
        return -2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),s, sizeof s);
    MSG_INFO(fprintf(stderr, "comm: client peer: connecting to %s\n", s););

    freeaddrinfo(ai);
    return 0;
}

/**
 * Open a server port on this machine
 */
int socket_init_server() {
	int rv;
	int yes = 1;
	char *option_value;
	char *port = PORT, res = 0;

	if ((res = get_var(&option_value, options, "comm:port")) == 0) {
		port = option_value;
	}
	// Reset the result variable, no error was possible in the former call.
	res = 0;

	// Prepare the connection parameters
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

    // Initialize own address and store it in (ai), set connection option
	// (hints), exit on error
	if ((rv = getaddrinfo(NULL, port, &hints, &ai)) != 0) {
		res = -1;
		cace_set_error(ERROR_SERVER_CANNOT_BIND, "Server peer could not get "
				"address to bind.\nDo you have permission for the chosen port?");
	}

    // Bind to the next possible address from all the addresses the server has
	for (p = ai; res >= 0 && p != NULL; p = p->ai_next)	{
		listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (listener < 0) {
			continue;
		}

		// Lose the pesky "address already in use" error message
		if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))) {
			continue;
		}
        // Bind to the socket
		if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
			close(listener);
			continue;
		}
		break;
	}
	if (res >= 0 && p == NULL) {
    	// No binding was possible with any of the addresses in (ai). Will most
		// probably lead to an unrecoverable connection error
		MSG_ERROR(fprintf(stderr, "comm: server peer: failed to bind\n"););
		res = -1;
		cace_set_error(ERROR_SERVER_CANNOT_BIND, "Server cannot bind to chosen"
				"port.\nPort already in use?");
	}

	// Setup listener to accept max. MAXPENDING requesting clients
	if (res >= 0 && listen(listener, MAXPENDING) == -1)	{
		MSG_ERROR(fprintf(stderr, "comm: server peer: failed to initialize listener\n"););
		res = -1;
		cace_set_error(ERROR_SERVER_CONNECTION_SETUP , "Failed to initialize listener");
	}


    // Set timeout to wait for a client to 59.5 sec
	IFNOERRORX(
			tv.tv_sec = 59;
			tv.tv_usec = 500000;
	);

	IFNOERRORX(FD_ZERO(&read_fds););
	// Add the listener to the reader set
	IFNOERRORX(FD_SET(listener, &read_fds););

    // Setup lisetner to wait max &tv time for clients
	// TCP socket connection setup, don't care about writefds and exceptfds:
	if (select(listener + 1, &read_fds, NULL, NULL, &tv) == -1)	{
		MSG_ERROR(fprintf(stderr,
				"comm: server peer: failed to initialize max wait time\n"););
		res = -1;
		cace_set_error(ERROR_SERVER_CONNECTION_SETUP,
				"Failed to initialize maximal wait time for server listener");
	}


	// Wait for the defined number of clients for the defined max time
	if (res >= 0 && FD_ISSET(listener, &read_fds)) {
		addrlen = sizeof remoteaddr;
		newfd = accept(listener, (struct sockaddr*) &remoteaddr, &addrlen);
		if (newfd == -1) {
			MSG_ERROR(fprintf(stderr, "comm: server peer: socket accept error"););
			res = -1;
			cace_set_error(ERROR_SERVER_CONNECTION_SETUP , "Error while setting up connection");
		} else {
			FD_SET(newfd, &read_fds);
		}
		MSG_INFO(fprintf(stderr, "comm: server peer: new connection from %s on socket %d\n",
				inet_ntop(remoteaddr.ss_family,
						get_in_addr((struct sockaddr*) &remoteaddr), remoteIP,
						INET6_ADDRSTRLEN),newfd););
	} else {
		MSG_ERROR(fprintf(stderr,
				"comm: server peer: The connection to the client timed out.\n"););
		res = -1;
		cace_set_error(ERROR_CONNECTION_TIMEOUT, "Timeout while waiting for client");
	}

	return res;
}

/**
 * Initialize the socket (hybrid mode: first try to connect to an eventual
 * server; if none exist open a server yourself).
 */
int socket_init(struct input_var *file_inputs, struct input_var *file_options) {
	int res = 0;

	inputs = file_inputs;
	//printf("inputs: %X\n",(unsigned long)inputs);
	options = file_options;
	//printf("options: %X\n",(unsigned long)options);
	// Try to be the client, if an error happens, take the server role.
	if ((res = socket_init_client()) < 0) {
		res = socket_init_server();
	}
	return res;
}


/**
 * Interface method, receives an array of requested values and tries to fetch
 * them one by one in the given order.
 */
int request_socket(struct input_request request[], int count, int force_comm) {
	  int res = 0;
	  //int maxsize = 0;
	  int inputsize = 0;
	  int i;
	  char *input_value;

	  // Calculate the maximum size of the biggest requested element
	  // this is used for memory allocation.
	  //for (i = 0; i < count; i++) {
		 // if (request[i].result->group->bitlen > maxsize) {
		//	  maxsize = request[i].result->group->bitlen;
		//  }
	  //}

	  // Read each requested input one by one.
	  i = 0;
	  while(i < count && res == 0) {
		  inputsize = (request[i].result->group->bitlen* log(2)/log(10))+1;
		  // Try to read the value from file,
		  res = -1;
		  if (force_comm != 1) {
			  res = get_var(&input_value, inputs, request[i].name);
			  MSG_DEBUG(5, fprintf(stderr,
					  "comm: get value from config: result is %d, %s\n",res,
					  input_value););
		  }
		  if (res < 0) {
			  // Reading from file was not successful, request it via socket
			  res = request_through_socket(request[i], inputsize);
		  } else {
			  if (inputsize < strlen(input_value)) {
				  res = -1;
				  MSG_ERROR(fprintf(stderr, "Variable %s longer than allowed: %d not > %d\n",request[i].name, inputsize, strlen(input_value)););
				  cace_set_error(ERROR_OVERLONG_VALUE_IN_CONFIGFILE,
						  "Value for variable is longer than the allowed for defined type");
			  } else {
				  // Save the value given from inputs
				  cace_int_assign(request[i].result, input_value,0);

				  MSG_DEBUG(5, fprintf(stderr, "comm: assigned %s from input: ",request[i].name););
				  MSG_DEBUG(5, cace_int_print_err(request[i].result););
				  MSG_DEBUG(5, fprintf(stderr, "\n"););
			  }
		  }
		  i++;
	  }
	  return res;
}
