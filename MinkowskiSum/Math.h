#ifndef MATH_H
#define MATH_H

#include "SFML\Graphics.hpp"

namespace sfmath
{
	float Dot(const sf::Vector2f& v1, const sf::Vector2f& v2)
	{
		return v1.x * v2.x + v1.y * v2.y;;
	}

	float Length(const sf::Vector2f& v)
	{
		return sqrt(v.x * v.x + v.y  * v.y); 
	}

	float Cross(const sf::Vector2f& v1, const sf::Vector2f& v2)
	{
		return (v1.x * v2.y) - (v1.y * v2.x);
	}

	sf::Vector3f Cross3D(const sf::Vector3f& v1, const sf::Vector3f& v2)
	{
		sf::Vector3f result;

		result.x = v1.y * v2.z - v1.z * v2.y;
		result.y = v1.z * v2.x - v1.x * v2.z;
		result.z = v1.x * v2.y - v1.y * v2.x;

		return result;
	}

	bool SameSideOfLine(const sf::Vector2f& a, const sf::Vector2f& b, const sf::Vector2f& p1, const sf::Vector2f& p2)
	{
		float cp1 = Cross(b - a, p1 - a);
		float cp2 = Cross(b - a, p2 - a);

		return ((cp1 < 0) != (cp2 < 0));

	}

	bool PointInTriangle(const sf::Vector2f& p0, const sf::Vector2f& p1, const sf::Vector2f& p2, const sf::Vector2f& p)
	{
		float s = p0.y*p2.x - p0.x*p2.y + (p2.y - p0.y)*p.x + (p0.x - p2.x)*p.y;
		float t = p0.x*p1.y - p0.y*p1.x + (p0.y - p1.y)*p.x + (p1.x - p0.x)*p.y;

		if ((s < 0) != (t < 0))
			return false;

		float A = -p1.y * p2.x + p0.y * (p2.x - p1.x) + p0.x * (p1.y - p2.y) + p1.x * p2.y;
		if (A < 0.0)
		{
			s = -s;
			t = -t;
			A = -A;
		}
		return s > 0 && t > 0 && (s + t) <= A;
	}
}

#endif