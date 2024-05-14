#pragma once

#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <string>
#include <algorithm>

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#define close closesocket
#else
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#endif

using namespace std;

const int PORT = 12345;
const int BUFFER_SIZE = 1024 * 8;
const int MAX_CLIENTS = 10;


// use this function to send a message to a client
// returns true if message sent successfully
// returns false otherwise
bool _sendToClient(int clientNo, string message);

// this function will be called when server receives a message from a client
void recvFromClient(int clientNo, string message) {
	cout << clientNo << " " << message;
}

// this function will be called when client connects to server
void handleClientConnect(int clientId) {
	cout << clientId << " connected" << endl;
}

// this function will be called when client disconnects from server
void handleClientDisconnect(int clientId) {
	cout << clientId << " disconnected" << endl;
}

void my_server_code() {
	while (true) {
	}
}