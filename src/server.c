#include <complex.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include "../include/universal.h"
#include "../include/book.h"
#include "../include/user.h"
#include "../include/transactions.h"
typedef struct arg_1{
    int csd;
} arg;
void *thread_fun(void *args){
    int *retval = (int *)malloc(sizeof(int));
    int fd_book = open("book.bin",O_CREAT | O_RDWR,DEFAULT_MODE); 
    int fd_user = open("user.bin",O_CREAT | O_RDWR, DEFAULT_MODE);
    int fd_trans = open("transactions.bin", O_CREAT | O_RDWR, DEFAULT_MODE);
    int csd = ((arg *)args)->csd;
    char pass[100] = {'\0'},uname[100] = {'\0'};
    read(csd,uname,100);
    read(csd, pass, 100);
    int user_type = login(fd_user, uname, pass);
    if(user_type == INVALID_USER){
        fprintf(stderr, "invalid user credentials\n");
        *retval = FAILURE;
        write(csd, &user_type, sizeof(int));
        pthread_exit((void *)retval);
    }
    write(csd, &user_type, sizeof(int));
    close(fd_book); 
    close(fd_user);
    close(fd_trans);
    close(csd);
    *retval = SUCCESS;
    return retval; //change it
}
int main(void){
    int sd = socket(AF_INET, SOCK_STREAM, 0);
    if(sd < 0){
        fprintf(stderr, "socket creation failed\n");
        return FAILURE;
    }
    struct sockaddr_in server = {
        .sin_family = AF_INET,
        .sin_addr = INADDR_ANY,
        .sin_port = htons(PORT),
    },client;
    if(bind(sd, (struct sockaddr *)&server, sizeof(server)) < 0){
        fprintf(stderr, "bind failed\n");
        return FAILURE;
    }
    if(listen(sd, 5) < 0){
        fprintf(stderr, "listen failed\n");
        return FAILURE;
    }
    while(true){
        socklen_t client_len = sizeof(client);
        int csd = accept(sd, (struct sockaddr *)&client,&client_len);
        if(csd < 0){
            fprintf(stderr, "cannot open client descriptor\n");
        }
        pthread_t thread;
        arg a = {
            .csd = csd,
        };
        if(pthread_create(&thread, NULL, thread_fun, (void*)&a) < 0){
            fprintf(stderr, "cannot create new thread\n");
        }
        if(pthread_detach(thread) < 0){
            fprintf(stderr, "cannot detach a thread\n");
        }
    }
    close(sd);
    return SUCCESS;
}
