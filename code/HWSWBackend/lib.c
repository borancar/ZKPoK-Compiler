#include <8051.h>
#include "lib.h"

void multiply1024(__xdata unsigned char *a, __xdata unsigned char *b, __xdata unsigned char *z) {
	int           i;
	int           j;
	unsigned int  cl;
	
	unsigned char carry = 0;
	int           m;

	for(cl = 0; cl < 2*SIZE; cl++) {
		z[cl] = 0;
	}

	for(i = SIZE-1; i >= 0; i--) {
		for(j = SIZE-1; j >= 0; j--) {
			m = a[i]*b[j] + z[i+j+1] + carry;

			z[i+j+1] = m;
			carry = m >> 8;
		}

		z[i] = carry;
		carry = 0;
	}
}

char larger_or_equal(__xdata unsigned char *z, __xdata unsigned char *p, unsigned char size)
{
	unsigned char i;

	for(i = 0; i < size-1; i++) {
		if(z[i] > p[i]) return 1;
		if(z[i] < p[i]) return 0;
	}

	return z[size-1] >= p[size-1];
}

char add1024(__xdata unsigned char *a, __xdata unsigned char *b, __xdata unsigned char *z) {
	a;
	b;
	z;

	__asm

	mov A, DPL
	add A, #0x7F
	mov DPL, A
	mov R2, DPL
	mov R3, DPH

	mov DPL, _add1024_PARM_2
	mov DPH, (_add1024_PARM_2+1)
	mov A, DPL
	add A, #0x7F
	mov DPL, A
	mov R4, DPL
	mov R5, DPH

	mov DPL, _add1024_PARM_3
	mov DPH, (_add1024_PARM_3+1)
	mov A, DPL
	add A, #0x7F
	mov DPL, A
	mov R6, DPL
	mov R7, DPH

		mov R0, #128
loop_add:	mov DPL, R2
		mov DPH, R3

		movx A, @DPTR
		mov R1, A

		mov DPL, R4
		mov DPH, R5

		movx A, @DPTR

		addc A, R1

		mov DPL, R6
		mov DPH, R7

		movx @DPTR, A

		dec R2
		dec R4
		dec R6

		djnz R0, loop_add

	mov A, #0
	addc A, #0
	mov DPL, A

	__endasm;
}

char subtract1024(__xdata unsigned char *a, __xdata unsigned char *b, __xdata unsigned char *z) {
	a;
	b;
	z;

	__asm

	mov A, DPL
	add A, #0x7F
	mov DPL, A
	mov R2, DPL
	mov R3, DPH

	mov DPL, _subtract1024_PARM_2
	mov DPH, (_subtract1024_PARM_2+1)
	mov A, DPL
	add A, #0x7F
	mov DPL, A
	mov R4, DPL
	mov R5, DPH

	mov DPL, _subtract1024_PARM_3
	mov DPH, (_subtract1024_PARM_3+1)
	mov A, DPL
	add A, #0x7F
	mov DPL, A
	mov R6, DPL
	mov R7, DPH

		mov R0, #128
loop_sub:	mov DPL, R4
		mov DPH, R5
	
		movx A, @DPTR
		mov R1, A
	
		mov DPL, R2
		mov DPH, R3
	
		movx A, @DPTR
	
		subb A, R1
	
		mov DPL, R6
		mov DPH, R7
	
		movx @DPTR, A
	
		dec R2
		dec R4
		dec R6
	
		djnz R0, loop_sub

	mov A, #0
	addc A, #0
	mov DPL, A

	__endasm;
}

void copy(__xdata unsigned char *dest, __xdata unsigned char *src) {
	int i;

	for(i = 0; i < SIZE; i++) {
		*dest++ = *src++;
	}
}

void coproc_execute() {
  volatile char __xdata __at(0x800) state[1];

  P1 = 0x01;

  P1 = 0x02;

  /* Wait for the coprocessor to signal done */
  while(state[0] == 0) {

  }

  P1 = 0x03;
}

void display_result() {
	volatile char __xdata __at(0x780) coproc_up[SIZE];

	coproc_up[0x00] = 0xFF;
	coproc_up[0x01] = 0x00;

	coproc_execute();
}

void montpro(__xdata unsigned char *a, __xdata unsigned char *b, __xdata unsigned char *m, __xdata unsigned char *n) {
	volatile char __xdata __at(0x600) coproc_a[SIZE];
	volatile char __xdata __at(0x680) coproc_b[SIZE];
	volatile char __xdata __at(0x700) coproc_n[SIZE];
	volatile char __xdata __at(0x780) coproc_up[SIZE];

	copy(coproc_a, a);
	copy(coproc_b, b);
	copy(coproc_n, n);

	coproc_up[0x00] = 0x03;
	coproc_up[0x01] = 0x40;
	coproc_up[0x02] = 0x45;
	coproc_up[0x03] = 0x01;
	coproc_up[0x04] = 0x50;
	coproc_up[0x05] = 0x00;

	coproc_execute();

	copy(m, coproc_a);
}

void montinv(__xdata unsigned char *a, __xdata unsigned char *p, __xdata unsigned char *r) {
  volatile char __xdata __at(0x600) coproc_a[SIZE];
  volatile char __xdata __at(0x700) coproc_n[SIZE];
  volatile char __xdata __at(0x780) coproc_up[SIZE];

  coproc_up[0x00] = 0x03;
  coproc_up[0x01] = 0x99;
  coproc_up[0x02] = 0x50;
  coproc_up[0x03] = 0x00;

  coproc_execute();

  copy(r, coproc_a);
}

void random(__xdata unsigned char *out, __xdata unsigned char *mod) {
	out[SIZE] = 4;
}

void montexp(__xdata unsigned char *xt, __xdata unsigned char *exp, __xdata unsigned char *yt, __xdata unsigned char *R) {
	volatile char __xdata __at(0x600) coproc_a[SIZE];
	volatile char __xdata __at(0x680) coproc_b[SIZE];
	volatile char __xdata __at(0x780) coproc_up[SIZE];

	int qpos = 0;
	int i, first;
	unsigned char e;

	copy(coproc_a, R);
	copy(coproc_b, xt);

	for(first = 0; exp[first] == 0 && first < SIZE; first++);

	e = exp[first];

	coproc_up[qpos++] = 0x40;
	coproc_up[qpos++] = 0x45;
	
	for(i = first; i < SIZE; i++) {
	  e = exp[i];

	  coproc_up[qpos++] = 0x02; if(e & 0x80) coproc_up[qpos++] = 0x01;
	  coproc_up[qpos++] = 0x02; if(e & 0x40) coproc_up[qpos++] = 0x01;
	  coproc_up[qpos++] = 0x02; if(e & 0x20) coproc_up[qpos++] = 0x01;
	  coproc_up[qpos++] = 0x02; if(e & 0x10) coproc_up[qpos++] = 0x01;
	  coproc_up[qpos++] = 0x02; if(e & 0x08) coproc_up[qpos++] = 0x01;
	  coproc_up[qpos++] = 0x02; if(e & 0x04) coproc_up[qpos++] = 0x01;
	  coproc_up[qpos++] = 0x02; if(e & 0x02) coproc_up[qpos++] = 0x01;
	  coproc_up[qpos++] = 0x02; if(e & 0x01) coproc_up[qpos++] = 0x01;

	  if(qpos > 100) {
	    coproc_up[qpos] = 0x00;
	    coproc_execute();
	    qpos = 0x00;
	  }
	}

	coproc_up[qpos++] = 0x50;
	coproc_up[qpos] = 0x00;

	coproc_execute();

	copy(yt, coproc_a);
}
