#pragma once

void deleteConnectionLine(int& i)
{
	connectionLines[i]->pins[0]->disconnectFrom(connectionLines[i]->pins[1]);
	connectionLines[i]->pins[1]->disconnectFrom(connectionLines[i]->pins[0]);
	delete connectionLines[i];
	connectionLines.erase(connectionLines.begin() + i);
}

inline void EventMouseLeftDown(sf::Vector2f& mousePos)
{
	for (GUI::Node* n : nodes)
	{
		if (!n->isMouseOver(mousePos)) // mouse not over this node
			continue;

		if (n->isMouseOverBar(mousePos))
		{
			draggingNode = n;
			startDraggingMousePosition = mousePos;
			startDraggingNodePosition = n->getPosition();
			continue;
		}
		
		if (n->isMouseOverContent(mousePos))
		{
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

inline void EventMouseLeftUp(sf::Vector2f& mousePos)
{
	if (creatingConnection)
	{
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

						a->pins[0]->establishConnection(&a->vertices[0], a->pins[1], false);
						a->pins[1]->establishConnection(&a->vertices[1], a->pins[0], true);

						// LOG
						/*std::cout << "CONNECTED PINS TO " << a->pins[0]->parentNode->title.getString().toAnsiString() << std::endl;
						for (auto elpin : a->pins[0]->connectedPins)
							std::cout << elpin << std::endl;
						std::cout << "CONNECTED PINS TO " << a->pins[1]->parentNode->title.getString().toAnsiString() << std::endl;
						for (auto elpin : a->pins[1]->connectedPins)
							std::cout << elpin << std::endl;*/
						
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

inline void EventMouseRightDown(sf::Vector2f& mousePos)
{
	for (GUI::Node* n : nodes)
	{
		if (!n->isMouseOver(mousePos))
			continue;

		if (n->isMouseOverBar(mousePos) && !n->isOutputNode)
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
			break;
		}
	}
}

inline void EventMouseMoved(sf::Event::MouseMoveEvent& newPosition)
{
	if (draggingNode != nullptr) // user is dragging a node
	{
		// compute displacement
		sf::Vector2f displacement(newPosition.x, newPosition.y);
		displacement -= startDraggingMousePosition;

		// update position
		sf::Vector2f currentPos = startDraggingNodePosition + displacement;
		draggingNode->setPosition(currentPos);
	}

	if (editingNode != nullptr) // user is setting a value
	{
		int disp = newPosition.x - startDraggingMousePosition.x;
		*editingValue += disp*disp*disp * (editingType == GUI::Pin::Float ? 0.01 : 0.1); // drag function

		editingNode->activate();

		/*if (newPosition.x < 0.0)
		{
			startDraggingMousePosition.x = window.getSize().x;
			sf::Mouse::setPosition(sf::Vector2i(window.getPosition().x + window.getSize().x, newPosition.y + window.getPosition().y));
		}
		else if (newPosition.x > window.getSize().x)
		{
			startDraggingMousePosition.x = 0;
			sf::Mouse::setPosition(sf::Vector2i(window.getPosition().x, newPosition.y + window.getPosition().y));
		}
		else
		{*/
			//startDraggingMousePosition.x = newPosition.x;
			editingNode->setValue(*editingValue);
		//}
		
		startDraggingMousePosition.x = newPosition.x;
	}

	newConnection.vertices[1].position = sf::Vector2f(newPosition.x, newPosition.y);
}

inline void EventKeyPressed(sf::Keyboard::Key keyCode)
{
	GUI::Node* newNode;
	switch (keyCode)
	{
		case sf::Keyboard::A:
		{
			int inputTypes[2];
			int outputTypes[1];
			std::string inputStrings[2] = {"A", "B"};
			std::string outputStrings[1] = {"Result"};
			inputTypes[0] = inputTypes[1] = outputTypes[0] = GUI::Pin::Image;
			nodes.push_back(newNode = new GUI::Node("Add", inputTypes, inputStrings, 2, outputTypes, outputStrings, 1, nullptr, font));
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
			nodes.push_back(newNode = new GUI::Node("Repeat", inputTypes, inputStrings, 2, outputTypes, outputStrings, 1, NodeActions::Repeat, font));
			newNode->setPosition(sf::Vector2f(50, 50));
			break;
		}
		case sf::Keyboard::I:
		{
			int outputTypes[1];
			std::string outputStrings[1] = {"Image"};
			outputTypes[0] = GUI::Pin::Image;
			nodes.push_back(newNode = new GUI::Node("Image", nullptr, nullptr, 0, outputTypes, outputStrings, 1, nullptr, font));
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
			nodes.push_back(newNode = new GUI::Node("Checker", inputTypes, inputStrings, 4, outputTypes, outputStrings, 1, NodeActions::Checker, font));
			newNode->setPosition(sf::Vector2f(50, 50));
			break;
		}
		case sf::Keyboard::G:
		{
			int inputTypes[1];
			int outputTypes[1];
			std::string inputStrings[1] = {"Image size"};
			std::string outputStrings[1] = {"Result"};
			inputTypes[0] = GUI::Pin::Vector2i;
			outputTypes[0] = GUI::Pin::Image;
			nodes.push_back(newNode = new GUI::Node("Linear Gradient", inputTypes, inputStrings, 1, outputTypes, outputStrings, 1, NodeActions::LinearGradient, font));
			newNode->setPosition(sf::Vector2f(50, 50));
			break;
		}
		case sf::Keyboard::M:
		{
			int inputTypes[2];
			int outputTypes[1];
			std::string inputStrings[2] = {"A", "B"};
			std::string outputStrings[1] = {"Result"};
			inputTypes[0] = inputTypes[1] = GUI::Pin::Image;
			outputTypes[0] = GUI::Pin::Image;
			nodes.push_back(newNode = new GUI::Node("Multiply", inputTypes, inputStrings, 2, outputTypes, outputStrings, 1, NodeActions::Multiply, font));
			newNode->setPosition(sf::Vector2f(50, 50));
			break;
		}
		case sf::Keyboard::O:
		{
			int inputTypes[2];
			int outputTypes[1];
			std::string inputStrings[1] = {"Image"};
			std::string outputStrings[1] = {"Result"};
			inputTypes[0] = GUI::Pin::Image;
			outputTypes[0] = GUI::Pin::Image;
			nodes.push_back(newNode = new GUI::Node("Rotate 90", inputTypes, inputStrings, 1, outputTypes, outputStrings, 1, NodeActions::Rotate90, font));
			newNode->setPosition(sf::Vector2f(50, 50));
			break;
		}/*
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
			nodes.push_back(newNode = new GUI::Node("ZZZZZZZZ", inputTypes, inputStrings, 5, outputTypes, outputStrings, 1, nullptr, font));
			newNode->setPosition(sf::Vector2f(50, 50));
			break;
		}*/
		case sf::Keyboard::Numpad0:
		{
			int outputTypes[1];
			std::string outputStrings[1] = {"Value"};
			outputTypes[0] = GUI::Pin::Integer;
			nodes.push_back(newNode = new GUI::InteractiveNode("Integer", nullptr, nullptr, 0, outputTypes, outputStrings, 1, nullptr, font));
			newNode->setPosition(sf::Vector2f(50, 50));
			break;
		}
		case sf::Keyboard::Numpad1:
		{
			int outputTypes[1];
			std::string outputStrings[1] = {"Value"};
			outputTypes[0] = GUI::Pin::Float;
			nodes.push_back(newNode = new GUI::InteractiveNode("Float", nullptr, nullptr, 0, outputTypes, outputStrings, 1, nullptr, font));
			newNode->setPosition(sf::Vector2f(50, 50));
			break;
		}
		case sf::Keyboard::Numpad2:
		{
			int inputTypes[2];
			int outputTypes[1];
			std::string inputStrings[2] = {"X", "Y"};
			std::string outputStrings[1] = {"Vector2i"};
			inputTypes[0] = inputTypes[1] = GUI::Pin::Integer;
			outputTypes[0] = GUI::Pin::Vector2i;
			nodes.push_back(newNode = new GUI::Node("Construct Vector2i", inputTypes, inputStrings, 2, outputTypes, outputStrings, 1, NodeActions::VectoriFromIntegers, font));
			newNode->setPosition(sf::Vector2f(50, 50));
			break;
		}
		case sf::Keyboard::Numpad3:
		{
			int inputTypes[4];
			int outputTypes[1];
			std::string inputStrings[4] = {"R", "G", "B", "A"};
			std::string outputStrings[1] = {"Color"};
			inputTypes[0] = inputTypes[1] = inputTypes[2] = inputTypes[3] = GUI::Pin::Integer;
			outputTypes[0] = GUI::Pin::Color;
			nodes.push_back(newNode = new GUI::Node("Construct Color", inputTypes, inputStrings, 4, outputTypes, outputStrings, 1, NodeActions::ColorFromIntegers, font));
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
						i--; // connection line deleted, if we go to the next one we skip one element
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
						i--; // connection line deleted, if we go to the next one we skip one element
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

inline void EventTextEntered(char c)
{
	if (editingNode == nullptr)
		return;

	if (currentInputIndex >= INPUT_BUFFER_SIZE)
		return;
}