#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include "img.hpp"

int main() {
  unsigned int *ptr = *img;
  char buffer[50];
  char *pEnd;
  int n;
  unsigned long i = (unsigned long) ptr;

  n=sprintf(buffer, "%p", &img[0][0]);

  printf("%d\n", n);
  printf("%s\n", buffer);

  long int a;
  a = strtol(buffer, &pEnd, 16);
  sprintf(buffer, "%ld", a);
  n = strlen(buffer);
  printf("%d: %s\n",n,  buffer);

  printf("%ld\n", a);

  int fd = open("/dev/dm_cmd", O_RDWR);
  write(fd, buffer, n+1);

  close(fd);
  return 0;
}
