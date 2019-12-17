#ifndef __SNMAPI_FTP_H__
#define __SNMAPI_FTP_H__

void absPath(char* absPath_s, const char* path, const char* cwd);
int isDir(const char* path);
int ssplit(const char* str, char* left, int lmaxlen, char* right, int rmaxlen);
int slisten(int port, int backlog);
void sclose(int *socket_e);
int my_atoi(const char *c);
int connect_to_server(char *server, uint16_t port);
void handleclient(uint64_t conn_s_p);

#endif //__SNMAPI_FTP_H__