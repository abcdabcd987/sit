#include "Diff.hpp"
#include <vector>

namespace Sit {
namespace Diff {

DiffList Diff(const Index::IndexBase& base, const Index::IndexBase& target)
{
	std::vector<boost::filesystem::path> paths;
	for (const auto &item : base.GetIndex()) {
		paths.push_back(item.first);
	}
	for (const auto &item : target.GetIndex()) {
		paths.push_back(item.first);
	}
	paths.resize(std::unique(paths.begin(), paths.end()) - paths.begin());

	DiffList diff;
	for (const auto &path : paths) {
		const bool inBase = base.InIndex(path);
		const bool inTarget = target.InIndex(path);
		if (inBase && !inTarget) {
			diff.insert(std::make_pair(path, DiffItem({Deleted})));
		} else if (!inBase && inTarget) {
			diff.insert(std::make_pair(path, DiffItem({Added})));
		} else if (base.GetID(path) == target.GetID(path)) {
			diff.insert(std::make_pair(path, DiffItem({Same})));
		} else {
			diff.insert(std::make_pair(path, DiffItem({Modified})));
		}
	}

	return diff;
}


}
}