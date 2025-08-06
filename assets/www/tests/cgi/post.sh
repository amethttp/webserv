#!/bin/bash
# curl -iX POST http://localhost:8080/tests/cgi/post.sh -d "hello=this+is+a+test"
echo "HTTP/1.1 200 OK"
echo "Content-Type: text/plain"
echo ""

echo "=== Environment Variables ==="
env
echo ""

echo "=== POST Data ==="

if [ -z "$CONTENT_LENGTH" ]; then
    echo "CONTENT_LENGTH is not set or is empty"
else
    echo "Reading $CONTENT_LENGTH bytes..."
    read -n "$CONTENT_LENGTH" POST_DATA
    echo "Received data: $POST_DATA"
fi