#include "ConcaveShape.h"


ConcaveShape::ConcaveShape(const std::vector<sf::ConvexShape>& shapes, const std::vector<sf::Vector2f>& vertices)
{
	m_shapes = shapes;
	m_vertices = vertices;
	CalcCentroid();
}
ConcaveShape::~ConcaveShape()
{
	m_shapes.clear();
}

void ConcaveShape::SetPosition(const sf::Vector2f& newPos)
{
	sf::Vector2f diff = m_currentPosition - newPos;
	sf::Vector2f oldPos;
	for (unsigned int i = 0; i < m_shapes.size(); ++i)
	{
		oldPos = m_shapes[i].getPosition();
		m_shapes[i].setPosition(oldPos + diff);
	}

	for (unsigned int i = 0; i < m_vertices.size(); ++i)
	{
		m_vertices[i] += diff;
	}

	m_currentPosition = newPos;
}
void ConcaveShape::SetFillColor(const sf::Color& newColor)
{
	for (unsigned int i = 0; i < m_shapes.size(); ++i)
		m_shapes[i].setFillColor(newColor);
}

void ConcaveShape::Draw(sf::RenderWindow& rw)
{
	for (unsigned int i = 0; i < m_shapes.size(); ++i)
		rw.draw(m_shapes[i]);
}

void ConcaveShape::CalcCentroid()
{
	float x = 0, y = 0;
	int pointCount = 0;
	for (unsigned int i = 0; i < m_vertices.size(); ++i)
	{
		x += m_vertices[i].x;
		y += m_vertices[i].y;

		pointCount++;
	}

	m_currentPosition.x = x / pointCount;
	m_currentPosition.y = y / pointCount;
}