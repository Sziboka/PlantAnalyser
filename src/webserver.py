import logging
from flask import Flask, request

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='[%(asctime)s] %(levelname)s in %(module)s: %(message)s'
)

app = Flask(__name__)

@app.route('/', methods=['POST'])
def handle_post():
    data = request.get_json(force=True)
    app.logger.info(f"Received POST data: {data}")
    app.logger.info(f"Headers: {dict(request.headers)}")
    return 'OK', 200

@app.route('/', methods=['GET'])
def handle_get():
    app.logger.info("Received GET request")
    app.logger.info(f"Headers: {dict(request.headers)}")
    return 'Hello, World!', 200

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=9090)