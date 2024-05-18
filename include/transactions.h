#ifndef TRANSACTIONS_H
#define TRANSACTIONS_H
#define BOOK_BORROWED 0
#define BOOK_RETURNED 1
typedef struct transaction_1 {
    char username[100];
    long long isbn;
    int returned;
} transaction;
int issue_book(int fd_trans,int fd_users,int fd_books,char *uname,long long isbn);
int return_book(int fd_trans,int fd_books,char *uname,long long isbn);
int show_books(int fd_book,int fd_trans,char *uname,int csd);
#endif