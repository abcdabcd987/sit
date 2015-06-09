#include "Diff.hpp"
#include "Refs.hpp"
#include "Objects.hpp"
#include "MurmurHash3.hpp"
#include "Util.hpp"
#include <cassert>
#include <vector>
#include <sstream>
#include <tuple>
#include <iostream>

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
		const std::string &baseID(inBase ? base.GetID(path) : Objects::EMPTY_OBJECT);
		const std::string &targetID(inTarget ? target.GetID(path) : Objects::EMPTY_OBJECT);
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

//
// Diff Algorithm
// See "An O(ND) Difference Algorithm and its Variations", by Eugene Myers.
//

template <class T>
class array {
	int lo, hi;
	T * const data;
	bool allocated;
	array(int l, int h, T *dt)
		: lo(l), hi(h), data(dt), allocated(false) {}
public:
	array(int l, int h)
		: lo(l), hi(h), data(new T[hi-lo+1]), allocated(true) {}
	array(int l, const std::vector<T> &v)
		: lo(l), hi(l+v.size()-1), data(new T[hi-lo+1]), allocated(true) {
		T *p = data;
		for (const T& item : v) {
			*p++ = item;
		}
	}
	const T& operator[](int i) const {
		assert(lo <= i && i <= hi);
		return data[i-lo];
	}
	T& operator[](int i) {
		assert(lo <= i && i <= hi);
		return data[i-lo];
	}
	array splice(int l, int r) const {
		assert(lo <= l && r <= hi);
		return array(1, r-l+1, data+l-lo);
	}
	int size() const {
		return hi-lo+1;
	}
	~array() {
		if (allocated) {
			delete [] data;
		}
	}
};

struct snake {
	int x, y, u, v, d;
};
snake middle_snake(const array<HashValue> &a, const array<HashValue> &b) {
	const int n = a.size();
	const int m = b.size();
	const int limd = (m+n+1)/2; // ceil((m+n)/2)
	const int delta = n-m;	  // center diag for reverse D-path
	const bool odd = delta & 1;
	array<int> vf(-n-m, n+m);   // for furthest reaching forward D-path in diag k
	array<int> vr(delta+(-n-m), delta+(n+m));   // for furthest reaching reverse D-path in diag k
	vf[1] = 0;		// at (0, -1)
	vr[delta-1] = n;  // at (n, m+1)
	for (int d = 0; d <= limd; ++d) {
		// forward, diag 0 centered
		for (int k = -d, x, y; k <= d; k += 2) {
			// find the end of the furthest reaching forward D-path in diag k
			if (k == -d || (k != d && vf[k-1] < vf[k+1])) {
				x = vf[k+1];		// go downward
			} else {
				x = vf[k-1] + 1;	// go rightward
			}
			y = x-k;
			while (x < n && y < m && a[x+1] == b[y+1]) ++x, ++y;
			vf[k] = x;

			// check overlap
			if (odd && delta-(d-1) <= k && k <= delta+(d-1)) {
				const int u = vr[k], v = u-k;
				if (u+v >= d && x+y <= n+m-(d-1) && x >= u) {
					return snake({u, v, x, y, 2*d-1});
				}
			}
		}

		// reverse, diag delta centered
		for (int k = -d, u, v; k <= d; k += 2) {
			// find the end of the furthest reaching reverse D-path in diag delta+k
			if (k == d || (k != -d && vr[delta+k-1] < vr[delta+k+1])) {
				u = vr[delta+k-1];	  // go upward
			} else {
				u = vr[delta+k+1] - 1;  // go leftward
			}
			v = u-(delta+k);
			while (u > 0 && v > 0 && a[u] == b[v]) --u, --v;
			vr[delta+k] = u;

			// check overlap
			if (!odd && -d <= k+delta && k+delta <= d) {
				const int x = vf[delta+k], y = x-(delta+k);
				if (u+v >= d && x+y <= n+m-d && x >= u) {
					return snake({u, v, x, y, 2*d});
				}
			}
		}
	}
	// shouldn't be here
	assert(false);
	return snake();
}
void lcs(std::vector<std::pair<int, int>> &out,
		const array<HashValue> &a, const int aoff,
		const array<HashValue> &b, const int boff) {
	const int n = a.size(), m = b.size();
	if (n > 0 && m > 0) {
		snake s(middle_snake(a, b)); // (x,y) to (u,v)
		if (s.d > 1) {
			lcs(out, a.splice(1, s.x), aoff, b.splice(1, s.y), boff);
			for (int i = s.x+1; i <= s.u; ++i) out.push_back(std::make_pair(aoff+i-1, boff-s.x+s.y+i-1));
			lcs(out, a.splice(s.u+1, n), aoff+s.u, b.splice(s.v+1, m), boff+s.v);
		} else if (n < m) {
			for (int i = 1, j = 1; i <= n && j <= m; ++i, ++j) {
				if (a[i] != b[j]) ++j;
				out.push_back(std::make_pair(aoff+i-1, boff+j-1));
			}
		} else {
			for (int i = 1, j = 1; i <= n && j <= m; ++i, ++j) {
				if (a[i] != b[j]) ++i;
				out.push_back(std::make_pair(aoff+i-1, boff+j-1));
			}
		}
	}
}
//
// Diff Algorithm End
//


