#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

# define MSG_SIZE 100

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
    char *buffer = malloc(MSG_SIZE * sizeof(char));
    int bytes_received = recv(sock, buffer, MSG_SIZE, 0);
    buffer[bytes_received] = '\0';
    return buffer;
}

void *client_stream(void *sock) {
    int c_sock = *(int *)sock;
    char *buffer;
    char *username = receive_msg(c_sock);
    printf("%s has joined the chat.\n", username);

    while (1) {
        buffer = receive_msg(c_sock);
	if (*buffer == 27) {
	  printf("%s has left.\n", username);
	  break;
	}
        printf("%s: %s\n", username, buffer);
        free(buffer);
    }
    free(username);
    close(c_sock);
}

int main() {
    int s_sock = create_server_sock(64209);
    close(s_sock);
    s_sock = create_server_sock(64209);
    printf("Server has started! Waiting for clients\n");
    int clients = 0;
    pthread_t threads[10];

    while (clients < 10) {
        int t_sock = connect_2_client(s_sock);
        pthread_create(&threads[clients], NULL, client_stream, (void *)&t_sock);
        ++clients;
    }
}

