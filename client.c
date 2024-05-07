#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

int create_client_socket(int port) {
	int c_sock;
   	struct sockaddr_in s_address;
   	s_address.sin_family = AF_INET;
	s_address.sin_port = htons(port);
	inet_pton(AF_INET, "10.110.234.245", &s_address.sin_addr);

	while (1) {
		c_sock = socket(AF_INET, SOCK_STREAM, 0);
		if (connect(c_sock, (struct sockaddr *)&s_address, sizeof(s_address)) < 0) {
			printf("Waiting on server, retrying in 5 seconds\n");
			close(c_sock);
			sleep(5);
		}
		else {
			printf("Connected to client!\n");
			return c_sock;
		}
	}
}


void send_msg(int sock) {
	char buffer[100];
	printf("Enter your message: ");
	fgets(buffer, sizeof(buffer), stdin);
	send(sock, buffer, sizeof(buffer), 0);
	printf("Message sent\n");
}

int main() {
	int c_sock = create_client_socket(64209);
	send_msg(c_sock);
	close(c_sock);
	return 0;
}
