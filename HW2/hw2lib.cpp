#include <string>
#include <vector>
#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <dirent.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

using namespace std;

int (*chdir_ptr)(const char *path) = NULL;
int (*chmod_ptr)(const char *pathname, mode_t mode) = NULL;
int (*chown_ptr)(const char *pathname, uid_t owner, gid_t group) = NULL;
int (*creat_ptr)(const char *pathname, mode_t mode) = NULL;
int (*creat64_ptr)(const char *pathname, mode_t mode) = NULL;
FILE *(*fopen_ptr)(const char *pathname, const char *mode) = NULL;
FILE *(*fopen64_ptr)(const char *pathname, const char *mode) = NULL;
int (*link_ptr)(const char *oldpath, const char *newpath) = NULL;
int (*mkdir_ptr)(const char *pathname, mode_t mode) = NULL;
int (*open_ptr)(const char *pathname, int flags, ...) = NULL;
int (*open64_ptr)(const char *pathname, int flags, ...) = NULL;
int (*openat_ptr)(int dirfd, const char *pathname, int flags, ...) = NULL;
int (*openat64_ptr)(int dirfd, const char *pathname, int flags, ...) = NULL;
DIR *(*opendir_ptr)(const char *name) = NULL;
ssize_t (*readlink_ptr)(const char *pathname, char *buf, size_t bufsiz) = NULL;
int (*remove_ptr)(const char *pathname) = NULL;
int (*rename_ptr)(const char *oldpath, const char *newpath) = NULL;
int (*rmdir_ptr)(const char *pathname) = NULL;
int (*stat_ptr)(int __ver, const char *__filename, struct stat *__stat_buf) = NULL;
int (*stat64_ptr)(int __ver, const char *__filename, struct stat64 *__stat_buf) = NULL;
int (*symlink_ptr)(const char *target, const char *linkpath) = NULL;
int (*unlink_ptr)(const char *pathname) = NULL;

bool perr_flag = false;

int check_valid_path(const char *path){
    char tmp_path[1024];
    char* basedir;
    char* token;
    string path_concat;
    vector<string> fullpath;

    strcpy(tmp_path, path);
    if(tmp_path[0] == '/') fullpath.push_back("");
    token = strtok(tmp_path, "/");
    while(token != NULL){
        fullpath.push_back(token);
        token = strtok(NULL, "/");
    }
    if(fullpath.at(0) == "~"){
        char envvar[1024];
        strcpy(envvar, getenv("HOME"));
        token = strtok(envvar, "/");
        for(int i = 0; token != NULL; i++){
            fullpath.insert(fullpath.begin()+i, token);
            token = strtok(NULL, "/");
            if(token == NULL) fullpath.erase(fullpath.begin()+i+1);
        }
    }else if(fullpath.at(0) == "." || (fullpath.at(0) != "" && fullpath.at(0) != "..")){
        char envvar[1024];
        getcwd(envvar, 1024);
        token = strtok(envvar, "/");
        for(int i = 0; token != NULL; i++){
            fullpath.insert(fullpath.begin()+i, token);
            token = strtok(NULL, "/");
            if(token == NULL) fullpath.erase(fullpath.begin()+i+1);
        }
    }else if(fullpath.at(0) == ".."){
        char envvar[1024];
        getcwd(envvar, 1024);
        token = strtok(envvar, "/");
        for(int i = 0; token != NULL; i++){
            fullpath.insert(fullpath.begin()+i, token);
            token = strtok(NULL, "/");
            if(token == NULL){
                fullpath.erase(fullpath.begin()+i+1);
                fullpath.erase(fullpath.begin()+i);
            }
        }
    }
    for(vector<string>::iterator it = fullpath.begin(); it != fullpath.end(); it++){
        vector<string>::iterator del = it;
        if((*it) == "."){
            fullpath.erase(del);
        }else if((*it) == ".."){
            fullpath.erase(del);
            del = it--;
            fullpath.erase(del);
        }
    }
    for(size_t i = 0; i < fullpath.size(); i++){
        if(fullpath.at(i) == ""){
            if(fullpath.size() == 1) path_concat = path_concat + "/";
            else continue;
        }
        else path_concat = path_concat + "/" + fullpath.at(i);
    }

    strcpy(tmp_path, path_concat.c_str());
    basedir = getenv("BASEDIR");
    if(strstr(tmp_path, basedir) == NULL){
        return -1;
    }else{
        return 0;
    }
}

