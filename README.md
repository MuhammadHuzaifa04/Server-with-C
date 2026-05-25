# C HTTP Server

A lightweight, modular HTTP server implementation in C with support for GET, POST, PUT, PATCH, and DELETE routes. Perfect for learning HTTP protocol fundamentals and socket programming.

## Project Structure

```
d:\server c\
├── include/
│   ├── http.h          (HTTP method enums)
│   ├── request.h       (Request parsing)
│   ├── response.h      (Response building)
│   ├── router.h        (Route dispatch)
│   └── connection.h    (Socket handling)
├── src/
│   ├── http.c
│   ├── request.c
│   ├── response.c
│   ├── router.c
│   └── connection.c
├── main.c              (Server entry point)
├── Makefile
└── README.md
```

## Architecture

### Modules (Separation of Concerns)

| Module         | Responsibility                                            |
| -------------- | --------------------------------------------------------- |
| **http**       | HTTP method enum conversions                              |
| **request**    | Parse raw HTTP buffer into structured Request             |
| **response**   | Build and send HTTP response to client                    |
| **router**     | Match HTTP method + path to handler function              |
| **connection** | Accept clients, orchestrate request→router→response cycle |
| **main**       | Server entry point, route registration, accept loop       |

## Building

### Prerequisites

- GCC compiler
- POSIX-compliant system (Linux, macOS, WSL on Windows)

### Compile

```bash
make
```

### Run

```bash
make run
```

Server will listen on `http://localhost:8080`

### Clean

```bash
make clean
```

## API Routes

```
GET     /              - Health check, returns server status
GET     /users         - List all users
POST    /users         - Create new user
PUT     /users         - Update user
PATCH   /users         - Partially update user
DELETE  /users         - Delete user
```

## Testing

### Using curl

**GET home:**

```bash
curl http://localhost:8080/
```

**GET users:**

```bash
curl http://localhost:8080/users
```

**POST user:**

```bash
curl -X POST http://localhost:8080/users \
  -H "Content-Type: application/json" \
  -d '{"name":"Charlie","age":28}'
```

**PUT user:**

```bash
curl -X PUT http://localhost:8080/users \
  -H "Content-Type: application/json" \
  -d '{"id":1,"name":"Charlie Updated"}'
```

**PATCH user:**

```bash
curl -X PATCH http://localhost:8080/users \
  -H "Content-Type: application/json" \
  -d '{"id":1,"age":29}'
```

**DELETE user:**

```bash
curl -X DELETE http://localhost:8080/users
```

## Features

✓ Clean modular architecture  
✓ Supports all 5 REST methods (GET, POST, PUT, PATCH, DELETE)  
✓ Dynamic request body parsing  
✓ Case-insensitive header lookup  
✓ Proper HTTP/1.1 formatting (CRLF, reason phrases)  
✓ JSON request/response examples  
✓ Extensible handler pattern  
✓ Memory-safe (malloc/free)

## Key Implementation Details

### Request Parsing

- Locates header/body boundary (`\r\n\r\n`)
- Parses request line: METHOD PATH VERSION
- Extracts and stores headers (case-insensitive keys)
- Handles Content-Length for request bodies

### Response Building

- Automatic status line generation
- Configurable content-type
- Dynamic body allocation
- Proper HTTP/1.1 headers

### Router

- Linear route matching (method + path)
- Default 404 response for unmatched routes
- Extensible: add routes at runtime

### Connection Handling

- Incremental socket recv() to handle large payloads
- Respects HTTP Content-Length header
- Proper socket cleanup and closure

## Extending

### Adding a New Route

```c
void custom_handler(Request *request, Response *response) {
    response_set_content_type(response, "application/json");
    response_set_body(response, "{\"data\":\"your response\"}");
}

// In main()
router_add(&router, HTTP_GET, "/custom", custom_handler);
```

### Reading Request Headers

```c
const char *content_type = request_get_header(request, "Content-Type");
const char *user_agent = request_get_header(request, "User-Agent");
```

### Accessing Request Body

```c
if (request->body) {
    printf("Body: %s\n", request->body);
}
```

## Limitations

- Single-threaded (processes one connection at a time)
- No URL path parameters (`/users/:id`)
- No query string parsing
- Fixed buffer sizes (65KB max request)
- No middleware/hooks
- No keep-alive (closes connection after response)

## Performance Notes

- Suitable for embedded systems, IoT, microservices
- Not production-grade for high-concurrency scenarios
- Can handle sequential requests efficiently
- Consider threading for concurrent connections

## License

Educational/Open Source

## Author

Created for learning HTTP server implementation in C
