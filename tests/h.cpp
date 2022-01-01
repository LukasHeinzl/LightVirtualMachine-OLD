#include <lni.hpp>
#include <cstdio>

u1 println(LNI* lni){
	lstring* s = lni->getParam(lni, 0)->val.stringVal;
	char* str = lni->toCString(lni, s);
	printf("%s\n", str);
	return 0;
}

LVM_SimpleLibrary lib[] = {
	{"Test.V(S)println", &println},
	{NULL, NULL}
};

LVM_EXPORT LVM_SimpleLibrary* LNI_registerSimpleLibrary(void){
	return lib;
}