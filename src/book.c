#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../include/book.h"
#include "../include/utils.h"
#include "../include/universal.h"
int add_book(int fd,long long isbn,char *name,char *author,int copies){
    lseek(fd,0L,SEEK_SET);
    int size_book = sizeof(book);
    book temp;
    while(read(fd,&temp,size_book) == size_book){
        if(temp.isbn == isbn && temp.status == BOOK_EXIST){
            return ALREADY_AVAL;
        }
    }
    book b = {
        .isbn = isbn,
        .no_of_copies = copies,
        .status = BOOK_EXIST,
    };
    strcpy(b.author,author);
    strcpy(b.name,name);
    struct flock lock = lock_file(fd);
    lseek(fd,0L,SEEK_END);
    if(write(fd,&b,size_book) != size_book){
        unlock_file(fd, lock);
        return FAILURE;
    }
    char log_msg[512];
    sprintf(log_msg,"book of isbn : %lld, name : %s, author : %s, number of copies : %d added",isbn,name,author,copies);
    logger(log_msg);
    unlock_file(fd, lock);
    return SUCCESS;
}
//how to send it to client??
int search_book(int fd,int sd,long long isbn){
    lseek(fd, 0L, SEEK_SET);
    book temp;
    int size_book = sizeof(book),retval;
    while(read(fd, &temp, size_book) == size_book){
        if(temp.isbn == isbn && temp.status == BOOK_EXIST){
            char out[1024];
            sprintf(out,"book details : \nisbn : %lld\nname : %s\nauthor : %s\nno of copies available : %d\n",temp.isbn,temp.name,temp.author,temp.no_of_copies);
            retval = SUCCESS;
            write(sd,&retval,sizeof(int));
            write(sd, out, 1024);
            return SUCCESS;
        }
    }
    // printf("Book not found\n");
    retval = NOT_FOUND;
    write(sd,&retval,sizeof(int));
    return NOT_FOUND;
}
int delete_book(int fd,long long isbn){
    lseek(fd,0L,SEEK_SET);
    int size_book = sizeof(book);
    book temp;
    while(read(fd,&temp,size_book) == size_book){
        if(temp.isbn == isbn && temp.status == BOOK_EXIST){
            temp.status = BOOK_REMOVED;
            off_t start = lseek(fd,-size_book,SEEK_CUR);
            struct flock lock = lock_a_record(fd, start, size_book);
            if(write(fd,&temp,size_book) != size_book){
                unlock_a_record(fd, lock);
                return FAILURE;
            }
            char log_msg[512];
            sprintf(log_msg, "book of isbn : %lld deleted",isbn);
            logger(log_msg);
            unlock_a_record(fd, lock);
            return SUCCESS;
        }
    }
    return NOT_FOUND;
}
int modify_book(int fd,long long isbn,char *value,int copies,int choice){
    lseek(fd, 0L, SEEK_SET);
    book temp;
    int size_book = sizeof(book);
    char wx[250];
    while(read(fd, &temp, size_book) == size_book){
        if(temp.isbn == isbn && temp.status == BOOK_EXIST){
            switch (choice) {
                case 1 : 
                    sprintf(wx, "name from %s to %s",temp.name,value);
                    memset(temp.name, '\0', 100);
                    strcpy(temp.name, value);
                    break;
                case 2 :
                    sprintf(wx, "author from %s to %s",temp.author,value);
                    memset(temp.author, '\0', 100);
                    strcpy(temp.author, value);
                    break;
                case 3 :
                    sprintf(wx, "name from %d to %d",temp.no_of_copies,copies);
                    temp.no_of_copies = copies;
                    break;
                default :
                    return INVALID_CHOICE;
                    break;
            }
            off_t start = lseek(fd, -size_book, SEEK_CUR);
            struct flock lock = lock_a_record(fd, start, size_book);
            write(fd, &temp, size_book);
            char log_msg[512];
            sprintf(log_msg, "book of isbn : %lld modified its %s",isbn,wx);
            logger(log_msg);
            unlock_a_record(fd, lock);
            return SUCCESS;
        }
    }
    return NOT_FOUND;
}
bool book_available(int fd,long long isbn){
    lseek(fd, 0L, SEEK_SET);
    book temp;
    int size_book = sizeof(book);
    while(read(fd, &temp, size_book) == size_book){
        if(temp.isbn == isbn && temp.status == BOOK_EXIST && temp.no_of_copies > 0){
            return true;
        }
    }
    return false;
}