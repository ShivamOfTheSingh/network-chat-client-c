#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

int create_server_sock(int port) {
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in s_addy;
	s_addy.sin_family = AF_INET;
	s_addy.sin_addr.s_addr = inet_addr("10.110.234.245");
	s_addy.sin_port = htons(port);
	bind(sock, (struct sockkaddr*)&s_addy, sizeof(s_addy));
	return sock;
}

int connect_2_client(int s_sock) {
     listen(s_sock, 5);
     int c_sock = accept(s_sock, NULL, NULL);
     return c_sock;
}

char* receive_msg(int sock) {
	char buffer[1000] = {0};
	int bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
	buffer[bytes_received] = '\0';
	char *ret = malloc((bytes_received + 1) * sizeof(char));
	strcpy(ret, buffer);
	return ret;
}

int main() {
	int s_sock = create_server_sock(64209);
	int c_sock = connect_2_client(s_sock);

	printf("%s\n", receive_msg(c_sock));

	close(c_sock);
	close(s_sock);
	return 0;

}
