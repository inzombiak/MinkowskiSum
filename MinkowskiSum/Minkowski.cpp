#include "Minkowski.h"
#include <map>
#include <stack>
#include <queue>

//Returns vertices of the sum
std::vector<sf::Vector2f> Minkowski::MinkoswkiSum(const std::vector<sf::Vector2f>& verticesA, const std::vector<int>& aReflexIndices,
													const std::vector<sf::Vector2f>& verticesB, const std::vector<int>& bReflexIndices)
{
	m_aDirections.clear();
	m_bDirections.clear();
	m_visited.clear();
	m_convolution.clear();
	m_convolution2.clear();
	m_convolutionVertices.clear();
	m_vertexToIndex.clear();
	m_adjacencyMatrix.clear();
	m_danglingEdgeIndices.clear();

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

	std::vector<Loop> loops;

	m_visited.clear();
	int aReflexCount = aReflexIndices.size();
	int bReflexCount = bReflexIndices.size();

	//Perform convolution construction if both shapes are convex, we have the M-sum
	//ReducedConvolution_CGAL(aReflexIndices, bReflexIndices);
	ReducedConvolution(aReflexIndices, bReflexIndices);
	printf("Convolution Size: %i \n", m_convolution.size());
	//Otherwise, we need to extract the orientable loops

	if (aReflexCount != 0 || bReflexCount != 0)
	{
		SplitIntersectingEdges();
		printf("Convolution Size after splitting: %i \n", m_convolution.size());
		//During this step, I also mark the easy to identify dangling edges
		//These are edges that are not part of the loop
		ConstructAdjacencyMatrix();
		//Now I go over the dangling edges, trying to find more
		//Basically, If a vertex has only one connection, then its dangling.
		//If its neightbor vertex has only 2 connections, then since one of its neighbors is dangling, it is too
		//Repeat this "cutting" process until we hit a point with 3 connections, where at least 1 is not dangling
		//The paper talks about collision and intersection tests, but I couldn't really understand what were checking
		MarkRemainingDanglingEdges();
		printf("Convolution Size after removing duplicates: %i \n", m_convolution.size());
		loops = ExtractOrientableLoops();
	}
	else
		return m_convolution;
	if (loops.empty())
	{
		std::vector<sf::Vector2f> result;

		for (unsigned int i = 0; i < m_convolution.size(); i += 2)
		{
			//if (m_danglingEdgeIndices.count(i) > 0)
			//	continue;

			result.push_back(m_convolution[i]);
			result.push_back(m_convolution[i + 1]);
		}
		return result;
	}
		

	return loops[0].edges;
}

void Minkowski::CalculateCycle(int i0, int j0, const std::vector<int>& aReflexIndices, const std::vector<int>& bReflexIndices)
{
	unsigned int AVertexCount = m_aVertices.size();
	unsigned int BVertexCount = m_bVertices.size();
	int i, j;
	int next_i, next_j, prev_i, prev_j, new_i, new_j;

	bool includeP, includeQ;
	sf::Vector2f s, t;

	VisitedEdge currentEdge;
	VisitedEdge startEdge;
	bool iIsReflex = false, jIsReflex = false;
	bool edgeFoundI, edgeFoundJ;

	new_i = i = i0;
	new_j = j = j0;

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
		s = m_aVertices[i] + m_bVertices[j];
		if (includeP)
		{
			t = m_aVertices[next_i] + m_bVertices[j];
			if (!jIsReflex)
			{
				m_convolution.push_back(s);
				m_convolution.push_back(t);
			}

			new_i = next_i;
		}
		if (includeQ)
		{
			t = m_aVertices[i] + m_bVertices[next_j];
			if (!iIsReflex)
			{
				m_convolution.push_back(s);
				m_convolution.push_back(t);
			}

			new_j = next_j;
		}
		i = new_i;
		j = new_j;
	} while (j != j0 && i != i0);
}

