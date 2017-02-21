#ifndef MATH_H
#define MATH_H

#include "SFML\Graphics.hpp"
#include <vector>

namespace sfmath
{
	struct Ray
	{
		sf::Vector2f origin;
		sf::Vector2f direction;
	};

	float Dot(const sf::Vector2f& v1, const sf::Vector2f& v2);
	float Length(const sf::Vector2f& v);
	float Cross(const sf::Vector2f& v1, const sf::Vector2f& v2);
	bool RayLineIntersect(const Ray& ray, const sf::Vector2f a, const sf::Vector2f b);

	sf::Vector3f Cross3D(const sf::Vector3f& v1, const sf::Vector3f& v2);

	bool SameSideOfLine(const sf::Vector2f& a, const sf::Vector2f& b, const sf::Vector2f& p1, const sf::Vector2f& p2);
	bool PointInTriangle(const sf::Vector2f& p0, const sf::Vector2f& p1, const sf::Vector2f& p2, const sf::Vector2f& p);

	bool IsReflex(const sf::Vector2f& p, const sf::Vector2f& prev, const sf::Vector2f& next, bool counterClockwise = true);
	std::vector<sf::Vector2f> InvertShape(const std::vector<sf::Vector2f>& vertices, sf::Vector2f origin = sf::Vector2f(0.f, 0.f));

}

#endif