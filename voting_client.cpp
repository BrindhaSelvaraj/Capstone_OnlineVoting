#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

void cast_vote(std::string candidate) {
    int client_socket;
    struct sockaddr_in server_addr;

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        std::cerr << "Error creating socket" << std::endl;
        return;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Error connecting to server" << std::endl;
        return;
    }

    send(client_socket, candidate.c_str(), candidate.size(), 0);
    close(client_socket);
}

int main() {
    std::string voter_id, candidate;
    std::cout << "Enter your voter ID: ";
    std::cin >> voter_id;
    std::cout << "Enter the candidate you want to vote for (DMK, ADMK, or BJP): ";
    std::cin >> candidate;

    if (candidate != "DMK" && candidate != "ADMK" && candidate != "BJP") {
        std::cerr << "Invalid candidate. Please try again." << std::endl;
        return 1;
    }

    cast_vote(candidate);

    return 0;
}
