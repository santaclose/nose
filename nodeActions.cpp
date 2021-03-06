namespace NodeActions
{
	const void Integer(const std::vector<GUI::Pin*>& inputPins, const std::vector<GUI::Pin*>& outputPins)
	{
		*((int*) outputPins[0]->data) = *((int*) inputPins[0]->getData());
	}
	const void Float(const std::vector<GUI::Pin*>& inputPins, const std::vector<GUI::Pin*>& outputPins)
	{
		*((float*) outputPins[0]->data) = *((float*) inputPins[0]->getData());
	}
	const void Image(const std::vector<GUI::Pin*>& inputPins, const std::vector<GUI::Pin*>& outputPins)
	{
		sf::RenderTexture* outputPointer = ((sf::RenderTexture*) outputPins[0]->data);
		sf::Vector2i* outputSize = ((sf::Vector2i*) outputPins[1]->data);
		sf::RenderTexture* a = ((sf::RenderTexture*) inputPins[0]->getData());

		sf::Vector2u size = a->getSize();

		outputPointer->create(size.x, size.y);

		loadImageShader.setParameter("tx", a->getTexture());

		sf::Sprite spr(a->getTexture());
		outputPointer->draw(spr, &loadImageShader);
		*outputSize = sf::Vector2i(size.x, size.y);
	}

	const void Flip(const std::vector<GUI::Pin*>& inputPins, const std::vector<GUI::Pin*>& outputPins)
	{
		sf::RenderTexture* outputPointer = ((sf::RenderTexture*) outputPins[0]->data);
		sf::RenderTexture* a = ((sf::RenderTexture*) inputPins[0]->getData());
		int* xAxis = ((int*) inputPins[1]->getData());

		sf::Vector2u size = a->getSize();

		outputPointer->create(size.x, size.y);

		shaders[6].setParameter("tx", a->getTexture());
		shaders[6].setParameter("xAxis", *xAxis % 2);

		sf::Sprite spr(a->getTexture());
		outputPointer->draw(spr, &shaders[6]);
	}

	const void VectoriFromIntegers(const std::vector<GUI::Pin*>& inputPins, const std::vector<GUI::Pin*>& outputPins)
	{
		sf::Vector2i* outputPointer = ((sf::Vector2i*) outputPins[0]->data);
		outputPointer->x = *((int*) inputPins[0]->getData());
		outputPointer->y = *((int*) inputPins[1]->getData());
	}

	const void ColorFromIntegers(const std::vector<GUI::Pin*>& inputPins, const std::vector<GUI::Pin*>& outputPins)
	{
		sf::Color* outputPointer = ((sf::Color*)outputPins[0]->data);
		outputPointer->r = *((int*) inputPins[0]->getData());
		outputPointer->g = *((int*) inputPins[1]->getData());
		outputPointer->b = *((int*) inputPins[2]->getData());
		outputPointer->a = *((int*) inputPins[3]->getData());
	}

	const void Checker(const std::vector<GUI::Pin*>& inputPins, const std::vector<GUI::Pin*>& outputPins)
	{
		sf::RenderTexture* outputPointer = ((sf::RenderTexture*) outputPins[0]->data);
		sf::Vector2i* imageSize = ((sf::Vector2i*) inputPins[0]->getData());

		outputPointer->create(imageSize->x, imageSize->y);

		shaders[0].setParameter("colorA", *((sf::Color*) inputPins[2]->getData()));
		shaders[0].setParameter("colorB", *((sf::Color*) inputPins[3]->getData()));
		shaders[0].setParameter("squareSize", *((int*) inputPins[1]->getData()));

		sf::Sprite spr(outputPointer->getTexture());
		outputPointer->draw(spr, &shaders[0]);
	}

	const void LinearGradient(const std::vector<GUI::Pin*>& inputPins, const std::vector<GUI::Pin*>& outputPins)
	{
		sf::RenderTexture* outputPointer = ((sf::RenderTexture*) outputPins[0]->data);
		sf::Vector2i* imageSize = ((sf::Vector2i*) inputPins[0]->getData());

		outputPointer->create(imageSize->x, imageSize->y);

		shaders[1].setParameter("xResolution", imageSize->x);

		sf::Sprite spr(outputPointer->getTexture());
		outputPointer->draw(spr, &shaders[1]);
	}

	const void Multiply(const std::vector<GUI::Pin*>& inputPins, const std::vector<GUI::Pin*>& outputPins)
	{
		sf::RenderTexture* outputPointer = ((sf::RenderTexture*) outputPins[0]->data);
		sf::RenderTexture* a = ((sf::RenderTexture*) inputPins[0]->getData());
		sf::RenderTexture* b = ((sf::RenderTexture*) inputPins[1]->getData());

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
		sf::RenderTexture* a = ((sf::RenderTexture*) inputPins[0]->getData());
		int* times = ((int*) inputPins[1]->getData());
		int fixed = *times < 0 ? *times * -1 + 2 : *times;

		sf::Vector2u size = a->getSize();

		if (fixed % 2 == 1)
			outputPointer->create(size.y, size.x);
		else
			outputPointer->create(size.x, size.y);

		std::cout << fixed << std::endl;

		shaders[3].setParameter("tx", a->getTexture());
		shaders[3].setParameter("times", (float) fixed);

		sf::Sprite spr(outputPointer->getTexture());
		outputPointer->draw(spr, &shaders[3]);
	}

	const void Repeat(const std::vector<GUI::Pin*>& inputPins, const std::vector<GUI::Pin*>& outputPins)
	{
		sf::RenderTexture* outputPointer = ((sf::RenderTexture*) outputPins[0]->data);
		sf::RenderTexture* a = ((sf::RenderTexture*) inputPins[0]->getData());
		sf::Vector2i* newSize = ((sf::Vector2i*) inputPins[1]->getData());

		sf::Vector2u aSize = a->getSize();
		sf::Vector2f originalSize(aSize.x, aSize.y);
		outputPointer->create(newSize->x, newSize->y);

		shaders[4].setParameter("tex", a->getTexture());
		shaders[4].setParameter("originalSize", originalSize);
		shaders[4].setParameter("newSize", sf::Vector2f(newSize->x, newSize->y));

		sf::Sprite spr(outputPointer->getTexture());
		outputPointer->draw(spr, &shaders[4]);
	}

	const void Invert(const std::vector<GUI::Pin*>& inputPins, const std::vector<GUI::Pin*>& outputPins)
	{
		sf::RenderTexture* outputPointer = ((sf::RenderTexture*) outputPins[0]->data);
		sf::RenderTexture* a = ((sf::RenderTexture*) inputPins[0]->getData());

		sf::Vector2u size = a->getSize();
		outputPointer->create(size.x, size.y);

		shaders[5].setParameter("tex", a->getTexture());

		sf::Sprite spr(outputPointer->getTexture());
		outputPointer->draw(spr, &shaders[5]);
	}
}