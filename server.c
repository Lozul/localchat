// Includes
// ----------------------------------------------------------------------------
#include <err.h>
#include <glib.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <gmodule.h>
#include <pthread.h>
#include <glib/gprintf.h>

// Defines
// ----------------------------------------------------------------------------

#define BUFFER_SIZE 512
#define SUFFIX "\r\n\r\n"

// Global Variables
// ----------------------------------------------------------------------------

struct addrinfo* rp;
struct addrinfo* result;

pthread_t cli_thr;

int cnx;
int run;

// Functions
// ----------------------------------------------------------------------------

// Gets the file descriptor of the server
void connect_socket(const char* port)
{
    struct addrinfo hints;
    int addrinfo_error;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    addrinfo_error = getaddrinfo(NULL, port, &hints, &result);
    if (addrinfo_error != 0)
    {
        errx(EXIT_FAILURE, "Fail getting address on port %s: %s",
            port, gai_strerror(addrinfo_error));
    }

    for (rp = result; rp != NULL; rp = rp -> ai_next)
    {
        cnx = socket(rp -> ai_family, rp -> ai_socktype, rp -> ai_protocol);
        if (cnx == -1) continue;

        if (setsockopt(cnx, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) == -1)
            errx(EXIT_FAILURE, "setsockopt(SO_REUSEADDR) failed.");

        if (bind(cnx, rp -> ai_addr, rp -> ai_addrlen) == 0) break;

        close(cnx);
    }

    if (rp == NULL)
        errx(EXIT_FAILURE, "Couldn't connect.");

    if (listen(cnx, 5) == -1)
        errx(EXIT_FAILURE, "Can't make the socket passive.");
}

void* cli_worker(void* arg)
{
    char* input;
    while (strcmp(input, "quit"))
    {
        printf("%s", "> ");
        scanf("%ms", &input);
    }

    run = 0;

    free(input);
    pthread_exit(NULL);
}

// Starts the CLI thread
void start_cli()
{
    if (pthread_create(&cli_thr, NULL, cli_worker, NULL) != 0)
        errx(EXIT_FAILURE, "pthread_create(cli_thr) failed.");
    pthread_detach(cli_thr);
}

// Frees allocated memorie and close the connection
void clean_exit()
{
    freeaddrinfo(result);
    close(cnx);
}

int main()
{
    const char* port = "2048";
    connect_socket(port);

    printf("%s\n%s %s\n", "Local Chat Server", "Listening to port", port);

    start_cli();

    run = 1;
    while (run);

    clean_exit();
}
