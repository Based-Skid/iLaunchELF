#include <stdio.h>
#include <tamtypes.h>
#include <unistd.h>

//ps2ip.c
int dhcpmain();
char *getIpAddress(void);

//VTSPS2-CRC32.c
extern char *file_crc32(char device[], char path[], char fn[]);

//misc.c
void substring(char s[], char sub[], int p, int l);
ssize_t getline(char **lineptr, size_t *n, FILE *stream);

void init(void);
void deinit(int browser);

void DoTask(int task, int id);

int http_mirror;
