#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED

#include <lvm.hpp>

#define LVM_ISWHOLE(t) (t == L_U1 || t == L_U2 || t == L_U4 || t == L_U8 || t == L_S1 || t == L_S2 || t == L_S4 || t == L_S8)
#define LVM_ISFLT(t) (t == L_FLOAT || t == L_DOUBLE)
#define LVM_ISARRAY(t) (t == L_ARRAY || t == L_UNIARRAY)
#define LVM_INVTYPE(t) (!LVM_ISWHOLE(t) && !LVM_ISFLT(t) && !LVM_ISARRAY(t) && t != L_CHAR && t != L_BOOLEAN && t != L_STRING && t != L_OBJECT && t != L_NULL)
#define LVM_INVREFTYPE(t) (LVM_INVTYPE(t) || LVM_ISARRAY(t) || t == L_OBJECT)
#define LVM_ISSIGNED(t) (t == L_S1 || t == L_S2 || t == L_S4 || t == L_S8)
#define LVM_ISUNSIGNED(t) (t == L_U1 || t == L_U2 || t == L_U4 || t == L_U8)

#define T2N(t) LVM_TYPES[t]
#define T2S(t) LVM_SIZES[t]

static const char* const LVM_TYPES[] = {
    "unsigned byte",
    "unsigned 2 bytes",
    "unsigned 4 bytes",
    "unsigned 8 bytes",
    "signed byte",
    "signed 2 bytes",
    "signed 4 bytes",
    "signed 8 bytes",
    "float",
    "double",
    "string",
    "char",
    "boolean",
    "class descriptor",
    "method descriptor",
    "field descriptor"
    "object",
    "array",
    "universal array",
    "null",
    "method reference",
    "generic type",
    "pointer",
    "void"
};

static const u1 LVM_SIZES[] = {
    1, 2, 4, 8, 1, 2, 4, 8, 4, 8, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

lstring* LVM_createString(LVM*, const char*);
lstring* LVM_createStringWithLen(LVM*, const char*, u4);
lstring* LVM_createStringFromArray(LVM*, larray*);
lclass* LVM_findClassByDescriptor(lThread*, const char*);
lmethod* LVM_findMethodByDescriptor(lclass*, const char*);
void LVM_methodDesc2Type(char*, u1*);
u1 LVM_refToVal(LVM*, LVM_Reference*, lValue*);
s8 LVM_toIntVal(lValue*);
void LVM_convertIntVal(lValue*);
lValue* LVM_findInstanceFieldByName(lThread*, lobject*, const char*, u1);
lValue* LVM_findClassFieldByName(lclass*, const char*, u1);
char* LVM_getMethodName(lmethod*);
char* LVM_getFullClassNameFromMethod(lmethod*);
char* LVM_getClassNameFromMethod(lmethod*);
u2 LVM_getLineNumber(lmethod*, u2);
LVM_CallInfo LVM_getCallInfo(lmethod*, u2);
u1 LVM_isInstanceOf(lThread*, lobject*, const char*);
char* LVM_getFullClassNameFromObject(lobject*);
void LVM_finalizeObject(lThread*, lobject*);
lobject* LVM_createNewObject(lThread*, lclass*);
larray* LVM_createNewArray(lThread*, u4, u1);
void LVM_nullOutObject(lThread*, lobject*);
void LVM_nullOutArray(lThread*, larray*);
lValue LVM_callMethod(lThread*, lmethod*, lValue*);
u1 LVM_checkMethodArgs(lThread*, lmethod*, lValue*);
lValue LVM_callNativeMethod(lThread*, lmethod*, lValue*);
lValue LVM_callOperatorMethod(lThread*, const char*, lValue*, lValue*, lValue*);
lmethod* LVM_findMethodDynamic(lclass*, const char*, u1, lValue*);
void LVM_throwException(lThread*, lobject*);
void LVM_throwExceptionNew(lThread*, const char*, const char*);

#endif // TYPES_H_INCLUDED
