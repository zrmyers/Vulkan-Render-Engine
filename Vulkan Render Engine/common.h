#pragma once
#include <vulkan\vulkan.h>
#include <GLFW\glfw3.h>

#include <fstream>
#include <iostream>
#include <vector>
#include <string>

namespace Tools
{
	std::vector<char> load_binary_file_contents(std::string);
}