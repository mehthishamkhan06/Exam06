# Exam06 -- Subject

**Assignment name**: `mini_serv`

**Expected files**: `mini_serv.c`

**Allowed functions**: `write`, `close`, `select`, `socket`, `accept`, `listen`, `send`, `recv`, `bind`, `strstr`, `malloc`, `realloc`, `free`, `calloc`, `bzero`, `atoi`, `sprintf`, `strlen`, `exit`, `strcpy`, `strcat`, `memset`

---

Write a program that will listen for clients to connect on a certain port on `127.0.0.1` and will let clients speak with each other.

### Error Handling & Setup

* This program will take as its first argument the port to bind to.
* If no argument is given, it should write to `stderr` `"Wrong number of arguments"` followed by a `\n` and exit with status `1`.
* If a System Call returns an error before the program starts accepting connections, it should write to `stderr` `"Fatal error"` followed by a `\n` and exit with status `1`.
* If you can't allocate memory, it should write to `stderr` `"Fatal error"` followed by a `\n` and exit with status `1`.

### Core Requirements

* Your program must be non-blocking. However, clients can be lazy; if they don't read your message, you must **NOT** disconnect them.
* Your program must **not** contain `#define` preprocessor directives.
* Your program must only listen to `127.0.0.1`.
* The `fd` that you will receive will already be set to make `recv` or `send` block if `select` hasn't been called before calling them, but will not block otherwise.

### Expected Behavior

**When a client connects to the server:**

* The client will be given an ID. The first client will receive the ID `0`, and each new client will receive the `last client id + 1`.
* `%d` will be replaced by this number.
* A message is sent to all the clients that were connected to the server: `"server: client %d just arrived\n"`

**When clients send messages:**

* Clients must be able to send messages to your program.
* Messages will only contain printable characters (no need to check).
* A single message can contain multiple `\n`.
* When the server receives a message, it must resend it to all the other clients with `"client %d: "` before every line!

**When a client disconnects from the server:**

* A message is sent to all the clients that were connected to the server: `"server: client %d just left\n"`

### Constraints

* Memory or fd leaks are **forbidden**.
* To help you, you will find the file `main.c` with the beginning of a server and maybe some useful functions. *(Beware: this file uses forbidden functions or writes things that must not be there in your final program).*
* **Warning:** Our tester is expecting that you send the messages as fast as you can. Don't do unnecessary buffering.
* Evaluation can be a bit longer than usual...

---

### Hints

> **Hint:** You can use `nc` to test your program.
> **Hint:** You *should* use `nc` to test your program.
> **Hint:** To test, you can use `fcntl(fd, F_SETFL, O_NONBLOCK)` but use `select` and **NEVER** check `EAGAIN` (`man 2 send`).
