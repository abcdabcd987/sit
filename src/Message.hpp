#ifndef _SRC_MESSAGE_HPP
#define _SRC_MESSAGE_HPP

#include "Util.hpp"
#include <iostream>
#include <iomanip>


#include <string>
#include <vector>
#include <map>

namespace Sit {
namespace Msg {

class Message {
public:
	virtual void Show() = 0;
};

class HelpMessage : public Message {
	std::string name;
	std::string description;
	std::string usage;
public:
	HelpMessage();
	HelpMessage(const std::string &_name, const std::string &_description, const std::string &_usage);
	virtual void Show();
	virtual void ShowAll();
	virtual void ShowUsage();
};

class AllHelp {
	std::map<std::string, HelpMessage> data;
public:
	AllHelp();
	void ShowHelp(const std::string &content);
	void ShowUsage(const std::string &content);
};

}
}

#endif