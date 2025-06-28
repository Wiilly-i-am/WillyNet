#include "includes.h"
#include "json_utils.h"
#include <fstream>
#include <sstream>
#include <algorithm> // [CHANGED] for std::remove_if and std::isspace

// escape special JSON characters
std::string escape_json(const std::string& s) {
	std::string out;
	for (char c : s) {
		switch (c) {
		case '\"': out += "\\\""; break;
		case '\\': out += "\\\\"; break;
		case '\b': out += "\\b"; break;
		case '\f': out += "\\f"; break;
		case '\n': out += "\\n"; break;
		case '\r': out += "\\r"; break;
		case '\t': out += "\\t"; break;
		default: out += c; break;
		}
	}
	return out;
}

// [CHANGED] Utility to trim quotes, commas, and whitespace from string
std::string clean_json_string(const std::string& input) {
	std::string s = input;
	s.erase(std::remove_if(s.begin(), s.end(), [](unsigned char c) {
		return std::isspace(c) || c == ',' || c == '"';
		}), s.end());
	return s;
}

// load existing lobbies (simple and naive)
std::vector<Lobby> load_lobbies(const std::string& path) {
	std::vector<Lobby> lobbies;
	std::ifstream file(path);
	if (!file.is_open())
		return lobbies;

	std::string line;
	while (std::getline(file, line)) {
		if (line.find("{") != std::string::npos) {
			Lobby lobby;
			while (std::getline(file, line) && line.find("}") == std::string::npos) {
				std::string value = line.substr(line.find(":") + 1);
				value = clean_json_string(value); // [CHANGED] clean quotes, commas, whitespace

				if (line.find("name") != std::string::npos)
					lobby.name = value;
				else if (line.find("version") != std::string::npos)
					lobby.version = value;
				else if (line.find("hasPassword") != std::string::npos)
					lobby.hasPassword = (value == "true");
				else if (line.find("ping") != std::string::npos)
					lobby.ping = std::stoi(value);
			}
			lobbies.push_back(lobby);
		}
	}

	return lobbies;
}

// save lobbies (merges old + new, prevents wipe on restart)
bool save_lobbies(const std::vector<Lobby>& newLobbies, const std::string& path) {
	std::vector<Lobby> combined = load_lobbies(path);
	combined.insert(combined.end(), newLobbies.begin(), newLobbies.end());

	std::ofstream out(path);
	if (!out.is_open())
		return false;

	out << "[\n";
	for (size_t i = 0; i < combined.size(); ++i) {
		const Lobby& lobby = combined[i];
		out << "  {\n";
		out << "    \"name\": \"" << escape_json(lobby.name) << "\",\n";      // [ENSURE] raw input only
		out << "    \"version\": \"" << escape_json(lobby.version) << "\",\n"; // [ENSURE] raw input only
		out << "    \"hasPassword\": " << (lobby.hasPassword ? "true" : "false") << ",\n";
		out << "    \"ping\": " << lobby.ping << "\n";
		out << "  }";
		if (i + 1 < combined.size()) out << ",";
		out << "\n";
	}
	out << "]\n";

	return true;
}
