# Get Started
  // First configure the ip address of server in client code.
  To compile : type make in the terminal.
  To run client executable : use ./client_grp 
  To run server executable : use ./server_grp
  Give correct username and password to get connected to the chat server, otherwise you will be disconnecte.
  All the valid usernames and password are available in "users.txt" file.
  Additional tip:  type \help to know all the functionalities.
 

# Chat Server Features
This multi-threaded chat server supports authentication, private messaging, group messaging, broadcasting, and group management using C++ and socket programming.

---
##  Authentication System
• User Login
- Clients must enter a valid "username and password" to access the chat.
- The credentials are loaded from a `users.txt` file.

• Invalid Credentials Handling
- If a client enters incorrect credentials, they receive an "Authentication failed." message and are    disconnected. 

• User Management
- The server stores logged-in users in a "map" (`std::unordered_map<int, std::string> clients`).

---
## Messaging Features
• Broadcast Message
- Sends a message to "all connected clients except the sender".
- Command:  /broadcast <message>

• Private Message
- Sends a "direct" message to a specific user.
- Command: /msg <username> <message>
- If the user does not exist, the sender gets a "User not found." response.

• Group Message
- Sends a message to "all users in a group except the sender".
- Command: /group_msg <group_name> <message>
- If the group does not exist, the user receives "Group not found" message.

---

## Group Management
 • Create Group
- Users can create a "new chat group" and become its first member.
- Command: /create_group <group_name>
- If the group already exist, the user get "Group <group_name> already exists. " 


• Join Group
- Users can "join an existing group" and receive messages from that group.
- Command: /join_group <group_name>
- Other members in the group receive a notification when someone joins.

• Leave Group
- Users can "leave a group", stopping further messages from that group.
- "Command:" /leave_group <group_name>
- Other members in the group receive a notification when someone leaves.
- If user is already not in group, user gets "You are not in the group."  

• List Joined Groups
- Users can check which groups they are part of.
- Command: /ls

---

##  "Server Features"

• "Multi-threaded Handling"
- Each client connection runs on a "separate thread" using `std::thread`, enabling multiple users to chat simultaneously.

• "Client Disconnection Handling"
- If a client "disconnects", they are removed from:
- The "active users list" (`clients` map).
- Any groups they were part of (`groups` map).
- Their socket is closed.

• "Help Command"
- Displays a "list of available commands" to the client.
- "Command:" /help
- "Response": /msg /broadcast /create_group /join_group /group_msg /leave_group /ls

• "Data Persistence (User Login Info)"
- Reads "username-password pairs" from `users.txt` for authentication.


#  Chat Server - Design Decisions

## " Multi-threading for Concurrent Clients"

- Used "std::thread" to handle each client "in a separate thread".
- Used "detached threads" (".detach()") to manage clients asynchronously.


- Ensures "multiple clients" can send/receive messages "simultaneously".  
- Prevents "blocking" while waiting for client input.  
- Threads exit automatically when a client disconnects, freeing up system resources.  

---

## " Authentication System"

- "User authentication" is done by reading from a "users.txt" file.
- Used "std::unordered_map<std::string, std::string>" to store "username-password pairs".

- "Fast lookup (O(1))" for authentication checks.  
- Keeps the "server stateless" in terms of user data (easy to replace with a database later).  
- Simple text file storage makes it easy to add/remove users manually.  

---

## Data Structures for Chat Functionality and group functionality


-std::unordered_map<int, std::string> clients; // Socket -> Username
-std::unordered_map<std::string, std::unordered_set<int>> groups;
-std::unordered_map<int, std::unordered_set<std::string>> member_info;

Reason for the data structures we used :
- Allows O(1) lookups to get the username from a client socket. Thread-safe with std::mutex.
- groups stores group name i.e., set of client sockets for fast messaging.
- member_info tracks which groups a user belongs to, making /ls easy to implement.
Why unordered sets?
- O(1) insertion and lookup for group membership.
- Prevents duplicate entries in groups.

# Codeflow

Image "codeflow.png" have been attached showing codeflow of the working of the server.

# Testing 
Stress Testing: Stress test Code "stresstext.cpp" and  Image "stresstest.png" shows how we test it for 11000 concurrent clients.
Correctness Testing: We acted like multiple clients and verified all the commands. We have attached few screenshots in correct1.jpeg, correct2.jpeg , correct3.jpeg. 

# Restrictions in your server:
How many clients can connect with your server? -- (We have checked till 11000 clients)
At max. how many groups can be created? -- maximum number of element in std::unordered_map<std::string, std::unordered_set<int>>.
At max. how many members can be part of any group? size of std::unordered_set<int>. 
How big message can be transferred in one go?  -- 1024 bytes.

# Challenges you faced while solving the assignment 

- During Correctness testing, we felt many of our features were not working properly like create_group
  where creating the group with same name means previous group is destroyed. We resolved it by giving the error "Group already exists".
- Even after the user was disconnected, they were still in the groups they had joined, we made sure they     left the group properly before closing their socket. 
- During stress testing, I created file with 11 lakhs of users and then wrote a code to stress_test it. when we were giving no delay, only 59 users were getting added , It stopped after that. But after that when I gave a delay of 10 milliseconds using std::this_thread::sleep_for(std::chrome::milliseconds(10)) it was able to get users as many as I was asking . I checked for 11000 users from my side and all were getting added.

# Individual Contributions
- Contribution of Kawaljeet Singh(220514) -- 33.33%
- Contribution of Nikhil Jain(220709) -- 33.33%
- Contribution ofl Rishikesh Sahil(220892) -- 33.33%

# Sources
- CLion Platform for Coding
- Figma for making code-flow

# Declaration
We Kawaljeet Singh, Nikhil Jain and Rishikesh Sahil declare that we did not indulge in plagiarism.










