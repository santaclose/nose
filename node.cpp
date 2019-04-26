#include <string>
#include <iostream>
#include <fstream>
#include <SFML/Graphics.hpp>
#include "node.h"
#include "nodeFileParser.h"

#define TEXT_COLOR 0xf0f0f0ff
#define BAR_COLOR 0x161616bb
#define SELECTED_BAR_COLOR 0x6b6b6bbb
#define CONTENT_RECT_COLOR 0x323232bb
#define INTERACTIVE_COMPONENT_COLOR 0x424242bb
#define INTERACTIVE_COMPONENT_WIDTH 56
#define INTERACTIVE_COMPONENT_HEIGHT 16

#define NODE_TITLE_FONT_SIZE 14
#define PIN_TEXT_FONT_SIZE 11
#define INTERACTIVE_COMPONENT_TEXT_FONT_SIZE 12

#define BAR_HEIGHT 25
#define PROPERTY_HEIGHT 26
#define NODE_WIDTH 180
#define CONTENT_MARGIN_TOP 10

#define TEXT_MARGIN_TOP 4
#define TEXT_MARGIN_LEFT 10

#define PIN_RECT_SIZE 5
#define PIN_RECT_MARGIN 8

#define PIN_TEXT_MARGIN_X 15
#define PIN_TEXT_MARGIN_Y -5
#define CHARACTER_WIDTH 7

#define SEARCH_BAR_WIDTH 400
#define SEARCH_BAR_HEIGHT 40
#define SEARCH_BAR_COLOR 0x323232ff
#define SEARCH_BAR_FONT_SIZE 16
#define SEARCH_BAR_TEXT_MARGIN 8

/////////// generic
void GUI::initializeFont()
{
	gFont.loadFromFile("firacode.ttf");
}

void GUI::initializeSearchBar(sf::RectangleShape*& searchBar, sf::Text& searchText, char* searchBuffer, sf::RenderWindow& window)
{
	searchBar = new sf::RectangleShape(sf::Vector2f(SEARCH_BAR_WIDTH, SEARCH_BAR_HEIGHT));
	searchBar->setPosition(window.getSize().x / 2 - SEARCH_BAR_WIDTH / 2, 0);
	searchBar->setFillColor(sf::Color(SEARCH_BAR_COLOR));
	searchText.setFillColor(sf::Color(TEXT_COLOR));
	searchText.setFont(gFont);
	searchText.setPosition(window.getSize().x / 2 - SEARCH_BAR_WIDTH / 2 + SEARCH_BAR_TEXT_MARGIN, SEARCH_BAR_TEXT_MARGIN);
	searchText.setCharacterSize(SEARCH_BAR_FONT_SIZE);
	searchBuffer[0] = '\0';
}

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
		case Pin::Integer:
			rect.setFillColor(sf::Color(0x4ca4fbff));
			break;
		case Pin::Float:
			rect.setFillColor(sf::Color(0xee1133ff));
			break;
		case Pin::Vector2i:
			rect.setFillColor(sf::Color(0x56957aff));
			break;
		/*case Pin::Recti:
			rect.setFillColor(sf::Color(0x324afbff));
			break;*/
		case Pin::Image:
			rect.setFillColor(sf::Color(0x00bc44ff));
			break;
		case Pin::Color:
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
	this->interactiveRect = sf::RectangleShape(sf::Vector2f(INTERACTIVE_COMPONENT_WIDTH, INTERACTIVE_COMPONENT_HEIGHT));
	this->interactiveRect.setFillColor(sf::Color(INTERACTIVE_COMPONENT_COLOR));

	//sf::Font theFont;
	//theFont.loadFromFile("firacode.ttf");

	this->text = sf::Text(name, gFont, PIN_TEXT_FONT_SIZE);
	this->text.setFillColor(sf::Color(TEXT_COLOR));

	switch(this->type)
	{
		case GUI::Pin::Integer:
			this->data = new int(1);
			this->interactiveText = sf::Text("1", gFont, 11);
			break;
		case GUI::Pin::Float:
			this->data = new float(0.5);
			this->interactiveText = sf::Text("0.5", gFont, 11);
			break;
		case GUI::Pin::Vector2i:
			this->data = new sf::Vector2i(0, 0);
			this->interactiveText = sf::Text("0, 0", gFont, 11);
			break;
		case GUI::Pin::Image:
			this->data = new sf::RenderTexture();
			this->interactiveText = sf::Text("None", gFont, 11);
			break;
		case GUI::Pin::Color:
			this->data = new sf::Color(255, 0, 255, 255);
			this->interactiveRect.setFillColor(*((sf::Color*) this->data));
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
		this->rect.setPosition(newPosition + sf::Vector2f(PIN_RECT_MARGIN, BAR_HEIGHT + PIN_RECT_MARGIN + PROPERTY_HEIGHT * (i + this->parentNode->outputPins.size()) + CONTENT_MARGIN_TOP));

		if (this->isDisconnected() && !this->parentNode->isOutputNode)
		{
			this->interactiveRect.setPosition(this->rect.getPosition() + sf::Vector2f(16, -7));
			this->text.setPosition(this->interactiveRect.getPosition() + sf::Vector2f(INTERACTIVE_COMPONENT_WIDTH + PIN_TEXT_MARGIN_X, 0));
			//this->text.setPosition(this->rect.getPosition() + sf::Vector2f(PIN_TEXT_MARGIN_X, PIN_TEXT_MARGIN_Y));
			//this->interactiveRect.setPosition(sf::Vector2f(newPosition.x + NODE_WIDTH - INTERACTIVE_COMPONENT_WIDTH - PIN_RECT_MARGIN, this->text.getPosition().y));
		}
		else
		{
			this->text.setPosition(this->rect.getPosition() + sf::Vector2f(PIN_TEXT_MARGIN_X, PIN_TEXT_MARGIN_Y));
		}
		this->interactiveText.setPosition(this->interactiveRect.getPosition() + sf::Vector2f(2, 2));
	}
	else
	{
		this->rect.setPosition(newPosition + sf::Vector2f(NODE_WIDTH - PIN_RECT_MARGIN - PIN_RECT_SIZE, BAR_HEIGHT + PIN_RECT_MARGIN + PROPERTY_HEIGHT * i + CONTENT_MARGIN_TOP));
		this->text.setPosition(this->rect.getPosition() + sf::Vector2f(-PIN_TEXT_MARGIN_X -this->stringOutputOffsetX, PIN_TEXT_MARGIN_Y));
	}

	for (auto pointer : this->connectionVertices)
	{
		pointer->position = this->rect.getPosition() + sf::Vector2f(PIN_RECT_SIZE/2, PIN_RECT_SIZE/2);
	}
}

