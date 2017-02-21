#include "GridManager.h"

#include <iostream>
#include <unordered_map>

#include "Math.h"

GridManager::GridManager()
{
	m_pointer = sf::CircleShape(CIRCLE_RADIUS);
	m_pointer.setFillColor(sf::Color::Red);
}

void GridManager::GenerateGrid(int windowWidth, int windowHeight, unsigned int rows, unsigned int columns)
{
	m_windowHeight = windowHeight;
	m_windowWidth = windowWidth;
	m_rowCount = rows;
	m_columnCount = columns;

	m_xSpacing = (float)(m_windowWidth - LINE_WIDTH * columns) / ((float)m_columnCount + 1);
	m_ySpacing = (float)(m_windowHeight - LINE_WIDTH * rows) / (float)(m_rowCount + 1);

	for (unsigned int i = 0; i < m_rowCount; ++i)
	{
		sf::RectangleShape newLine(sf::Vector2f(m_windowWidth , LINE_WIDTH));
		newLine.setPosition(0, (m_ySpacing + LINE_WIDTH) * (i + 1));
		newLine.setFillColor(sf::Color(0, 0, 0, 25));

		if (i == m_rowCount / 2)
		{
			newLine.setFillColor(sf::Color::Blue);
			//newLine.setSize(sf::Vector2f(m_windowWidth, 2));
		}
		
		m_gridLines.push_back(newLine);
	}
	

	for (unsigned int j = 0; j < m_columnCount; ++j)
	{
		sf::RectangleShape newLine(sf::Vector2f(m_windowHeight, LINE_WIDTH));
		newLine.setPosition((m_xSpacing + LINE_WIDTH) * (j + 1), 0);
		newLine.setFillColor(sf::Color(0, 0, 0, 25));

		if (j == m_columnCount / 2)
		{
			newLine.setFillColor(sf::Color::Blue);
			//newLine.setSize(sf::Vector2f(m_windowHeight, 2));
		}

		newLine.rotate(90.f);
		m_gridLines.push_back(newLine);
	}



}

void GridManager::Draw(sf::RenderWindow& rw)
{
	for (unsigned int i = 0; i < m_gridLines.size(); ++i)
		rw.draw(m_gridLines[i]);

	rw.draw(m_pointer);

	for (unsigned int i = 0; i < m_shapeVertiecies.size(); ++i)
	{
		rw.draw(m_shapeVertiecies[i]);
	}

	for (unsigned int i = 0; i < m_convexShapes.size(); ++i)
	{
		rw.draw(m_convexShapes[i]);
	}

	for (unsigned int i = 0; i < m_concaveShapes.size(); ++i)
	{
		m_concaveShapes[i].Draw(rw);
	}
}

void GridManager::AddVertex()
{
	m_shapeVertexPos.push_back(m_pointer.getPosition());

	sf::CircleShape vertex(CIRCLE_RADIUS);
	vertex.setFillColor(sf::Color::Green);
	vertex.setPosition(sf::Vector2f(m_pointer.getPosition()));
	m_shapeVertiecies.push_back(vertex);

	//Sholace formula
	int currVertex = m_shapeVertexPos.size() - 1;
	if (currVertex > 0)
		m_shoelaceSum += (m_shapeVertexPos[currVertex].x - m_shapeVertexPos[currVertex - 1].x)*(m_shapeVertexPos[currVertex].y + m_shapeVertexPos[currVertex - 1].y);
}

void GridManager::MoveDot(const sf::Vector2i& mousePos)
{
	sf::Vector2i snappedPos = SnapToGrid(mousePos);
	m_pointer.setPosition(snappedPos.x, snappedPos.y);
}

sf::Vector2i GridManager::SnapToGrid(const sf::Vector2i& pos)
{
	int x = std::round(pos.x / (m_xSpacing + LINE_WIDTH)) * (m_xSpacing + LINE_WIDTH);
	int y = std::round(pos.y / (m_ySpacing + LINE_WIDTH)) * (m_ySpacing + LINE_WIDTH);

	return sf::Vector2i(x - CIRCLE_RADIUS, y - CIRCLE_RADIUS /2);
}

