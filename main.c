#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include "arm.h"

#define BUFFER_SIZE 1024
#define PORT "/dev/ttyUSB0"
//the thread function
void *connection_handler(void *);
void printArmErr(armError_t err);
int showInfo();
int


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

        //Now join the thread, so that we don't terminate before the thread
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
    char  client_message[BUFFER_SIZE];

    //receive a message from client
    while( (read_size = recv(sock, client_message, BUFFER_SIZE, 0))>0)
    {
        client_message[read_size] = '\0';
        printf(client_message);
        if(showInfo())
        {
            printf("error");
        }
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

int showInfo()
{
    arm_t myArm;
    armError_t err;
    armType_t armType = ARM_TYPE_NONE;
    uint8_t rev[16] = "";
    uint64_t sn = 0;
    uint16_t rfFreq = 0;
    uint8_t rfPower = 0;

    err = armInit(&myArm,PORT);
    if(err != ARM_ERR_NONE)
    {
        printf("ARM init failed\n");
        printArmErr(err);
        armDeInit(&myArm);
        return -1;
    }

    err = armInfo(&myArm,&armType,rev,&sn,&rfFreq,&rfPower);
    if (err != ARM_ERR_NONE)
    {
        printf("ARM get info failed\n");
        printArmErr(err);
        armDeInit(&myArm);
        return -1;
    }

    switch (armType)
    {
        case ARM_TYPE_N8_LW:
            printf("ARM_TYPE_N8_LW\n");
            break;

        default:
            printf("Not LW\n");
            break;
    }

    if(armType != ARM_TYPE_NONE)
    {
        printf("Revision name:%s\n", rev);
        printf("Serial number: 0x%x\n", (unsigned int)sn);
        printf("Radio frequency band : %u MHz\n", rfFreq);
        printf("Radio max power : %u dBm\n", rfPower);
    }

    err = armDeInit(&myArm);
    if(err != ARM_TYPE_NONE)
    {
        printf("ARM DeInit failed\n");
        printArmErr(err);
        return -1;
    }

    return 0;
}

void printArmErr(armError_t err)
{
    switch(err)
    {
        case ARM_ERR_NONE:
            printf("ARM_ERR_NONE: 'No error.'\n");
            break;

        case ARM_ERR_NO_SUPPORTED:
            printf("ARM_ERR_NO_SUPPORTED: 'Functionality no supported by theARM.'\n");
            break;

        case ARM_ERR_PORT_OPEN:
            printf("ARM_ERR_PORT_OPEN: 'Port Error, at the port opening.'\n");
            break;

        case ARM_ERR_PORT_CONFIG:
            printf("ARM_ERR_PORT_CONFIG: 'Port Error, at the port configuring.'\n");
            break;

        case ARM_ERR_PORT_READ:
            printf("ARM_ERR_PORT_READ: 'Port Error, at the port reading.'\n");
            break;

        case ARM_ERR_PORT_WRITE:
            printf("ARM_ERR_PORT_WRITE: 'Port Error, at the port writing.'\n");
            break;

        case ARM_ERR_PORT_WRITE_READ:
            printf("ARM_ERR_PORT_WRITE_READ: 'Port Error, at the port reading/writing.'\n");
            break;

        case ARM_ERR_PORT_CLOSE:
            printf("ARM_ERR_PORT_CLOSE: 'Port Error, at the port closing.'\n");
            break;

        case ARM_ERR_PARAM_OUT_OF_RANGE:
            printf("ARM_ERR_PARAM_OUT_OF_RANGE: 'Error, one or more of parameters is out of range.'\n");
            break;

        case ARM_ERR_PARAM_INCOMPATIBLE:
            printf("ARM_ERR_PARAM_INCOMPATIBLE: 'Error, the parameters is incompatible between them.'\n");
            break;

        case ARM_ERR_ADDRESSING_NOT_ENABLE:
            printf("ARM_ERR_ADDRESSING_NOT_ENABLE: 'Error, the addressing is not enable.'\n");
            break;

        case ARM_ERR_WOR_ENABLE:
            printf("ARM_ERR_WOR_ENABLE: 'Error, the WOR mode is enable.'\n");
            break;

        case ARM_ERR_ARM_GO_AT:
            printf("ARM_ERR_ARM_GO_AT: 'ARM command Error, can't switch to AT command.'\n");
            break;

        case ARM_ERR_ARM_BACK_AT:
            printf("ARM_ERR_ARM_BACK_AT: 'ARM command Error, can't quit AT command.'\n");
            break;

        case ARM_ERR_ARM_CMD:
            printf("ARM_ERR_ARM_CMD: 'ARM command Error, from AT command.'\n");
            break;

        case ARM_ERR_ARM_GET_REG:
            printf("ARM_ERR_ARM_GET_REG: 'ARM command Error, from get register.'\n");
            break;

        case ARM_ERR_ARM_SET_REG:
            printf("ARM_ERR_ARM_SET_REG: 'ARM command Error, from set register.'\n");
            break;

        default:
            printf("ARM_ERR_UNKNOWN: 'Error unknown'\n");
            break;
    }
}