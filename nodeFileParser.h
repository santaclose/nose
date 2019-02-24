#include <iostream>
#include <string>
#include <fstream>
#include <stdio.h>
#include "node.h"

namespace FileParser
{
	void parsePinLine(const std::string& line, std::string& a, std::string& b);
	int typeFromString(const std::string& s);
	void pushPinsFromFile(GUI::Node* node, const std::string& nodeName, int& maxInputOutputCount);
}

void FileParser::parsePinLine(const std::string& line, std::string& a, std::string& b)
{
	int i = 2;
	for (;line[i] != ':'; i++);
	a = line.substr(2, i-2);
	i += 2;
	b = line.substr(i, line.length()-i);
}

int FileParser::typeFromString(const std::string& s)
{
	if (s == "Image")
		return GUI::Pin::Image;
	else if (s == "Vector2i")
		return GUI::Pin::Vector2i;
	else if (s == "Float")
		return GUI::Pin::Float;
	else if (s == "Integer")
		return GUI::Pin::Integer;
	else if (s == "Color")
		return GUI::Pin::Color;
}

void FileParser::pushPinsFromFile(GUI::Node* node, const std::string& nodeName, int& maxInputOutputCount)
{
	using namespace std;

	bool found = false;

	bool in = false;
	bool out = false;

	string currentType;
	string a, b;

	int inputCount = 0;
	int outputCount = 0;

	ifstream inputStream("nodes.dat");
	string line;
	while (getline(inputStream, line))
	{
		if (nodeName == line)
			found = true;
		if (found)
		{
			//cout << line << endl;

			if (line[1] == '\t')
			{
				if (in == true)
				{
					parsePinLine(line, a, b);
					node->inputPins.push_back(new GUI::Pin(b, typeFromString(a), true, node));
					inputCount++;
				}
				else if (out == true)
				{
					parsePinLine(line, a, b);
					node->outputPins.push_back(new GUI::Pin(b, typeFromString(a), false, node));
					outputCount++;
				}
			}

			if (line.find("in:") != string::npos) // found
			{
				in = true;
				//cout << "input\n";
			}
			else if (line.find("out:") != string::npos) // found
			{
				in = false;
				out = true;
				//cout << "output\n";
			}

			if (line == "]")
				break;
		}

	}

	maxInputOutputCount = inputCount > outputCount ? inputCount : outputCount;

	//cout << "iph: " << inputCount << endl;
	//cout << "oph: " << outputCount << endl;
	//cout << " ph: " << pinHeight << endl;
}