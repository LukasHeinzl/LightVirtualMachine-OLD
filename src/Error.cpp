#include <Error.hpp>
#include <cstdarg>
#include <cstdio>

static const char* const LVM_ERR_MSGS[] = {
    "FatalError: ",
    "NullPointerError: ",
    "OutOfMemoryError: ",
    "CastError: ",
    "IndexOutOfBoundsError: ",
    "FileNotFoundError: ",
    "MainMethodNotFoundError: ",
    "MethodNotFoundError: ",
    "ClassNotFoundError: ",
    "TypeError: ",
    "InvalidOperationError: ",
    "InvalidPointerError: ",
	"ParameterCountMissmatchError: "
};

void LVM_error(LVM* lvm, LVM_ERROR_TYPE t, ...){
    va_list args;
    va_start(args, t);

    printf("\n%s", LVM_ERR_MSGS[t]);
	fflush(stdout);

    char* fmt = va_arg(args, char*);

    vfprintf(stderr, fmt, args);
    va_end(args);

	LVM_printStackTrace(lvm);

    LVM_exitVM(lvm);
}
