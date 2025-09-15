# Simple C HTTP Server with Thread Pool

A minimal HTTP server written in C that serves static HTML pages.
It uses a **thread pool** to efficiently handle multiple client connections concurrently.

---

## Features


* Serves static HTML pages from `static-pages/` directory.
* Uses a fixed-size **thread pool** to handle concurrent clients.
* Proper HTTP response headers, including `Content-Length` and `Content-Type`.
* Graceful handling of file not found or read errors.
* **Graceful shutdown on Ctrl+C**: cleans up thread pool and closes sockets properly.
* Easy-to-extend architecture for future features (dynamic pages, MIME types, etc.).

---

## Project Structure

```
multithreaded-http-server/
├── main.c
├── Makefile
├── http-server/
│   ├── http_server.c
│   └── http_server.h
├── thread-pooler/
│   ├── thread_pooler.c
│   └── thread_pooler.h
├── worker-threads/
│   ├── worker_thread.c
│   └── worker_thread.h
├── static-pages/
│   └── index.html
```

---

## Prerequisites

* GCC compiler
* Linux / Unix-like environment
* `pthread` library (included with most Linux distributions)

---

## Build Instructions

1. Open a terminal in the project root folder.
2. Run the following commands:

```
make clean
make run
```

* `make clean` removes old object files and executable.
* `make run` compiles and runs the server.

---

## Usage

1. The server listens on **port 8080** by default.
2. Open a web browser and go to:

[http://127.0.0.1:8080](http://127.0.0.1:8080)

3. The server will serve `static-pages/index.html`.
4. Press **Ctrl+C** to stop the server gracefully. The thread pool will be destroyed, and all sockets will be closed properly.

---

## Customization

* **Port number:** Modify `#define PORT 8080` in `http_server.h`.
* **Static pages:** Add more HTML files in the `static-pages/` directory.
* **Thread pool size:** Adjust `QUEUE_SIZE` in `http_server.c`.

---

## Notes

* Currently, the server always serves `index.html`.
* You can extend it to parse the HTTP request and serve different pages based on the URL.
* MIME types are currently hardcoded as `text/html`.
* Graceful shutdown ensures no memory leaks or dangling threads when stopping the server.


---

## Author

Sravan Gandla
[GitHub](https://github.com/sravan-3)
