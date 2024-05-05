#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>

#define MaxConnects 5
#define BuffSize 256
#define ConversationLen 5
#define Host "localhost"

FILE *chat_history;

struct Node
{
    int fd;
    struct Node *next;
};

struct args_for_thread
{
    int fd_Of_ChatHistory;
    int client_fd;
    struct flock lock;
    struct Node *head;
} args_for_thread;

void report(const char *msg, int terminate)
{
    perror(msg);
    if (terminate)
        exit(EXIT_FAILURE);
}

void *handleCommunicationWithClient(void *args)
{
    struct args_for_thread *argsFor = args;
    char buffer[BuffSize];              // Define a buffer of size BuffSize
    int client_fd = argsFor->client_fd; // Cast the argument to an integer and assign it to client_fd
    int fd_chat_history = argsFor->fd_Of_ChatHistory;
    struct flock chat_history_lock = argsFor->lock;

    while (1)
    {
        ssize_t bytes_read = read(client_fd, buffer, BuffSize); // Read up to BuffSize bytes from the client into the buffer
        if (bytes_read < 0)
        {
            perror("Error on write");
            continue;
        }

        struct Node *temp = argsFor->head;
        while (temp->next != NULL)
        {
            if (temp->fd != client_fd)
            {

                ssize_t bytes_written = write(temp->fd, buffer, bytes_read); // Write the bytes read back to the client
                if (bytes_written < 0 || bytes_written != bytes_read)
                {
                    fprintf(stderr, "Warning: Not all bytes written to client\n");
                    continue;
                }
            }

            temp = temp->next;
        }

        if (fcntl(fd_chat_history, F_SETLKW, &argsFor->lock) < 0)
        {
            perror("fcntl");
            exit(1);
        }
        else
        {
            fprintf(chat_history, "%s\n", buffer);
        }
        chat_history_lock.l_type = F_UNLCK;
        if (fcntl(fd_chat_history, F_SETLK, &chat_history_lock) < 0)
        {
            perror("fcntl");
            exit(1);
        }
    }
}

int main()
{
    int portNo;
    printf("Enter port number: ");
    scanf("%d", &portNo);

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
        report("socket", 1);

    struct sockaddr_in saddr;
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    saddr.sin_port = htons(portNo);

    if (bind(fd, (struct sockaddr *)&saddr, sizeof(saddr)) < 0)
        report("bind", 1);

    if (listen(fd, MaxConnects) < 0)
        report("listen", 1);

    fprintf(stderr, "Listening on port %i\n", portNo);

    chat_history = fopen("chat_history", "w");

    struct flock lockThis;
    lockThis.l_type = F_WRLCK;
    lockThis.l_whence = SEEK_SET;
    lockThis.l_start = 0;
    lockThis.l_len = 0;
    lockThis.l_pid = getpid();

    int fd_chat_history;
    if ((fd_chat_history = open("chat_history", O_RDWR, 0666)) < 0)
    {
        perror("open");
        exit(1);
    }
    struct Node *head = (struct Node *)malloc(sizeof(struct Node));
    int numClients = 0;

    while (1)
    {
        struct sockaddr_in caddr;
        int len = sizeof(caddr);

        int client_fd = accept(fd, (struct sockaddr *)&caddr, (socklen_t *)&len);
        if (client_fd < 0)
        {
            report("accept", 0);
            continue;
        }
        if (numClients == 0)
        {
            struct Node *newNode = (struct Node *)malloc(sizeof(struct Node));
            newNode->fd = client_fd;
            head->next = newNode;
            numClients++;
        }
        else
        {
            struct Node *temp = head;
            while (temp->next != NULL)
            {
                temp = temp->next;
            }
            struct Node *newNode = (struct Node *)malloc(sizeof(struct Node));
            newNode->fd = client_fd;
            temp->next = newNode;
        }

        pthread_t thread;
        struct args_for_thread *args = malloc(sizeof(struct args_for_thread));
        args->head = head;
        args->client_fd = client_fd;
        args->fd_Of_ChatHistory = fd;
        args->lock = lockThis;

        pthread_create(&thread, NULL, handleCommunicationWithClient, (void *)args);
    }

    return 0;
}