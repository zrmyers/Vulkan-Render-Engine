#include "common.h"

using namespace std;

std::vector<char> Tools::load_binary_file_contents(std::string filename)
{
	vector<char> file;

	ifstream fin(filename);
	
	if (!fin.eof() && !fin.fail())
	{
		fin.seekg(0, ios_base::end);
		streampos fsz = fin.tellg();
		file.resize(fsz);

		fin.seekg(0, ios_base::beg);
		fin.read(&file[0], fsz);
	}

	return file;
}