#include "Minkowski.h"
#include "Math.h"

#include <map>

//Returns vertices of the sum
std::vector<sf::Vector2f> Minkowski::MinkoswkiSum(const std::vector<sf::Vector2f>& verticesA, const std::vector<int>& aReflexIndices,
													const std::vector<sf::Vector2f>& verticesB, const std::vector<int>& bReflexIndices)
{
	std::vector<sf::Vector2f> result;
	m_visited.clear();
	int aReflexCount = aReflexIndices.size();
	int bReflexCount = bReflexIndices.size();

	//Perform 1 iteration and if both shapes are convex, we have the M-sum
	result = ReducedConvolution(verticesA, aReflexIndices, verticesB, bReflexIndices);
	if (aReflexCount == 0 || bReflexCount == 0)
		return result;
	else
	{
		
	}
	return result;
}

//Returns all edges(i, i+1) of the reduced convolution
std::vector<sf::Vector2f> Minkowski::ReducedConvolution(const std::vector<sf::Vector2f>& verticesA, const std::vector<int>& aReflexIndices,
														const std::vector<sf::Vector2f>& verticesB, const std::vector<int>& bReflexIndices)
{ 
	//DEBUG
	int MAX_LOOP = 100;
	std::vector<sf::Vector2f> result;

	unsigned int AVertexCount = verticesA.size();
	unsigned int BVertexCount = verticesB.size();
	//The points on both shapes closest to the origin are on the M-sum
	int i0 = MinIndex(verticesA);
	int j0 = MinIndex(verticesB);
	int i = i0, j = j0;
	int aIndex, bIndex;

	bool includeP, includeQ;
	//The edge st is the edge that we add to the convolution
	//The main job of the program is to find valid values for these
	//Since the minima points are guaranteed to be on the M-sun boundry, we start there.
	sf::Vector2f s = verticesA[i] + verticesB[j], t;

	VisitedEdge st;

	Edge e, e1, e2;
	bool iIsReflex = false, jIsReflex = false;

	//I sue this flag to stop program from stalling, it's probably replaceable with soem code restructuring
	bool edgeAdded = true;

	//Itereate
	do
	{
		MAX_LOOP--;

		/*if (!edgeAdded)
		{
			if (iIsReflex)
				i = sfmath::Mod(i + 1, AVertexCount);
			else if (jIsReflex)
				j = sfmath::Mod(j + 1, BVertexCount);
		}*/

		//Check if either i or j are index of a reflex vertex
		iIsReflex = (std::find(aReflexIndices.begin(), aReflexIndices.end(), i) != aReflexIndices.end());
		jIsReflex = (std::find(bReflexIndices.begin(), bReflexIndices.end(), j) != bReflexIndices.end());

		edgeAdded = false;
		//if (!jIsReflex)
		//{
			//First we check a edge (A[i], A[i+1]) against a vertex B[j]
			//Create the edges
		e.a = verticesA[i];
		e.b = verticesA[sfmath::Mod(i + 1, AVertexCount)];
		e1.a = verticesB[sfmath::Mod(j - 1, BVertexCount)];
		e1.b = verticesB[j];
		e2.a = verticesB[j];
		e2.b = verticesB[sfmath::Mod((j + 1), BVertexCount)];
		//Check if the edge is valid for the covolution
		includeP = IsBetweenCCW(e, e1, e2);

		//We assign to t here so we can check if st has been visited or not
		//If it hasn't and its valid, we add it.
		t = e.b + e1.b;
		st.first = s;
		st.second = t;
		if (!jIsReflex)
		{
			if (includeP && (std::find(m_visited.begin(), m_visited.end(), st)) == m_visited.end())
			{
				result.push_back(s);
				result.push_back(t);
				m_visited.push_back(st);
				printf("A VALID: %i %i \n", i, j);
				s = t;
				i = sfmath::Mod(i + 1, AVertexCount);
				edgeAdded = true;
				continue;
			}
		}
		
		//}
		
		//if (!iIsReflex)
		//{
		
		//Now we consider (B[j], B[j+1]) with A[i].
		//Swap the values
		e1 = e;  //e1 = (A[i], A[i+1]) 
		e = e2;  //e = (B[j], B[j+1])
		e2 = e1; //e2 = (A[i], A[i+1]) 
		e1.a = verticesA[sfmath::Mod(i - 1, AVertexCount)];
		e1.b = verticesA[i];

		//Check if valid
		includeQ = IsBetweenCCW(e, e1, e2);
		if (!iIsReflex)
		{
			if (includeQ)
			{
				t = e1.b + e.b;
				result.push_back(s);
				result.push_back(t);
				st.second = t;
				m_visited.push_back(st);
				printf("B VALID: %i %i \n", i, j);
				s = t;
				j = sfmath::Mod((j + 1), BVertexCount);
				edgeAdded = true;
			}
		}
		else
			i++;
	} while (MAX_LOOP && (i != i0 || j != j0)); // Repeat until we're back where we started
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

bool Minkowski::IsBetweenCCW(const Edge& e, const Edge& e1, const Edge& e2)
{
	if (e.a == e1.a && e.b == e1.b)
		return true;

	sf::Vector2f eVec = e.b - e.a, e1Vec = e1.b - e1.a, e2Vec = e2.b - e2.a;
	//I wasted a lot of time at night trying to fix this function, 
	//until I remembered that SFML has an inverted y-axis
	eVec.y *= -1;
	e1Vec.y *= -1;
	e2Vec.y *= -1;
	float crossE1E, crossE1E2, crossE2E, crossE2E1;
	crossE1E = sfmath::Cross(e1Vec, eVec);
	crossE2E = sfmath::Cross(e2Vec, eVec);

	crossE1E2 = sfmath::Cross(e1Vec, e2Vec);
	crossE2E1 = sfmath::Cross(e2Vec, e1Vec);

	//If E1/E2 are in the same dircetion as E, return true
	if ((crossE1E == 0 && sfmath::Dot(e1Vec, eVec) > 0) ||
		(crossE2E == 0 && sfmath::Dot(e2Vec, eVec) > 0))
		return true;
	// First attempt, seems to work but also returns true if it is inbetween in CW direction.Needs more testing
	//return ((crossE1E * crossE1E2) >= 0 && (crossE2E * crossE2E1) >= 0);
	//TODO: I didnt keep a record of which of these 3 is correct, so I'm keepign all 3 just in case
	/*if (crossE1E2 < 0)
	{
		if (crossE1E < 0)
			return false;
		else
			return true;
	}
	else if (crossE1E2 > 0)
	{
		if (crossE1E < 0)
			return false;
		else if ((crossE1E * crossE1E2) >= 0 && (crossE2E * crossE2E1) >= 0)
			return true;
	}*/

	//If e2 is cw from e1
	if (crossE1E2 < 0)
	{
		//if e is ccw from e1, its inbetween, otherwise, check if it is inbetween
		if (crossE1E >= 0)
			return true;
		else
			return ((crossE1E * crossE1E2) >= 0 && (crossE2E * crossE2E1) >= 0);
	}
	else if (crossE1E2 > 0) // If e2 is ccw from e1
	{
		//If e is cw from e1, it isn't valid, otherwise, check if it is inbetween
		if (crossE1E < 0)
			return false;
		else
			return ((crossE1E * crossE1E2) >= 0 && (crossE2E * crossE2E1) >= 0);
	}

	return false;
}

int Minkowski::MinIndex(const std::vector<sf::Vector2f>& vertices)
{
	int min = 0;
	for (unsigned int i = 1; i < vertices.size(); ++i)
	{
		if (sfmath::Length2(vertices[min]) > sfmath::Length2(vertices[i]))
			min = i;
	}

	return min;
}
