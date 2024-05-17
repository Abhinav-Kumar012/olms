#define _GNU_SOURCE
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include "../include/universal.h"
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