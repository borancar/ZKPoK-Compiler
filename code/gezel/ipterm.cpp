#include <gezel/ipblock.h>
#include <termios.h>
#include <fcntl.h>

/** 
 * The ipterm used to communicate raw data through the terminal
 * interface
 */
class ipterm : public aipblock {
  int fd;			/**< the file descriptor associated
				   with the terminal */

public:

  /** 
   * Creates a new term with the provided terminal name
   * 
   * @param name terminal name to use
   */
  ipterm(char *name);

  /** 
   * Releases the used resources
   */
  ~ipterm();

  /** 
   * Runs the current clock cyclee iteration
   */
  void run();

  /** 
   * Checks the ports of the block
   * 
   * @param n order of the port
   * @param tname name of the port
   * @param dir direction of the port
   * 
   * @return true if the ports are specified correctly, false otherwise
   */
  bool checkterminal(int n, char *tname, aipblock::iodir dir);

  /** 
   * Sets the parameter of this block
   * 
   * @param _name the parameter name
   */
  void setparm(char *_name);

  /** 
   * Returns if the block cannot be put to sleep
   * 
   * @return true if the block cannot be put to sleep, false otherwise
   */
  bool cannotSleepTest() { return false; }

  /** 
   * Returns if the block needs wakeup test after sleep
   *
   * @return true if the block needs wakeup test, false otherwise
   */
  bool needsWakeupTest() { return false; }
};

/** 
 * Provides the entry point for instantiating the block
 * 
 * @param instname name of the block
 * 
 * @return the instantiated block
 */
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
