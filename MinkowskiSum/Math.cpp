#include "Math.h"

float sfmath::Dot(const sf::Vector2f& v1, const sf::Vector2f& v2)
{
	return v1.x * v2.x + v1.y * v2.y;;
}

float sfmath::Length(const sf::Vector2f& v)
{
	return sqrt(v.x * v.x + v.y  * v.y);
}

float sfmath::Cross(const sf::Vector2f& v1, const sf::Vector2f& v2)
{
	return (v1.x * v2.y) - (v1.y * v2.x);
}

sf::Vector3f sfmath::Cross3D(const sf::Vector3f& v1, const sf::Vector3f& v2)
{
	sf::Vector3f result;

	result.x = v1.y * v2.z - v1.z * v2.y;
	result.y = v1.z * v2.x - v1.x * v2.z;
	result.z = v1.x * v2.y - v1.y * v2.x;

	return result;
}

bool sfmath::SameSideOfLine(const sf::Vector2f& a, const sf::Vector2f& b, const sf::Vector2f& p1, const sf::Vector2f& p2)
{
	float cp1 = Cross(b - a, p1 - a);
	float cp2 = Cross(b - a, p2 - a);

	return ((cp1 < 0) != (cp2 < 0));

}

bool sfmath::PointInTriangle(const sf::Vector2f& p0, const sf::Vector2f& p1, const sf::Vector2f& p2, const sf::Vector2f& p)
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

bool sfmath::RayLineIntersect(const Ray& ray, const sf::Vector2f a, const sf::Vector2f b)
{
	//Read this http://stackoverflow.com/questions/563198/how-do-you-detect-where-two-line-segments-intersect/565282#565282
	//The direction of the line, normalized
	sf::Vector2f lineDir = b - a;

	//Cross product of the directions
	float dirCross = Cross(ray.direction, lineDir);
	if (dirCross == 0)
		return false;

	sf::Vector2f originDiff = a - ray.origin;
	float t = (Cross(originDiff, lineDir)) / dirCross;
	float u = (Cross(originDiff, ray.direction)) / dirCross;

	//Since t is for ray it just needs to be >= 0 and u needs to between 0 and 1
	return (t >= 0 & u >= 0 & u <= 1);
}

bool sfmath::IsReflex(const sf::Vector2f& p, const sf::Vector2f& prev, const sf::Vector2f& next, bool counterClockwise)
{
	sf::Vector2f vec1, vec2;
	float angle;
	vec1 = prev - p;
	vec2 = next - p;

	angle = atan2(sfmath::Cross(vec1, vec2), sfmath::Dot(vec1, vec2));
	printf("Angle: %lf \n", angle);
	if (angle > 0)
		return false;

	return true;
}

std::vector<sf::Vector2f> sfmath::InvertShape(const std::vector<sf::Vector2f>& vertices, sf::Vector2f origin)
{
	std::vector<sf::Vector2f> result;
	result.reserve(vertices.size());
	for (unsigned int i = 0; i < vertices.size(); ++i)
	{
		result.push_back(2.f*origin - vertices[i]);
	}
	return result;
}