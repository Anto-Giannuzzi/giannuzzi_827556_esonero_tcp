/*
 * main.c
 *
 * TCP Server - Template for Computer Networks assignment
 *
 * This file contains the boilerplate code for a TCP server
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
#include <time.h>
#include "protocol.h"

#define NO_ERROR 0
weather_request_t request;
weather_response_t response;

static const char *SUPPORTED_CITIES[] = {
	"Bari",
	"Roma",
	"Milano",
	"Napoli",
	"Torino",
	"Palermo",
	"Genova",
	"Bologna",
	"Firenze",
	"Venezia"
};

void clearwinsock()
{
#if defined WIN32
	WSACleanup();
#endif
}

void valida_tipo(char tipo)
{
	if(tipo!='t' && tipo!='h' && tipo!='w' && tipo!='p')
	{
		response.status=2;
	}else
	{
		response.status=0;
	}
}

void valida_citta(char* citta)
{
	int flag=1;
	for (int i=0;i<10;i++)
	{
		if(strcmp(citta,SUPPORTED_CITIES[i])==0)
		{
			flag=0;
		}
	}
	if(flag==1)
	{
		response.status=1;
	}else
	{
		response.status=0;
	}
}

float get_temperature(void)
{
	srand(time(NULL));
	float min_value=-10.0;
	float max_value=40.0;
	float temp = rand() % (max_value - min_value + 1)+ min_value;
	return temp;
}
float get_humidity(void)
{
	srand(time(NULL));
	float min_value=20.0;
	float max_value=100.0;
	float hum = rand() % (max_value - min_value + 1)+ min_value;
	return hum;
}
float get_wind(void)
{
	srand(time(NULL));
	float min_value=0.0;
	float max_value=100.0;
	float wind = rand() % (max_value - min_value + 1)+ min_value;
	return wind;
}
float get_pressure(void)
{
	srand(time(NULL));
	float min_value=950.0;
	float max_value=1050.0;
	float pres = rand() % (max_value - min_value + 1)+ min_value;
	return pres;
}



int main(int argc, char *argv[])
{

	#if defined WIN32
		// Initialize Winsock
		WSADATA wsa_data;
		int result = WSAStartup(MAKEWORD(2,2), &wsa_data);
		if (result != NO_ERROR)
		{
			printf("Error at WSAStartup()\n");
			return 0;
		}
	#endif

	int my_socket;

	my_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (my_socket < 0)
	{
		errorhandler("socket creation failed.\n");
		clearwinsock();
		return -1;
	}

	struct sockaddr_in sad;
	memset(&sad, 0, sizeof(sad));
	sad.sin_family = AF_INET;
	sad.sin_addr.s_addr = inet_addr("127.0.0.1");
	sad.sin_port = htons(SERVER_PORT);

	if (bind(my_socket, (struct sockaddr*) &sad, sizeof(sad)) < 0)
	{
		errorhandler("bind() failed.\n");
		closesocket(my_socket);
		clearwinsock();
		return -1;
	}

	if (listen(my_socket, QUEUE_SIZE) < 0)
	{
		errorhandler("listen() failed.\n");
		closesocket(my_socket);
		clearwinsock();
		return -1;
	}

	struct sockaddr_in cad; // structure for the client address
	int client_socket;       // socket descriptor for the client
	int client_len;          // the size of the client address
	printf("Waiting for a client to connect...\n\n");
	while (1)
	{
		client_len = sizeof(cad); // set the size of the client address
		if ((client_socket = accept(my_socket, (struct sockaddr*) &cad,
				&client_len)) < 0)
		{
			errorhandler("accept() failed.\n");
			// close connection
			closesocket(client_socket);
			clearwinsock();
			return 0;
		}

		printf("Handling client %s\n", inet_ntoa(cad.sin_addr));

		char *s = "Connection established";
		if (send(client_socket, s, strlen(s), 0) != strlen(s))
		{
			errorhandler("send() sent a different number of bytes than expected");
			closesocket(client_socket);
			clearwinsock();
			return -1;
		}


		if ((recv(client_socket, &request, sizeof(weather_response_t), 0)) <= 0)
		{
			errorhandler("recv() failed or connection closed prematurely");
			closesocket(client_socket);
			clearwinsock();
			return -1;
		}

		printf("Richiesta '%c %s' dal client ip %s\n",request.type, request.city, sad.sin_addr.s_addr);

		valida_tipo(request.type);

		valida_citta(request.city);

		if(response.status==0)
		{
			switch (request.type)
			{
			case 't':
				response.value=get_temperature();
				break;

			case 'h':
				response.value=get_humidity();
				break;

			case 'w':
				response.value=get_wind();
				break;

			case 'p':
				response.value=get_pressure();
				break;
			}
			response.type=request.type;
		}
		else
		{
			response.type='\0';
			response.value=0.0;
		}






		if (send(client_socket, &response, sizeof(response), 0) != sizeof(response))
		{
			errorhandler("send() sent a different number of bytes than expected");
			closesocket(client_socket);
			clearwinsock();
			return -1;
		}

		printf("Server terminated.\n");

		closesocket(my_socket);
		clearwinsock();
		return 0;
	}
} // main end
