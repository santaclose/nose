#pragma once

namespace GUI
{
	bool isPointOverRect(const sf::Vector2f& vector, const sf::RectangleShape& rect);
	bool isPointOverRect(const sf::Vector2f& vector, const sf::Vector2f& rectPosition, const sf::Vector2f& rectSize);

	class Node;

	class Pin 
	{
	private:
		void assignColor(const int inputType, sf::RectangleShape& rect);
	public:
		int stringOutputOffsetX;	//  only for output pins
		int type;
		bool isInput;
		sf::Text text;
		bool dataAvailable;			//  only for output pins
		sf::RectangleShape rect;
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

		Pin(const std::string& name, const int type, const bool isInput, const sf::Font& font, Node* parentNode);
		~Pin();
		bool isMouseOver(sf::Vector2f& mousePos);
		bool isDisconnected();
		void establishConnection(sf::Vertex* newConnectionVertex, Pin* other, bool isSecondConnection);
		sf::Vector2f getRectCenter();
		void disconnectFrom(Pin*& p);
	};

	class Node
	{
	public:
		bool isInteractive;
		bool isOutputNode;
		sf::Text title;
		sf::RectangleShape barRect;
		sf::RectangleShape contentRect;
		std::vector<Pin*> inputPins;
		std::vector<Pin*> outputPins;

		const void (*action)(const std::vector<Pin*>& inputPins, const std::vector<Pin*>& outputPins);

		Node(const std::string& name, const int* inputTypes, const std::string* inputNames, const int inputCount, const int* outputTypes, const std::string* outputNames, const int outputCount, const void (*action)(const std::vector<Pin*>& inputPins, const std::vector<Pin*>& outputPins), const sf::Font& font);
		~Node();
		virtual void activate();
		virtual void setPosition(const sf::Vector2f& newPosition);
		sf::Vector2f getPosition();
		bool isMouseOver(sf::Vector2f& mousePos);
		bool isMouseOverBar(sf::Vector2f& mousePos);
		bool isMouseOverContent(sf::Vector2f& mousePos);
		bool isMouseOverPin(sf::Vector2f& mousePos, Pin*& resultingPin);
		bool hasAllInputData();
		virtual bool isMouseOverInteractionComponent(sf::Vector2f& mousePos); // implemented for normal nodes to return false always
		void paintAsSelected();
		void paintAsUnselected();
		virtual void draw(sf::RenderWindow& window);
	};

	class InteractiveNode : public Node
	{
	public:
		sf::RectangleShape interactionComponentRect;
		sf::Text interactionComponentText;

		InteractiveNode(const std::string& name, const int* inputTypes, const std::string* inputNames, const int inputCount, const int* outputTypes, const std::string* outputNames, const int outputCount, const void (*action)(const std::vector<Pin*>& inputPins, const std::vector<Pin*>& outputPins), const sf::Font& font);

		void activate();
		void setPosition(const sf::Vector2f& newPosition) override;
		bool isMouseOverInteractionComponent(sf::Vector2f& mousePos) override;
		void setValue(float value);
		void draw(sf::RenderWindow& window) override;
	};

	class OutputNode : public Node
	{
	private:
		sf::RenderWindow* window;
	public:
		OutputNode(const std::string& name, const int* inputTypes, const std::string* inputNames, const int inputCount, const int* outputTypes, const std::string* outputNames, const int outputCount, const void (*action)(const std::vector<Pin*>& inputPins, const std::vector<Pin*>& outputPins), const sf::Font& font, sf::RenderWindow* window);

		void activate();
	};
}