#include <string>
#include <iostream>
#include <SFML/Graphics.hpp>
#include "node.h"

#define TEXT_COLOR 0xffffff
#define BAR_COLOR 0x2f2fbb
#define CONTENT_RECT_COLOR 0x4444bb

#define BAR_HEIGHT 25
#define PROPERTY_HEIGHT 25
#define NODE_WIDTH 180
#define CONTENT_MARGIN_TOP 10

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
			rect.setFillColor(sf::Color(0xff1d34ff));
			break;
		case GUI::Pin::Vector2i:
			rect.setFillColor(sf::Color(0x56957aff));
			break;
		case GUI::Pin::Recti:
			rect.setFillColor(sf::Color(0x324afbff));
			break;
		case GUI::Pin::Image:
			rect.setFillColor(sf::Color(0x00aa00ff));
			break;

	}
}

GUI::Pin::Pin(const std::string& name, const int type, const bool isInput, const sf::Font& font)
{
	this->type = type;
	this->isInput = isInput;
	if (!isInput)
		this->stringOutputOffsetX = name.length() * CHARACTER_WIDTH;

	this->rect = sf::RectangleShape(sf::Vector2f(PIN_RECT_SIZE, PIN_RECT_SIZE));
	assignColor(type, this->rect);

	this->text.setFillColor(sf::Color(TEXT_COLOR));
	this->text.setFont(font);
	this->text.setCharacterSize(11);
	this->text.setString(name);
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

void GUI::Pin::establishConnection(sf::Vertex* newConnectionVertex, GUI::Pin* other)
{
	this->connectionVertices.push_back(newConnectionVertex);
	this->connectedPins.push_back(other);
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
		this->inputPins.push_back(new Pin(inputNames[i], inputTypes[i], true, font));
	}
	for (int i = 0; i < outputCount; i++)
	{
		this->outputPins.push_back(new Pin(outputNames[i], outputTypes[i], false, font));
	}

	setPosition(initialPosition);
}
GUI::Node::~Node()
{

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