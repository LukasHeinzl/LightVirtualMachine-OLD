#ifndef BYTEREADER_H_INCLUDED
#define BYTEREADER_H_INCLUDED

#include <lvm.hpp>

typedef struct{
    u1* content;
    u4 length;
    u4 pos;
} LVM_FILE;

LVM_FILE LVM_loadFile(LVM*, char*);
void LVM_freeFile(LVM*, LVM_FILE*);
u1 LVM_readU1(LVM*, LVM_FILE*);
u2 LVM_readU2(LVM*, LVM_FILE*);
u4 LVM_readU4(LVM*, LVM_FILE*);
u8 LVM_readU8(LVM*, LVM_FILE*);
void LVM_ensurePosition(LVM*, LVM_FILE*);
const char* LVM_getFExt(char*);

#endif // BYTEREADER_H_INCLUDED
