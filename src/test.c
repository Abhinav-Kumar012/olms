#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "../include/universal.h"
#include "../include/book.h"
int main(void){
    int fd = open("book.bin",O_CREAT | O_RDWR,DEFAULT_MODE);
    if(fd < 0){
        fprintf(stderr,"file openeing failed\n");
        return FAILURE;
    }
    return SUCCESS;
}