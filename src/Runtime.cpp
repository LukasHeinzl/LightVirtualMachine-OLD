#include <Runtime.hpp>
#include <Error.hpp>
#include <Bytecodes.hpp>
#include <Types.hpp>
#include <lni.hpp>
#include <iostream>

lValue LVM_execute(lThread* t){
    u2* i = &(t->pc);

    for(; t->state != L_TDEAD && *i < t->currentMethod->bcSize; (*i)++){
		if(t->state == L_TTHROW){
			if(LVM_handleException(t) == 0){
				lValue ret;
				ret.type = L_OBJECT;
				ret.val.objectVal = t->curExcep;
				return ret;
			}
		}
		
		LVM_CallInfo ci = LVM_getCallInfo(t->currentMethod, *i);
		t->lvm->callStack.push(ci);
		
        u1* bcs = t->currentMethod->bytecodes;
        u1 code = bcs[*i];
		
        if(BCFUNCS[code] != NULL){
            BCFUNCS[code](t);
			t->lvm->callStack.pop();
            continue;
        }
		
        switch(code){
            case RETURNVAL:{
                u1 ri = LVM_GETARGU1();
                lValue v = *(LVM_R(ri));
                LVM_CLEARR();
				t->curExcep = NULL;
                t->lvm->callStack.pop();
				return v;
			}
            case RETURN:{
                LVM_CLEARR();
				t->curExcep = NULL;
                t->lvm->callStack.pop();
				return t->lvm->voidValue;
			}
        }
    }
	
    return t->lvm->voidValue;
}

u1 LVM_handleException(lThread* t){
	lmethod* m = t->currentMethod;
	if(m->etSize == 0){
		return 0;
	}
	
	lclass* c = m->cls;
	
	u1 i;
	for(i = 0; i < m->etSize; i++){
		LVM_ExceptionEntry* e = &(m->exceptionTable)[i];
		
		if(t->pc < e->startOffset || t->pc > e->endOffset){
			continue;
		}
		
		LVM_Reference* r = &(c->refTable)[e->exceptionRef];
		if(LVM_isInstanceOf(t, t->curExcep, (char*) r->bytes) == 0){
			continue;
		}
		
		t->pc = e->exceptionOffset;
		t->state = L_TACTIVE;
		return 1;
	}
	return 0;
}
