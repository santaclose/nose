#define BACKGROUND_COLOR 0x2121ff

#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include "node.h"
#include "nodeActions.h"

class ConnectionLine
{
public:
	GUI::Pin* pins[2];
	sf::Vertex vertices[2];
};

sf::Font font;

GUI::Node* draggingNode = nullptr;
sf::Vector2f startDraggingMousePosition;
sf::Vector2f startDraggingNodePosition;

bool creatingConnection = false;
ConnectionLine newConnection;

std::vector<GUI::Node*> nodes;
std::vector<ConnectionLine*> connectionLines;

void createOutputNode()
{
	int inputTypes[1];
	std::string inputStrings[1] = {"Resulting Image"};
	inputTypes[0] = GUI::Pin::Image;
	nodes.push_back(new GUI::Node("Output", inputTypes, inputStrings, 1, nullptr, nullptr, 0, sf::Vector2f(1000, 380), font));
}

int main()
{    
	font.loadFromFile("firacode.ttf");

	createOutputNode();

	// create the window
	sf::RenderWindow window(sf::VideoMode(1200, 800), "nose");

	window.setFramerateLimit(60); // 10

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
					for (GUI::Node* n : nodes)
						delete n;
					nodes.clear();
					for (ConnectionLine* c : connectionLines)
						delete c;
					connectionLines.clear();
					window.close();
					break;
				}
				case sf::Event::Resized:
				{
					sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
					window.setView(sf::View(visibleArea));
				}
				case sf::Event::MouseButtonPressed:
				{
					if (event.mouseButton.button == sf::Mouse::Left)
					{
						sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);

						for (GUI::Node* n : nodes)
						{
							if (n->isMouseOverBar(mousePos))
							{
								draggingNode = n;
								startDraggingMousePosition = mousePos;
								startDraggingNodePosition = n->getPosition();
							}
							else if (n->isMouseOverContent(mousePos))
							{
								GUI::Pin* p;
								if (n->isMouseOverPin(mousePos, p))
								{
									std::cout << p->connectionVertices.size() << std::endl;
									std::cout << p->connectedPins.size() << std::endl;
									if (!p->isDisconnected() && p->isInput)
									{
										for (int i = 0; i < connectionLines.size(); i++) // finding a single connection line
										{
											if (connectionLines[i]->pins[0] == p || connectionLines[i]->pins[1] == p)
											{
												connectionLines[i]->pins[0]->disconnectFrom(connectionLines[i]->pins[1]);
												connectionLines[i]->pins[1]->disconnectFrom(connectionLines[i]->pins[0]);
												delete connectionLines[i];
												connectionLines.erase(connectionLines.begin() + i);
												break;
											}
										}
									}
									if (!p->isInput || p->isInput && p->isDisconnected())
									{
										newConnection.pins[0] = p;
										newConnection.vertices[0].position = p->getRectCenter();
										creatingConnection = true;
									}
								}
							}
						}
					}
					break;
				}
				case sf::Event::MouseButtonReleased:
				{
					if (event.mouseButton.button == sf::Mouse::Left)
					{
						if (creatingConnection)
						{
							sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
							for (GUI::Node* n : nodes)
							{
								if (n->isMouseOverContent(mousePos))
								{
									GUI::Pin* p;
									if (n->isMouseOverPin(mousePos, p))
									{
										if ((!p->isInput || p->isInput && p->isDisconnected()) && p->isInput != newConnection.pins[0]->isInput && p->type == newConnection.pins[0]->type) // valid connection
										{
											ConnectionLine* a = new ConnectionLine();
											a->vertices[0] = newConnection.vertices[0];
											a->vertices[1].position = p->getRectCenter();
											a->pins[0] = newConnection.pins[0];
											a->pins[1] = p;

											a->pins[0]->establishConnection(&a->vertices[0], a->pins[1]);
											a->pins[1]->establishConnection(&a->vertices[1], a->pins[0]);
											
											connectionLines.push_back(a);
										}
									}
								}
							}
						}

						draggingNode = nullptr;
						creatingConnection = false;
					}
					break;
				}
				case sf::Event::MouseMoved:
				{
					if (draggingNode != nullptr)
					{
						// compute displacement
						sf::Vector2f displacement(event.mouseMove.x, event.mouseMove.y);
						displacement -= startDraggingMousePosition;

						// update position
						sf::Vector2f currentPos = startDraggingNodePosition + displacement;
						draggingNode->setPosition(currentPos);
					}

					newConnection.vertices[1].position = sf::Vector2f(event.mouseMove.x, event.mouseMove.y);
					break;
				}
				case sf::Event::KeyPressed:
				{
					switch (event.key.code)
					{
						case sf::Keyboard::A:
						{
							int inputTypes[2];
							int outputTypes[1];
							std::string inputStrings[2] = {"A", "B"};
							std::string outputStrings[1] = {"Result"};
							inputTypes[0] = inputTypes[1] = outputTypes[0] = GUI::Pin::Image;
							nodes.push_back(new GUI::Node("Add", inputTypes, inputStrings, 2, outputTypes, outputStrings, 1, sf::Vector2f(20, 50), font));
							break;
						}
						case sf::Keyboard::R:
						{
							int inputTypes[2];
							int outputTypes[1];
							std::string inputStrings[2] = {"Image", "Output Size"};
							std::string outputStrings[1] = {"Result"};
							inputTypes[0] = outputTypes[0] = GUI::Pin::Image;
							inputTypes[1] = GUI::Pin::Vector2i;
							nodes.push_back(new GUI::Node("Repeat", inputTypes, inputStrings, 2, outputTypes, outputStrings, 1, sf::Vector2f(20, 50), font));
							break;
						}
						case sf::Keyboard::I:
						{
							int outputTypes[1];
							std::string outputStrings[1] = {"Image"};
							outputTypes[0] = GUI::Pin::Image;
							nodes.push_back(new GUI::Node("Image", nullptr, nullptr, 0, outputTypes, outputStrings, 1, sf::Vector2f(20, 50), font));
							break;
						}
					}
				}
			}
		}

		// clear the window
		window.clear(sf::Color(BACKGROUND_COLOR));

		// draw everything here...

		for (GUI::Node* n : nodes)
		{
			window.draw(n->barRect);
			window.draw(n->contentRect);
			window.draw(n->title);
			for (auto p : n->inputPins)
			{
				window.draw(p->rect);
				window.draw(p->text);
			}
			for (auto p : n->outputPins)
			{
				window.draw(p->rect);
				window.draw(p->text);
			}

			for (ConnectionLine* c : connectionLines)
				window.draw(c->vertices, 2, sf::Lines);

			if (creatingConnection)
				window.draw(newConnection.vertices, 2, sf::Lines);
		}

		// end the current frame

		window.display();
	}

	return 0;
}