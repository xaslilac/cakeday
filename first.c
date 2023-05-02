#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define forever for (;;)
#define CHUNK_SIZE 8192
#define PORT 4989
#define MAX_CONN 32

typedef FILE hfile;

int main(void) {
	int r;

	// Create a socket descriptor
	int socket_fd = socket(PF_INET, SOCK_STREAM, 0);
	if (socket_fd == -1) {
		perror("ERROR: failed to create socket");
		return errno;
	}

	// Describe our listening address
	struct sockaddr_in host_address = {
		.sin_family = AF_INET,
		.sin_port = htons(PORT),
		.sin_addr = {
			.s_addr = INADDR_ANY,
		},
	};
	// ...and compute this weird `sizeof` that we'll need a pointer to later, and we can't
	// just have in place because it'll be an rvalue and something something undefined behavior
	int sizeof_host_address = sizeof(host_address);

	// Ask the OS for our socket
	r = bind(socket_fd, (struct sockaddr*)&host_address, sizeof(host_address));
	if (r == -1) {
		perror("ERROR: failed to bind port");
		return errno;
	}

	// Stack buffers
	char conn_read_buffer[CHUNK_SIZE];
	char file_read_buffer[CHUNK_SIZE];

	// Start accepting connections
	listen(socket_fd, MAX_CONN);
	printf("\x1b[1m---\x1b[0m \x1b[1mINFO\x1b[0m listening on http://localhost:%i\n", PORT);

	// Start processing connections
	forever {
		// Accept the next queued connection (or wait for one to come in)
		int client_fd = accept(socket_fd,
			(struct sockaddr *)&host_address, (socklen_t *)&sizeof_host_address);

		if (client_fd < 0) {
			perror("ERROR: accept");
			continue;
		}

		// Log request data
		printf("\x1b[1;32m==>\x1b[0m \x1b[1mGET\x1b[0m /\n");

		// Read request data into our connection read buffer
		// TODO: Loop to read requests that are larger than `CHUNK_SIZE`
		int bytes_read = read(client_fd, conn_read_buffer, CHUNK_SIZE);
		if (bytes_read < 0) {
			perror("ERROR: read");
			goto cleanup_1;
		}

		// Log response data
		printf("\x1b[1;32m<==\x1b[0m \x1b[1m200\x1b[0m OK\n");

		// Send response head
		char response[] =
			"HTTP/1.0 200 OK\r\n"
			"server: cakeday\r\n"
		  "content-type: text/html; charset=utf-8\r\n"
			"\r\n";
		write(client_fd, response, strlen(response));

		// Send response body
		hfile* file = fopen("./docs/index.html", "r");
		if (file == NULL) {
			perror("failed to open file");
			goto cleanup_2;
		}
		// TODO: Loop to read files that are larger than `CHUNK_SIZE`
		int read_size = fread(file_read_buffer, sizeof(char), sizeof(file_read_buffer), file);
		write(client_fd, file_read_buffer, read_size);

		cleanup_2:
		fclose(file);
		cleanup_1:
		close(client_fd);
	}
}
