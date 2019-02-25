#define BACKGROUND_COLOR 0xa9a9a9ff
#define TEXT_COLOR 0xf0f0f0ff
#define INPUT_BUFFER_SIZE 20
#define SEARCH_BAR_WIDTH 400
#define SEARCH_BAR_HEIGHT 40
#define SEARCH_BAR_COLOR 0x323232ff
#define SEARCH_BAR_BUFFER_SIZE 31
#define SEARCH_BAR_FONT_SIZE 16
#define SEARCH_BAR_TEXT_MARGIN 8

#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <SFML/Graphics.hpp>
#include "node.h"
sf::Shader* shaders;
#include "nodeActions.h"

struct ConnectionLine
{
	GUI::Pin* pins[2];
	sf::Vertex vertices[2];
};

GUI::OutputNode* outputNode = nullptr;

GUI::Node* selectedNode = nullptr;

GUI::Node* draggingNode = nullptr;
sf::Vector2f startDraggingMousePosition;
sf::Vector2f startDraggingNodePosition;

bool creatingConnection = false;
ConnectionLine newConnection;

std::vector<GUI::Node*> nodes;
std::vector<ConnectionLine*> connectionLines;

int currentInputIndex;
char inputField[INPUT_BUFFER_SIZE];

bool searching = false;
sf::RectangleShape* searchBar;
sf::Text searchText;
char searchBuffer[SEARCH_BAR_BUFFER_SIZE];
int searchBufferCurrentChar = 0;

#include "userInteraction.h"

void createOutputNode(sf::RenderWindow& window)
{
	nodes.push_back(outputNode = new GUI::OutputNode("Output", nullptr, &window));
	outputNode->setPosition(sf::Vector2f(1000, 380));
}

inline void init(sf::RenderWindow& window)
{
	GUI::initializeFont();
	editingFloat = new float();

	searchBar = new sf::RectangleShape(sf::Vector2f(SEARCH_BAR_WIDTH, SEARCH_BAR_HEIGHT));
	searchBar->setPosition(window.getSize().x / 2 - SEARCH_BAR_WIDTH / 2, 0);
	searchBar->setFillColor(sf::Color(SEARCH_BAR_COLOR));
	searchText.setFillColor(sf::Color(TEXT_COLOR));
	searchText.setFont(GUI::gFont);
	searchText.setPosition(window.getSize().x / 2 - SEARCH_BAR_WIDTH / 2 + SEARCH_BAR_TEXT_MARGIN, SEARCH_BAR_TEXT_MARGIN);
	searchText.setCharacterSize(SEARCH_BAR_FONT_SIZE);
	searchBuffer[0] = '\0';

	shaders = new sf::Shader[5];
	shaders[0].loadFromFile("shaders/checker.glsl", sf::Shader::Fragment);
	shaders[1].loadFromFile("shaders/gradient.glsl", sf::Shader::Fragment);
	shaders[2].loadFromFile("shaders/multiply.glsl", sf::Shader::Fragment);
	shaders[3].loadFromFile("shaders/rotate90.glsl", sf::Shader::Fragment);
	shaders[4].loadFromFile("shaders/repeat.glsl", sf::Shader::Fragment);

	createOutputNode(window);
	window.setFramerateLimit(60); // 10
}

int main()
{
	// create the window
	sf::RenderWindow window(sf::VideoMode(1200, 800), "nose");
	init(window);

	// run the program as long as the window is open

	while (window.isOpen())
	{
		// check all the window's events that were triggered since the last iteration of the loop
		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
				case sf::Event::Closed:
				{
					delete editingFloat;
					delete searchBar;
					delete[] shaders;

					for (GUI::Node* n : nodes)
						delete n;
					nodes.clear();
					for (ConnectionLine* c : connectionLines)
						delete c;
					connectionLines.clear();
					window.close();
					return 0;
					break;
				}
				case sf::Event::Resized:
				{
					sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
					window.setView(sf::View(visibleArea));
					searchBar->setPosition(event.size.width / 2 - SEARCH_BAR_WIDTH / 2, 0);
					searchText.setPosition(event.size.width / 2 - SEARCH_BAR_WIDTH / 2 + SEARCH_BAR_TEXT_MARGIN, SEARCH_BAR_TEXT_MARGIN);
				}
				case sf::Event::MouseButtonPressed:
				{
					sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
					if (event.mouseButton.button == sf::Mouse::Left)
					{
						EventMouseLeftDown(mousePos);
					}
					else if (event.mouseButton.button == sf::Mouse::Right)
					{
						EventMouseRightDown(mousePos);
					}
					break;
				}
				case sf::Event::MouseButtonReleased:
				{
					sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
					if (event.mouseButton.button == sf::Mouse::Left)
					{
						EventMouseLeftUp(mousePos);
					}
					break;
				}
				case sf::Event::MouseMoved:
				{
					EventMouseMoved(event.mouseMove);
					break;
				}
				case sf::Event::KeyPressed:
				{
					EventKeyPressed(event.key.code);
					break;
				}
				case sf::Event::TextEntered:
				{
					EventTextEntered(event.text.unicode);
					break;
				}
			}
		}

		// clear the window
		window.clear(sf::Color(BACKGROUND_COLOR));
		/////////////////////////////////////////////////////

		// draw everything here...

		if (searching)
		{
			window.draw(*searchBar);
			window.draw(searchText);
		}

		for (GUI::Node* n : nodes)
		{
			n->draw(window);
		}

		for (ConnectionLine* c : connectionLines)
			window.draw(c->vertices, 2, sf::Lines);

		if (creatingConnection)
			window.draw(newConnection.vertices, 2, sf::Lines);

		outputNode->activate();

		/////////////////////////////////////////////////////
		// end the current frame
		window.display();
	}

	return 0;
}