//Returns all edges(i, i+1) of the reduced convolution
void Minkowski::ReducedConvolution(const std::vector<int>& aReflexIndices, const std::vector<int>& bReflexIndices)
{ 
	unsigned int AVertexCount = m_aVertices.size();
	unsigned int BVertexCount = m_bVertices.size();
	//The points on both shapes closest to the origin are on the M-sum
	int i = MinIndex(m_aVertices);
	int j = MinIndex(m_bVertices);
	/*int i, j;
	int next_i, next_j, prev_i, prev_j, new_i, new_j;

	bool includeP, includeQ;
	//The edge st is the edge that we add to the convolution
	//The main job of the program is to find valid values for these
	//Since the minima points are guaranteed to be on the M-sun boundry, we start there.
	sf::Vector2f s, t;

	VisitedEdge currentEdge;
	VisitedEdge startEdge;

	bool iIsReflex = false, jIsReflex = false;
	bool edgeFoundI, edgeFoundJ;
	int itCount = 0;
	*/
	VisitedEdge currentEdge;
	//Itereate
	for (unsigned int i0 = 0; i0 < AVertexCount; ++i0)
	{
		for (unsigned int j0 = 0; j0 < BVertexCount; ++j0)
		{
			currentEdge.first = i;
			currentEdge.second = j;
			j = (j + 1) % BVertexCount;
			if (m_visited.count(currentEdge) > 0)
			{
				continue;
			}
			CalculateCycle(currentEdge.first, currentEdge.second, aReflexIndices, bReflexIndices);
		}

		i = (i + 1) % AVertexCount;
	}		
			//new_i = i = i0;
			//new_j = j = j0;
			
			//do
			//{
			//	currentEdge.first = i;
			//	currentEdge.second = j;
			//	edgeFoundI = false;
			//	edgeFoundJ = false;

			//	if (m_visited.count(currentEdge) > 0)
			//	{
			//		continue;
			//	}

			//	m_visited.insert(currentEdge);

			//	next_i = (i + 1) % AVertexCount;
			//	next_j = (j + 1) % BVertexCount;
			//	prev_i = sfmath::Mod(i - 1, AVertexCount);
			//	prev_j = sfmath::Mod(j - 1, BVertexCount);

			//	iIsReflex = (std::find(aReflexIndices.begin(), aReflexIndices.end(), i) != aReflexIndices.end());
			//	jIsReflex = (std::find(bReflexIndices.begin(), bReflexIndices.end(), j) != bReflexIndices.end());

			//	includeP = IsBetweenCCW(m_aDirections[i], m_bDirections[prev_j], m_bDirections[j]);
			//	includeQ = IsBetweenCCW(m_bDirections[j], m_aDirections[prev_i], m_aDirections[i]);
			//	s = m_aVertices[i] + m_bVertices[j];
			//	if (includeP)
			//	{
			//		t = m_aVertices[next_i] + m_bVertices[j];
			//		if (!jIsReflex)
			//		{
			//			m_convolution.push_back(s);
			//			m_convolution.push_back(t);
			//		}

			//		new_i = next_i;
			//	}
			//	if (includeQ)
			//	{
			//		t = m_aVertices[i] + m_bVertices[next_j];
			//		if (!iIsReflex)
			//		{
			//			m_convolution.push_back(s);
			//			m_convolution.push_back(t);
			//		}

			//		new_j = next_j;
			//	}
			//	//if (!includeP && !includeQ)
			//	//{
			//	//	//Should never trigger but it does.
			//	//	//The paper says:
			//	//	/*
			//	//		For every state ( i,j ) that the algorithm reaches, which corresponds to a vertex v and the next two possible states 
			//	//		( i + 1 ,j ) and ( i,j + 1), which correspond to the vertices w and w0 , respectively, 
			//	//		at least one of the segments −→ vw and −→ vw0 is in the convolution. (Page 29, Baram)
			//	//	*/
			//	//	//I don't know why this happens, but it happens in the CGAL implementation too
			//	//	//i = next_i;
			//	//	//j = next_j;
			//	//}
			//	i = new_i;
			//	j = new_j;
			//	itCount++;
			//} while (j != j0 && i != i0);
	//	}
	//}
}
void Minkowski::MarkRemainingDanglingEdges()
{
	return;
	std::queue<int, std::deque<int>> danglers(std::deque<int>(m_danglingEdgeIndices.begin(), m_danglingEdgeIndices.end()));
	m_danglingEdgeIndices.clear();
	int index, sIndex, eIndex;
	int connectionsIndex = m_adjacencyMatrix[0].size() - 1;
	sf::Vector2f start, end;

	while (!danglers.empty())
	{
		index = danglers.front();
		danglers.pop();

		m_danglingEdgeIndices.insert(index);
		start = m_convolution[index];
		end = m_convolution[index + 1];
		sIndex = m_vertexToIndex[start];
		eIndex = m_vertexToIndex[end];

		//I could speed this up by storing directions, but this will do for now
		//Basically find which vertex has another edge
		/*if (m_adjacencyMatrix[sIndex][connectionsIndex] > 1)
		{

		}
		else if (m_adjacencyMatrix[eIndex][connectionsIndex] > 1)
		{

		}*/
	}

}

