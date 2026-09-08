
#include <zarchive/zarchivereader.h>
#include <cstring>
extern "C" {
#include "zarchive_wrapper.h"
}

struct za_ctx { ZArchiveReader* reader; };

za_handle za_open(const char* path) {
    auto* ctx = new za_ctx;
    ctx->reader = ZArchiveReader::OpenFromFile(path);
    return ctx->reader ? ctx : nullptr;
}

void za_close(za_handle h)            { delete static_cast<za_ctx*>(h)->reader; delete static_cast<za_ctx*>(h); }
uint8_t za_is_dir (za_handle h, const char* p){ return static_cast<za_ctx*>(h)->reader->IsDirectory(
                                        static_cast<za_ctx*>(h)->reader->LookUp(p,false,true)); }
uint8_t za_is_file(za_handle h, const char* p){ return static_cast<za_ctx*>(h)->reader->IsFile(
                                        static_cast<za_ctx*>(h)->reader->LookUp(p,true,false)); }
uint64_t za_size(za_handle h,const char* p)   { return static_cast<za_ctx*>(h)->reader->GetFileSize(
                                        static_cast<za_ctx*>(h)->reader->LookUp(p,true,false)); }
int64_t za_read(za_handle h,const char* p,uint64_t off,uint64_t len,void* buf){
    auto* r = static_cast<za_ctx*>(h)->reader;
    ZArchiveNodeHandle node = r->LookUp(p,true,false);
    if(!r->IsFile(node))
        return -1; /* IsFile bounds-checks, so this covers an invalid handle */
    /* Resolve the ambiguity here, where the size is known: past the end is EOF,
       and nothing read from within the file is a decompression failure. */
    if(off >= r->GetFileSize(node))
        return 0;
    uint64_t got = r->ReadFromFile(node,off,len,buf);
    return got == 0 ? -1 : (int64_t)got;
}

struct dir_cb_ctx{ za_iter_cb cb; void* u; const char* base; };
static void push_entry(const char* name,bool isFile,bool isDir,void* opaque){
    auto* c=static_cast<dir_cb_ctx*>(opaque);
    if(strcmp(name,"." )&&strcmp(name,".."))
        c->cb(name,isDir,c->u);
}

void za_list(za_handle h,const char* path,za_iter_cb cb,void* u){
    dir_cb_ctx ctx{cb,u,path};
    auto* r = static_cast<za_ctx*>(h)->reader;
    auto dh = r->LookUp(path,false,true);
    uint32_t cnt = r->GetDirEntryCount(dh);
    for(uint32_t i=0;i<cnt;i++){
        ZArchiveReader::DirEntry de;
        if(r->GetDirEntry(dh,i,de))
            push_entry(std::string(de.name).c_str(),de.isFile,de.isDirectory,&ctx);
    }
}
