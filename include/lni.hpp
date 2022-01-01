#ifndef LNI_HPP_INCLUDED
#define LNI_HPP_INCLUDED

#ifdef _WIN32

#include <windows.h>
#undef DELETE

#define LVM_EXPORT extern "C" __declspec(dllexport)
#define LVM_IMPORT __declspec(dllimport)
#define LVM_LOADLIB(N) (void*) LoadLibrary(N)
#define LVM_GETFUNC(L, N) (void*) GetProcAddress((HINSTANCE) L, N)
#define LVM_FREELIB(L) FreeLibrary((HINSTANCE) L)

#else

#include <dlfcn.h>

#define LVM_EXPORT extern "C"
#define LVM_IMPORT
#define LVM_LOADLIB(N) dlopen(N, RTLD_NOW)
#define LVM_GETFUNC(L, N) dlsym(L, N)
#define LVM_FREELIB(L) dlclose(L)

#endif // _WIN32

#include <lvm.hpp>

typedef struct LNI_{
    lThread* thread;
    u1 paramCount;
    lValue* params;

    lValue* (*getParam)(LNI_*, u1);
    void (*setParam)(LNI_*, u1, lValue*);
    void (*returnValue)(LNI_*, lValue*);
    void (*throwException)(LNI_*, lobject*);
    lclass* (*getClass)(LNI_*, char*);
    lobject* (*newObject)(LNI_*, lclass*);
	void (*finalizeObject)(LNI_*, lobject*);
    void (*deleteObject)(LNI_*, lobject*);
    larray* (*newArray)(LNI_*, u1, u4);
	u4 (*getArrayLength)(LNI_*, larray*);
    void (*deleteArray)(LNI_*, larray*);
    lstring* (*newString)(LNI_*, const char*);
	u4 (*getStringLength)(LNI_*, lstring*);
    void (*deleteString)(LNI_*, lstring*);
    char* (*toCString)(LNI_*, lstring*);
    lValue* (*getStaticMember)(LNI_*, lclass*, char*);
    void (*setStaticMember)(LNI_*, lclass*, char*, lValue*);
    lValue* (*getMember)(LNI_*, lobject*, char*);
    void (*setMember)(LNI_*, lobject*, char*, lValue*);
	lValue (*callMethod)(LNI_*, lobject*, const char*, lValue*, u1);
	lValue (*callStaticMethod)(LNI_*, lclass*, const char*, lValue*);
    void* (*allocate)(LNI_*, size_t);
    void (*free)(LNI_*, void*);
} LNI;

LNI LVM_getNewLNI(lThread*, u1, lValue*);

lValue* LNI_getParam(LNI*, u1);
void LNI_setParam(LNI*, u1, lValue*);
void LNI_returnValue(LNI*, lValue*);
void LNI_throwException(LNI*, lobject*);
lclass* LNI_getClass(LNI*, char*);
lobject* LNI_newObject(LNI*, lclass*);
void LNI_finalizeObject(LNI*, lobject*);
void LNI_deleteObject(LNI*, lobject*);
larray* LNI_newArray(LNI*, u1, u4);
u4 LNI_getArrayLength(LNI*, larray*);
void LNI_deleteArray(LNI*, larray*);
lstring* LNI_newString(LNI*, const char*);
u4 LNI_getStringLength(LNI*, lstring*);
void LNI_deleteString(LNI*, lstring*);
char* LNI_toCString(LNI*, lstring*);
lValue* LNI_getStaticMember(LNI*, lclass*, char*);
void LNI_setStaticMember(LNI*, lclass*, char*, lValue*);
lValue* LNI_getMember(LNI*, lobject*, char*);
void LNI_setMember(LNI*, lobject*, char*, lValue*);
lValue LNI_callMethod(LNI*, lobject*, const char*, lValue*, u1);
lValue LNI_callStaticMethod(LNI*, lclass*, const char*, lValue*);
void* LNI_allocate(LNI*, size_t);
void LNI_free(LNI*, void*);

#endif // LNI_HPP_INCLUDED
