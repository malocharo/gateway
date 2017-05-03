#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#define BUFFER_SIZE 1024
//the thread function
void *connection_handler(void *);

int main() {
    int socket_desc, client_sock, c;
    struct sockaddr_in server, client;

    //create socket
    if ((socket_desc = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        printf("Cannot create socket\n");

    printf("Socket created\n");

    //prepare the sockaddr_in struct
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8888);

    //bind
    if (bind(socket_desc, (struct sockaddr *) &server, sizeof(server)) < 0) {
        perror("bind failed");
        return -1;
    }
    printf("Bind done\n");

    listen(socket_desc, 5);
    printf("Waiting for incoming connection\n");
    c = sizeof(struct sockaddr_in);
    pthread_t thread_id;

    while ((client_sock = accept(socket_desc, (struct sockaddr *) &client, (socklen_t *) &c))) {
        printf("connection accepted\n");
        if (pthread_create(&thread_id, NULL, connection_handler, (void *) &client_sock)) {
            perror("cannot create thread\n");
            //return -1;
        }
        
        //Now join the thread, so that we dont terminate before the thread
        printf("Handler assigned\n");
    }

    if (client_sock < 0) {
        perror("accept failed\n");
        return -1;
    }
    return 0;
}

void *connection_handler(void *socket_desc)
{
// get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size;
    char *message, client_message[BUFFER_SIZE];

    //receive a message from client
    while( (read_size = recv(sock, client_message, BUFFER_SIZE, 0))>0)
    {

        client_message[read_size] = '\0';
        printf(client_message);
        memset(client_message,0,BUFFER_SIZE);

    }

    if(read_size == 0)
    {
        printf("Client disco");
        fflush(stdout);
        close(sock);
    }

    else if(read_size == -1)
    {
        perror("recv failed");
    }

    return 0;

}
