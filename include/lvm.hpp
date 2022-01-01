/*
    COPYRIGHT (C) 2016

    This file contains all fundamental types, IDs and other information
        that the LVM needs to function properly
*/

#ifndef LVM_H_INCLUDED
#define LVM_H_INCLUDED

//needed headers
#include <vector>
#include <stack>
#include <set>
#include <cstddef>
#include <cstdint>
#include <cinttypes>

//general information
#define LVM_MA_VER 1
#define LVM_MI_VER 0
#define LVM_MAJOR_VERSION "1"
#define LVM_MINOR_VERSION "0"

//command line options
#define LVM_OFREECOUNT 0
#define LVM_OFREEARRCNT 1
#define LVM_OFREEOBJCNT 2
#define LVM_OCLSRSRV 3
#define LVM_OARRAYRSRV 4
#define LVM_OMEMRSRV 5
#define LVM_OOBJRSRV 6
#define LVM_OTHRDRSRV 7
#define LVM_OMAXMEMSLOT 8
#define LVM_OMINSLOTFREE 9
#define LVM_OUPK 10

//define type and reference-type ID's
#define L_U1 0
#define L_U2 1
#define L_U4 2
#define L_U8 3
#define L_S1 4
#define L_S2 5
#define L_S4 6
#define L_S8 7

#define L_FLOAT 8
#define L_DOUBLE 9
#define L_STRING 10
#define L_CHAR 11
#define L_BOOLEAN 12
#define L_CLASSDESC 13
#define L_METHODDESC 14
#define L_FIELDDESC 15

#define L_OBJECT 16
#define L_ARRAY 17
#define L_UNIARRAY 18
#define L_NULL 19
#define L_METHODREF 20
#define L_VOID 21
#define L_GENERIC 22
#define L_POINTER 23

//define short-hands for common integer type names
#define L_SHORT L_S2
#define L_INT L_S4
#define L_LONG L_S8
#define L_BYTE L_S1

//bitmasks
#define L_STATIC 0x0001
#define L_FINAL 0x0002
#define L_NATIVE 0x0004
#define L_PRIVATE 0x0008
#define L_INTERFACE 0x0010
#define L_ENUM 0x0020
#define L_NOSUPER 0x0040
#define L_NOCHECK 0x0080
#define L_SYNCHRONIZED 0x0100
#define L_VIRTUAL 0x0200
#define L_VOLATILE 0x0400
#define L_THROWS 0x0800
#define L_ISBITSET(BM, B) (((BM) & (B)) == (B))

//Thread states
#define L_TREADY 0
#define L_TACTIVE 1
#define L_TBLOCKED 2
#define L_TSUSPENDED 3
#define L_TDEAD 4
#define L_TTHROW 5

//Generic platform independent types
typedef uint8_t u1;
typedef int8_t s1;
typedef uint16_t u2;
typedef int16_t s2;
typedef uint32_t u4;
typedef int32_t s4;
typedef uint64_t u8;
typedef int64_t s8;

//print-formats for int types
#define L_PU1 "%" PRIu8
#define L_PU2 "%" PRIu16
#define L_PU4 "%" PRIu32
#define L_PU8 "%" PRIu64
#define L_PS1 "%" PRId8
#define L_PS2 "%" PRId16
#define L_PS4 "%" PRId32
#define L_PS8 "%" PRId64

//forward declarations
typedef struct lValue_ lValue;
typedef struct lclass_ lclass;
typedef struct lstaticobject_ lstaticobject;
typedef struct lobject_ lobject;
typedef struct _LVM LVM;

//'complicated' VM types

//the primitive string typedef
typedef struct{
	u4 length;
	u1* chars;
} lstring;

//lvm array type
typedef struct{
    u1 type;
    u4 length;
    lValue* arr;
	u1 reachable;
} larray;

//all posible types together
typedef union{
    u1 u1Val;
    u2 u2Val;
    u4 u4Val;
    u8 u8Val;
    s1 s1Val;
    s2 s2Val;
    s4 s4Val;
    s8 s8Val;
    float floatVal;
    double doubleVal;
    lstring* stringVal;
    lobject* objectVal;
    larray* arrayVal;
    void* pointerVal;
} LVM_VAL;

//representation of all types and values
typedef struct lValue_{
    u1 type;
    LVM_VAL val;
} lValue;

