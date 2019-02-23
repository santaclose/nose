#pragma once

float editingFloat;
GUI::Pin* editingPin = nullptr;

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
			else if (n->isMouseOverInteractionComponent(mousePos, p))
			{
				std::cout << "editing" << std::endl;
				if (p->type == GUI::Pin::Integer)
					editingFloat = (float)*((int*)n->outputPins[0]->data);
				else
					editingFloat = *((float*)n->outputPins[0]->data);
				startDraggingMousePosition = mousePos;
				editingPin = p;
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

	editingPin = nullptr;
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

	if (editingPin != nullptr) // user is setting a value
	{
		int disp = newPosition.x - startDraggingMousePosition.x;
		if (editingPin->type == GUI::Pin::Float)
		{
			editingFloat += disp * 0.031;
		}
		else if (editingPin->type == GUI::Pin::Integer)
		{
			editingFloat += disp * 0.1;
		}
		
		editingPin->setValue(&editingFloat);
		editingPin->parentNode->activate();

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
			//editingPin->setValue(&editingFloat);
		//}
		
		startDraggingMousePosition.x = newPosition.x;
	}

	newConnection.vertices[1].position = sf::Vector2f(newPosition.x, newPosition.y);
}

inline void EventKeyPressed(sf::Keyboard::Key keyCode)
{
	GUI::Node* newNode = nullptr;
	switch (keyCode)
	{
		case sf::Keyboard::Space:
		{
			searching = true;
			break;
		}
		case sf::Keyboard::Escape:
		{
			searchBuffer[0] = '\0';
			searchText.setString(searchBuffer);
			searchBufferCurrentChar = 0;
			searching = false;
			break;
		}
		case sf::Keyboard::R:
		{
			if (searching)
				break;
			nodes.push_back(newNode = new GUI::Node("Repeat", NodeActions::Repeat));
			break;
		}
		case sf::Keyboard::C:
		{
			if (searching)
				break;
			nodes.push_back(newNode = new GUI::Node("Checker", NodeActions::Checker));
			break;
		}
		case sf::Keyboard::G:
		{
			if (searching)
				break;
			nodes.push_back(newNode = new GUI::Node("Linear Gradient", NodeActions::LinearGradient));
			break;
		}
		case sf::Keyboard::M:
		{
			if (searching)
				break;
			nodes.push_back(newNode = new GUI::Node("Multiply", NodeActions::Multiply));
			break;
		}
		case sf::Keyboard::O:
		{
			if (searching)
				break;
			nodes.push_back(newNode = new GUI::Node("Rotate 90", NodeActions::Rotate90));
			break;
		}
		case sf::Keyboard::Numpad2:
		{
			if (searching)
				break;
			nodes.push_back(newNode = new GUI::Node("Construct Vector2i", NodeActions::VectoriFromIntegers));
			break;
		}
		case sf::Keyboard::Numpad3:
		{
			if (searching)
				break;
			nodes.push_back(newNode = new GUI::Node("Construct Color", NodeActions::ColorFromIntegers));
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
	if (newNode != nullptr)
		newNode->setPosition(sf::Vector2f(50, 50));
}

inline void EventTextEntered(char c)
{
	if (searching) // type on search box
	{
		if (c == '\b')
		{
			if (searchBufferCurrentChar > 0)
			{
				searchBuffer[searchBufferCurrentChar-1] = '\0';
				searchBufferCurrentChar--;
			}
		}
		else
		{
			if (searchBufferCurrentChar == SEARCH_BAR_BUFFER_SIZE - 1)
				return;
			if (c == ' ' && searchBuffer[0] == '\0')
				return;
			searchBuffer[searchBufferCurrentChar] = c;
			searchBuffer[searchBufferCurrentChar+1] = '\0';
			searchBufferCurrentChar++;
		}
		//std::cout << searchBuffer << std::endl;
		searchText.setString(searchBuffer);
	}
}