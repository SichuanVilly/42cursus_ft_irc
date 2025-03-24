# 42Cursus ft_irc

This project is part of the **42cursus** curriculum and focuses on implementing a basic Internet Relay Chat (IRC) server. The goal is to understand how client-server communication works over a network and to build a simple IRC protocol implementation with features like user authentication, channels, and message broadcasting.

## Table of Contents

- [Description](#description)
- [Installation](#installation)
- [Usage](#usage)
- [Features](#features)
- [Built With](#built-with)
- [License](#license)

## Description

**ft_irc** is an IRC server program written in C++ that implements basic IRC protocols. The project aims to provide hands-on experience with networking, socket programming, and multithreading. The server allows multiple clients to connect, authenticate, join channels, and send messages.

### Key Features:

- Client-server communication using sockets
- User authentication (nickname, password)
- Channel creation and management
- Message broadcasting to channels and private messages
- Handling multiple clients simultaneously with multithreading
- IRC protocol compliance for messages and commands

## Installation

1. Clone the repository:

    ```bash
    git clone https://github.com/SichuanVilly/42cursus_ft_irc.git
    ```

2. Navigate to the project directory:

    ```bash
    cd 42cursus_ft_irc
    ```

3. Compile the project using `make`:

    ```bash
    make
    ```

    This will generate the `irc_server` executable.

## Usage

### Running the IRC server:

To start the IRC server, execute the following command:

```bash
./irc_server <port_number>
```
### Example Usage:

1. Start the IRC server:

    ```bash
    ./irc_server 6667
    ```

2. The server will now wait for client connections. You can use **irssi** or any IRC client to connect to the server.

### Tested with irssi client:

To test with the **irssi** IRC client, run the following command:

```bash
irssi -c 127.0.0.1 -p 6667
```
### Example IRC Commands in irssi:

- **/join <channel_name>**  
  Join a channel (e.g., `/join #general`)

- **/msg <nickname> <message>**  
  Send a private message to a user (e.g., `/msg user1 Hello!`)

- **/quit**  
  Quit the IRC session

- **/nick <nickname>**  
  Change your nickname (e.g., `/nick new_user`)

- **/list**  
  List all available channels

- **/whois <nickname>**  
  Get information about a user (e.g., `/whois user1`)

- **/part <channel_name>**  
  Leave a channel (e.g., `/part #general`)

- **/topic <channel_name> <topic>**  
  Set the topic of a channel (e.g., `/topic #general Welcome to the chat!`)

- **/away <message>**  
  Set an away message (e.g., `/away I'm currently offline`)
  
## Built With

- **C++** – The programming language used to implement the IRC server.
- **Sockets** – For client-server communication over the network.
- **Threads** – Used for handling multiple clients concurrently.
- **Make** – The build automation tool used to compile the project.

