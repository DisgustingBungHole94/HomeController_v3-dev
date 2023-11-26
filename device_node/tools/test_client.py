from socket import create_connection
import ssl
from ssl import SSLContext, PROTOCOL_TLS_CLIENT


hostname='MyHost'
ip = '127.0.0.1'
port = 4434
context = SSLContext(PROTOCOL_TLS_CLIENT)
context.load_verify_locations('../cert/fullchain.pem')
context.check_hostname = False
context.verify_mode = ssl.CERT_NONE

with create_connection((ip, port)) as client:
    # with context.wrap_socket(client, server_hostname=hostname) as tls:
    with context.wrap_socket(client, server_hostname=hostname) as tls:
        print(f'Using {tls.version()}\n')
        tls.sendall(b'GET / HTTP/1.1\r\n\r\n')

        while(True):
            data = tls.recv(1024000000)
            if not data:
                break
            print(f'Server says: {data}')