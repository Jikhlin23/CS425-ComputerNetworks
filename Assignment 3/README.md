# TCP Handshake Implementation - README

## Overview
This Assignment implements the client-side of the TCP three-way handshake using raw sockets in C++. The server-side code is provided and handles the server's part of the handshake. The main objective here is to construct and send a SYN packet, receive and process a SYN-ACK response, and send an ACK to complete the handshake using raw sockets.

## Contents
- **client.cpp**: The source code for the TCP client that sends the initial SYN, processes the SYN-ACK from the server, and sends the final ACK.
- **server.cpp**: The provided server-side code that listens for the SYN packet, sends a SYN-ACK in response, and awaits the final ACK.
- **Makefile**: For compiling both the client and server programs.
- **README.md**: This file with instructions on how to build and run the programs.

## Requirements
- Linux-based system (e.g., Ubuntu, Debian, etc.)
- Root privileges to send raw packets via raw sockets.
- A C++17 compatible compiler (e.g., `g++`).
- The `inet` and `arpa/inet.h` libraries for networking.

## Instructions

### Step 1: Clone the repository
Clone the repository from the provided GitHub link:

```bash
git clone https://github.com/privacy-iitk/cs425-2025.git
cd cs425-2025/Homeworks/A3
```

### Step 2: Compile the Code

To compile both the client and server code, use the following `g++` commands:

```bash
g++ -std=c++17 client.cpp -o client
g++ -std=c++17 server.cpp -o server
```

Alternatively, you can use a `Makefile` to compile the programs easily:

```bash
make
```

### Step 3: Run the Server

To start the server, run the following command:

```bash
sudo ./server
```

The server will listen on port `12345` and wait for the client to initiate the handshake.

### Step 4: Run the Client

In a separate terminal window, run the client:

```bash
sudo ./client
```

The client will initiate the TCP three-way handshake by sending a SYN packet, receiving a SYN-ACK from the server, and finally sending an ACK to complete the handshake.

### Step 5: Verify the Handshake

The server should print the following output when it receives the SYN, SYN-ACK, and ACK packets:

```
[+] TCP Flags:  SYN: 1 ACK: 0 FIN: 0 RST: 0 PSH: 0 SEQ: 200
[+] Received SYN from 127.0.0.1
[+] Sent SYN-ACK
[+] TCP Flags:  SYN: 0 ACK: 1 FIN: 0 RST: 0 PSH: 0 SEQ: 600
[+] Received ACK, handshake complete.
```

The client should print the following output:

```
[+] Sent SYN (seq=200)
[+] TCP Flags:  SYN: 1 ACK: 1 FIN: 0 RST: 0 PSH: 0 SEQ: 400
[+] Received SYN-ACK from 127.0.0.1
[+] Sent final ACK (seq=600)
[+] Handshake complete
```

### Step 6: Shutdown the Server

After the handshake is complete, we can safely close the server by pressing `Ctrl+C`.

## Key Concepts

- **Raw Sockets**: This allows manual construction of IP and TCP packets, bypassing the operating system's transport layer. We directly set the IP and TCP headers and compute the checksums ourselves.
- **TCP Three-Way Handshake**: The client and server exchange three packets:
  1. **SYN**: Client sends a SYN packet with an initial sequence number.
  2. **SYN-ACK**: Server responds with a SYN-ACK, acknowledging the client's sequence number and sending its own sequence number.
  3. **ACK**: Client responds with an ACK to acknowledge the server's SYN-ACK, completing the handshake.

## Explanation of the Code

### Server-side Code (`server.cpp`)

1. **Listening**: The server listens on port `12345` for incoming TCP packets using raw sockets.
2. **SYN Handling**: Upon receiving a SYN packet, the server sends a SYN-ACK with a sequence number of `400`.
3. **Final ACK Handling**: Once the server receives the final ACK from the client with the sequence number `600`, the handshake is complete, and the server terminates.

### Client-side Code (`client.cpp`)

1. **SYN Packet**: The client sends an initial SYN packet with a sequence number of `200` to the server.
2. **SYN-ACK Response**: The client listens for the SYN-ACK from the server and checks that it contains the correct sequence number (`400`).
3. **Final ACK**: The client then sends a final ACK with a sequence number of `600` to complete the handshake.

### Sequence Numbers

- **SYN Packet**: The client sends a SYN packet with the sequence number `200`.
- **SYN-ACK Response**: The server sends a SYN-ACK with the sequence number `400`.
- **Final ACK**: The client sends a final ACK with the sequence number `600`.
