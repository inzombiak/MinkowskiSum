#ifndef GRID_MANAGER_H
#define GRID_MANAGER_H

#include <vector>
#include <queue>

#include "ConcaveShape.h"
#include "ConvexShape.h"

//TODO: Since i was using int,ShapeType for shape selecting then changed to pointers, 
//there is inconsistency. Need to change currentshape to pointer also.
class GridManager
{

public:
	GridManager();

	void GenerateGrid(int windowWidth, int windowHeight, unsigned int rows, unsigned int columns);

	void Draw(sf::RenderWindow& rw);

	void AddVertex();
	void SelectShape(const sf::Vector2f& mousePos);
	void MarkShape(const sf::Vector2f& mousePos);

	void DropShape();

	void MoveDot(const sf::Vector2f& mousePos);
	void MoveShape(const sf::Vector2f& mousePos);
	void CreateShape();
	void CreateMinkoswkiSum();
	void CreateMinkowskiDifference();
	void ShowPointer(bool val);
	void DeleteCurrentShape();


private:
	//TODO: Should asssign to a pointer not to m_currentShape(which should be a pointer)
	bool GetShapeContainingPoint(const sf::Vector2f& point);

	struct Node
	{
		sf::Vector2f value;
		int next = -1;
		int prev = -1;

		bool isReflex = false;
		int index;
	};

	bool IsEar(const Node& n, const std::vector<Node>& nodes);
	void CreateConcaveShape();


	sf::Vector2i SnapToGrid(const sf::Vector2f& pos);

	int m_windowHeight;
	int m_windowWidth;
	int m_rowCount;
	int m_columnCount;
	float m_xSpacing;
	float m_ySpacing;

	
	std::pair<int, ShapeType> m_currentShape = std::make_pair(-1, ShapeType::Concave);
	std::pair<int, ShapeType> m_shapeA = std::make_pair(-1, ShapeType::Concave);
	std::pair<int, ShapeType> m_shapeB = std::make_pair(-1, ShapeType::Concave);

	bool m_drawPointer;
	sf::CircleShape m_pointer;

	std::vector<sf::RectangleShape> m_gridLines;
	std::vector<sf::Vector2f> m_shapeVertexPos;
	std::vector<sf::CircleShape> m_shapeVertices;

	std::vector<sf::Vector2f> m_convolutionEdges;

	std::vector<ConvexShape> m_convexShapes;
	std::vector<ConcaveShape> m_concaveShapes;

	std::queue<int> m_unusedConvexShapes;
	std::queue<int> m_unusedConcaveShapes;

	float m_shoelaceSum = 0;

	static const int LINE_WIDTH = 2;
	static const int CIRCLE_RADIUS = 4;

	static const sf::Color MARKED_A;
	static const sf::Color MARKED_B;
	sf::Vector2f ORIGIN_OFFSET;
	sf::Font m_labelFont;
};

#endif
