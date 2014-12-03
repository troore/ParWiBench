/* 
 * echoserveri.c - An iterative echo server 
 */ 
/* $begin echoserverimain */
#include "csapp.h"

#include <iostream>

#include <complex>

typedef float complex_type;

#define MAXBUFSZ 10
#define BYTES_PER_COMPLEX (2 * sizeof(complex_type))

int connfd;

int usrp_source(std::complex<float> *recv_items, int recv_num)
{
	int n, cplx_num;
	int buf_len = BYTES_PER_COMPLEX * recv_num;
	char *buf = (char *)malloc(buf_len);
	rio_t rio;
	
	int i;
	float real, imag;

	Rio_readinitb(&rio, connfd);

	n = Rio_readlineb(&rio, buf, buf_len);
	cplx_num = n / BYTES_PER_COMPLEX;

	printf("server received %d complex\n", cplx_num);

	for (i = 0; i < cplx_num; i++)
	{
		real = *((float *)&(buf[i * BYTES_PER_COMPLEX]));
		imag = *((float *)&(buf[i * BYTES_PER_COMPLEX + 4]));
		recv_items[i] = std::complex<float>(real, imag);

		std::cout << recv_items[i] << std::endl;
	}
		
	free(buf);

	return cplx_num;
}

/*
void echo(int connfd) 
{
    size_t n; 
    char buf[MAXLINE]; 
    rio_t rio;

    Rio_readinitb(&rio, connfd);
	
    while((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) { //line:netp:echo:eof
		printf("server received %d bytes\n", n);

		Rio_writen(connfd, buf, n);
		
    }
	
}
*/

int main(int argc, char **argv) 
{
    int listenfd/*, connfd*/, port, clientlen;
    struct sockaddr_in clientaddr;
    struct hostent *hp;
    char *haddrp;

	std::complex<float> recv_buf[MAXBUFSZ];
	int buf_len = MAXBUFSZ;

	int recv_len;
	
    if (argc != 2) {
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(0);
    }
    port = atoi(argv[1]);

    listenfd = Open_listenfd(port);
    while (1) {
		clientlen = sizeof(clientaddr);
		connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *)&clientlen);

		/* determine the domain name and IP address of the client */
		hp = Gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr, 
						   sizeof(clientaddr.sin_addr.s_addr), AF_INET);
		haddrp = inet_ntoa(clientaddr.sin_addr);
		printf("server connected to %s (%s)\n", hp->h_name, haddrp);

		//	echo(connfd);
		recv_len = usrp_source(recv_buf, buf_len);

		if (recv_len < buf_len)
		{
			printf("Data missed.\n");
		}
		
		Close(connfd);
    }
    exit(0);
}
/* $end echoserverimain */
