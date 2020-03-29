#include <fstream>
#include <iostream>
#include <vector>

#include "XML/document.h"

using namespace AngryParser;

std::vector<char> readFile(const char* path) {

	std::ifstream is(path, std::ios::binary);
	if (!is) throw Core::IOException("Cannot read file");
	is.seekg(0, std::ios::end);
	std::size_t size = static_cast<std::size_t>(is.tellg());
	is.seekg(0);
	std::vector<char> data(size + 1);
	is.read(data.data(), size);
	data[size] = 0;
	return data;

}

int main(int argc, char** argv) 
{
	auto data = readFile("d:/tree.xml");
	XML::XMLDocument document;
	document.parse<>(data.data());
	std::cout << document << std::endl;

	return 0;

}
