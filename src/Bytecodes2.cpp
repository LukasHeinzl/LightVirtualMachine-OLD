void LVM_move(lThread* t){
	LVM_R2();
	*v1 = *v2;
}

void LVM_swap(lThread* t){
	LVM_R2();
	u8 x = *v1;
	*v1 = *v2;
	*v2 = x;
}

void LVM_store(lThread* t){
	LVM_R1();
	LVM_RT1();
	**r = *v;
}

void LVM_storeoffset(lThread* t){
	LVM_R3();
	u8* mem = (u8*) (*v1 + *v2);
	*mem = *v3;
}

void LVM_load(lThread* t){
	LVM_R1();
	LVM_RT1();
	*v = **r;
}

void LVM_loadconstant(lThread* t){
	LVM_R1();
	LVM_RT1();
	*v = (u8) *r;
}

void LVM_loadmethod(lThread* t){
	LVM_R1();
	LVM_RT2();
	//TODO-------------------------------------------------------------------
}

void LVM_loadvirtual(lThread* t){
	LVM_R2();
	LVM_RT2();
	//TODO-------------------------------------------------------------------
}

void LVM_loadexception(lThread* t){
	LVM_R1();
	*v = *(t->er);
}

void LVM_loadoffset(lThread* t){
	LVM_R3();
	u8* mem = (u8*) (*v2 + *v3);
	*v1 = *mem;
}

void LVM_loadm1(lThread* t){
	LVM_R1();
	*v = (u8) -1;
}

void LVM_load0(lThread* t){
	LVM_R1();
	*v = 0;
}

void LVM_load1(lThread* t){
	LVM_R1();
	*v = 1;
}

void LVM_load2(lThread* t){
	LVM_R1();
	*v = 2;
}

void LVM_load5(lThread* t){
	LVM_R1();
	*v = 5;
}

void LVM_add(lThread* t){
	LVM_R3();
	*v1 = *v2 + *v3;
}

void LVM_addf(lThread* t){
	LVM_R3();
	union{
		u8 i;
		double d;
	} u;
	
	u.i = *v2;
	double d2 = u.d;
	u.i = *v3;
	double d3 = u.d;
	u.d = d2 + d3;
	*v1 = u.i;
}

void LVM_sub(lThread* t){
	LVM_R3();
	*v1 = *v2 - *v3;
}

void LVM_subf(lThread* t){
	LVM_R3();
	union{
		u8 i;
		double d;
	} u;
	
	u.i = *v2;
	double d2 = u.d;
	u.i = *v3;
	double d3 = u.d;
	u.d = d2 - d3;
	*v1 = u.i;
}

void LVM_mul(lThread* t){
	LVM_R3();
	*v1 = *v2 * *v3;
}

void LVM_mulf(lThread* t){
	LVM_R3();
	union{
		u8 i;
		double d;
	} u;
	
	u.i = *v2;
	double d2 = u.d;
	u.i = *v3;
	double d3 = u.d;
	u.d = d2 * d3;
	*v1 = u.i;
}

void LVM_div(lThread* t){
	LVM_R3();
	*v1 = *v2 / *v3;
}

void LVM_divf(lThread* t){
	LVM_R3();
	union{
		u8 i;
		double d;
	} u;
	
	u.i = *v2;
	double d2 = u.d;
	u.i = *v3;
	double d3 = u.d;
	u.d = d2 / d3;
	*v1 = u.i;
}

void LVM_rem(lThread* t){
	LVM_R3();
	*v1 = *v2 % *v3;
}

void LVM_neg(lThread* t){
	LVM_R2();
	*v1 = -(*v2);
}

void LVM_inc(lThread* t){
	LVM_R1();
	(*v1)++;
}

void LVM_incf(lThread* t){
	LVM_R1();
	union{
		u8 i;
		double d;
	} u;
	u.i = *v1;
	(u.d)++;
	*v1 = u.i;
}

void LVM_dec(lThread* t){
	LVM_R1();
	(*v1)--;
}

void LVM_decf(lThread* t){
	LVM_R1();
	union{
		u8 i;
		double d;
	} u;
	u.i = *v1;
	(u.d)--;
	*v1 = u.i;
}

void LVM_lshift(lThread* t){
	LVM_R3();
	*v1 = *v2 << *v3;
}

void LVM_rshift(lThread* t){
	LVM_R3();
	*v1 = *v2 >> *v3;
}

void LVM_or(lThread* t){
	LVM_R3();
	*v1 = *v2 | *v3;
}

void LVM_xor(lThread* t){
	LVM_R3();
	*v1 = *v2 ^ *v3;
}

void LVM_and(lThread* t){
	LVM_R3();
	*v1 = *v2 & *v3;
}

void LVM_not(lThread* t){
	LVM_R2();
	*v1 = ~(*v2);
}

// return - instanceof

void LVM_throw(lThread* t){
	LVM_R1();
	*(t->er) = *v;
	t->state = L_EXCEPTION;
}

void LVM_checkcast(lThread* t){
	//TODO-------------------------------------------------------------------
}

void LVM_i2f(lThread* t){
	LVM_R2();
	union{
		u8 i;
		double d;
	} u;
	
	u.d = (double) *v1;
	*v2 = u.i;
}

void LVM_f2i(lThread* t){
	LVM_R2();
	union{
		u8 i;
		double d;
	} u;
	
	u.i = *v1;
	*v2 = (u8) u.d;
}

void LVM_less(lThread* t){
	LVM_R3();
	*v1 = *v2 < *v2 ? 1 : 0;
}

void LVM_lessf(lThread* t){
	LVM_R3();
	union{
		u8 i;
		double d;
	} u;
	
	u.i = *v2;
	double d2 = u.d;
	u.i = *v3;
	double d3 = u.d;
	*v1 = d2 < d3 ? 1 : 0;
}

void LVM_lessequal(lThread* t){
	LVM_R3();
	*v1 = *v2 <= *v3 ? 1 : 0;
}

void LVM_lessequalf(lThread* t){
	LVM_R3();
	union{
		u8 i;
		double d;
	} u;
	
	u.i = *v2;
	double d2 = u.d;
	u.i = *v3;
	double d3 = u.d;
	*v1 = d2 <= d3 ? 1 : 0;
}

void LVM_equal(lThread* t){
	LVM_R3();
	*v1 = *v2 == *v3;
}

void LVM_if(lThread* t){
	LVM_R1();
	u2 pos = LVM_GETARGU2();
	if(*v1 == 1){
		t->pc = pos - 1;
	}
}

void LVM_goto(lThread* t){
	u2 pos = LVM_GETARGU2();
	t->pc = pos - 1;
}