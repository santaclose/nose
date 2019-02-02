#include <SFML/Graphics.hpp>
#include <iostream>

int main() {
	const float winW = 800;
	const float winH = 600;

	sf::RenderWindow window(sf::VideoMode(winW, winH), "SFML Shader Example");

	// Create a texture and a sprite for the shader
	sf::RenderTexture targetTexture;
	if (!targetTexture.create(1024, 1024))
		return -1;

	sf::Shader shader;
	shader.loadFromFile("add.glsl", sf::Shader::Fragment); // load the shader

	if (!shader.isAvailable()) {
		std::cout << "The shader is not available\n";
	}

	sf::Texture textureA, textureB;
	if (!textureA.loadFromFile("mulA.jpg"))
	{
		std::cout << "Image file not available\n";
	}
	if (!textureB.loadFromFile("mulB.jpg"))
	{
		std::cout << "Image file not available\n";
	}



	shader.setParameter("tex0", textureA);
	shader.setParameter("tex1", textureB);


	while (window.isOpen()) {
		// Event handling
		sf::Event event;

		while (window.pollEvent(event)) {
			switch (event.type)
            {
                case sf::Event::Closed:
                {
                    window.close();
                    break;
                }
                case sf::Event::MouseButtonPressed:
                {
                    if (event.mouseButton.button == sf::Mouse::Left)
                    {
                    	sf::Image result = targetTexture.getTexture().copyToImage();
                    	result.saveToFile("addResult.png");
                    }
                }
            }
		}

		// Draw the sprite with the shader on it
		window.clear(sf::Color(0xaa2233ff));

		sf::Vector2i mousePos = sf::Mouse::getPosition(window);
		shader.setParameter("a", mousePos.x);

		sf::Sprite spr(targetTexture.getTexture());
		targetTexture.draw(spr, &shader);

		window.draw(spr);

		window.display();
	}

	return 0;
}