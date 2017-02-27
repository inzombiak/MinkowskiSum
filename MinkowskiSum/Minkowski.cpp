#include "Minkowski.h"
#include "Math.h"

#include <map>

//Returns vertices of the sum
std::vector<sf::Vector2f> Minkowski::MinkoswkiSum(const std::vector<sf::Vector2f>& verticesA, const std::vector<int>& aReflexIndices,
													const std::vector<sf::Vector2f>& verticesB, const std::vector<int>& bReflexIndices)
{
	m_aVertices = verticesA;
	for (int i = 0; i < m_aVertices.size(); ++i)
	{
		m_aDirections.push_back(m_aVertices[(i + 1) % m_aVertices.size()] - m_aVertices[i]);
	}

	m_bVertices = verticesB;
	for (int i = 0; i < m_bVertices.size(); ++i)
	{
		m_bDirections.push_back(m_bVertices[(i + 1) % m_bVertices.size()] - m_bVertices[i]);
	}

	std::vector<sf::Vector2f> result;
	m_visited.clear();
	int aReflexCount = aReflexIndices.size();
	int bReflexCount = bReflexIndices.size();

	//Perform 1 iteration and if both shapes are convex, we have the M-sum
	//result = ReducedConvolution(verticesA, aReflexIndices, verticesB, bReflexIndices);
	result = ReducedConvolution2(aReflexIndices, bReflexIndices);
	if (aReflexCount == 0 || bReflexCount == 0)
		return result;
	else
	{
		
	}
	return result;
}

//Returns all edges(i, i+1) of the reduced convolution
std::vector<sf::Vector2f> Minkowski::ReducedConvolution(const std::vector<int>& aReflexIndices, const std::vector<int>& bReflexIndices)
{ 
	//DEBUG
	int MAX_LOOP = 100;
	std::vector<sf::Vector2f> result;

	unsigned int AVertexCount = m_aVertices.size();
	unsigned int BVertexCount = m_bVertices.size();
	//The points on both shapes closest to the origin are on the M-sum
	int i0 = MinIndex(m_aVertices);
	int j0 = MinIndex(m_bVertices);
	int i = i0, j = j0;
	int aIndex, bIndex;

	bool includeP, includeQ;
	//The edge st is the edge that we add to the convolution
	//The main job of the program is to find valid values for these
	//Since the minima points are guaranteed to be on the M-sun boundry, we start there.
	sf::Vector2f s = m_aVertices[i] + m_bVertices[j], t;

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
		e.a = m_aVertices[i];
		e.b = m_aVertices[sfmath::Mod(i + 1, AVertexCount)];
		e1.a = m_bVertices[sfmath::Mod(j - 1, BVertexCount)];
		e1.b = m_bVertices[j];
		e2.a = m_bVertices[j];
		e2.b = m_bVertices[sfmath::Mod((j + 1), BVertexCount)];
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
		e1.a = m_aVertices[sfmath::Mod(i - 1, AVertexCount)];
		e1.b = m_aVertices[i];

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

bool Minkowski::IsBetweenCCW(sf::Vector2f eVec, sf::Vector2f e1Vec,sf::Vector2f e2Vec)

{
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
		if (crossE1E > 0)
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
	if ((crossE1E == 0 && sfmath::Dot(e1Vec, eVec) == 1) ||
		(crossE2E == 0 && sfmath::Dot(e2Vec, eVec) == 1))
	{
		return true;
	}
		
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
#include <queue>
#include <set>
typedef std::pair<int, int>                           State;

std::vector<sf::Vector2f> Minkowski::ReducedConvolution2(const std::vector<int>& aReflexIndices, const std::vector<int>& bReflexIndices)
{
	std::vector<sf::Vector2f> result;
	int n1 = static_cast<int>(m_aVertices.size());
	int n2 = static_cast<int>(m_bVertices.size());

	// Contains states that were already visited
	std::set<State> visited_states;
	int minA = MinIndex(m_aVertices);
	int minB = MinIndex(m_bVertices);
	// Init the queue with vertices from the first column
	std::queue<State> state_queue;
	int i = n1 - 1;// minA;
	int j = minB;
	do
	{
		state_queue.push(State(i, 0));

		i = --i;// (i + 1) % n1;
	} while (i >= 0); //(i != minA);
	

	/*int i = minA;
	int j = minB;
	do
	{
		state_queue.push(State(i, 0));

		i = (i + 1) % n1;
	} while (i != minA);*/

	while (state_queue.size() > 0)
	{
		State curr_state = state_queue.front();
		state_queue.pop();

		int i1 = curr_state.first;
		int i2 = curr_state.second;

		// If this state was already visited, skip it
		if (visited_states.count(curr_state) > 0)
		{
			continue;
		}
		visited_states.insert(curr_state);

		int next_i1 = (i1 + 1) % n1;
		int next_i2 = (i2 + 1) % n2;
		int prev_i1 = (n1 + i1 - 1) % n1;
		int prev_i2 = (n2 + i2 - 1) % n2;

		// Try two transitions: From (i,j) to (i+1,j) and to (i,j+1). Add
		// the respective segments, if they are in the reduced convolution.
		for (int step_in_pgn1 = 0; step_in_pgn1 <= 1; step_in_pgn1++)
		{
			int new_i1, new_i2;
			if (step_in_pgn1)
			{
				new_i1 = next_i1;
				new_i2 = i2;
			}
			else
			{
				new_i1 = i1;
				new_i2 = next_i2;
			}

			// If the segment's direction lies counterclockwise in between
			// the other polygon's vertex' ingoing and outgoing directions,
			// the segment belongs to the full convolution.
			bool belongs_to_convolution;
			if (step_in_pgn1)
				belongs_to_convolution = IsBetweenCCW(m_aDirections[i1], m_bDirections[sfmath::Mod((i2 - 1), n2)], m_bDirections[i2]);
			else
				belongs_to_convolution = IsBetweenCCW(m_bDirections[i2], m_aDirections[sfmath::Mod((i1 - 1), n1)], m_aDirections[i1]);

			if (belongs_to_convolution)
			{
				state_queue.push(State(new_i1, new_i2));

				// Only edges added to convex vertices can be on the M-sum's boundary.
				// This filter only leaves the *reduced* convolution.
				bool concave;
				if (step_in_pgn1)
				{
					concave = (std::find(bReflexIndices.begin(), bReflexIndices.end(), i2) != bReflexIndices.end());
				}
				else
				{
					concave = (std::find(aReflexIndices.begin(), aReflexIndices.end(), i1) != aReflexIndices.end());
				}

				if (!concave) //7660
				{
					printf("VALID POINT FOUND- A: s%i e%i, B s:%i e:%i \n", i1, i2, new_i1, new_i2);
					sf::Vector2f s = m_aVertices[i1] + m_bVertices[i2];// get_point(i1, i2, p1_vertices, p2_vertices);
					sf::Vector2f t = m_aVertices[new_i1] + m_bVertices[new_i2];
					result.push_back(s);
					result.push_back(t);
				}
				else
					printf("POINT IS CONCAVE : %s: %i \n", (step_in_pgn1) ? "B" : "A", (step_in_pgn1) ? i2 : i1);
			}
		}
	}

	return result;
 }