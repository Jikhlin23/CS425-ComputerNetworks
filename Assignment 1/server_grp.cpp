#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <limits.h>

#define PORT 12345
#define BUFFER_SIZE 1024
#define MAX_USERS INT_MAX

// Mutex to ensure thread-safety for shared data structures
std::mutex clients_mutex;

// Maps and sets to store clients, users, and groups
std::unordered_map<int, std::string> clients;  // Socket -> Username
std::unordered_map<std::string, std::string> users;  // Username -> Password
std::unordered_map<std::string, std::unordered_set<int>> groups; // Group -> Clients
std::unordered_map<int, std::unordered_set<std::string>> member_info;

// Function to send a message to a specific client
void send_message(int client_socket, const std::string &message) {
    send(client_socket, message.c_str(), message.size(), 0);
}

// Function to broadcast a message to all connected clients, excluding the sender
void broadcast_message(const std::string &message, int sender_socket) {
    std::lock_guard<std::mutex> lock(clients_mutex);
    for (const auto &[socket, username] : clients) {
        if (socket != sender_socket) {
            send_message(socket, "[Broadcast] " + clients[sender_socket]+ " : " + message);
        }
    }
}

// Function to send a private message to a specific recipient
void private_message(int sender_socket, const std::string &recipient, const std::string &message) {
    std::lock_guard<std::mutex> lock(clients_mutex);
    for (const auto &[socket, username] : clients) {
        if (username == recipient) {
            send_message(socket, "[Private] " + clients[sender_socket] + ": " + message);
            return;
        }
    }
    send_message(sender_socket, "User not found.");
}


// Function to authenticate a client by username and password
bool authenticate_client(int client_socket) {
    char buffer[BUFFER_SIZE];

    // Request username and password from client
    send_message(client_socket, "Enter username: ");
    recv(client_socket, buffer, BUFFER_SIZE, 0);
    std::string username(buffer);

    send_message(client_socket, "Enter password: ");
    recv(client_socket, buffer, BUFFER_SIZE, 0);
    std::string password(buffer);

    // Authentication check
    std::lock_guard<std::mutex> lock(clients_mutex);
   
    if (users.find(username) == users.end() || users[username] != password) {
        send_message(client_socket, "Authentication failed.");
        return false;
    }

    // Add client to the list of authenticated clients
    clients[client_socket] = username;
    send_message(client_socket, "Welcome to the chat server!");
    std::cout<<username<<" joined the server\n";
    return true;
}

// Function to disconnect a client
void disconnect_client(int client_socket) {
    std::lock_guard<std::mutex> lock(clients_mutex);
    std::cout<<clients[client_socket]<<" left the server\n";
    clients.erase(client_socket);
    for(auto& it:member_info[client_socket]){
        groups[it].erase(client_socket);
    }
    member_info[client_socket]={};
    close(client_socket);
}

// Function to handle private message to another user
void handle_private_message(int sender_socket, const std::string &message) {
    size_t space = message.find(' ', 5);
    if (space != std::string::npos) {
        std::string recipient = message.substr(5, space - 5);
        std::string msg = message.substr(space + 1);
        private_message(sender_socket, recipient, msg);
    }
}

// Function to handle the creation of a new group
void handle_create_group(int client_socket, const std::string &message) {
    std::string group_name = message.substr(14);
    std::lock_guard<std::mutex> lock(clients_mutex);
    if(!groups[group_name].empty()){
        send_message(client_socket, "Group '" + group_name + "' already exists.");
        return ;
    }
    groups[group_name] = {};
    groups[group_name].insert(client_socket);
    member_info[client_socket].insert(group_name);
    send_message(client_socket, "Group " + group_name + " created and you've joined it.");
}

// Function to handle a client joining a group
void handle_join_group(int client_socket, const std::string &message) {
    std::string group_name = message.substr(12);
    std::lock_guard<std::mutex> lock(clients_mutex);
    if (groups.find(group_name) == groups.end()) {
        send_message(client_socket, "Group not found.");
    } else {
        if(member_info[client_socket].find(group_name)!=member_info[client_socket].end()){
            send_message(client_socket, "You are already member of group '" + group_name + "'");
            return ;    
        }
        for(auto group_member : groups[group_name]) {
            send_message(group_member, "** " +clients[client_socket]+ " has Joined the group " + group_name + ".");
        }
        groups[group_name].insert(client_socket);
        member_info[client_socket].insert(group_name);
        send_message(client_socket, "You joined the group " + group_name);
    }
}

