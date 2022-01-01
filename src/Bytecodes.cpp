#include <Bytecodes.hpp>
#include <Comp.hpp>
#include <Error.hpp>
#include <Types.hpp>
#include <Runtime.hpp>
#include <iostream>
#include <cstring>
#include <cstdio>

void LVM_loadconstant(lThread* t){
    LVM_R1();
    LVM_RT1();

    if(LVM_INVTYPE(r->type)){
        LVM_error(t->lvm, LVM_OPERR, "Value cannot have type: %s", T2N(r->type));
    }

    v->type = r->type;

    if(!LVM_refToVal(t->lvm, r, v)){
        LVM_error(t->lvm, LVM_OPERR, "LOADCONSTANT could not convert reference-table-entry to value!");
    }
}

void LVM_move(lThread* t){
    LVM_R2();

    v1->type = v2->type;
    v1->val = v2->val;
	
	if(v1->type == L_OBJECT){
		t->lvm->objectValues.insert(v1);
	}else if(v1->type == L_ARRAY || v1->type == L_UNIARRAY){
		t->lvm->arrayValues.insert(v1);
	}
}

void LVM_new(lThread* t){
    LVM_R1();
	LVM_RT1();

    char* desc = (char*) r->bytes;

    lclass* toCreate = LVM_findClassByDescriptor(t, desc);

    if(toCreate == NULL){
        LVM_error(t->lvm, LVM_NOCLASS, "%s not loaded!", desc);
    }

	lobject* obj = LVM_createNewObject(t, toCreate);

    LVM_trackObject(t->lvm, obj);

    v->type = L_OBJECT;
    v->val.objectVal = obj;
	
	t->lvm->objectValues.insert(v);
}

void LVM_newarray(lThread* t){
    LVM_R2();
    u1 type = LVM_GETARGU1();

    if(!LVM_ISUNSIGNED(v2->type) || v2->type == L_U8){
        LVM_error(t->lvm, LVM_TYPEERR, "Array cannot have size from type: %s!", T2N(v2->type));
    }

    if(LVM_INVTYPE(type)){
        LVM_error(t->lvm, LVM_TYPEERR, "Array cannot have type: %s!", T2N(type));
    }

    larray* arr = LVM_createNewArray(t, (u4) LVM_toIntVal(v2), type);

    LVM_trackArray(t->lvm, arr);

    v1->type = L_ARRAY;
    v1->val.arrayVal = arr;
	
	t->lvm->arrayValues.insert(v1);
}

void LVM_delete(lThread* t){
    u1 ri = LVM_GETARGU1();
    lValue* v = LVM_R(ri);
    if(v->type == L_OBJECT){
        lobject* o = v->val.objectVal;

        LVM_finalizeObject(t, o);
		
		LVM_nullOutObject(t, o);

        LVM_untrackObject(t->lvm, o);

        LVM_free(t->lvm, o->instanceVariables);
        LVM_free(t->lvm, o);
        v->type = L_NULL;
    }else if(v->type == L_ARRAY || L_UNIARRAY){
        larray* arr = v->val.arrayVal;

        LVM_nullOutArray(t, arr);

        LVM_untrackArray(t->lvm, arr);

		LVM_free(t->lvm, arr->arr);
        LVM_free(t->lvm, arr);
        v->type = L_NULL;
    }else{
        LVM_error(t->lvm, LVM_TYPEERR, "Operator delete cannot be applied to type: %s!", T2N(v->type));
    }
}

void LVM_getarrayelem(lThread* t){
    LVM_R3();
	
	if(v1->type == L_OBJECT){
		lValue ret = LVM_callOperatorMethod(t, "<[]>", v1, v2, NULL);
		v3->type = ret.type;
		v3->val = ret.val;
	}else{
		if(!LVM_ISARRAY(v1->type) || !LVM_ISUNSIGNED(v2->type) || v2->type == L_U8){
			LVM_error(t->lvm, LVM_TYPEERR, "[] operator not defined for types: %s, %s!", T2N(v1->type), T2N(v2->type));
		}
		
		u4 pos = (u4) LVM_toIntVal(v2);

		if(pos >= v1->val.arrayVal->length){
			char msg[255];
			sprintf(msg, "Cannot access element at %ud, max %ud!", pos, v1->val.arrayVal->length - 1);
			LVM_throwExceptionNew(t, "std.core.IndexOutOfBoundsException", msg);
			return;
		}

		lValue* aVal = &(v1->val.arrayVal->arr[pos]);

		v3->type = aVal->type;
		v3->val = aVal->val;
	}

	if(v3->type == L_OBJECT){
		t->lvm->objectValues.insert(v3);
	}else if(v3->type == L_ARRAY || v3->type == L_UNIARRAY){
		t->lvm->arrayValues.insert(v3);
	}
}

