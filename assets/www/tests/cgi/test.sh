#!/bin/bash

body="Hello, Bash CGI works!!!"

echo -e "HTTP/1.1 200 OK\r"
echo -e "Content-Type: text/html\r"
echo -e "Content-Length: ${#body}\r"
echo -e "\r"
echo -e "$body"