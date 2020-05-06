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
pthread_t client_thr;

int cnx;
int run;

GString* members;

// Functions
// ----------------------------------------------------------------------------

gsize count_members();

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

// Worker for client connection
void* client_worker(void* arg)
{
    // Gets file descriptor
    int client_cnx = *(int*) arg;

    // Variables
    int r;
    int w = 1;
    gsize offset;
    gboolean suffixed;
    GString* request = g_string_new("");
    GString* response = g_string_new("");
    char* buffer = malloc(sizeof(char) * BUFFER_SIZE);

    // Attempts to read client request
    r = read(client_cnx, buffer, BUFFER_SIZE);
    if (r == 0)
    {
        // Empty request
        close (client_cnx);
    }
    else
    {
        // Reads client request (until reach SUFFIX)
        suffixed = g_str_has_suffix(request -> str, SUFFIX);
        while (!suffixed && r > 0)
        {
            g_string_append(request, buffer);
            memset(buffer, 0, BUFFER_SIZE);

            suffixed = g_str_has_suffix(request -> str, SUFFIX);
            if (!suffixed)
                r = read(client_cnx, buffer, BUFFER_SIZE);
        }

        if (r == -1)
            errx(EXIT_FAILURE, "read() failed.");

        // Gets the rsc (between the 5th char and the first space char).
        gchar* tmp_0 = request -> str + 5;
        gchar* tmp_1 = g_strstr_len(tmp_0, request -> len, " ");
        gchar* rsc = g_strndup(tmp_0, tmp_1 - tmp_0);
        gchar* file = g_strjoin("", "www/", rsc, NULL);

        // printf("[%i]: %s\n", client_cnx, rsc);

        // Prepares response
        g_string_append(response, "HTTP/1.1 200 OK\r\n\r\n");

        // If file exist, return contents, else return resul of command
        gchar* contents;
        gsize length;
        if (g_file_get_contents(file, &contents, &length, NULL))
        {
            g_string_append_len(response, contents, length);
            g_free(contents);
        }
        else
        {
            if (strcmp(rsc, "members") == 0)
            {
                char* nb = malloc(sizeof(gsize));
                sprintf(nb, "%lu", count_members());
                g_string_append_printf(response, "%s\n%s", nb, members -> str);
            }
            else if (strstr(rsc, "login") != NULL)
            {
                gchar* name = g_strjoin("\n",
                    g_strstr_len(rsc, tmp_1 - tmp_0, "=") + 1, "", NULL);

                printf("[%i] New connection: %s", client_cnx, name);

                g_file_get_contents("www/chat.html", &contents, &length, NULL);
                g_string_append_printf(response, contents, name);
                g_string_append(members, name);
                g_free(contents);
                g_free(name);
            }
            else
            {
                g_file_get_contents("www/login.html", &contents, &length, NULL);
                g_string_append_len(response, contents, length);
                g_free(contents);
            }
        }

        // Sends response
        offset = 0;
        while (offset < response -> len && w > 0)
        {
            w = write(client_cnx, response -> str + offset,
                    response -> len - offset);
            offset += w;
        }

        if (w == -1)
            errx(EXIT_FAILURE, "write() failed.");

        g_free(rsc);
        g_free(file);
    }

    g_string_free(request, TRUE);
    g_string_free(response, TRUE);
    free(buffer);
    close(client_cnx);

    pthread_exit(NULL);
}

// Worker for CLI thread
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

// Returns number of members
gsize count_members()
{
    gsize nb = 0;

    for (gsize i = 0; i < members -> len; i ++)
    {
        if (members -> str[i] == '\n')
            nb ++;
    }

    return nb;
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
    int client_cnx;

    members = g_string_new("");

    connect_socket(port);

    printf("%s\n%s %s\n", "Local Chat Server", "Listening to port", port);

    // start_cli();

    run = 1;
    while (run)
    {
        client_cnx = accept(cnx, rp -> ai_addr, &(rp -> ai_addrlen));
        if (client_cnx == -1)
            errx(EXIT_FAILURE, "accept() failed.");

        if (pthread_create(&client_thr, NULL, client_worker, &client_cnx) != 0)
            errx(EXIT_FAILURE, "pthread_create() failed.");
        pthread_detach(client_thr);
    }

    clean_exit();
}
