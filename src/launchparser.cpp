#include "launchparser.h"

class paramEntryAscii
{
public:
    paramEntryAscii(std::string _nameVal, std::string _typeVal, std::string _valueVal)
    {
        nameVal = _nameVal;
        typeVal = _typeVal;
        valueVal = _valueVal;
        setCheckStatus(999, "untested");
        minMaxGiven = false;
    };

    void setPointerToXmlNode(tinyxml2::XMLElement* paramEntryPtr)
    {
        this->nodePtr = paramEntryPtr;
    }

    tinyxml2::XMLElement* getPointerToXmlNode(void)
    {
        return(this->nodePtr);
    }
    void setValues(std::string _nameVal, std::string _typeVal, std::string _valueVal)
    {
        nameVal = _nameVal;
        typeVal = _typeVal;
        valueVal = _valueVal;
    };


    bool isMinMaxGiven()
    {
        return(minMaxGiven);
    }

    void setMinMaxValues(std::string _valueMinVal, std::string _valueMaxVal)
    {

        valueMinVal = _valueMinVal;
        valueMaxVal = _valueMaxVal;
        minMaxGiven = true;

    };

    std::string getName()
    {
        return(nameVal);
    }

    std::string getType()
    {
        return(typeVal);
    }

    std::string getValue()
    {
        return(valueVal);
    }

    std::string getMinValue()
    {
        return(valueMinVal);
    }

    std::string getMaxValue()
    {
        return(valueMaxVal);
    }

    void setCheckStatus(int errCode, std::string errMsg)
    {
        errorCode = errCode;
        errorMsg = errMsg;
    };

    int getErrorCode()
    {
        return(errorCode);
    }

    std::string getErrorMsg()
    {
        return(errorMsg);
    }

private:
    std::string nameVal;
    std::string typeVal;
    std::string valueVal;
    std::string valueMinVal;
    std::string valueMaxVal;
    bool minMaxGiven;
    int errorCode;
    std::string errorMsg;
    tinyxml2::XMLElement* nodePtr;
};

std::vector<paramEntryAscii> getParamList(tinyxml2::XMLNode* paramList)
{
    std::vector<paramEntryAscii> tmpList;


    tinyxml2::XMLElement* paramEntry = (tinyxml2::XMLElement*)paramList->FirstChildElement("param"); // first child
    while (paramEntry)
    {
        std::string nameVal = "";
        std::string typeVal = "";
        std::string valueVal = "";
        std::string minValueVal = "";
        std::string maxValueVal = "";

        bool minValFnd = false;
        bool maxValFnd = false;
        // is this a param-node?
        // if this is valid than process attributes
        const char* entryVal = paramEntry->Value();
        bool searchAttributes = true;
        if (strcmp(entryVal, "param") == 0)
        {
            // expected value
        }
        else
        {
            searchAttributes = false;
        }
        if (paramEntry->ToElement())
        {
            // expected value
        }
        else
        {
            searchAttributes = false;
        }
        if (searchAttributes)
        {
            //for (auto node = paramEntry->FirstAttribute(); ; node = node->Next())
            for (const tinyxml2::XMLAttribute* node = paramEntry->FirstAttribute(); node!=0; node = node->Next())
            {
                const char* tag = node->Name();
                const char* val = node->Value();

                if (strcmp(tag, "name") == 0)
                {
                    nameVal = val;
                }
                if (strcmp(tag, "type") == 0)
                {
                    typeVal = val;
                }
                if (strcmp(tag, "value") == 0)
                {
                    valueVal = val;
                }
                if (strcmp(tag, "valueMin") == 0)
                {
                    minValFnd = true;
                    minValueVal = val;

                }
                if (strcmp(tag, "valueMax") == 0)
                {
                    maxValFnd = true;
                    maxValueVal = val;
                }
            }

            paramEntryAscii tmpEntry(nameVal, typeVal, valueVal);
            if (maxValFnd && minValFnd)
            {
                tmpEntry.setMinMaxValues(minValueVal, maxValueVal);
            }

            tmpEntry.setPointerToXmlNode(paramEntry);
            tmpList.push_back(tmpEntry);
        }
        paramEntry = (tinyxml2::XMLElement*)paramEntry->NextSibling();  // go to next sibling
    }

    return(tmpList);
}

bool LaunchParser::parseFile(std::string launchFileFullName, std::vector<std::string>& nameVec,
    std::vector<std::string>& typeVec, std::vector<std::string>& valVec)
{
    bool ret = false;
    
    std::cout << "Try loading launchfile : " << launchFileFullName << std::endl;

    doc.LoadFile(launchFileFullName.c_str());

    if (doc.Error() == true)
    {
        std::cout << "## ERROR parsing launch file " << doc.ErrorName() << std::endl;
        return(ret);
    }
    tinyxml2::XMLNode* node = doc.FirstChildElement("launch");
    if (node != NULL)
    {
        std::map<std::string, std::string> default_args;
        tinyxml2::XMLElement* arg_node = (tinyxml2::XMLElement*)node->FirstChildElement("arg");
        while (arg_node)
        {
            if (strcmp(arg_node->Value(), "arg") == 0 && arg_node->ToElement())
            {
                // parse default arguments, f.e. <arg name="hostname" default="192.168.0.1"/>
                const char* p_attr_name = arg_node->Attribute("name");
                const char* p_attr_default = arg_node->Attribute("default");
                if (p_attr_name && p_attr_default)
                {
                    std::string attr_name(p_attr_name), attr_default(p_attr_default);
                    default_args[attr_name] = attr_default;
                    std::cout << "LaunchParser::parseFile(" << launchFileFullName << "): default_args[\"" << attr_name << "\"]=\"" << default_args[attr_name] << "\"" << std::endl;
                }
            }
            arg_node = (tinyxml2::XMLElement*)arg_node->NextSibling();  // go to next sibling
        }
        // parse all node specific parameters
        node = node->FirstChildElement("node");
        std::vector<paramEntryAscii> paramOrgList = getParamList(node);

        for (size_t j = 0; j < paramOrgList.size(); j++)
        {
            nameVec.push_back(paramOrgList[j].getName());
            typeVec.push_back(paramOrgList[j].getType());
            valVec.push_back(paramOrgList[j].getValue());
            if (valVec.back().substr(0, 6) == "$(arg ") // overwrite with default argument, f.e. name="hostname", type="string", value="$(arg hostname)"
            {
                std::string default_arg_name = valVec.back().substr(6, valVec.back().length() - 1 - 6);
                if (default_args.find(default_arg_name) != default_args.end())
                {
                    std::string default_arg_val = default_args[default_arg_name];
                    std::cout << "LaunchParser::parseFile(" << launchFileFullName << "): name=\"" << nameVec.back() << "\", type=\"" << typeVec.back() << "\", value=\"" << valVec.back()
                        << "\" overwritten by default value \"" << default_arg_val << "\"" << std::endl;
                    valVec.back() = default_arg_val;
                }
            }
            std::cout << "LaunchParser::parseFile(" << launchFileFullName << "): name=\"" << nameVec.back() << "\", type=\"" << typeVec.back() << "\", value=\"" << valVec.back() << "\"" << std::endl;
        }

        ret = true;

    }

    return(ret);
}
