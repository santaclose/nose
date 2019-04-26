#pragma once

#define INTEGER_SET_MULTIPLIER 0.1
#define FLOAT_SET_MULTIPLIER 0.031

float* editingFloat;
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
			break; // cannot click two nodes at the same time
		}
		
		if (n->isMouseOverContent(mousePos))
		{
			if (n == outputNode)
			{
				if (outputNode->isMouseOverSaveButton(mousePos))
				{
					outputNode->saveImage();
					return;
				}
			}

			GUI::Pin* p;
			if (n->isMouseOverPin(mousePos, p))
			{
				if (!p->isDisconnected() && p->isInput) // disconnect
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
				if (!p->isInput || p->isInput && p->isDisconnected()) // can connect
				{
					newConnection.pins[0] = p;
					newConnection.vertices[0].position = p->getRectCenter();
					creatingConnection = true;
				}
				break; // cannot click two nodes at the same time
			}
			else if (n->isMouseOverInteractionComponent(mousePos, p))
			{
				std::cout << "editing" << std::endl;
				switch (p->type)
				{
					case GUI::Pin::Integer:
					{
						*editingFloat = (float) (*((int*) p->data));
						startDraggingMousePosition = mousePos;
						editingPin = p;
						break;
					}
					case GUI::Pin::Float:
					{
						*editingFloat = *((float*) p->data);
						startDraggingMousePosition = mousePos;
						editingPin = p;
						break;
					}
					case GUI::Pin::Color:
					{
						char colorString[32];
						FILE *f = popen("zenity --color-selection", "r");
						fgets(colorString, 32, f);
						std::cout << "color set to \"" << colorString << "\"" << std::endl;
						if (colorString[0] == '\0')
						{
							std::cout << "No color selected" << std::endl;
							return;
						}
						int i; for (i = 0; colorString[i] != '\n'; i++); colorString[i] = '\0'; // remove endline
						std::string redString = "", greenString = "", blueString = "", alphaString = "";
						bool usingAlpha = false;
						for (i = 0; colorString[i] != '('; i++)
						{
							if (colorString[i] == 'a')
								usingAlpha = true;
						}
						i++;
						for ( ; colorString[i] != ',' ; i++) redString += colorString[i]; i++;
						for ( ; colorString[i] != ',' ; i++) greenString += colorString[i]; i++;
						if (!usingAlpha)
						{
							for ( ; colorString[i] != ')' ; i++) blueString += colorString[i];
							alphaString = "1.0";
						}
						else
						{
							for ( ; colorString[i] != ',' ; i++) blueString += colorString[i]; i++;
							for ( ; colorString[i] != ')' ; i++) alphaString += colorString[i];

						}

						sf::Color newColor(std::stoi(redString), std::stoi(greenString), std::stoi(blueString), (int) (std::stof(alphaString) * 255.0));

						std::cout << "Red: " << unsigned(newColor.r) << std::endl;
						std::cout << "Green: " << unsigned(newColor.g) << std::endl;
						std::cout << "Blue: " << unsigned(newColor.b) << std::endl;
						std::cout << "Alpha: " << unsigned(newColor.a) << std::endl;

						p->setValue(&newColor);

						break;
					}
					case GUI::Pin::Image:
					{
						char filePath[1024];
						FILE *f = popen("zenity --file-selection", "r");
						fgets(filePath, 1024, f);
						int i; for (i = 0; filePath[i] != '\n'; i++); filePath[i] = '\0'; // remove endline
						std::cout << "image set to \"" << filePath << "\"\n";

						sf::Texture tx;
						if (!tx.loadFromFile(filePath))
						{
							std::cout << "Could not open the image" << std::endl;
							return;
						}
						loadImageShader.setParameter("tx", tx);

						sf::Sprite spr(tx);

						sf::Vector2u txSize = tx.getSize();

						sf::RenderTexture* pointer = (sf::RenderTexture*) p->data;

						pointer->create(txSize.x, txSize.y);
						pointer->draw(spr, &loadImageShader);

						std::string fileName = "";
						for (int j = 0; filePath[j] != '\0'; j++)
						{
							fileName += filePath[j];
							if (filePath[j] == '/')
							{
								fileName = "";
							}
						}
						p->setInteractiveText(fileName);
						p->dataAvailable = true;

						p->setValue(&spr); // nothing but activate the node

						break;
					}
				}
				break; // cannot click two nodes at the same time
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
						a->pins[1]->establishConnection(&a->vertices[1], a->pins[0], true); // last boolean tells if it is the second call

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
				break;
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
			*editingFloat += disp * FLOAT_SET_MULTIPLIER;
		}
		else if (editingPin->type == GUI::Pin::Integer)
		{
			*editingFloat += disp * INTEGER_SET_MULTIPLIER;
		}

		if (editingPin->type == GUI::Pin::Float || editingPin->type == GUI::Pin::Integer)
		{
			editingPin->setValue(editingFloat);
		}
		//editingPin->parentNode->activate();

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
		case sf::Keyboard::I:
		{
			if (searching)
				break;
			nodes.push_back(newNode = new GUI::Node("Invert", NodeActions::Invert));
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
		case sf::Keyboard::W:
		{
			if (searching)
				break;
			nodes.push_back(newNode = new GUI::Node("Flip", NodeActions::Flip));
			break;
		}
		case sf::Keyboard::Q:
		{
			if (searching)
				break;
			nodes.push_back(newNode = new GUI::Node("Image", NodeActions::Image));
			break;
		}
		case sf::Keyboard::Numpad0:
		case sf::Keyboard::Num0:
		{
			if (searching)
				break;
			nodes.push_back(newNode = new GUI::Node("Integer", NodeActions::Integer));
			break;
		}
		case sf::Keyboard::Numpad1:
		case sf::Keyboard::Num1:
		{
			if (searching)
				break;
			nodes.push_back(newNode = new GUI::Node("Float", NodeActions::Float));
			break;
		}
		case sf::Keyboard::Numpad2:
		case sf::Keyboard::Num2:
		{
			if (searching)
				break;
			nodes.push_back(newNode = new GUI::Node("Construct Vector2i", NodeActions::VectoriFromIntegers));
			break;
		}
		case sf::Keyboard::Numpad3:
		case sf::Keyboard::Num3:
		{
			if (searching)
				break;
			nodes.push_back(newNode = new GUI::Node("Construct Color", NodeActions::ColorFromIntegers));
			break;
		}

		//////////////////////// other than node creation
		case sf::Keyboard::X:
		case sf::Keyboard::Delete:
		case sf::Keyboard::Backspace:
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