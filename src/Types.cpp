#include <Types.hpp>
#include <Error.hpp>
#include <Runtime.hpp>
#include <lni.hpp>
#include <cstring>
#include <iostream>

lstring* LVM_createString(LVM* lvm, const char* str){
    return LVM_createStringWithLen(lvm, str, (u4) strlen(str));
}

lstring* LVM_createStringWithLen(LVM* lvm, const char* str, u4 len){
    lstring* s = (lstring*) LVM_alloc(lvm, sizeof(lstring), 1);
	s->chars = (u1*) LVM_alloc(lvm, sizeof(u1), len + 1);
    u4 i;
    for(i = 0; i < len; i++){
        s->chars[i] = str[i];
    }
    s->chars[len] = '\0';
    s->length = len;
    return s;
}

lstring* LVM_createStringFromArray(LVM* lvm, larray* arr){
    lstring* s = (lstring*) LVM_alloc(lvm, sizeof(lstring), 1);
	s->chars = (u1*) LVM_alloc(lvm, sizeof(u1), arr->length + 1);
    u4 i;
    for(i = 0; i < arr->length; i++){
        s->chars[i] = arr->arr[i].val.u1Val;
    }
    s->chars[arr->length] = '\0';
    s->length = arr->length;
    return s;
}

lclass* LVM_findClassByDescriptor(lThread* t, const char* clsDesc){
    u2 i;
    for(i = 0; i < t->lvm->loadedClasses.size(); i++){
        lclass* cls = t->lvm->loadedClasses[i];
        LVM_Reference* r = &(cls->refTable[0]);
        if(!strcmp((char*) r->bytes, clsDesc)){
            return cls;
        }
    }
    return NULL;
}

lmethod* LVM_findMethodByDescriptor(lclass* cls, const char* mDesc){
    u2 i;
    for(i = 0; i < cls->mtSize; i++){
        lmethod* m = &(cls->metTable[i]);
        LVM_Reference* r = &(cls->refTable[m->descriptor]);
        if(!strcmp((char*) r->bytes, mDesc)){
            return m;
        }
    }
    return NULL;
}

void LVM_methodDesc2Type(char* desc, u1* params){
    size_t i;
    size_t len = strlen(desc);
    u1 paramPos = 0;
    u1 state = 0, extraState = 0;
    for(i = 0; i < len; i++){
        if(extraState == 1){
            if(desc[i] == 'R'){
                extraState = 2;
            }
            continue;
        }else if(extraState == 2){
            if(desc[i] == ';'){
                extraState = 0;
            }
            continue;
        }

        if(desc[i] == '(' && state == 0){
            state = 1;
        }else if(desc[i] == ')'){
            break;
        }else if(state == 1){
            if(desc[i] == 'B'){
                params[paramPos] = L_BOOLEAN;
            }else if(desc[i] == 'C'){
                params[paramPos] = L_CHAR;
            }else if(desc[i] == 'F'){
                params[paramPos] = L_FLOAT;
            }else if(desc[i] == 'D'){
                params[paramPos] = L_DOUBLE;
            }else if(desc[i] == 'S'){
                params[paramPos] = L_STRING;
            }else if(desc[i] == 'V'){
                params[paramPos] = L_VOID;
            }else if(desc[i] == 'U'){
                params[paramPos] = L_UNIARRAY;
            }else if(desc[i] == 'M'){
                params[paramPos] = L_METHODREF;
            }else if(desc[i] == 'G'){
                params[paramPos] = L_GENERIC;
            }else if(desc[i] == 'P'){
                params[paramPos] = L_POINTER;
            }else if(desc[i] == '['){
                params[paramPos] = L_ARRAY;
                extraState = 1;
            }else if(desc[i] == 'R'){
                params[paramPos] = L_OBJECT;
                extraState = 2;
            }else if(desc[i] == '1'){
                params[paramPos] = L_S1;
            }else if(desc[i] == '2'){
                params[paramPos] = L_S2;
            }else if(desc[i] == '3'){
                params[paramPos] = L_S4;
            }else if(desc[i] == '4'){
                params[paramPos] = L_S8;
            }else if(desc[i] == '3'){
                params[paramPos] = L_U1;
            }else if(desc[i] == '5'){
                params[paramPos] = L_U2;
            }else if(desc[i] == '6'){
                params[paramPos] = L_U4;
            }else if(desc[i] == '7'){
                params[paramPos] = L_U8;
            }
            paramPos++;
        }
    }
}

