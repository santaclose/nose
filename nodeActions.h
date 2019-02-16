namespace NodeActions
{
	const void VectoriFromIntegers(const std::vector<GUI::Pin*>& inputPins, const std::vector<GUI::Pin*>& outputPins)
	{
		sf::Vector2i* outputPointer = ((sf::Vector2i*)outputPins[0]->data);
		outputPointer->x = *((int*) inputPins[0]->connectedPins[0]->data);
		outputPointer->y = *((int*) inputPins[1]->connectedPins[0]->data);
	}

	const void ColorFromIntegers(const std::vector<GUI::Pin*>& inputPins, const std::vector<GUI::Pin*>& outputPins)
	{
		sf::Color* outputPointer = ((sf::Color*)outputPins[0]->data);
		outputPointer->r = *((int*) inputPins[0]->connectedPins[0]->data);
		outputPointer->g = *((int*) inputPins[1]->connectedPins[0]->data);
		outputPointer->b = *((int*) inputPins[2]->connectedPins[0]->data);
		outputPointer->a = *((int*) inputPins[3]->connectedPins[0]->data);
	}

	const void Checker(const std::vector<GUI::Pin*>& inputPins, const std::vector<GUI::Pin*>& outputPins)
	{
		sf::RenderTexture* outputPointer = ((sf::RenderTexture*) outputPins[0]->data);
		sf::Vector2i* imageSize = ((sf::Vector2i*) inputPins[0]->connectedPins[0]->data);

		outputPointer->create(imageSize->x, imageSize->y);

		sf::Shader shader;
		shader.loadFromFile("shaders/checker.glsl", sf::Shader::Fragment); // load the shader
		shader.setParameter("colorA", *((sf::Color*) inputPins[2]->connectedPins[0]->data));
		shader.setParameter("colorB", *((sf::Color*) inputPins[3]->connectedPins[0]->data));
		shader.setParameter("squareSize", *((int*) inputPins[1]->connectedPins[0]->data));

		sf::Sprite spr(outputPointer->getTexture());
		outputPointer->draw(spr, &shader);
	}

	const void LinearGradient(const std::vector<GUI::Pin*>& inputPins, const std::vector<GUI::Pin*>& outputPins)
	{
		sf::RenderTexture* outputPointer = ((sf::RenderTexture*) outputPins[0]->data);
		sf::Vector2i* imageSize = ((sf::Vector2i*) inputPins[0]->connectedPins[0]->data);

		outputPointer->create(imageSize->x, imageSize->y);

		sf::Shader shader;
		shader.loadFromFile("shaders/gradient.glsl", sf::Shader::Fragment); // load the shader
		shader.setParameter("xResolution", imageSize->x);

		sf::Sprite spr(outputPointer->getTexture());
		outputPointer->draw(spr, &shader);
	}

	const void Multiply(const std::vector<GUI::Pin*>& inputPins, const std::vector<GUI::Pin*>& outputPins)
	{
		sf::RenderTexture* outputPointer = ((sf::RenderTexture*) outputPins[0]->data);
		sf::RenderTexture* a = ((sf::RenderTexture*) inputPins[0]->connectedPins[0]->data);
		sf::RenderTexture* b = ((sf::RenderTexture*) inputPins[1]->connectedPins[0]->data);

		sf::Vector2u size = a->getSize();
		outputPointer->create(size.x, size.y);

		sf::Shader shader;
		shader.loadFromFile("shaders/multiply.glsl", sf::Shader::Fragment); // load the shader
		shader.setParameter("tex0", a->getTexture());
		shader.setParameter("tex1", b->getTexture());

		sf::Sprite spr(outputPointer->getTexture());
		outputPointer->draw(spr, &shader);
	}

	//////////// trash
	/*sf::Image* Add (const sf::Image& a, const sf::Image& b, bool clamp)
	{
		sf::Vector2u size = a.getSize();

		sf::Image* result = new sf::Image();
		result->create(size.x, size.y, sf::Color::Black);

		if (clamp)
		{
			for (unsigned int i = 0; i < size.x; i++)
			{
				for (unsigned int j = 0; j < size.y; j++)
				{
					// the operator defined for pixel addition clamps to 255
					result->setPixel(i, j, a.getPixel(i, j) + b.getPixel(i, j));
				}
			}
		}
		else
		{
			for (unsigned int i = 0; i < size.x; i++)
			{
				for (unsigned int j = 0; j < size.y; j++)
				{
					sf::Color aPixel = a.getPixel(i, j);
					sf::Color bPixel = b.getPixel(i, j);
					sf::Color resultColor(aPixel.r + bPixel.r, aPixel.g + bPixel.g, aPixel.b + bPixel.b, aPixel.a + bPixel.a);
					result->setPixel(i, j, resultColor);
				}
			}
		}
		return result;
	}

	sf::Image* Tile (const sf::Image& image, const sf::Vector2u& outputSize)
	{
		sf::Vector2u originalSize = image.getSize();

		sf::Image* result = new sf::Image();
		result->create(outputSize.x, outputSize.y, sf::Color::Black);

		for (unsigned int i = 0; i < outputSize.x; i++)
		{
			for (unsigned int j = 0; j < outputSize.y; j++)
			{
				result->setPixel(i, j, image.getPixel(i % originalSize.x, j % originalSize.y));
			}
		}
		return result;
	}*/
}