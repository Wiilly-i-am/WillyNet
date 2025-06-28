#include "includes.h"
#include "json_utils.h"

// escapes json special characters in a string
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

// saves lobby list to a json file, e.g., lobbies.json
bool save_lobbies(const std::vector<Lobby>& lobbies, const std::string& path) {
	std::ofstream out(path);
	if (!out.is_open())
		return false;

	out << "[\n";
	for (size_t i = 0; i < lobbies.size(); ++i) {
		const Lobby& lobby = lobbies[i];
		out << "  {\n";
		out << "    \"name\": \"" << escape_json(lobby.name) << "\",\n";
		out << "    \"version\": \"" << escape_json(lobby.version) << "\",\n";
		out << "    \"hasPassword\": " << (lobby.hasPassword ? "true" : "false") << ",\n";
		out << "    \"ping\": " << lobby.ping << "\n";
		out << "  }";
		if (i + 1 < lobbies.size()) out << ",";
		out << "\n";
	}
	out << "]\n";

	out.close();
	return true;
}