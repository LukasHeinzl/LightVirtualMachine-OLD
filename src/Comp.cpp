#include <Comp.hpp>
#include <cstring>

u1 LVM_comp00(lValue* v1, lValue* v2){
    if(v1->type != v2->type){
        return 2;
    }

    u1 t = v1->type;

    if(t == L_U1){
        return v1->val.u1Val == v2->val.u1Val;
    }else if(t == L_U2){
        return v1->val.u2Val == v2->val.u2Val;
    }else if(t == L_U4){
        return v1->val.u4Val == v2->val.u4Val;
    }else if(t == L_U8){
        return v1->val.u8Val == v2->val.u8Val;
    }else if(t == L_S1){
        return v1->val.s1Val == v2->val.s1Val;
    }else if(t == L_S2){
        return v1->val.s2Val == v2->val.s2Val;
    }else if(t == L_S4){
        return v1->val.s4Val == v2->val.s4Val;
    }else if(t == L_S8){
        return v1->val.s8Val == v2->val.s8Val;
    }else if(t == L_FLOAT){
        return v1->val.floatVal == v2->val.floatVal;
    }else if(t == L_DOUBLE){
        return v1->val.doubleVal == v2->val.doubleVal;
    }else if(t == L_STRING){
        return strcmp((char*) v1->val.stringVal->chars, (char*) v2->val.stringVal->chars) == 0 ? 1 : 0;
    }else if(t == L_ARRAY || t == L_UNIARRAY){
        return v1->val.arrayVal->arr == v2->val.arrayVal->arr;
    }
    return 2;
}

u1 LVM_comp11(lValue* v1, lValue* v2){
    u1 ret = LVM_comp00(v1, v2);
    if(ret == 0){
        return 1;
    }else if(ret == 1){
        return 0;
    }
    return 2;
}

u1 LVM_comp10(lValue* v1, lValue* v2){
    if(v1->type != v2->type){
        return 2;
    }

    u1 t = v1->type;

    if(t == L_U1){
        return v1->val.u1Val < v2->val.u1Val;
    }else if(t == L_U2){
        return v1->val.u2Val < v2->val.u2Val;
    }else if(t == L_U4){
        return v1->val.u4Val < v2->val.u4Val;
    }else if(t == L_U8){
        return v1->val.u8Val < v2->val.u8Val;
    }else if(t == L_S1){
        return v1->val.s1Val < v2->val.s1Val;
    }else if(t == L_S2){
        return v1->val.s2Val < v2->val.s2Val;
    }else if(t == L_S4){
        return v1->val.s4Val < v2->val.s4Val;
    }else if(t == L_S8){
        return v1->val.s8Val < v2->val.s8Val;
    }else if(t == L_FLOAT){
        return v1->val.floatVal < v2->val.floatVal;
    }else if(t == L_DOUBLE){
        return v1->val.doubleVal < v2->val.doubleVal;
    }
    return 2;
}

u1 LVM_comp01(lValue* v1, lValue* v2){
    if(v1->type != v2->type){
        return 2;
    }

    u1 t = v1->type;

    if(t == L_U1){
        return v1->val.u1Val > v2->val.u1Val;
    }else if(t == L_U2){
        return v1->val.u2Val > v2->val.u2Val;
    }else if(t == L_U4){
        return v1->val.u4Val > v2->val.u4Val;
    }else if(t == L_U8){
        return v1->val.u8Val > v2->val.u8Val;
    }else if(t == L_S1){
        return v1->val.s1Val > v2->val.s1Val;
    }else if(t == L_S2){
        return v1->val.s2Val > v2->val.s2Val;
    }else if(t == L_S4){
        return v1->val.s4Val > v2->val.s4Val;
    }else if(t == L_S8){
        return v1->val.s8Val > v2->val.s8Val;
    }else if(t == L_FLOAT){
        return v1->val.floatVal > v2->val.floatVal;
    }else if(t == L_DOUBLE){
        return v1->val.doubleVal > v2->val.doubleVal;
    }
    return 2;
}

u1 LVM_comp21(lValue* v1, lValue* v2){
    u2 ret = LVM_comp01(v1, v2);
    if(ret == 0){
        return 1;
    }else if(ret == 1){
        return 0;
    }
    return 2;
}

u1 LVM_comp12(lValue* v1, lValue* v2){
    u2 ret = LVM_comp10(v1, v2);
    if(ret == 0){
        return 1;
    }else if(ret == 1){
        return 0;
    }
    return 2;
}
