#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <sys/types.h>
#include <sys/ptrace.h>

void errquit(const char *msg) {
	perror(msg);
	exit(-1);
}

int main(int argc, char *argv[]) {
	pid_t child;
	if(argc < 2) {
		fprintf(stderr, "usage: %s program [args ...]\n", argv[0]);
		return -1;
	}
	if((child = fork()) < 0) errquit("fork");
	if(child == 0) {
		if(ptrace(PTRACE_TRACEME, 0, 0, 0) < 0) errquit("ptrace@child");
		execvp(argv[1], argv+1);
		errquit("execvp");
	} else {
		int wait_status;
		FILE* pfile;

		pfile = fopen("5-no_more_traps.txt", "r");
		if(pfile == NULL) errquit("fopen failed");

		if(waitpid(child, &wait_status, 0) < 0) errquit("waitpid");
		ptrace(PTRACE_SETOPTIONS, child, 0, PTRACE_O_EXITKILL);
		while (WIFSTOPPED(wait_status)) {
			char buffer[3];
			unsigned long code = 0;
			struct user_regs_struct regs;

			if(ptrace(PTRACE_CONT, child, 0, 0) < 0) errquit("ptrace@parent");
			if(waitpid(child, &wait_status, 0) < 0) errquit("waitpid");

			if(WSTOPSIG(wait_status) != SIGTRAP) continue;

			fread(buffer, sizeof(char), 2, pfile);
			if(ptrace(PTRACE_GETREGS, child, 0, &regs) != 0) errquit("ptrace(GETREGS)");
			code = ptrace(PTRACE_PEEKTEXT, child, regs.rip-1, 0);
			code = (code & 0xffffffffffffff00) | strtol(buffer, NULL, 16);
			if(ptrace(PTRACE_POKETEXT, child, regs.rip-1, code) != 0) errquit("ptrace(POKETEXT)");

			regs.rip = regs.rip-1;
			if(ptrace(PTRACE_SETREGS, child, 0, &regs) != 0) errquit("ptrace(SETREGS)");
		}
		fclose(pfile);
	}
	return 0;
}