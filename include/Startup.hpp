#ifndef STARTUP_H_INCLUDED
#define STARTUP_H_INCLUDED

#include <lvm.hpp>

lmethod* LVM_createStaticObjectsAndGetMain(LVM*);
void LVM_classInit(lThread*);

#endif // STARTUP_H_INCLUDED
