#ifndef MINKOWSKI_H
#define MINKOWSKI_H

#include <SFML\Graphics.hpp>
#include <vector>
#include <set>

class Minkowski
{
public:
	//Returns vertices of the sum. If either one of the shapes are not concave, pass in an empty vector
	std::vector<sf::Vector2f> MinkoswkiSum( const std::vector<sf::Vector2f>& verticesA, const std::vector<int>& aReflexIndices, 
											const std::vector<sf::Vector2f>& verticesB, const std::vector<int>& bReflexIndices);

private:
	struct Loop
	{
		std::vector<sf::Vector2f> edges;
	};
	//Returns all edges(i, i+1) of the reduced convolution
	std::vector<sf::Vector2f> ReducedConvolution(const std::vector<int>& aReflexIndices, const std::vector<int>& bReflexIndices);
	std::vector<sf::Vector2f> ReducedConvolution_CGAL(const std::vector<int>& aReflexIndices, const std::vector<int>& bReflexIndices);
	//Removes duplicate edges. Again not an optimized function
	std::vector<sf::Vector2f> RemoveDuplicateEdges(const std::vector<sf::Vector2f>& edges);
	//Splits edges at intersection points. At this point, I want to move on to other projects, so I'm just going to brute force it
	std::vector<sf::Vector2f> SplitIntersectingEdges(const std::vector<sf::Vector2f>& edges);
	//Used for recursively splitting edges returns ALL segements of the original edge
	std::vector<sf::Vector2f> SplitEdgeAtIntersections(const sf::Vector2f& a, const sf::Vector2f& b, const std::vector<sf::Vector2f>& edges);

	//Returns the loops of the reduced convolution
	std::vector<Loop> ExtractOrientableLoops(const std::vector<sf::Vector2f>& edges);

	//Places edge with largest CW turn from e in out variables.
	bool BestDirection(const sf::Vector2f& start, const sf::Vector2f& end, const std::vector<sf::Vector2f>& edges, sf::Vector2f& outStart, sf::Vector2f& outEnd, int& outID);

	//Traces loop starting from e
	Loop RecordLoop(int startIndex, const std::vector<sf::Vector2f>& edges);

	//Used during convolution construction
	typedef std::pair<int, int> VisitedEdge;

	std::set<VisitedEdge> m_visited;

	std::vector<sf::Vector2f> m_aVertices;
	std::vector<sf::Vector2f> m_bVertices;
	std::vector<sf::Vector2f> m_aDirections;
	std::vector<sf::Vector2f> m_bDirections;

	//Returns true if e is between e1 and e2 in the CCW direction
	bool IsBetweenCCW(sf::Vector2f eVec, sf::Vector2f e1Vec, sf::Vector2f e2Vec);

	//Returns the index of the point closest to the origin
	int MinIndex(const std::vector<sf::Vector2f>& vertices);

};
	

#endif
