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

GUI::Node* selectedNode = nullptr;

GUI::Node* draggingNode = nullptr;
sf::Vector2f startDraggingMousePosition;
sf::Vector2f startDraggingNodePosition;

bool creatingConnection = false;
ConnectionLine newConnection;

GUI::InteractiveNode* editingNode = nullptr;
int editingType = -1;
float* editingValue;

std::vector<GUI::Node*> nodes;
std::vector<ConnectionLine*> connectionLines;

void createOutputNode()
{
	int inputTypes[1];
	std::string inputStrings[1] = {"Resulting Image"};
	inputTypes[0] = GUI::Pin::Image;
	GUI::Node* newNode;
	nodes.push_back(newNode = new GUI::Node("Output", inputTypes, inputStrings, 1, nullptr, nullptr, 0, font));
	newNode->setPosition(sf::Vector2f(1000, 380));
}

void deleteConnectionLine(int& i)
{
	connectionLines[i]->pins[0]->disconnectFrom(connectionLines[i]->pins[1]);
	connectionLines[i]->pins[1]->disconnectFrom(connectionLines[i]->pins[0]);
	delete connectionLines[i];
	connectionLines.erase(connectionLines.begin() + i);
}

int main()
{
	editingValue = new float(0.0);

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
					delete editingValue;
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
								std::cout << "mouse is over content\n";
								GUI::Pin* p;
								if (n->isMouseOverPin(mousePos, p))
								{
									if (!p->isDisconnected() && p->isInput)
									{
										for (int i = 0; i < connectionLines.size(); i++) // finding a single connection line
										{
											if (connectionLines[i]->pins[0] == p || connectionLines[i]->pins[1] == p)
											{
												deleteConnectionLine(i);
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
								else if (n->isMouseOverInteractionComponent(mousePos))
								{
									editingType = n->outputPins[0]->type;
									std::cout << "editing " << editingType << std::endl;
									if (editingType == GUI::Pin::Integer)
										*editingValue = (float)*((int*)n->outputPins[0]->data);
									else
										*editingValue = *((float*)n->outputPins[0]->data);
									startDraggingMousePosition = mousePos;
									editingNode = (GUI::InteractiveNode*) n;
								}
							}
						}
					}
					else if (event.mouseButton.button == sf::Mouse::Right)
					{
						sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);

						for (GUI::Node* n : nodes)
						{
							if (n->isMouseOverBar(mousePos))
							{
								if (n == selectedNode)
								{
									selectedNode = nullptr;
									n->paintAsUnselected();
								}
								else
								{
									if (selectedNode != nullptr)
										selectedNode->paintAsUnselected();
									n->paintAsSelected();
									selectedNode = n;
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

						editingNode = nullptr;
						draggingNode = nullptr;
						creatingConnection = false;
					}
					break;
				}
				case sf::Event::MouseMoved:
				{
					if (draggingNode != nullptr) // user is dragging a node
					{
						// compute displacement
						sf::Vector2f displacement(event.mouseMove.x, event.mouseMove.y);
						displacement -= startDraggingMousePosition;

						// update position
						sf::Vector2f currentPos = startDraggingNodePosition + displacement;
						draggingNode->setPosition(currentPos);
					}

					if (editingNode != nullptr) // user is setting a value
					{
						int disp = event.mouseMove.x - startDraggingMousePosition.x;
						*editingValue += disp*disp*disp * (editingType == GUI::Pin::Float ? 0.01 : 0.1);

						/*if (event.mouseMove.x < 0.0)
						{
							startDraggingMousePosition.x = window.getSize().x;
							sf::Mouse::setPosition(sf::Vector2i(window.getPosition().x + window.getSize().x, event.mouseMove.y + window.getPosition().y));
						}
						else if (event.mouseMove.x > window.getSize().x)
						{
							startDraggingMousePosition.x = 0;
							sf::Mouse::setPosition(sf::Vector2i(window.getPosition().x, event.mouseMove.y + window.getPosition().y));
						}
						else
						{*/
							//startDraggingMousePosition.x = event.mouseMove.x;
							editingNode->setValue(*editingValue);
						//}
						
						startDraggingMousePosition.x = event.mouseMove.x;
					}

					newConnection.vertices[1].position = sf::Vector2f(event.mouseMove.x, event.mouseMove.y);
					break;
				}
				case sf::Event::KeyPressed:
				{
					GUI::Node* newNode;
					switch (event.key.code)
					{
						case sf::Keyboard::A:
						{
							int inputTypes[2];
							int outputTypes[1];
							std::string inputStrings[2] = {"A", "B"};
							std::string outputStrings[1] = {"Result"};
							inputTypes[0] = inputTypes[1] = outputTypes[0] = GUI::Pin::Image;
							nodes.push_back(newNode = new GUI::Node("Add", inputTypes, inputStrings, 2, outputTypes, outputStrings, 1, font));
							newNode->setPosition(sf::Vector2f(50, 50));
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
							nodes.push_back(newNode = new GUI::Node("Repeat", inputTypes, inputStrings, 2, outputTypes, outputStrings, 1, font));
							newNode->setPosition(sf::Vector2f(50, 50));
							break;
						}
						case sf::Keyboard::I:
						{
							int outputTypes[1];
							std::string outputStrings[1] = {"Image"};
							outputTypes[0] = GUI::Pin::Image;
							nodes.push_back(newNode = new GUI::Node("Image", nullptr, nullptr, 0, outputTypes, outputStrings, 1, font));
							newNode->setPosition(sf::Vector2f(50, 50));
							break;
						}
						case sf::Keyboard::C:
						{
							int inputTypes[4];
							int outputTypes[1];
							std::string inputStrings[4] = {"Image size", "Square size", "Color A", "Color B"};
							std::string outputStrings[1] = {"Result"};
							inputTypes[0] = GUI::Pin::Vector2i;
							inputTypes[1] = GUI::Pin::Integer;
							inputTypes[2] = inputTypes[3] = GUI::Pin::Color;
							outputTypes[0] = GUI::Pin::Image;
							nodes.push_back(newNode = new GUI::Node("Checker", inputTypes, inputStrings, 4, outputTypes, outputStrings, 1, font));
							newNode->setPosition(sf::Vector2f(50, 50));
							break;
						}
						case sf::Keyboard::Z:
						{
							int inputTypes[6];
							int outputTypes[1];
							std::string inputStrings[6] = {"Integer", "Float", "Vector2i", "Image", "Color"};
							std::string outputStrings[1] = {"r"};
							inputTypes[0] = GUI::Pin::Integer;
							inputTypes[1] = GUI::Pin::Float;
							inputTypes[2] = GUI::Pin::Vector2i;
							inputTypes[3] = GUI::Pin::Image;
							inputTypes[4] = GUI::Pin::Color;
							outputTypes[0] = GUI::Pin::Image;
							nodes.push_back(newNode = new GUI::Node("ZZZZZZZZ", inputTypes, inputStrings, 5, outputTypes, outputStrings, 1, font));
							newNode->setPosition(sf::Vector2f(50, 50));
							break;
						}
						case sf::Keyboard::Num0:
						{
							int outputTypes[1];
							std::string outputStrings[1] = {"Value"};
							outputTypes[0] = GUI::Pin::Integer;
							nodes.push_back(newNode = new GUI::InteractiveNode("Integer", nullptr, nullptr, 0, outputTypes, outputStrings, 1, font));
							newNode->setPosition(sf::Vector2f(50, 50));
							break;
						}
						case sf::Keyboard::Num1:
						{
							int outputTypes[1];
							std::string outputStrings[1] = {"Value"};
							outputTypes[0] = GUI::Pin::Float;
							nodes.push_back(newNode = new GUI::InteractiveNode("Float", nullptr, nullptr, 0, outputTypes, outputStrings, 1, font));
							newNode->setPosition(sf::Vector2f(50, 50));
							break;
						}
						case sf::Keyboard::Num2:
						{
							int inputTypes[2];
							int outputTypes[1];
							std::string inputStrings[2] = {"X", "Y"};
							std::string outputStrings[1] = {"Vector2i"};
							inputTypes[0] = inputTypes[1] = GUI::Pin::Integer;
							outputTypes[0] = GUI::Pin::Vector2i;
							nodes.push_back(newNode = new GUI::Node("Construct Vector2i", inputTypes, inputStrings, 2, outputTypes, outputStrings, 1, font));
							newNode->setPosition(sf::Vector2f(50, 50));
							break;
						}
						case sf::Keyboard::Num3:
						{
							int inputTypes[4];
							int outputTypes[1];
							std::string inputStrings[4] = {"R", "G", "B", "A"};
							std::string outputStrings[1] = {"Color"};
							inputTypes[0] = inputTypes[1] = inputTypes[2] = inputTypes[3] = GUI::Pin::Integer;
							outputTypes[0] = GUI::Pin::Color;
							nodes.push_back(newNode = new GUI::Node("Construct Color", inputTypes, inputStrings, 4, outputTypes, outputStrings, 1, font));
							newNode->setPosition(sf::Vector2f(50, 50));
							break;
						}

						//////////////////////// other than node creation
						case sf::Keyboard::Delete:
						{
							if (selectedNode == nullptr)
								break;

							// delete all conections and node
							for (GUI::Pin* p : selectedNode->inputPins)
							{
								for (int i = 0; i < connectionLines.size(); i++) // finding a single connection line
								{
									if (connectionLines[i]->pins[0] == p || connectionLines[i]->pins[1] == p)
									{
										deleteConnectionLine(i);
										break;
									}
								}
							}
							for (GUI::Pin* p : selectedNode->outputPins)
							{
								for (int i = 0; i < connectionLines.size(); i++) // finding a single connection line
								{
									if (connectionLines[i]->pins[0] == p || connectionLines[i]->pins[1] == p)
									{
										deleteConnectionLine(i);
										break;
									}
								}
							}

							delete selectedNode; // free node memory

							int i = 0;
							for (GUI::Node* n : nodes) // remove from nodes vector
							{
								if (n == selectedNode)
								{
									nodes.erase(nodes.begin() + i);
									selectedNode = nullptr;
									break;
								}
								i++;
							}
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
			n->draw(window);
		}

		for (ConnectionLine* c : connectionLines)
			window.draw(c->vertices, 2, sf::Lines);

		if (creatingConnection)
			window.draw(newConnection.vertices, 2, sf::Lines);

		// end the current frame

		window.display();
	}

	return 0;
}