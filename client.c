/*
Trabalho 1 - Redes de Computadores
Vitor de Oliveira Mafra - 2018046831
*/

#include "common.h"

// Bibliotecas padrao
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// Bibliotecas relacionadas a sockets
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int main(int argc, char **argv){

    struct sockaddr_storage storage;
    if(addrParse(argv[1], argv[2], &storage) != 0){ // fazendo o parse dos argumentos recebidos
        printf("Wrong arguments\n");
    }

    // inicializando um socket IPv4 ou IPv6
    int s;
    s = socket(storage.ss_family, SOCK_STREAM, 0);

    if(s == -1){
        exitLog("Socket error!");
    }

    // connect
    struct sockaddr *addr = (struct sockaddr *)(&storage);
    if (connect(s, addr, sizeof(storage)) != 0){ // verificando erros no connect
        exitLog("connect");
    }

    // imprimindo mensagem de inicializacao
    char addrStr[BUFFER_SIZE];
    addrToStr(addr, addrStr, BUFFER_SIZE);
    printf("Connented to server %s\n", addrStr);

    char fileName[BUFFER_SIZE]; // buffer para armazenar o nome do arquivo selecionado
    int fileWasSelected = 0;

    while(1){
        // aqui comeca o menu cli para o envio dos arquivos
        char input[BUFFER_SIZE];
        memset(input, 0, BUFFER_SIZE); // limpando a memoria para evitar lixo
        printf("> ");
        fgets(input, BUFFER_SIZE-1, stdin);
        input[strcspn(input, "\n")] = 0; // removendo o \n do final da string para evitar problemas
        
        char *splitedInput;
        splitedInput = strtok(input, " "); // similar ao .split() do Python

        int param = 0; // contador para iterarmos sobre os parametros passados

        while(splitedInput){
            if (param == 0){ // procurando pelas opcoes "select", "sendOptionSelected" e "exitOptionSelected"
                // select
                if (strcmp("select", splitedInput) == 0){
                    // "select" corretamente detectado
                    fileWasSelected = 1;
                }
                // send
                else if (strcmp("send", splitedInput) == 0){
                    // "send" corretamente detectado
                    if (fileWasSelected){
                        // envia o arquivo!
                        char* buf = readFileToString(fileName);

                        int count = send(s, buf, strlen(buf)+1, 0); // numero de bytes transmitidos na rede
                        if (count != strlen(buf)+1){ // verificando se a mensagem foi transmitida corretamente
                            exitLog("send");
                        }
                        
                    } else{
                        printf("no file selected!\n");
                    }
                }
                // exit
                else if (strcmp("exit", splitedInput) == 0){
                    // "exit" corretamente detectado
                    exit(EXIT_SUCCESS);
                    close(s);
                    break;

                } else{
                    exit(EXIT_FAILURE);
                    close(s);
                    break;
                }

            } else if (param == 1){ // verificando a existencia do segundo termo
                if (strcmp("file", splitedInput) != 0){
                    // "file" foi escrito incorretamente
                    exit(EXIT_FAILURE);
                    close(s);
                    break;
                }

            } else if (param == 2){ // verificando os arquivos
                // verifica a extensao dos arquivos
                if (! hasValidExtension(splitedInput)){
                    // extensao invalida
                    printf("%s not valid!\n", splitedInput);
                    fileWasSelected = 0;
                }
                // verifica a existencia do arquivo
                else if (fileExists(splitedInput) == 0){
                    printf("%s does not exist\n", splitedInput);
                    fileWasSelected = 0;
                }
                
                else if (strcmp(splitedInput, "") == 0){
                    printf("no file selected!\n");
                }

                else{ // tudo certo
                    if(fileWasSelected){
                        printf("%s selected!\n", splitedInput);
                        // armazena o nome do arquivo que sera enviado
                        strcpy(fileName, splitedInput);
                    }
                }

            } else{
                // alguma coisa deu errado, fecha a conexao
                close(s);
            }

            splitedInput = strtok(NULL, " ");
            param += 1;
        }
    }

    char buf[BUFFER_SIZE];

    int count = send(s, buf, strlen(buf)+1, 0); // numero de bytes transmitidos na rede
    if (count != strlen(buf)+1){ // verificando se a mensagem foi transmitida corretamente
        exitLog("send");
    }

    memset(buf, 0, BUFFER_SIZE); // limpando a memoria para evitar lixo
    unsigned totalBytes = 0; // total de bytes recebidos ate o momento

    while(1){
        count = recv(s, buf + totalBytes, BUFFER_SIZE - totalBytes, 0); // recebendo a resposta do servidor
        if (count == 0){ // nao recebeu nenhum dado
            break; // entao terminamos a conexao
        }
        totalBytes += count;
    }
    close(s);

    printf("Bytes receiveds: %u bytes\n", totalBytes);
    puts(buf);

    exit(EXIT_SUCCESS);
}