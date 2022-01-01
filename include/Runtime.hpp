#ifndef RUNTIME_H_INCLUDED
#define RUNTIME_H_INCLUDED

#include <lvm.hpp>

lValue LVM_execute(lThread*);
u1 LVM_handleException(lThread*);

#endif // RUNTIME_H_INCLUDED
