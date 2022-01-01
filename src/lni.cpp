#include <lni.hpp>
#include <lvm.hpp>
#include <Error.hpp>
#include <Types.hpp>
#include <Runtime.hpp>

LNI LVM_getNewLNI(lThread* t, u1 paramCount, lValue* params){
    LNI lni;

    lni.getParam = &LNI_getParam;
    lni.setParam = &LNI_setParam;
    lni.returnValue = &LNI_returnValue;
    lni.throwException = &LNI_throwException;
    lni.getClass = &LNI_getClass;
    lni.newObject = &LNI_newObject;
	lni.finalizeObject = &LNI_finalizeObject;
    lni.deleteObject = &LNI_deleteObject;
    lni.newArray = &LNI_newArray;
	lni.getArrayLength = &LNI_getArrayLength;
    lni.deleteArray = &LNI_deleteArray;
    lni.newString = &LNI_newString;
	lni.getStringLength = &LNI_getStringLength;
    lni.deleteString = &LNI_deleteString;
    lni.toCString = &LNI_toCString;
    lni.getStaticMember = &LNI_getStaticMember;
    lni.setStaticMember = &LNI_setStaticMember;
    lni.getMember = &LNI_getMember;
    lni.setMember = &LNI_setMember;
	lni.callMethod = &LNI_callMethod;
	lni.callStaticMethod = &LNI_callStaticMethod;
    lni.allocate = &LNI_allocate;
    lni.free = &LNI_free;

    lni.thread = t;
    lni.paramCount = paramCount;
    lni.params = params;

    return lni;
}

lValue* LNI_getParam(LNI* lni, u1 idx){
    if(lni == NULL){
		LVM_throwExceptionNew(lni->thread, "std.core.NullPointerException", "In LNI / getParam()");
        return NULL;
    }

    if(idx >= lni->paramCount){
		LVM_throwExceptionNew(lni->thread, "std.core.IndexOutOfBoundsException", "In LNI / getParam()");
        return NULL;
    }
    return &(lni->params[idx]);
}

void LNI_setParam(LNI* lni, u1 idx, lValue* v){
    if(lni == NULL || v == NULL){
		LVM_throwExceptionNew(lni->thread, "std.core.NullPointerException", "In LNI / setParam()");
        return;
    }

    if(idx >= lni->paramCount){
		LVM_throwExceptionNew(lni->thread, "std.core.IndexOutOfBoundsException", "In LNI / setParam()");
    }
    lni->params[idx].type = v->type;
    lni->params[idx].val = v->val;
}

void LNI_returnValue(LNI* lni, lValue* v){
    if(lni == NULL || v == NULL){
		LVM_throwExceptionNew(lni->thread, "std.core.NullPointerException", "In LNI / returnValue()");
        return;
    }

    lni->paramCount = 0;
    lni->params[0].type = v->type;
    lni->params[0].val = v->val;
}

void LNI_throwException(LNI* lni, lobject* o){
    if(lni == NULL || o == NULL){
		LVM_throwExceptionNew(lni->thread, "std.core.NullPointerException", "In LNI / throwException()");
        return;
    }
	
	LVM_throwException(lni->thread, o);
	
    lni->paramCount = 0;
}

lclass* LNI_getClass(LNI* lni, char* c){
    if(lni == NULL || c == NULL){
		LVM_throwExceptionNew(lni->thread, "std.core.NullPointerException", "In LNI / getClass()");
        return NULL;
    }

    return LVM_findClassByDescriptor(lni->thread, c);
}

lobject* LNI_newObject(LNI* lni, lclass* c){
    if(lni == NULL || c == NULL){
		LVM_throwExceptionNew(lni->thread, "std.core.NullPointerException", "In LNI / newObject()");
        return NULL;
    }

    return LVM_createNewObject(lni->thread, c);
}

void LNI_finalizeObject(LNI* lni, lobject* o){
	if(lni == NULL || o == NULL){
		LVM_throwExceptionNew(lni->thread, "std.core.NullPointerException", "In LNI / finalizeObject()");
        return;
    }
	
	LVM_finalizeObject(lni->thread, o);
}

void LNI_deleteObject(LNI* lni, lobject* o){
    if(lni == NULL || o == NULL){
		LVM_throwExceptionNew(lni->thread, "std.core.NullPointerException", "In LNI / deleteObject");
        return;
    }

    lni->free(lni, o->instanceVariables);
    lni->free(lni, o);
}

larray* LNI_newArray(LNI* lni, u1 type, u4 length){
    if(lni == NULL){
		LVM_throwExceptionNew(lni->thread, "std.core.NullPointerException", "In LNI / newArray()");
        return NULL;
    }

    if(LVM_INVTYPE(type)){
        LVM_error(lni->thread->lvm, LVM_TYPEERR, "Array cannot have type: %s!\n", T2N(type));
    }

    return LVM_createNewArray(lni->thread, length, type);
}

u4 LNI_getArrayLength(LNI* lni, larray* a){
	if(lni == NULL || a == NULL){
		LVM_throwExceptionNew(lni->thread, "std.core.NullPointerException", "In LNI / getArrayLength()");
		return 0;
    }
	return a->length;
}

void LNI_deleteArray(LNI* lni, larray* a){
    if(lni == NULL || a == NULL){
		LVM_throwExceptionNew(lni->thread, "std.core.NullPointerException", "In LNI / deleteArray()");
        return;
    }
    lni->free(lni, a->arr);
    lni->free(lni, a);
}

lstring* LNI_newString(LNI* lni, const char* str){
    if(lni == NULL || str == NULL){
		LVM_throwExceptionNew(lni->thread, "std.core.NullPointerException", "In LNI / newString()");
        return NULL;
    }
    return LVM_createString(lni->thread->lvm, str);
}

