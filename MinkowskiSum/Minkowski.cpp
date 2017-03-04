#include "Minkowski.h"
#include "Math.h"

#include <map>
#include <stack>

//Returns vertices of the sum
std::vector<sf::Vector2f> Minkowski::MinkoswkiSum(const std::vector<sf::Vector2f>& verticesA, const std::vector<int>& aReflexIndices,
													const std::vector<sf::Vector2f>& verticesB, const std::vector<int>& bReflexIndices)
{
	m_aDirections.clear();
	m_bDirections.clear();
	m_visited.clear();

	m_aVertices = verticesA;
	for (unsigned int i = 0; i < m_aVertices.size(); ++i)
	{
		m_aDirections.push_back(m_aVertices[(i + 1) % m_aVertices.size()] - m_aVertices[i]);
	}

	m_bVertices = verticesB;
	for (unsigned int i = 0; i < m_bVertices.size(); ++i)
	{
		m_bDirections.push_back(m_bVertices[(i + 1) % m_bVertices.size()] - m_bVertices[i]);
	}

	std::vector<sf::Vector2f> convolution;
	std::vector<Loop> loops;

	m_visited.clear();
	int aReflexCount = aReflexIndices.size();
	int bReflexCount = bReflexIndices.size();

	//Perform convolution construction if both shapes are convex, we have the M-sum
	convolution = ReducedConvolution_CGAL(aReflexIndices, bReflexIndices);
	printf("Convolution Size: %i \n", convolution.size());
	//Otherwise, we need to extract the orientable loops
	if (aReflexCount != 0 || bReflexCount != 0)
	{
		//convolution = RemoveDuplicateEdges(convolution);
		//printf("Convolution Size: %i \n", convolution.size());
		convolution = SplitIntersectingEdges(convolution);
		printf("Convolution Size: %i \n", convolution.size());
		return convolution;
		//loops = ExtractOrientableLoops(convolution);
	}
	else
		return convolution;
	if (loops.empty())
		return convolution;

	return loops[0].edges;
}

//Returns all edges(i, i+1) of the reduced convolution
std::vector<sf::Vector2f> Minkowski::ReducedConvolution(const std::vector<int>& aReflexIndices, const std::vector<int>& bReflexIndices)
{ 
	std::vector<sf::Vector2f> result;

	unsigned int AVertexCount = m_aVertices.size();
	unsigned int BVertexCount = m_bVertices.size();
	//The points on both shapes closest to the origin are on the M-sum
	//int i0 = MinIndex(m_aVertices);
	//int j0 = MinIndex(m_bVertices);
	int i, j;
	int next_i, next_j, prev_i, prev_j;

	bool includeP, includeQ;
	//The edge st is the edge that we add to the convolution
	//The main job of the program is to find valid values for these
	//Since the minima points are guaranteed to be on the M-sun boundry, we start there.
	sf::Vector2f s, t;

	VisitedEdge currentEdge;
	VisitedEdge startEdge;
	bool iIsReflex = false, jIsReflex = false;
	bool edgeFoundI, edgeFoundJ;
	//Itereate
	for (unsigned int i0 = 0; i0 < AVertexCount; ++i0)
	{
		for (unsigned int j0 = 0; j0 < BVertexCount; ++j0)
		{
			i = i0;
			j = j0;
			s = m_aVertices[i] + m_bVertices[j];
			do
			{
				currentEdge.first = i;
				currentEdge.second = j;
				edgeFoundI = false;
				edgeFoundJ = false;

				if (m_visited.count(currentEdge) > 0)
				{
					break;
				}

				m_visited.insert(currentEdge);

				next_i = (i + 1) % AVertexCount;
				next_j = (j + 1) % BVertexCount;
				prev_i = sfmath::Mod(i - 1, AVertexCount);
				prev_j = sfmath::Mod(j - 1, BVertexCount);

				iIsReflex = (std::find(aReflexIndices.begin(), aReflexIndices.end(), i) != aReflexIndices.end());
				jIsReflex = (std::find(bReflexIndices.begin(), bReflexIndices.end(), j) != bReflexIndices.end());

				includeP = IsBetweenCCW(m_aDirections[i], m_bDirections[prev_j], m_bDirections[j]);
				includeQ = IsBetweenCCW(m_bDirections[j], m_aDirections[prev_i], m_aDirections[i]);

				if (includeP)
				{
					i = next_i;
					t = m_aVertices[next_i] + m_bVertices[j];
					if (!jIsReflex)
					{
						result.push_back(s);
						result.push_back(t);
					}
					s = t;
				}
				if (includeQ)
				{
					j = next_j;
					t = m_aVertices[i] + m_bVertices[next_j];
					if (!iIsReflex)
					{
						result.push_back(s);
						result.push_back(t);
					}
					s = t;
				}

			} while (j != j0 && i != i0);// Repeat until we're back where we started

		}
	}
	return result;
}

