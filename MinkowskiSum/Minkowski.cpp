#include "Minkowski.h"
#include "Math.h"

#include <map>

//Returns vertices of the sum
std::vector<sf::Vector2f> Minkowski::MinkoswkiSum(const std::vector<sf::Vector2f>& verticesA, const std::vector<int>& aReflexIndices,
													const std::vector<sf::Vector2f>& verticesB, const std::vector<int>& bReflexIndices)
{
	m_aDirections.clear();
	m_bDirections.clear();
	m_visited.clear();

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
	result = ReducedConvolution_CGAL(aReflexIndices, bReflexIndices);
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
	//int i0 = MinIndex(m_aVertices);
	//int j0 = MinIndex(m_bVertices);
	int i, j;
	int next_i, next_j, prev_i, prev_j;
	int aIndex, bIndex;

	bool includeP, includeQ;
	//The edge st is the edge that we add to the convolution
	//The main job of the program is to find valid values for these
	//Since the minima points are guaranteed to be on the M-sun boundry, we start there.
	sf::Vector2f s, t;

	VisitedEdge currentEdge;
	bool iIsReflex = false, jIsReflex = false;
	//Itereate
	for (unsigned int i0 = 0; i0 < AVertexCount; ++i0)
	{
		i = i0;
		j = 0;
		s = m_aVertices[i] + m_bVertices[j];
		MAX_LOOP = 30;
		do
		{
			MAX_LOOP--;
			currentEdge.first = i;
			currentEdge.second = j;
			printf("i: %i, j: %i \n", i, j);
			if (m_visited.count(currentEdge))
				continue;

			next_i = (i + 1) % AVertexCount;
			next_j = (j + 1) % BVertexCount;
			prev_i = sfmath::Mod(i - 1, AVertexCount);
			prev_j = sfmath::Mod(j - 1, BVertexCount);

			iIsReflex = (std::find(aReflexIndices.begin(), aReflexIndices.end(), i) != aReflexIndices.end());
			jIsReflex = (std::find(bReflexIndices.begin(), bReflexIndices.end(), j) != bReflexIndices.end());

			includeP = IsBetweenCCW(m_aDirections[i], m_bDirections[prev_j], m_bDirections[j]);
			includeQ = IsBetweenCCW(m_bDirections[j], m_aDirections[prev_i], m_aDirections[i]);
			if (includeP && !jIsReflex)
			{
				//m_visited.insert(VisitedEdge(next_i, j));
				t = m_aVertices[next_i] + m_bVertices[j];
				result.push_back(s);
				result.push_back(t);
				s = t;

				i = next_i;
			}
			
			if (includeQ && !iIsReflex)
			{
				//m_visited.insert(VisitedEdge(i, next_j));
				t = m_aVertices[i] + m_bVertices[next_j];
				result.push_back(s);
				result.push_back(t);
				s = t;

				j = next_j;
			}
		} while (MAX_LOOP > 0 && (i != i0 || j != 0)); // Repeat until we're back where we started
	}
	
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

	float crossE1E, crossE1E2, crossE2E, crossE2E1;
	crossE1E = sfmath::Cross(e1Vec, eVec);
	crossE2E = sfmath::Cross(e2Vec, eVec);

	crossE1E2 = sfmath::Cross(e1Vec, e2Vec);
	crossE2E1 = sfmath::Cross(e2Vec, e1Vec);

	//If E1/E2 are in the same dircetion as E, return true
	if ((crossE1E == 0 && sfmath::Dot(e1Vec, eVec) > 0) ||
		(crossE2E == 0 && sfmath::Dot(e2Vec, eVec) > 0))
		return true;

	//E2 is CCW from E1
	if (crossE1E2 < 0)
	{
		//if E is CW from e1, it can't be inbetween, otherwise, check if it is inbetween
		if (crossE1E > 0)
			return false;
		else
			return ((crossE1E * crossE1E2) >= 0 && (crossE2E * crossE2E1) >= 0);
	}
	else if (crossE1E2 > 0) // If E2 is CW from E1
	{
		//If E is CCW from E1, it is valid otherwise, check if it is inbetween
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

/*
	CGAL CODE FOR REFERENCE: https://github.com/CGAL/cgal/blob/master/Minkowski_sum_2/include/CGAL/Minkowski_sum_2/Minkowski_sum_by_reduced_convolution_2.h\
	I haven't used CGAL, but the code provided in the link above did not work for my polygons without one simple change. 
	This issue may be my fault, but I'm going to explain what I found anyway.
	I found that if either polygon was concave, then the algorithm would fail depending on what order the vertices were placed.
	Testing on a simple 4 vertex concave polygon(arrow-shaped), I found it would fail everytime unless the reflex vertex was the first vertex.
	I've attempted to draw a dengenerate case. If we move the first vertex to the relfex angle, the algorithm works as is.
	                   
					   1
	                  * ^
	0 < * * 3		 * B *
	*       ^		*     * 
	*   A   *	   *   3   * 
	v       *     v  ^   v  *
	1 * * > 2    2 *       * 0 

	An interesting thing to note is that when the code reaches A0, B0, it does not find any valid edges, controdicting what is written in the paper.
	The specific issue was that A1 + B3 -> A1 + B0 was never added to the final convolution.

	The fix is simple: move state_queue.push(State(new_i1, new_i2)) outside of the "if".
	My reasoning for this is that for a concave polygon a loop isn't closed, so we won't traverse all the vertices if we only move forwards(or backwards).
	When I did it by hand I found that the algorithm would ONLY consider the B3 when traversing the edges of A and since it is a reflex vertex it would not be valid, 
	and the state "A1 B3" would never be added to the queue. By adding it to the queue, B3 ends up being considered with A0->A1, completing the convolution.

*/
#include <queue>
typedef std::pair<int, int>                           State;

std::vector<sf::Vector2f> Minkowski::ReducedConvolution_CGAL(const std::vector<int>& aReflexIndices, const std::vector<int>& bReflexIndices)
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
	for (int i = n1 - 1; i >= 0; --i)
	{
		state_queue.push(State(i, 0));
	}

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
				belongs_to_convolution = IsBetweenCCW(m_aDirections[i1], m_bDirections[prev_i2], m_bDirections[i2]);
			else
				belongs_to_convolution = IsBetweenCCW(m_bDirections[i2], m_aDirections[prev_i1], m_aDirections[i1]);
			//New location
			state_queue.push(State(new_i1, new_i2));
			if (new_i1 == 1 && new_i2 == 3)
			{
				//This is the point that would be the cause of the issue for my test case
				//It would be considered when traversing A, and since it is reflex it wouldn't be added to the list
				//But it WOULD'NT be considered as a part of the edge 3->0, thus leading to an incomplete convolution
			}
			if (belongs_to_convolution)
			{

				//Old location
				//state_queue.push(State(new_i1, new_i2));
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

				if (!concave)
				{
					sf::Vector2f s = m_aVertices[i1] + m_bVertices[i2];
					sf::Vector2f t = m_aVertices[new_i1] + m_bVertices[new_i2];
					result.push_back(s);
					result.push_back(t);
				}
			}
		}
	}
	printf("Convolution Size: %i \n", result.size());
	return result;
 }