u1 LVM_refToVal(LVM* lvm, LVM_Reference* r, lValue* v){
    if(LVM_INVREFTYPE(r->type)){
        return 0;
    }

    if(r->type == L_NULL){
        v->type = r->type;
        v->val.objectVal = lvm->NULLOBJ;
        return 1;
    }else if(LVM_ISWHOLE(r->type)){
        s8 val = 0;
        u1 i;
        for(i = 0; i < r->byteCount; i++){
            val <<= 8;
            val += r->bytes[i];
        }
        v->type = r->type;
        v->val.s8Val = val;
        LVM_convertIntVal(v);
        return 1;
    }else if(r->type == L_BOOLEAN || r->type == L_CHAR){
        v->type = r->type;
        v->val.u1Val = r->bytes[0];
        return 1;
    }else if(r->type == L_STRING){
        v->type = r->type;
        v->val.stringVal = LVM_createStringWithLen(lvm, (char*) r->bytes, r->byteCount);
        return 1;
    }else if(r->type == L_FLOAT){
        v->type = r->type;
        s4 f = 0;
        u1 i;
        for(i = 0; i < r->byteCount; i++){
            f <<= 0;
            f += r->bytes[i];
        }
        v->val.s4Val = f;
        return 1;
    }else if(r->type == L_DOUBLE){
        v->type = r->type;
        s8 d = 0;
        u1 i;
        for(i = 0; i < r->byteCount; i++){
            d <<= 0;
            d += r->bytes[i];
        }
        v->val.s8Val = d;
        return 1;
    }

    return 0;
}

s8 LVM_toIntVal(lValue* v){
    if(v->type == L_U1){
        return (s8) v->val.u1Val;
    }else if(v->type == L_U2){
        return (s8) v->val.u2Val;
    }else if(v->type == L_U4){
        return (s8) v->val.u4Val;
    }else if(v->type == L_U8){
        return (s8) v->val.u8Val;
    }else if(v->type == L_S1){
        return (s8) v->val.s1Val;
    }else if(v->type == L_S2){
        return (s8) v->val.s2Val;
    }else if(v->type == L_S4){
        return (s8) v->val.s4Val;
    }else if(v->type == L_S8){
        return (s8) v->val.s8Val;
    }
    return 0;
}

void LVM_convertIntVal(lValue* v){
    s8 val = v->val.s8Val;
    if(v->type == L_U1){
        v->val.u1Val = (u1) val;
    }else if(v->type == L_U2){
        v->val.u2Val = (u2) val;
    }else if(v->type == L_U4){
        v->val.u4Val = (u4) val;
    }else if(v->type == L_U8){
        v->val.u8Val = (u8) val;
    }else if(v->type == L_S1){
        v->val.s1Val = (s1) val;
    }else if(v->type == L_S2){
        v->val.s2Val = (s2) val;
    }else if(v->type == L_S4){
        v->val.s4Val = (s4) val;
    }else if(v->type == L_S8){
        v->val.s8Val = (s8) val;
    }
}

