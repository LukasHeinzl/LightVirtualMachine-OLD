#include <Startup.hpp>
#include <Types.hpp>
#include <Runtime.hpp>
#include <Error.hpp>
#include <cstring>

lmethod* LVM_createStaticObjectsAndGetMain(LVM* lvm){
    u2 i;
    lmethod* mainMeth = NULL;
    for(i = 0; i < lvm->loadedClasses.size(); i++){
        u2 j;
        lclass* cls = lvm->loadedClasses[i];

        if(cls->classVarCount > 0){
            lstaticobject* so = (lstaticobject*) LVM_alloc(lvm, sizeof(lstaticobject), 1);
            so->cls = cls;
            so->classVarCount = cls->classVarCount;
            so->classVariables = (lValue*) LVM_alloc(lvm, sizeof(lValue), so->classVarCount);
            u2 k;
            for(k = 0; k < so->classVarCount; k++){
                so->classVariables[k].type = cls->classVars[k].type;
            }
            cls->staticObj = so;
        }

        for(j = 0; j < cls->mtSize; j++){
            lmethod* m = &(cls->metTable[j]);
            LVM_Reference* r = &(cls->refTable[m->descriptor]);
			if(r->type != L_METHODDESC){
                continue;
            }
            LVM_methodDesc2Type((char*) r->bytes, m->params);
            if(m->paramCount > 127){
                LVM_error(lvm, LVM_INDEXERR, "Method cannot have more than 127 parameters, got %u!", m->paramCount);
            }
            if(!strcmp((char*) r->bytes, "V([S)main") && m->bitmask == L_STATIC){
                mainMeth = m;
            }
        }
    }
    return mainMeth;
}

void LVM_classInit(lThread* t){
    u2 i;
    for(i = 0; i < t->lvm->loadedClasses.size(); i++){
        lclass* cls = t->lvm->loadedClasses[i];
        if(cls->staticObj == NULL){
            continue;
        }

        u2 j;
        for(j = 0; j < cls->mtSize; j++){
            lmethod* met = &(cls->metTable[j]);
            LVM_Reference* r = &(cls->refTable[met->descriptor]);
            if(r->type != L_METHODDESC){
                continue;
            }
            if(!strcmp((char*) r->bytes, "V()<cinit>") && met->bitmask == L_STATIC){
                LVM_callMethod(t, met, NULL);
            }
        }
    }
}
