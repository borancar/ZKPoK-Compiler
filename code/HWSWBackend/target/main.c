#include <8051.h>
#include "lib.h"

volatile __xdata __at(0x000) unsigned char _const_1[SIZE] = {0};
volatile __xdata __at(0x080) unsigned char _const_2[SIZE] = {0};
volatile __xdata __at(0x100) unsigned char _const_3[SIZE] = {0};
volatile __xdata __at(0x180) unsigned char _const_4[SIZE] = {0};
volatile __xdata __at(0x200) unsigned char _const_11[SIZE] = {0};
volatile __xdata __at(0x280) unsigned char _const_23[SIZE] = {0};
volatile __xdata __at(0x780) unsigned char g_r_1[SIZE] = {0};
volatile __xdata __at(0x800) unsigned char g_s_1[SIZE] = {0};

void Round0(void) {

}

void Round1(__xdata unsigned char *Out0) {
  __xdata __at(0x480) unsigned char _r_1[SIZE];
  __xdata __at(0x500) unsigned char _t_11[SIZE];
  __xdata __at(0x580) unsigned char _t_112[SIZE];
  __xdata __at(0x600) unsigned char t[SIZE];

  random(_r_1, _const_11);
  copy(g_r_1, _r_1);
  montpro(_const_3, _const_4, t, _const_23);
  montexp(t, _r_1, _t_11, _const_2);
  montpro(_t_11, _const_1, _t_112, _const_23);
  copy(Out0, _t_112);
}

void Round2(__xdata unsigned char *Out0, __xdata unsigned char *_c) {
  __xdata __at(0x480) unsigned char _ct[SIZE];
  __xdata __at(0x500) unsigned char _r_1[SIZE];
  __xdata __at(0x580) unsigned char _s_11[SIZE];
  __xdata __at(0x600) unsigned char _s_13[SIZE];
  __xdata __at(0x680) unsigned char _s_134[SIZE];
  __xdata __at(0x700) unsigned char t[SIZE];

  copy(_r_1, g_r_1);
  montpro(_const_4, _const_3, t, _const_11);  
  montpro(_c, _const_3, _ct, _const_11);
  montpro(t, _ct, _s_13, _const_11);
  montpro(_s_13, _const_1, _s_134, _const_11);
  
  if(add1024(_r_1, _s_134, _s_11))
    subtract1024(_s_11, _const_11, _s_11);
  else if(larger_or_equal(_s_11, _const_11, SIZE))
    subtract1024(_s_11, _const_11, _s_11);
  
  copy(g_s_1, _s_11);
  copy(Out0, _s_11);
}

void main() {
	volatile __xdata __at(0x300) unsigned char _t_1[SIZE] = {0};
	volatile __xdata __at(0x380) unsigned char _c[SIZE] = {0};
	volatile __xdata __at(0x400) unsigned char _s_1[SIZE] = {0};

	_const_1[SIZE-1] = 1;
	_const_2[SIZE-1] = 2;
	_const_3[SIZE-1] = 3;
	_const_4[SIZE-1] = 4;
	_const_11[SIZE-1] = 11;
	_const_23[SIZE-1] = 23;

	_c[SIZE-1] = 0x70;
	_c[SIZE-2] = 0x33;
	_c[SIZE-3] = 0x50;
		
	Round0();

	Round1(_t_1);

	display_result(_t_1);
	
	display_result(_c);
	
	Round2(_s_1, _c);

	display_result(_s_1);
	
	P3 = 0x55;
}
