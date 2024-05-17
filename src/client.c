#include <complex.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../include/universal.h"
int take_creds(int sd){
    int choice,valid;
    char uname[100],pass[100];
    printf("enter 1 for admin user and 2 for normal user : ");
    scanf("%d",&choice);
    printf("enter username : ");
    scanf("%s",uname);
    printf("enter the password : ");
    scanf("%s",pass);
    write(sd, &choice, sizeof(int));
    write(sd, uname, 100);
    write(sd, pass, 100);
    read(sd,&valid,sizeof(int));
    return valid;
}
int admin_menu(int sd){
    int c1 = -1;
    while(1){
        printf("add a book -> 1\ndelete a number -> 2\nmodify a book -> 3\nsearch a book -> 4\nadd a user -> 5\nmodify password of a user -> 6\nlog out -> 0\n");
        printf("enter a option : ");
        scanf("%d",&c1);
        if(c1 < 0 && c1 > 6){
            fprintf(stderr, "enter a valid choice\n");
            continue;
        }
        write(sd, &c1, sizeof(int));
        int copies,status;
        long long isbn;
        switch (c1) {
            case 0:{
                printf("logged out\n");
                fflush(stdout);
                exit(SUCCESS);
            }break;
            case 1:{
                char name[100],author[100];
                printf("enter name of book : ");
                fflush(stdout);
                read(STDIN_FILENO, name, 100);
                printf("enter name of author : ");
                fflush(stdout);
                read(STDIN_FILENO, author, 100);
                printf("enter isbn of book : ");
                scanf("%lld",&isbn);
                printf("enter number of copies : ");
                scanf("%d",&copies);
                int len_1 = strcspn(name, "\n");
                name[len_1] = '\0';
                int len_2 = strcspn(author, "\n");
                author[len_2] = '\0';
                write(sd, name, 100);
                write(sd, author, 100);
                write(sd, &isbn, sizeof(long long));
                write(sd, &copies, sizeof(int));
                read(sd,&status,sizeof(int));
                if(status == ALREADY_AVAL){
                    printf("book of same isbn already exist\n");
                }else if(status == SUCCESS){
                    printf("book added successfully\n");
                }else{
                    printf("failed to add book\n");
                }
            }break;
            case 2:{
                printf("enter isbn of book to be deleted : ");
                scanf("%lld",&isbn);
                write(sd, &isbn, sizeof(long long));
                read(sd,&status,sizeof(int));
                if(status == NOT_FOUND){
                    fprintf(stderr, "book not found\n");
                }else if(status == SUCCESS){
                    printf("book deleted successfully\n");
                }else{
                    fprintf(stderr,"failed to delete the book\n");
                }
            }break;
            case 3:{
                printf("enter isbn of book to be modified : ");
                scanf("%lld",&isbn);
                int choi = -1;
                char in[100];
                printf("1->name\n2->author\n3->no of copies\n");
                printf("enter a option from above : ");
                scanf("%d",&choi);
                switch (choi) {
                    case 1:
                        printf("enter the name of book : ");
                        fflush(stdout);
                        read(STDIN_FILENO, in, 100);
                        break;
                    case 2:
                        printf("enter the author of book : ");
                        fflush(stdout);
                        read(STDIN_FILENO, in, 100);
                        break;
                    case 3:
                        printf("enter number of copies : ");
                        scanf("%d",&copies);
                        break;
                    default:
                        fprintf(stderr, "choose a valid option\n");   
                        continue;
                        break;
                }
                int len_3 = strcspn(in, "\n");
                in[len_3] = '\0';
                write(sd, &isbn, sizeof(long long));
                write(sd,&choi,sizeof(int));
                write(sd, in, 100);
                write(sd, &copies, sizeof(int));
                read(sd,&status,sizeof(int));
                if(status == INVALID_CHOICE){
                    fprintf(stderr, "choose a valid option\n");
                }else if(status == SUCCESS){
                    printf("book modified successfully\n");
                }else{
                    fprintf(stderr,"failed to modify book\n");
                }
            }break;
            case 4:{
                printf("enter isbn of book to be shown : ");
                scanf("%lld",&isbn);
                write(sd, &isbn, sizeof(long long));
                read(sd,&status,sizeof(int));
                if(status == SUCCESS){
                    char out[1024];
                    read(sd,out,1024);
                    printf("\n%s\n",out);
                }else{
                    fprintf(stderr, "cannot find the book\n");
                }
            }break;
            case 5:{
                char username[100],password[100];
                printf("username : ");
                fflush(stdout);
                read(STDIN_FILENO,username,100);
                printf("password : ");
                fflush(stdout);
                read(STDIN_FILENO,password,100);
                int len_4 = strcspn(username, "\n");
                int len_5 = strcspn(password, "\n");
                username[len_4] = '\0';
                password[len_5] = '\0';
                write(sd, username, 100);
                write(sd, password, 100);
                read(sd,&status, sizeof(int));
                if(status == ALREADY_AVAL){
                    fprintf(stderr, "user with same username already exists\n");
                }else if(status == SUCCESS){
                    printf("user added successully\n");
                }else{
                    fprintf(stderr,"failed to add user\n");
                }
            }break;
            case 6:{
                char uname[100],pass[100];
                printf("username : ");
                fflush(stdout);
                read(STDIN_FILENO,uname,100);
                printf("new password : ");
                fflush(stdout);
                read(STDIN_FILENO,pass,100);
                int len_6 = strcspn(uname, "\n");
                int len_7 = strcspn(pass, "\n");
                uname[len_6] = '\0';
                pass[len_7] = '\0';
                write(sd, uname, 100);
                write(sd, pass, 100);
                read(sd,&status, sizeof(int));
                if(status == NOT_FOUND){
                    fprintf(stderr, "user not found\n");
                }else if(status == SUCCESS){
                    printf("user password modified successfully\n");
                }else{
                    fprintf(stderr, "failed to modify user password\n");
                }
            }break;
            default:{
                fprintf(stderr,"enter a valid option\n");
            }break;
        }
    };
    return SUCCESS;
}
int normal_menu(int sd){
    int c1 = -1;
    while(1){
        printf("issue a book -> 1\nreturn a book -> 2\nlog out -> 0\n");
        printf("enter a option : ");
        scanf("%d",&c1);
        if(c1 < 0 && c1 > 2){
            fprintf(stderr, "enter a valid choice\n");
            continue;
        }
        write(sd, &c1, sizeof(int));
        int status;
        long long isbn;
        switch (c1) {
            case 0:{
                printf("logged out\n");
                fflush(stdout);
                exit(SUCCESS);
            }break;
            case 1:{
                printf("enter isbn of book to be borrowed : ");
                scanf("%lld",&isbn);
                write(sd, &isbn, sizeof(long long));
                read(sd, &status, sizeof(int));
                if(status == BOOK_ALREADY_BORROWED){
                    fprintf(stderr,"book has already been borrowed\n");
                }else if (status == SUCCESS) {
                    printf("book borrowed successfully\n");
                }else{
                    fprintf(stderr,"failed to borrow book\n");
                }
            }break;
            case 2:{
                printf("enter isbn of book to be returned : ");
                scanf("%lld",&isbn);
                write(sd, &isbn, sizeof(long long));
                read(sd, &status, sizeof(int));
                if(status == BOOK_NOT_BORROWED){
                    fprintf(stderr,"book has not been borrowed\n");
                }else if (status == SUCCESS) {
                    printf("book returned successfully\n");
                }else{
                    fprintf(stderr,"failed to return book\n");
                }
            }break;
            default:
                break;
        }
    }
}
int menu(int sd){
    int valid = take_creds(sd);
    if(valid == ADMIN_USER){
        return admin_menu(sd);
    }else if(valid == NORMAL_USER){
        return normal_menu(sd);
    }else{
        printf("invalid user credentials\n");
        return FAILURE;
    }
}
int main(){
    int sd = socket(AF_INET,SOCK_STREAM,0);
    if(sd < 0){
        fprintf(stderr,"socket failed\n");
        return FAILURE;
    }
    signal(SIGINT, SIG_IGN);
    char link[15] = LINK;
    struct sockaddr_in server = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = inet_addr(link),
        .sin_port = htons(PORT)
    };
    if(connect(sd,(struct sockaddr *)&server,sizeof(server)) == -1){
        fprintf(stderr,"connect failed\n");
        return FAILURE;
    }
    menu(sd);
    close(sd);
    return SUCCESS;
}