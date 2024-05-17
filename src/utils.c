#define _GNU_SOURCE
#include <time.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "../include/universal.h"
static int log_fd;
pthread_mutex_t mutex_log = PTHREAD_MUTEX_INITIALIZER;
struct flock lock_a_record(int fd,off_t start,int record_size){
    struct flock lock = {
        .l_pid = 0,
        .l_type = F_WRLCK,
        .l_whence = SEEK_SET,
        .l_start = start,
        .l_len = record_size,
    };
    if(fcntl(fd,F_OFD_SETLKW,&lock) == -1){
        fprintf(stderr, "record not locked\n");
    }
    return lock;
}
int unlock_a_record(int fd,struct flock lock){
    lock.l_type = F_UNLCK;
    if(fcntl(fd,F_OFD_SETLK,&lock) == -1){
        fprintf(stderr, "record not unlocked\n");
        return FAILURE;
    }
    return SUCCESS;
}
struct flock lock_file(int fd){
    struct flock lock = {
        .l_pid = 0,
        .l_type = F_WRLCK,
        .l_whence = SEEK_SET,
        .l_start = 0,
        .l_len = 0,
    };
    if(fcntl(fd, F_OFD_SETLKW,&lock) == -1){
        fprintf(stderr, "file not locked");
    }
    return lock;
}
int unlock_file(int fd,struct flock lock){
    lock.l_type = F_UNLCK;
    if(fcntl(fd,F_OFD_SETLK,&lock) == -1){
        fprintf(stderr, "file not unlocked\n");
        return FAILURE;
    }
    return SUCCESS;
}
void initilise_log(){
    log_fd = open("activity.log",O_CREAT | O_RDWR,DEFAULT_MODE);
}
void logger(char * message){
    lseek(log_fd, 0L, SEEK_END);
    time_t t;
    time(&t);
    struct tm *tmp  = localtime(&t);
    char ftime[50],final_msg[1024];
    strftime(ftime, 100, "%d-%m-%Y %H:%M:%S", tmp);
    sprintf(final_msg, "[%s] %s\n",ftime,message);
    pthread_mutex_lock(&mutex_log);
    write(log_fd, final_msg, strlen(final_msg));
    pthread_mutex_unlock(&mutex_log);
    printf("%s",final_msg);
}