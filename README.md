# Socket File Transfer

This project implements a file transfer mechanism using sockets in C. It allows you to send and receive files between a client and server over a network connection.

## Prerequisites

To compile and run this project, you need:

- A C compiler (e.g., GCC)
- Linux or a Unix-like operating system

## Usage

### Preparation
1. Compile everything using the following command:

   ```shell
   make
   ```
### Server

1. Run the server program:

   ```shell
   ./server <v4/v6> <port_number>
   ```

   Replace `<v4/v6>` with the v4 for IPv4 addresses or v6 for IPv6 adresses and `<port_number>` with the port number on which you want the server to listen for incoming connections.

### Client

1. Run the client program:

   ```shell
   ./client <server_ip> <port_number>
   ```

   Replace `<server_ip>` with the IP address or hostname of the server, `<port_number>` with the port number on which the server is listening

## How it Works

The server and client communicate using TCP/IP sockets. Here's a high-level overview of how the file transfer process works:

1. The server creates a socket and binds it to a specified port number. It listens for incoming connections from clients.

2. The client creates a socket and establishes a connection with the server using the server's IP address and port number.

3. Once the connection is established, the client can select and send files via cli using "select <file>" and "send file".

4. The server receives the file chunks and writes them to a file on the server-side.

5. After the file transfer is complete, the client and server close their sockets and terminate the connection.

## Limitations

- The maximum file size that can be transferred depends on the underlying operating system and file system limitations.
- The program assumes that the client and server are running on the same network and can communicate with each other directly.
