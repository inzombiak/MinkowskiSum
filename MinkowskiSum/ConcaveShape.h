#ifndef CONCAVE_SHAPE_H
#define CONCAVE_SHAPE_H

#include "SFML\Graphics.hpp"

class ConcaveShape
{
public:
	ConcaveShape(const std::vector<sf::ConvexShape>& shapes, const std::vector<sf::Vector2f>& vertices);
	~ConcaveShape();

	void SetPosition(const sf::Vector2f& newPos);
	void SetFillColor(const sf::Color& newColor);

	void Draw(sf::RenderWindow& rw);

private:
	void CalcCentroid();

	std::vector<sf::Vector2f> m_vertices;
	std::vector<sf::ConvexShape> m_shapes;
	sf::Vector2f m_currentPosition;
};

#endif
