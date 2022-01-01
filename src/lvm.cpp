#include <lvm.hpp>
#include <Error.hpp>
#include <lni.hpp>
#include <Types.hpp>
#include <csignal>
#include <iostream>
#include <algorithm>

static LVM* globalVM;
static u1 gcFreed = 0;
static u2 freeCount = 0;
static u2 arrayFreeCount = 0;
static u2 objectFreeCount = 0;

LVM* LVM_createVM(u4* options){
    LVM* lvm = new LVM;
    if(lvm == NULL){
        return NULL;
    }
	
	lvm->options = options;

	lvm->loadedClasses.reserve(lvm->options[LVM_OCLSRSRV]);
    lvm->arrays.reserve(lvm->options[LVM_OARRAYRSRV]);
    lvm->mem.reserve(lvm->options[LVM_OMEMRSRV]);
    lvm->objects.reserve(lvm->options[LVM_OOBJRSRV]);
    lvm->threads.reserve(lvm->options[LVM_OTHRDRSRV]);
    lvm->NULLOBJ = (lobject*) NULL;
    lvm->voidValue.type = L_VOID;

    globalVM = lvm;

    signal(SIGSEGV, LVM_sighandler);
    signal(SIGFPE, LVM_sighandler);
    signal(SIGILL, LVM_sighandler);
    signal(SIGABRT, LVM_sighandler);
    signal(SIGTERM, LVM_sighandler);
    signal(SIGINT, LVM_sighandler);
    return lvm;
}

void LVM_exitVM(LVM* lvm){
    LVM_freeAll(lvm);
	
    auto it = lvm->dynamicLibraries.begin();

    while(it != lvm->dynamicLibraries.end()){
        LVM_FREELIB(*it);
        it++;
    }
	
	lvm->loadedClasses.clear();
	lvm->arrays.clear();
    lvm->mem.clear();
    lvm->objects.clear();
    lvm->objectValues.clear();
    lvm->arrayValues.clear();
    lvm->threads.clear();
	lvm->dynamicLibraries.clear();

    delete lvm;
    exit(1);
}

void LVM_newThread(LVM* lvm, lThread* t){
    t->lvm = lvm;
    lvm->threads.push_back(t);
    t->currentMethod = NULL;
    t->pc = 0;
    t->registers = NULL;
    t->state = L_TREADY;
	t->curExcep = NULL;
}

void LVM_killThread(lThread* t){
    if(t->state == L_TDEAD){
        return;
    }
    LVM_free(t->lvm, t->registers);
	t->registers = NULL;
    t->currentMethod = NULL;
    t->pc = 0;
    t->state = L_TDEAD;

    u2 i;
    for(i = 0; i < t->lvm->threads.size(); i++){
        if(t->lvm->threads[i] == t){
            t->lvm->threads.erase(t->lvm->threads.begin() + i);
			return;
        }
    }
}

void* LVM_alloc(LVM* lvm, size_t bytes, size_t amount){
    restartAllocFrame:
    if(bytes == 0 || amount == 0){
        return NULL;
    }

    if(lvm->mem.size() >= lvm->options[LVM_OMAXMEMSLOT]){
        if(!gcFreed){
            LVM_gc(lvm);
            goto restartAllocFrame;
        }
        LVM_error(lvm, LVM_NOMEM, "No more memory slots available!");
    }

    void* p = operator new (amount * bytes, std::nothrow);
    if(p == NULL){
        if(!gcFreed){
            LVM_gc(lvm);
            goto restartAllocFrame;
        }
        LVM_error(lvm, LVM_NOMEM, "Could not allocate %u bytes!", bytes * amount);
    }

    lvm->mem.push_back(p);

    if(lvm->mem.capacity() - lvm->mem.size() < lvm->options[LVM_OMINSLOTFREE]){
        LVM_gc(lvm);
    }
    gcFreed = 0;
    return p;
}

void LVM_free(LVM* lvm, void* p){
    if(p == NULL){
        return;
    }

    u8 i;
    for(i = 0; i < lvm->mem.size(); i++){
        if(lvm->mem[i] == p){
            lvm->mem[i] = NULL;
            operator delete(p);
			break;
        }
    }
	
	freeCount++;
	
	if(freeCount >= lvm->options[LVM_OFREECOUNT]){
		lvm->mem.erase(std::remove_if(lvm->mem.begin(), lvm->mem.end(), [](void* ptr){
			return ptr == NULL;
		}), lvm->mem.end());
		freeCount = 0;
	}
}

void LVM_freeAll(LVM* lvm){
    if(lvm->mem.size() == 0){
        return;
    }
    
	auto it = lvm->mem.begin();
    while(it != lvm->mem.end()){
        operator delete(*it);
		it++;
    }
    lvm->mem.clear();
}

