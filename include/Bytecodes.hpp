#ifndef BYTECODES_H_INCLUDED
#define BYTECODES_H_INCLUDED

#include <lvm.hpp>

#define LVM_R(i) &(t->registers[(i)])
#define LVM_RT(i) &(((lclass*) (t->currentMethod->cls))->refTable[(i)])
#define LVM_CURRBCPOS(o) t->currentMethod->bytecodes[t->pc + o]
#define LVM_GETARGU1() LVM_CURRBCPOS(1); (t->pc)++;
#define LVM_GETARGU2() (((u2) LVM_CURRBCPOS(1)) << 8) + LVM_CURRBCPOS(2); (t->pc) += 2;
#define LVM_CLEARR() u1 i; for(i = 0; i < t->currentMethod->registerCount; i++){t->registers[0].type = L_NULL;}

#define LVM_SETV3(v1, t1, v2, t2, v3, t3, op) v3->val.t3##Val = v1->val.t1##Val op v2->val.t2##Val;

#define LVM_R1() u1 ri = LVM_GETARGU1(); lValue* v = LVM_R(ri)
#define LVM_R2() u1 ri1 = LVM_GETARGU1(); u1 ri2 = LVM_GETARGU1(); lValue* v1 = LVM_R(ri1); lValue* v2 = LVM_R(ri2)
#define LVM_R3() u1 ri1 = LVM_GETARGU1(); u1 ri2 = LVM_GETARGU1(); u1 ri3 = LVM_GETARGU1(); lValue* v1 = LVM_R(ri1); lValue* v2 = LVM_R(ri2); lValue* v3 = LVM_R(ri3)
#define LVM_R4() u1 ri1 = LVM_GETARGU1(); u1 ri2 = LVM_GETARGU1(); u1 ri3 = LVM_GETARGU1(); u1 ri4 = LVM_GETARGU1(); lValue* v1 = LVM_R(ri1); lValue* v2 = LVM_R(ri2); lValue* v3 = LVM_R(ri3); lValue* v4 = LVM_R(ri4)

#define LVM_RT1() u2 rti = LVM_GETARGU2(); LVM_Reference* r = LVM_RT(rti)
#define LVM_RT2() u2 rti1 = LVM_GETARGU2(); u2 rti2 = LVM_GETARGU2(); LVM_Reference* r1 = LVM_RT(rti1); LVM_Reference* r2 = LVM_RT(rti2)

typedef void (*LVM_BCFUNC)(lThread*);

void LVM_loadconstant(lThread*);
void LVM_move(lThread*);
void LVM_new(lThread*);
void LVM_newarray(lThread*);
void LVM_delete(lThread*);
void LVM_getarrayelem(lThread*);
void LVM_setarrayelem(lThread*);
void LVM_getobjfield(lThread*);
void LVM_setobjfield(lThread*);
void LVM_x2y(lThread*);
void LVM_checkcast(lThread*);
void LVM_or(lThread*);
void LVM_and(lThread*);
void LVM_not(lThread*);
void LVM_xor(lThread*);
void LVM_lshift(lThread*);
void LVM_rshift(lThread*);
void LVM_add(lThread*);
void LVM_sub(lThread*);
void LVM_mul(lThread*);
void LVM_div(lThread*);
void LVM_rem(lThread*);
void LVM_neg(lThread*);
void LVM_inc(lThread*);
void LVM_dec(lThread*);
void LVM_callnormal(lThread*);
void LVM_callnative(lThread*);
void LVM_callspecial(lThread*);
void LVM_if(lThread*);
void LVM_goto(lThread*);
void LVM_comp(lThread*);
void LVM_instanceof(lThread*);
void LVM_arraylen(lThread*);
void LVM_stringlen(lThread*);
void LVM_strcat(lThread*);
void LVM_sizeof(lThread*);
void LVM_swap(lThread*);
void LVM_throw(lThread*);
void LVM_tostr(lThread*);
void LVM_getstatic(lThread*);
void LVM_setstatic(lThread*);
void LVM_loadnull(lThread*);
void LVM_loadm1(lThread*);
void LVM_load0(lThread*);
void LVM_load1(lThread*);
void LVM_load2(lThread*);
void LVM_load5(lThread*);
void LVM_loadfalse(lThread*);
void LVM_loadtrue(lThread*);
void LVM_getstrchar(lThread*);
void LVM_setstrchar(lThread*);
void LVM_strfromarr(lThread*);
void LVM_newarraydyn(lThread*);
void LVM_getptr(lThread*);
void LVM_derefptr(lThread*);
void LVM_updateptr(lThread*);
void LVM_isprimitive(lThread*);
void LVM_getnormal(lThread*);
void LVM_getvirtual(lThread*);
void LVM_getexception(lThread*);

static const LVM_BCFUNC BCFUNCS[] = {NULL, &LVM_loadconstant, &LVM_move, NULL, NULL, &LVM_new, &LVM_newarray, &LVM_delete,
                                     &LVM_getarrayelem, &LVM_setarrayelem, &LVM_getobjfield, &LVM_setobjfield, &LVM_x2y,
                                     &LVM_checkcast, &LVM_or, &LVM_and, &LVM_not, &LVM_xor, &LVM_lshift, &LVM_rshift, &LVM_add,
                                     &LVM_sub, &LVM_mul, &LVM_div, &LVM_rem, &LVM_neg, &LVM_inc, &LVM_dec, &LVM_callnormal,
									 &LVM_callnative, &LVM_callspecial, &LVM_if, &LVM_goto,
									 &LVM_comp, &LVM_instanceof, &LVM_arraylen, &LVM_stringlen, &LVM_strcat, &LVM_sizeof,
									 &LVM_swap, &LVM_throw, &LVM_tostr, &LVM_getstatic, &LVM_setstatic,
									 &LVM_loadnull, &LVM_loadm1, &LVM_load0, &LVM_load1, &LVM_load2, &LVM_load5, &LVM_loadfalse,
									 &LVM_loadtrue, &LVM_getstrchar, &LVM_setstrchar, &LVM_strfromarr, &LVM_newarraydyn,
									 &LVM_getptr, &LVM_derefptr, &LVM_updateptr, &LVM_isprimitive, &LVM_getnormal,
									 &LVM_getvirtual, &LVM_getexception};

enum BYTECODES{NOP = 0, LOADCONSTANT, MOVE, RETURNVAL, RETURN, NEW, NEWARRAY, DELETE, GETARRAYELEM, SETARRAYELEM,
                            GETOBJFIELD, SETOBJFIELD, X2Y, CHECKCAST, OR, AND, NOT, XOR, LSHIFT, RSHIFT, ADD, SUB, MUL, DIV,
                            REM, NEG, INC, DEC, CALLNORMAL, CALLNATIVE, CALLSPECIAL, IF, GOTO, COMP, INSTANCEOF, ARRAYLEN,
							STRINGLEN, STRCAT, SIZEOF, SWAP, THROW, TOSTR, GETSTATIC, SETSTATIC, LOADNULL, LOADM1,
							LOAD0, LOAD1, LOAD2, LOAD5, LOADFALSE, LOADTRUE, GETSTRCHAR, SETSTRCHAR, STRFROMARR, NEWARRAYDYN,
							GETPTR, DEREFPTR, UPDATEPTR, ISPRIMITIVE, GETNORMAL, GETVIRTUAL, GETEXCEPTION};

#endif // BYTECODES_H_INCLUDED
