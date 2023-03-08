#include "launchparser.h"

int main()
{
    LaunchParser launchparser;
    std::vector<std::string> tagList, typeList, valList;
    std::string argv_str = "C:\\dev\\launchparser\\launch\\sick_lrs_36x1.launch";
    if (true)
    {
        bool ret = launchparser.parseFile(argv_str, tagList, typeList, valList);
        if (ret == false)
        {
            std::cout << "Cannot parse launch file (check existence and content): >>>" << argv_str << "<<<\n";
            exit(-1);
        }
        for (size_t i = 0; i < tagList.size(); i++)
        {
            printf("%-30s %-10s %-20s\n", tagList[i].c_str(), typeList[i].c_str(), valList[i].c_str());
        }
    }
}