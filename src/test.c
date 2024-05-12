#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "../include/universal.h"
#include "../include/book.h"
#include "../include/user.h"
int main(void){
    int fd_book = open("book.bin",O_CREAT | O_RDWR,DEFAULT_MODE); //place this in pthread function
    int fd_user = open("user.bin",O_CREAT | O_RDWR, DEFAULT_MODE);
    if(fd_book < 0 || fd_user < 0){
        fprintf(stderr,"file openeing failed\n");
        return FAILURE;
    }
    add_user(fd_user, "abhinav", "12345");
    modify_user(fd_user, "abhinav", "13452");
    delete_user(fd_user, "abhinav");
    close(fd_book); // place this in pthread function
    close(fd_user);
    return SUCCESS;
}