#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <optional>
#include <unordered_map>
#include <type_traits>

#include <stdio.h>

template <typename T> struct is_bool
{
	constexpr static bool value = false;
};

template <> struct is_bool<bool>
{
	constexpr static bool value = true;
};

template <typename T> struct is_stdstring
{
	constexpr static bool value = false;
};

template <> struct is_stdstring<std::string>
{
	constexpr static bool value = false;
};

class ConfigManager
{
public:
	ConfigManager(const char**);
	template <typename T> std::optional<typename std::enable_if<std::is_integral<T>::value || std::is_floating_point<T>::value, T>::type> get(const char*) const;

private:
	static void create(const std::string&);
	static std::string location();

	std::unordered_map<std::string, std::string> params;
};

template <typename T> std::optional<typename std::enable_if<std::is_integral<T>::value || std::is_floating_point<T>::value, T>::type> ConfigManager::get(const char *setting) const
{
	std::unordered_map<std::string, std::string>::const_iterator cit = params.find(setting);
	if(cit == params.end())
		return std::nullopt;

	const char *const param = cit->second.c_str();

	if constexpr(is_bool<T>::value)
	{
		if(cit->second == "true")
			return true;
		else if(cit->second == "false")
			return false;
		else
			throw std::runtime_error("Could not convert \"" + cit->second + "\" to a boolean!");
	}
	else if constexpr(std::is_integral<T>::value)
	{
		int value;
		if(sscanf(param, "%d", &value) != 1)
			throw std::runtime_error("Could not convert \"" + cit->second + "\" to an integer!");
		return value;
	}
	else if constexpr(std::is_floating_point<T>::value)
	{
		double value;
		if(scanf(param, "%f", &value) != 1)
			throw std::runtime_error("Could not convert \"" + cit->second + "\" to an integer");
		return value;
	}
	else if constexpr(is_stdstring<T>::value)
	{
		return cit->second;
	}

	std::abort();
}

#endif