void LVM_gc(LVM* lvm){
    if(gcFreed == 1){
        return;
    }

    auto i1 = lvm->arrays.begin();
    while(i1 != lvm->arrays.end()){
        larray* arr = *i1;
		arr->reachable = 0;
		
		auto setIt = lvm->arrayValues.begin();
		while(setIt != lvm->arrayValues.end()){
			if(((*setIt)->type == L_ARRAY || (*setIt)->type == L_UNIARRAY) && (*setIt)->val.arrayVal == arr){
				arr->reachable = 1;
				break;
			}else if(((*setIt)->type != L_ARRAY && (*setIt)->type != L_UNIARRAY) || (*setIt)->val.objectVal == lvm->NULLOBJ){
				setIt = lvm->arrayValues.erase(setIt);
			}
		}
		
        if(!(arr->reachable)){
			LVM_free(lvm, arr->arr);
            LVM_free(lvm, arr);
            *i1 = NULL;
        }else{
            ++i1;
        }
		freeCount = 0;
		arrayFreeCount = 0;
    }

    auto i2 = lvm->objects.begin();
    while(i2 != lvm->objects.end()){
        lobject* obj = *i2;
		obj->reachable = 0;
		
		auto setIt = lvm->objectValues.begin();
		while(setIt != lvm->objectValues.end()){
			if((*setIt)->type == L_OBJECT && (*setIt)->val.objectVal == obj){
				obj->reachable = 1;
				break;
			}else if((*setIt)->type != L_OBJECT || (*setIt)->val.objectVal == lvm->NULLOBJ){
				setIt = lvm->objectValues.erase(setIt);
			}
		}
		
        if(!(obj->reachable)){
			lThread t;
			LVM_finalizeObject(&t, obj);
            LVM_free(lvm, obj->instanceVariables);
            LVM_free(lvm, obj);
            *i2 = NULL;
        }else{
            ++i2;
        }
		freeCount = 0;
		objectFreeCount = 0;
    }
	
	lvm->mem.erase(std::remove_if(lvm->mem.begin(), lvm->mem.end(), [](void* ptr){
		return ptr == NULL;
	}), lvm->mem.end());
	
	lvm->arrays.erase(std::remove_if(lvm->arrays.begin(), lvm->arrays.end(), [](larray* ptr){
		return ptr == NULL;
	}), lvm->arrays.end());
	
	lvm->objects.erase(std::remove_if(lvm->objects.begin(), lvm->objects.end(), [](lobject* ptr){
		return ptr == NULL;
	}), lvm->objects.end());
	
    gcFreed = 1;
}

void LVM_trackObject(LVM* lvm, lobject* obj){
    lvm->objects.push_back(obj);
}

void LVM_untrackObject(LVM* lvm, lobject* obj){
    u8 i;
    for(i = 0; i < lvm->objects.size(); i++){
        if(lvm->objects[i] == obj){
            lvm->objects[i] = NULL;;
            return;
        }
    }
	
	objectFreeCount++;
	
	if(objectFreeCount >= lvm->options[LVM_OFREEOBJCNT]){
		lvm->objects.erase(std::remove_if(lvm->objects.begin(), lvm->objects.end(), [](lobject* ptr){
			return ptr == NULL;
		}), lvm->objects.end());
		objectFreeCount = 0;
	}
}

void LVM_trackArray(LVM* lvm, larray* arr){
    lvm->arrays.push_back(arr);
}

void LVM_untrackArray(LVM* lvm, larray* arr){
    u8 i;
    for(i = 0; i < lvm->arrays.size(); i++){
        if(lvm->arrays[i] == arr){
            lvm->arrays[i] = NULL;
            return;
        }
    }
	
	arrayFreeCount++;
	
	if(arrayFreeCount >= lvm->options[LVM_OFREEARRCNT]){
		lvm->arrays.erase(std::remove_if(lvm->arrays.begin(), lvm->arrays.end(), [](larray* ptr){
			return ptr == NULL;
		}), lvm->arrays.end());
		arrayFreeCount = 0;
	}
}

u1 LVM_loadSimpleLibrary(LNI* lni){
    lstring* nameStr = lni->getParam(lni, 0)->val.stringVal;
    char* name = lni->toCString(lni, nameStr);

    void* lib = LVM_LOADLIB(name);
    if(lib == NULL){
		LVM_throwExceptionNew(lni->thread, "std.core.NullPointerException", "Could not load dynamic library!");
        return 0;
    }
    lni->thread->lvm->dynamicLibraries.push_back(lib);
    LVM_regSimLib rsl = (LVM_regSimLib) LVM_GETFUNC(lib, "LNI_registerSimpleLibrary");
	
	if(rsl == NULL){
		LVM_throwExceptionNew(lni->thread, "std.core.NullPointerException", "Register function in dynamic library not found!");
		return 0;
	}

    LVM_SimpleLibrary* sl = rsl();

    while(sl->name != NULL){
        lni->thread->lvm->simpleLibraries.push_back(*sl);
        sl++;
    }
    return 0;
}

void LVM_sighandler(int sig){
    if(sig == SIGINT){
        std::cout << "\n\nVM received SIGINT (interrupt)! Terminating..." << std::endl;
        LVM_exitVM(globalVM);
    }
    const char* signame = (sig == SIGSEGV ? "SIGSEGV" : (sig == SIGFPE ? "SIGFPE" : (sig == SIGILL ? "SIGILL" : (sig == SIGABRT ? "SIGABRT" : (sig == SIGTERM ? "SIGTERM" : "UNDEFINED")))));
    LVM_error(globalVM, LVM_FATAL, "Caught signal %d (%s). Terminating VM...", sig, signame);
}

void LVM_printStackTrace(LVM* lvm){
	std::cout << std::endl;
	while(!(lvm->callStack.empty())){
		LVM_CallInfo ci = lvm->callStack.top();
		std::cout << "\tat: " << ci.fullPath << "." << ci.funcName << "(" << ci.fileName << ":";
		
		if(ci.valid == 1){
			std::cout << ci.lineNumber << ")" << std::endl;
		}else if(ci.valid == 2){
			std::cout << "native)" << std::endl;
		}else{
			std::cout << "?)" << std::endl;
		}
		
		lvm->callStack.pop();
	}
}
