#include "client.h"


int clientSocket;

bool _send_to_server(string msg) {
    if (send(clientSocket, msg.c_str(), msg.length(), 0) == -1) {
        return false;
    }
    return true;
}

int main() {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cout << "Error initializing Winsock: " << WSAGetLastError() << endl;
        return 1;
    }
#endif

    struct sockaddr_in serverAddress;

    // Create a TCP socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        cout << "Error creating socket: " << errno << endl;
        return 1;
    }

    // Set up server address
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");  // Replace with server IP if connecting remotely
    serverAddress.sin_port = htons(PORT);

    // Connect to the server
    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        cout << "Error connecting to server: " << endl;
        close(clientSocket);
        return 1;
    }

    cout << "Connected to server." << endl;
    thread messageThread([&]() {
        my_client_code();
    });

    char buffer[BUFFER_SIZE];
    while (true) {
        // Receive data from server
        int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0);
        if (bytesReceived == 0) { // Server disconnected
            cout << "Server disconnected." << endl;
            break;
        }
        else if (bytesReceived == -1) {
            cout << "Error receiving data from server: " << endl;
            break;
        }

        // Print received message
        recvFromServer(string(buffer, bytesReceived));
    }

    // Close the socket
    close(clientSocket);

#ifdef _WIN32
    WSACleanup();
#endif

    return 0;
}