void LVM_setarrayelem(lThread* t){
    LVM_R3();
	
	if(v1->type == L_OBJECT){
		LVM_callOperatorMethod(t, "<[]>", v1, v2, v3);
	}else{
		if(!LVM_ISARRAY(v1->type) || !LVM_ISUNSIGNED(v2->type) || v2->type == L_U8){
			LVM_error(t->lvm, LVM_TYPEERR, "[] operator not defined for types: %s, %s!", T2N(v1->type), T2N(v2->type));
		}

		u4 pos = (u4) LVM_toIntVal(v2);

		if(pos >= v1->val.arrayVal->length){
			char msg[255];
			sprintf(msg, "Cannot access element at %ud, max %ud!", pos, v1->val.arrayVal->length - 1);
			LVM_throwExceptionNew(t, "std.core.IndexOutOfBoundsException", msg);
			return;
		}

		if(v1->val.arrayVal->type != L_UNIARRAY && v3->type != v1->val.arrayVal->type){
			LVM_error(t->lvm, LVM_TYPEERR, "%s cannot be stored in %s array!\n", T2N(v3->type), T2N(v1->val.arrayVal->type));
		}

		v1->val.arrayVal->arr[pos].val = v3->val;
		
		if(v3->type == L_OBJECT){
			t->lvm->objectValues.insert(&(v1->val.arrayVal->arr[pos]));
		}else if(v3->type == L_ARRAY || v3->type == L_UNIARRAY){
			t->lvm->arrayValues.insert(&(v1->val.arrayVal->arr[pos]));
		}
	}
}

void LVM_getobjfield(lThread* t){
    LVM_R2();
    LVM_RT1();

    if(v1->type != L_OBJECT){
        LVM_error(t->lvm, LVM_TYPEERR, "Cannot access field in non object variable!");
    }

    lobject* o = (lobject*) v1->val.objectVal;
    lValue* v = LVM_findInstanceFieldByName(t, o, (char*) r->bytes, 0);

    if(v == NULL){
        LVM_error(t->lvm, LVM_NULLPTR, "%s in %s not found!", (char*) r->bytes, (char*) (o->cls->refTable[0]).bytes);
    }

    v2->type = v->type;
    v2->val = v->val;
	
	if(v2->type == L_OBJECT){
		t->lvm->objectValues.insert(v2);
	}else if(v2->type == L_ARRAY || v2->type == L_UNIARRAY){
		t->lvm->arrayValues.insert(v2);
	}
}

void LVM_setobjfield(lThread* t){
    LVM_R2();
    LVM_RT1();

    if(v1->type != L_OBJECT){
        LVM_error(t->lvm, LVM_TYPEERR, "Cannot access field in non object variable!");
    }

    lobject* o = (lobject*) v1->val.objectVal;
    lValue* v = LVM_findInstanceFieldByName(t, o, (char*) r->bytes, 1);

    if(v == NULL){
        LVM_error(t->lvm, LVM_NULLPTR, "%s in %s not found or field is final (and cannot be changed)!", (char*) r->bytes, (char*) (o->cls->refTable[0]).bytes);
    }

    if(v->type != L_GENERIC && v->type != v2->type){
        LVM_error(t->lvm, LVM_TYPEERR, "Field in object has type %s, got %s!", T2N(v->type), T2N(v2->type));
    }

	v->type = v2->type;
    v->val = v2->val;
	
	if(v->type == L_OBJECT){
		t->lvm->objectValues.insert(v);
	}else if(v->type == L_ARRAY || v->type == L_UNIARRAY){
		t->lvm->arrayValues.insert(v);
	}
}

void LVM_x2y(lThread* t){
    LVM_R2();
    u1 type = LVM_GETARGU1();

    if((LVM_ISWHOLE(type) || type == L_CHAR) && (LVM_ISWHOLE(v1->type) || v1->type == L_CHAR)){
        v2->type = type;
        v2->val.s8Val = LVM_toIntVal(v1);
        LVM_convertIntVal(v2);
        return;
    }else if(LVM_ISWHOLE(type) || type == L_CHAR){
        if(v1->type == L_FLOAT){
            v2->type = type;
            v2->val.s8Val = (s8) v1->val.floatVal;
            LVM_convertIntVal(v2);
            return;
        }else if(v1->type == L_DOUBLE){
            v2->type = type;
            v2->val.s8Val = (s8) v1->val.doubleVal;
            LVM_convertIntVal(v2);
            return;
        }
    }else if(type == L_FLOAT){
        if(LVM_ISWHOLE(v1->type) || v1->type == L_CHAR){
            v2->type = type;
            v2->val.floatVal = (float) LVM_toIntVal(v1);
            return;
        }else if(v1->type == L_DOUBLE){
            v2->type = type;
            v2->val.floatVal = (float) v1->val.doubleVal;
            return;
        }
    }else if(type == L_DOUBLE){
        if(LVM_ISWHOLE(v1->type) || v1->type == L_CHAR){
            v2->type = type;
            v2->val.doubleVal = (double) LVM_toIntVal(v1);
            return;
        }else if(v1->type == L_FLOAT){
            v2->type = type;
            v2->val.doubleVal = (double) v1->val.floatVal;
            return;
        }
    }
	char msg[255];
	sprintf(msg, "Cannot cast %s to %s!", T2N(v1->type), T2N(type));
	LVM_throwExceptionNew(t, "std.core.CastException", msg);
}

