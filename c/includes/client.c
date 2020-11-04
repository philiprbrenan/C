#line 1 "/home/phil/c/z/z/tcp/client.c"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/tcp.h>

#define MAX 1100
#define PORT 65001

void func(int sockfd)
 {char buff[MAX]; memset(buff, 'A', sizeof(buff));
//buff[0] = 5; buff[10] = 0xd; buff[11] = 0xa; buff[12] = 0;
  buff[0] = 5; buff[10] = 0;
  int w = write(sockfd, buff, strlen(buff));
  printf("Wrote %d bytes", w);
  int n = read (sockfd, buff, sizeof(buff));
  printf("Received : %d %s", n, buff);
 }

int main()
 {struct sockaddr_in servaddr;

  int sockfd = socket(AF_INET, SOCK_STREAM, 0);                                     // Create socket
  if (sockfd == -1)
   {printf("socket creation failed...\n");
    exit(0);
    }
  else printf("Socket successfully created..\n");
  bzero(&servaddr, sizeof(servaddr));

  int noDelay = 1;
  setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (void *)&noDelay, sizeof(noDelay));

  servaddr.sin_family = AF_INET;                                                // Assign IP, PORT
  servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  servaddr.sin_port = htons(PORT);

  if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0)                   // Connect the client socket to server socket
   {printf("connection with the server failed %m\n");
    exit(0);
   }
  else printf("connected to the server..\n");

  func(sockfd);

  close(sockfd);
 }
