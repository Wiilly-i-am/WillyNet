from flask import Flask, request, jsonify
import time

app = Flask(__name__)
lobbies = {}

@app.route("/register", methods=["POST"])
def register():
    data = request.json
    lobby_id = data.get("id")
    lobbies[lobby_id] = {
        "ip": request.remote_addr,
        "name": data.get("name"),
        "version": data.get("version"),
        "has_password": data.get("has_password", False),
        "timestamp": time.time()
    }
    return jsonify({"status": "ok"})

@app.route("/lobbies", methods=["GET"])
def get_lobbies():
    # remove stale lobbies after 30 seconds
    now = time.time()
    for k in list(lobbies):
        if now - lobbies[k]["timestamp"] > 30:
            del lobbies[k]
    return jsonify(lobbies)

@app.route("/ping/<lobby_id>", methods=["POST"])
def ping(lobby_id):
    if lobby_id in lobbies:
        lobbies[lobby_id]["timestamp"] = time.time()
        return jsonify({"status": "refreshed"})
    return jsonify({"status": "not_found"}), 404

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000)
