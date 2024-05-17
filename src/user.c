#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "../include/user.h"
#include "../include/utils.h"
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
    struct flock lock = lock_file(fd);
    lseek(fd, 0L, SEEK_END);
    if(write(fd, &u, size_user) != size_user){
        unlock_file(fd, lock);
        return FAILURE;
    }
    unlock_file(fd, lock);
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
            off_t start = lseek(fd, -size_user, SEEK_CUR);
            struct flock lock = lock_a_record(fd, start, size_user);
            write(fd, &temp, size_user);
            unlock_a_record(fd, lock);
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
            off_t start = lseek(fd, -size_user, SEEK_CUR);
            struct flock lock = lock_a_record(fd, start, size_user);
            write(fd, &temp, size_user);
            unlock_a_record(fd, lock);
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
int login(int fd,char *uname,char *pass,int choice){
    if(strcmp(uname, "admin") == 0 && strcmp(pass, "admin123") == 0 && choice == 1){
        return ADMIN_USER;
    }
    if(choice == 2){
        lseek(fd, 0L, SEEK_SET);
        int size_user = sizeof(user);
        user temp;
        while (read(fd, &temp, size_user) == size_user){
            if(strcmp(temp.name, uname) == 0 && temp.exists == USER_EXIST && strcmp(temp.password, pass) == 0){
                return NORMAL_USER;
            }
        }
    }
    return INVALID_USER;
}