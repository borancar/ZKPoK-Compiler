#include <gezel/ipblock.h>
#include <termios.h>
#include <fcntl.h>
#include <cstring>
using namespace std;

/** 
 * The iptransciever used to communicate numbers through the terminal
 * interface
 */
class iptransceiver : public aipblock {
  int fd;			/**< the file descriptor associated
				   with the terminal */
  static const int number_size = 2048; /**< the maximal size of the number */

  /** 
   * Sends the value in the tx buffer through the terminal
   */
  void send();

  /** 
   * Receives the value from the other side of the terminal and puts
   * it in rx
   */
  void receive();

public:

  /** 
   * Creates a new transeiver with the provided terminal name
   * 
   * @param name terminal name to use
   */
  iptransceiver(char *name);

  /** 
   * Releases the used resources
   */
  ~iptransceiver();

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
extern "C" aipblock *create_transceiver(char *instname) {
  return new iptransceiver(instname);
}

iptransceiver::iptransceiver(char *name) : aipblock(name) {
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

  tcflush(fd, TCIOFLUSH);
}

iptransceiver::~iptransceiver() {
  close(fd);
}

bool iptransceiver::checkterminal(int n, char *tname, aipblock::iodir dir) {
  switch(n) {
  case 0:
    return (isinput(dir) && isname(tname, "tx"));
  case 1:
    return (isoutput(dir) && isname(tname, "rx"));
  case 2:
    return (isinput(dir) && isname(tname, "sr"));
  case 3:
    return (isoutput(dir) && isname(tname, "done"));
  }

  return false;
}

void iptransceiver::receive() {
  char buffer[number_size];

  buffer[0] = 0;

  int pos = 0;

  while(!isalnum(buffer[pos])) {
    read(fd, buffer+pos, 1);
  }

  pos++;

  while(pos + 1 < number_size) {
    int chars = read(fd, buffer+pos, 1);

    if(chars == 0) continue;

    if(buffer[pos] == '\n' || buffer[pos] == '\r') {
      buffer[pos] = 0;

      *ioval[1] = *make_gval(buffer);
       ioval[3]->assignulong(1);
      return;
    }

    if(!isalnum(buffer[pos])) continue;

    pos += chars;
  }

  ioval[3]->assignulong(0);
}

void iptransceiver::send() {
  char buffer[number_size];

  ioval[0]->setbase(10);

  ioval[0]->writebuf(buffer);

  strcat(buffer, "\r\n");

  write(fd, buffer, strlen(buffer));

  ioval[3]->assignulong(1);
}

void iptransceiver::run() {
  switch(ioval[2]->toulong()) {
  case 1:
    send();
    break;
  case 2:
    receive();
    break;
  default:
    ioval[3]->assignulong(0);
  }
}

void iptransceiver::setparm(char *_name) {

}
