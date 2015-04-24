#include "sit-operations.h"

#include <sstream>
#include <string>

namespace MainFrame {
void InitRepo()
{
	try {
		/**
		 * Create sit repository directories
		 */
		boost::filesystem::create_directories(".sit");
#ifdef WIN32
		SetFileAttributes(L".sit", FILE_ATTRIBUTE_HIDDEN);
#endif
		boost::filesystem::create_directories(".sit/files");
		boost::filesystem::create_directories(".sit/stage");
		boost::filesystem::create_directories(".sit/objects");

		boost::uuids::uuid rootVersion = boost::uuids::nil_uuid();
		std::ostringstream uuidStr;
		uuidStr << rootVersion;
		std::string rootUuidStr = uuidStr.str();

		boost::filesystem::ofstream object(objects_dir / rootUuidStr);
		boost::archive::binary_oarchive boarch(object);

		boarch << rootVersion;
		boarch << static_cast<const boost::posix_time::ptime>(boost::posix_time::microsec_clock::universal_time());
		

		boost::filesystem::ofstream headFile(objects_dir / "HEAD");
		boost::archive::binary_oarchive headBoArch(headFile);

		boost::filesystem::ofstream branchMasterFile(objects_dir / "master");
		boost::archive::binary_oarchive masterBoArch(branchMasterFile);

		masterBoArch << rootVersion;

		headBoArch << static_cast<const std::string>(std::string("master"));
	} catch (const boost::filesystem::filesystem_error &ec) {
		std::cerr << ec.what() << std::endl;
	} catch (const boost::archive::archive_exception &ae) {
		std::cerr << ae.what() << std::endl;
	} catch (const std::exception &se) {
		std::cerr << se.what() << std::endl;
	}
}
}