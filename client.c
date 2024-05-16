#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MSG_SIZE 100
#define USRNM_SIZE 20

int c_sock;

void handle_exit(int sig) {
    char exit_msg = 27;
    send(c_sock, &exit_msg, sizeof(char), 0);
    close(c_sock);
    printf("\nDisconnected from server\n");
    exit(0);
}

int create_client_socket(int port) {
    struct sockaddr_in s_address;
    s_address.sin_family = AF_INET;
    s_address.sin_addr.s_addr = INADDR_ANY;
    s_address.sin_port = htons(port);

    while (1) {
        c_sock = socket(AF_INET, SOCK_STREAM, 0);
        if (connect(c_sock, (struct sockaddr*)&s_address, sizeof(s_address)) < 0) {
            printf("Waiting on server, retrying in 5 seconds\n");
            close(c_sock);
            sleep(5);
        } else {
            printf("Connected to server!\n");
            return c_sock;
        }
    }
}

int send_msg(int sock) {
    char *buffer = malloc(MSG_SIZE * sizeof(char));
    printf("Enter your message: ");
    fgets(buffer, MSG_SIZE, stdin);
    buffer[strlen(buffer) - 1] = '\0';
    send(sock, buffer, strlen(buffer), 0);
    free(buffer);
    return 0;
}

int main() {
    signal(SIGINT, handle_exit);
    c_sock = create_client_socket(64209);

    char *username = malloc(USRNM_SIZE * sizeof(char));
    printf("Enter your username: ");
    fgets(username, USRNM_SIZE, stdin);
    username[strlen(username) - 1] = '\0';
    send(c_sock, username, strlen(username), 0);
    free(username);

    while (1) {
        send_msg(c_sock);
    }

    return 0;
}
