#include <ClassLoader.hpp>
#include <Error.hpp>
#include <cstring>
#include <iostream>
#include <vector>

u1 LVM_loadClass(LVM* lvm, LVM_FILE* f, lclass* cls){
    u2 i;
    u1 majorVersion = LVM_readU1(lvm, f);
    u1 minorVersion = LVM_readU1(lvm, f);

    if(majorVersion != LVM_MA_VER || minorVersion != LVM_MI_VER){
        std::cout << "Class does not match vm version!" << std::endl;
        return 0;
    }

	cls->classID = LVM_readU1(lvm, f);
    cls->bitmask = LVM_readU2(lvm, f);

    cls->instVarCount = LVM_readU2(lvm, f);
    cls->instVars = (LVM_VariableEntry*) LVM_alloc(lvm, sizeof(LVM_VariableEntry), cls->instVarCount);
    for(i = 0; i < cls->instVarCount; i++){
        (cls->instVars)[i].desc = LVM_readU2(lvm, f);
        (cls->instVars)[i].bitmask = LVM_readU2(lvm, f);
        (cls->instVars)[i].type = LVM_readU1(lvm, f);
    }

    cls->classVarCount = LVM_readU2(lvm, f);
    cls->classVars = (LVM_VariableEntry*) LVM_alloc(lvm, sizeof(LVM_VariableEntry), cls->classVarCount);
    for(i = 0; i < cls->classVarCount; i++){
        (cls->classVars)[i].desc = LVM_readU2(lvm, f);
        (cls->classVars)[i].bitmask = LVM_readU2(lvm, f);
        (cls->classVars)[i].type = LVM_readU1(lvm, f);
    }

    cls->interfaceCount = LVM_readU1(lvm, f);
    cls->interfaces = (u2*) LVM_alloc(lvm, sizeof(2), cls->interfaceCount);
    for(i = 0; i < cls->interfaceCount; i++){
        (cls->interfaces)[i] = LVM_readU2(lvm, f);
    }

    cls->rtSize = LVM_readU2(lvm, f);
    cls->refTable = (LVM_Reference*) LVM_alloc(lvm, sizeof(LVM_Reference), cls->rtSize);
    for(i = 0; i < cls->rtSize; i++){
        (cls->refTable)[i].type = LVM_readU1(lvm, f);
        u1 t = (cls->refTable)[i].type;
        u1 add = (t == L_STRING ? 1 : (t == L_CLASSDESC ? 1 : (t == L_METHODDESC ? 1 : (t == L_FIELDDESC ? 1 : 0))));
        (cls->refTable)[i].byteCount = LVM_readU2(lvm, f);
        (cls->refTable)[i].bytes = (u1*) LVM_alloc(lvm, sizeof(u1), ((cls->refTable)[i].byteCount + add));
        u1 j;
        for(j = 0; j < (cls->refTable)[i].byteCount; j++){
            ((cls->refTable)[i].bytes)[j] = LVM_readU1(lvm, f);
        }
        if(add){
            ((cls->refTable)[i].bytes)[j] = '\0';
        }
    }

    cls->mtSize = LVM_readU2(lvm, f);
    cls->metTable = (lmethod*) LVM_alloc(lvm, sizeof(lmethod), cls->mtSize);
    for(i = 0; i < cls->mtSize; i++){
        (cls->metTable)[i].descriptor = LVM_readU2(lvm, f);
        (cls->metTable)[i].registerCount = LVM_readU1(lvm, f);
        (cls->metTable)[i].paramCount = LVM_readU1(lvm, f);
		if((cls->metTable)[i].registerCount < (cls->metTable)[i].paramCount){
			LVM_error(lvm, LVM_PARMIMA, "Register count cannot be less than parameter count!");
		}
        (cls->metTable)[i].params = (u1*) LVM_alloc(lvm, sizeof(u1), (cls->metTable)[i].paramCount);
        (cls->metTable)[i].bitmask = LVM_readU2(lvm, f);
		(cls->metTable)[i].ltSize = LVM_readU2(lvm, f);
		(cls->metTable)[i].lineTable = (LVM_LineTableEntry*) LVM_alloc(lvm, sizeof(LVM_LineTableEntry), (cls->metTable)[i].ltSize);
		u2 j;
		for(j = 0; j < (cls->metTable)[i].ltSize; j++){
			((cls->metTable)[i].lineTable)[j].bytecodeOffset = LVM_readU2(lvm, f);
			((cls->metTable)[i].lineTable)[j].lineNumber = LVM_readU2(lvm, f);
		}
		
		(cls->metTable)[i].etSize = LVM_readU1(lvm, f);
		(cls->metTable)[i].exceptionTable = (LVM_ExceptionEntry*) LVM_alloc(lvm, sizeof(LVM_ExceptionEntry), (cls->metTable)[i].etSize);
		
		for(j = 0; j < (cls->metTable)[i].etSize; j++){
			((cls->metTable)[i].exceptionTable)[j].startOffset = LVM_readU2(lvm, f);
			((cls->metTable)[i].exceptionTable)[j].endOffset = LVM_readU2(lvm, f);
			((cls->metTable)[i].exceptionTable)[j].exceptionOffset = LVM_readU2(lvm, f);
			((cls->metTable)[i].exceptionTable)[j].exceptionRef = LVM_readU2(lvm, f);
		}
		
        (cls->metTable)[i].bcSize = LVM_readU2(lvm, f);
        (cls->metTable)[i].bytecodes = (u1*) LVM_alloc(lvm, sizeof(u1), (cls->metTable)[i].bcSize);
        (cls->metTable)[i].cls = cls;
        for(j = 0; j < (cls->metTable)[i].bcSize; j++){
            ((cls->metTable)[i].bytecodes)[j] = LVM_readU1(lvm, f);
        }
    }

    cls->staticObj = NULL;
    return 1;
}

char** LVM_unpackUPK(LVM* lvm, char* fName){
	return NULL;
}

u1 LVM_loadClasses(LVM* lvm, char* fName, u1 fType, std::vector<lclass*>* clses){
    if(fType == 0){
        LVM_FILE f = LVM_loadFile(lvm, fName);
		lclass* c = (lclass*) LVM_alloc(lvm, sizeof(lclass), 1);
        u1 success = LVM_loadClass(lvm, &f, c);
        LVM_freeFile(lvm, &f);
		
		if(!success){
			return 0;
		}
		
		clses->push_back(c);
		return 1;
    }else{
        char** fs = LVM_unpackUPK(lvm, fName);
        u2 i;
        for(i = 0; i < 1; i++){
			LVM_FILE f = LVM_loadFile(lvm, fs[i]);
			lclass* c = (lclass*) LVM_alloc(lvm, sizeof(lclass), 1);
            u1 success = LVM_loadClass(lvm, &f, c);
			
            LVM_freeFile(lvm, &f);
            if(!success){
                return 0;
            }
			
			clses->push_back(c);
        }
        LVM_free(lvm, fs);
		return 1;
    }
}
