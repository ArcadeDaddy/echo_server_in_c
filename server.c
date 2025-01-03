#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <process.h>

#pragma comment(lib, "ws2_32.lib")  // Link against the Winsock library

#define DEFAULT_PORT "27015"  // Default port for server to listen on

#define BUFFER_LENGTH_DEFAULT 8192

typedef struct {
    char VERB[10];
    char PATH[1024];
    char VERSION[10];
} RequestMethodLine;

typedef struct {
    char VERSION[10];
    int STATUS_CODE;
    char STATUS_MESSAGE[64];
} ResponseMethodLine;

typedef struct {
    RequestMethodLine methodLine;
    char headers[100][2][1024];
    int headerCount;
    char body[4096];
} HttpRequest;

typedef struct {
    ResponseMethodLine methodLine;
    char headers[100][2][1024];
    int headerCount;
    char body[4096];
} HttpResponse;

HttpRequest parsingRequest(char *TerminatedStringFromBuffer){

    HttpRequest *request;
    


}

typedef struct {
    SOCKET client;
} SocketThreadData;

unsigned __stdcall handleClientConnection(void *data) {
    SocketThreadData *scthdp = (SocketThreadData *)data;
    SOCKET clientSocket = scthdp->client;

    char recvbuffer[BUFFER_LENGTH_DEFAULT];    
    int recvbufferlen = BUFFER_LENGTH_DEFAULT;

    int bytesRecieved,bytesSent;

    printf("Client connected at thread: %u\n", GetCurrentThreadId());

    while ((bytesRecieved = recv(clientSocket, recvbuffer, recvbufferlen - 1, 0)) > 0) {
        recvbuffer[bytesRecieved] = '\0';
        printf("Client sent: %s", recvbuffer);

        bytesSent = send(clientSocket, recvbuffer, bytesRecieved, 0);

        if (bytesSent == SOCKET_ERROR) {
            printf("Send failed : %d\n", WSAGetLastError());
            break;
        }
    }

    if (bytesRecieved == 0) {
        printf("Client disconnected.");
    } else {
        printf("Recieve failed : %d\n", WSAGetLastError());
    }

    closesocket(clientSocket);
    free(scthdp);
    return 0;

}


int main() {
    WSADATA wsaData;
    SOCKET listenSocket = INVALID_SOCKET, clientSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL, *ptr = NULL, hints;
    int iResult;

    int reqResult, resResult;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData); 
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }

    // Set up the hints for the address info
    ZeroMemory(&hints, sizeof(hints));  // Clear out memory for hints structure
    hints.ai_family = AF_INET;          // Use IPv4 addresses
    hints.ai_socktype = SOCK_STREAM;    // TCP socket
    hints.ai_protocol = IPPROTO_TCP;    // TCP protocol
    hints.ai_flags = AI_PASSIVE;        // Suitable for binding to a local address

    // Resolve the local address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Create a socket
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
        listenSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (listenSocket == INVALID_SOCKET) {
            printf("socket failed: %ld\n", WSAGetLastError());
            continue;
        }

        // Bind the socket
        iResult = bind(listenSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            printf("bind failed: %ld\n", WSAGetLastError());
            closesocket(listenSocket);
            continue;
        }
        break;
    }

    freeaddrinfo(result);  // Free the memory allocated by getaddrinfo

    if (listenSocket == INVALID_SOCKET) {
        printf("Unable to bind to the address and port\n");
        WSACleanup();
        return 1;
    }
    

    // Listen for incoming connections
    iResult = listen(listenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        printf("listen failed: %ld\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    printf("Listening for clients on %s...\n", DEFAULT_PORT);

    char running = !0;
    while (running) {
        if (!running) {
            break;
        }

        SOCKET clientSocket = accept(listenSocket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET){
            printf("Accept failed : %d\n", WSAGetLastError());
            continue;
        }

        SocketThreadData *threadData = (SocketThreadData *)malloc(sizeof(SocketThreadData));
        if (threadData == NULL) {
            printf("Memory allocation failed...\n");
            closesocket(clientSocket);
            continue;
        }
        threadData->client = clientSocket;

        HANDLE threadHandle = (HANDLE)_beginthreadex(NULL, 0, handleClientConnection, threadData, 0, NULL);
        if (threadHandle == 0){
            printf("Thread creation failed... \n");
            free(threadData);
            closesocket(clientSocket);
            continue;
        }

        CloseHandle(threadHandle);
    }




   
     
    // Cleanup and shutdown
    closesocket(listenSocket);  // Close the listening socket
    closesocket(clientSocket);  // Close the client connection socket
    WSACleanup();               // Clean up Winsock

    return 0;
}
