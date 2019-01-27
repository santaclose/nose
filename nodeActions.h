namespace NF
{
	sf::Image* Add (const sf::Image& a, const sf::Image& b, bool clamp)
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
	}
}