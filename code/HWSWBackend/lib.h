 /**
  * The size of the words, in bytes
  */
#define SIZE 128

/**
 * Displays the last result on the coprocessor
 * (for testing purposes only)
 */
void display_result();

 /**
  * Multiplies 2 numbers provided in a and b. Stores the result in z. z must be preallocated.
  *
  * @param a 1024-bit big-endian operand
  * @param b 1024-bit big-endian operand
  * @param z 2048-bit big-endian result
  */
void multiply1024(__xdata unsigned char *a, __xdata unsigned char *b, __xdata unsigned char *z);

 /**
  * Checks if z is larger or equal than p
  *
  * @param z 8*size-bit big-endian number
  * @param p 8*size-bit big-endian number
  * @param size size in bytes
  * @return 1 when z >= p, 0 otherwise
  */
char larger_or_equal(__xdata unsigned char *z, __xdata unsigned char *p, unsigned char size);

 /**
  * Adds 2 numbers provided in a and b. Stores the result in z. z must be preallocated.
  * The numbers have to be aligned on a 1024-bit boundary.
  *
  * @param a 1024-bit big-endian operand
  * @param b 1024-bit big-endian operand
  * @param z 1024-bit big-endian result
  * @return 1 if overflow, 0 if no overflow
  */
char add1024(__xdata unsigned char *a, __xdata unsigned char *b, __xdata unsigned char *z);

 /**
  * Subtracts 2 numbers provided in a and b. Stores the result in z. z must be preallocated.
  * The numbers have to be aligned on a 1024-bit boundary.
  *
  * @param a 1024-bit big-endian first operand
  * @param b 1024-bit big-endian second operand
  * @param z 1024-bit big-endian result
  * @return 1 if borrow needed, 0 if no borrow needed
  */
char subtract1024(__xdata unsigned char *a, __xdata unsigned char *b, __xdata unsigned char *z);

/**
 * Copies source number to destination number
 * @param dest destination 1024-bit number
 * @param src source 1024-bit number
 */
void copy(__xdata unsigned char *dest, __xdata unsigned char *src);

/**
 * Product in Montgomery domain (a * b * R mod n, R = 2^1024)
 * @param a 1024-bit big-endian operand
 * @param b 1024-bit big-endian operand
 * @param m 1024-bit big-endian result
 * @param n 1024-bit big-endian modulus
 */
void montpro(__xdata unsigned char *a, __xdata unsigned char *b, __xdata unsigned char *m, __xdata unsigned char *n);

/**
 * Inversion in Montgomery domain (a^-1 mod n, R = 2^1024)
 * @param a 1024-bit big-endian operand
 * @param p 1024-bit big-endian modulus
 * @param r 1024-bit big-endian result
 */
void montinv(__xdata unsigned char *a, __xdata unsigned char *p, __xdata unsigned char *r);

/**
 * Generates a random number in the interval [0, mod)
 * @param out 1024-bit generated number
 * @param mod 1024-bit modulus
 */
void random(__xdata unsigned char *out, __xdata unsigned char *mod);
  
/**
 * Modular exponentiation in Montgomery domain yt = xt^exp mod n
 * @param xt base (in Montgomery domain)
 * @param exp exponent
 * @param yt result (in Montgomery domain)
 * @param R R mod n
 */
void montexp(__xdata unsigned char *xt, __xdata unsigned char *exp, __xdata unsigned char *yt, __xdata unsigned char *R);
