#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sstream>
#include<vector>

void display_results() {
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

    std::string request = "GET_RESULTS";
    send(client_socket, request.c_str(), request.size(), 0);

    char buffer[1024];
    recv(client_socket, buffer, 1024, 0);
    std::string response(buffer);

    std::cout << "Election Results:" << std::endl;
    std::istringstream iss(response);
    std::string candidate;
    int votes;
    int max_votes = 0;
    std::vector<std::string> winners;

    while (iss >> candidate >> votes) {
        std::cout << candidate << ": " << votes << std::endl;
        if (votes > max_votes) {
            max_votes = votes;
            winners.clear();
            winners.push_back(candidate);
        } else if (votes == max_votes) {
            winners.push_back(candidate);
        }
    }

    if (winners.size() == 1) {
        std::cout << "The winner is: " << winners[0] << std::endl;
    } else {
        std::cout << "The winners are: ";
        for (const auto& winner : winners) {
            std::cout << winner << " ";
        }
        std::cout << std::endl;
    }

    close(client_socket);
}

int main() {
    display_results();
    return 0;
}
