#pragma once
#include "json.hpp"
#include <string>
#include <fstream>
#include "World.h"

using namespace nlohmann;

class SceneManager
{
private:
	json data;

public:
	SceneManager(std::string filename);
	void LoadScenes(int SceneId);
	void SetupObjects(const json & objects, int ObjectNum);
	void DumpScenes(std::string filename);
	~SceneManager();
};

