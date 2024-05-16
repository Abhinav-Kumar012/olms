#include <stdbool.h>
#ifndef USER_H
#define USER_H
#define USER_EXIST 1
#define USER_REMOVED 0
typedef struct user_1 {
    char name[100];
    char password[100];
    int exists;
} user;
int add_user(int fd,char *username,char *password);
int delete_user(int fd,char *username);
int modify_user(int fd,char *username,char *new_password);
bool user_exists(int fd,char *username);
int login(int fd,char *uname,char *pass,int choice);
#endif