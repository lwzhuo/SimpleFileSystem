#ifndef __SHELL__
#define __SHELL__
#include"../global/var.h"
char * header();
char ** getInstruction(int *argc);
void go();
#endif