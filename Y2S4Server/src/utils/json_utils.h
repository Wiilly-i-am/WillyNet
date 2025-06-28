#pragma once
#include <string>
#include <vector>

struct Lobby {
    std::string name;
    std::string version;
    bool hasPassword;
    int ping;
};

// Function declarations
bool save_lobbies(const std::vector<Lobby>& lobbies, const std::string& path);
bool load_lobbies(std::vector<Lobby>& lobbies, const std::string& path);
