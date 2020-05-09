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
                argset.sopath = optarg;
                break;
            case 'd':
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

    return;
}

void check_argset(){
    bool err = false;
    char* sopath;
    char* basedir;

    if((sopath = realpath(argset.sopath, NULL)) == NULL){
        err = true;
        printf("The sopath you provide does not exist. Please check it again.\n");
    }
    if((basedir = realpath(argset.basedir, NULL)) == NULL){
        err = true;
        printf("The basedir you provide does not exist. Please check it again.\n");
    }

    if(err) exit(EXIT_FAILURE);

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
    }

    setenv("LD_PRELOAD", argset.sopath, 0);
    setenv("BASEDIR", argset.basedir, 0);
    execvp(cmd_arg[0], cmd_arg);
    return 0;
}