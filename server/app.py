from flask import Flask, request, jsonify
import logging

logging.basicConfig(
        level=logging.INFO, format="%(asctime)s [%(levelname)s] %(message)s",
        )


app = Flask(__name__)

@app.route('/data', methods=['POST'])
def receive_data():
    try:
        data = request.get_json(force=True)
        app.logger.info("Received data: %s", data)
        return jsonify({"Stattus": "ok"}), 200
    except Exception as e:
        app.logger.error("Bad request: %s", e)
        return jsonify({"Error": "invalid payload"}), 400

@app.route("/health", methods=['GET'])
def health():
    return jsonify({"Status": "healthy"}), 200
