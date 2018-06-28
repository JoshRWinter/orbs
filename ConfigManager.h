#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <optional>
#include <unordered_map>

class ConfigManager
{
public:
	ConfigManager(const char**);
	std::optional<std::string> get(const char*) const;

private:
	static void create(const std::string&);
	static std::string location();

	std::unordered_map<std::string, std::string> params;
};

#endif
