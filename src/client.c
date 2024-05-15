#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../include/universal.h"
int main(){
    int sd = socket(AF_INET,SOCK_STREAM,0);
    if(sd < 0){
        fprintf(stderr,"socket failed\n");
        return FAILURE;
    }
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
    int choice,valid;
    char uname[100];
    char pass[100];
    printf("enter 1 for admin user and 2 for normal user : ");
    scanf("%d",&choice);
    printf("enter username : ");
    scanf("%s",uname);
    printf("enter the password : ");
    scanf("%s",pass);
    write(sd, uname, 100);
    write(sd, pass, 100);
    read(sd,&valid,sizeof(int));
    if(choice == 1 && valid == ADMIN_USER){
        printf("1\n");
    }else if(choice == 2 && valid == NORMAL_USER){
        printf("2\n");
    }else{
        printf("invalid user credentials\n");
        return FAILURE;
    }
    return SUCCESS;
}