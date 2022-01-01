#ifndef ERROR_H_INCLUDED
#define ERROR_H_INCLUDED

#include <lvm.hpp>

enum LVM_ERROR_TYPE{
    LVM_FATAL = 0,
    LVM_NULLPTR = 1,
    LVM_NOMEM = 2,
    LVM_CASTERR = 3,
    LVM_INDEXERR = 4,
    LVM_FILEERR = 5,
    LVM_NOMAIN = 6,
    LVM_NOMETHOD = 7,
    LVM_NOCLASS = 8,
    LVM_TYPEERR = 9,
    LVM_OPERR = 10,
    LVM_PTRERR = 11,
	LVM_PARMIMA = 12
};


void LVM_error(LVM*, LVM_ERROR_TYPE, ...);
#endif // ERROR_H_INCLUDED
