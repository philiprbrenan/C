#line 1 "/home/phil/c/z/z/tcp/server.c"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main(void)
 {int server, client;
  char buffer[1024];
  struct sockaddr_in serverAddr;
  struct sockaddr_storage serverStorage;
  socklen_t addr_size;

  server = socket(PF_INET, SOCK_STREAM, 0);

  if (1)
   {int optval = 1;
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
   }

  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(65001);
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

  bind(server, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

  if(listen(server, 5)==0)
    printf("Listening\n");
  else
    printf("Error %m\n");

  for(size_t i = 0; i < 99; ++i)
   {addr_size = sizeof serverStorage;
    client = accept(server, (struct sockaddr *) &serverStorage, &addr_size);
    memset(buffer, 0, sizeof(buffer));
    int n = read(client, buffer, sizeof(buffer));
    fprintf(stderr, "Received: %d %s\n", n, buffer);

    strcpy(buffer,"Hello World\n");
    int o = send(client,buffer,strlen(buffer),0);
    fprintf(stderr, "Sent: %d %s\n", o, buffer);
    close(client);
   }
  return 0;
}
