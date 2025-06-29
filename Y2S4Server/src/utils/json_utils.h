#pragma once
#include <string>
#include <vector>

// globals [willy]
struct Lobby {
    std::string name;
    std::string version;
    bool hasPassword;
    int ping;
};

// Function declarations [willy]
bool save_lobbies(const std::vector<Lobby>& lobbies, const std::string& path);
bool load_lobbies(std::vector<Lobby>& lobbies, const std::string& path);
