#include <string>
#include <vector>
#include <dlfcn.h>
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

void init() __attribute__((constructor));
void end() __attribute__((destructor));

void* handle;
// typedef int (*path_t)(const char *path);
// typedef int (*pathmod_t)(const char *pathname, mode_t mode);
// typedef int (*pathog_t)(const char *pathname, uid_t owner, gid_t group);
// typedef FILE *(*pathmodc_t)(const char *pathname, const char *mode);
// typedef int (*onpath_t)(const char *oldpath, const char *newpath);
// typedef int (*pathflag_t)(const char *pathname, int flags, ...);
// typedef int (*fdpathflag_t)(int dirfd, const char *pathname, int flags, ...);
// typedef DIR *(*dirname_t)(const char *name);
// typedef int (*pathstat_t)(const char *pathname, struct stat *statbuf);
// path_t chdir_ptr;
// pathmod_t chmod_ptr;
// pathog_t chown_ptr;
// pathmod_t creat_ptr;
// pathmod_t creat64_ptr;
// pathmodc_t fopen_ptr;
// pathmodc_t fopen64_ptr;
// onpath_t link_ptr;
// pathmod_t mkdir_ptr;
// pathflag_t open_ptr;
// pathflag_t open64_ptr;
// fdpathflag_t openat_ptr;
// fdpathflag_t openat64_ptr;
// dirname_t opendir_ptr;
// pathstat_t stat_ptr;
// pathstat_t stat64_ptr;
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
int (*stat_ptr)(const char *pathname, struct stat *statbuf) = NULL;
int (*stat64_ptr)(const char *pathname, struct stat *statbuf) = NULL;

void init(){
    handle = dlopen("libc.so.6", RTLD_LAZY);
    if(handle == NULL){
        fprintf(stderr, "[sandbox] libc.so.6 load error\n");
        exit(EXIT_FAILURE);
    }
}

// void end(){
//     dlclose(handle);
// }

