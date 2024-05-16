#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

int create_server_sock(int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in s_addy;
    s_addy.sin_family = AF_INET;
    s_addy.sin_addr.s_addr = INADDR_ANY;
    s_addy.sin_port = htons(port);
    bind(sock, (struct sockaddr *)&s_addy, sizeof(s_addy));
    return sock;
}

int connect_2_client(int s_sock) {
    listen(s_sock, 5);
    int c_sock = accept(s_sock, NULL, NULL);
    return c_sock;
}

char* receive_msg(int sock) {
    char *buffer = malloc(100 * sizeof(char));
    int bytes_received = recv(sock, buffer, 100, 0);
    buffer[bytes_received - 3] = '\0';
    return buffer;
}

void *client_stream(void *sock) {
    int c_sock = *(int *)sock;
    char *buffer = receive_msg(c_sock);
    printf("%s\n", buffer);
    free(buffer);
    close(c_sock);
}

int main() {
    int s_sock = create_server_sock(64209);
    close(s_sock);
    s_sock = create_server_sock(64209);

    int clients = 0;
    pthread_t threads[10];

    while (clients < 10) {
        printf("Waiting on client %d\n", clients);
        int t_sock = connect_2_client(s_sock);
        printf("Client %d connected\n", clients);
        pthread_create(&threads[clients], NULL, client_stream, (void *)&t_sock);
        ++clients;

    }
}
