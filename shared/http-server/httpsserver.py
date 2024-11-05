from http.server import HTTPServer, SimpleHTTPRequestHandler
import ssl

# 创建 HTTP 服务器
httpd = HTTPServer(('192.168.3.14', 7981), SimpleHTTPRequestHandler)

# 创建 SSL 上下文
context = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
context.load_cert_chain(certfile="./fullchain.pem", keyfile="./privkey.pem")  # 加载证书和密钥

# 将 SSL 上下文应用于服务器的 socket
httpd.socket = context.wrap_socket(httpd.socket, server_side=True)

# 启动服务器
print("Starting HTTPS server on port 7981...")
httpd.serve_forever()
