#include "FileCombinator.h"

int main(int, char ** argv) {
	typedef std::list<std::string> List;
	typedef List::iterator It;

	std::string archiveName = argv[1];
	archiveName += ".bin";

	List fileNameList;
	Archive::enumerateFiles(fileNameList, argv[1]);
	int numFile = static_cast<int>(fileNameList.size());
	std::vector<std::string> fileNames(fileNameList.begin(), fileNameList.end());

	//	アーカイブ生成
	Archive::create(fileNames, archiveName.c_str());

	printf("Complete");

#ifndef NDEBUG 
	while (true) { ; } 
#endif
	return 0;
}