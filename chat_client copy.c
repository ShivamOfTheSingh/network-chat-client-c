#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>

#define MaxConnects 5
#define BuffSize 256
#define ConversationLen 5
#define Host "localhost"

int portNumber;

void *listenToServer(void *arg)
{
    int client_fd = *(int *)arg;
    char buffer[BuffSize];
    ssize_t bytes_read = read(client_fd, buffer, BuffSize);
    if (bytes_read < 0)
    {
        perror("Error on read");
        return NULL;
    }
    printf("%s\n", buffer);
}

int main(char *argv[])
{
    portNumber = argv[1];

    printf("Enter your username: ");
    char username[BuffSize];
    scanf("%s", username);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("socket");
        return 1;
    }

    struct hostent *hptr = gethostbyname(Host);
    if (!hptr)
    {
        perror("gethostbyname");
        return 1;
    }

    if (hptr->h_addrtype != AF_INET)
    {
        perror("bad address family");
        return 1;
    }

    struct sockaddr_in saddr;
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = ((struct in_addr *)hptr->h_addr)->s_addr;
    saddr.sin_port = htons(portNumber);

    if (connect(sockfd, (struct sockaddr *)&saddr, sizeof(saddr)) < 0)
    {
        perror("connect");
        return 1;
    }

    pthread_t listenThread;
    pthread_create(&listenThread, NULL, listenToServer, &sockfd);

    while (1)
    {
        char buffer[BuffSize];
        printf("%s: ", username);
        fgets(buffer, BuffSize, stdin);
        write(sockfd, buffer, strlen(buffer));
    }

    return 0;
}