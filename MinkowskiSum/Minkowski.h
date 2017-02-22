#ifndef MINKOWSKI_H
#define MINKOWSKI_H

#include <SFML\Graphics.hpp>
#include <vector>

namespace Minkowski
{

	//Returns vertices of the sum
	std::vector<sf::Vector2f> MinkoswkiSum(const std::vector<sf::Vector2f>& verticesA, bool aIsConcave, const std::vector<sf::Vector2f>& verticesB, bool bIsConcave);

	//Returns all edges(i, i+1) of the reduced convolution
	std::vector<sf::Vector2f> ReducedConvolution(const std::vector<sf::Vector2f>& verticesA, bool aIsConcave, const std::vector<sf::Vector2f>& verticesB, bool bIsConcave);
	
	//Returns the loops of the reduced convolution
	std::vector<sf::Vector2f> ExtractOrientableLoops(const std::vector<sf::Vector2f>& edges);

}

#endif