lValue* LVM_findInstanceFieldByName(lThread* t, lobject* obj, const char* name, u1 checkFinal){
	char* fieldClassDesc = strrchr(name, ':');
	lclass* fieldClass = LVM_findClassByDescriptor(t, fieldClassDesc);
	
	if(fieldClass == obj->cls){
		u2 i;
		for(i = 0; i < obj->instVarCount; i++){
			LVM_VariableEntry* e = &((obj->cls->instVars)[i]);
			LVM_Reference* r = &((obj->cls->refTable)[e->desc]);
			if(!strcmp((char*) r->bytes, name)){
				if(checkFinal && L_ISBITSET(e->bitmask, L_FINAL)){
					return NULL;
				}
				return &((obj->instanceVariables)[i + 1]);
			}
		}
		return NULL;
	}else{
		u1 dimension = obj->cls->classID - fieldClass->classID;
		larray* a = (obj->instanceVariables)[0].val.arrayVal;
		u2 i;
		for(i = 1; i < dimension; i++){
			a = (a->arr)[0].val.arrayVal;
		}
		for(i = 0; i < a->length; i++){
			LVM_VariableEntry* e = &((fieldClass->instVars)[i]);
			LVM_Reference* r = &(fieldClass->refTable)[e->desc];
			if(!strcmp((char*) r->bytes, name)){
				if(checkFinal && L_ISBITSET(e->bitmask, L_FINAL)){
					return NULL;
				}
				return &((a->arr)[i + (fieldClass->classID == 0 ? 0 : 1)]);
			}
		}
	}   
	return NULL;
}

lValue* LVM_findClassFieldByName(lclass* cls, const char* name, u1 checkFinal){
    u2 i;
    for(i = 0; i < cls->classVarCount; i++){
        LVM_VariableEntry* e = &((cls->classVars)[i]);
        LVM_Reference* r = &((cls->refTable)[e->desc]);
        if(!strcmp((char*) r->bytes, name)){
            if(checkFinal && L_ISBITSET(e->bitmask, L_FINAL)){
                return NULL;
            }
            return &((cls->staticObj->classVariables)[i]);
        }
    }
    return NULL;
}

char* LVM_getMethodName(lmethod* m){
	LVM_Reference* r = &(m->cls->refTable[m->descriptor]);
	char* desc = (char*) r->bytes;
	char* bracket = strrchr(desc, ')');
	if(!bracket || bracket == desc){
		return desc;
	}
	return bracket + 1;
}

char* LVM_getFullClassNameFromMethod(lmethod* m){
	LVM_Reference* r = &(m->cls->refTable[0]);
	return (char*) r->bytes;
}

char* LVM_getClassNameFromMethod(lmethod* m){
	char* fullName = LVM_getFullClassNameFromMethod(m);
	char* dot = strrchr(fullName, '.');
	if(!dot || dot == fullName){
		return fullName;
	}
	return dot + 1;
}

u2 LVM_getLineNumber(lmethod* m, u2 bo){
	u2 i;
	for(i = 0; i < m->ltSize; i++){
		if((m->lineTable)[i].bytecodeOffset == bo){
			return (m->lineTable)[i].lineNumber;
		}else if((m->lineTable)[i].bytecodeOffset > bo){
			return (m->lineTable)[i - 1].lineNumber;
		}
	}
	return 0;
}

LVM_CallInfo LVM_getCallInfo(lmethod* m, u2 bo){
	LVM_CallInfo ci;
	ci.funcName = LVM_getMethodName(m);
	ci.fullPath = LVM_getFullClassNameFromMethod(m);
	ci.fileName = LVM_getClassNameFromMethod(m);
	
	if(L_ISBITSET(m->bitmask, L_NATIVE)){
		ci.valid = 2;
	}else if(m->ltSize == 0){
		ci.valid = 0;
	}else{
		ci.valid = 1;
		ci.lineNumber = LVM_getLineNumber(m, bo);
	}
	
	return ci;
}