void LVM_checkcast(lThread* t){
    LVM_R1();
    u2 rti = LVM_GETARGU2();
    LVM_Reference* r = LVM_RT(rti);

    if(v->type != L_OBJECT){
        LVM_error(t->lvm, LVM_TYPEERR, "Cannot checkcast on non object value!");
    }

    lobject* o = (lobject*) v->val.objectVal;
	char* myDesc = (char*) (o->cls->refTable)[0].bytes;
    char* desc = (char*) r->bytes;
	
	u1 isInst = LVM_isInstanceOf(t, o, desc);
	
	if(!isInst){
		char msg[255];
		sprintf(msg, "%s cannot be cast to %s", myDesc, desc);
		LVM_throwExceptionNew(t, "std.core.CastException", msg);
	}
}

#define DIOP(N, n, OP) void LVM_##n(lThread* t){\
	LVM_R3();\
    if(v1->type == v2->type)\
    {\
        if(LVM_ISWHOLE(v1->type))\
        {\
            v3->type = v1->type;\
            s8 i1 = LVM_toIntVal(v1);\
            s8 i2 = LVM_toIntVal(v2);\
            v3->val.s8Val = i1 OP i2;\
            LVM_convertIntVal(v3);\
            return;\
        }\
        else if(v1->type == L_BOOLEAN)\
        {\
            v3->type = L_BOOLEAN;\
            u1 b1 = v1->val.u1Val;\
            u1 b2 = v2->val.u1Val;\
            v3->val.u1Val = b1 OP b2;\
            return;\
        }\
        else if(v1->type == L_CHAR)\
        {\
            v3->type = L_CHAR;\
            s1 b1 = v1->val.u1Val;\
            s1 b2 = v2->val.u1Val;\
            v3->val.u1Val = b1 OP b2;\
            return;\
        }else if(v1->type == L_OBJECT || v2->type == L_OBJECT){\
			lValue ret = LVM_callOperatorMethod(t, "<##OP>", v1, NULL, v2);\
			v3->type = ret.type;\
			v3->val = ret.val;\
		}\
    }\
    LVM_error(t->lvm, LVM_TYPEERR, "##OP cannot be applied to types: %s, %s!", T2N(v1->type), T2N(v2->type));\
}

DIOP(OR, or, |)
DIOP(AND, and, &)
DIOP(XOR, xor, ^)

#undef DIOP

void LVM_not(lThread* t){
    LVM_R2();
    if(v1->type == L_BOOLEAN){
        v2->type = v1->type;
        v2->val.u1Val = v1->val.u1Val == 1 ? 0 : 1;
    }else if(LVM_ISWHOLE(v1->type)){
        v2->val.s8Val = ~(LVM_toIntVal(v1));
        LVM_convertIntVal(v2);
    }else if(v1->type == L_CHAR){
        v2->type = v1->type;
        v2->val.u1Val = ~v1->val.u1Val;
    }else if(v1->type == L_OBJECT){
		lValue ret = LVM_callOperatorMethod(t, "<!>", v1, NULL, NULL);
		v2->type = ret.type;
		v2->val = ret.val;
	}else{
        LVM_error(t->lvm, LVM_TYPEERR, "NOT cannot be applied to type: %s!", T2N(v1->type));
    }
}

#define SHRM(N, n, OP) void LVM_##n(lThread* t){\
    LVM_R3();\
    if(LVM_ISWHOLE(v1->type) || v1->type == L_CHAR){\
        if(LVM_ISWHOLE(v2->type) || v2->type == L_CHAR){\
            v3->type = (v1->type == v2->type ? v1->type : (v2->type != L_CHAR ? (v2->type > v1->type ? v2->type : v1->type) : v1->type));\
            v3->val.s8Val = LVM_toIntVal(v1) OP LVM_toIntVal(v2);\
            LVM_convertIntVal(v3);\
            return;\
        }\
    }else if(v1->type == L_OBJECT || v2->type == L_OBJECT){\
			lValue ret = LVM_callOperatorMethod(t, "<##OP>", v1, NULL, v2);\
			v3->type = ret.type;\
			v3->val = ret.val;\
		}\
    LVM_error(t->lvm, LVM_TYPEERR, "##OP cannot be applied to types: %s, %s", T2N(v1->type), T2N(v2->type));\
}

SHRM(RSHIFT, rshift, >>)
SHRM(LSHIFT, lshift, <<)
SHRM(REM, rem, %)

#undef SHRM

#define ARITH(N, n, OP) void LVM_##n(lThread* t){\
    LVM_R3();\
    if(v1->type == v2->type){\
        if(LVM_ISWHOLE(v1->type)){\
            v3->type = v1->type;\
            v3->val.s8Val = LVM_toIntVal(v1) OP LVM_toIntVal(v2);\
            LVM_convertIntVal(v3);\
            return;\
        }else if(v1->type == L_DOUBLE){\
            v3->type = v1->type;\
            v3->val.doubleVal = v1->val.doubleVal OP v2->val.doubleVal;\
            return;\
        }else if(v1->type == L_FLOAT){\
            v3->type = v1->type;\
            v3->val.floatVal = v1->val.floatVal OP v2->val.floatVal;\
            return;\
        }else if(v1->type == L_CHAR){\
            v3->type = v1->type;\
            v3->val.u1Val = v1->val.u1Val OP v2->val.u1Val;\
            return;\
        }else if(v1->type == L_OBJECT){\
			lValue ret = LVM_callOperatorMethod(t, "<##OP>", v1, NULL, v2);\
			v3->type = ret.type;\
			v3->val = ret.val;\
		}\
    }else if(v1->type == L_OBJECT || v2->type == L_OBJECT){\
			lValue ret = LVM_callOperatorMethod(t, "<##OP>", v1, NULL, v2);\
			v3->type = ret.type;\
			v3->val = ret.val;\
		}\
	LVM_error(t->lvm, LVM_TYPEERR, "##OP cannot be applied to types: %s, %s", T2N(v1->type), T2N(v2->type));\
}

