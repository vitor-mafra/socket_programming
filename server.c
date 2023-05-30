#include "common.h"

// Bibliotecas padrao
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// Bibliotecas relacionadas a sockets
#include <sys/types.h>
#include <sys/socket.h>

#define BUFFER_SIZE 1024
#define MAX_CONNECTIONS_WAITLIST 50

int main(int argc, char **argv){

    struct sockaddr_storage storage;
    if(serverInit(argv[1], argv[2], &storage) != 0){
        printf("Wrong arguments\n");
    }

    // inicializando um socket IPv4 ou IPv6
    int s;
    s = socket(storage.ss_family, SOCK_STREAM, 0);

    if(s == -1){
        exitLog("Socket! error");
    }

    // fazendo com que o servidor possa reutilizar uma porta quando ela estiver livre
    int enable = 1;
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) != 0){
        exitLog("setsockopt() error");
    }

    struct sockaddr *addr = (struct sockaddr *)(&storage);

    // bind
    if(bind(s, addr, sizeof(storage)) != 0){ // verificando erros no bind
        exitLog("Bind error!");
    }

    // listen
    if(listen(s, MAX_CONNECTIONS_WAITLIST) != 0){ // verificando erros no listen
        exitLog("Listen error!");
    }

    // imprimindo mensagem de inicializacao
    char addrStr[BUFFER_SIZE];
    addrToStr(addr, addrStr, BUFFER_SIZE);
    printf("%s\n", addrStr);
    printf("Waiting for connections...\n");


    //accept
    while(1){
        struct sockaddr_storage clientStorage; // armazenando o endereco do cliente 
        struct sockaddr * clientAddr = (struct sockaddr *)(&clientStorage);
        socklen_t clientAddrLen = sizeof(clientStorage);

        // inicializando o socket que usaremos para falar com o cliente
        int clientSocket = accept(s, clientAddr, &clientAddrLen); 

        if (clientSocket == -1){
            exitLog("Socket Error");
        }

        // imprimindo o endereco do cliente
        char clientAddrStr[BUFFER_SIZE];
        addrToStr(clientAddr, clientAddrStr, BUFFER_SIZE);
        printf("[log] Conected to %s\n", clientAddrStr);

        // recebendo os dados do cliente
        char buf[BUFFER_SIZE];
        memset(buf, 0, BUFFER_SIZE); // limpando a memoria para evitar lixo
        size_t count = recv(clientSocket, buf, BUFFER_SIZE, 0);// numero de bytes recebidos

        char *fileName = strtok(buf, "0"); // usamos 0 para separar o nome do arquivo com o conteudo da msg
        char *msg = strtok(NULL, ""); // todo o resto eh a mensagem

        writeStringToFile(msg, fileName);
        
        sprintf(buf, "remote endpoint: %.1000s\n", clientAddrStr);

        // conferindo a quantidade de dados enviados
        count = send(clientSocket, buf, strlen(buf)+1, 0); // numero de bytes transmitidos na rede
        if (count != strlen(buf)+1){ // verificando se a mensagem foi transmitida corretamente
            exitLog("send error");
        }

        close(clientSocket); // fechando a conexao
        printf("connection closed");
    }

    exit(EXIT_SUCCESS);
}