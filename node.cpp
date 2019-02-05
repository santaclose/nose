#include <string>
#include <iostream>
#include <SFML/Graphics.hpp>
#include "node.h"

#define TEXT_COLOR 0xffffff
#define BAR_COLOR 0x2f2fbb
#define CONTENT_RECT_COLOR 0x4444bb
#define INTERACTIVE_COMPONENT_COLOR 0x24249b

#define NODE_TITLE_FONT_SIZE 14
#define PIN_TEXT_FONT_SIZE 11
#define INTERACTIVE_COMPONENT_TEXT_FONT_SIZE 12

#define BAR_HEIGHT 25
#define PROPERTY_HEIGHT 25
#define NODE_WIDTH 180
#define CONTENT_MARGIN_TOP 10
#define INTERACTIVE_COMPONENT_HEIGHT 28

#define TEXT_MARGIN_TOP 4
#define TEXT_MARGIN_LEFT 10

#define PIN_RECT_SIZE 5
#define PIN_RECT_MARGIN 8

#define PIN_TEXT_MARGIN_X 15
#define PIN_TEXT_MARGIN_Y -5
#define CHARACTER_WIDTH 7


/////////// generic

bool GUI::isVectorOverRect(const sf::Vector2f& vector, const sf::RectangleShape& rect)
{
	sf::Vector2f pos = rect.getPosition();
	sf::Vector2f size = rect.getSize();

	return vector.x > pos.x && vector.x < pos.x + size.x && vector.y > pos.y && vector.y < pos.y + size.y;
}

bool GUI::isVectorOverRect(const sf::Vector2f& vector, const sf::Vector2f& rectPosition, const sf::Vector2f& rectSize)
{
	return vector.x > rectPosition.x && vector.x < rectPosition.x + rectSize.x && vector.y > rectPosition.y && vector.y < rectPosition.y + rectSize.y;
}


/////////// pins

void GUI::Pin::assignColor(const int inputType, sf::RectangleShape& rect)
{
	switch (inputType)
	{
		case GUI::Pin::Integer:
			rect.setFillColor(sf::Color(0x4ca4fbff));
			break;
		case GUI::Pin::Float:
			rect.setFillColor(sf::Color(0xee1133ff));
			break;
		case GUI::Pin::Vector2i:
			rect.setFillColor(sf::Color(0x56957aff));
			break;
		/*case GUI::Pin::Recti:
			rect.setFillColor(sf::Color(0x324afbff));
			break;*/
		case GUI::Pin::Image:
			rect.setFillColor(sf::Color(0x00bc44ff));
			break;
		case GUI::Pin::Color:
			rect.setFillColor(sf::Color(0xaa7700ff));
			break;

	}
}

GUI::Pin::Pin(const std::string& name, const int type, const bool isInput, const sf::Font& font, Node* parentNode)
{
	this->parentNode = parentNode;
	this->type = type;
	this->isInput = isInput;
	this->data = nullptr;

	if (!isInput)
		this->stringOutputOffsetX = name.length() * CHARACTER_WIDTH;

	this->rect = sf::RectangleShape(sf::Vector2f(PIN_RECT_SIZE, PIN_RECT_SIZE));
	assignColor(type, this->rect);

	this->text.setFillColor(sf::Color(TEXT_COLOR));
	this->text.setFont(font);
	this->text.setCharacterSize(PIN_TEXT_FONT_SIZE);
	this->text.setString(name);
}
GUI::Pin::~Pin()
{
	switch(type)
	{
		case GUI::Pin::Integer:
			delete (int*)data;
			break;
		case GUI::Pin::Float:
			delete (float*)data;
			break;
		case GUI::Pin::Vector2i:
			delete (sf::Vector2i*)data;
			break;
		case GUI::Pin::Image:
			delete (sf::Texture*)data;
			break;
		case GUI::Pin::Color:
			delete (sf::Color*)data;
			break;
	}
}

