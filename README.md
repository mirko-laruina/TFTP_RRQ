# Basic TFTP implementation
A basic TFTP implementation, supporting RRQ only, for the course of Computer Networking.
The server supports multiple concurrent users.

## Usage
### Server
To run the server

    ./tftp_server <port number> <file directory>

The server will make available all the files in `file directory` to the requesting clients.
The specified `port number` will be used to listen for incoming RRQ packets.

### Client
The clients use an interactive mode, started by

    ./tftp_server <server ip> <server port number>

The possible commands are:

    !help --> shows available commands list
    !mode {txt|bin} --> sets transfer mode, can be binary or txt
    !get filename local_name --> gets filename (if available on the server file directory) and
    saves it as local_name (in the current client directory)
    !quit --> exits interactive mode