int check_valid_path(const char *path){
    char tmp_path[1024];
    char* basedir;
    char* token;
    string path_concat;
    vector<string> fullpath;
    // printf("%s\n", path);

    strcpy(tmp_path, path);
    if(tmp_path[0] == '/') fullpath.push_back("");
    token = strtok(tmp_path, "/");
    while(token != NULL){
        fullpath.push_back(token);
        token = strtok(NULL, "/");
    }
    if(fullpath.at(0) == "~"){
        token = strtok(getenv("HOME"), "/");
        for(int i = 0; token != NULL; i++){
            fullpath.insert(fullpath.begin()+i, token);
            token = strtok(NULL, "/");
            if(token == NULL) fullpath.erase(fullpath.begin()+i+1);
        }
    }else if(fullpath.at(0) == "." || (fullpath.at(0) != "" && fullpath.at(0) != "..")){
        token = strtok(getenv("PWD"), "/");
        for(int i = 0; token != NULL; i++){
            fullpath.insert(fullpath.begin()+i, token);
            token = strtok(NULL, "/");
            if(token == NULL) fullpath.erase(fullpath.begin()+i+1);
        }
    }else if(fullpath.at(0) == ".."){
        token = strtok(getenv("PWD"), "/");
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
            printf("im\n");
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

    // printf("%s , %s\n", tmp_path, basedir);
    if(strstr(tmp_path, basedir) == NULL){
        return 1;
    }else{
        return 0;
    }
}

int chdir(const char *path){
    if(check_valid_path(path) != 0){
        fprintf(stderr, "[sandbox] chdir: access to %s not allowed\n", path);
        return 1;
    }

    if(chdir_ptr == NULL) chdir_ptr = (int (*)(const char *))dlsym(handle, "chdir");
    return chdir_ptr(path);
}

int chmod(const char *pathname, mode_t mode){
    if(check_valid_path(pathname) != 0){
        fprintf(stderr, "[sandbox] chmod: access to %s not allowed\n", pathname);
        return 1;
    }

    if(chmod_ptr == NULL) chmod_ptr = (int (*)(const char *, mode_t))dlsym(handle, "chmod");
    return chmod_ptr(pathname, mode);
}

int chown(const char *pathname, uid_t owner, gid_t group){
    if(check_valid_path(pathname) != 0){
        fprintf(stderr, "[sandbox] chown: access to %s not allowed\n", pathname);
        return 1;
    }

    if(chown_ptr == NULL) chown_ptr = (int (*)(const char *, uid_t, gid_t))dlsym(handle, "chown");
    return chown_ptr(pathname, owner, group);
}

int creat(const char *pathname, mode_t mode){
    if(check_valid_path(pathname) != 0){
        fprintf(stderr, "[sandbox] creat: access to %s not allowed\n", pathname);
        return 1;
    }

    if(creat_ptr == NULL) creat_ptr = (int (*)(const char *, mode_t))dlsym(handle, "creat");
    return creat_ptr(pathname, mode);
}

int creat64(const char *pathname, mode_t mode){
    if(check_valid_path(pathname) != 0){
        fprintf(stderr, "[sandbox] creat64: access to %s not allowed\n", pathname);
        return 1;
    }

    if(creat64_ptr == NULL) creat64_ptr = (int (*)(const char *, mode_t))dlsym(handle, "creat64");
    return creat64_ptr(pathname, mode);
}

FILE *fopen(const char *pathname, const char *mode){
    if(check_valid_path(pathname) != 0){
        fprintf(stderr, "[sandbox] fopen: access to %s not allowed\n", pathname);
        return NULL;
    }

    if(fopen_ptr == NULL) fopen_ptr = (FILE *(*)(const char *, const char *))dlsym(handle, "fopen");
    return fopen_ptr(pathname, mode);
}

FILE *fopen64(const char *pathname, const char *mode){
    if(check_valid_path(pathname) != 0){
        fprintf(stderr, "[sandbox] fopen64: access to %s not allowed\n", pathname);
        return NULL;
    }

    if(fopen64_ptr == NULL) fopen64_ptr = (FILE *(*)(const char *, const char *))dlsym(handle, "fopen64");
    return fopen64_ptr(pathname, mode);
}

int link(const char *oldpath, const char *newpath){
    bool err = false;
    if(check_valid_path(oldpath) != 0){
        fprintf(stderr, "[sandbox] link: access to %s not allowed\n", oldpath);
        err = true;
    }
    if(check_valid_path(newpath) != 0){
        fprintf(stderr, "[sandbox] link: access to %s not allowed\n", newpath);
        err = true;
    }
    if(err) return 1;

    if(link_ptr == NULL) link_ptr = (int (*)(const char *, const char *))dlsym(handle, "link");
    return link_ptr(oldpath, newpath);
}

int mkdir(const char *pathname, mode_t mode){
    if(check_valid_path(pathname) != 0){
        fprintf(stderr, "[sandbox] mkdir: access to %s not allowed\n", pathname);
        return 1;
    }

    if(mkdir_ptr == NULL) mkdir_ptr = (int (*)(const char *, mode_t))dlsym(handle, "mkdir");
    return mkdir_ptr(pathname, mode);
}

int open(const char *pathname, int flags, ...){
    if(check_valid_path(pathname) != 0){
        fprintf(stderr, "[sandbox] open: access to %s not allowed\n", pathname);
        return 1;
    }

    va_list valist;
    va_start(valist, flags);
    if(open_ptr == NULL) open_ptr = (int (*)(const char *, int, ...))dlsym(handle, "open");
    return open_ptr(pathname, flags, va_arg(valist, mode_t));
}

int open64(const char *pathname, int flags, ...){
    if(check_valid_path(pathname) != 0){
        fprintf(stderr, "[sandbox] open64: access to %s not allowed\n", pathname);
        return 1;
    }

    va_list valist;
    va_start(valist, flags);
    if(open64_ptr == NULL) open64_ptr = (int (*)(const char *, int, ...))dlsym(handle, "open64");
    return open64_ptr(pathname, flags, va_arg(valist, mode_t));
}

int openat(int dirfd, const char *pathname, int flags,...){
    if(check_valid_path(pathname) != 0){
        fprintf(stderr, "[sandbox] openat: access to %s not allowed\n", pathname);
        return 1;
    }

    va_list valist;
    va_start(valist, flags);
    if(openat_ptr == NULL) openat_ptr = (int (*)(int, const char *, int, ...))dlsym(handle, "openat");
    return openat_ptr(dirfd, pathname, flags, va_arg(valist, mode_t));
}

int openat64(int dirfd, const char *pathname, int flags, ...){
    if(check_valid_path(pathname) != 0){
        fprintf(stderr, "[sandbox] openat64: access to %s not allowed\n", pathname);
        return 1;
    }

    va_list valist;
    va_start(valist, flags);
    if(openat64_ptr == NULL) openat64_ptr = (int (*)(int, const char *, int, ...))dlsym(handle, "openat64");
    return openat64_ptr(dirfd, pathname, flags, va_arg(valist, mode_t));
}

DIR *opendir(const char *name){
    if(check_valid_path(name) != 0){
        fprintf(stderr, "[sandbox] opendir: access to %s not allowed\n", name);
        return NULL;
    }

    if(opendir_ptr == NULL) opendir_ptr = (DIR *(*)(const char *))dlsym(handle, "opendir");
    return opendir_ptr(name);
}

// ssize_t readlink(const char *pathname, char *buf, size_t bufsiz){
//     if(check_valid_path(pathname) != 0){
//         fprintf(stderr, "[sandbox] readlink: access to %s not allowed\n", pathname);
//         return 1;
//     }
// }

// int remove(const char *pathname){
//     if(check_valid_path(pathname) != 0){
//         fprintf(stderr, "[sandbox] remove: access to %s not allowed\n", pathname);
//         return 1;
//     }
// }

// int rename(const char *oldpath, const char *newpath){
//     bool err = false;
//     if(check_valid_path(oldpath) != 0){
//         fprintf(stderr, "[sandbox] link: access to %s not allowed\n", oldpath);
//         err = true;
//     }
//     if(check_valid_path(newpath) != 0){
//         fprintf(stderr, "[sandbox] link: access to %s not allowed\n", newpath);
//         err = true;
//     }
//     if(err) return 1;
// }

// int rmdir(const char *pathname){
//     if(check_valid_path(pathname) != 0){
//         fprintf(stderr, "[sandbox] rmdir: access to %s not allowed\n", pathname);
//         return 1;
//     }
// }

int __xstat(const char *pathname, struct stat *statbuf){
    if(check_valid_path(pathname) != 0){
        fprintf(stderr, "[sandbox] __xstat: access to %s not allowed\n", pathname);
        return 1;
    }

    if(stat_ptr == NULL) stat_ptr = (int (*)(const char *, struct stat *))dlsym(handle, "__xstat");
    return stat_ptr(pathname, statbuf);
}

int __xstat64(const char *pathname, struct stat *statbuf){
    if(check_valid_path(pathname) != 0){
        fprintf(stderr, "[sandbox] __xstat64: access to %s not allowed\n", pathname);
        return 1;
    }

    if(stat64_ptr == NULL) stat64_ptr = (int (*)(const char *, struct stat *))dlsym(handle, "__xstat64");
    return stat64_ptr(pathname, statbuf);
}

// int symlink(const char *target, const char *linkpath){
//     bool err = false;
//     if(check_valid_path(target) != 0){
//         fprintf(stderr, "[sandbox] link: access to %s not allowed\n", target);
//         err = true;
//     }
//     if(check_valid_path(linkpath) != 0){
//         fprintf(stderr, "[sandbox] link: access to %s not allowed\n", linkpath);
//         err = true;
//     }
//     if(err) return 1;
// }

// int unlink(const char *pathname){
//     if(check_valid_path(pathname) != 0){
//         fprintf(stderr, "[sandbox] unlink: access to %s not allowed\n", pathname);
//         return 1;
//     }
// }

/* The following function is forbiddened */
int execl(const char *path, const char *arg, ...){
    fprintf(stderr, "[sandbox] execl(%s): not allowed\n", path);
    return 1;
}

int execlp(const char *file, const char *arg, ...){
    fprintf(stderr, "[sandbox] execlp(%s): not allowed\n", file);
    return 1;
}

int execle(const char *path, const char *arg, ...){
    fprintf(stderr, "[sandbox] execle(%s): not allowed\n", path);
    return 1;
}

int execv(const char *path, char *const argv[]){
    fprintf(stderr, "[sandbox] execv(%s): not allowed\n", path);
    return 1;
}

int execvp(const char *file, char *const argv[]){
    fprintf(stderr, "[sandbox] execvp(%s): not allowed\n", file);
    return 1;
}

int execve(const char *path, char *const argv[], char *const envp[]){
    fprintf(stderr, "[sandbox] execve(%s): not allowed\n", path);
    return 1;
}

int system(const char *command){
    fprintf(stderr, "[sandbox] system(%s): not allowed\n", command);
    return 1;
}
