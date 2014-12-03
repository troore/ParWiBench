/*
 * echoclient.c - An echo client
 */
/* $begin echoclientmain */
#include "csapp.h"

#include <complex>

#define MAXBUFSZ 10

int clientfd;

void usrp_sink(std::complex<float> *send_items, int send_num)
{
	rio_t rio;
	int buf_len = 2 * sizeof(float) * send_num;
	char *buf = (char *)malloc(buf_len);
	float real, imag;
	int i;

	for (int i = 0; i < send_num; i++)
	{
		real = std::real(send_items[i]);
		imag = std::imag(send_items[i]);
		memcpy(&buf[i * 8], (char *)&(real), 4);
		memcpy(&buf[i * 8 + 4], (char *)&(imag), 4);
	}

	Rio_readinitb(&rio, clientfd);

	Rio_writen(clientfd, buf, buf_len);

	free(buf);
}

int main(int argc, char **argv) 
{
    int /*clientfd, */port;
    char *host/*, buf[MAXLINE]*/;
	//   rio_t rio;

	std::complex<float> send_buf[MAXBUFSZ];
	int buf_len = MAXBUFSZ;

	int i;

    if (argc != 3) {
		fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
		exit(0);
    }
	
    host = argv[1];
    port = atoi(argv[2]);

    clientfd = Open_clientfd(host, port);
	
	//	Rio_readinitb(&rio, clientfd);

	/*
    while (Fgets(buf, MAXLINE, stdin) != NULL) {
		Rio_writen(clientfd, buf, strlen(buf));
		Rio_readlineb(&rio, buf, MAXLINE);
		Fputs(buf, stdout);
    }
	*/

	for (i = 0; i < buf_len; i++)
	{
		send_buf[i] = std::complex<float>(1.0, -1.0);
	}
	usrp_sink(send_buf, buf_len);
	
    Close(clientfd); //line:netp:echoclient:close
	
    exit(0);
}
/* $end echoclientmain */