bool GUI::Pin::isMouseOver(sf::Vector2f& mousePos)
{
	sf::Vector2f size = rect.getSize();
	sf::Vector2f pos = rect.getPosition();
	pos -= size;
	size = sf::Vector2f(size.x * 3, size.y * 3);
	if (isVectorOverRect(mousePos, pos, size))
		return true;
	return false;
}

bool GUI::Pin::isDisconnected()
{
	return connectionVertices.empty();
}

void GUI::Pin::establishConnection(sf::Vertex* newConnectionVertex, GUI::Pin* otherPin)
{
	this->connectionVertices.push_back(newConnectionVertex);
	this->connectedPins.push_back(otherPin);
}

sf::Vector2f GUI::Pin::getRectCenter()
{
	sf::Vector2f size = rect.getSize();
	return this->rect.getPosition() + sf::Vector2f(size.x / 2, size.y / 2);
}

void GUI::Pin::disconnectFrom(GUI::Pin*& p)
{
	for (int i = 0; i < this->connectedPins.size(); i++)
	{
		if (this->connectedPins[i] == p)
		{
			this->connectedPins.erase(this->connectedPins.begin() + i);
			this->connectionVertices.erase(this->connectionVertices.begin() + i);
			break;
		}
	}
}

//////////// nodes

GUI::Node::Node(const std::string& name, const int* inputTypes, const std::string* inputNames, const int inputCount, const int* outputTypes, const std::string* outputNames, const int outputCount, const sf::Vector2f& initialPosition, const sf::Font& font)
{
	float contentHeight = PROPERTY_HEIGHT * (outputCount > inputCount ? outputCount : inputCount) + CONTENT_MARGIN_TOP;
	this->barRect = sf::RectangleShape(sf::Vector2f(NODE_WIDTH, BAR_HEIGHT));
	this->barRect.setFillColor(sf::Color(BAR_COLOR));
	this->contentRect = sf::RectangleShape(sf::Vector2f(NODE_WIDTH, contentHeight));
	this->contentRect.setFillColor(sf::Color(CONTENT_RECT_COLOR));

	this->title.setFillColor(sf::Color(TEXT_COLOR));
	this->title.setFont(font);
	this->title.setCharacterSize(14);
	this->title.setString(name);

	for (int i = 0; i < inputCount; i++)
	{
		this->inputPins.push_back(new Pin(inputNames[i], inputTypes[i], true, font, this));
	}
	for (int i = 0; i < outputCount; i++)
	{
		this->outputPins.push_back(new Pin(outputNames[i], outputTypes[i], false, font, this));
	}

	setPosition(initialPosition);
}

GUI::Node::~Node()
{
	for (Pin* p : inputPins)
		delete p;
	inputPins.clear();
	for (Pin* p : outputPins)
		delete p;
	outputPins.clear();
}

void GUI::Node::setPosition(const sf::Vector2f& newPosition)
{
	sf::Vector2f displacement = newPosition - barRect.getPosition();

	barRect.setPosition(newPosition);
	title.setPosition(newPosition + sf::Vector2f(TEXT_MARGIN_LEFT, TEXT_MARGIN_TOP));
	contentRect.setPosition(newPosition + sf::Vector2f(0, BAR_HEIGHT));

	int i = 0;
	for (auto p : inputPins)
	{
		p->rect.setPosition(newPosition + sf::Vector2f(PIN_RECT_MARGIN, BAR_HEIGHT + PIN_RECT_MARGIN + PROPERTY_HEIGHT * i + CONTENT_MARGIN_TOP));
		p->text.setPosition(p->rect.getPosition() + sf::Vector2f(PIN_TEXT_MARGIN_X, PIN_TEXT_MARGIN_Y));

		for (auto pointer : p->connectionVertices)
		{
			pointer->position += displacement;
		}
		i++;
	}
	i = 0;
	for (auto p : outputPins)
	{
		p->rect.setPosition(newPosition + sf::Vector2f(NODE_WIDTH - PIN_RECT_MARGIN - PIN_RECT_SIZE, BAR_HEIGHT + PIN_RECT_MARGIN + PROPERTY_HEIGHT * i + CONTENT_MARGIN_TOP));
		p->text.setPosition(p->rect.getPosition() + sf::Vector2f(-PIN_TEXT_MARGIN_X -p->stringOutputOffsetX, PIN_TEXT_MARGIN_Y));

		for (auto pointer : p->connectionVertices)
		{
			pointer->position += displacement;
		}
		i++;
	}
}

