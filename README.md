# OLMS (online library mangement system)
The Online Library Management System (OLMS) is a secure and efficient platform designed to streamline library operations. It features user authentication, password-protected administrative access, and comprehensive book management. Leveraging system calls for file handling and multithreading, along with socket programming for concurrent client access, OLMS ensures optimal performance, data consistency, and secure access to library resources.
other requirements for project are mentioned in [MiniProject.pdf](MIniProject.pdf)
## Features 
  - admin user
	- add books
	- delete books
	- modify books
	- search books
	- add users
	- modify user's password
 - normal user 
	- issue books
	- return books
	- see books to be returned
- the server generates the log file for successful change in library in ```activity.log``` file produced during runtime

## project structure
```src``` : contains source code for server and client
```include``` : contains all header files
```bin``` : will store final binaries and other files produced during runtime like logs and binary files
## How to run
### installing  prerequisites
#### ubuntu
``` bash
sudo apt update
sudo apt install build-essential -y
```
### building the project 
``` bash
git clone https://github.com/Abhinav-Kumar012/olms.git
cd olms
make all #to build both server and client
make server #to build server only
make client #to build client only
make clean #to clean bin folder
```
### running the project
to run the server 
``` bash 
cd bin
chmod +x server
./server
```
to run the client
```  bash
cd bin
chmod +x client
./client
```
Note : client can't run without starting the server