std::vector<sf::Vector2f> Minkowski::SplitIntersectingEdges(const std::vector<sf::Vector2f>& edges)
{
	//TODO: This is a terrible way to find intersecting edges, I'm just running out of time.
	std::vector<sf::Vector2f> result;
	std::vector<sf::Vector2f> split;
	for (unsigned int i = 0; i < edges.size(); i += 2)
	{
		split = SplitEdgeAtIntersections(edges[i], edges[i + 1], edges);
		std::copy(split.begin(), split.end(), std::back_inserter(result));
	}

	return result;
}

std::vector<sf::Vector2f> Minkowski::SplitEdgeAtIntersections(const sf::Vector2f& a, const sf::Vector2f& b, const std::vector<sf::Vector2f>& edges)
{
	sf::Vector2f intersectionPoint;
	std::vector<sf::Vector2f> result, left, right;
	bool intersectionFound = false;
	for (unsigned int i = 0; i < edges.size(); i += 2)
	{
		if (edges[i] != a && edges[i + 1] != b && sfmath::LineLineIntersect(a, b, edges[i], edges[i + 1], intersectionPoint))
		{
			if (intersectionPoint != a && intersectionPoint != b &&
				intersectionPoint != edges[i] && intersectionPoint != edges[i + 1])
			{
				intersectionFound = true;
				left = SplitEdgeAtIntersections(a, intersectionPoint, edges);
				right = SplitEdgeAtIntersections(intersectionPoint, b, edges);
				std::copy(left.begin(), left.end(), std::back_inserter(result));
				std::copy(right.begin(), right.end(), std::back_inserter(result));
			}
		}
	}

	if (!intersectionFound)
	{
		result.push_back(a);
		result.push_back(b);
	}

	return result;
}

std::vector<sf::Vector2f> Minkowski::RemoveDuplicateEdges(const std::vector<sf::Vector2f>& edges)
{
	std::vector<sf::Vector2f> result;
	result.push_back(edges[0]);
	result.push_back(edges[1]);
	for (unsigned int i = 2; i < edges.size(); i += 2)
	{
		auto it = std::find(result.begin(), result.end(), edges[i]);
		if (it != result.end() && it != result.end() - 1 && *(it + 1) == edges[i + 1])
		{
			continue;
		}

		result.push_back(edges[i]);
		result.push_back(edges[i + 1]);

	}
	return result;
}

