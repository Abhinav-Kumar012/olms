#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "../include/user.h"
#include "../include/universal.h"
int add_user(int fd,char *username,char *password){
    lseek(fd, 0L, SEEK_SET);
    int size_user = sizeof(user);
    user temp;
    while (read(fd, &temp, size_user) == size_user){
        if(strcmp(temp.name, username) == 0 && temp.exists == USER_EXIST){
            return ALREADY_AVAL;
        }
    }
    user u = {
        .exists = USER_EXIST,
    };
    strcpy(u.name,username);
    strcpy(u.password, password);
    lseek(fd, 0L, SEEK_END);
    if(write(fd, &u, size_user) != size_user){
        return FAILURE;
    }
    return SUCCESS;
}
int modify_user(int fd,char *username,char *new_password){
    lseek(fd, 0L, SEEK_SET);
    int size_user = sizeof(user);
    user temp;
    while (read(fd, &temp, size_user) == size_user){
        if(strcmp(temp.name, username) == 0 && temp.exists == USER_EXIST){
            memset(temp.password,'\0',100);
            strcpy(temp.password, new_password);
            lseek(fd, -size_user, SEEK_CUR);
            write(fd, &temp, size_user);
            return SUCCESS;
        }
    }
    return NOT_FOUND;
}
int delete_user(int fd, char *username){
    lseek(fd, 0L, SEEK_SET);
    int size_user = sizeof(user);
    user temp;
    while (read(fd, &temp, size_user) == size_user){
        if(strcmp(temp.name, username) == 0 && temp.exists == USER_EXIST){
            temp.exists = USER_REMOVED;
            lseek(fd, -size_user, SEEK_CUR);
            write(fd, &temp, size_user);
            return SUCCESS;
        }
    }
    return NOT_FOUND;
}
bool user_exists(int fd,char *username){
    lseek(fd, 0L, SEEK_SET);
    int size_user = sizeof(user);
    user temp;
    while (read(fd, &temp, size_user) == size_user){
        if(strcmp(temp.name, username) == 0 && temp.exists == USER_EXIST){
            return true;
        }
    }
    return false;
}