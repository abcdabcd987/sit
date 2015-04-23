#include "..\..\src\sit-sha1.h"
#include <map>

using namespace std;

map<string, string> sha_map;

int main(int argc, char *const argv[])
{
	if (argc > 1) {
		Util::SitSha get_sha(argv[1]);
		cout << ios::hex << get_sha.ResultStr();
		return 0;
	}
	{
		using namespace boost::filesystem;
		for (directory_iterator iter("./"), end; iter != end; ++iter) {
			if (is_regular_file(*iter)) {
				Util::SitSha get_sha(*iter);
				sha_map.insert(make_pair(iter->path().string(), get_sha.ResultStr()));
			}
		}
	}
	ofstream sha_result;
	sha_result.open("result.txt", ios::out | ios::trunc);
	for (auto v : sha_map) {
		sha_result << "{\"" << v.first << "\", \"" << v.second << "\"},\n";
	}
	sha_result.close();
	return 0;
}