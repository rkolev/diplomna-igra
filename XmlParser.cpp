#include "xmlparser.h"

XmlParser::XmlParser(char *fileName)
{
	m_document.load_file(fileName);
}

void XmlParser::ReadText(char* nodePath, char* output, int outputSize)
{
	pugi::xpath_node node = m_document.select_single_node(nodePath);
	char* result = (char*)node.node().child_value();
	strcpy_s(output, outputSize, result);
}

bool XmlParser::ReadBool(char* nodePath)
{
	return !!ReadInt(nodePath);
}

float XmlParser::ReadFloat(char* nodePath)
{
	char output[256];
	ReadText(nodePath, output, sizeof(output));
	return static_cast<float>(atof(output));
}

int XmlParser::ReadInt(char* nodePath)
{
	char output[256];
	ReadText(nodePath, output, sizeof(output));
	return atoi(output);
}

void XmlParser::ReadTextAttr(char* nodePath, char* attr, char* output, int outputSize)
{
	pugi::xpath_node node = m_document.select_single_node(nodePath);
	char* result = (char*)node.node().attribute(attr).value();
	strcpy_s(output, outputSize, result);
}

bool XmlParser::ReadBoolAttr(char* nodePath, char* attr)
{
	return !!ReadIntAttr(nodePath, attr);
}

float XmlParser::ReadFloatAttr(char* nodePath, char* attr)
{
	char output[256];
	ReadTextAttr(nodePath, attr, output, sizeof(output));
	return static_cast<float>(atof(output));
}

int XmlParser::ReadIntAttr(char* nodePath, char* attr)
{
	char output[256];
	ReadTextAttr(nodePath, attr, output, sizeof(output));
	int result = atoi(output);
	return atoi(output);
}

ObjectDataList XmlParser::GetObjectInfo(char* nodePath)
{
	ObjectDataList objects;
	pugi::xpath_node_set result = m_document.select_nodes(nodePath);

	if (result.size() < 1) {
		return objects;
	}

	for (pugi::xpath_node_set::iterator iter = result.begin(); iter != result.end(); ++iter) {
		char* name = (char*)iter->node().attribute("name").value();
		int posX = atoi(iter->node().attribute("posX").value());
		int posY = atoi(iter->node().attribute("posY").value());
		ObjectData objectData;
		if (sizeof(name) > sizeof(objectData.name)) {
			return objects;
		}
		strcpy_s(objectData.name, name);
		objectData.posX = posX;
		objectData.posY = posY;
		objects.push_back(objectData);
	}
	return objects;
}