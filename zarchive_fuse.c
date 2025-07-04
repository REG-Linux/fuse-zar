
#define FUSE_USE_VERSION 35
#include <fuse3/fuse.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "zarchive_wrapper.h"

static za_handle g_za = NULL;

static int za_error(int cond) { return cond ? 0 : -ENOENT; }

static int z_getattr(const char* path, struct stat* st, struct fuse_file_info* fi){
    (void)fi; memset(st,0,sizeof(*st));
    if (za_is_dir(g_za, path)) {
        st->st_mode = S_IFDIR | 0555;
        st->st_nlink = 2;
    } else if (za_is_file(g_za, path)) {
        st->st_mode = S_IFREG | 0444;
        st->st_nlink = 1;
        st->st_size = za_size(g_za, path);
        st->st_blksize = 512;
        st->st_blocks = (st->st_size + 511) / 512;
    } else return -ENOENT;
    return 0;
}

static int z_readdir(const char* path, void* buf, fuse_fill_dir_t filler,
                     off_t off, struct fuse_file_info* fi,
                     enum fuse_readdir_flags flags){
    (void)off; (void)fi; (void)flags;
    filler(buf, ".", NULL, 0, 0);
    filler(buf, "..", NULL, 0, 0);

    void list_cb(const char* name, int is_dir, void* u){
        (void)is_dir; fuse_fill_dir_t f = *(fuse_fill_dir_t*)u;
        f(buf, name, NULL, 0, 0);
    }
    za_list(g_za, path, list_cb, &filler);
    return 0;
}

static int z_open(const char* path, struct fuse_file_info* fi){
    return za_error(za_is_file(g_za, path));
}

static int z_read(const char* path, char* buf, size_t size, off_t off,
                  struct fuse_file_info* fi){
    uint64_t got = za_read(g_za, path, off, size, buf);
    return got < 0 ? -EIO : (int)got;
}

static const struct fuse_operations z_oper = {
    .getattr = z_getattr,
    .readdir = z_readdir,
    .open    = z_open,
    .read    = z_read,
};

int main(int argc, char* argv[]){
    if (argc < 3){ fprintf(stderr,"usage: %s archive.zar MOUNTPOINT\n",argv[0]); return 1; }
    g_za = za_open(argv[1]);
    if(!g_za){ fprintf(stderr,"cannot open %s\n", argv[1]); return 1; }
    argv[1] = argv[2]; argv[2] = NULL; argc--;
    int ret = fuse_main(argc, argv, &z_oper, NULL);
    za_close(g_za);
    return ret;
}
