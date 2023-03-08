#pragma once

#include "tinyxml2.h"

#include <string>
#include <vector>
#include <iostream>
#include <map>

class LaunchParser
{
public:
    bool parseFile(std::string launchFileFullName, std::vector<std::string>& nameVec,
        std::vector<std::string>& typeVec, std::vector<std::string>& valVec);

private:
    tinyxml2::XMLDocument doc;
};