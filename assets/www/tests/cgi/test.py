body = """
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>CGI Test</title>
</head>
<body>
    <h1>Hello from CGI!</h1>
    <p>This is a simple CGI script written in Python.</p>
</body>
</html>
"""

print("HTTP/1.1 200 OK\r")
print("Content-Type:text/html\r")
print("Content-Length:" + repr(len(body)) + "\r")
print("\r")
print(body)