u1 LVM_isInstanceOf(lThread* t, lobject* o, const char* clsName){
    lclass* myClass = o->cls;

    lclass* lookup = LVM_findClassByDescriptor(t, clsName);
    if(lookup == NULL){
        LVM_error(t->lvm, LVM_NOCLASS, "%s not loaded!", clsName);
    }

    if(L_ISBITSET(lookup->bitmask, L_INTERFACE)){
        if(myClass->interfaceCount == 0){
            return 0;
        }
        u1 i;
        for(i = 0; i < myClass->interfaceCount; i++){
            LVM_Reference* rr = &(myClass->refTable[myClass->interfaces[i]]);
            char* comp = (char*) rr->bytes;
            if(!strcmp(comp, clsName)){
                return 1;
            }
        }
    }else{
        lclass* toCheck = myClass;
        while(toCheck != NULL){
            LVM_Reference* rr = &(toCheck->refTable[0]);
            char* comp = (char*) rr->bytes;
            if(!strcmp(comp, clsName)){
                return 1;
            }
            toCheck = LVM_findClassByDescriptor(t, (char*) toCheck->refTable[1].bytes);
        }

    }

    return 0;
}

char* LVM_getFullClassNameFromObject(lobject* o){
	LVM_Reference* r = &(o->cls->refTable[0]);
	return (char*) r->bytes;
}

void LVM_finalizeObject(lThread* t, lobject* o){
	lmethod* fM = LVM_findMethodByDescriptor(o->cls, "V()finalize");
	if(fM == NULL){
		LVM_error(t->lvm, LVM_NOMETHOD, "finalize method in class %s not found!", LVM_getFullClassNameFromObject(o));
	}
	
	lValue thisRef;
	thisRef.type = L_OBJECT;
	thisRef.val.objectVal = o;
	
	LVM_callMethod(t, fM, &thisRef);
}

lobject* LVM_createNewObject(lThread* t, lclass* toCreate){
	u1 hasSuper = !L_ISBITSET(toCreate->bitmask, L_NOSUPER);
	
    lobject* obj = (lobject*) LVM_alloc(t->lvm, sizeof(lobject), 1);
    obj->cls = toCreate;
    obj->instVarCount = toCreate->instVarCount;
    obj->instanceVariables = (lValue*) LVM_alloc(t->lvm, sizeof(lValue), obj->instVarCount + hasSuper);
	
    u2 i;
    for(i = 0; i < obj->instVarCount; i++){
        (obj->instanceVariables)[i + hasSuper].type = (toCreate->instVars)[i].type;
    }
	
	if(hasSuper){
		lclass* currentClass = LVM_findClassByDescriptor(t, (char*) (toCreate->refTable)[1].bytes);
		
		if(currentClass == NULL){
			LVM_error(t->lvm, LVM_NOCLASS, "%s not loaded!", (char*) (toCreate->refTable)[1].bytes);
		}
		
		hasSuper = !L_ISBITSET(toCreate->bitmask, L_NOSUPER);
		larray* a = LVM_createNewArray(t, currentClass->instVarCount + hasSuper, L_UNIARRAY);
		(obj->instanceVariables)[0].type = L_UNIARRAY;
		(obj->instanceVariables)[0].val.arrayVal = a;
		
		for(i = 0; i < currentClass->instVarCount; i++){
			a->arr[i + hasSuper].type = (currentClass->instVars)[i].type;
		}
		
		while(hasSuper){
			char* tempName = (char*) (currentClass->refTable)[1].bytes;
			currentClass = LVM_findClassByDescriptor(t, tempName);
			
			if(currentClass == NULL){
				LVM_error(t->lvm, LVM_NOCLASS, "%s not loaded!", tempName);
			}
		
			hasSuper = !L_ISBITSET(currentClass->bitmask, L_NOSUPER);
			larray* next = LVM_createNewArray(t, currentClass->instVarCount + hasSuper, L_UNIARRAY);
			a->arr[0].type = L_UNIARRAY;
			a->arr[0].val.arrayVal = next;
			
			for(i = 0; i < currentClass->instVarCount; i++){
				next->arr[i + hasSuper].type = (currentClass->instVars)[i].type;
			}
			a = next;
		}
	}
	
	return obj;
}

