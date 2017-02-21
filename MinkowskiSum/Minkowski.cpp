#include "Minkowski.h"
#include "Math.h"

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

//Returns all edges(i, i+1) of the reduced convolution
std::vector<sf::Vector2f> Minkowski::ReducedConvolution(const std::vector<sf::Vector2f>& verticesA, bool aIsConcave, const std::vector<sf::Vector2f>& verticesB, bool bIsConcave)
{ 

	std::vector<sf::Vector2f> result;

	int AVertexCount = verticesA.size();
	int BVertexCountB = verticesB.size();

	Edge e, e1, e2;
	float crossE1E, crossE1E2, crossE2E, crossE2E1;
	for (unsigned int i = 0; i < AVertexCount; ++i)
	{
		e.a = verticesA[i];
		e.b = verticesA[(i + 1) % AVertexCount];
		if (aIsConcave)
		{
			//TOD: Store reflex vertices in shape maybe?
			if (sfmath::IsReflex(verticesA[i], verticesA[(i - 1) % AVertexCount], verticesA[(i + 1) % AVertexCount]))
				continue;
		}
		for (unsigned j = 0; j < BVertexCountB; ++j)
		{
			e1.a = verticesB[j];
			e1.b = verticesB[(j + 1) % BVertexCountB];

			e2.a = verticesB[(j - 1) % BVertexCountB];
			e2.b = verticesB[j];

			crossE1E = sfmath::Cross(e1.b - e1.a, e.b - e.a);
			crossE2E = sfmath::Cross(e2.b - e2.a, e.b - e.a);

			crossE1E2 = sfmath::Cross(e1.b - e1.a, e2.b - e2.a);
			crossE2E1 = sfmath::Cross(e2.b - e2.a, e1.b - e1.a);

			if ((crossE1E * crossE1E2) >= 0 && (crossE2E * crossE2E1) >= 0)
			{
				result.push_back(e.a + e1.a);
				result.push_back(e.b + e1.a);
			}
		}
	}

	for (unsigned int i = 0; i < verticesB.size(); ++i)
	{

		e.a = verticesB[i];
		e.b = verticesB[(i + 1) % BVertexCountB];
		if (bIsConcave)
		{
			//TOD: Store reflex vertices in shape maybe?
			if (sfmath::IsReflex(verticesB[i], verticesB[(i - 1) % BVertexCountB], verticesB[(i + 1) % BVertexCountB]))
				continue;
		}
		for (unsigned j = 0; j < AVertexCount; ++j)
		{
			e1.a = verticesA[j];
			e1.b = verticesA[(j + 1) % AVertexCount];

			e2.a = verticesA[(j - 1) % AVertexCount];
			e2.b = verticesA[j];

			crossE1E = sfmath::Cross(e1.b - e1.a, e.b - e.a);
			crossE2E = sfmath::Cross(e2.b - e2.a, e.b - e.a);

			crossE1E2 = sfmath::Cross(e1.b - e1.a, e2.b - e2.a);
			crossE2E1 = sfmath::Cross(e2.b - e2.a, e1.b - e1.a);

			if ((crossE1E * crossE1E2) >= 0 && (crossE2E * crossE2E1) >= 0)
			{
				result.push_back(e.a + e1.a);
				result.push_back(e.b + e1.a);
			}
		}
	}

	return result;
}