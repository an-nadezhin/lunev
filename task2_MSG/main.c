#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <wait.h>

int getNum(int argc, char **argv);

typedef struct msgbuf {
    long mtype;
    int mtext[1];
    pid_t pid;
} message_buf;

int main(int argc, char **argv) {
    int n = getNum(argc, argv);
    int msgid = 0;
    message_buf msg;

    if ((msgid = msgget(IPC_PRIVATE, IPC_CREAT | 0644)) == -1) {
        printf("ERROR in msgget!\n");
    }

    for (int i = 1; i < n; i++) {
        if (fork() == 0) {
            msg.mtype = i;
            msg.mtext[0] = i + 15;
            msg.pid = getpid();
            if (msgsnd(msgid, (void *) &msg, sizeof(msg.mtext) + sizeof(msg.pid), IPC_NOWAIT) == -1) {
                printf("ERROR in msgsnd!\n");
            }
            return 0;
        }
    }

    for (int i = 1; i < n; i++) {
        if (msgrcv(msgid, (void *) &msg, sizeof(msg.mtext) + sizeof(msg.pid), i, 0) == -1) {
            printf("ERROR in msgrcv!\n");
        }
        printf("%d) %d, %d;\n", i, msg.mtext[0], msg.pid);
    }

    if(msgctl(msgid, IPC_RMID, NULL) == -1) {
        printf("ERORR in msgctl!\n");
    }
    return 0;
}

int getNum(int argc, char **argv) {

    char *endptr = NULL;
    if (argc == 1) {
        printf("Empty pointer\n");
        return 0;
    }
    long n = strtol(argv[1], &endptr, 10);
    if (errno == ERANGE) {
        printf("Overflow\n");
        return 0;
    }
    if (endptr[0] != '\0') {
        printf("Wrong number\n");
        return 0;
    }
    return n;
}