void GridManager::CreateShape()
{
	if (m_shapeVertexPos.size() <= 2)
		return;

	m_shoelaceSum += (m_shapeVertexPos[0].x - m_shapeVertexPos[m_shapeVertexPos.size() - 1].x)*(m_shapeVertexPos[0].y + m_shapeVertexPos[m_shapeVertexPos.size() - 1].y);
	if (m_shoelaceSum > 0)
		m_verticesAreClockwise = false;
	else
		m_verticesAreClockwise = true;

	sf::ConvexShape shape;
	shape.setPointCount(m_shapeVertexPos.size() + 1);

	sf::Vector2f prevVec = m_shapeVertexPos[1] - m_shapeVertexPos[0];
	sf::Vector2f currVec = m_shapeVertexPos[2] - m_shapeVertexPos[1];
	shape.setPoint(0, m_shapeVertexPos[0]);
	shape.setPoint(1, m_shapeVertexPos[1]);
	shape.setPoint(2, m_shapeVertexPos[2]);

	float prevCross = sfmath::Cross(prevVec, currVec);
	float currCross;
	bool isConcave = false;
	int pointCount = m_shapeVertexPos.size();

	//

	for (unsigned int i = 3; i <= pointCount; ++i)
	{
		prevVec = currVec;
		currVec = m_shapeVertexPos[i % pointCount] - m_shapeVertexPos[(i - 1) % pointCount];
		currCross = sfmath::Cross(prevVec, currVec);
		if ((prevCross < 0) != (currCross < 0))
		{
			isConcave = true;
			break;
		}
		else
		{
			shape.setPoint(i, m_shapeVertexPos[i % pointCount]);
		}
		
		prevCross = currCross;
	}
	if (isConcave)
	{
		//TODO change this so we dont start over from scratch for concave
		CreateConcaveShape();
	}
	else if (!isConcave)
	{
		shape.setFillColor(sf::Color(125, 125, 0, 50));
<<<<<<< HEAD
<<<<<<< HEAD
		m_convexShapes.push_back(shape);
	}

	m_shapeVertexPos.clear();
	//m_shapeVertiecies.clear();
=======
		if (!m_unusedConvexShapes.empty())
		{
			int index = m_unusedConvexShapes.front();
			m_convexShapes[index] = ConvexShape(shape);
			m_unusedConvexShapes.pop();
		}
		else
			m_convexShapes.push_back(ConvexShape(shape));
	}

	m_shapeVertexPos.clear();
	m_shapeVertiecies.clear();
>>>>>>> parent of 0dbf6c8... Added Convolution, still need to do loops and sum
=======
		m_convexShapes.push_back(shape);
	}

	m_shapeVertexPos.clear();
	//m_shapeVertiecies.clear();
>>>>>>> parent of 5923412... Added convex shape picking, now to add minkowski sum
	m_shoelaceSum = 0;
	m_verticesAreClockwise = false;
}

<<<<<<< HEAD
=======

<<<<<<< HEAD

void GridManager::ShowPointer(bool val)
{
	m_drawPointer = val;
}
>>>>>>> parent of 0dbf6c8... Added Convolution, still need to do loops and sum

bool GridManager::IsReflex(const Node& n)
{
<<<<<<< HEAD
	sf::Vector2f p	= m_shapeVertexPos[n.index];
	sf::Vector2f p1 = m_shapeVertexPos[n.prev];
	sf::Vector2f p2 = m_shapeVertexPos[n.next];

=======
	if (m_currentShape.first == -1)
		return;


}
void GridManager::SelectShape(const sf::Vector2f& mousePos)
{
	if (m_currentShape.first != -1)
		if (m_currentShape.second == Concave)
			m_concaveShapes[m_currentShape.first].Selected(false, sf::Vector2f(0, 0));
		else
			m_convexShapes[m_currentShape.first].Selected(false, sf::Vector2f(0, 0));


	bool found = false;

	for (unsigned int i = 0; i < m_concaveShapes.size(); ++i)
	{
		if (m_concaveShapes[i].ContainsPoint(mousePos))
		{
			m_currentShape.first = i;
			m_currentShape.second = Concave;
			m_concaveShapes[i].Selected(true, mousePos);
			found = true;
			break;

		}
	}

	if (!found)
	{
		for (unsigned int i = 0; i < m_convexShapes.size(); ++i)
		{
			if (m_convexShapes[i].ContainsPoint(mousePos))
			{
				m_currentShape.first = i;
				m_currentShape.second = Convex;
				m_convexShapes[i].Selected(true, mousePos);
				found = true;
				return;

			}
		}
	}

	if (!found)
		m_currentShape.first = -1;
}

=======
>>>>>>> parent of 5923412... Added convex shape picking, now to add minkowski sum
bool GridManager::IsReflex(const Node& n)
{
	sf::Vector2f p	= m_shapeVertexPos[n.index];
	sf::Vector2f p1 = m_shapeVertexPos[n.prev];
	sf::Vector2f p2 = m_shapeVertexPos[n.next];

>>>>>>> parent of 0dbf6c8... Added Convolution, still need to do loops and sum
	sf::Vector2f vec1, vec2;
	float angle;
	vec1 = p1 - p;
	vec2 = p2 - p;

	angle =  atan2(sfmath::Cross(vec1, vec2), sfmath::Dot(vec1, vec2));
	if ((m_verticesAreClockwise && angle < 0) || (!m_verticesAreClockwise && angle > 0))
		return false;
	
	return true;
}

