#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pwd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <omp.h>
#define BUFSIZE 1024
#define PORT_NO 80

int Post(char *neptunID, char *message, int NumCh)
{
    int s;
    int bytes;
    int err;
    int flag;
    char on;
    char buffer[BUFSIZE];
    unsigned int server_size;
    struct sockaddr_in server;
    char ip[] = {"193.6.135.162"};

    on = 1;
    flag = 0;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(ip);
    server.sin_port = htons(80);
    server_size = sizeof server;

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0)
    {
        fprintf(stderr, " %s: Socket creation error.\n");
        return 2;
    }
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &on, sizeof on);
    setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, &on, sizeof on);

    err = connect(s, (struct sockaddr *)&server, server_size);
    if (err < 0)
    {
        fprintf(stderr, " %s: Connecting error.\n");
        return 3;
    }
    printf("Connection is OK.\n");

    char s_data[1024];
    int l_lengt = NumCh + 27;
    char l_neptunID[strlen(neptunID)];
    strcpy(l_neptunID, neptunID);
    char l_message[strlen(message)];
    strcpy(l_message, message);
    sprintf(s_data, "POST /~vargai/post.php HTTP/1.1\r\nHost: irh.inf.unideb.hu\r\nContent-Length: %d\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\nNeptunID=%s&PostedText=%s\r\n", l_lengt, l_neptunID, l_message);

    bytes = send(s, s_data, strlen(s_data) + 1, flag);
    if (bytes <= 0)
    {
        fprintf(stderr, " %s: Sending error.\n");
        return 4;
    }
    printf("%i bytes have been sent to server.\n", bytes - 1);

    bytes = recv(s, buffer, BUFSIZE, flag);
    if (bytes < 0)
    {
        fprintf(stderr, " %s: Receiving error.\n");
        return 5;
    }
    printf(" Server's (%s:%d) acknowledgement:\n  %s\n",
           inet_ntoa(server.sin_addr), ntohs(server.sin_port), buffer);

    close(s);

    return 0;
}

int BrowseForOpen()
{
    int inode_tmp;
    char file_browser[2048];
    char current[256];
    DIR *d;
    struct stat inode;
    struct dirent *entry;

    strcpy(file_browser, ".");
    inode_tmp = stat(file_browser, &inode);
    printf("---------------------------------------\n");
    printf("|Type the folder or file name to open.|\n");
    printf("|Type \'..\' to go back.                |\n");
    printf("---------------------------------------\n");

    chdir(getenv("HOME"));
    d = opendir(".");

    while (1)
    {

        chdir(current);
        d = opendir(".");
        if (inode_tmp < 0)
        {
            fprintf(stderr, "Nem létező fájl.\n");
            return 1;
        }
        if (inode.st_mode & S_IFREG)
        {
            break;
        }
        if (inode.st_mode & S_IFDIR)
        {
            while ((entry = readdir(d)) != NULL)
            {
                if ((*entry).d_name[0] != '.')
                {
                    printf("%s\t", (*entry).d_name);
                }
            }
        }

        printf("\n");
        scanf("%s", &current);
        inode_tmp = stat(current, &inode);
        closedir(d);
    }
    printf("\n");
    return open(current, O_RDONLY);
}

char *ReadPixels(int f, int *NumCh)
{
    lseek(f, 6, SEEK_SET);
    unsigned int num;
    int size;
    read(f, &num, 4);
    char *ptr = (char *)malloc(num * 3);
    if (ptr == NULL)
    {
        fprintf(stderr, "Hiba! A memóriafoglalás sikertelen!\n");
        exit(1);
    }
    read(f, &size, 4);
    lseek(f, size, SEEK_SET);
    read(f, ptr, num * 3);

    close(f);
    *NumCh = num;
    return ptr;
}

char *Unwrap(char *Pbuff, int NumCh)
{
    char *ptr = (char *)malloc(NumCh);
    if (ptr == NULL)
    {
        fprintf(stderr, "Hiba! A memóriafoglalás sikertelen!\n");
        exit(1);
    }
#pragma omp parallel for schedule(guided)
    for (int i = 0; i < NumCh; i++)
    {
        ptr[i] = (Pbuff[(i * 3)]) << 6 | (Pbuff[(i * 3) + 1] & 7) << 3 | Pbuff[(i * 3) + 2] & 7;
    }
    free(Pbuff);
    return (ptr);
}

void WhatToDo(int sig)
{
    if (sig == SIGALRM)
    {
        fprintf(stderr, " Időkorlát túllépés.\n");
        exit(1);
    }
    if (sig == SIGINT)
    {
        pid_t pid;
        pid = fork();
        if (pid == 0)
        {
            printf(" A programot jelenleg nem tudod leállítani ctrl + c billentyűkombinációval.\n");
            raise(SIGKILL);
        }
    }
}
