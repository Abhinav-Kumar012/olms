#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "../include/universal.h"
#include "../include/book.h"
#include "../include/user.h"
#include "../include/transactions.h"
int main(void){
    int fd_book = open("book.bin",O_CREAT | O_RDWR,DEFAULT_MODE); //place this in pthread function
    int fd_user = open("user.bin",O_CREAT | O_RDWR, DEFAULT_MODE);
    int fd_trans = open("transactions.bin", O_CREAT | O_RDWR, DEFAULT_MODE);
    if(fd_book < 0 || fd_user < 0 || fd_trans < 0){
        fprintf(stderr,"file openeing failed\n");
        return FAILURE;
    }
    int x = issue_book(fd_trans, fd_user, fd_book, "abhinav", 1234L);
    int y = issue_book(fd_trans, fd_user, fd_book, "abhinav", 1234L);
    int z = return_book(fd_trans, fd_book, "abhinav", 1234L);
    int p = return_book(fd_trans, fd_book, "abhinav", 1234L);
    printf("%d %d %d %d\n",x,y,z,p);
    close(fd_book); // place this in pthread function
    close(fd_user);
    close(fd_trans);
    return SUCCESS;
}