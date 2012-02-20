#include <gezel/ipblock.h>
#include <termios.h>
#include <fcntl.h>

class ipterm : public aipblock {
  int fd;

public:

  ipterm(char *name);

  ~ipterm();

  void run();

  bool checkterminal(int n, char *tname, aipblock::iodir dir);

  void setparm(char *_name);

  bool cannotSleepTest() { return false; }

  bool needsWakeupTest() { return false; }
};

extern "C" aipblock *create_term(char *instname) {
  return new ipterm(instname);
}

ipterm::ipterm(char *name) : aipblock(name) {
  fd = open(name, O_RDWR | O_NOCTTY | O_NONBLOCK);

  if(fd < 0) {
    perror(name);
  }

  struct termios state;

  if(tcgetattr(fd, &state) < 0) {
    perror(name);
  }

  state.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  state.c_iflag &= ~(BRKINT | ICRNL | ISTRIP | IXON);
  state.c_cflag &= ~(CSIZE | PARENB);

  state.c_cflag &= CS8;
  state.c_oflag &= ~(OPOST);

  if(tcsetattr(fd, TCSAFLUSH, &state) < 0) {
    perror(name);
  }
}

ipterm::~ipterm() {
  close(fd);
}

bool ipterm::checkterminal(int n, char *tname, aipblock::iodir dir) {
  switch(n) {
  case 0:
    return (isoutput(dir) && isname(tname, "rx"));
  case 1:
    return (isinput(dir) && isname(tname, "tx"));
  }

  return false;
}

void ipterm::run() {
  unsigned char buffer[1];

  int n = read(fd, buffer, 1);

  if(n > 0) {
    ioval[0]->assignulong(buffer[0]);
  } else {
    ioval[0]->assignulong(0);
  }

  buffer[0] = (unsigned char) ioval[1]->toulong();

  write(fd, buffer, 1);
}

void ipterm::setparm(char *_name) {

}
