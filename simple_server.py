from flask import Flask, request

app = Flask(__name__)

@app.route('/', methods=['POST'])
def handle_post():
    if request.is_json:
        data = request.get_json()
        print("Received JSON data:")
        print(data)
        return "JSON data received!", 200
    else:
        return "Request was not JSON", 400

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=8899)
