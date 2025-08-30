#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>


// 用户消息缓冲区
typedef struct msgbuf {
    long mtype; // 消息类型
    char mtext[64]; // 消息正文
} Msgbuf;
#define MSGSIZE (sizeof(msgbuf) - sizeof(long))

int main() {
    
    key_t key = ftok(".", 'a');
    if(-1 == key){
        printf("ftok error\n");
        exit(-1);
    }
    // 创建消息队列
    int msgid = msgget(key, IPC_CREAT | 0666);
    if(-1 == msgid){
        printf("msgget error\n");
        exit(-1);
    }

    // 发送消息
    Msgbuf msgbuf;
    msgbuf.mtype = 1;
    //fgets(msgbuf.mtext, 64, stdin);
    // 发送三个消息体
    strcpy(msgbuf.mtext, "message 1");
    int ret = msgsnd(msgid, &msgbuf, MSGSIZE, 0);
    if (ret == -1) {
        printf("msgsnd error\n");
        exit(-1);
    }

    msgbuf.mtype = 2;
    strcpy(msgbuf.mtext, "message 2");
    msgsnd(msgid, &msgbuf, MSGSIZE, 0);
    
    msgbuf.mtype = 3;
    strcpy(msgbuf.mtext, "message 3");
    msgsnd(msgid, &msgbuf, MSGSIZE, 0);
    return 0;
}
