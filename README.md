# Basic TFTP implementation
A basic TFTP implementation, support RRQ only, for the course of computer networking.
The server supports multiple concurrent users.

## Usage
### Server
To run the server

    ./tftp_server <port number> <file directory>

The server will make available all the files in `file directory` to the requesting clients.

### Client
The clients use an interactive mode, started by

    ./tftp_server <server ip> <server port number>

The possible commands are:

    !help --> shows available commands list
    !mode {txt|bin} --> sets transfer mode, can be binary or txt
    !get filename local_name --> gets filename from the files available on the server and saves it as local_name (in the current client directory)
    !quit --> exits interactive mode