larray* LVM_createNewArray(lThread* t, u4 size, u1 type){
    larray* arr = (larray*) LVM_alloc(t->lvm, sizeof(larray), 1);
    arr->type = type;
    arr->length = size;
    arr->arr = (lValue*) LVM_alloc(t->lvm, sizeof(lValue), size);
	return arr;
}

void LVM_nullOutObject(lThread* t, lobject* o){
	auto it = t->lvm->objectValues.begin();
	while(it != t->lvm->objectValues.end()){
		if((*it)->type == L_OBJECT && (*it)->val.objectVal == o){
			(*it)->type = L_NULL;
			(*it)->val.objectVal = t->lvm->NULLOBJ;
			it = t->lvm->objectValues.erase(it);
		}else if((*it)->type != L_OBJECT){
			it = t->lvm->objectValues.erase(it);
		}
	}
}

void LVM_nullOutArray(lThread* t, larray* a){
	auto it = t->lvm->arrayValues.begin();
	while(it != t->lvm->arrayValues.end()){
		if(((*it)->type == L_ARRAY || (*it)->type == L_UNIARRAY) && (*it)->val.arrayVal == a){
			(*it)->type = L_NULL;
			(*it)->val.objectVal = t->lvm->NULLOBJ;
			it = t->lvm->arrayValues.erase(it);
		}else if((*it)->type != L_ARRAY && (*it)->type != L_UNIARRAY){
			it = t->lvm->arrayValues.erase(it);
		}
	}
}

lValue LVM_callMethod(lThread* t, lmethod* m, lValue* params){
	if(L_ISBITSET(m->bitmask, L_NATIVE)){
		return LVM_callNativeMethod(t, m, params);
	}
	
	if(!LVM_checkMethodArgs(t, m, params)){
		return t->lvm->voidValue;
	}
	
	lmethod* currentMethod = t->currentMethod;
	u2 currentPC = t->pc;
	lValue* currentRegisters = t->registers;
	
	t->currentMethod = m;
	t->pc = 0;
	t->registers = (lValue*) LVM_alloc(t->lvm, sizeof(lValue), m->registerCount);
	
	u1 isNotStatic = L_ISBITSET(m->bitmask, L_STATIC) ? 0 : 1;
	
	u1 i;
	for(i = 0; i < m->paramCount + isNotStatic; i++){
		(t->registers)[i].type = params[i].type;
		(t->registers)[i].val = params[i].val;
	}
	
	lValue ret = LVM_execute(t);
	
	LVM_free(t->lvm, t->registers);
	t->registers = currentRegisters;
	t->pc = currentPC;
	t->currentMethod = currentMethod;
	
	return ret;
}

u1 LVM_checkMethodArgs(lThread* t, lmethod* m, lValue* params){
	if(L_ISBITSET(m->bitmask, L_NOCHECK)){
		return 1;
	}
	
	u1 i, success = 1;
	for(i = 0; i < m->paramCount; i++){
		if(m->params[i] == L_GENERIC){
			continue;
		}
		
		if(params[i].type != m->params[i]){
			if(t != NULL){
				std::cout << "\nMethod parameter " << i + 1 << " requires " << T2N(m->params[i]) << ", got ";
				std::cout << T2N(params[i].type) << "!" << std::endl;
			}
			success = 0;
		}
	}
	
	if(!success && t != NULL){
		LVM_error(t->lvm, LVM_TYPEERR, "Called method with wrong types!");
	}
	
	return success;
}

