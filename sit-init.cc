#include "sit-operations.h"

namespace MainFrame {
void InitRepo()
{
	try {
		boost::filesystem::create_directories(".sit");
#ifdef WIN32
		SetFileAttributes(L".sit", FILE_ATTRIBUTE_HIDDEN);
#endif
		boost::filesystem::create_directories(".sit/files");
		boost::filesystem::create_directories(".sit/stage");
		boost::filesystem::create_directories(".sit/objects");
		boost::uuids::uuid root_version = boost::uuids::nil_uuid();
	} catch (const boost::filesystem::filesystem_error &ec) {
		std::cerr << ec.what() << std::endl;
	}
}
}