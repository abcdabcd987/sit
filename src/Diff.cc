#include "Diff.hpp"
#include "Refs.hpp"
#include "Objects.hpp"
#include <vector>
#include <sstream>

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
		const std::string &baseID(inBase ? base.GetID(path) : Refs::EMPTY_REF);
		const std::string &targetID(inTarget ? target.GetID(path) : Refs::EMPTY_REF);
		if (inBase && !inTarget) {
			diff.insert(std::make_pair(path, DiffItem({baseID, targetID, path, Deleted})));
		} else if (!inBase && inTarget) {
			diff.insert(std::make_pair(path, DiffItem({baseID, targetID, path, Added})));
		} else if (base.GetID(path) == target.GetID(path)) {
			diff.insert(std::make_pair(path, DiffItem({baseID, targetID, path, Same})));
		} else {
			diff.insert(std::make_pair(path, DiffItem({baseID, targetID, path, Modified})));
		}
	}

	return diff;
}

std::vector<std::string> lines(const std::string &objectID, const std::string &indexID, const boost::filesystem::path &path)
{
	std::vector<std::string> v;
	if (objectID == Refs::EMPTY_REF)
		return v;
	std::istringstream ss(indexID != "work" ? Objects::GetBlob(objectID) : FileSystem::Read(path));
	std::string line;
	while (std::getline(ss, line))
		v.push_back(line);
	return v;
}
std::vector<int> linesToInts(const std::vector<std::string> &lines, const std::vector<std::string> &dict)
{
	std::vector<int> v;
	v.push_back(-1); // padding
	for (const std::string &line : lines)
		v.push_back(std::lower_bound(dict.begin(), dict.end(), line) - dict.begin());
	return v;
}
void getLCS(std::vector<std::pair<int, int>> &sol, char **pre, int i, int j)
{
	if (!i || !j) return;
	if (pre[i][j] == 'y') {
		getLCS(sol, pre, i-1, j-1);
		sol.push_back(std::make_pair(i-1, j-1));
	} else if (pre[i][j] == 'u') {
		getLCS(sol, pre, i-1, j);
	} else if (pre[i][j] == 'l') {
		getLCS(sol, pre, i, j-1);
	}
}
std::string DiffString(const DiffItem &item,
	const std::string &baseID,
	const std::string &targetID)
{
	// Convert objectIDs to two vector<int>, as input
	std::vector<std::string> base(lines(item.baseid, baseID, item.path));
	std::vector<std::string> target(lines(item.targetid, targetID, item.path));
	std::vector<std::string> dict(base);
	for (const std::string &line : target)
		dict.push_back(line);
	std::sort(dict.begin(), dict.end());
	dict.resize(std::unique(dict.begin(), dict.end()) - dict.begin());
	const std::vector<int> b(linesToInts(base, dict));
	const std::vector<int> t(linesToInts(target, dict));

	// Calculate longest common subsequence
	int **f = new int*[b.size()];
	char **pre = new char*[b.size()];
	for (auto i = 0; i < b.size(); ++i) {
		f[i] = new int[t.size()];
		pre[i] = new char[t.size()];
		memset(f[i], 0, sizeof(*f[i])*t.size());
	}
	for (auto i = 1; i < b.size(); ++i) {
		for (auto j = 1; j < t.size(); ++j) {
			if (b[i] == t[j]) {
				f[i][j] = f[i-1][j-1] + 1;
				pre[i][j] = 'y';     // upper left
			} else {
				if (f[i-1][j] < f[i][j-1]) {
					f[i][j] = f[i][j-1];
					pre[i][j] = 'l'; // left
				} else {
					f[i][j] = f[i-1][j];
					pre[i][j] = 'u'; // upper
				}
			}
		}
	}

	// Get a LCS soltuion
	std::vector<std::pair<int, int>> sol;
	getLCS(sol, pre, b.size()-1, t.size()-1);

	// Generate a diff string
	std::ostringstream out;
	out << "diff --git a/" << item.path.generic_string() << " b/" << item.path.generic_string() << std::endl
	    << baseID << " " << targetID << " 100644" << std::endl
	    << "--- a/" << item.path.generic_string() << std::endl
	    << "+++ b/" << item.path.generic_string() << std::endl;

	// TODO: simplify the diff string
	int baseEnd = base.size(), targetEnd = target.size();
	out << "@@ -1," << baseEnd << " +1," << targetEnd << " @@" << std::endl;
	sol.push_back(std::make_pair(baseEnd, targetEnd)); // padding
	base.push_back("");                                // padding
	target.push_back("");                              // padding

	int baseLast = 0, targetLast = 0;
	for (const auto &p : sol) {
		for (; baseLast < p.first; ++baseLast) 
			out << "-" << base[baseLast] << std::endl;
		for (; targetLast < p.second; ++targetLast)
			out << "+" << target[targetLast] << std::endl;
		out << " " << base[p.first] << std::endl;
		++baseLast, ++targetLast;
	}

	// Free the memory
	for (int i = 0; i < baseEnd-1; ++i) {
		delete [] f[i];
		delete [] pre[i];
	}
	delete [] f;
	delete [] pre;

	return out.str();
}

std::string DiffIndex(const std::string &baseID, const std::string &targetID)
{
	const Index::IndexBase base(Index::GetIndex(baseID));
	const Index::IndexBase target(Index::GetIndex(targetID));
	const DiffList diff(Diff(base, target));
	std::ostringstream out;
	for (const auto &item : diff) {
		if (item.second.status != Same) {
			out << DiffString(item.second, baseID, targetID);
		};
	}
	return out.str();
}

}
}