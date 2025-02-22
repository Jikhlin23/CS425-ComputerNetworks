#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <chrono>

std::mutex lock;

// code for joining the server

void simulate_client(int i) {
    const char* server_ip = "127.0.0.1";
    const int server_port = 12345;

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        std::lock_guard<std::mutex> guard(lock);
        std::cerr << "Client " << i << " failed to create socket." << std::endl;
        return;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    inet_pton(AF_INET, server_ip, &server_addr.sin_addr);

    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        std::lock_guard<std::mutex> guard(lock);
        std::cerr << "Client " << i << " failed to connect." << std::endl;
        close(client_socket);
        return;
    }

    std::string username = "user" + std::to_string(i);
    std::string password = "pass" + std::to_string(i + 1);
    char buffer[1024] = {0};

    recv(client_socket, buffer, 1024, 0);  // Receive username prompt
    send(client_socket, username.c_str(), username.length(), 0);
    recv(client_socket, buffer, 1024, 0);  // Receive password prompt
    send(client_socket, password.c_str(), password.length(), 0);
    recv(client_socket, buffer, 1024, 0);  // Receive server response

    {
        std::lock_guard<std::mutex> guard(lock);
        std::cout << "Client " << i << ": " << buffer << std::endl;
    }

    close(client_socket);
}

void create_multiple_clients(int client_count) {
    std::vector<std::thread> threads;
    for (int i = 1; i <= client_count; ++i) {
        threads.emplace_back(simulate_client, i);
        std::this_thread::sleep_for(std::chrono::milliseconds(10)); // 0.01 seconds delay
    }
    for (auto& thread : threads) {
        thread.join();
    }
    std::cout << "All " << client_count << " clients have connected and completed the login process." << std::endl;
}

int main() {
    int client_count = 11000;
    create_multiple_clients(client_count);
    return 0;
}