std::vector<Minkowski::Loop> Minkowski::ExtractOrientableLoops(const std::vector<sf::Vector2f>& edges)
{
	//std::
	std::vector<int> edgeIDs;
	edgeIDs.resize(edges.size() / 2, -1);

	std::stack<sf::Vector2f, std::vector<sf::Vector2f>> segStack;
	for (unsigned int i = edges.size(); i-- > 0; )
	{
		segStack.push(edges[i]);
	}
	std::vector<Loop> result;

	int loopStart = 0;
	int currEdge = 0;
	int currentID = 0;
	int bestIndex;
	bool hasNext;
	sf::Vector2f segStart;
	sf::Vector2f segEnd;
	sf::Vector2f bestStart;
	sf::Vector2f bestEnd;
	while (!segStack.empty())
	{
		segStart= segStack.top();
		segStack.pop();
		segEnd = segStack.top();
		segStack.pop();
		
		if (edgeIDs[currEdge] != -1)
		{
			currEdge++;
			continue;
		}
			

		edgeIDs[currEdge] = currentID;
		
		hasNext = BestDirection(segStart, segEnd, edges, bestStart, bestEnd, bestIndex);
		
		while (hasNext && edgeIDs[bestIndex / 2] == -1)
		{
			edgeIDs[bestIndex / 2] = currentID;
			hasNext = BestDirection(bestStart, bestEnd, edges, bestStart, bestEnd, bestIndex);
		}

		if (hasNext && edgeIDs[bestIndex / 2] == currentID)
		{
			Loop loop = RecordLoop(currEdge * 2, edges);
			if (loop.edges.size() > 4)
			{
				result.push_back(loop);
			}

		}
		currEdge++;
		currentID++;
	}

	return result;
}

bool Minkowski::BestDirection(const sf::Vector2f& start, const sf::Vector2f& end, const std::vector<sf::Vector2f>& edges, 
							  sf::Vector2f& outStart, sf::Vector2f& outEnd, int& outIndex)
{
	//TODO: Need to change how the dges are stored for faster search

	int currBestIndex = -1;
	float currBestAngle = 0;
	float nextAngle = 0;
	sf::Vector2f edgeDir = end - start;
	sf::Vector2f currDir;
	for (unsigned int i = 0; i < edges.size(); i += 2)
	{
		if (edges[i] != end)
			continue;

		currDir = edges[i + 1] - edges[i];
		nextAngle = sfmath::Angle(-edgeDir, -currDir) + M_PI;
		if (std::abs(nextAngle) < FLT_EPSILON)
			nextAngle = 0;
		if (nextAngle > currBestAngle && nextAngle != 0 && std::abs(nextAngle - M_PI) > 0.001)
		{
			currBestIndex = i;
			currBestAngle = nextAngle;
		}

		if (nextAngle < 0)
		{
			printf("WTF \n");
		}
	}

	if (currBestIndex == -1)
	{
		printf("WTF \n");
		return false;
	}

	outStart = edges[currBestIndex];
	outEnd = edges[currBestIndex + 1];
	outIndex = currBestIndex;

	return true;
}

Minkowski::Loop Minkowski::RecordLoop(int startIndex, const std::vector<sf::Vector2f>& edges)
{
	int MAX_LOOP = 100;
	Loop result;
	int nextIndex;
	bool hasNext;
	sf::Vector2f nextStart = edges[startIndex];
	sf::Vector2f nextEnd = edges[startIndex + 1];
	result.edges.push_back(nextStart);
	result.edges.push_back(nextEnd);
	hasNext = BestDirection(nextStart, nextEnd, edges, nextStart, nextEnd, nextIndex);
	while (nextIndex != startIndex && hasNext && MAX_LOOP >= 0)
	{
		result.edges.push_back(nextStart);
		result.edges.push_back(nextEnd);
		hasNext = BestDirection(nextStart, nextEnd, edges, nextStart, nextEnd, nextIndex);
		MAX_LOOP--;
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
			return true;
		else
			return !((crossE1E * crossE1E2) >= 0 && (crossE2E * crossE2E1) >= 0);
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

//CGAL CODE USED FOR REFERENCE
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

				if (!concave)
				{
					sf::Vector2f s = m_aVertices[i1] + m_bVertices[i2];
					sf::Vector2f t = m_aVertices[new_i1] + m_bVertices[new_i2];

					if (s == t)
					{
						continue;
					}

					result.push_back(s);
					result.push_back(t);
				}
			}
		}
	}
	return result;
 }