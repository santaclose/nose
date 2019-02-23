#include <string>
#include <iostream>
#include <fstream>
#include <SFML/Graphics.hpp>
#include "node.h"

#define TEXT_COLOR 0xf0f0f0ff
#define BAR_COLOR 0x161616bb
#define SELECTED_BAR_COLOR 0x6b6b6bbb
#define CONTENT_RECT_COLOR 0x323232bb
#define INTERACTIVE_COMPONENT_COLOR 0x424242bb
#define INTERACTIVE_COMPONENT_SIZE 10

#define NODE_TITLE_FONT_SIZE 14
#define PIN_TEXT_FONT_SIZE 11
#define INTERACTIVE_COMPONENT_TEXT_FONT_SIZE 12

#define BAR_HEIGHT 25
#define PROPERTY_HEIGHT 25
#define NODE_WIDTH 180
#define CONTENT_MARGIN_TOP 10
#define INTERACTIVE_COMPONENT_HEIGHT 34

#define TEXT_MARGIN_TOP 4
#define TEXT_MARGIN_LEFT 10

#define PIN_RECT_SIZE 5
#define PIN_RECT_MARGIN 8

#define PIN_TEXT_MARGIN_X 15
#define PIN_TEXT_MARGIN_Y -5
#define CHARACTER_WIDTH 7


/////////// generic

bool GUI::isPointOverRect(const sf::Vector2f& vector, const sf::RectangleShape& rect)
{
	sf::Vector2f pos = rect.getPosition();
	sf::Vector2f size = rect.getSize();

	return vector.x > pos.x && vector.x < pos.x + size.x && vector.y > pos.y && vector.y < pos.y + size.y;
}

bool GUI::isPointOverRect(const sf::Vector2f& vector, const sf::Vector2f& rectPosition, const sf::Vector2f& rectSize)
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

GUI::Pin::Pin(const std::string& name, const int type, const bool isInput, Node* parentNode)
{
	this->parentNode = parentNode;
	this->type = type;
	this->isInput = isInput;
	//this->data = nullptr;

	if (!isInput)
		this->stringOutputOffsetX = name.length() * CHARACTER_WIDTH;

	this->rect = sf::RectangleShape(sf::Vector2f(PIN_RECT_SIZE, PIN_RECT_SIZE));
	assignColor(type, this->rect);
	this->interactiveRect = sf::RectangleShape(sf::Vector2f(INTERACTIVE_COMPONENT_SIZE, INTERACTIVE_COMPONENT_SIZE));
	this->interactiveRect.setFillColor(sf::Color(INTERACTIVE_COMPONENT_COLOR));

	this->text.setFillColor(sf::Color(TEXT_COLOR));
	this->text.setFont(GUI::font);
	this->text.setCharacterSize(PIN_TEXT_FONT_SIZE);
	this->text.setString(name);

	switch(this->type)
	{
		case GUI::Pin::Integer:
			this->data = new int(1);
			break;
		case GUI::Pin::Float:
			this->data = new float(0.5);
			break;
		case GUI::Pin::Vector2i:
			this->data = new sf::Vector2i(0, 0);
			break;
		case GUI::Pin::Image:
			this->data = new sf::RenderTexture();
			break;
		case GUI::Pin::Color:
			this->data = new sf::Color(255, 0, 255, 255);
			break;
	}
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
			delete (sf::RenderTexture*)data;
			break;
		case GUI::Pin::Color:
			delete (sf::Color*)data;
			break;
	}
}