void Minkowski::ConstructAdjacencyMatrix()
{
	m_adjacencyMatrix.clear();
	int size = m_convolutionVertices.size();
	m_adjacencyMatrix.resize(size);
	int index;
	int lastIndex = -1;
	int edgeIndex;
	for (unsigned int i = 0; i < size; i ++)
	{
		//I use the last element to keep track of the number of edges a vertex is a part of
		//I'm sure theres a better way of dealing with this, but I'm getting tired of Minkowski and his sums
		m_adjacencyMatrix[i].resize(size + 1, -1);
		m_adjacencyMatrix[i][size] = 0;

		for (unsigned int j = 0; j < m_convolution.size(); j++)
		{
			
			if (sfmath::Length2(m_convolution[j] - m_convolutionVertices[i]) < 5.f)
			{
				
				if (j % 2 == 0)
				{
					//i is starting vertex
					index = m_vertexToIndex[m_convolution[j + 1]];
					edgeIndex = j;
				}
				else
				{
					//i is ending vertex
					index = m_vertexToIndex[m_convolution[j - 1]];
					edgeIndex = j - 1;
				}
				lastIndex = index;
				m_adjacencyMatrix[i][index] = edgeIndex;
				m_adjacencyMatrix[i][size]++;
			}
		}

		//Can't be in a loop if it has a single connection
		if (m_adjacencyMatrix[i][size] < 2 && lastIndex != -1)
		{
			m_danglingEdgeIndices.insert(m_adjacencyMatrix[i][lastIndex]);

			//TODO: This might be a horrible idea
			m_adjacencyMatrix[i][size] = 0;
		}
	}
}

void Minkowski::SplitIntersectingEdges()
{
	//TODO: This is a terrible way to find intersecting edges, I'm just running out of time.
	std::vector<sf::Vector2f> result;
	std::vector<sf::Vector2f> split;
	std::vector<sf::Vector2f> encounteredVertices;
	for (unsigned int i = 0; i < m_convolution.size(); i += 2)
	{
		split = SplitEdgeAtIntersections(m_convolution[i], m_convolution[i + 1], encounteredVertices);
		std::copy(split.begin(), split.end(), std::back_inserter(result));
	}

	m_convolution = result;
}

//TODO: BAD
bool CheckForSimilarPoint(const sf::Vector2f& point, const  std::vector<sf::Vector2f>& encounteredVertices, sf::Vector2f& similairPoint)
{
	int i = 1;
	for (unsigned int i = 0; i < encounteredVertices.size(); ++i)
	{
		if (sfmath::Length2(encounteredVertices[i] - point) < 8.f)
		{
			similairPoint = encounteredVertices[i];
			return true;
		}
		//checkPoint = point;
		//checkPoint.x += std::sin((i + 1) * -M_PI_2);
		//for (unsigned int j = 0; j < 3; ++j)
		//{
		//	checkPoint.y += std::sin((j + 1) * -M_PI_2);
		//	if (i == 1 && j == 1)
		//		continue;
		//	if (encounteredVertices.count(checkPoint) > 0)
		//	{
		//		similairPoint = checkPoint;
		//		return true;
		//	}
		//}
	}

	return false;
}

