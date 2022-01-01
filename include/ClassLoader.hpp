#ifndef CLASSLOADER_H_INCLUDED
#define CLASSLOADER_H_INCLUDED

#include <ByteReader.hpp>
#include <lvm.hpp>
#include <vector>

u1 LVM_loadClass(LVM*, LVM_FILE*, lclass*);
u1 LVM_loadClasses(LVM*, char*, u1, std::vector<lclass*>*);

#endif // CLASSLOADER_H_INCLUDED
