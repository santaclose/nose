#include <SFML/Graphics.hpp>
#include <iostream>

int main() {
	const float winW = 800;
	const float winH = 600;

	sf::RenderWindow window(sf::VideoMode(winW, winH), "SFML Shader Example");

	// Create a texture and a sprite for the shader
	sf::RenderTexture targetTexture;
	if (!targetTexture.create(512, 512))
		return -1;

	sf::Shader shader;
	shader.loadFromFile("checker.glsl", sf::Shader::Fragment); // load the shader

	if (!shader.isAvailable()) {
		std::cout << "The shader is not available\n";
	}

	sf::Color a(0xffffffff);
	sf::Color b(0x000000ff);
	int squareSize = 80;
	shader.setParameter("colorA", a);
	shader.setParameter("colorB", b);
	shader.setParameter("squareSize", squareSize);


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
            }
		}

		// Draw the sprite with the shader on it
		window.clear(sf::Color(0xaa2233ff));

		sf::Sprite spr(targetTexture.getTexture());
		targetTexture.draw(spr, &shader);

		window.draw(spr);

		window.display();
	}

	return 0;
}