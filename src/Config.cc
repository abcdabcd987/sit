#include <iostream>
#include <boost/algorithm/string.hpp>
#include "FileSystem.hpp"
#include "Config.hpp"
namespace Sit {
namespace Config {

const std::string NOT_FOUND("\0<CONFIG_NOT_FOUND>\0");

Config readConfig()
{
	const auto config_path = FileSystem::REPO_ROOT / FileSystem::SIT_ROOT / "config";
	if (!FileSystem::IsFile(config_path)) {
		throw Util::SitException(std::string("Cannot read configuration file."));
	}
	boost::filesystem::ifstream fin(config_path);
	std::string line;
	Config config;
	while (std::getline(fin, line)) {
		std::string::size_type col = 0;
		while (col < line.size() && line[col] != ':') ++col;
		if (col != line.size()) {
			std::string key(line.substr(0, col));
			std::string value(line.substr(col+1));
			boost::trim(key);
			boost::trim(value);
			config[key] = value;
		} else {
			std::cerr << "Warning: In configuration file, skipping line: " << line << std::endl;
		}
	}
	return config;
}

void writeConfig(const Config& config)
{
	std::ostringstream ss;
	for (auto &entry : config) {
		ss << entry.first << ": " << entry.second << "\n";
	}
	const auto config_path = FileSystem::REPO_ROOT / FileSystem::SIT_ROOT / "config";
	FileSystem::Write(config_path, ss.str());
}

std::string Get(const std::string& key)
{
	const Config config(readConfig());
	auto iter = config.find(key);
    return iter != config.end() ? iter->second : NOT_FOUND;
}

void Set(const std::string& key, const std::string& value)
{
	Config config(readConfig());
	config[key] = value;
	writeConfig(config);
}

}
}