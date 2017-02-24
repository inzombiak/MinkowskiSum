#ifndef MINKOWSKI_H
#define MINKOWSKI_H

#include <SFML\Graphics.hpp>
#include <vector>

class Minkowski
{
public:
	//Returns vertices of the sum. If either one of the shapes are not concave, pass in an empty vector
	std::vector<sf::Vector2f> MinkoswkiSum( const std::vector<sf::Vector2f>& verticesA, const std::vector<int>& aReflexIndices, 
											const std::vector<sf::Vector2f>& verticesB, const std::vector<int>& bReflexIndices);

private:

	//Returns all edges(i, i+1) of the reduced convolution
	std::vector<sf::Vector2f> ReducedConvolution(const std::vector<sf::Vector2f>& verticesA, const std::vector<int>& aReflexIndices,
												 const std::vector<sf::Vector2f>& verticesB, const std::vector<int>& bReflexIndices);

	//Returns the loops of the reduced convolution
	std::vector<sf::Vector2f> ExtractOrientableLoops(const std::vector<sf::Vector2f>& edges);

	//Helper functions/data structures
	struct Edge
	{
		sf::Vector2f a;
		sf::Vector2f b;
	};
	//Used during convolution construction
	typedef std::pair<sf::Vector2f, sf::Vector2f> VisitedEdge;
	//TODO: In a real world application this needs to be a hash map or something similair for faster searches.
	std::vector<VisitedEdge> m_visited;

	//Returns true if e is between e1 and e2 in the ccw direction
	bool IsBetweenCCW(const Edge& e, const Edge& e1, const Edge& e2);

	//Returns the index of the point closest to the origin
	int MinIndex(const std::vector<sf::Vector2f>& vertices);

};
	

#endif
