#include "Refs.hpp"
#include "FileSystem.hpp"
#include "Util.hpp"
#include <boost/algorithm/string.hpp>

namespace Sit {
namespace Refs {

const std::string EMPTY_REF("0000000000000000000000000000000000000000");

std::string Local(const std::string& name)
{
	return std::string("refs/heads/") + name;
}

std::string Get(const std::string& path)
{
	const auto filepath(FileSystem::REPO_ROOT / FileSystem::SIT_ROOT / path);
	if (!FileSystem::IsFile(filepath)) {
		throw Util::SitException(std::string("Ref not exist: ") + path);
	}
	std::string id(FileSystem::Read(filepath));
	boost::trim(id);
	return id;
}

void Set(const std::string& ref, const std::string& id)
{
	const auto path(FileSystem::REPO_ROOT / FileSystem::SIT_ROOT / ref);
	FileSystem::Write(path, id);
}

}
}