#pragma once

#ifndef UNIX
#include <filesystem>
#endif
#include <jce/string.hpp>
#include <string>

void createFolder(std::string str)
{
	createFolder(str.c_str());
}

void createFolder(char name[])
{
#ifndef UNIX
	std::filesystem::create_directories(name);
#else
	const char* foo = "mkdir -p ";
	char* full_text = new char[100];
	strcpy(full_text, foo);
	strcat(full_text, name);
	system(full_text);
#endif
}

void inline createFolders()
{
	char folder[100];

	sprintf(folder, "models/run_%d", runVersion);
	createFolder(folder);

	sprintf(folder, "memory/run_%d", runVersion);
	createFolder(folder);

	sprintf(folder, "logs/c++");
	createFolder(folder);

	sprintf(folder, "logs/games");
	createFolder(folder);
}