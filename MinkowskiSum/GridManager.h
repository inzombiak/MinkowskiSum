#ifndef GRID_MANAGER_H
#define GRID_MANAGER_H

#include <vector>
#include <queue>
#include "ConcaveShape.h"
<<<<<<< HEAD

<<<<<<< HEAD
<<<<<<< HEAD
=======
//TODO: Merge convex and concave into a single hierarchy

>>>>>>> parent of 0dbf6c8... Added Convolution, still need to do loops and sum
=======
//TODO: Merge convex and concave into a single hierarchy
=======
>>>>>>> parent of 5923412... Added convex shape picking, now to add minkowski sum

>>>>>>> parent of 0dbf6c8... Added Convolution, still need to do loops and sum
class GridManager
{

public:
	GridManager();

	void GenerateGrid(int windowWidth, int windowHeight, unsigned int rows, unsigned int columns);

	void Draw(sf::RenderWindow& rw);

	void AddVertex();
<<<<<<< HEAD
<<<<<<< HEAD
	void MoveDot(const sf::Vector2i& mousePos);
	void CreateShape();
=======
	void SelectShape(const sf::Vector2f& mousePos);

	void DropShape();

	void MoveDot(const sf::Vector2f& mousePos);
	void MoveShape(const sf::Vector2f& mousePos);
	void CreateShape();

	void ShowPointer(bool val);
	void DeleteCurrentShape();


>>>>>>> parent of 0dbf6c8... Added Convolution, still need to do loops and sum
=======
	void MoveDot(const sf::Vector2i& mousePos);
	void CreateShape();
>>>>>>> parent of 5923412... Added convex shape picking, now to add minkowski sum
private:
	struct Node
	{
		sf::Vector2f value;
		int next = -1;
		int prev = -1;

		bool isReflex = false;
		int index;
	};

	bool IsReflex(const Node& n);
	bool IsEar(const Node& n, const std::vector<int>& vertices);
	void CreateConcaveShape();

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
	sf::Vector2i SnapToGrid(const sf::Vector2i& pos);
=======
=======
>>>>>>> parent of 0dbf6c8... Added Convolution, still need to do loops and sum
	sf::Vector2i SnapToGrid(const sf::Vector2f& pos);
>>>>>>> parent of 0dbf6c8... Added Convolution, still need to do loops and sum
=======
	sf::Vector2i SnapToGrid(const sf::Vector2i& pos);
>>>>>>> parent of 5923412... Added convex shape picking, now to add minkowski sum

	int m_windowHeight;
	int m_windowWidth;
	int m_rowCount;
	int m_columnCount;
	float m_xSpacing;
	float m_ySpacing;
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD

=======
=======
>>>>>>> parent of 0dbf6c8... Added Convolution, still need to do loops and sum
	std::pair<int, ShapeType> m_currentShape = std::make_pair(-1, ShapeType::Concave);
	bool m_drawPointer;
>>>>>>> parent of 0dbf6c8... Added Convolution, still need to do loops and sum
=======

>>>>>>> parent of 5923412... Added convex shape picking, now to add minkowski sum
	sf::CircleShape m_pointer;

	std::vector<sf::RectangleShape> m_gridLines;
	std::vector<sf::Vector2f> m_shapeVertexPos;
	std::vector<sf::CircleShape> m_shapeVertiecies;
<<<<<<< HEAD

<<<<<<< HEAD
	std::vector<sf::ConvexShape> m_convexShapes;
	std::vector<ConcaveShape> m_concaveShapes;
=======
>>>>>>> parent of 0dbf6c8... Added Convolution, still need to do loops and sum

<<<<<<< HEAD
=======
	std::vector<ConvexShape> m_convexShapes;
	std::vector<ConcaveShape> m_concaveShapes;

	std::queue<int> m_unusedConvexShapes;
	std::queue<int> m_unusedConcaveShapes;

<<<<<<< HEAD
>>>>>>> parent of 0dbf6c8... Added Convolution, still need to do loops and sum
=======
>>>>>>> parent of 0dbf6c8... Added Convolution, still need to do loops and sum
=======
	std::vector<sf::ConvexShape> m_convexShapes;
	std::vector<ConcaveShape> m_concaveShapes;

>>>>>>> parent of 5923412... Added convex shape picking, now to add minkowski sum
	bool m_verticesAreClockwise = false;
	float m_shoelaceSum = 0;

	static const int LINE_WIDTH = 2;
	static const int CIRCLE_RADIUS = 4;
};

#endif
