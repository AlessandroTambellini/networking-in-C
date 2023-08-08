#include <stdio.h>
#include <string.h>

#include "html.h"

void create_HTML_content(char *HTML)
{
    char *HTML_header = "HTTP/1.0 200 OK\r\nContent-Type: text/html;\r\n\r\n";
    strcpy(HTML, HTML_header);

    FILE *HTML_p = fopen("client/index.html", "r");
    char *text_line;
    while (text_line != NULL) {
        text_line = fgets(HTML + strlen(HTML), HTML_SIZE - strlen(HTML), HTML_p);
    }

    fclose(HTML_p);
}