from flask import Flask, request, jsonify
import json
import time
import os
# backend shizzle

app = Flask(__name__)
LOBBY_FILE = "lobbies.json"
LOBBY_EXPIRY_SECONDS = 60 # clean lobbies after 1 minute

def load_lobbies():
    if not os.path.exists(LOBBY_FILE):
        return[]
    with open(LOBBY_FILE, "r") as f:
        try:
            return json.load(f)
        except:
            return[]
        
def save_lobbies(lobbies):
    with open(LOBBY_FILE, "w") as f:
        json.dump(lobbies, f, indent=2)

def cleanup_lobbies():
    now = time.time()
    lobbies = load_lobbies()
    lobbies = [l for l in lobbies if now -l["timestamp"] < LOBBY_EXPIRY_SECONDS]
    save_lobbies(lobbies)

@app.route("/lobbies.json", methods=["GET"])
def get_lobbies():
    cleanup_lobbies()
    return jsonify(load_lobbies())

@app.route("/upload", methods=["POST"])
def post_lobby():
    lobby = request.json
    if not lobby or "name" not in lobby or "encrypted_host" not in lobby:
        return jsonify({"error": "Missing fields"}), 400
    
    lobby["timestamp"] = time.time()
    lobbies = load_lobbies()

    #replace if lobby from same host alr exists
    lobbies = [l for l in lobbies if l["encrypted_host"] != lobby["encrypted_host"]]
    lobbies.append(lobby)
    save_lobbies(lobbies)

    return jsonify({"status": "ok"}), 200

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000)