from app import app
import ssl

if __name__ == "__main__":
    context = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
    context.load_cert_chain(
            certfile="certs/server.pem",
            keyfile="certs/server-key.pem",
            )
    app.run(
            host="0.0.0.0",
            port=9443,
            ssl_context = context,
            debug=False,
            )