bool GridManager::IsEar(const Node& n, const std::vector<int>& reflexVertices)
{
	bool isEar = true;

	for (unsigned int j = 0; j < reflexVertices.size(); ++j)
	{
		if (reflexVertices[j] != -1 && j != n.index && j != n.next && j != n.prev)
			if (sfmath::PointInTriangle(m_shapeVertexPos[n.index], m_shapeVertexPos[n.next], m_shapeVertexPos[n.prev], m_shapeVertexPos[reflexVertices[j]]))
			{
				isEar = false;
				break;
			}
				
	}

	return isEar;
}

void GridManager::CreateConcaveShape()
{
	//Used ear clipping method to decompose polygon

	int pointCount = m_shapeVertexPos.size();
	std::vector<sf::ConvexShape> shapes;
	std::vector<Node> nodes;
	nodes.resize(pointCount);
	std::vector<int> reflexVertices;
	reflexVertices.resize(pointCount, -1);
	std::vector<int> convexVertices;
	convexVertices.resize(pointCount, -1);
	std::unordered_map<int,int> ears;


	for (unsigned int i = 0; i < pointCount; ++i)
	{
		nodes[i].value = m_shapeVertexPos[i];
		nodes[i].index = i;
		nodes[i].prev = (i - 1) % pointCount;
		nodes[i].next = (i + 1) % pointCount;
		if ((int)i - 1 < 0)
			nodes[i].prev = pointCount - 1;
		if (i + 1 > pointCount - 1)
			nodes[i].next = 0;

		if (IsReflex(nodes[i]))
		{
			reflexVertices[i] = i;
			nodes[i].isReflex = true;
		}
		else
		{
			convexVertices[i] = i;
		}
	}
	std::vector<int> temp;
	temp.reserve(reflexVertices.size() + convexVertices.size());
	temp.insert(temp.end(), reflexVertices.begin(), reflexVertices.end());
	temp.insert(temp.end(), convexVertices.begin(), convexVertices.end());
	{
		for (unsigned int i = 0; i < pointCount; ++i)
		{
			if (!nodes[i].isReflex)
				if (IsEar(nodes[i], temp))
					ears[i] = i;
		}
	}
	

	sf::ConvexShape cs;
	int steps = 0;
	while (!ears.empty())
	{
		int ear = ears.begin()->second;
		ears.erase(ear);


		Node currentNode = nodes[ear];
		int next = currentNode.next;
		int prev = currentNode.prev;

		if ((next == prev) && (next == ear))
			break;

		cs = sf::ConvexShape(3);
		cs.setPoint(0, currentNode.value);
		cs.setPoint(1, nodes[currentNode.prev].value);
		cs.setPoint(2, nodes[currentNode.next].value);
		cs.setFillColor(sf::Color(0.f, 0.f, 200.f, 25.f));
		cs.setOutlineThickness(0);
		shapes.push_back(cs);

		nodes[prev].next = next;
		nodes[next].prev = prev;

		if (nodes[prev].isReflex)
			if (!(nodes[prev].isReflex = IsReflex(nodes[prev])))
			{
				reflexVertices[prev] = -1;
				convexVertices[prev] = prev;
			}
				

		if (nodes[next].isReflex)
			if (!(nodes[next].isReflex = IsReflex(nodes[next])))
			{
				reflexVertices[next] = -1;
				convexVertices[next] = next;
			}
				


		temp.clear();
		temp.reserve(reflexVertices.size() + convexVertices.size());
		temp.insert(temp.end(), reflexVertices.begin(), reflexVertices.end());
		temp.insert(temp.end(), convexVertices.begin(), convexVertices.end());

		auto it = ears.begin();
		while(it != ears.end())
		{
			if (!IsEar(nodes[it->second], temp))
				it = ears.erase(it);
			else
				it++;
		}

		if (!nodes[prev].isReflex)
			if (ears.find(prev) == ears.end() && IsEar(nodes[prev], temp));
				ears[prev] = prev;

		if (!nodes[next].isReflex)
			if (ears.find(next) == ears.end() && IsEar(nodes[next], reflexVertices));
				ears[next] = next;

		steps++;
	}
	
	ConcaveShape concaveShape(shapes, m_shapeVertexPos);
<<<<<<< HEAD
<<<<<<< HEAD
	m_concaveShapes.push_back(concaveShape);
}
=======
	if (!m_unusedConcaveShapes.empty())
	{
		int index = m_unusedConcaveShapes.front();
		m_concaveShapes[index] = concaveShape;
		m_unusedConcaveShapes.pop();
	}
	else
		m_concaveShapes.push_back(concaveShape);
}
>>>>>>> parent of 0dbf6c8... Added Convolution, still need to do loops and sum
=======
	m_concaveShapes.push_back(concaveShape);
}
>>>>>>> parent of 5923412... Added convex shape picking, now to add minkowski sum
