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

	//Returns all edges(i, i+1) of the reduced convolution
	std::vector<sf::Vector2f> ReducedConvolution(const std::vector<int>& aReflexIndices, const std::vector<int>& bReflexIndices);
	std::vector<sf::Vector2f> ReducedConvolution_CGAL(const std::vector<int>& aReflexIndices, const std::vector<int>& bReflexIndices);
	//Returns the loops of the reduced convolution
	std::vector<sf::Vector2f> ExtractOrientableLoops(const std::vector<sf::Vector2f>& edges);

	//Used during convolution construction
	typedef std::pair<int, int> VisitedEdge;
	//TODO: In a real world application this needs to be a hash map or something similair for faster searches.
	std::set<VisitedEdge> m_visited;
	std::vector<sf::Vector2f> m_aVertices;
	std::vector<sf::Vector2f> m_bVertices;
	std::vector<sf::Vector2f> m_aDirections;
	std::vector<sf::Vector2f> m_bDirections;

	//Returns true if e is between e1 and e2 in the ccw direction
	bool IsBetweenCCW(sf::Vector2f eVec, sf::Vector2f e1Vec, sf::Vector2f e2Vec);

	//Returns the index of the point closest to the origin
	int MinIndex(const std::vector<sf::Vector2f>& vertices);

};
	

#endif
