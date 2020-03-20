#include <string>
#include <vector>
#include <regex.h>
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <algorithm>
#include <sys/stat.h>
#include <sys/types.h>
#include <arpa/inet.h>

extern int optind;

#define TCP_FILE_PATH "/proc/net/tcp"
#define UDP_FILE_PATH "/proc/net/udp"
#define TCP6_FILE_PATH "/proc/net/tcp6"
#define UDP6_FILE_PATH "/proc/net/udp6"
#define HEX_IP_LEN 8
#define HEX_IP6_LEN 32
#define HEX_PORT_LEN 4

using namespace std;

struct argstruct{
    bool tcp = false;
    bool udp = false;
    char* filter = NULL;
};

struct netstat{
    string command;
    string protocol;
    string local_addr;
    string remote_addr;
    int local_port = 0;
    int remote_port = 0;
    int pid = 0;
    int inode = 0;
};


void usage(char* progname){
    printf("Usage:\n");
    printf("    %s [-t|--tcp] [-u|--udp] [filter-string]\n", progname);
    return;
}

argstruct arg_parse(int argc, char* argv[]){
    int c;
    struct argstruct argset;

    const char* shortopt = "tu";
    struct option longopt[] = {
        {"tcp", 0, NULL, 't'},
        {"udp", 0, NULL, 'u'}
    };

    while((c = getopt_long(argc, argv, shortopt, longopt, NULL)) != -1) {
        switch(c) {
            case 't':
                argset.tcp = true;
                break;
            case 'u':
                argset.udp = true;
                break;
            case '?':
                usage(argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if(optind != (argc-1) && optind != argc){
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }else{
        argset.filter = argv[optind];
    }

    if(!argset.tcp && !argset.udp){
        argset.tcp = true;
        argset.udp = true;
    }
    return argset;
}

vector<netstat> traverse_net_file(const char* filepath, string protocol, string ipver){
    vector<netstat> statlist;
    char* line = NULL;
    char* token = NULL;
    size_t buf = 0;
    FILE* fd = fopen(filepath, "r");


    if(!fd){
        fprintf(stderr, "Cannot open \"%s\"!\n", filepath);
        exit(EXIT_FAILURE);
    }
    if(getline(&line, &buf, fd) != -1){
        while(getline(&line, &buf, fd) != -1){
            struct netstat single_stat;

            token = strtok(line, " ");
            if(ipver == "4") single_stat.protocol = protocol;
            if(ipver == "6") single_stat.protocol = protocol + ipver;

            for(int i = 0; i < 10; i++){
                if(i == 1){
                    // local_address
                    if(ipver == "4"){
                        struct in_addr addr;
                        char hex_address[HEX_IP_LEN + 1] = "\0";;
                        char hex_port[HEX_PORT_LEN + 1] = "\0";;
                        char address[INET_ADDRSTRLEN] = "\0";;

                        strncpy(hex_address, token, HEX_IP_LEN);
                        strncpy(hex_port, token+HEX_IP_LEN+1, HEX_PORT_LEN);
                        addr.s_addr = strtol(hex_address, NULL, 16);
                        inet_ntop(AF_INET, &addr, address, INET_ADDRSTRLEN);

                        single_stat.local_addr = address;
                        single_stat.local_port = strtol(hex_port, NULL, 16);
                    }else if(ipver == "6"){
                        struct in6_addr addr;
                        char hex_address[HEX_IP6_LEN + 1] = "\0";;
                        char hex_port[HEX_PORT_LEN + 1] = "\0";;
                        char address[INET_ADDRSTRLEN] = "\0";;

                        strncpy(hex_address, token, HEX_IP6_LEN);
                        strncpy(hex_port, token+HEX_IP6_LEN+1, HEX_PORT_LEN);
                        for(int j = 0; j < 16; j++){
                            char hex_for_8bit[3] = "\0";
                            strncpy(hex_for_8bit, hex_address + (j * 2), 2);
                            addr.s6_addr[j] = strtol(hex_for_8bit, NULL, 16);
                        }
                        inet_ntop(AF_INET6, &addr, address, INET6_ADDRSTRLEN);

                        single_stat.local_addr = address;
                        single_stat.local_port = strtol(hex_port, NULL, 16);
                    }
                }else if(i == 2){
                    // remote_address
                    if(ipver == "4"){
                        struct in_addr addr;
                        char hex_address[HEX_IP_LEN + 1] = "\0";;
                        char hex_port[HEX_PORT_LEN + 1] = "\0";;
                        char address[INET_ADDRSTRLEN] = "\0";;

                        strncpy(hex_address, token, HEX_IP_LEN);
                        strncpy(hex_port, token+HEX_IP_LEN+1, HEX_PORT_LEN);
                        addr.s_addr = strtol(hex_address, NULL, 16);
                        inet_ntop(AF_INET, &addr, address, INET_ADDRSTRLEN);

                        single_stat.remote_addr = address;
                        single_stat.remote_port = strtol(hex_port, NULL, 16);
                    }else if(ipver == "6"){
                        struct in6_addr addr;
                        char hex_address[HEX_IP6_LEN + 1] = "\0";;
                        char hex_port[HEX_PORT_LEN + 1] = "\0";;
                        char address[INET_ADDRSTRLEN] = "\0";;

                        strncpy(hex_address, token, HEX_IP6_LEN);
                        strncpy(hex_port, token+HEX_IP6_LEN+1, HEX_PORT_LEN);
                        for(int j = 0; j < 16; j++){
                            char hex_for_8bit[3] = "\0";
                            strncpy(hex_for_8bit, hex_address + (j * 2), 2);
                            addr.s6_addr[j] = strtol(hex_for_8bit, NULL, 16);
                        }
                        inet_ntop(AF_INET6, &addr, address, INET6_ADDRSTRLEN);

                        single_stat.remote_addr = address;
                        single_stat.remote_port = strtol(hex_port, NULL, 16);
                    }
                }else if(i == 9){
                    // inode
                    single_stat.inode = atoi(token);
                }
                token = strtok(NULL, " ");
            }
            if(single_stat.inode) statlist.push_back(single_stat);
        }
    }
    fclose(fd);
    if(line){
        free(line);
        line = NULL;
    }
    return statlist;
}

void traverse_proc_pid(vector<netstat>& whole_list){
    struct dirent* proc_direntp;
    DIR* proc_dir = opendir("/proc");
    regex_t reg;
    regmatch_t pmatch[2];
    const char* pattern = "^socket\\:\\[([0-9]+)\\]";

    regcomp(&reg, pattern, REG_EXTENDED);

    if(!proc_dir){
        fprintf(stderr, "Cannot open \"/proc\"!\n");
        exit(EXIT_FAILURE);
    }

    while((proc_direntp = readdir(proc_dir)) != NULL){
        bool is_pid = true;

        for(uint i = 0; i < strlen(proc_direntp->d_name); i++){
            if(!isdigit(proc_direntp->d_name[i])){
                is_pid = false;
                break;
            }
        }
        if(!is_pid){
            continue;
        }else{
            string pid = proc_direntp->d_name;
            string fd_path = "/proc/" + pid + "/fd";
            struct dirent* fd_direntp;
            DIR* fd_dir = opendir(fd_path.c_str());

            if(!fd_dir){
                continue;
            }else{
                while((fd_direntp = readdir(fd_dir)) != NULL){
                    string fd_link = fd_direntp->d_name;
                    string fd_link_path = fd_path + "/" + fd_link;
                    char target_path[32] = "\0";

                    if(readlink(fd_link_path.c_str(), target_path, sizeof(target_path)-1) != -1){
                        if(regexec(&reg, target_path, 2, pmatch, 0) == 0){
                            char inode[pmatch[1].rm_eo - pmatch[1].rm_so + 1] = "\0";
                            strncpy(inode, target_path + pmatch[1].rm_so, pmatch[1].rm_eo - pmatch[1].rm_so);

                            for(uint i = 0; i < whole_list.size(); i++){
                                if(atoi(inode) == whole_list.at(i).inode && whole_list.at(i).pid == 0){
                                    string cmd;
                                    string args;
                                    string token;
                                    string cmdline;
                                    string cmd_path = "/proc/" + pid + "/cmdline";
                                    FILE* fd = fopen(cmd_path.c_str(), "r");
                                    char* line = NULL;
                                    size_t pos = 0;
                                    size_t buf = 0;

                                    while(getdelim(&line, &buf, '\0', fd) != -1){
                                        cmdline = cmdline + " " + line;
                                    }
                                    if(line){
                                        free(line);
                                        line = NULL;
                                    }
                                    cmdline = cmdline.substr(1, cmdline.size());
                                    if((pos = cmdline.find(" ", 0)) != string::npos){
                                        token = cmdline.substr(0, pos);
                                        args = cmdline.substr(pos, cmdline.size());
                                        if((pos = token.rfind("/")) != string::npos){
                                            cmd = token.substr(pos+1, token.size());
                                        }else{
                                            cmd = token;
                                        }
                                        cmdline = cmd + args;
                                    }else{
                                        if((pos = cmdline.rfind("/")) != string::npos){
                                            cmd = cmdline.substr(pos+1, cmdline.size());
                                        }else{
                                            cmd = cmdline;
                                        }
                                        cmdline = cmd;
                                    }
                                    whole_list.at(i).command = cmdline;
                                    whole_list.at(i).pid = atoi(pid.c_str());
                                    fclose(fd);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    regfree(&reg);

    return;
}

bool filter(string filter_pattern, string command){
    regex_t reg;
    regmatch_t pmatch[1];

    regcomp(&reg, filter_pattern.c_str(), REG_EXTENDED);

    if(regexec(&reg, command.c_str(), 1, pmatch, 0) == 0){
        regfree(&reg);
        return true;
    }else{
        regfree(&reg);
        return false;
    }
}

void output(argstruct argset, vector<netstat>& whole_list){
    bool tcp_title_isprint = false;
    bool udp_title_isprint = false;
    for(uint i = 0; i < whole_list.size(); i++){
        char outputmsg[1024] = "\0";
        string local_addr_port = whole_list.at(i).local_addr + ":" + to_string(whole_list.at(i).local_port);
        string remote_addr_port = whole_list.at(i).remote_addr + ":" + to_string(whole_list.at(i).remote_port);
        string pid_cmd = to_string(whole_list.at(i).pid) + "/" + whole_list.at(i).command;

        if(pid_cmd == "0/") pid_cmd = "-";
        if(argset.tcp && (whole_list.at(i).protocol == "tcp" || whole_list.at(i).protocol == "tcp6")){
            if(!tcp_title_isprint){
                printf("List of TCP connections:\n");
                printf("Proto Local Address           Foreign Address         PID/Program name and arguments\n");
                tcp_title_isprint = true;
            }
            if(argset.filter){
                if(filter(argset.filter, whole_list.at(i).command)){
                    sprintf(outputmsg, "%-6s%-24s%-24s%s", whole_list.at(i).protocol.c_str(), local_addr_port.c_str(), remote_addr_port.c_str(), pid_cmd.c_str());
                    printf("%s\n", outputmsg);
                }
            }else{
                sprintf(outputmsg, "%-6s%-24s%-24s%s", whole_list.at(i).protocol.c_str(), local_addr_port.c_str(), remote_addr_port.c_str(), pid_cmd.c_str());
                printf("%s\n", outputmsg);
            }
        }
        if(argset.udp && (whole_list.at(i).protocol == "udp" || whole_list.at(i).protocol == "udp6")){
            if(!udp_title_isprint){
                if(tcp_title_isprint) printf("\n");
                printf("List of UDP connections:\n");
                printf("Proto Local Address           Foreign Address         PID/Program name and arguments\n");
                udp_title_isprint = true;
            }
            if(argset.filter){
                if(filter(argset.filter, whole_list.at(i).command)){
                    sprintf(outputmsg, "%-6s%-24s%-24s%s", whole_list.at(i).protocol.c_str(), local_addr_port.c_str(), remote_addr_port.c_str(), pid_cmd.c_str());
                    printf("%s\n", outputmsg);
                }
            }else{
                sprintf(outputmsg, "%-6s%-24s%-24s%s", whole_list.at(i).protocol.c_str(), local_addr_port.c_str(), remote_addr_port.c_str(), pid_cmd.c_str());
                printf("%s\n", outputmsg);
            }
        }
    }
}

int main(int argc, char* argv[]){
    struct argstruct argset;
    argset = arg_parse(argc, argv);
    vector<netstat> whole_list;

    if(argset.tcp){
        vector<netstat> tcp_list = traverse_net_file(TCP_FILE_PATH, "tcp", "4");
        whole_list.insert(whole_list.end(), tcp_list.begin(), tcp_list.end());
        vector<netstat> tcp6_list = traverse_net_file(TCP6_FILE_PATH, "tcp", "6");
        whole_list.insert(whole_list.end(), tcp6_list.begin(), tcp6_list.end());
    }
    if(argset.udp){
        vector<netstat> udp_list = traverse_net_file(UDP_FILE_PATH, "udp", "4");
        whole_list.insert(whole_list.end(), udp_list.begin(), udp_list.end());
        vector<netstat> udp6_list = traverse_net_file(UDP6_FILE_PATH, "udp", "6");
        whole_list.insert(whole_list.end(), udp6_list.begin(), udp6_list.end());
    }
    traverse_proc_pid(whole_list);
    output(argset, whole_list);
    return 0;
}