ARITH(ADD, add, +)
ARITH(SUB, sub, -)
ARITH(MUL, mul, *)
ARITH(DIV, div, /)

#undef ARITH

void LVM_neg(lThread* t){
    LVM_R2();
    if(LVM_ISSIGNED(v1->type)){
        v2->type = v1->type;
        v2->val.s8Val = -LVM_toIntVal(v1);
        LVM_convertIntVal(v2);
    }else if(v1->type == L_FLOAT){
        v2->type = v1->type;
        v2->val.floatVal = -v1->val.floatVal;
    }else if(v1->type == L_DOUBLE){
        v2->type = v1->type;
        v2->val.doubleVal = -v1->val.doubleVal;
    }else if(v1->type == L_OBJECT){
		lValue ret = LVM_callOperatorMethod(t, "<-T>", v1, NULL, NULL);
		v2->type = ret.type;
		v2->val = ret.val;
	}else{
        LVM_error(t->lvm, LVM_TYPEERR, "- cannot be applied to type: %s!", T2N(v1->type));
    }
}

void LVM_inc(lThread* t){
    LVM_R1();
    if(LVM_ISWHOLE(v->type)){
        v->val.s8Val = LVM_toIntVal(v) + 1;
        LVM_convertIntVal(v);
    }else if(v->type == L_DOUBLE){
        ++(v->val.doubleVal);
    }else if(v->type == L_FLOAT){
        ++(v->val.floatVal);
    }else if(v->type == L_CHAR){
        ++(v->val.u1Val);
    }else if(v->type == L_OBJECT){
		lValue ret = LVM_callOperatorMethod(t, "<++>", v, NULL, NULL);
		v->type = ret.type;
		v->val = ret.val;
	}else{
        LVM_error(t->lvm, LVM_TYPEERR, "++ cannot be applied to type: %s!", T2N(v->type));
    }
}

void LVM_dec(lThread* t){
    LVM_R1();
    if(LVM_ISWHOLE(v->type)){
        v->val.s8Val = LVM_toIntVal(v) - 1;
        LVM_convertIntVal(v);
    }else if(v->type == L_DOUBLE){
        --(v->val.doubleVal);
    }else if(v->type == L_FLOAT){
        --(v->val.floatVal);
    }else if(v->type == L_CHAR){
        --(v->val.u1Val);
    }else if(v->type == L_OBJECT){
		lValue ret = LVM_callOperatorMethod(t, "<-->", v, NULL, NULL);
		v->type = ret.type;
		v->val = ret.val;
	}else{
        LVM_error(t->lvm, LVM_TYPEERR, "-- cannot be applied to type: %s!", T2N(v->type));
    }
}

void LVM_callnormal(lThread* t){
	LVM_R3();
	
	if(v1->type != L_METHODREF){
		LVM_error(t->lvm, LVM_TYPEERR, "CALLNORMAL needs a method reference to call, got: %s!", T2N(v1->type));
	}
	
	lmethod* m = (lmethod*) v1->val.pointerVal;
	
	if(L_ISBITSET(m->bitmask, L_NATIVE)){
		LVM_error(t->lvm, LVM_TYPEERR, "CALLNORMAL cannot call native method!");
	}
	
	lValue ret = LVM_callMethod(t, m, v2);
	
	if(ret.type != L_VOID){
		v3->type = ret.type;
		v3->val = ret.val;
		
		if(v3->type == L_OBJECT){
			t->lvm->objectValues.insert(v3);
		}else if(v3->type == L_ARRAY || v3->type == L_UNIARRAY){
			t->lvm->arrayValues.insert(v3);
		}
	}
}

void LVM_callnative(lThread* t){
	LVM_R3();
	
	if(v1->type != L_METHODREF){
		LVM_error(t->lvm, LVM_TYPEERR, "CALLNATIVE needs a method reference to call, got: %s!", T2N(v1->type));
	}
	
	lmethod* m = (lmethod*) v1->val.pointerVal;
	
	if(!L_ISBITSET(m->bitmask, L_NATIVE)){
		LVM_error(t->lvm, LVM_TYPEERR, "CALLNATIVE cannot call non native method!");
	}
	
	lValue ret = LVM_callNativeMethod(t, m, v2);
	
	if(ret.type != L_VOID){
		v3->type = ret.type;
		v3->val = ret.val;
		
		if(v3->type == L_OBJECT){
			t->lvm->objectValues.insert(v3);
		}else if(v3->type == L_ARRAY || v3->type == L_UNIARRAY){
			t->lvm->arrayValues.insert(v3);
		}
	}
}

