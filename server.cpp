#include "server.h"


// Mutex to protect the client list
mutex clientMutex;
vector<int> connectedClients;


bool _sendToClient(int clientNo, string message) {
    if (send(clientNo, message.c_str(), message.length(), 0) == -1) {
        connectedClients.erase(remove(connectedClients.begin(), connectedClients.end(), clientNo), connectedClients.end());
        return false;
    }
    return true;
}

void handleClient(int clientSocket) {
    char buffer[BUFFER_SIZE];
    while (true) {
        int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0);
        if (bytesReceived == 0) { // Client disconnected
            cout << "Client " << clientSocket << " disconnected." << endl;
            clientMutex.lock();
            connectedClients.erase(remove(connectedClients.begin(), connectedClients.end(), clientSocket), connectedClients.end());
            clientMutex.unlock();
            handleClientDisconnect(clientSocket);
            break;
        }
        else if (bytesReceived == -1) {
            cout << "Error receiving data from client: " << clientSocket << endl;
            break;
        }

        // Process received data from client (if needed)
        string message(buffer, bytesReceived);
        recvFromClient(clientSocket, message);

    }
    close(clientSocket);
}

int main() {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cout << "Error initializing Winsock: " << WSAGetLastError() << endl;
        return 1;
    }
#endif

    int serverSocket;
    struct sockaddr_in serverAddress;

    // Create a TCP socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        cout << "Error creating socket: " << errno << endl;
        return 1;
    }

    // Set up server address
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;  // Listen on all interfaces
    serverAddress.sin_port = htons(PORT);

    // Bind the socket to the address
    if (::bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        cout << "Error binding socket: " << errno << endl;
        return 1;
    }

    // Listen for incoming connections
    if (listen(serverSocket, MAX_CLIENTS) == -1) {
        cout << "Error listening for connections: " << errno << endl;
        return 1;
    }

    cout << "Server started on port " << PORT << endl;

    // Thread to send messages to clients periodically
    thread messageThread(my_server_code);

    // Accept incoming connections in a loop
    while (true) {
        struct sockaddr_in clientAddress;

        customsize clientAddressSize = sizeof(clientAddress);
        int clientSocket = ::accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressSize);
        
        if (clientSocket == -1) {
            cout << "Error accepting connection: " << errno << endl;
            continue;
        }

        cout << "Client connected from " << inet_ntoa(clientAddress.sin_addr) << ":" << ntohs(clientAddress.sin_port) << " with ID: " << clientSocket << endl;

        // Add the new client socket to the list of connected clients
        clientMutex.lock();
        connectedClients.push_back(clientSocket);
        clientMutex.unlock();
        handleClientConnect(clientSocket);

        // Create a thread to handle the client connection
        thread clientThread(handleClient, clientSocket);
        clientThread.detach();  // Detach the thread so it doesn't need to be joined
    }

    // Close the socket (would happen when the loop exits)
    close(serverSocket);

#ifdef _WIN32
    WSACleanup();
#endif

    return 0;
}