std::vector<sf::Vector2f> Minkowski::SplitEdgeAtIntersections(sf::Vector2f& a, sf::Vector2f& b, std::vector<sf::Vector2f>& encounteredVertices)
{
	sf::Vector2f intersectionPoint;
	std::vector<sf::Vector2f> result, left, right;
	if (a == b)
		return result;

	if (!CheckForSimilarPoint(a, encounteredVertices, a))
	{
		encounteredVertices.push_back(a);
	}
	if (!CheckForSimilarPoint(b, encounteredVertices, b))
	{
		encounteredVertices.push_back(b);
	}

	bool intersectionFound = false;
	for (unsigned int i = 0; i < m_convolution.size(); i += 2)
	{
		if ((m_convolution[i] != a && m_convolution[i + 1] != b) &&
			(m_convolution[i] != b && m_convolution[i + 1] != a) &&
			sfmath::LineLineIntersect(a, b, m_convolution[i], m_convolution[i + 1], intersectionPoint))
		{
			intersectionPoint.x = std::round(intersectionPoint.x + 0.5f);
			intersectionPoint.y = std::round(intersectionPoint.y + 0.5f);

			if (!CheckForSimilarPoint(intersectionPoint, encounteredVertices, intersectionPoint))
			{
				encounteredVertices.push_back(intersectionPoint);
			}

			if (sfmath::Length(intersectionPoint - a) > 2 && sfmath::Length(intersectionPoint - b) > 2 &&
				intersectionPoint != m_convolution[i] && intersectionPoint != m_convolution[i + 1])
			{
				intersectionFound = true;
				if (a != intersectionPoint)
					left = SplitEdgeAtIntersections(a, intersectionPoint, encounteredVertices);

				if (b != intersectionPoint)
					right = SplitEdgeAtIntersections(intersectionPoint, b, encounteredVertices);

				std::copy(left.begin(), left.end(), std::back_inserter(result));
				std::copy(right.begin(), right.end(), std::back_inserter(result));
			}
		}
	}

	if (!intersectionFound && a != b)
	{

		if (m_vertexToIndex.find(a) == m_vertexToIndex.end())
		{
			m_convolutionVertices.push_back(a);
			m_vertexToIndex[a] = m_convolutionVertices.size() - 1;
		}

		if (m_vertexToIndex.find(b) == m_vertexToIndex.end())
		{
			m_convolutionVertices.push_back(b);
			m_vertexToIndex[b] = m_convolutionVertices.size() - 1;
		}

		result.push_back(a);
		result.push_back(b);
	}

	return result;
}