// Function to handle sending messages to a group
void handle_group_message(int client_socket, const std::string &message) {
    size_t space = message.find(' ', 11);
    if (space != std::string::npos) {
        std::string group_name = message.substr(11, space - 11);
        std::string group_msg = message.substr(space + 1);

        std::lock_guard<std::mutex> lock(clients_mutex);
        if (groups.find(group_name) == groups.end()) {
            send_message(client_socket, "Group not found.");
        } else {
            if(member_info[client_socket].find(group_name)==member_info[client_socket].end()){
                send_message(client_socket,"You are not member of the group '" + group_name + "'");
                return ;
            }
            for (int socket : groups[group_name]) {
                if (socket != client_socket) {
                    send_message(socket, "[Group " + group_name + "] " + clients[client_socket] + ": " + group_msg);
                }
            }
        }
    }
}

// Function to handle a client leaving a group
void handle_leave_group(int client_socket, const std::string &message) {
    std::string group_name = message.substr(13);
    std::lock_guard<std::mutex> lock(clients_mutex);
    if (groups.find(group_name) == groups.end() || groups[group_name].find(client_socket) == groups[group_name].end()) {
        send_message(client_socket, "You are not in the group.");
    } else {
        for(auto& it:groups[group_name]){
            if(it!=client_socket){
                send_message(it,"** '"+ clients[client_socket] + "' left the group");    
            }
        }
        groups[group_name].erase(client_socket);
        member_info[client_socket].erase(group_name);
        send_message(client_socket, "You left the group " + group_name);
    }
}
void handle_ls(int client_socket) {
    std::lock_guard<std::mutex> lock(clients_mutex);
    for(auto it = member_info[client_socket].begin(); it != member_info[client_socket].end(); it++) {
        send_message(client_socket, (*it).c_str());
        send_message(client_socket, "\n");
    }

}

// Function to process a client's message and execute appropriate commands
void process_client_message(int client_socket, const std::string &message) {
    if (message.starts_with("/broadcast ")) {
        broadcast_message(message.substr(11), client_socket);
    } else if (message.starts_with("/msg ")) {
        handle_private_message(client_socket, message);
    } else if (message.starts_with("/create_group ")) {
        handle_create_group(client_socket, message);
    } else if (message.starts_with("/join_group ")) {
        handle_join_group(client_socket, message);
    } else if (message.starts_with("/group_msg ")) {
        handle_group_message(client_socket, message);
    } else if (message.starts_with("/leave_group ")) {
        handle_leave_group(client_socket, message);
    } else if (message.starts_with("/ls")) {
        handle_ls(client_socket);
    } else if (message.starts_with("/help")) {
        std::string functions = "/msg /broadcast /create_group /join_group /group_msg /leave_group /ls";
        send_message(client_socket, functions);
    }
    else {
        send_message(client_socket, "Command not recognized.");
    }
}


// Function to handle the client communication process
void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];

    // Authentication process
    if (!authenticate_client(client_socket)) {
        close(client_socket);
        return;
    }

    // Client interaction loop
    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) {
            disconnect_client(client_socket);
            break;
        }

        std::string message(buffer);
        process_client_message(client_socket, message);
    }
}

// Function to load users and passwords from the users.txt file
void load_users() {
    std::ifstream file("users.txt");
    std::string line;

    //std::cout <<"----users.txt----"<<std::endl;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string username, password;
        if (std::getline(iss, username, ':') && std::getline(iss, password)) {
            users[username] = password;
        }
    }

}



// Main function to initialize the server and accept client connections
int main() {
    users.clear();
    groups.clear();
    member_info.clear();

    load_users();

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(server_socket < 0){
        std::cout<<"Socket creation failed\n";
        return -1;
    }
    sockaddr_in server_address{};
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    if(bind(server_socket, (sockaddr*)&server_address, sizeof(server_address)) < 0){
        std::cout<<"bind failed\n";
        close(server_socket);
        return -1;
    }

    if(listen(server_socket, MAX_USERS) < 0){
        std::cout<<"listen failed\n";
        close(server_socket);
        return -1;
    }

    std::cout << "Server started on port " << PORT << std::endl;

    // Accept incoming client connections and spawn a new thread to handle each client
    while (true) {
        sockaddr_in client_address;
        socklen_t client_len = sizeof(client_address);
        int client_socket = accept(server_socket, (sockaddr*)&client_address, &client_len);

        if (client_socket >= 0) {
            std::thread(handle_client, client_socket).detach();
        }
    }

    close(server_socket);
    return 0;
}