lValue LVM_callNativeMethod(lThread* t, lmethod* m, lValue* params){
	if(!L_ISBITSET(m->bitmask, L_NATIVE)){
		return LVM_callMethod(t, m, params);
	}
	
	if(!LVM_checkMethodArgs(t, m, params)){
		return t->lvm->voidValue;
	}
	
	lValue ret;
	
	if(m->paramCount == 0){
		params = &ret;
	}
	
	u1 isNotStatic = L_ISBITSET(m->bitmask, L_STATIC) ? 0 : 1;
	
	LVM_CallInfo ci = LVM_getCallInfo(m, t->pc);
	t->lvm->callStack.push(ci);
	
	LNI lni = LVM_getNewLNI(t, m->paramCount + isNotStatic, params);
	u1 retCount = m->nativeFunc(&lni);
	
	t->lvm->callStack.pop();
	
	if(retCount == 0){
		return t->lvm->voidValue;
	}else{
		return params[0];
	}
}

lValue LVM_callOperatorMethod(lThread* t, const char* mDesc, lValue* lhs, lValue* arrayIdx, lValue* rhs){
	lclass* methodClass = NULL;
	lmethod* operatorMethod = NULL;
	u1 nonNull = 0;
	lValue params[3];
	
	if(lhs != NULL){
		nonNull++;
		
		params[0].type = lhs->type;
		params[0].val = lhs->val;
	}
	
	if(arrayIdx != NULL){
		nonNull++;
		params[nonNull - 1].type = arrayIdx->type;
		params[nonNull - 1].val = arrayIdx->val;
	}
	
	if(rhs != NULL){
		nonNull++;
		
		params[nonNull - 1].type = rhs->type;
		params[nonNull - 1].val = rhs->val;
	}
	
	if(lhs != NULL && lhs->type == L_OBJECT){
		methodClass = lhs->val.objectVal->cls;
		operatorMethod = LVM_findMethodDynamic(methodClass, mDesc, nonNull, params);
	}
	
	if(operatorMethod == NULL && rhs != NULL && rhs->type == L_OBJECT){
		methodClass = rhs->val.objectVal->cls;
		operatorMethod = LVM_findMethodDynamic(methodClass, mDesc, nonNull, params);
	}
	
	if(operatorMethod == NULL){
		LVM_error(t->lvm, LVM_NOMETHOD, "No operator-method found in class %s!", (char*) methodClass->refTable[0].bytes);
	}
	
	return LVM_callMethod(t, operatorMethod, params);
}

lmethod* LVM_findMethodDynamic(lclass* cls, const char* mName, u1 paramCount, lValue* params){
	u2 i;
    for(i = 0; i < cls->mtSize; i++){
        lmethod* m = &(cls->metTable[i]);
		
		if(m->paramCount != paramCount){
			continue;
		}
		
        LVM_Reference* r = &(cls->refTable[m->descriptor]);
		char* name = strchr((char*) r->bytes, ')');
        if(!strcmp(name, mName) && LVM_checkMethodArgs(NULL, m, params)){
            return m;
        }
    }
    return NULL;
}

void LVM_throwException(lThread* t, lobject* o){
	if(!LVM_isInstanceOf(t, o, "std.core.Exception")){
		LVM_error(t->lvm, LVM_TYPEERR, "Cannot throw object of non-exception type!");
	}
	
	t->state = L_TTHROW;
	t->curExcep = o;
}

void LVM_throwExceptionNew(lThread* t, const char* clsName, const char* msg){
	lclass* cls = LVM_findClassByDescriptor(t, clsName);
	
	if(cls == NULL){
		LVM_error(t->lvm, LVM_NOCLASS, "%s not loaded!", clsName);
	}
	
	lobject* o = LVM_createNewObject(t, cls);
	lmethod* m = LVM_findMethodByDescriptor(cls, "V(S)<init>");
	if(m == NULL){
		LVM_error(t->lvm, LVM_NOMETHOD, "Message-Constructor of exception %s not found!", clsName);
	}
	
	lstring* msgVal = LVM_createString(t->lvm, msg);
	
	lValue vals[2];
	vals[0].type = L_OBJECT;
	vals[0].val.objectVal = o;
	vals[1].type = L_STRING;
	vals[1].val.stringVal = msgVal;
	
	LVM_callMethod(t, m, vals);	
	LVM_throwException(t, o);
}