void print_err(int type, const char *funcname, const char *path){
    perr_flag = true;
    FILE* fp = fopen("/dev/tty", "w");

    if(type == 0){
        fprintf(fp,"[sandbox] %s(%s): not allowed\n", funcname, path);
    }else{
        fprintf(fp,"[sandbox] %s: access to %s is not allowed\n", funcname, path);
    }

    perr_flag = false;
    return;
}

int chdir(const char *path){
    if(check_valid_path(path) != 0){
        print_err(1, "chdir", path);
        // fprintf(stderr, "[sandbox] chdir: access to %s not allowed\n", path);
        errno = EACCES;
        return -1;
    }

    void *handle = dlopen("libc.so.6", RTLD_LAZY);
    if(chdir_ptr == NULL) chdir_ptr = (int (*)(const char *))dlsym(handle, "chdir");
    return chdir_ptr(path);
}

int chmod(const char *pathname, mode_t mode){
    if(check_valid_path(pathname) != 0){
        print_err(1, "chmod", pathname);
        // fprintf(stderr, "[sandbox] chmod: access to %s not allowed\n", pathname);
        errno = EACCES;
        return -1;
    }

    void *handle = dlopen("libc.so.6", RTLD_LAZY);
    if(chmod_ptr == NULL) chmod_ptr = (int (*)(const char *, mode_t))dlsym(handle, "chmod");
    return chmod_ptr(pathname, mode);
}

int chown(const char *pathname, uid_t owner, gid_t group){
    if(check_valid_path(pathname) != 0){
        print_err(1, "chown", pathname);
        // fprintf(stderr, "[sandbox] chown: access to %s not allowed\n", pathname);
        errno = EACCES;
        return -1;
    }

    void *handle = dlopen("libc.so.6", RTLD_LAZY);
    if(chown_ptr == NULL) chown_ptr = (int (*)(const char *, uid_t, gid_t))dlsym(handle, "chown");
    return chown_ptr(pathname, owner, group);
}

int creat(const char *pathname, mode_t mode){
    if(check_valid_path(pathname) != 0){
        print_err(1, "creat", pathname);
        // fprintf(stderr, "[sandbox] creat: access to %s not allowed\n", pathname);
        errno = EACCES;
        return -1;
    }

    void *handle = dlopen("libc.so.6", RTLD_LAZY);
    if(creat_ptr == NULL) creat_ptr = (int (*)(const char *, mode_t))dlsym(handle, "creat");
    return creat_ptr(pathname, mode);
}

int creat64(const char *pathname, mode_t mode){
    if(check_valid_path(pathname) != 0){
        print_err(1, "creat64", pathname);
        // fprintf(stderr, "[sandbox] creat64: access to %s not allowed\n", pathname);
        errno = EACCES;
        return -1;
    }

    void *handle = dlopen("libc.so.6", RTLD_LAZY);
    if(creat64_ptr == NULL) creat64_ptr = (int (*)(const char *, mode_t))dlsym(handle, "creat64");
    return creat64_ptr(pathname, mode);
}

