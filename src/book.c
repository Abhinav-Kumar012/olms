#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../include/book.h"
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
    lseek(fd,0L,SEEK_END);
    if(write(fd,&b,size_book) != size_book){
        return FAILURE;
    }
    return SUCCESS;
}
//how to send it to client??
int search_book(int fd,long long isbn){
    lseek(fd, 0L, SEEK_SET);
    book temp;
    int size_book = sizeof(book);
    while(read(fd, &temp, size_book) == size_book){
        if(temp.isbn == isbn && temp.status == BOOK_EXIST){
            printf("book details : \n");
            printf("isbn : %lld\n",temp.isbn);
            printf("name : %s\n",temp.name);
            printf("author : %s\n",temp.author);
            printf("no of copies available : %d\n",temp.no_of_copies);
            return SUCCESS;
        }
    }
    printf("Book not found\n");
    return NOT_FOUND;
}
int delete_book(int fd,long long isbn){
    lseek(fd,0L,SEEK_SET);
    int size_book = sizeof(book);
    book temp;
    while(read(fd,&temp,size_book) == size_book){
        if(temp.isbn == isbn && temp.status == BOOK_EXIST){
            temp.status = BOOK_REMOVED;
            lseek(fd,-size_book,SEEK_CUR);
            if(write(fd,&temp,size_book) != size_book){
                return FAILURE;
            }
            return SUCCESS;
        }
    }
    return NOT_FOUND;
}
int modify_book(int fd,long long isbn,char *value,int copies,int choice){
    lseek(fd, 0L, SEEK_SET);
    book temp;
    int size_book = sizeof(book);
    while(read(fd, &temp, size_book) == size_book){
        if(temp.isbn == isbn && temp.status == BOOK_EXIST){
            switch (choice) {
                case 1 : 
                    memset(temp.name, '\0', 100);
                    strcpy(temp.name, value);
                    break;
                case 2 :
                    memset(temp.author, '\0', 100);
                    strcpy(temp.author, value);
                    break;
                case 3 :
                    temp.no_of_copies = copies;
                    break;
                default :
                    return INVALID_CHOICE;
                    break;
            }
            lseek(fd, -size_book, SEEK_CUR);
            write(fd, &temp, size_book);
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