std::vector<std::string> lines(const std::string &objectID, const std::string &indexID, const boost::filesystem::path &path)
{
	std::vector<std::string> v;
	if (objectID == Objects::EMPTY_OBJECT)
		return v;
	std::istringstream ss(indexID != "work" ? Objects::GetBlob(objectID) : FileSystem::Read(path));
	std::string line;
	while (std::getline(ss, line))
		v.push_back(line);
	return v;
}
void hashlines(array<HashValue> &out, const std::vector<std::string> &v)
{
	for (size_t i = 0; i < v.size(); ++i)
		out[i+1] = MurmurHash3(v[i].c_str(), v[i].size());
}
void DiffObject(std::ostream &out, const DiffItem &item,
	const std::string &baseID,
	const std::string &targetID)
{
	// Convert objectIDs to two array<HashValue>, as input
	std::vector<std::string> base(lines(item.baseid, baseID, item.path));
	std::vector<std::string> target(lines(item.targetid, targetID, item.path));
	array<HashValue> b(1, base.size()), t(1, target.size());
	hashlines(b, base);
	hashlines(t, target);

	// Calculate longest common subsequence
	std::vector<std::pair<int, int>> sol;
	lcs(sol, b, 0, t, 0);

	// Generate a diff string
	out << Color::WHITE << "diff --git a/" << item.path.generic_string() << " b/" << item.path.generic_string() << Color::RESET << std::endl
	    << Color::WHITE << baseID << " " << targetID << " 100644" << Color::RESET << std::endl
	    << Color::WHITE << "--- a/" << item.path.generic_string() << Color::RESET << std::endl
	    << Color::WHITE << "+++ b/" << item.path.generic_string() << Color::RESET << std::endl;

	int baseEnd = base.size(), targetEnd = target.size();

	sol.push_back(std::make_pair(baseEnd, targetEnd)); // padding

	std::vector<std::tuple<char, std::string, unsigned, unsigned>> diffLines;
	int baseLast = 0, targetLast = 0;
	for (const auto &p : sol) {
		for (; baseLast < p.first; ++baseLast)
			diffLines.push_back(std::make_tuple('-', base[baseLast], baseLast, targetLast));
		for (; targetLast < p.second; ++targetLast)
			diffLines.push_back(std::make_tuple('+', target[targetLast], baseLast, targetLast));
		if (p.first != baseEnd)
			diffLines.push_back(std::make_tuple('=', base[p.first], baseLast, targetLast));
		++baseLast, ++targetLast;
	}
	std::vector<std::vector<unsigned>> diffBlob;

	for (size_t i = 0; i < diffLines.size(); ++i) {
		if (std::get<0>(diffLines[i]) != '=') {
			if (std::get<0>(diffLines[i]) == '+') {
				if (diffBlob.empty()) {
					diffBlob.push_back(std::vector<unsigned>());
				}
				diffBlob.back().push_back(i);
			} else {
				if (diffBlob.empty()) {
					diffBlob.push_back(std::vector<unsigned>());
				}
				diffBlob.back().push_back(i);
			}
		} else {
			if ((i >= 1 && std::get<0>(diffLines[i - 1]) != '=')
				|| (i >= 2 && std::get<0>(diffLines[i - 2]) != '=')
				|| (i + 1 < diffLines.size() && std::get<0>(diffLines[i + 1]) != '=')
				|| (i + 2 < diffLines.size() && std::get<0>(diffLines[i + 2]) != '=')) {
				if (diffBlob.empty()) {
					diffBlob.push_back(std::vector<unsigned>());
				}
				diffBlob.back().push_back(i);
			} else {
				if (diffBlob.empty()) {
					diffBlob.push_back(std::vector<unsigned>());
				}
				if (!diffBlob.back().empty()) {
					diffBlob.push_back(std::vector<unsigned>());
				}
			}
		}
	}
	
	for (const auto &vec : diffBlob) {
		if (vec.empty()) {
			break;
		}
		const int st1 = std::get<2>(diffLines[vec.front()]) + 1;
		const int ed1 = std::get<2>(diffLines[vec.back()]) + 1;
		const int st2 = std::get<3>(diffLines[vec.front()]) + 1;
		const int ed2 = std::get<3>(diffLines[vec.back()]) + 1;
		out << Color::CYAN 
			<< "@@ -" << st1 << "," << ed1-st1+1
			<< " +" << st2 << "," << ed2-st2+1 
			<< " @@" << Color::RESET << std::endl;
		for (const auto &x : vec) {
			auto status = std::get<0>(diffLines[x]);
			auto str = std::get<1>(diffLines[x]);
			if (status == '-') {
				out << Color::RED << "-" << str << Color::RESET << std::endl;
			} else if (status == '+') {
				out << Color::GREEN << "+" << str << Color::RESET << std::endl;
			} else if (status == '=') {
				out << " " << str << std::endl;
			}
		}
		out << std::endl;
	}
	
}

void DiffIndex(std::ostream &out, const std::string &baseID, const std::string &targetID)
{
	const Index::IndexBase base(Index::GetIndex(baseID));
	const Index::IndexBase target(Index::GetIndex(targetID));
	const DiffList diff(Diff(base, target));
	for (const auto &item : diff) {
		if (item.second.status != Same) {
			DiffObject(out, item.second, baseID, targetID);
		};
	}
}

}
}
