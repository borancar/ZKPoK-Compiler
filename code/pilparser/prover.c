#include <gmp.h>
#include <termios.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

mpz_t g_r_1, g_s_1;

mpz_t _mpz_3, _mpz_4, _mpz_11, _mpz_23;

gmp_randstate_t rstate;

int fd, res;
struct termios tstate;

void init() {
  mpz_init(g_r_1);
  mpz_init(g_s_1);
  
  mpz_init_set_str(_mpz_3, "3", 10);
  mpz_init_set_str(_mpz_4, "4", 10);
  mpz_init_set_str(_mpz_11, "11", 10);
  mpz_init_set_str(_mpz_23, "23", 10);
  
  gmp_randinit_default(rstate);
}

void Round0(void) {


}

void Round1(mpz_t Out0) {
  mpz_t _r_1, _t_1;
  mpz_init(_r_1);
  mpz_init(_t_1);

  mpz_urandomm(_r_1, rstate, _mpz_11);
  mpz_set(g_r_1, _r_1);
  mpz_powm(_t_1, _mpz_3, _r_1, _mpz_23);
  mpz_set(Out0, _t_1);

  mpz_clear(_r_1);
  mpz_clear(_t_1);
}

void Round2(mpz_t Out0, mpz_t _c) {
  mpz_t _r_1, _s_1, _s_11;
  mpz_init(_r_1);
  mpz_init(_s_1);
  mpz_init(_s_11);

  mpz_set(_r_1, g_r_1);
  mpz_mul(_s_1, _mpz_4, _c);
  mpz_mod(_s_1, _s_1, _mpz_11);
  mpz_add(_s_11, _r_1, _s_1);
  mpz_mod(_s_11, _s_11, _mpz_11);
  mpz_set(g_s_1, _s_11);
  mpz_set(Out0, _s_11);

  mpz_clear(_r_1);
  mpz_clear(_s_1);
  mpz_clear(_s_11);
}

void clear() {
  mpz_clear(g_r_1);
  mpz_clear(g_s_1);
  
  mpz_clear(_mpz_3);
  mpz_clear(_mpz_4);
  mpz_clear(_mpz_11);
  mpz_clear(_mpz_23);
}

int init_term(char *file) {
  if((fd = res = open(file, O_RDWR | O_NOCTTY)) < 0) {
    fprintf(stderr, "Could not open port!\n");
    return res;
  }

  if((res = tcgetattr(fd, &tstate)) < 0) {
    fprintf(stderr, "%s\n", strerror(errno));
    return res;
  }

  tstate.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  tstate.c_iflag &= ~(BRKINT | ICRNL | ISTRIP | IXON);
  tstate.c_cflag &= ~(CSIZE | PARENB);

  tstate.c_cflag &= CS8;
  tstate.c_oflag &= ~(OPOST);

  if((res = tcsetattr(fd, TCSAFLUSH, &tstate)) < 0) {
    fprintf(stderr, "%s\n", strerror(errno));
    return res;
  }

  tcflush(fd, TCIOFLUSH);
}

int receive_term(mpz_t val) {
  int inputsize = 1024;
  int i, pos = 0;
  char *buffer;

  buffer = malloc(inputsize);

  buffer[pos] = 0;

  while(!isalnum(buffer[pos])) {
    read(fd, buffer+pos, 1);
  }

  pos++;

  while(pos + 1 < inputsize) {
    int chars = read(fd, buffer+pos, 1);

    if(chars == 0) continue;

    if(buffer[pos] == '\n' || buffer[pos] == '\r') {
      buffer[pos] = 0;
      
      mpz_init_set_str(val, buffer, 10);
	  
      free(buffer);
      return 0;
    }
	
    if(!isalnum(buffer[pos])) continue;

    pos += chars;
  }

  if(pos + 1 == inputsize) {
    fprintf(stderr, "input too long\n");
  }

  free(buffer);
  return 0;
}

int send_term(mpz_t val) {
  char *str;

  str = mpz_get_str(0, 10, val);

  write(fd, str, strlen(str));
  write(fd, "\r\n", 2);

  free(str);
}

int close_term() {
  close(fd);
}

int main(int argc, char *argv[]) {
  mpz_t _t_1, _C, _s_1;

  mpz_init(_t_1);
  mpz_init(_C);
  mpz_init(_s_1);

  init_term(argv[1]);
  init();
  
  Round0();
  
  Round1(_t_1);
  
  gmp_printf("Round1 output: %Zd\n", _t_1);
  
  send_term(_t_1);

  gmp_printf("Round2 input: ");
  
  receive_term(_C);

  gmp_printf("%Zd\n", _C);

  Round2(_s_1, _C);

  gmp_printf("Round2 output: %Zd\n", _s_1);

  send_term(_s_1);

  clear();

  close_term();
  
  mpz_clear(_t_1);
  mpz_clear(_C);
  mpz_clear(_s_1);

  return 0;
}
