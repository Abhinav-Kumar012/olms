#ifndef UTILS_H
#define UTILS_H
#include <fcntl.h>
struct flock lock_a_record(int fd,off_t start,int record_size);
int unlock_a_record(int fd,struct flock lock);
struct flock lock_file(int fd);
int unlock_file(int fd,struct flock lock);
void initilise_log();
void logger(char * message);
#endif