sf::Vector2f GUI::Node::getPosition()
{
	return barRect.getPosition();
}

bool GUI::Node::isMouseOverBar(sf::Vector2f& mousePos)
{
	return isVectorOverRect(mousePos, barRect);
}

bool GUI::Node::isMouseOverContent(sf::Vector2f& mousePos)
{
	return isVectorOverRect(mousePos, contentRect);
}

bool GUI::Node::isMouseOverPin(sf::Vector2f& mousePos, Pin*& resultingPin)
{
	for (auto p : inputPins)
	{
		if (p->isMouseOver(mousePos))
		{
			resultingPin = p;
			return true;
		}
	}
	for (auto p : outputPins)
	{
		if (p->isMouseOver(mousePos))
		{
			resultingPin = p;
			return true;
		}
	}
	return false;
}

/////////// typeNodes
/*
GUI::InteractiveNode::InteractiveNode(const std::string& name, const int outputType, const std::string& outputName, const sf::Vector2f& initialPosition, const sf::Font& font)
{
	float contentHeight = PROPERTY_HEIGHT + CONTENT_MARGIN_TOP + INTERACTIVE_COMPONENT_HEIGHT;
	this->barRect = sf::RectangleShape(sf::Vector2f(NODE_WIDTH, BAR_HEIGHT));
	this->barRect.setFillColor(sf::Color(BAR_COLOR));
	this->contentRect = sf::RectangleShape(sf::Vector2f(NODE_WIDTH, contentHeight));
	this->contentRect.setFillColor(sf::Color(CONTENT_RECT_COLOR));

	this->title.setFillColor(sf::Color(TEXT_COLOR));
	this->title.setFont(font);
	this->title.setCharacterSize(NODE_TITLE_FONT_SIZE);
	this->title.setString(name);

	this->outputPins.push_back(new Pin(outputName, outputType, false, font, this));

	this->interactionComponentRect = sf::RectangleShape(sf::Vector2f(NODE_WIDTH * 0.9, INTERACTIVE_COMPONENT_HEIGHT * 0.8));
	this->interactionComponentRect.setFillColor(sf::Color(INTERACTIVE_COMPONENT_COLOR));

	this->interactionComponentText.setFillColor(sf::Color(TEXT_COLOR));
	this->interactionComponentText.setFont(font);
	this->interactionComponentText.setCharacterSize(INTERACTIVE_COMPONENT_TEXT_FONT_SIZE);

	switch(outputType)
	{
		case GUI::Pin::Integer:
			this->interactionComponentText.setString("1");
			this->outputPins[0]->data = new int(1);
			break;
		case GUI::Pin::Float:
			this->interactionComponentText.setString("0.5");
			this->outputPins[0]->data = new float(0.5);
			break;
	}

	setPosition(initialPosition);
}

void GUI::InteractiveNode::setPosition(const sf::Vector2f& newPosition)
{
	Base::setPosition(newPosition);

	interactionComponentRect.setPosition(newPosition + sf::Vector2f(NODE_WIDTH * 0.9, BAR_HEIGHT + PROPERTY_HEIGHT + CONTENT_MARGIN_TOP + 0.8 * INTERACTIVE_COMPONENT_HEIGHT));
	interactionComponentText.setPosition(interactionComponentRect.getPosition() + sf::Vector2f((NODE_WIDTH - NODE_WIDTH * 0.2)/2.0 - 10, (INTERACTIVE_COMPONENT_HEIGHT - INTERACTIVE_COMPONENT_HEIGHT * 0.4)/2.0) - 10);
}*/