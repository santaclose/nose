
namespace GUI
{
	bool isVectorOverRect(const sf::Vector2f& vector, const sf::RectangleShape& rect);
	bool isVectorOverRect(const sf::Vector2f& vector, const sf::Vector2f& rectPosition, const sf::Vector2f& rectSize);

	class Pin 
	{
	private:
		void assignColor(const int inputType, sf::RectangleShape& rect);
	public:
		int type;
		bool isInput;
		sf::Text text;
		int stringOutputOffsetX;
		sf::RectangleShape rect;
		sf::Vertex* connectionVertex; // nullptr if not connected
	//public:
		enum Type {

		    Integer,
		    Float,
		    Vector2i,
		    Recti,
		    Image
		};
		static const Type pinTypes;

		Pin(const std::string& name, const int type, const bool isInput, const sf::Font& font);
		bool isMouseOver(sf::Vector2f& mousePos, sf::Vector2f& rectCenter);
		void establishConnection(sf::Vertex* newConnectionVertex);
	};

	class Node
	{
	public:
		sf::Text title;
		sf::RectangleShape barRect;
		sf::RectangleShape contentRect;
		std::vector<Pin*> inputPins;
		std::vector<Pin*> outputPins;

		Node(const std::string& name, const int* inputTypes, const std::string* inputNames, const int inputCount, const int* outputTypes, const std::string* outputNames, const int outputCount, const sf::Vector2f& initialPosition, const sf::Font& font);
		~Node();
		void setPosition(const sf::Vector2f& newPosition);
		sf::Vector2f getPosition();
		bool isMouseOverBar(sf::Vector2f& mousePos);
		bool isMouseOverContent(sf::Vector2f& mousePos);
		bool isMouseOverUnconnectedPin(sf::Vector2f& mousePos, sf::Vector2f& pinRectCenterReturn, Pin*& resultingPin);
	};
}