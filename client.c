#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>

int create_client_socket(int port) {
	int c_sock;
   	struct sockaddr_in s_address;
   	s_address.sin_family = AF_INET;
    	s_address.sin_addr.s_addr = INADDR_ANY;
    	s_address.sin_port = htons(port);
        c_sock = socket(AF_INET, SOCK_STREAM, 0);
        connect(c_sock, (struct sockaddr *)&s_address, sizeof(s_address));
   	return c_sock;
}


int main() {
	int c_sock = create_client_socket(64209);
	printf("connected client");
	return 0;
}