void LVM_callspecial(lThread* t){
	LVM_R2();
	
	if(v1->type != L_METHODREF){
		LVM_error(t->lvm, LVM_TYPEERR, "CALLSPECIAL needs a method reference to call, got: %s!", T2N(v1->type));
	}
	
	lmethod* m = (lmethod*) v1->val.pointerVal;
	
	if(L_ISBITSET(m->bitmask, L_NATIVE)){
		LVM_error(t->lvm, LVM_TYPEERR, "CALLSPECIAL cannot call native method!");
	}
	
	LVM_callMethod(t, m, v2);
}

void LVM_if(lThread* t){
    LVM_R1();
    u2 pos = LVM_GETARGU2();

    if(v->type != L_BOOLEAN){
        LVM_error(t->lvm, LVM_TYPEERR, "IF needs boolean, got: %s!", T2N(v->type));
    }

    if(v->val.u1Val == 1){
        t->pc = pos - 1;
    }
}

void LVM_goto(lThread* t){
    u2 pos = LVM_GETARGU2();
    t->pc = pos - 1;
}

void LVM_comp(lThread* t){
    LVM_R3();
    u1 comp = LVM_GETARGU1();
    u1 res = 2;
    if(comp == 0x00){
        if(v1->type == v2->type && v1->type == L_OBJECT){
			res = LVM_callOperatorMethod(t, "<==>", v1, NULL, v2).val.u1Val == 1 ? 0 : 1;
		}else{
			res = LVM_comp00(v1, v2);
		}
    }else if(comp == 0x11){
		if(v1->type == v2->type && v1->type == L_OBJECT){
			res = LVM_callOperatorMethod(t, "<==>", v1, NULL, v2).val.u1Val;
		}else{
			res = LVM_comp11(v1, v2);
		}
    }else if(comp == 0x10){
        res = LVM_comp10(v1, v2);
    }else if(comp == 0x01){
        res = LVM_comp01(v1, v2);
    }else if(comp == 0x21){
        res = LVM_comp21(v1, v2);
    }else if(comp == 0x12){
        res = LVM_comp12(v1, v2);
    }else{
        LVM_error(t->lvm, LVM_OPERR, "COMP cannot be applied to operation: %x!", comp);
    }

    if(res < 2){
        v3->type = L_BOOLEAN;
        v3->val.u1Val = res;
        return;
    }

    LVM_error(t->lvm, LVM_OPERR, "COMP cannot be applied to types: %s, %s!", T2N(v1->type), T2N(v2->type));
}

void LVM_instanceof(lThread* t){
    LVM_R2();
    u2 rti = LVM_GETARGU2();
    LVM_Reference* r = LVM_RT(rti);

    if(v1->type != L_OBJECT){
        LVM_error(t->lvm, LVM_TYPEERR, "Cannot checkcast on non object value!\n");
    }

    lobject* o = (lobject*) v1->val.objectVal;
    char* desc = (char*) r->bytes;
	
	u1 isInst = LVM_isInstanceOf(t, o, desc);
	v2->type = L_BOOLEAN;
	v2->val.u1Val = isInst;
}

void LVM_arraylen(lThread* t){
    LVM_R2();
    if(v1->type == L_ARRAY || v1->type == L_UNIARRAY){
        v2->type = L_U4;
        v2->val.u4Val = v1->val.arrayVal->length;
        return;
    }
    LVM_error(t->lvm, LVM_TYPEERR, "ARRAYLEN cannot be applied to type: %s!", T2N(v1->type));
}

void LVM_stringlen(lThread* t){
    LVM_R2();
    if(v1->type == L_STRING){
        v2->type = L_U4;
        v2->val.u2Val = v1->val.stringVal->length;
        return;
    }
    LVM_error(t->lvm, LVM_TYPEERR, "STRINGLEN cannot be applied to type: %s!", T2N(v1->type));
}

void LVM_strcat(lThread* t){
    LVM_R3();
    if(v1->type == L_STRING && v2->type == L_STRING){
        u4 len = v1->val.stringVal->length + v2->val.stringVal->length;
		
        lstring* s = (lstring*) LVM_alloc(t->lvm, sizeof(lstring), 1);
		s->length = len;
		s->chars = (u1*) LVM_alloc(t->lvm, sizeof(u1), len + 1);
		
        u4 i;
        for(i = 0; i < s->length - 1; i++){
            if(i < v1->val.stringVal->length){
                s->chars[i] = v1->val.stringVal->chars[i];
            }else{
                s->chars[i] = v2->val.stringVal->chars[i - v1->val.stringVal->length];
            }
        }
		s->chars[i] = '\0';
        v3->type = L_STRING;
        v3->val.stringVal = s;
        return;
    }else if(v1->type == L_STRING && v2->type == L_CHAR){
		u4 len = v1->val.stringVal->length + 1;
		
		lstring* s = (lstring*) LVM_alloc(t->lvm, sizeof(lstring), 1);
		s->length = len;
		s->chars = (u1*) LVM_alloc(t->lvm, sizeof(u1), len + 1);
		
		u4 i;
		for(i = 0; i < s->length - 2; i++){
			s->chars[i] = v1->val.stringVal->chars[i];
		}
		s->chars[i++] = v2->val.u1Val;
		s->chars[i] = '\0';
		v3->type = L_STRING;
        v3->val.stringVal = s;
        return;
	}
    LVM_error(t->lvm, LVM_TYPEERR, "STRCAT needs string and string/char, got: %s, %s!", T2N(v1->type), T2N(v2->type));
}

