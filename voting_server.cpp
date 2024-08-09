#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <map>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<unistd.h>

std::mutex mtx;
std::condition_variable cv;
std::map<std::string, int> election_results;
std::vector<std::thread> threads;

void handle_client(int client_socket) {
    char buffer[1024];
    recv(client_socket, buffer, 1024, 0);
    std::string request(buffer);

    if (request == "GET_RESULTS") {
        std::string response;
        std::lock_guard<std::mutex> lock(mtx);
        for (auto& result : election_results) {
            response += result.first + " " + std::to_string(result.second) + "\n";
        }
        send(client_socket, response.c_str(), response.size(), 0);
    } else {
        std::lock_guard<std::mutex> lock(mtx);
        if (election_results.find(request) != election_results.end()) {
            election_results[request]++;
        } else {
            election_results[request] = 1;
        }
        cv.notify_all();
    }

    close(client_socket);
}

void display_results() {
    std::cout << "Election Results:" << std::endl;
    for (auto& result : election_results) {
        std::cout << result.first << ": " << result.second << std::endl;
    }
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        std::cerr << "Error creating socket" << std::endl;
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Error binding socket" << std::endl;
        return 1;
    }

    if (listen(server_socket, 3) < 0) {
        std::cerr << "Error listening on socket" << std::endl;
        return 1;
    }

    std::cout << "Voting server started. Waiting for clients..." << std::endl;

    while (true) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket < 0) {
            std::cerr << "Error accepting client" << std::endl;
            continue;
        }

        std::thread t(handle_client, client_socket);
        threads.push_back(std::move(t));
    }

    return 0;
}
