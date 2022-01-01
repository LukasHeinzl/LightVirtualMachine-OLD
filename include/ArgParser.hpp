#ifndef ARGPARSER_H_INCLUDED
#define ARGPARSER_H_INCLUDED

#include <lvm.hpp>

int LVM_parseArgs(int, char**, char**, u4*);
void LVM_trimString(char*, u1, u1);
u1 LVM_handleArg(char*, char*, u4*);
s8 LVM_parseNumber(char*);
void LVM_showHelp();

#endif // ARGPARSER_H_INCLUDED
