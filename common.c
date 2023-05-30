// Este arquivo contem funcoes utilizadas em comum por client.c e server.c

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>


void exitLog(const char *msgError){
    // essa funcao exibe mensagens de erro e termina o programa com um codigo de erro
    perror(msgError);
    exit(EXIT_FAILURE);
}


int addrParse(const char *addrstr, const char *portstr, struct sockaddr_storage *storage){
    if (addrstr == NULL || portstr == NULL){
        return -1;
    }

    unsigned short port = (unsigned short)atoi(portstr);
    if (port == 0){
        return -1;
    }
    port = htons(port); // host to network short (dispositivo -> rede)

    // ipV4
    struct in_addr inAddrv4; // endereco de IPv4 

    if (inet_pton(AF_INET, addrstr, &inAddrv4)){
        struct sockaddr_in *addrv4 = (struct sockaddr_in *)(storage); // aponta para o storage, que foi o parametro recebido
        addrv4->sin_family = AF_INET;
        addrv4->sin_port = port;
        addrv4->sin_addr = inAddrv4;

        return 0;
    }

    // ipV6
    struct in6_addr inAddrv6; // endereco IPv6

    if (inet_pton(AF_INET6, addrstr, &inAddrv6)){
        struct sockaddr_in6 *addrv6 = (struct sockaddr_in6 *)(storage); // aponta para o storage, que foi o parametro recebido
        addrv6->sin6_family = AF_INET6;
        addrv6->sin6_port = port;
        memcpy(&(addrv6->sin6_addr), &inAddrv6, sizeof(inAddrv6)); // copiando o dado do addr6 para outro vetor

        return 0;
    }

    return -1; // erro, nao conseguiu instanciar nem um IPv4 nem um IPv6
}


void addrToStr(const struct sockaddr *addr, char *str, size_t strsize){
    int ipVersion;
    unsigned short port;
    char addrStr[INET6_ADDRSTRLEN+1] = ""; // string do tamanho textual necessario para armazenar um endereco IPv6


    if(addr->sa_family == AF_INET){ // IPv4
        ipVersion = 4;
        struct sockaddr_in *addrv4 = (struct sockaddr_in *)(addr);

        if(!inet_ntop(AF_INET, &(addrv4->sin_addr), addrStr, INET6_ADDRSTRLEN+1)){
            exitLog("Problema com a conversao ntop!");
        }

        port = ntohs(addrv4->sin_port); // network to host short (rede -> dispositivo)

    } else if (addr->sa_family == AF_INET6){ // IPv6
        ipVersion = 6;
        struct sockaddr_in6 *addrv6 = (struct sockaddr_in6 *)(addr);

        if(!inet_ntop(AF_INET6, &(addrv6->sin6_addr), addrStr, INET6_ADDRSTRLEN+1)){
            exitLog("Problema com a conversao ntop!");
        }

        port = ntohs(addrv6->sin6_port); // network to host short (rede -> dispositivo)

    } else{
        exitLog("Protocolo desconhecido!");
    }

    if(str){
        snprintf(str, strsize, "IPv%d %s %hu", ipVersion, addrStr, port); // mostrando algumas infos
    }

}


int serverInit(const char *protocol, const char* portStr, struct sockaddr_storage *storage){
    unsigned short port = (unsigned short)atoi(portStr);
    if (port == 0){
        return -1;
    }
    port = htons(port); // host to network short (dispositivo -> rede)

    memset(storage, 0, sizeof(*storage)); // limpando a memoria para evitar erros

    if (strcmp(protocol, "v4") == 0){ // IPv4
        struct sockaddr_in *addrv4 = (struct sockaddr_in *)(storage);
        // inicializando os campos
        addrv4->sin_family = AF_INET;
        addrv4->sin_port = port;
        addrv4->sin_addr.s_addr = INADDR_ANY; // dizendo para o servidor rodar em todos os enderecos

        return 0;

    } else if (strcmp(protocol, "v6") == 0){ // IPv6
        struct sockaddr_in6 *addrv6 = (struct sockaddr_in6 *)(storage);

        // inicializando os campos
        addrv6->sin6_family = AF_INET6;
        addrv6->sin6_port = port;
        addrv6->sin6_addr = in6addr_any; // dizendo para o servidor rodar em todos os enderecos

        return 0;

    } else {
        return -1; // erro!
    }
}

int fileExists(const char *fileName) {
    FILE *file = fopen(fileName, "r");
    if (file != NULL) {
        fclose(file);
        return 1;  // arquivo existe
    }
    return 0;  // arquivo nao existe
}

int hasValidExtension(const char* fileName) {
    const char* validExtensions[] = {".txt", ".c", ".cpp", ".py", ".tex", ".java"};
    const int numExtensions = sizeof(validExtensions) / sizeof(validExtensions[0]);

    // pega a extensao
    const char* fileExtension = strrchr(fileName, '.');
    if (fileExtension == NULL) {
        return 0; // nao tem extensao
    }
    
    // compara a extensao do arquivo com a lista de extensoes validas
    for (int i = 0; i < numExtensions; i++) {
        if (strcmp(fileExtension, validExtensions[i]) == 0) {
            return 1; // tem uma extensao valida
        }
    }

    return 0; // nao tem uma extensao valida
}


char* readFileToString(char* fileName) {
    FILE* file = fopen(fileName, "r");
    if (file == NULL) {
        fprintf(stderr, "Failed to open file: %s\n", fileName);
        return NULL;
    }

    // determinando o tamanho do arquivo
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);

    // alocando memoria para a string que sera enviada
    // aqui temos que adicionar tambem espaco para o nome do arquivo
    char* content = (char*)malloc((fileSize + strlen(fileName) + 6) * sizeof(char));
    if (content == NULL) {
        fprintf(stderr, "Failed to allocate memory for string\n");
        fclose(file);
        return NULL;
    }

    char fileNameAux[1000];
    strcpy(fileNameAux, fileName); 
    strcat(fileNameAux, "0"); // adicionando 0 para usar como delimitador 
    // adicionando o nome do arquivo ao inicio da string
    sprintf(content, "%s\n", fileNameAux);

    // adicionando o conteudo do arquivo a string
    size_t bytesRead = fread(content + strlen(fileNameAux) + 1, sizeof(char), fileSize, file);
    if (bytesRead != fileSize) {
        fprintf(stderr, "Error reading file: %s\n", fileName);
        free(content);
        fclose(file);
        return NULL;
    }

    // adicionando o \end ao fim da string
    strcat(content, "\\end");

    fclose(file);
    return content;
}

void writeStringToFile(const char* string, const char* fileName) {
    const char* folderName = "server_files";
    char filePath[1000] = "";
    int overwritten = 0; // flag para verificar se o arquivo sera sobrescrito

    // criando o path
    strcat(filePath, folderName);
    strcat(filePath, "/");
    strcat(filePath, fileName);

    if (access(filePath, F_OK) == 0){
    // arquivo existe
        overwritten = 1;
    }

    // abre o arquivo para escrita
    FILE* file = fopen(filePath, "w");

    // escreve a string no arquivo (excluindo os ultimos 4 caracteres-> \end)
    size_t length = strlen(string);
    if (length > 4) {
        length -= 4;
        fwrite(string, sizeof(char), length, file);
    }

    // temos sempre que fechar o arquivo
    fclose(file);

    if (overwritten){
        printf("file %s overwritten\n", fileName);
    } else{
        printf("file %s received\n", fileName);
    }
}