bool GUI::Pin::hasDataAvailable()
{
	if (this->type != Pin::Image)
		return true;

	if (this->dataAvailable == true) // image set for the input pin
		return true;

	if (this->connectedPins.size() == 0) // pin not connected
		return false;
	if (!this->connectedPins[0]->dataAvailable) // connected pin has no data
		return false;
	return true;
}

void* GUI::Pin::getData()
{
	if (this->isDisconnected())
		return this->data;
	else
		return this->connectedPins[0]->data;
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

void GUI::Pin::setInteractiveText(const std::string& newText)
{
	this->interactiveText.setString(newText);
}

bool GUI::Pin::isDisconnected()
{
	return connectionVertices.empty();
}

void GUI::Pin::setValue(const void* data) // copies the data into its data field
{
	switch (this->type)
	{
		case Pin::Float:
			this->interactiveText.setString(std::to_string(*((float*) this->data) = *((float*) data)));
			break;
		case Pin::Integer:
			this->interactiveText.setString(std::to_string(*((int*) this->data) = (int) *((float*) data)));
			break;
		case Pin::Vector2i:
			*((sf::Vector2i*) this->data) = *((sf::Vector2i*) data);
			break;
		case Pin::Color:
//			*((sf::Color*) this->data) = *((sf::Color*) data);
			this->interactiveRect.setFillColor(*((sf::Color*) this->data) = *((sf::Color*) data));
			break;
		case Pin::Image:
			break;
	}

	this->parentNode->activate();
}

void GUI::Pin::establishConnection(sf::Vertex* newConnectionVertex, GUI::Pin* otherPin, bool isSecondConnection)
{
	this->connectionVertices.push_back(newConnectionVertex);
	this->connectedPins.push_back(otherPin);

	// refresh position to display properly
	this->parentNode->setPosition(this->parentNode->getPosition());

	if (!isSecondConnection) // first connection, cannot activate yet
		return;

	// activate the left side node (the one that has the output pin)
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

			// refresh position to display properly
			this->parentNode->setPosition(this->parentNode->getPosition());

			break;
		}
	}
}

