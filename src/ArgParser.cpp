#include <ArgParser.hpp>
#include <cstring>
#include <cstdio>
#include <iostream>

int LVM_parseArgs(int argc, char** argv, char** fName, u4* options){
    if(argc == 1){
        std::cout << "ERR: Please run with option -h OR --help to show help." << std::endl;
        return -1;
    }
    int i, count = 1, isOps = 0;
    for(i = 1; i < argc; i++, count++){
		if(i == 1 && argv[i][0] == '-'){
			isOps = 1;
		}else if(isOps == 1 && argv[i][0] != '-'){
			isOps = 2;
		}
		
		if(isOps == 1 && argv[i][0] == '-'){
			u1 result = LVM_handleArg(argv[i], argv[i + 1], options);
			if(result == 0){
				return -1;
			}else if(result == 2){
				i++;
			}
		}else if(isOps != 1){
            if(*fName != NULL){
                count--;
            }else{
                *fName = argv[i];
            }
        }
    }
    return count;
}

void LVM_trimString(char* s, u1 pref, u1 suff){
    strncpy(s, s + pref, suff);
    s[suff] = '\0';
}

u1 LVM_handleArg(char* arg, char* next, u4* options){
    if(!strcmp(arg, "-h") || !strcmp(arg, "--help")){
        LVM_showHelp();
        return 0;
    }else if(!strcmp(arg, "-fc") || !strcmp(arg, "--freeCount")){
		s8 result = LVM_parseNumber(next);
		if(result == -1){
			return 0;
		}
		options[LVM_OFREECOUNT] = (u4) result;
		return 2;
	}else if(!strcmp(arg, "-fa") || !strcmp(arg, "--freeArrayCount")){
		s8 result = LVM_parseNumber(next);
		if(result == -1){
			return 0;
		}
		options[LVM_OFREEARRCNT] = (u4) result;
		return 2;
	}else if(!strcmp(arg, "-fo") || !strcmp(arg, "--freeObjectCount")){
		s8 result = LVM_parseNumber(next);
		if(result == -1){
			return 0;
		}
		options[LVM_OFREEOBJCNT] = (u4) result;
		return 2;
	}else if(!strcmp(arg, "-cr") || !strcmp(arg, "--classReserve")){
		s8 result = LVM_parseNumber(next);
		if(result == -1){
			return 0;
		}
		options[LVM_OCLSRSRV] = (u4) result;
		return 2;
	}else if(!strcmp(arg, "-ar") || !strcmp(arg, "--arrayReserve")){
		s8 result = LVM_parseNumber(next);
		if(result == -1){
			return 0;
		}
		options[LVM_OARRAYRSRV] = (u4) result;
		return 2;
	}else if(!strcmp(arg, "-mr") || !strcmp(arg, "--memoryReserve")){
		s8 result = LVM_parseNumber(next);
		if(result == -1){
			return 0;
		}
		options[LVM_OMEMRSRV] = (u4) result;
		return 2;
	}else if(!strcmp(arg, "-or") || !strcmp(arg, "--objectReserve")){
		s8 result = LVM_parseNumber(next);
		if(result == -1){
			return 0;
		}
		options[LVM_OOBJRSRV] = (u4) result;
		return 2;
	}else if(!strcmp(arg, "-tr") || !strcmp(arg, "--threadReserve")){
		s8 result = LVM_parseNumber(next);
		if(result == -1){
			return 0;
		}
		options[LVM_OTHRDRSRV] = (u4) result;
		return 2;
	}else if(!strcmp(arg, "-mm") || !strcmp(arg, "--maxMem")){
		s8 result = LVM_parseNumber(next);
		if(result == -1){
			return 0;
		}
		options[LVM_OMAXMEMSLOT] = (u4) result;
		return 2;
	}else if(!strcmp(arg, "-mf") || !strcmp(arg, "--memFree")){
		s8 result = LVM_parseNumber(next);
		if(result == -1){
			return 0;
		}
		options[LVM_OMINSLOTFREE] = (u4) result;
		return 2;
	}else if(!strcmp(arg, "-u") || !strcmp(arg, "--upk")){
		options[LVM_OUPK] = 1;
		return 1;
	}else{
        std::cout << "Unknown option: " << arg << std::endl;
        return 0;
    }
}

s8 LVM_parseNumber(char* s){
	u4 num;
	char c;
	int ok = sscanf(s, "%ud%c", &num, &c);
	if(ok != 1){
		std::cout << s << " cannot be parsed as a number!" << std::endl;
		return -1;
	}
	return (s8) num;
}

void LVM_showHelp(){
    std::cout << "LVM [" LVM_MAJOR_VERSION "." LVM_MINOR_VERSION "] | Options:" << std::endl;
    std::cout << "Usage: lvm [options] <fName> <args>" << std::endl;;
    std::cout << "\t-h --help\t\t\tShows this page." << std::endl;
	std::cout << "\t-fc --freeCount\t\t\tSets the amount of free calls until memory is compacted (higher can be more efficient | hcbme) default: 100" << std::endl;
	std::cout << "\t-fa --freeArrayCount\t\tSets the amount of array free calls until memory is compacted (hcbme) default: 100" << std::endl;
	std::cout << "\t-fo --freeObjectCount\t\tSets the amount of object free calls until memory is compacted (hcbme) default: 100" << std::endl;
	std::cout << "\t-cr --classReserve\t\tSets the amount of class entries to reserve at startup (hcbme) default: 64" << std::endl;
	std::cout << "\t-ar --arrayReserve\t\tSets the amount of array entries to reserve at startup (hcbme) default: 32" << std::endl;
	std::cout << "\t-mr --memoryReserve\t\tSets the amount of memory slots to reserve at startup (hcbme) default: 1024" << std::endl;
	std::cout << "\t-or --objectReserve\t\tSets the amount of object entires to reserve at startup (hcbme) default: 512" << std::endl;
	std::cout << "\t-tr --threadReserve\t\tSets the amount of thread slots to reserve at startup (hcbme) default: 8" << std::endl;
	std::cout << "\t-mm --maxMem\t\t\tSets the maximum amount of memory slots (this is the VM's memory limit!) default: 8192" << std::endl;
	std::cout << "\t-mf --memFree\t\t\tSets the minimum amount of memory slots to be available (lower can be more efficient) default: 100" << std::endl;
	std::cout << "\t-u --upk\t\t\tTreats the input file as an upk archive" << std::endl;
}
