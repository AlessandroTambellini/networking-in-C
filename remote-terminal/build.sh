#!/bin/bash

gcc -std=c11 -Wall -Wextra server.c common.c -o server.out

gcc -std=c11 -Wall -Wextra client.c common.c -o client.out

chmod +x server.out client.out
