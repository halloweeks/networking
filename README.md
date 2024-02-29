## Description: 

This repository contains a concurrent TCP server and client implemented in C++. The server is designed to handle multiple simultaneous connections using pthreads, and the client can connect to the server, send messages, and receive responses. The server is optimized for concurrent connections, and its capacity depends on system resources such as CPU and RAM.




<H2>Installation</H2>
<p>clone this repository :</p>

```
git clone https://github.com/halloweeks/networking.git
```

```
cd networking
```


2. Compile and run the server.

    ```bash
    g++ server.cpp -o server -pthread
    ./server
    ```

3. The server will start and listen for incoming connections. It is optimized for concurrent connections, and its capacity depends on system resources.


### Client Usage:

1. Open a new terminal window.

2. Compile and run the client.

    ```bash
  
    g++ client.cpp -o client
    ./client
    ```

3. The client will connect to the server. Enter messages to send to the server.

### Concurrent Capacity:

- The server is optimized for concurrent connections.
- The maximum number of concurrent connections depends on your system's CPU, RAM, and other resources.
- During testing, the server successfully handled up to 200 concurrent connections.

