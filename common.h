#pragma once // incluindo tudo apenas uma vez

#include <stdlib.h>
#include <arpa/inet.h>

void exitLog(const char *msgError);

int addrParse(const char *addrstr, const char *portstr, struct sockaddr_storage *storage);

void addrToStr(const struct sockaddr *addr, char *str, size_t strsize);

int serverInit(const char *protocol, const char* portStr, struct sockaddr_storage *storage);

int fileExists(const char *filename);

int hasValidExtension(const char* filename);

char* readFileToString(char* filename);

const char* findExtension(const char* string);

void writeStringToFile(const char* string, const char* fileName);