void LVM_sizeof(lThread* t){
    LVM_R2();
    v2->type = L_U1;
    v2->val.u1Val = T2S(v1->type);
}

void LVM_swap(lThread* t){
    LVM_R2();
    lValue x = {v1->type, v1->val};
    v1->type = v2->type;
    v1->val = v2->val;
    v2->type = x.type;
    v2->val = x.val;
	
	if(v1->type == L_OBJECT){
		t->lvm->objectValues.insert(v1);
	}else if(v1->type == L_ARRAY || v1->type == L_UNIARRAY){
		t->lvm->arrayValues.insert(v1);
	}
	
	if(v2->type == L_OBJECT){
		t->lvm->objectValues.insert(v2);
	}else if(v2->type == L_ARRAY || v2->type == L_UNIARRAY){
		t->lvm->arrayValues.insert(v2);
	}
}

void LVM_throw(lThread* t){
    u1 ri = LVM_GETARGU1();
    lValue* v = LVM_R(ri);
	
	if(v->type != L_OBJECT){
		LVM_error(t->lvm, LVM_TYPEERR, "Cannot throw non-object value: %s!", T2N(v->type));
	}
	
	LVM_throwException(t, v->val.objectVal);
}

void LVM_tostr(lThread* t){
    LVM_R2();
    if(v1->type == L_CHAR){
        char s[2];
        s[0] = (char) v1->val.u1Val;
        s[1] = '\0';
        v2->val.stringVal = LVM_createString(t->lvm, s);
    }else if(LVM_ISWHOLE(v1->type)){
        char s[32];
        sprintf(s, L_PS8, LVM_toIntVal(v1));
        v2->val.stringVal = LVM_createString(t->lvm, s);
    }else if(v1->type == L_FLOAT){
        char s[32];
        sprintf(s, "%f", v1->val.floatVal);
        v2->val.stringVal = LVM_createString(t->lvm, s);
    }else if(v1->type == L_DOUBLE){
        char s[32];
        sprintf(s, "%lf", v1->val.doubleVal);
        v2->val.stringVal = LVM_createString(t->lvm, s);
    }else if(v1->type == L_BOOLEAN){
        if(v1->val.u1Val == 0){
            v2->val.stringVal = LVM_createString(t->lvm, "false");
        }else{
            v2->val.stringVal = LVM_createString(t->lvm, "true");
        }
    }else{
        LVM_error(t->lvm, LVM_TYPEERR, "TOSTR cannot be applied to type: %s!", T2N(v1->type));
    }
    v2->type = L_STRING;
}

void LVM_getstatic(lThread* t){
    LVM_R1();
    LVM_RT1();
    u2 rti2 = LVM_GETARGU2();

    char* desc = (char*) r->bytes;

    lclass* cls = LVM_findClassByDescriptor(t, desc);
    if(cls == NULL){
        LVM_error(t->lvm, LVM_NOCLASS, "%s not loaded!", desc);
    }

    LVM_Reference* r2 = &(cls->refTable[rti2]);
    lValue* vv = LVM_findClassFieldByName(cls, (char*) r2->bytes, 0);

    if(vv == NULL){
        LVM_error(t->lvm, LVM_NULLPTR, "%s in %s not found!", (char*) r2->bytes, (char*) (cls->refTable[0]).bytes);
    }

    v->type = vv->type;
    v->val = vv->val;
	
	if(v->type == L_OBJECT){
		t->lvm->objectValues.insert(v);
	}else if(v->type == L_ARRAY || v->type == L_UNIARRAY){
		t->lvm->arrayValues.insert(v);
	}
}

void LVM_setstatic(lThread* t){
    LVM_R1();
    LVM_RT1();
    u2 rti2 = LVM_GETARGU2();

    char* desc = (char*) r->bytes;

    lclass* cls = LVM_findClassByDescriptor(t, desc);
    if(cls == NULL){
        LVM_error(t->lvm, LVM_NOCLASS, "%s not loaded!", desc);
    }

    LVM_Reference* r2 = &(cls->refTable[rti2]);
    lValue* vv = LVM_findClassFieldByName(cls, (char*) r2->bytes, 0);

    if(vv == NULL){
        LVM_error(t->lvm, LVM_NULLPTR, "%s in %s not found or field is final (and cannot be changed)!", (char*) r2->bytes, (char*) (cls->refTable[0]).bytes);
    }

    if(vv->type != L_GENERIC && vv->type != v->type){
        LVM_error(t->lvm, LVM_TYPEERR, "Field in object has type %s, got %s!", T2N(vv->type), T2N(v->type));
    }

	vv->type = v->type;
    vv->val = v->val;
	
	if(vv->type == L_OBJECT){
		t->lvm->objectValues.insert(vv);
	}else if(vv->type == L_ARRAY || vv->type == L_UNIARRAY){
		t->lvm->arrayValues.insert(vv);
	}
}

void LVM_loadnull(lThread* t){
    LVM_R1();

    v->type = L_NULL;
    v->val.objectVal = t->lvm->NULLOBJ;
}

