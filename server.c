#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <string.h>

#define MAX_USR 10
#define MSG_SIZE 100

pthread_mutex_t t_mutex;
int client_socks[MAX_USR];
int users = 0;

int create_server_sock(int port)
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in s_addy;
    s_addy.sin_family = AF_INET;
    s_addy.sin_addr.s_addr = INADDR_ANY;
    s_addy.sin_port = htons(port);
    bind(sock, (struct sockaddr *)&s_addy, sizeof(s_addy));
    return sock;
}

int connect_2_client(int s_sock)
{
    listen(s_sock, 5);
    int c_sock = accept(s_sock, NULL, NULL);
    return c_sock;
}

char *receive_msg(int sock)
{
    char *buffer = malloc(MSG_SIZE * sizeof(char));
    int bytes_received = recv(sock, buffer, MSG_SIZE, 0);
    buffer[bytes_received] = '\0';
    return buffer;
}

void send_2_all(char* msg) {
    for (int i = 0; i < users; ++i) {
        send(client_socks[i], msg, strlen(msg), 0);
    }
} 

void log_line(char* msg)
{
    pthread_mutex_lock(&t_mutex);
    FILE *logs = fopen("logs.txt", "a");
    fprintf(logs, "%s", msg);
    send_2_all(msg);
    fclose(logs);
    pthread_mutex_unlock(&t_mutex);
}

void *client_stream(void *sock)
{
    int c_sock = *(int *)sock;
    char *buffer;
    char *username = receive_msg(c_sock);

    char log_msg[MSG_SIZE];
    snprintf(log_msg, MSG_SIZE, "%s has joined the chat.\n", username);
    log_line(log_msg);
    printf("%s\n", log_msg);

    while (1)
    {
        buffer = receive_msg(c_sock);
        if (*buffer == 27)
        {
            snprintf(log_msg, MSG_SIZE, "%s has left.\n", username);
            log_line(log_msg);
            printf("%s\n", log_msg);
            break;
        }
        snprintf(log_msg, MSG_SIZE, "%s : %s\n", username, buffer);
        log_line(log_msg);
        printf("%s\n", log_msg);
        free(buffer);
    }
    free(username);
    close(c_sock);
}

int main()
{
    pthread_mutex_init(&t_mutex, NULL);

    int s_sock = create_server_sock(64207);
    close(s_sock);
    s_sock = create_server_sock(64207);
    printf("Server has started! Waiting for clients\n");
    pthread_t threads[MAX_USR];

    while (users < MAX_USR)
    {
        int t_sock = connect_2_client(s_sock);
        client_socks[users] = t_sock;
        pthread_create(&threads[users], NULL, client_stream, (void *)&t_sock);
        ++users;
    }

    pthread_mutex_destroy(&t_mutex);

    return 0;
}
