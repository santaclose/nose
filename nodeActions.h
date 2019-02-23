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

		shaders[0].setParameter("colorA", *((sf::Color*) inputPins[2]->connectedPins[0]->data));
		shaders[0].setParameter("colorB", *((sf::Color*) inputPins[3]->connectedPins[0]->data));
		shaders[0].setParameter("squareSize", *((int*) inputPins[1]->connectedPins[0]->data));

		sf::Sprite spr(outputPointer->getTexture());
		outputPointer->draw(spr, &shaders[0]);
	}

	const void LinearGradient(const std::vector<GUI::Pin*>& inputPins, const std::vector<GUI::Pin*>& outputPins)
	{
		sf::RenderTexture* outputPointer = ((sf::RenderTexture*) outputPins[0]->data);
		sf::Vector2i* imageSize = ((sf::Vector2i*) inputPins[0]->connectedPins[0]->data);

		outputPointer->create(imageSize->x, imageSize->y);

		shaders[1].setParameter("xResolution", imageSize->x);

		sf::Sprite spr(outputPointer->getTexture());
		outputPointer->draw(spr, &shaders[1]);
	}

	const void Multiply(const std::vector<GUI::Pin*>& inputPins, const std::vector<GUI::Pin*>& outputPins)
	{
		sf::RenderTexture* outputPointer = ((sf::RenderTexture*) outputPins[0]->data);
		sf::RenderTexture* a = ((sf::RenderTexture*) inputPins[0]->connectedPins[0]->data);
		sf::RenderTexture* b = ((sf::RenderTexture*) inputPins[1]->connectedPins[0]->data);

		sf::Vector2u size = a->getSize();

		outputPointer->create(size.x, size.y);

		shaders[2].setParameter("tex0", a->getTexture());
		shaders[2].setParameter("tex1", b->getTexture());

		sf::Sprite spr(outputPointer->getTexture());
		outputPointer->draw(spr, &shaders[2]);
	}

	const void Rotate90(const std::vector<GUI::Pin*>& inputPins, const std::vector<GUI::Pin*>& outputPins)
	{
		sf::RenderTexture* outputPointer = ((sf::RenderTexture*) outputPins[0]->data);
		sf::RenderTexture* a = ((sf::RenderTexture*) inputPins[0]->connectedPins[0]->data);

		sf::Vector2u size = a->getSize();
		outputPointer->create(size.y, size.x);

		shaders[3].setParameter("tex", a->getTexture());

		sf::Sprite spr(outputPointer->getTexture());
		outputPointer->draw(spr, &shaders[3]);
	}

	const void Repeat(const std::vector<GUI::Pin*>& inputPins, const std::vector<GUI::Pin*>& outputPins)
	{
		sf::RenderTexture* outputPointer = ((sf::RenderTexture*) outputPins[0]->data);
		sf::RenderTexture* a = ((sf::RenderTexture*) inputPins[0]->connectedPins[0]->data);
		sf::Vector2i* newSize = ((sf::Vector2i*) inputPins[1]->connectedPins[0]->data);

		sf::Vector2u aSize = a->getSize();
		sf::Vector2f originalSize(aSize.x, aSize.y);
		outputPointer->create(newSize->x, newSize->y);

		shaders[4].setParameter("tex", a->getTexture());
		shaders[4].setParameter("originalSize", originalSize);
		shaders[4].setParameter("newSize", sf::Vector2f(newSize->x, newSize->y));

		sf::Sprite spr(outputPointer->getTexture());
		outputPointer->draw(spr, &shaders[4]);
	}
}