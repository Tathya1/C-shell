#include "man.h"


void url_encode(const char *src, char *dest, size_t max_len)
{
    static const char *hex = "0123456789ABCDEF";
    while (*src && max_len > 2)
    {
        if (isalnum(*src) || *src == '-' || *src == '_' || *src == '.' || *src == '~')
        {
            *dest++ = *src;
            max_len--;
        }
        else
        {
            *dest++ = '%';
            *dest++ = hex[(*src >> 4) & 0xF];
            *dest++ = hex[*src & 0xF];
            max_len -= 3;
        }
        src++;
    }
    *dest = '\0';
}
void fetch_man_page(const char *command_name)
{
    int sockfd;
    struct sockaddr_in server_addr;
    struct hostent *server;
    char request[BUFFER_SIZE], response[BUFFER_SIZE];
    int header_end = 0;
    char *content_start, *content_end;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("ERROR: Could not open socket");
        return;
    }

    server = gethostbyname("man.he.net");
    if (server == NULL)
    {
        fprintf(stderr, "ERROR: No such host\n");
        close(sockfd);
        return;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(80);
    memcpy(&server_addr.sin_addr.s_addr, server->h_addr_list[0], server->h_length);

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("ERROR: Could not connect to server");
        close(sockfd);
        return;
    }

    char encoded_command[BUFFER_SIZE];
    char *p_encoded_command = encoded_command;
    const char *p_command_name = command_name;

    while (*p_command_name)
    {
        if (*p_command_name == ' ')
        {
            strcpy(p_encoded_command, "%20");
            p_encoded_command += 3;
        }
        else
        {
            *p_encoded_command++ = *p_command_name;
        }
        p_command_name++;
    }
    *p_encoded_command = '\0';

    snprintf(request, sizeof(request),
             "GET /?topic=%s&section=all HTTP/1.0\r\n"
             "Host: man.he.net\r\n"
             "Connection: close\r\n\r\n",
             encoded_command);

    send(sockfd, request, strlen(request), 0);

    char body[BUFFER_SIZE * 2] = {0}; // buffer to store content
    int body_len = 0;

    while (1)
    {
        int bytes_received = recv(sockfd, response, sizeof(response) - 1, 0);
        if (bytes_received <= 0)
        {
            if (bytes_received < 0)
            {
                perror("Error reading from socket");
            }
            break; // Stop if no more data is received
        }

        response[bytes_received] = '\0'; // Null-terminate the response

        if (body_len + bytes_received < sizeof(body) - 1)
        {
            memcpy(body + body_len, response, bytes_received);
            body_len += bytes_received;
        }
        else
        {
            fprintf(stderr, "Body buffer overflow\n");
            break;
        }
    }

    close(sockfd);

    body[body_len] = '\0';

    char *ptr = body;
    while (*ptr)
    {
        if (*ptr == '<')
        {
            while (*ptr && *ptr != '>')
            {
                ptr++;
            }
            if (*ptr)
            {
                ptr++;
            }
        }
        else
        {
            putchar(*ptr++);
        }
    }
    putchar('\n');
}
