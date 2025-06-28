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

// loads lobbies from json [midly]
bool load_lobbies(std::vector<Lobby>& lobbies, const std::string& path)
{
	std::ifstream in(path);
	if (!in.is_open())
		return false;

	std::string line;
	Lobby current;
	while (std::getline(in, line))
	{
		size_t pos;
		if ((pos = line.find("\"name\":")) != std::string::npos)
		{
			current.name = line.substr(pos + 8);
			current.name.erase(current.name.find_last_of('"'));
			current.name.erase(std::remove(current.name.begin(), current.name.end(), '"'), current.name.end());
		}
		else if ((pos = line.find("\"version\":")) != std::string::npos)
		{
			current.version = line.substr(pos + 11);
			current.version.erase(current.version.find_last_of('"'));
			current.version.erase(std::remove(current.version.begin(), current.version.end(), '"'), current.version.end());
		}
		else if ((pos = line.find("\"hasPassword\":")) != std::string::npos)
		{
			current.hasPassword = line.find("true") != std::string::npos;
		}
		else if ((pos = line.find("\"ping\":")) != std::string::npos)
		{
			current.ping = std::stoi(line.substr(pos + 7));
		}
		else if (line.find("}") != std::string::npos)
		{
			lobbies.push_back(current);
			current = Lobby();
		}
	}

	return true;
}

// writes lobbies to json [midly]
bool write_lobbies_to_file(const std::vector<Lobby>& lobbies, const std::string& path)
{
	std::ofstream out(path);
	if (!out.is_open())
		return false;

	out << "[\n";
	for (size_t i = 0; i < lobbies.size(); ++i)
	{
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

// checks if lobby already exists to prevent dupes [midly]
bool lobby_exists(const std::vector<Lobby>& lobbies, const Lobby& lobby)
{
	for (const auto& l : lobbies)
	{
		if (l.name == lobby.name && l.version == lobby.version)
			return true;
	}
	return false;
}

// saves lobby list to a json file, e.g., lobbies.json
bool save_lobbies(const std::vector<Lobby>& lobbies, const std::string& path) {
	std::vector<Lobby> allLobbies;

	load_lobbies(allLobbies, path);

	for (const auto& lobby : lobbies) {
		if (!lobby_exists(allLobbies, lobby)) {
			allLobbies.push_back(lobby);
		}
	}

	return write_lobbies_to_file(allLobbies, path);
}