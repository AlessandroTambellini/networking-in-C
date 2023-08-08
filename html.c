#include "html.h"

void create_HTML_content(char *HTML)
{
    char *HTML_header = "HTTP/1.0 200 OK\r\nContent-Type: text/html;\r\n\r\n";
    char *HTML_body = "<h1 style=\"color: purple; text-align: center; font-family: sans-serif;\">Hello World !</h1>";

    strcpy(HTML, HTML_header);
    strcat(HTML, HTML_body);
}