FILE *fopen(const char *pathname, const char *mode){
    if(perr_flag == false){
        if(check_valid_path(pathname) != 0){
            print_err(1, "fopen", pathname);
            // fprintf(stderr, "[sandbox] fopen: access to %s not allowed\n", pathname);
            errno = EACCES;
            return NULL;
        }
    }

    void *handle = dlopen("libc.so.6", RTLD_LAZY);
    if(fopen_ptr == NULL) fopen_ptr = (FILE *(*)(const char *, const char *))dlsym(handle, "fopen");
    return fopen_ptr(pathname, mode);
}

FILE *fopen64(const char *pathname, const char *mode){
    if(perr_flag == false){
        if(check_valid_path(pathname) != 0){
            print_err(1, "fopen64", pathname);
            // fprintf(stderr, "[sandbox] fopen64: access to %s not allowed\n", pathname);
            errno = EACCES;
            return NULL;
        }
    }

    void *handle = dlopen("libc.so.6", RTLD_LAZY);
    if(fopen64_ptr == NULL) fopen64_ptr = (FILE *(*)(const char *, const char *))dlsym(handle, "fopen64");
    return fopen64_ptr(pathname, mode);
}

int link(const char *oldpath, const char *newpath){
    bool err = false;
    if(check_valid_path(oldpath) != 0){
        print_err(1, "link", oldpath);
        // fprintf(stderr, "[sandbox] link: access to %s not allowed\n", oldpath);
        err = true;
    }
    if(check_valid_path(newpath) != 0){
        print_err(1, "link", newpath);
        // fprintf(stderr, "[sandbox] link: access to %s not allowed\n", newpath);
        err = true;
    }
    if(err){
        errno = EACCES;
        return -1;
    }

    void *handle = dlopen("libc.so.6", RTLD_LAZY);
    if(link_ptr == NULL) link_ptr = (int (*)(const char *, const char *))dlsym(handle, "link");
    return link_ptr(oldpath, newpath);
}

int mkdir(const char *pathname, mode_t mode){
    if(check_valid_path(pathname) != 0){
        print_err(1, "mkdir", pathname);
        // fprintf(stderr, "[sandbox] mkdir: access to %s not allowed\n", pathname);
        errno = EACCES;
        return -1;
    }

    void *handle = dlopen("libc.so.6", RTLD_LAZY);
    if(mkdir_ptr == NULL) mkdir_ptr = (int (*)(const char *, mode_t))dlsym(handle, "mkdir");
    return mkdir_ptr(pathname, mode);
}

int open(const char *pathname, int flags, ...){
    if(check_valid_path(pathname) != 0){
        print_err(1, "open", pathname);
        // fprintf(stderr, "[sandbox] open: access to %s not allowed\n", pathname);
        errno = EACCES;
        return -1;
    }

    void *handle = dlopen("libc.so.6", RTLD_LAZY);
    va_list valist;
    va_start(valist, flags);
    if(open_ptr == NULL) open_ptr = (int (*)(const char *, int, ...))dlsym(handle, "open");
    return open_ptr(pathname, flags, va_arg(valist, mode_t));
}

int open64(const char *pathname, int flags, ...){
    if(check_valid_path(pathname) != 0){
        print_err(1, "open64", pathname);
        // fprintf(stderr, "[sandbox] open64: access to %s not allowed\n", pathname);
        errno = EACCES;
        return -1;
    }

    void *handle = dlopen("libc.so.6", RTLD_LAZY);
    va_list valist;
    va_start(valist, flags);
    if(open64_ptr == NULL) open64_ptr = (int (*)(const char *, int, ...))dlsym(handle, "open64");
    return open64_ptr(pathname, flags, va_arg(valist, mode_t));
}

int openat(int dirfd, const char *pathname, int flags,...){
    if(check_valid_path(pathname) != 0){
        print_err(1, "openat", pathname);
        // fprintf(stderr, "[sandbox] openat: access to %s not allowed\n", pathname);
        errno = EACCES;
        return -1;
    }

    void *handle = dlopen("libc.so.6", RTLD_LAZY);
    va_list valist;
    va_start(valist, flags);
    if(openat_ptr == NULL) openat_ptr = (int (*)(int, const char *, int, ...))dlsym(handle, "openat");
    return openat_ptr(dirfd, pathname, flags, va_arg(valist, mode_t));
}