std::vector<Minkowski::Loop> Minkowski::ExtractOrientableLoops()
{
	//std::
	std::vector<int> edgeIDs;
	edgeIDs.resize(m_convolution.size() / 2, -1);

	std::stack<sf::Vector2f, std::vector<sf::Vector2f>> segStack;
	for (unsigned int i = m_convolution.size(); i-- > 0;)
	{
		segStack.push(m_convolution[i]);
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
		
		if (edgeIDs[currEdge] != -1 || m_danglingEdgeIndices.count(currEdge * 2) > 0)
		{
			currEdge++;
			continue;
		}
			

		edgeIDs[currEdge] = currentID;
		
		hasNext = BestDirection(segStart, segEnd, edgeIDs, currentID, bestStart, bestEnd, bestIndex);
		
		while (hasNext && edgeIDs[bestIndex / 2] == -1)
		{
			printf("Next best: %i \n", bestIndex);
			edgeIDs[bestIndex / 2] = currentID;
			hasNext = BestDirection(bestStart, bestEnd, edgeIDs, currentID, bestStart, bestEnd, bestIndex);
		}

		if (hasNext && edgeIDs[bestIndex / 2] == currentID)
		{
			Loop loop = RecordLoop(currEdge * 2, currentID);
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

bool Minkowski::BestDirection(const sf::Vector2f& start, const sf::Vector2f& end, const std::vector<int> edgeIDs, const int targetID,
							  sf::Vector2f& outStart, sf::Vector2f& outEnd, int& outIndex)
{
	//TODO: Need to change how the dges are stored for faster search

	int currBestIndex = -1;
	float currBestAngle = 0;
	float nextAngle = 0;
	sf::Vector2f edgeDir = end - start;
	sf::Vector2f currDir;
	for (unsigned int i = 0; i < m_convolution.size(); i += 2)
	{
		if (m_convolution[i] != end)
			continue;

		currDir = m_convolution[i + 1] - m_convolution[i];
		nextAngle = sfmath::Angle(edgeDir, currDir) + M_PI;
		if (std::abs(nextAngle) < FLT_EPSILON)
			nextAngle = 0;
		if (nextAngle > currBestAngle && m_danglingEdgeIndices.count(i) < 1)
		{
			if (edgeIDs.empty() || edgeIDs[i / 2] == -1 || edgeIDs[i / 2] == targetID)
			{
				currBestIndex = i;
				currBestAngle = nextAngle;
			}
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

	outStart = m_convolution[currBestIndex];
	outEnd = m_convolution[currBestIndex + 1];
	outIndex = currBestIndex;

	return true;
}

Minkowski::Loop Minkowski::RecordLoop(int startIndex, const int targetID)
{
	int MAX_LOOP = 100;
	Loop result;
	int nextIndex;
	bool hasNext;
	sf::Vector2f nextStart = m_convolution[startIndex];
	sf::Vector2f nextEnd = m_convolution[startIndex + 1];
	result.edges.push_back(nextStart);
	result.edges.push_back(nextEnd);
	hasNext = BestDirection(nextStart, nextEnd, std::vector<int>(), targetID, nextStart, nextEnd, nextIndex);
	while (nextIndex != startIndex && hasNext && MAX_LOOP >= 0)
	{
		result.edges.push_back(nextStart);
		result.edges.push_back(nextEnd);
		hasNext = BestDirection(nextStart, nextEnd, std::vector<int>(), targetID, nextStart, nextEnd, nextIndex);

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

void Minkowski::ReducedConvolution_CGAL(const std::vector<int>& aReflexIndices, const std::vector<int>& bReflexIndices)
{
	int n1 = static_cast<int>(m_aVertices.size());
	int n2 = static_cast<int>(m_bVertices.size());
	bool found = false;
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
		found = false;
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
				found = true;
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

								 
					m_convolution.push_back(s);
					m_convolution.push_back(t);
				}
			}

			if (!found)
			{
				//Should never trigger but it does.
				//The paper says:
				/*
				For every state ( i,j ) that the algorithm reaches, which corresponds to a vertex v and the next two possible states
				( i + 1 ,j ) and ( i,j + 1), which correspond to the vertices w and w 0 , respectively,
				at least one of the segments −→ vw and −→ vw 0 is in the convolution. (Page 29, Baram)
				*/
				//I don't know why this happens

				printf("ERROR \n");
			}

		}
	}
	m_convolutionVertices.shrink_to_fit();
 }



 //void Minkowski::RemoveDuplicateEdges()
 //{
	// std::vector<sf::Vector2f> result;
	// result.push_back(m_convolution[0]);
	// result.push_back(m_convolution[1]);
	// for (unsigned int i = 2; i < m_convolution.size(); i += 2)
	// {
	//	 auto it = std::find(result.begin(), result.end(), m_convolution[i]);
	//	 if (it != result.end() && it != result.end() - 1 && *(it + 1) == m_convolution[i + 1])
	//	 {
	//		 continue;
	//	 }

	//	 result.push_back(m_convolution[i]);
	//	 result.push_back(m_convolution[i + 1]);

	// }

	// m_convolution = result;
 //}




 //Cleans up the adjacency matrix and vertices, merging points that are close to each other
 //I got tired of trying a bunch of simpler methods, so I'm doing this
 //void Minkowski::MergePoints()
 //{
 //	std::vector<sf::Vector2f> cleanedVertices;
 //	//Maybe use a set?
 //	std::map<int, int> mergedPoints;
 //	sf::Vector2f currPoint, twinPoint;
 //	int currIndex, twinIndex;
 //	const float THRESHOLD_DISTANCE = 4.f;
 //	bool pointFound = false;
 //	for (unsigned int i = 0; i < m_convolutionVertices.size(); ++i)
 //	{
 //		pointFound = false;
 //		currPoint = m_convolutionVertices[i];
 //		currIndex = m_vertexToIndex[currPoint];
 //		if (mergedPoints.find(i) != mergedPoints.end())
 //			continue;
 //
 //		for (unsigned int j = i + 1; j < m_convolutionVertices.size(); ++j)
 //		{
 //			if (sfmath::Length2(m_convolutionVertices[j] - m_convolutionVertices[i]) > THRESHOLD_DISTANCE)
 //				continue;
 //
 //			twinPoint = m_convolutionVertices[j];
 //			twinIndex = m_vertexToIndex[twinPoint];
 //			
 //			for (unsigned int k = 0; k < m_adjacencyMatrix[twinIndex].size(); ++k)
 //			{
 //				if (m_adjacencyMatrix[twinIndex][k] == -1)
 //					continue;
 //
 //				
 //
 //			}
 //			mergedPoints[j] = i;
 //		}
 //
 //		cleanedVertices.push_back(currPoint);
 //	}
 //}