void GUI::Pin::draw(sf::RenderWindow& window)
{
	window.draw(this->rect);
	window.draw(this->text);
	if (this->parentNode->isOutputNode)
		return;
	if (this->isInput && this->isDisconnected())
	{
		window.draw(this->interactiveRect);
		window.draw(this->interactiveText);
	}
}

//////////// NODES //////////////////

GUI::Node::Node(const std::string& name, const void (*action)(const std::vector<Pin*>& inputPins, const std::vector<Pin*>& outputPins))
{
	std::cout << "construct node " << name << std::endl;

	this->action = action;
	this->isOutputNode = false;

	//sf::Font theFont;
	//theFont.loadFromFile("firacode.ttf");

	int pinCountSum; // used for setting content height
	FileParser::pushPinsFromFile(this, name, pinCountSum); // sets pinCountSum and pushes pins

	float contentHeight = PROPERTY_HEIGHT * pinCountSum + CONTENT_MARGIN_TOP;
	this->barRect = sf::RectangleShape(sf::Vector2f(NODE_WIDTH, BAR_HEIGHT));
	this->barRect.setFillColor(sf::Color(BAR_COLOR));
	this->contentRect = sf::RectangleShape(sf::Vector2f(NODE_WIDTH, contentHeight));
	this->contentRect.setFillColor(sf::Color(CONTENT_RECT_COLOR));

	this->title = sf::Text(name, gFont, NODE_TITLE_FONT_SIZE);
	this->title.setFillColor(sf::Color(TEXT_COLOR));
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
		i++;
	}
	i = 0;
	for (auto p : outputPins)
	{
		p->setPosition(newPosition, i);
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
		if (!p->hasDataAvailable()){
			if (!this->isOutputNode)
				std::cout << "false";
			return false;
		}
	}
	if (!this->isOutputNode)
		std::cout<< "true";
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

	for (Pin* p : this->inputPins)
	{
		p->draw(window);
	}
	for (Pin* p : this->outputPins)
	{
		p->draw(window);
	}
}

/////////// output node

GUI::OutputNode::OutputNode(const std::string& name, const void (*action)(const std::vector<Pin*>& inputPins, const std::vector<Pin*>& outputPins), sf::RenderWindow* window) : GUI::Node(name, action)
{
	std::cout << "construct output node" << std::endl;
	this->isOutputNode = true;
	this->window = window;
	this->saveButtonRect = sf::RectangleShape(sf::Vector2f(NODE_WIDTH * 0.3, BAR_HEIGHT * 0.8));
	this->saveButtonRect.setFillColor(sf::Color(INTERACTIVE_COMPONENT_COLOR));
	this->saveButtonText = sf::Text("save", gFont, PIN_TEXT_FONT_SIZE);
	this->saveButtonText.setFillColor(sf::Color(TEXT_COLOR));
}

bool GUI::OutputNode::isMouseOverSaveButton(sf::Vector2f& mousePos)
{
	return isPointOverRect(mousePos, this->saveButtonRect);
}

void GUI::OutputNode::saveImage()
{
	if (!this->hasAllInputData())
	{
		std::cout << "NO DATA TO SAVE" << std::endl;
		return;
	}

	sf::RenderTexture* pointer = (sf::RenderTexture*) inputPins[0]->connectedPins[0]->data;
	pointer->getTexture().copyToImage().saveToFile("out.png");
	std::cout << "IMAGE SAVED AS \"out.png\"\n";
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

void GUI::OutputNode::setPosition(const sf::Vector2f& newPosition)
{
	Node::setPosition(newPosition);
	this->saveButtonRect.setPosition(this->contentRect.getPosition() + sf::Vector2f(NODE_WIDTH * 0.65, 8.0));
	this->saveButtonText.setPosition(this->saveButtonRect.getPosition() + sf::Vector2f(12.0, 3.0));
}

void GUI::OutputNode::draw(sf::RenderWindow& window)
{
	Node::draw(window);
	window.draw(this->saveButtonRect);
	window.draw(this->saveButtonText);
}