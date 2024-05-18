#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../include/book.h"
#include "../include/user.h"
#include "../include/utils.h"
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
                off_t start = lseek(fd_books, -size_book, SEEK_CUR);
                struct flock lock = lock_a_record(fd_books, start, size_book);
                write(fd_books, &b_temp, size_book);
                unlock_a_record(fd_books, lock);
                break;
            }
        }
        transaction t = {
            .isbn = isbn,
            .returned = BOOK_BORROWED,
        };
        strcpy(t.username, uname);
        struct flock lock = lock_file(fd_trans);
        lseek(fd_trans, 0L, SEEK_END);
        write(fd_trans, &t, size_trans);
        char log_msg[512];
        sprintf(log_msg, "user with username : %s issued book of isbn : %lld",uname,isbn);
        logger(log_msg);
        unlock_file(fd_trans, lock);
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
                    off_t start = lseek(fd_books, -size_book, SEEK_CUR);
                    struct flock lock = lock_a_record(fd_books, start, size_book);
                    write(fd_books, &b_temp, size_book);
                    unlock_a_record(fd_books, lock);
                    book_found = true;
                    break;
                }
            }
            if(book_found == false){
                return FAILURE;
            }
            temp.returned = BOOK_RETURNED;
            off_t start = lseek(fd_trans, -size_trans, SEEK_CUR);
            struct flock lock = lock_a_record(fd_trans, start, size_trans);
            write(fd_trans, &temp, size_trans);
            char log_msg[512];
            sprintf(log_msg, "user with username : %s returned book of isbn : %lld",uname,isbn);
            logger(log_msg);
            unlock_a_record(fd_trans, lock);
            return SUCCESS;
        }
    }
    return BOOK_NOT_BORROWED;
}
int show_books(int fd_book,int fd_trans,char *uname,int csd){
    struct flock lock = lock_file(fd_trans);
    lseek(fd_trans, 0L, SEEK_SET);
    int size_trans = sizeof(transaction),no_of_books_raw = 0,curr_ind = 0;
    transaction temp;
    while(read(fd_trans,&temp,size_trans) == size_trans){
        if(strcmp(temp.username, uname) == 0 && temp.returned == BOOK_BORROWED){
            ++no_of_books_raw;
        }
    }
    long long isbns[no_of_books_raw];
    lseek(fd_trans, 0L, SEEK_SET);
    while(read(fd_trans,&temp,size_trans) == size_trans){
        if(strcmp(temp.username, uname) == 0 && temp.returned == BOOK_BORROWED){
            isbns[curr_ind] = temp.isbn;
            ++curr_ind;
        }
    }
    unlock_file(fd_trans, lock);
    int curr_ind2 = 0;
    book *books_borrowed[curr_ind],*temp_book;
    for(int i=0;i<curr_ind;i++){
        temp_book = book_exists(fd_book, isbns[i]);
        if(temp_book != NULL){
            books_borrowed[curr_ind2] = temp_book;
            ++curr_ind2;
        }
    }
    write(csd, &curr_ind2, sizeof(int));
    char out[1024];
    for(int i=0;i<curr_ind2;i++){
        sprintf(out,"\nisbn : %lld\nname : %s\nauthor : %s\n",books_borrowed[i]->isbn,books_borrowed[i]->name,books_borrowed[i]->author);
        write(csd,out,1024);
        free(books_borrowed[i]);
    }
    return SUCCESS;
}