void GUI::Pin::setPosition(const sf::Vector2f& newPosition, const int i)
{
	if (this->isInput)
	{
		this->rect.setPosition(newPosition + sf::Vector2f(PIN_RECT_MARGIN, BAR_HEIGHT + PIN_RECT_MARGIN + PROPERTY_HEIGHT * i + CONTENT_MARGIN_TOP));
		this->text.setPosition(this->rect.getPosition() + sf::Vector2f(PIN_TEXT_MARGIN_X, PIN_TEXT_MARGIN_Y));
		this->interactiveRect.setPosition(this->text.getPosition() + sf::Vector2f(20, 0));
	}
	else
	{
		this->rect.setPosition(newPosition + sf::Vector2f(NODE_WIDTH - PIN_RECT_MARGIN - PIN_RECT_SIZE, BAR_HEIGHT + PIN_RECT_MARGIN + PROPERTY_HEIGHT * i + CONTENT_MARGIN_TOP));
		this->text.setPosition(this->rect.getPosition() + sf::Vector2f(-PIN_TEXT_MARGIN_X -this->stringOutputOffsetX, PIN_TEXT_MARGIN_Y));
	}

	for (auto pointer : this->connectionVertices)
	{
		//pointer->position += displacement;
		pointer->position = this->rect.getPosition() + sf::Vector2f(PIN_RECT_SIZE/2, PIN_RECT_SIZE/2);
	}
}

bool GUI::Pin::hasDataAvailable()
{
	if (this->connectedPins.size() == 0) // pin not connected
		return false;
	if (!this->connectedPins[0]->dataAvailable) // connected pin has no data
		return false;
}

bool GUI::Pin::isMouseOver(sf::Vector2f& mousePos)
{
	sf::Vector2f size = rect.getSize();
	sf::Vector2f pos = rect.getPosition();
	pos -= size;
	size = sf::Vector2f(size.x * 3, size.y * 3);
	if (isPointOverRect(mousePos, pos, size))
		return true;
	return false;
}

bool GUI::Pin::isMouseOverInteractionComponent(sf::Vector2f& mousePos)
{
	if (isPointOverRect(mousePos, interactiveRect))
		return true;
	return false;
}

bool GUI::Pin::isDisconnected()
{
	return connectionVertices.empty();
}

void GUI::Pin::setValue(const void* data)
{
	if (this->type == Pin::Float)
		*((float*) this->data) = *((float*) data);
	else if (this->type == Pin::Integer)
		*((int*) this->data) = *((int*) data);
	else if (this->type == Pin::Vector2i)
		*((sf::Vector2i*) this->data) = *((sf::Vector2i*) data);
	else if (this->type == Pin::Color)
		*((sf::Color*) this->data) = *((sf::Color*) data);
	else
		std::cout << "cannot set value for type " << this->type << std::endl;
}