int openat64(int dirfd, const char *pathname, int flags, ...){
    if(check_valid_path(pathname) != 0){
        print_err(1, "openat64", pathname);
        // fprintf(stderr, "[sandbox] openat64: access to %s not allowed\n", pathname);
        errno = EACCES;
        return -1;
    }

    void *handle = dlopen("libc.so.6", RTLD_LAZY);
    va_list valist;
    va_start(valist, flags);
    if(openat64_ptr == NULL) openat64_ptr = (int (*)(int, const char *, int, ...))dlsym(handle, "openat64");
    return openat64_ptr(dirfd, pathname, flags, va_arg(valist, mode_t));
}

DIR *opendir(const char *name){
    if(check_valid_path(name) != 0){
        print_err(1, "opendir", name);
        // fprintf(stderr, "[sandbox] opendir: access to %s not allowed\n", name);
        errno = EACCES;
        return NULL;
    }

    void *handle = dlopen("libc.so.6", RTLD_LAZY);
    if(opendir_ptr == NULL) opendir_ptr = (DIR *(*)(const char *))dlsym(handle, "opendir");
    return opendir_ptr(name);
}

ssize_t readlink(const char *pathname, char *buf, size_t bufsiz){
    if(check_valid_path(pathname) != 0){
        print_err(1, "readlink", pathname);
        // fprintf(stderr, "[sandbox] readlink: access to %s not allowed\n", pathname);
        errno = EACCES;
        return -1;
    }

    void *handle = dlopen("libc.so.6", RTLD_LAZY);
    if(readlink_ptr == NULL) readlink_ptr = (ssize_t (*)(const char *, char *, size_t))dlsym(handle, "readlink");
    return readlink_ptr(pathname, buf, bufsiz);
}

int remove(const char *pathname){
    if(check_valid_path(pathname) != 0){
        print_err(1, "remove", pathname);
        // fprintf(stderr, "[sandbox] remove: access to %s not allowed\n", pathname);
        errno = EACCES;
        return -1;
    }

    void *handle = dlopen("libc.so.6", RTLD_LAZY);
    if(remove_ptr == NULL) remove_ptr = (int (*)(const char *))dlsym(handle, "remove");
    return remove_ptr(pathname);
}

int rename(const char *oldpath, const char *newpath){
    bool err = false;
    if(check_valid_path(oldpath) != 0){
        print_err(1, "rename", oldpath);
        // fprintf(stderr, "[sandbox] rename: access to %s not allowed\n", oldpath);
        err = true;
    }
    if(check_valid_path(newpath) != 0){
        print_err(1, "rename", newpath);
        // fprintf(stderr, "[sandbox] rename: access to %s not allowed\n", newpath);
        err = true;
    }
    if(err){
        errno = EACCES;
        return -1;
    }

    void *handle = dlopen("libc.so.6", RTLD_LAZY);
    if(rename_ptr == NULL) rename_ptr = (int (*)(const char *, const char *))dlsym(handle, "rename");
    return rename_ptr(oldpath, newpath);
}

int rmdir(const char *pathname){
    if(check_valid_path(pathname) != 0){
        print_err(1, "rmdir", pathname);
        // fprintf(stderr, "[sandbox] rmdir: access to %s not allowed\n", pathname);
        errno = EACCES;
        return -1;
    }

    void *handle = dlopen("libc.so.6", RTLD_LAZY);
    if(rmdir_ptr == NULL) rmdir_ptr = (int (*)(const char *))dlsym(handle, "rmdir");
    return rmdir_ptr(pathname);
}

