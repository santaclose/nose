#pragma once

namespace GUI
{
	static sf::Font font;
	bool isPointOverRect(const sf::Vector2f& vector, const sf::RectangleShape& rect);
	bool isPointOverRect(const sf::Vector2f& vector, const sf::Vector2f& rectPosition, const sf::Vector2f& rectSize);

	class Node;

	class Pin 
	{
	private:
		void assignColor(const int inputType, sf::RectangleShape& rect);
	public:
		int stringOutputOffsetX;	//  only for setting output pins text position
		int type;
		bool isInput;
		sf::Text text;
		bool dataAvailable;			//  using for both input and output pins
		sf::RectangleShape rect;
		sf::RectangleShape interactiveRect;
		Node* parentNode;
		void* data; // Integer, Float, Image or any type //  only for output pins

		std::vector<sf::Vertex*> connectionVertices;
		std::vector<Pin*> connectedPins;
	//public:
		enum Type {

			Integer,
			Float,
			Vector2i,
			Image,
			Color
		};
		static const Type pinTypes;

		Pin(const std::string& name, const int type, const bool isInput, Node* parentNode);
		~Pin();
		void setPosition(const sf::Vector2f& newPosition, const int i); // receives node's top left corner as new position, i contains the pin index
		bool hasDataAvailable(); // only for input pins. checks if connected pin has data available
		bool isMouseOver(sf::Vector2f& mousePos);
		bool isMouseOverInteractionComponent(sf::Vector2f& mousePos);
		bool isDisconnected();
		void setValue(const void* data);
		void establishConnection(sf::Vertex* newConnectionVertex, Pin* other, bool isSecondConnection);
		sf::Vector2f getRectCenter();
		void disconnectFrom(Pin*& p);
		void draw(sf::RenderWindow& window);
	};

	class Node
	{
	public:
		bool isOutputNode;
		sf::Text title;
		sf::RectangleShape barRect;
		sf::RectangleShape contentRect;
		std::vector<Pin*> inputPins;
		std::vector<Pin*> outputPins;

		const void (*action)(const std::vector<Pin*>& inputPins, const std::vector<Pin*>& outputPins);

		//Node(const std::string& name, const int* inputTypes, const std::string* inputNames, const int inputCount, const int* outputTypes, const std::string* outputNames, const int outputCount, const void (*action)(const std::vector<Pin*>& inputPins, const std::vector<Pin*>& outputPins), const sf::Font& font);

		Node(const std::string& name, const void (*action)(const std::vector<Pin*>& inputPins, const std::vector<Pin*>& outputPins));

		~Node();
		virtual void activate();
		void setPosition(const sf::Vector2f& newPosition); // receives node's top left corner as new position
		sf::Vector2f getPosition();
		bool isMouseOver(sf::Vector2f& mousePos);
		bool isMouseOverBar(sf::Vector2f& mousePos);
		bool isMouseOverContent(sf::Vector2f& mousePos);
		bool isMouseOverPin(sf::Vector2f& mousePos, Pin*& resultingPin);
		bool hasAllInputData();
		bool isMouseOverInteractionComponent(sf::Vector2f& mousePos, Pin*& resultingPin);
		void paintAsSelected();
		void paintAsUnselected();
		void draw(sf::RenderWindow& window);
	};

	/*class InteractiveNode : public Node
	{
	public:
		sf::RectangleShape interactionComponentRect;
		sf::Text interactionComponentText;

		//InteractiveNode(const std::string& name, const int* inputTypes, const std::string* inputNames, const int inputCount, const int* outputTypes, const std::string* outputNames, const int outputCount, const void (*action)(const std::vector<Pin*>& inputPins, const std::vector<Pin*>& outputPins), const sf::Font& font);

		InteractiveNode(const std::string& name, const void (*action)(const std::vector<Pin*>& inputPins, const std::vector<Pin*>& outputPins));

		void activate();
		void setPosition(const sf::Vector2f& newPosition) override;
		bool isMouseOverInteractionComponent(sf::Vector2f& mousePos) override;
		void setValue(float value);
		void draw(sf::RenderWindow& window) override;
	};*/

	class OutputNode : public Node
	{
	private:
		sf::RenderWindow* window;
	public:
		//OutputNode(const std::string& name, const int* inputTypes, const std::string* inputNames, const int inputCount, const int* outputTypes, const std::string* outputNames, const int outputCount, const void (*action)(const std::vector<Pin*>& inputPins, const std::vector<Pin*>& outputPins), const sf::Font& font, sf::RenderWindow* window);

		OutputNode(const std::string& name, const void (*action)(const std::vector<Pin*>& inputPins, const std::vector<Pin*>& outputPins), sf::RenderWindow* window);

		void activate();
	};
}