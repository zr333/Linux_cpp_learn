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
    Msgbuf msgbuf;
    int count = 0;

    struct msqid_ds mqs;
    memset(&mqs, 0, sizeof(mqs));
    msgctl(msgid,IPC_STAT, &mqs);
    printf("msg_qnum: %ld\n", mqs.msg_qnum); // 消息队列中消息的个数
    printf("--------------------------------------\n");

    while(1){
        if(msgrcv(msgid, &msgbuf, MSGSIZE, 0, 0) == -1){
            printf("msgrcv error\n");
            exit(-1);
        }
        /*
        遍历查找：内核从链表头开始遍历，根据 msgtype 的值寻找匹配的节点：
        msgtype == 0：匹配第一个节点（链表头）。
        msgtype > 0：匹配第一个 mtype 等于 msgtype 的节点。
        msgtype < 0：匹配所有 mtype 小于等于 |msgtype| 的节点，并从中选出 mtype 值最小的那个节点。如果多个节点具有相同的最小 mtype，则选择其中最早的（即最靠近链表头的）。
        */
        printf("recv msg: %s\n", msgbuf.mtext);

        msgctl(msgid,IPC_STAT, &mqs);
        printf("msg_qnum: %ld\n", mqs.msg_qnum);
        printf("--------------------------------------\n");
        count++;
        if(count == 3){
            break;
        }
        
    }
    
    
    
    
    // 删除消息队列
    msgctl(msgid, IPC_RMID, NULL);
    return 0;
}
