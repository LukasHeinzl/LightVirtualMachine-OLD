#include <ByteReader.hpp>
#include <Error.hpp>
#include <cstdio>
#include <cstring>

LVM_FILE LVM_loadFile(LVM* lvm, char* src){
    FILE* fp = fopen(src, "rb");
    if(fp == NULL){
        LVM_error(lvm, LVM_FILEERR, "%s", src);
        LVM_exitVM(lvm);
    }

    fseek(fp, 0, SEEK_END);
    long len = ftell(fp);
    rewind(fp);

    u1* buffer = (u1*) LVM_alloc(lvm, sizeof(u1), len + 1);
    if(buffer == NULL){
        LVM_error(lvm, LVM_NOMEM, "Cannot allocate buffer-memory.");
        LVM_exitVM(lvm);
    }
    fread(buffer, len, 1, fp);
    fclose(fp);
    LVM_FILE f;
    f.content = buffer;
    f.length = (u4) len;
    f.pos = 0;
    return f;
}

void LVM_freeFile(LVM* lvm, LVM_FILE* f){
    LVM_free(lvm, f->content);
}

u1 LVM_readU1(LVM* lvm, LVM_FILE* f){
    u1 u = (f->content)[f->pos];
    (f->pos)++;
    LVM_ensurePosition(lvm, f);
    return u;
}

u2 LVM_readU2(LVM* lvm, LVM_FILE* f){
    u2 u = (u2) (LVM_readU1(lvm, f)) << 8;
    return u + LVM_readU1(lvm, f);
}

u4 LVM_readU4(LVM* lvm, LVM_FILE* f){
    u4 u = (u4) (LVM_readU2(lvm, f)) << 16;
    return u + LVM_readU2(lvm, f);
}

u8 LVM_readU8(LVM* lvm, LVM_FILE* f){
    u8 u = (u8) (LVM_readU4(lvm, f)) << 32;
    return u + LVM_readU4(lvm, f);
}

void LVM_ensurePosition(LVM* lvm, LVM_FILE* f){
    if(f->pos > f->length){
        LVM_error(lvm, LVM_INDEXERR, "File-read out of bounds!");
        LVM_exitVM(lvm);
    }
}

const char* LVM_getFExt(char* f){
    char* dot = strrchr(f, '.');
    if(!dot || dot == f){
        return f;
    }
    return dot + 1;
}