u4 LNI_getStringLength(LNI* lni, lstring* s){
	if(lni == NULL || s == NULL){
		LVM_throwExceptionNew(lni->thread, "std.core.NullPointerException", "In LNI / getStringLength()");
        return 0;
    }
	return s->length;
}

void LNI_deleteString(LNI* lni, lstring* s){
    if(lni == NULL || s == NULL){
		LVM_throwExceptionNew(lni->thread, "std.core.NullPointerException", "In LNI / deleteString()");
        return;
    }
    lni->free(lni, s);
}

char* LNI_toCString(LNI* lni, lstring* s){
    if(lni == NULL || s == NULL){
		LVM_throwExceptionNew(lni->thread, "std.core.NullPointerException", "In LNI / toCString()");
        return NULL;
    }
    return (char*) s->chars;
}

lValue* LNI_getStaticMember(LNI* lni, lclass* c, char* name){
    if(lni == NULL || c == NULL || name == NULL){
		LVM_throwExceptionNew(lni->thread, "std.core.NullPointerException", "In LNI / getStaticMember()");
        return NULL;
    }
    return LVM_findClassFieldByName(c, name, 0);
}

void LNI_setStaticMember(LNI* lni, lclass* c, char* name, lValue* v){
    if(lni == NULL || c == NULL || name == NULL || v == NULL){
		LVM_throwExceptionNew(lni->thread, "std.core.NullPointerException", "In LNI / setStaticMember()");
        return;
    }

    lValue* vv = LVM_findClassFieldByName(c, name, 1);

    if(vv == NULL){
		LVM_throwExceptionNew(lni->thread, "std.core.NullPointerException", "In LNI / setStaticMember() / field not found or final (and cannot be changed)");
        return;
    }

    if(vv->type !=L_GENERIC && vv->type != v->type){
        LVM_error(lni->thread->lvm, LVM_TYPEERR, "In LNI / setStaticMember() / field and value do not have same type: %s, %s!", T2N(vv->type), T2N(v->type));
    }

	vv->type = v->type;
    vv->val = v->val;
}

lValue* LNI_getMember(LNI* lni, lobject* o, char* name){
    if(lni == NULL || o == NULL || name == NULL){
		LVM_throwExceptionNew(lni->thread, "std.core.NullPointerException", "In LNI / getMember()");
        return NULL;
    }

    return LVM_findInstanceFieldByName(lni->thread, o, name, 0);
}

void LNI_setMember(LNI* lni, lobject* o, char* name, lValue* v){
    if(lni == NULL || o == NULL || name == NULL || v == NULL){
		LVM_throwExceptionNew(lni->thread, "std.core.NullPointerException", "In LNI / setMember()");
        return;
    }

    lValue* vv = LVM_findInstanceFieldByName(lni->thread, o, name, 1);

    if(vv == NULL){
		LVM_throwExceptionNew(lni->thread, "std.core.NullPointerException", "In LNI / setMember() / field not found or final (and cannot be changed)");
        return;
    }

    if(vv->type != L_GENERIC && vv->type != v->type){
        LVM_error(lni->thread->lvm, LVM_TYPEERR, "In LNI / setMember() / field and value do not have same type: %s, %s!", T2N(vv->type), T2N(v->type));
    }

	vv->type = v->type;
    vv->val = v->val;
}

lValue LNI_callMethod(LNI* lni, lobject* o, const char* desc, lValue* params, u1 paramCount){
	if(lni == NULL || o == NULL || desc == NULL || params == NULL){
		LVM_throwExceptionNew(lni->thread, "std.core.NullPointerException", "In LNI / callMethod()");
		return lni->thread->lvm->voidValue;
	}
	
	lmethod* m = LVM_findMethodByDescriptor(o->cls, desc);
	
	if(m == NULL){
		LVM_error(lni->thread->lvm, LVM_NOMETHOD, "Method %s not found in class %s!", desc, (char*) o->cls->refTable[0].bytes);
	}
	
	lValue vals[paramCount + 1];
	vals[0].type = L_OBJECT;
	vals[0].val.objectVal = o;
	
	u1 i;
	for(i = 0; i < paramCount; i++){
		vals[i + 1].type = params[i].type;
		vals[i + 1].val = params[i].val;
	}
	
	return LVM_callMethod(lni->thread, m, vals);
}

lValue LNI_callStaticMethod(LNI* lni, lclass* c, const char* desc, lValue* params){
	if(lni == NULL || c == NULL || desc == NULL || params == NULL){
		LVM_throwExceptionNew(lni->thread, "std.core.NullPointerException", "In LNI / callmethod()");
		return lni->thread->lvm->voidValue;
	}
	
	lmethod* m = LVM_findMethodByDescriptor(c, desc);
	
	if(m == NULL){
		LVM_error(lni->thread->lvm, LVM_NOMETHOD, "Method %s not found in class %s!", desc, (char*) c->refTable[0].bytes);
	}
	
	return LVM_callMethod(lni->thread, m, params);
}

void* LNI_allocate(LNI* lni, size_t size){
    if(lni == NULL){
		LVM_throwExceptionNew(lni->thread, "std.core.NullPointerException", "In LNI / allocate()");
        return NULL;
    }

    return LVM_alloc(lni->thread->lvm, size, 1);
}

void LNI_free(LNI* lni, void* ptr){
    if(lni == NULL){
		LVM_throwExceptionNew(lni->thread, "std.core.NullPointerException", "In LNI / free()");
        return;
    }

    LVM_free(lni->thread->lvm, ptr);
}