int __xstat(int __ver, const char *__filename, struct stat *__stat_buf){
    if(check_valid_path(__filename) != 0){
        print_err(1, "__xstat", __filename);
        // fprintf(stderr, "[sandbox] __xstat: access to %s not allowed\n", __filename);
        errno = EACCES;
        return -1;
    }

    void *handle = dlopen("libc.so.6", RTLD_LAZY);
    if(stat_ptr == NULL) stat_ptr = (int (*)(int, const char *, struct stat *))dlsym(handle, "__xstat");
    return stat_ptr(__ver, __filename, __stat_buf);
}

int __xstat64(int __ver, const char *__filename, struct stat64 *__stat_buf){
    if(check_valid_path(__filename) != 0){
        print_err(1, "__xstat64", __filename);
        // fprintf(stderr, "[sandbox] __xstat64: access to %s not allowed\n", __filename);
        errno = EACCES;
        return -1;
    }

    void *handle = dlopen("libc.so.6", RTLD_LAZY);
    if(stat64_ptr == NULL) stat64_ptr = (int (*)(int, const char *, struct stat64 *))dlsym(handle, "__xstat64");
    return stat64_ptr(__ver, __filename, __stat_buf);
}

int symlink(const char *target, const char *linkpath){
    bool err = false;
    if(check_valid_path(target) != 0){
        print_err(1, "symlink", target);
        // fprintf(stderr, "[sandbox] symlink: access to %s not allowed\n", target);
        err = true;
    }
    if(check_valid_path(linkpath) != 0){
        print_err(1, "symlink", linkpath);
        // fprintf(stderr, "[sandbox] symlink: access to %s not allowed\n", linkpath);
        err = true;
    }
    if(err){
        errno = EACCES;
        return -1;
    }

    void *handle = dlopen("libc.so.6", RTLD_LAZY);
    if(symlink_ptr == NULL) symlink_ptr = (int (*)(const char *, const char *))dlsym(handle, "symlink");
    return symlink_ptr(target, linkpath);
}

int unlink(const char *pathname){
    if(check_valid_path(pathname) != 0){
        print_err(1, "unlink", pathname);
        // fprintf(stderr, "[sandbox] unlink: access to %s not allowed\n", pathname);
        errno = EACCES;
        return -1;
    }

    void *handle = dlopen("libc.so.6", RTLD_LAZY);
    if(unlink_ptr == NULL) unlink_ptr = (int (*)(const char *))dlsym(handle, "unlink");
    return unlink_ptr(pathname);
}

/* The following function is forbiddened */
int execl(const char *path, const char *arg, ...){
    print_err(0, "execl", path);
    // fprintf(stderr, "[sandbox] execl(%s): not allowed\n", path);
    errno = EACCES;
    return -1;
}

int execlp(const char *file, const char *arg, ...){
    print_err(0, "execlp", file);
    // fprintf(stderr, "[sandbox] execlp(%s): not allowed\n", file);
    errno = EACCES;
    return -1;
}

int execle(const char *path, const char *arg, ...){
    print_err(0, "execle", path);
    // fprintf(stderr, "[sandbox] execle(%s): not allowed\n", path);
    errno = EACCES;
    return -1;
}

int execv(const char *path, char *const argv[]){
    print_err(0, "execv", path);
    // fprintf(stderr, "[sandbox] execv(%s): not allowed\n", path);
    errno = EACCES;
    return -1;
}

int execvp(const char *file, char *const argv[]){
    print_err(0, "execvp", file);
    // fprintf(stderr, "[sandbox] execvp(%s): not allowed\n", file);
    errno = EACCES;
    return -1;
}

int execve(const char *path, char *const argv[], char *const envp[]){
    print_err(0, "execve", path);
    // fprintf(stderr, "[sandbox] execve(%s): not allowed\n", path);
    errno = EACCES;
    return -1;
}

int system(const char *command){
    print_err(0, "system", command);
    // fprintf(stderr, "[sandbox] system(%s): not allowed\n", command);
    errno = EACCES;
    return -1;
}
