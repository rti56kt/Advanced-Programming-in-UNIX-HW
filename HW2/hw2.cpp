// #include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <sys/stat.h>

using namespace std;

extern char *optarg;
extern int optind;

struct argstruct{
    char* sopath = (char*)"./sandbox.so";
    char* basedir = (char*)"./";
    // char sopath[1024] = "./sandbox.so";
    // char basedir[1024] = "./";
};

argstruct argset;


void usage(char* progname){
    printf("usage: %s [-p sopath] [-d basedir] [--] cmd [cmd args ...]\n", progname);
    printf("        -p: set the path to sandbox.so, default = ./sandbox.so\n");
    printf("        -d: the base directory that is allowed to access, default = .\n");
    printf("        --: separate the arguments for sandbox and for the executed command\n");
    return;
}

void arg_parse(int argc, char* argv[]){
    int c;

    while((c = getopt(argc, argv, "p:d:")) != -1) {
        switch(c) {
            case 'p':
                // strcpy(argset.sopath, optarg);
                argset.sopath = optarg;
                break;
            case 'd':
                // strcpy(argset.basedir, optarg);
                argset.basedir = optarg;
                break;
            case ':':
                usage(argv[0]);
                exit(EXIT_FAILURE);
            case '?':
                usage(argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    // printf("p: %s\n", argset.sopath);
    // printf("d: %s\n", argset.basedir);

    return;
}

void check_argset(){
    bool err = false;
    char* sopath;
    char* basedir;
    // char tmp_sopath[1024] = "\0";
    // char tmp_basedir[1024] = "\0";

    if((sopath = realpath(argset.sopath, NULL)) == NULL){
        err = true;
        printf("The sopath you provide does not exist. Please check it again.\n");
    }
    if((basedir = realpath(argset.basedir, NULL)) == NULL){
        err = true;
        printf("The basedir you provide does not exist. Please check it again.\n");
    }

    // if(stat(argset.sopath.c_str(), &buf) == -1){
    //     err = true;
    //     printf("The sopath you provide does not exist. Please check it again.\n");
    // }
    // if(stat(argset.basedir.c_str(), &buf) == -1){
    //     err = true;
    //     printf("The basedir you provide does not exist. Please check it again.\n");
    // }

    if(err) exit(EXIT_FAILURE);

    // strcpy(argset.sopath, tmp_sopath);
    // strcpy(argset.basedir, tmp_basedir);
    argset.sopath = sopath;
    argset.basedir = basedir;

    return;
}

int main(int argc, char* argv[]){
    arg_parse(argc, argv);
    check_argset();

    char* cmd_arg[argc-optind+1] = {NULL};
    for(int i = 0; i < argc-optind; i++){
        cmd_arg[i] = argv[optind+i];
        // printf("%d , %s\n", i, cmd_arg[i]);
    }

    setenv("LD_PRELOAD", argset.sopath, 0);
    setenv("BASEDIR", argset.basedir, 0);
    execvp(cmd_arg[0], cmd_arg);
    return 0;
}