void LVM_loadm1(lThread* t){
    LVM_R1();
    v->type = L_S1;
    v->val.s1Val = -1;
}

void LVM_load0(lThread* t){
    LVM_R1();
    v->type = L_S1;
    v->val.s1Val = 0;
}

void LVM_load1(lThread* t){
    LVM_R1();
    v->type = L_S1;
    v->val.s1Val = 1;
}

void LVM_load2(lThread* t){
    LVM_R1();
    v->type = L_S1;
    v->val.s1Val = 2;
}

void LVM_load5(lThread* t){
    LVM_R1();
    v->type = L_S1;
    v->val.s1Val = 5;
}

void LVM_loadfalse(lThread* t){
    LVM_R1();
    v->type = L_BOOLEAN;
    v->val.u1Val = 0;
}

void LVM_loadtrue(lThread* t){
    LVM_R1();
    v->type = L_BOOLEAN;
    v->val.u1Val = 1;
}

void LVM_getstrchar(lThread* t){
    LVM_R3();
    if(v1->type != L_STRING){
        LVM_error(t->lvm, LVM_TYPEERR, "To access a char a string is required, got: %s!", T2N(v1->type));
    }
    if(v2->type != L_U4){
        LVM_error(t->lvm, LVM_TYPEERR, "To access a char an unsigned 2 byte index is required, got: %s!", T2N(v2->type));
    }
    u4 idx = v2->val.u4Val;
    if(idx >= v1->val.stringVal->length){
		char msg[255];
		sprintf(msg, "Cannot access char at %ud, max %ud!", idx, v1->val.stringVal->length - 1);
		LVM_throwExceptionNew(t, "std.core.IndexOutOfBoundsException", msg);
        return;
    }

    v3->type = L_CHAR;
    v3->val.u1Val = v1->val.stringVal->chars[idx];
}

void LVM_setstrchar(lThread* t){
    LVM_R3();
    if(v1->type != L_STRING){
        LVM_error(t->lvm, LVM_TYPEERR, "To access a char a string is required, got: %s!", T2N(v1->type));
    }
    if(v2->type != L_U4){
        LVM_error(t->lvm, LVM_TYPEERR, "To access a char an unsigned 4 byte index is required, got: %s!", T2N(v2->type));
    }
    if(v3->type != L_CHAR){
        LVM_error(t->lvm, LVM_TYPEERR, "Only a char can be saved into an string, got: %s!", T2N(v3->type));
    }
    u4 idx = v2->val.u4Val;
    if(idx >= v1->val.stringVal->length){
		char msg[255];
		sprintf(msg, "Cannot access char at %ud, max %ud", idx, v1->val.stringVal->length - 1);
        LVM_throwExceptionNew(t, "std.core.IndexOutOfBoundsException", msg);
		return;
    }

    v1->val.stringVal->chars[idx] = v3->val.u1Val;
}

void LVM_strfromarr(lThread* t){
    LVM_R2();
	
    if(v1->type != L_ARRAY){
        LVM_error(t->lvm, LVM_TYPEERR, "Required array, got: %s!", T2N(v1->type));
    }
	
    if(v1->val.arrayVal->type != L_CHAR){
        LVM_error(t->lvm, LVM_TYPEERR, "Required char array, got: %s array!", T2N(v1->val.arrayVal->type));
    }

    v2->type = L_STRING;
    v2->val.stringVal = LVM_createStringFromArray(t->lvm, v1->val.arrayVal);
}

void LVM_newarraydyn(lThread* t){
    LVM_R3();

    if(!LVM_ISUNSIGNED(v3->type) || v3->type == L_U8){
        LVM_error(t->lvm, LVM_TYPEERR, "Array cannot have size from type: %s!", T2N(v2->type));
    }

    u1 type = v2->type;

    if(LVM_INVTYPE(type)){
        LVM_error(t->lvm, LVM_TYPEERR, "Array cannot have type: %s!", T2N(type));
    }

    u4 size = (u4) LVM_toIntVal(v2);
    larray* arr = (larray*) LVM_alloc(t->lvm, sizeof(larray), 1);
    arr->type = type;
    arr->length = size;
    arr->arr = (lValue*) LVM_alloc(t->lvm, sizeof(lValue), size);

    LVM_trackArray(t->lvm, arr);

    v1->type = L_ARRAY;
    v1->val.arrayVal = arr;
	
	t->lvm->arrayValues.insert(v1);
}

void LVM_getptr(lThread* t){
    LVM_R2();
    v1->type = L_POINTER;
    v1->val.pointerVal = (void*) v2;
}
void LVM_derefptr(lThread* t){
    LVM_R2();

    if(v2->type != L_POINTER){
        LVM_error(t->lvm, LVM_TYPEERR, "To dereference a pointer-type is needed, got: %s!", T2N(v2->type));
    }

    lValue* ptrVal = (lValue*) v2->val.pointerVal;

    v1->val = ptrVal->val;
	v1->type = ptrVal->type;
	
	if(v1->type == L_OBJECT){
		t->lvm->objectValues.insert(v1);
	}else if(v1->type == L_ARRAY || v1->type == L_UNIARRAY){
		t->lvm->arrayValues.insert(v1);
	}
}

