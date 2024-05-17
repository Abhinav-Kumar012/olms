#include <signal.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include "../include/universal.h"
#include "../include/book.h"
#include "../include/user.h"
#include "../include/utils.h"
#include "../include/transactions.h"
int *ptr_sd;
void handle_sigint(int sig){
    printf("\nclosing socket\n");
    close(*ptr_sd);
    exit(EXIT_SUCCESS);
}
typedef struct arg_1{
    int csd;
} arg;
int normal_fxn(int csd,char *uname,int fd_books,int fd_users,int fd_trans){
    int c = -1;
    while(1){
        read(csd, &c, sizeof(int));
        if(c==0){
            char log_msg[512];
            sprintf(log_msg, "user : %s logged out",uname);
            logger(log_msg);
            break;
        }
        int status;
        switch (c) {
            case 0:
                break;
            case 1:{
                long long isbn;
                read(csd, &isbn, sizeof(long long));
                status = issue_book(fd_trans, fd_users, fd_books, uname, isbn);
                write(csd, &status, sizeof(int));
            }break;
            case 2:{
                long long isbn;
                read(csd, &isbn, sizeof(long long));
                status = return_book(fd_trans, fd_books, uname, isbn);
                write(csd, &status, sizeof(int));
            }break;
            default:
                break;
        }
    }
    return SUCCESS;
}
int admin_fxn(int csd,char *uname,int fd_books,int fd_users){
    int c = -1;
    while(1){
        read(csd,&c,sizeof(int));
        if(c == 0){
            char log_msg[512];
            sprintf(log_msg, "admin logged out");
            logger(log_msg);
            break;
        }
        long long isbn;
        int copies,stat;
        switch (c) {
            case 0:
                break;
            case 1:{
                char author[100],name[100];
                read(csd, name,100);
                read(csd, author, 100);
                read(csd, &isbn, sizeof(long long));
                read(csd, &copies, sizeof(int));
                stat = add_book(fd_books, isbn,name,author,copies);
                write(csd, &stat, sizeof(int));
            }break;
            case 2:{
                read(csd,&isbn,sizeof(long long));
                stat = delete_book(fd_books, isbn);
                write(csd, &stat, sizeof(int));
            }break;
            case 3:{
                char in[100];
                int choi;
                read(csd,&isbn,sizeof(long long));
                read(csd, &choi, sizeof(int));
                read(csd,in,100);
                read(csd,&copies,sizeof(int));
                stat = modify_book(fd_books,isbn,in,copies,choi);
                write(csd, &stat, sizeof(int));
            }break;
            case 4:{
                read(csd,&isbn,sizeof(long long));
                search_book(fd_books, csd, isbn);
            }break;
            case 5:{
                char uname[100],pass[100];
                read(csd,uname,100);
                read(csd,pass,100);
                stat = add_user(fd_users, uname, pass);
                write(csd,&stat,sizeof(int));
            }break;
            case 6:{
                char uname[100],pass[100];
                read(csd,uname,100);
                read(csd,pass,100);
                stat = modify_user(fd_users, uname, pass);
                write(csd,&stat,sizeof(int));
            }break;
            default:
                break;
        }
    }
    return SUCCESS;
}
void *thread_fun(void *args){
    int fd_book = open("book.bin",O_CREAT | O_RDWR, DEFAULT_MODE); 
    int fd_user = open("user.bin",O_CREAT | O_RDWR, DEFAULT_MODE);
    int fd_trans = open("transactions.bin", O_CREAT | O_RDWR, DEFAULT_MODE);
    int csd = ((arg *)args)->csd,choice;
    char pass[100] = {'\0'},uname[100] = {'\0'};
    read(csd, &choice, sizeof(int));
    read(csd,uname,100);
    read(csd, pass, 100);
    int user_type = login(fd_user, uname, pass,choice);
    if(user_type == INVALID_USER){
        fprintf(stderr, "invalid user credentials\n");
        write(csd, &user_type, sizeof(int));
        return NULL;
    }
    write(csd, &user_type, sizeof(int));
    switch (user_type) {
        case ADMIN_USER:
            admin_fxn(csd,uname,fd_book,fd_user);
            break;
        case NORMAL_USER:
            normal_fxn(csd, uname,fd_book,fd_user,fd_trans);
            break;
        default:
            fprintf(stderr,"invalid user\n");
            break;
    }
    close(fd_book); 
    close(fd_user);
    close(fd_trans);
    close(csd);
    free(args);
    return NULL;
}
int main(void){
    int sd = socket(AF_INET, SOCK_STREAM, 0);
    if(sd < 0){
        fprintf(stderr, "socket creation failed\n");
        return FAILURE;
    }
    ptr_sd = &sd;
    signal(SIGINT, handle_sigint);
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
    printf("  ooooooo  ooooo       oooo     oooo  oooooooo8 \n");
    printf("o888   888o 888         8888o   888  888       \n");
    printf("888     888 888         88 888o8 88   888oooooo\n");
    printf("888o   o888 888      o  88  888  88          888\n");
    printf("  88ooo88  o888ooooo88 o88o  8  o88o o88oooo888\n");
    initilise_log();
    while(true){
        socklen_t client_len = sizeof(client);
        int csd = accept(sd, (struct sockaddr *)&client,&client_len);
        if(csd < 0){
            fprintf(stderr, "cannot open client descriptor\n");
        }
        pthread_t thread;
        arg *a = malloc(sizeof(arg));
        a->csd = csd;
        if(pthread_create(&thread, NULL, thread_fun, (void*)a) < 0){
            fprintf(stderr, "cannot create new thread\n");
        }
        if(pthread_detach(thread) < 0){
            fprintf(stderr, "cannot detach a thread\n");
        }
    }
    close(sd);
    return SUCCESS;
}