void GUI::Pin::establishConnection(sf::Vertex* newConnectionVertex, GUI::Pin* otherPin, bool isSecondConnection)
{
	this->connectionVertices.push_back(newConnectionVertex);
	this->connectedPins.push_back(otherPin);

	if (!isSecondConnection) // first connection, cannot activate yet
		return;

	// activate the left side node
	if (!this->isInput)
		this->parentNode->activate();
	else
		otherPin->parentNode->activate();
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

void GUI::Pin::draw(sf::RenderWindow& window)
{
	window.draw(this->rect);
	window.draw(this->text);
	if (this->isInput && this->isDisconnected())
		window.draw(this->interactiveRect);
}

//////////// NODES //////////////////

/*GUI::Node::Node(const std::string& name, const int* inputTypes, const std::string* inputNames, const int inputCount, const int* outputTypes, const std::string* outputNames, const int outputCount, const void (*action)(const std::vector<Pin*>& inputPins, const std::vector<Pin*>& outputPins), const sf::Font& font)
{
	std::cout << "construct node" << std::endl;

	this->isInteractive = false;
	this->isOutputNode = false;

	float contentHeight = PROPERTY_HEIGHT * (outputCount > inputCount ? outputCount : inputCount) + CONTENT_MARGIN_TOP;
	this->barRect = sf::RectangleShape(sf::Vector2f(NODE_WIDTH, BAR_HEIGHT));
	this->barRect.setFillColor(sf::Color(BAR_COLOR));
	this->contentRect = sf::RectangleShape(sf::Vector2f(NODE_WIDTH, contentHeight));
	this->contentRect.setFillColor(sf::Color(CONTENT_RECT_COLOR));

	this->title.setFillColor(sf::Color(TEXT_COLOR));
	this->title.setFont(font);
	this->title.setCharacterSize(14);
	this->title.setString(name);

	this->action = action;

	for (int i = 0; i < inputCount; i++)
	{
		this->inputPins.push_back(new Pin(inputNames[i], inputTypes[i], true, font, this));
	}
	for (int i = 0; i < outputCount; i++)
	{
		this->outputPins.push_back(new Pin(outputNames[i], outputTypes[i], false, font, this));
	}
}*/

GUI::Node::Node(const std::string& name, const void (*action)(const std::vector<Pin*>& inputPins, const std::vector<Pin*>& outputPins))
{
	std::cout << "construct node" << std::endl;
	this->action = action;
	this->isOutputNode = false;

	std::ifstream datFile("nodes.dat");

	std::string line;
	while (std::getline(datFile, line))
	{
		std::cout << line << std::endl;
	}
	//this->inputPins.push_back(new Pin("inputNames[i]", Pin::Image, true, this));



	/*float contentHeight = PROPERTY_HEIGHT * (outputCount > inputCount ? outputCount : inputCount) + CONTENT_MARGIN_TOP;
	this->barRect = sf::RectangleShape(sf::Vector2f(NODE_WIDTH, BAR_HEIGHT));
	this->barRect.setFillColor(sf::Color(BAR_COLOR));
	this->contentRect = sf::RectangleShape(sf::Vector2f(NODE_WIDTH, contentHeight));
	this->contentRect.setFillColor(sf::Color(CONTENT_RECT_COLOR));*/
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

void GUI::Node::activate() // tries to compute
{
	if (this->action == nullptr) // node cannot compute
	{
		std::cout << "Node " << this->title.getString().toAnsiString() << "has no action." << std::endl;
		return;
	}
	
	if (!this->hasAllInputData())
	{
		std::cout << "Not enough data to compute " << this->title.getString().toAnsiString() << std::endl;
		return;
	}

	std::cout << "Computing " << this->title.getString().toAnsiString() << std::endl;

	this->action(this->inputPins, this->outputPins);
	for (Pin* outputPin : this->outputPins)
	{
		outputPin->dataAvailable = true;
		std::vector<GUI::Node*> computedNodes;
		for (Pin* p : outputPin->connectedPins)
		{
			if (std::find(computedNodes.begin(), computedNodes.end(), p->parentNode) == computedNodes.end()) // if not already computed
			{
				p->parentNode->activate();
				computedNodes.push_back(p->parentNode);
			}
		}
		computedNodes.clear();

		switch (outputPin->type)	//testing
		{
			case GUI::Pin::Integer:
			{
				std::cout << *((int*)outputPin->data) << std::endl;
				break;
			}
			case GUI::Pin::Float:
			{
				std::cout << *((float*)outputPin->data) << std::endl;
				break;
			}
			case GUI::Pin::Vector2i:
			{
				sf::Vector2i* debug = ((sf::Vector2i*)outputPin->data);
				std::cout << debug->x << ", " << debug->y << std::endl;
				break;
			}
			case GUI::Pin::Image:
			{
				std::cout << "image generated?" << std::endl;
				break;
			}
			case GUI::Pin::Color:
			{
				sf::Color* debug = ((sf::Color*)outputPin->data);
				std::cout << unsigned(debug->r) << ", " << unsigned(debug->g) << ", " << unsigned(debug->b) << ", " << unsigned(debug->a) << std::endl;
				break;
			}
		}
	}
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
		p->setPosition(newPosition, i);
		/*p->rect.setPosition(newPosition + sf::Vector2f(PIN_RECT_MARGIN, BAR_HEIGHT + PIN_RECT_MARGIN + PROPERTY_HEIGHT * i + CONTENT_MARGIN_TOP));
		p->text.setPosition(p->rect.getPosition() + sf::Vector2f(PIN_TEXT_MARGIN_X, PIN_TEXT_MARGIN_Y));

		for (auto pointer : p->connectionVertices)
		{
			pointer->position += displacement;
		}*/
		i++;
	}
	i = 0;
	for (auto p : outputPins)
	{
		p->setPosition(newPosition, i);
		/*p->rect.setPosition(newPosition + sf::Vector2f(NODE_WIDTH - PIN_RECT_MARGIN - PIN_RECT_SIZE, BAR_HEIGHT + PIN_RECT_MARGIN + PROPERTY_HEIGHT * i + CONTENT_MARGIN_TOP));
		p->text.setPosition(p->rect.getPosition() + sf::Vector2f(-PIN_TEXT_MARGIN_X -p->stringOutputOffsetX, PIN_TEXT_MARGIN_Y));

		for (auto pointer : p->connectionVertices)
		{
			pointer->position += displacement;
		}*/
		i++;
	}
}

sf::Vector2f GUI::Node::getPosition()
{
	return barRect.getPosition();
}

bool GUI::Node::isMouseOver(sf::Vector2f& mousePos)
{
	sf::Vector2f barSize = barRect.getSize();

	return isPointOverRect(mousePos, barRect.getPosition(), sf::Vector2f(barSize.x, barSize.y + contentRect.getSize().y));
}

bool GUI::Node::isMouseOverBar(sf::Vector2f& mousePos)
{
	return isPointOverRect(mousePos, barRect);
}

bool GUI::Node::isMouseOverContent(sf::Vector2f& mousePos)
{
	return isPointOverRect(mousePos, contentRect);
}

bool GUI::Node::isMouseOverPin(sf::Vector2f& mousePos, Pin*& resultingPin)
{
	for (Pin* p : inputPins)
	{
		if (p->isMouseOver(mousePos))
		{
			resultingPin = p;
			return true;
		}
	}
	for (Pin* p : outputPins)
	{
		if (p->isMouseOver(mousePos))
		{
			resultingPin = p;
			return true;
		}
	}
	return false;
}

bool GUI::Node::hasAllInputData()
{
	for (Pin* p : inputPins)
	{
		if (!p->hasDataAvailable())
			return false;
	}
	return true;
}

bool GUI::Node::isMouseOverInteractionComponent(sf::Vector2f& mousePos, Pin*& resultingPin)
{
	for (Pin* pin : inputPins)
	{
		if (pin->isMouseOverInteractionComponent(mousePos))
		{
			resultingPin = pin;
			return true;
		}
	}
	return false;
}

void GUI::Node::paintAsSelected()
{
	barRect.setFillColor(sf::Color(SELECTED_BAR_COLOR));
}
void GUI::Node::paintAsUnselected()
{
	barRect.setFillColor(sf::Color(BAR_COLOR));
}

void GUI::Node::draw(sf::RenderWindow& window)
{
	window.draw(this->barRect);
	window.draw(this->contentRect);
	window.draw(this->title);
	for (auto p : this->inputPins)
	{
		p->draw(window);
	}
	for (auto p : this->outputPins)
	{
		p->draw(window);
	}
}

/////////// interactive nodes

/*GUI::InteractiveNode::InteractiveNode(const std::string& name, const int* inputTypes, const std::string* inputNames, const int inputCount, const int* outputTypes, const std::string* outputNames, const int outputCount, const void (*action)(const std::vector<Pin*>& inputPins, const std::vector<Pin*>& outputPins), const sf::Font& font) : GUI::Node(name, inputTypes, inputNames, inputCount, outputTypes, outputNames, outputCount, action, font)
{
	std::cout << "construct interactive node" << std::endl;

	this->isInteractive = true;

	// setting the size again :(
	this->contentRect.setSize(sf::Vector2f(NODE_WIDTH, PROPERTY_HEIGHT + CONTENT_MARGIN_TOP + INTERACTIVE_COMPONENT_HEIGHT));

	this->interactionComponentRect = sf::RectangleShape(sf::Vector2f(NODE_WIDTH * 0.9, INTERACTIVE_COMPONENT_HEIGHT * 0.6));
	this->interactionComponentRect.setFillColor(sf::Color(INTERACTIVE_COMPONENT_COLOR));

	this->interactionComponentText.setFillColor(sf::Color(TEXT_COLOR));
	this->interactionComponentText.setFont(font);
	this->interactionComponentText.setCharacterSize(INTERACTIVE_COMPONENT_TEXT_FONT_SIZE);

	if (outputTypes[0] == GUI::Pin::Float)
		this->interactionComponentText.setString("0.5");

	else if (outputTypes[0] == GUI::Pin::Integer)
		this->interactionComponentText.setString("1");

	this->outputPins[0]->dataAvailable = true; // all interactive nodes have data ready when instantiated
}*/
/*
void GUI::InteractiveNode::activate() // propagates activation
{
	std::cout << "Not computing " << this->title.getString().toAnsiString() << std::endl;
	std::vector<GUI::Node*> computedNodes;
	for (Pin* p : this->outputPins[0]->connectedPins) // should be only one output pin
	{
		if (std::find(computedNodes.begin(), computedNodes.end(), p->parentNode) == computedNodes.end()) // if not already computed
		{
			p->parentNode->activate();
			computedNodes.push_back(p->parentNode);
		}
	}
	computedNodes.clear();
}

void GUI::InteractiveNode::setPosition(const sf::Vector2f& newPosition)
{
	Node::setPosition(newPosition);

	this->interactionComponentRect.setPosition(newPosition + sf::Vector2f(NODE_WIDTH * 0.05, BAR_HEIGHT + PROPERTY_HEIGHT + CONTENT_MARGIN_TOP + 0.15 * INTERACTIVE_COMPONENT_HEIGHT));
	this->interactionComponentText.setPosition(interactionComponentRect.getPosition() + sf::Vector2f((NODE_WIDTH - NODE_WIDTH * 0.2)/2.0 - 10, INTERACTIVE_COMPONENT_HEIGHT*0.1));
}

bool GUI::InteractiveNode::isMouseOverInteractionComponent(sf::Vector2f& mousePos)
{
	return isPointOverRect(mousePos, interactionComponentRect);
}

void GUI::InteractiveNode::setValue(float value)
{
	if (outputPins[0]->type == Pin::Integer)
	{
		*((int*)outputPins[0]->data) = (int) value;
		interactionComponentText.setString(std::to_string((int) value));
	}
	else
	{
		*((float*)outputPins[0]->data) = value;
		interactionComponentText.setString(std::to_string(value));
	}
}

void GUI::InteractiveNode::draw(sf::RenderWindow& window)
{
	Node::draw(window);
	window.draw(this->interactionComponentRect);
	window.draw(this->interactionComponentText);
}
*/
/////////// output node

/*GUI::OutputNode::OutputNode(const std::string& name, const int* inputTypes, const std::string* inputNames, const int inputCount, const int* outputTypes, const std::string* outputNames, const int outputCount, const void (*action)(const std::vector<Pin*>& inputPins, const std::vector<Pin*>& outputPins), const sf::Font& font, sf::RenderWindow* window) : GUI::Node(name, inputTypes, inputNames, inputCount, outputTypes, outputNames, outputCount, action, font)
{
	std::cout << "construct output node" << std::endl;
	this->isOutputNode = true;
	this->window = window;
}*/

GUI::OutputNode::OutputNode(const std::string& name, const void (*action)(const std::vector<Pin*>& inputPins, const std::vector<Pin*>& outputPins), sf::RenderWindow* window) : GUI::Node(name, action)
{
	std::cout << "construct output node" << std::endl;
	this->isOutputNode = true;
	this->window = window;
}

void GUI::OutputNode::activate()
{
	if (this->hasAllInputData())
	{
		//std::cout << "Drawing output" << std::endl;
		sf::RenderTexture* pointer = (sf::RenderTexture*) inputPins[0]->connectedPins[0]->data;

		sf::Sprite spr(pointer->getTexture());
		spr.setPosition(20, 20);
		this->window->draw(spr);
	}
}