void LVM_updateptr(lThread* t){
    LVM_R2();

    if(v1->type != L_POINTER){
        LVM_error(t->lvm, LVM_TYPEERR, "To dereference a pointer-type is needed, got: %s!", T2N(v2->type));
    }

    lValue* ptrVal = (lValue*) v1->val.pointerVal;

    if(ptrVal->type != v2->type){
        LVM_error(t->lvm, LVM_TYPEERR, "Value must have same type as pointer: %s must be %s!", T2N(ptrVal->type), T2N(v2->type));
    }

    ptrVal->val = v2->val;
	ptrVal->type = v2->type;
	
	if(ptrVal->type == L_OBJECT){
		t->lvm->objectValues.insert(ptrVal);
	}else if(ptrVal->type == L_ARRAY || ptrVal->type == L_UNIARRAY){
		t->lvm->arrayValues.insert(ptrVal);
	}
}

void LVM_isprimitive(lThread* t){
    LVM_R2();
    if(v2->type != L_OBJECT){
        v1->type = L_BOOLEAN;
        v1->val.u1Val = 0;
    }else{
        v1->type = L_BOOLEAN;
        v1->val.u1Val = 1;
    }
}

void LVM_getnormal(lThread* t){
    LVM_R1();
    LVM_RT2();
    if(r1->type != L_METHODDESC || r2->type != L_CLASSDESC){
        LVM_error(t->lvm, LVM_TYPEERR, "References cannot have type: %s and %s!", T2N(r1->type), T2N(r2->type));
    }

    char* mDesc = (char*) r1->bytes;
    char* clsDesc = (char*) r2->bytes;

    lclass* cls = LVM_findClassByDescriptor(t, clsDesc);
    if(cls == NULL){
        LVM_error(t->lvm, LVM_NOCLASS, "Class %s not loaded!", clsDesc);
    }

    lmethod* m = LVM_findMethodByDescriptor(cls, mDesc);
    if(m == NULL){
        LVM_error(t->lvm, LVM_NOMETHOD, "Method %s not found in class %s!", mDesc, clsDesc);
    }

    if(L_ISBITSET(m->bitmask, L_NATIVE)){
        char fullDesc[strlen(clsDesc) + strlen(mDesc) + 2];
        sprintf(fullDesc, "%s.%s", clsDesc, mDesc);
        auto it = t->lvm->simpleLibraries.begin();
        while(it != t->lvm->simpleLibraries.end()){
            const char* name = (*it).name;
			
            if(!strcmp(name, fullDesc)){
                m->nativeFunc = (*it).f;
                v->val.pointerVal = m;
				v->type = L_METHODREF;
				return;
            }
			it++;
        }
        LVM_error(t->lvm, LVM_NOMETHOD, "Native method %s not found in class %s!", mDesc, clsDesc);
    }else{
        v->val.pointerVal = m;
    }

    v->type = L_METHODREF;
}

void LVM_getvirtual(lThread* t){
	LVM_R2();
	LVM_RT2();
	
	if(r1->type != L_METHODDESC || r2->type != L_CLASSDESC){
		LVM_error(t->lvm, LVM_TYPEERR, "References cannot have type: %s and %s!", T2N(r1->type), T2N(r2->type));
	}
	
	if(v1->type != L_OBJECT){
		LVM_error(t->lvm, LVM_TYPEERR, "GETVIRTUAL needs an object reference, got: %s!", T2N(v1->type));
	}
	
	lclass* objClass = v1->val.objectVal->cls;
	char* clsDesc = (char*) objClass->refTable[0].bytes;
	char* mDesc = (char*) r1->bytes;
	lmethod* m = LVM_findMethodByDescriptor(objClass, mDesc);
	
	if(m == NULL){
		clsDesc = (char*) r2->bytes;
		objClass = LVM_findClassByDescriptor(t, clsDesc);
		
		if(objClass == NULL){
			LVM_error(t->lvm, LVM_NOCLASS, "Class %s not loaded!", clsDesc);
		}
		
		m = LVM_findMethodByDescriptor(objClass, mDesc);
		
		if(m == NULL){
			LVM_error(t->lvm, LVM_NOMETHOD, "Method %s not found in class %s!", mDesc, clsDesc);
		}
	}
	
	if(L_ISBITSET(m->bitmask, L_NATIVE)){
        char fullDesc[strlen(clsDesc) + strlen(mDesc) + 2];
        sprintf(fullDesc, "%s.%s", clsDesc, mDesc);
        auto it = t->lvm->simpleLibraries.begin();
        while(it != t->lvm->simpleLibraries.end()){
            const char* name = (*it).name;

            if(!strcmp(name, fullDesc)){
                m->nativeFunc = (*it).f;
                v2->val.pointerVal = m;
				v2->type = L_METHODREF;
				return;
            }
			it++;
        }
        LVM_error(t->lvm, LVM_NOMETHOD, "Native method %s not found in class %s!", mDesc, clsDesc);
    }else{
        v2->val.pointerVal = m;
    }

    v2->type = L_METHODREF;
}

void LVM_getexception(lThread* t){
	if(t->curExcep == NULL){
		LVM_error(t->lvm, LVM_NULLPTR, "No exception available!");
	}
	
	LVM_R1();
	v->type = L_OBJECT;
	v->val.objectVal = t->curExcep;
}