//for better debug messages
//used to map bytecode offsets to their coresponding line numbers
typedef struct{
	u2 bytecodeOffset;
	u2 lineNumber;	
} LVM_LineTableEntry;

//used for exception handling
//specifies a bytecode-range and an exception to handle in that range
typedef struct{
	u2 startOffset;
	u2 endOffset;
	u2 exceptionOffset;
	u2 exceptionRef;
} LVM_ExceptionEntry;

//needed for the native interface
//resembles a native c/c++ function
typedef u1 (*LVM_simpleFunc)(struct LNI_*);

//a method that can be interpreted by the vm or run natively
typedef struct{
    u2 descriptor;
    u1 registerCount;
    u1 paramCount;
    u1* params;
    u2 bitmask;
	u2 ltSize;
	LVM_LineTableEntry* lineTable;
	u1 etSize;
	LVM_ExceptionEntry* exceptionTable;
    u2 bcSize;
    u1* bytecodes;
    lclass* cls;
    LVM_simpleFunc nativeFunc;
} lmethod;

//a reference entry from a classes reference table
typedef struct{
    u1 type;
    u2 byteCount;
    u1* bytes;
} LVM_Reference;

//a runtime representation of an variable-entry
typedef struct{
	u2 desc;
	u1 bitmask;
	u1 type;
} LVM_VariableEntry;

//the runtime representation of a loaded class
typedef struct lclass_{
	u1 classID;
    u2 bitmask;
    u2 instVarCount;
	LVM_VariableEntry* instVars;
    u2 classVarCount;
	LVM_VariableEntry* classVars;
    u1 interfaceCount;
	u2* interfaces;
    u2 rtSize;
    LVM_Reference* refTable;
    u2 mtSize;
    lmethod* metTable;
    lstaticobject* staticObj;
} lclass;

//the runtime representation of an static object
//it holds all class variables shared by all objects
//of a certain class type
typedef struct lstaticobject_{
    lclass* cls;
    u2 classVarCount;
    lValue* classVariables;
} lstaticobject;

//the runtime representation of an object
//it holds all instance variables of an
//instance of a certain class type
typedef struct lobject_{
    lclass* cls;
    u2 instVarCount;
    lValue* instanceVariables;
	u1 reachable;
} lobject;

//the runtime representation of a Thread
//acts like a per thread state
typedef struct{
    LVM* lvm;
    lmethod* currentMethod;
    u2 pc;
    lValue* registers;
    u1 state;
	lobject* curExcep;
} lThread;

//needed for the native interface
//resembles a dynmic library and its function
typedef struct{
    const char* name;
    LVM_simpleFunc f;
} LVM_SimpleLibrary;

//for more convenience
//the function signature for registering a simple library
typedef LVM_SimpleLibrary* (*LVM_regSimLib)(void);

//for better debug-messages
//used to create a virtual call stack
typedef struct{
	const char* funcName;
	const char* fullPath;
	const char* fileName;
	u4 lineNumber;
	u1 valid;
} LVM_CallInfo;

//the runtime representation of the VM
//acts like a global state of the VM
typedef struct _LVM{
    std::vector<void*> mem;
    std::vector<lclass*> loadedClasses;
    std::vector<lThread*> threads;
    std::vector<lobject*> objects;
    std::vector<larray*> arrays;
    std::set<lValue*> objectValues;
	std::set<lValue*> arrayValues;
    std::vector<void*> dynamicLibraries;
    std::vector<LVM_SimpleLibrary> simpleLibraries;
	std::stack<LVM_CallInfo> callStack;
    lobject* NULLOBJ;
    lValue voidValue;
	u4* options;
} LVM;

//function prototypes
LVM* LVM_createVM(u4*);
void LVM_exitVM(LVM*);
void LVM_newThread(LVM*, lThread*);
void LVM_killThread(lThread*);
void* LVM_alloc(LVM*, size_t, size_t);
void LVM_free(LVM*, void*);
void LVM_freeAll(LVM*);
void LVM_gc(LVM*);
void LVM_trackObject(LVM*, lobject*);
void LVM_untrackObject(LVM*, lobject*);
void LVM_trackArray(LVM*, larray*);
void LVM_untrackArray(LVM*, larray*);
u1 LVM_loadSimpleLibrary(struct LNI_*);
void LVM_sighandler(int);
void LVM_printStackTrace(LVM*);

#endif // LVM_H_INCLUDED
