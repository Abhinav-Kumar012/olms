#ifndef BOOK_H
#define BOOK_H
#define BOOK_EXIST 1
#define BOOK_REMOVED 0
typedef struct book_1 {
    long long isbn;
    char name[100];
    char author[100];
    int no_of_copies;
    int status;
} book;
int add_book(int fd,long long isbn,char *name,char *author,int copies);
int delete_book(int fd,long long isbn);
int search_book(int fd,long long isbn);
int modify_book(int fd,long long isbn,char *value,int copies,int choice);
#endif