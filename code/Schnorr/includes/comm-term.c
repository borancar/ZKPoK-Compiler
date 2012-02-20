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
 * Implementation of the communication interface via terminal.
 * Verifier must be started first to ensure nothing is lost in the
 * process.
 *
 * #######################################################################
 *
 * Licence: GNU GPLv2 (http://www.gnu.org/licenses/gpl-2.0.html)
 *
 *
 *  Created on: 01.12.2011
 *      Author: Boran Car
 */

#include <termios.h>
#include <fcntl.h>
#include <math.h>
#include <errno.h>
#include <stdio.h>
#include <ctype.h>

#include "comm.h"
#include "types.h"

int fd;
struct termios state;
struct input_var *inputs;
struct input_var *options;

int term_init(struct input_var *file_inputs, struct input_var *file_options) {
  char *port_file;
  char *mode;
  int res;

  inputs = file_inputs;
  options = file_options;

  if((res = get_var(&port_file, options, "term:port")) != 0) {
    return res;
  }

  MSG_INFO(fprintf(stderr, "comm: opening %s\n", port_file););

  res = open(port_file, O_RDWR | O_NOCTTY);

  if(res < 0) {
    MSG_ERROR(fprintf(stderr, "comm: port %s: %s\n", port_file, strerror(errno)););
    return res;
  }

  fd = res;

  if((res = get_var(&mode, options, "term:mode")) == 0) {
    if(strcmp(mode, "raw") == 0) {
      MSG_INFO(fprintf(stderr, "comm: setting raw mode\n"););

      if((res = tcgetattr(fd, &state)) < 0) {
	MSG_ERROR(fprintf(stderr, "comm: %s\n", strerror(errno)););
	return res;
      }

      state.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
      state.c_iflag &= ~(BRKINT | ICRNL | ISTRIP | IXON);
      state.c_cflag &= ~(CSIZE | PARENB);

      state.c_cflag &= CS8;
      state.c_oflag &= ~(OPOST);

      if((res = tcsetattr(fd, TCSAFLUSH, &state)) < 0) {
	MSG_ERROR(fprintf(stderr, "comm: %s\n", strerror(errno)););
	return res;
      }
    } else {
      MSG_ERROR(fprintf(stderr, "comm: %s not supported, only raw!\n", mode););
    }
  } else {
    MSG_ERROR(fprintf(stderr, "comm: must specify term:mode!\n"););
  }

  tcflush(fd, TCIOFLUSH);

  MSG_INFO(fprintf(stderr, "Connected\n"););

  return 0;
}

int request_term(struct input_request request[], int count, int force_comm) {
  char *buffer;
  int inputsize, res;
  int i, pos;

  for(i = 0; i < count; i++) {
    inputsize = (request[i].result->group->bitlen*log(2)/log(10))+1;

    res = -1;

    if(force_comm != 1) {
      res = get_var(&buffer, inputs, request[i].name);
      MSG_DEBUG(5, fprintf(stderr, "comm: get value from config: result is %d, %s\n", res, buffer););

      cace_int_assign(request[i].result, buffer, 1);
    }

    if(res < 0) {
      MSG_DEBUG(4, fprintf(stderr, "comm: trying to read (term) %s (%d)\n", request[i].name, inputsize););

      buffer = malloc(inputsize);

      buffer[0] = 0;

      while(!isalnum(buffer[0])) {
	read(fd, buffer, 1);
      }

      for(pos = 1; pos + 1 < inputsize; pos++) {
	read(fd, buffer+pos, 1);
	if(buffer[pos] == '\n' || buffer[pos] == '\r') {
	  break;
	}
      }

      if(pos + 1 == inputsize) {
	MSG_ERROR(fprintf(stderr, "comm: input too long\n"););
      }

      buffer[pos] = 0;

      cace_int_assign(request[i].result, buffer, 1);

      free(buffer);
    }
  }

  return 0;
}

int send_through_term(char *value, int size) {
  int written;

  MSG_DEBUG(4, fprintf(stderr, "comm: send %s of length %d through term (%d)\n", value, size, fd););

  write(fd, value, size);
  write(fd, "\r\n", 2);
}

int term_stop() {
  return close(fd);
}
