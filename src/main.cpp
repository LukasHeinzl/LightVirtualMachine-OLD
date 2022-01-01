#include <Types.hpp>
#include <ByteReader.hpp>
#include <ArgParser.hpp>
#include <ClassLoader.hpp>
#include <Runtime.hpp>
#include <Startup.hpp>
#include <Error.hpp>
#include <iostream>
#include <Bytecodes.hpp>

larray generateStringArray(LVM*, lstring**, u4);
void convertStrings(LVM*, char**, u4, lstring**);
void LVM_handleUncaughtException(lThread*);

int main(int argc, char** argv)
{
	u4 options[11];
	
	//set default options
	options[LVM_OFREECOUNT] = 100;
	options[LVM_OFREEARRCNT] = 100;
    options[LVM_OFREEOBJCNT] = 100;
    options[LVM_OCLSRSRV] = 64;
    options[LVM_OARRAYRSRV] = 32;
    options[LVM_OMEMRSRV] = 1024;
    options[LVM_OOBJRSRV] = 512;
    options[LVM_OTHRDRSRV] = 8;
    options[LVM_OMAXMEMSLOT] = 8192;
    options[LVM_OMINSLOTFREE] = 100;
	options[LVM_OUPK] = 0;
	
	char* fName = NULL;
	int i = LVM_parseArgs(argc, argv, &fName, options);
	if(i == -1){
		return -1;
	}
	
	LVM* lvm = LVM_createVM(options);
	
    u4 progArgsCount = (u4) argc - i;
    lstring* progArgs = (lstring*) LVM_alloc(lvm, sizeof(lstring), progArgsCount);
    convertStrings(lvm, argv + i, progArgsCount, &progArgs);
    larray progArgsArr = generateStringArray(lvm, &progArgs, progArgsCount);
	lValue mainArg;
	mainArg.type = L_ARRAY;
	mainArg.val.arrayVal = &progArgsArr;

    u1 success = LVM_loadClasses(lvm, fName, options[LVM_OUPK], &(lvm->loadedClasses));
	if(!success){
		std::cout << "One or more classes could not be loaded, terminating..." << std::endl;
		LVM_exitVM(lvm);
		return -1;
	}

    ///load std-lib and register natives
    LVM_SimpleLibrary baseLoadLibrary = {"Test.V(S)loadSimpleLibrary", &LVM_loadSimpleLibrary};
    lvm->simpleLibraries.push_back(baseLoadLibrary);

    lmethod* mainMethod = LVM_createStaticObjectsAndGetMain(lvm);

    if(mainMethod == NULL){
        LVM_error(lvm, LVM_NOMAIN, "Terminating VM...");
        LVM_exitVM(lvm);
        return -1;
    }

    lThread mainThread;
    LVM_newThread(lvm, &mainThread);

    LVM_classInit(&mainThread);

	LVM_callMethod(&mainThread, mainMethod, &mainArg);
	
	if(mainThread.state == L_TTHROW){
		LVM_handleUncaughtException(&mainThread);
	}
	
    LVM_killThread(&mainThread);
    LVM_exitVM(lvm);
    return 0;
}

void convertStrings(LVM* lvm, char** arr, u4 len, lstring** strs){
    u8 i;
    for(i = 0; i < len; i++){
        strs[i] = LVM_createString(lvm, arr[i]);
    }
}

larray generateStringArray(LVM* lvm, lstring** strs, u4 count){
    larray arr;
    arr.length = count;
    arr.type = L_STRING;
    arr.arr = (lValue*) LVM_alloc(lvm, sizeof(lValue), count);
    u8 i;
    for(i = 0; i < count; i++){
        arr.arr[i].type = L_STRING;
        arr.arr[i].val.stringVal = strs[i];
    }
    return arr;
}

void LVM_handleUncaughtException(lThread* t){
	std::cout << "Uncaught exception in thread \"main\":" << std::endl;
	lValue* trace = LVM_findInstanceFieldByName(t, t->curExcep, "S:trace:std.core.Exception", 0);
	if(trace == NULL || trace->type != L_STRING){
		std::cout << "Could not load stack-trace!" << std::endl;
		return;
	}
	
	std::cout << trace->val.stringVal->chars << std::endl;
}
