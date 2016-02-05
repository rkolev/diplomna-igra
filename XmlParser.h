#pragma once

#include "Precomp.h"

class XmlParser
{
public: //methods
	//Constructor with parameters
	XmlParser(char *fileName);

	//Default constructor
	//TODO: private?
	~XmlParser() {}

	//Read value of node at @nodePath and save in @output
	void ReadText(char* nodePath, char* output, int outputSize);

	//Read boolean value at @nodePath
	bool ReadBool(char* nodePath);

	//Read float value at @nodePath
	float ReadFloat(char* nodePath);

	//Read integer value at @nodePath
	int ReadInt(char* nodePath);
	
	//Read @attr of node at @nodePath and save in @output
	void ReadTextAttr(char* nodePath, char* attr, char* output, int outputSize);

	//Read boolean @attr of node at @nodePath
	bool ReadBoolAttr(char* nodePath, char* attr);

	//Read float @attr of node at @nodePath
	float ReadFloatAttr(char* nodePath, char* attr);

	//Read integer @attr of node at @nodePath
	int ReadIntAttr(char* nodePath, char* attr);

	//Get object data saved in @nodePath
	ObjectDataList GetObjectInfo(char* nodePath);

private: //members
	pugi::xml_document m_document;
};