/*
 * main.c
 *
 * TCP Client - Template for Computer Networks assignment
 *
 * This file contains the boilerplate code for a TCP client
 * portable across Windows, Linux and macOS.
 */

#if defined WIN32
#include <winsock.h>
#else
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#define closesocket close
#endif

#include <stdio.h>
#include <stdlib.h>
#include "protocol.h"

#define NO_ERROR 0
weather_request_t request;
weather_response_t response;

void clearwinsock() {
#if defined WIN32
	WSACleanup();
#endif
}

void parse_string(char str[])
{
	const char s[2] = " ";
	char* tok;
	int i=0;
	char* array[3];

	tok = strtok(str, s);

	while (tok != 0) {
		array[i]=tok;
		i++;
		tok = strtok(0, s);
	}

	strcpy(request.type,array[1]);
	strcpy(request.type,array[2]);
}


int main(int argc, char *argv[])
{

	// TODO: Implement client logic

#if defined WIN32
	// Initialize Winsock
	WSADATA wsa_data;
	int result = WSAStartup(MAKEWORD(2,2), &wsa_data);
	if (result != NO_ERROR) {
		printf("Error at WSAStartup()\n");
		return 0;
	}
#endif

	int my_socket;

	int c_socket;
	c_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (c_socket < 0)
	{
		errorhandler("socket creation failed.\n");
		closesocket(c_socket);
		clearwinsock();
		return -1;
	}

	struct sockaddr_in sad;
	memset(&sad, 0, sizeof(sad));
	sad.sin_family = AF_INET;
	sad.sin_addr.s_addr = inet_addr("127.0.0.1");
	sad.sin_port = htons(SERVER_PORT);

	if (connect(c_socket, (struct sockaddr *)&sad, sizeof(sad))< 0)
	{
		errorhandler( "Failed to connect.\n" );
		closesocket(c_socket);
		clearwinsock();
		return -1;
	}

	char buf[BUFFER_SIZE]; // buffer for data from the server
	memset(buf, '\0', BUFFER_SIZE); // ensures extra bytes contain 0
	if ((recv(c_socket, buf, BUFFER_SIZE - 1, 0)) <= 0)
	{
		errorhandler("recv() failed or connection closed prematurely");
		closesocket(c_socket);
		clearwinsock();
		return -1;
	}
	printf("%s", buf); // Print the echo buffer

	char string[BUFFER_SIZE];
	memset(string,'\0',BUFFER_SIZE);
	printf("Enter the string: ");
	scanf("%s",string);
	parse_string(string);

	if(send(c_socket,&request,sizeof(request),0)!=sizeof(request))
	{
		errorhandler("send() sent a different number of bytes than expected");
		closesocket(c_socket);
		clearwinsock();
		return -1;
	}


	if ((recv(c_socket, &response, sizeof(response), 0)) <= 0) {
		errorhandler("recv() failed or connection closed prematurely");
		closesocket(c_socket);
		clearwinsock();
		return -1;
	}

	printf("Ricevuto risultato dal server ip %s. %s: %c %f",sad.sin_addr.s_addr,request.city, response.type, response.value );

	closesocket(c_socket);

	printf("Client terminated.\n");

	clearwinsock();
	return 0;
} // main end
