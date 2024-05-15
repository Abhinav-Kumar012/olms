#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../include/book.h"
#include "../include/user.h"
#include "../include/universal.h"
#include "../include/transactions.h"
int issue_book(int fd_trans,int fd_users,int fd_books,char *uname,long long isbn){
    lseek(fd_trans, 0L, SEEK_SET);
    int size_trans = sizeof(transaction);
    transaction temp;
    if(user_exists(fd_users, uname) && book_available(fd_books, isbn)){
        while(read(fd_trans, &temp, size_trans) == size_trans){
            if(temp.isbn == isbn && strcmp(temp.username, uname) == 0 && temp.returned == BOOK_BORROWED){
                return BOOK_ALREADY_BORROWED;
            }
        }
        book b_temp;
        int size_book = sizeof(book);
        lseek(fd_books, 0L, SEEK_SET);
        while(read(fd_books, &b_temp, size_book) == size_book){
            if(b_temp.isbn == isbn && b_temp.status == BOOK_EXIST && b_temp.no_of_copies > 0){
                b_temp.no_of_copies -= 1;
                lseek(fd_books, -size_book, SEEK_CUR);
                write(fd_books, &b_temp, size_book);
                break;
            }
        }
        transaction t = {
            .isbn = isbn,
            .returned = BOOK_BORROWED,
        };
        strcpy(t.username, uname);
        lseek(fd_trans, 0L, SEEK_END);
        write(fd_trans, &t, size_trans);
        return SUCCESS;
    }
    return FAILURE;
}
int return_book(int fd_trans,int fd_books,char *uname,long long isbn){
    lseek(fd_trans, 0L, SEEK_SET);
    int size_trans = sizeof(transaction);
    transaction temp;
    while(read(fd_trans, &temp, size_trans) == size_trans){
        if(temp.isbn == isbn && strcmp(temp.username, uname) == 0 && temp.returned == BOOK_BORROWED){
            book b_temp;
            int size_book = sizeof(book);
            bool book_found = false;
            lseek(fd_books, 0L, SEEK_SET);
            while(read(fd_books, &b_temp, size_book) == size_book){
                if(b_temp.isbn == isbn && b_temp.status == BOOK_EXIST){
                    b_temp.no_of_copies += 1;
                    lseek(fd_books, -size_book, SEEK_CUR);
                    write(fd_books, &b_temp, size_book);
                    book_found = true;
                    break;
                }
            }
            if(book_found == false){
                return FAILURE;
            }
            temp.returned = BOOK_RETURNED;
            lseek(fd_trans, -size_trans, SEEK_CUR);
            write(fd_trans, &temp, size_trans);
            return SUCCESS;
        }
    }
    return BOOK_NOT_BORROWED;
}