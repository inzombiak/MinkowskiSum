#include "Minkowski.h"
#include "Math.h"

#include <map>

struct Edge
{
	sf::Vector2f a;
	sf::Vector2f b;
};

//Returns vertices of the sum
std::vector<sf::Vector2f> Minkowski::MinkoswkiSum(const std::vector<sf::Vector2f>& verticesA, bool aIsConcave, const std::vector<sf::Vector2f>& verticesB, bool bIsConcave)
{
	std::vector<sf::Vector2f> result;

	return result;
}

bool IsBetweenCCW(const Edge& e, const Edge& e1, const Edge& e2)
{
	if (e.a == e1.a && e.b == e1.b)
		return true;

	sf::Vector2f eVec = e.b - e.a, e1Vec = e1.b - e1.a, e2Vec = e2.b - e2.a;
	eVec.y *= -1;
	e1Vec.y *= -1;
	e2Vec.y *= -1;
	float crossE1E, crossE1E2, crossE2E, crossE2E1;
	crossE1E = sfmath::Cross(e1Vec, eVec);
	crossE2E = sfmath::Cross(e2Vec, eVec);

	crossE1E2 = sfmath::Cross(e1Vec, e2Vec);
	crossE2E1 = sfmath::Cross(e2Vec, e1Vec);

	
	if ((crossE1E == 0 && sfmath::Dot(e1Vec, eVec) > 0) ||
		(crossE2E == 0 && sfmath::Dot(e2Vec, eVec) > 0))
		return true;

	if (crossE1E2 < 0)
	{
		if (crossE1E >= 0)
			return true;
		else
			return false;
	}
	else if (crossE1E2 > 0)
	{
		if (crossE1E < 0)
			return false;
		else if ((crossE1E * crossE1E2) >= 0 && (crossE2E * crossE2E1) >= 0)
			return true;
	}


	return false;
}

int MinIndex(const std::vector<sf::Vector2f>& vertices)
{
	int min = 0;
	for (unsigned int i = 1; i < vertices.size(); ++i)
	{
		//TODO: USE Length^2
		if (sfmath::Length(vertices[min]) > sfmath::Length(vertices[i]))
			min = i;
	}

	return min;
}

typedef std::pair<sf::Vector2f, sf::Vector2f> VisitedEdge;

//Returns all edges(i, i+1) of the reduced convolution
std::vector<sf::Vector2f> Minkowski::ReducedConvolution(const std::vector<sf::Vector2f>& verticesA, bool aIsConcave, const std::vector<sf::Vector2f>& verticesB, bool bIsConcave)
{ 
	int MAX_LOOP = 100;
	std::vector<sf::Vector2f> result;

	int AVertexCount = verticesA.size();
	int BVertexCount = verticesB.size();
	int i0 = MinIndex(verticesA);
	int j0 = MinIndex(verticesB);
	int i = i0, j = j0;

	bool includeP, includeQ;
	sf::Vector2f s = verticesA[i] + verticesB[j], t;

	VisitedEdge st;
	std::vector<VisitedEdge> visited;

	Edge e, e1, e2;
	do
	{
		MAX_LOOP--;

		e.a = verticesA[i];
		e.b = verticesA[(i + 1) % AVertexCount];
		if (j == 0)
			e1.a = verticesB[BVertexCount - 1];
		else
			e1.a = verticesB[(j - 1) % BVertexCount];
		e1.b = verticesB[j];
		e2.a = verticesB[j];
		e2.b = verticesB[(j + 1) % BVertexCount];
		includeP = IsBetweenCCW(e, e1, e2);

		t = e.b + e1.b;

		st.first = s;
		st.second = t;
		if (includeP && (std::find(visited.begin(), visited.end(), st)) == visited.end())
		{
			result.push_back(s);
			result.push_back(t);
			visited.push_back(st);
			s = t;
			i++;
			i %= AVertexCount;
			continue;
		}

		e1 = e;
		e = e2;
		e2 = e1;
		if (i == 0)
			e1.a = verticesA[AVertexCount - 1];
		else
			e1.a = verticesA[(i - 1) % AVertexCount];
		e1.b = verticesA[i];
		includeQ = IsBetweenCCW(e, e1, e2);
		if (includeQ)
		{
			t = e1.b + e.b;
			result.push_back(s);
			result.push_back(t);
			st.second = t;
			visited.push_back(st);

			s = t;
			j++;
			j %= BVertexCount;
		}

	} while (MAX_LOOP && (i != i0 || j != j0));

	/*edge e, e1, e2;
	float crosse1e, crosse1e2, crosse2e, crosse2e1;
	for (unsigned int i = 0; i < avertexcount; ++i)
	{
		e.a = verticesa[i];
		e.b = verticesa[(i + 1) % avertexcount];
		for (unsigned j = 0; j < bvertexcount; ++j)
		{
			if (bisconcave)
			{
				//TODO: store reflex vertices in shape maybe?
				if (sfmath::isreflex(verticesb[j], verticesb[(j - 1) % bvertexcount], verticesb[(j + 1) % bvertexcount]))
					continue;
			}
			e2.a = verticesb[j];
			e2.b = verticesb[(j + 1) % bvertexcount];

			e1.a = verticesb[(j - 1) % bvertexcount];
			e1.b = verticesb[j];

			crosse1e = sfmath::cross(e1.b - e1.a, e.b - e.a);
			crosse2e = sfmath::cross(e2.b - e2.a, e.b - e.a);

			crosse1e2 = sfmath::cross(e1.b - e1.a, e2.b - e2.a);
			crosse2e1 = sfmath::cross(e2.b - e2.a, e1.b - e1.a);

			if ((crosse1e * crosse1e2) >= 0 && (crosse2e * crosse2e1) >= 0)
			{
				result.push_back(e.a + e1.a);
				result.push_back(e.b + e1.a);
			}
		}
	}

	for (unsigned int i = 0; i < verticesb.size(); ++i)
	{

		e.a = verticesb[i];
		e.b = verticesb[(i + 1) % bvertexcount];

		for (unsigned j = 0; j < avertexcount; ++j)
		{
			if (aisconcave)
			{
				//TODO: store reflex vertices in shape maybe?
				if (sfmath::isreflex(verticesa[j], verticesa[(j - 1) % avertexcount], verticesa[(j + 1) % avertexcount]))
					continue;
			}

			e2.a = verticesa[j];
			e2.b = verticesa[(j + 1) % avertexcount];

			e1.a = verticesa[(j - 1) % avertexcount];
			e1.b = verticesa[j];

			crosse1e = sfmath::cross(e1.b - e1.a, e.b - e.a);
			crosse2e = sfmath::cross(e2.b - e2.a, e.b - e.a);

			crosse1e2 = sfmath::cross(e1.b - e1.a, e2.b - e2.a);
			crosse2e1 = sfmath::cross(e2.b - e2.a, e1.b - e1.a);

			if ((crosse1e * crosse1e2) >= 0 && (crosse2e * crosse2e1) >= 0)
			{
				result.push_back(e.a + e1.a + sf::vector2f(100.f,0.f));
				result.push_back(e.b + e1.a + sf::vector2f(100.f,0.f));
			}
		}
	}*/
	printf("Convolution Size: %i MAX_LOOP: %i \n", result.size(), MAX_LOOP);
	return result;
}

std::vector<sf::Vector2f> Minkowski::ExtractOrientableLoops(const std::vector<sf::Vector2f>& edges)
{
	std::vector<sf::Vector2f> result;
	int loopStart = 0;
	for (unsigned int i = 0; i < edges.size(); ++i)
	{

	}

	return result;
}