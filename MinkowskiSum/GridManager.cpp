#include "GridManager.h"

#include <iostream>
#include <unordered_map>

#include "Math.h"
#include "Minkowski.h"

GridManager::GridManager()
{
	m_pointer = sf::CircleShape(CIRCLE_RADIUS);
	m_pointer.setFillColor(sf::Color::Red);

	m_labelFont.loadFromFile("arial.ttf");
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
			ORIGIN_OFFSET.y = (m_ySpacing + LINE_WIDTH) * (i + 1);
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
			ORIGIN_OFFSET.x = (m_xSpacing + LINE_WIDTH) * (j + 1);
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

	if (m_drawPointer)
		rw.draw(m_pointer);

	for (unsigned int i = 0; i < m_shapeVertices.size(); ++i)
	{
		rw.draw(m_shapeVertices[i]);
	}

	for (unsigned int i = 0; i < m_convexShapes.size(); ++i)
	{
		m_convexShapes[i].Draw(rw);
	}

	for (unsigned int i = 0; i < m_concaveShapes.size(); ++i)
	{
		m_concaveShapes[i].Draw(rw);
	}

	int ceCount = m_convolutionEdges.size();
	if (ceCount != 0)
	{
		sf::VertexArray va(sf::Lines, ceCount);
		for (unsigned int i = 0; i < ceCount; ++i)
		{
			va[i].position = m_convolutionEdges[i] - ORIGIN_OFFSET;
			va[i].color = sf::Color::Black;

		}

		rw.draw(va);
	}
	
}

void GridManager::AddVertex()
{
	m_shapeVertexPos.push_back(m_pointer.getPosition() + sf::Vector2f(CIRCLE_RADIUS, CIRCLE_RADIUS));

	sf::CircleShape vertex(CIRCLE_RADIUS);
	vertex.setFillColor(sf::Color::Green);
	vertex.setPosition(sf::Vector2f(m_pointer.getPosition()));
	m_shapeVertices.push_back(vertex);

	//Shoelace formula
	int currVertex = m_shapeVertexPos.size() - 1;
	if (currVertex > 0)
		m_shoelaceSum += (m_shapeVertexPos[currVertex].x - m_shapeVertexPos[currVertex - 1].x)*(m_shapeVertexPos[currVertex].y + m_shapeVertexPos[currVertex - 1].y);
}

void GridManager::MoveDot(const sf::Vector2f& mousePos)
{
	sf::Vector2i snappedPos = SnapToGrid(mousePos);
	m_pointer.setPosition(snappedPos.x, snappedPos.y);
}

void GridManager::MoveShape(const sf::Vector2f& mousePos)
{
	if (m_currentShape.first == -1)
		return;
	if (m_currentShape.second == Concave)
		m_concaveShapes[m_currentShape.first].SetPosition(mousePos);
	else
		m_convexShapes[m_currentShape.first].SetPosition(mousePos);
}

void GridManager::DropShape()
{
	if (m_currentShape.first == -1)
		return;
	if (m_currentShape.second == Concave)
		m_concaveShapes[m_currentShape.first].Selected(false, sf::Vector2f(0, 0));
	else
		m_convexShapes[m_currentShape.first].Selected(false, sf::Vector2f(0, 0));
	m_currentShape.first = -1;
}

sf::Vector2i GridManager::SnapToGrid(const sf::Vector2f& pos)
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
	{
		std::reverse(std::begin(m_shapeVertexPos), std::end(m_shapeVertexPos));
	}

	sf::ConvexShape shape;
	shape.setPointCount(m_shapeVertexPos.size() + 1);

	
	bool isConcave = false;
	int pointCount = m_shapeVertexPos.size();

	//

	/*
	sf::Vector2f prevVec = m_shapeVertexPos[1] - m_shapeVertexPos[0];
	sf::Vector2f currVec = m_shapeVertexPos[2] - m_shapeVertexPos[1];
	shape.setPoint(0, m_shapeVertexPos[0]);
	shape.setPoint(1, m_shapeVertexPos[1]);
	shape.setPoint(2, m_shapeVertexPos[2]);

	float prevCross = sfmath::Cross(prevVec, currVec);
	float currCross;
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
	}*/

	for (unsigned int i = 0; i < pointCount; ++i)
	{
		if (sfmath::IsReflex(m_shapeVertexPos[i], m_shapeVertexPos[(i - 1) % pointCount], m_shapeVertexPos[(i + 1) % pointCount]))
		{
			isConcave = true;
			break;
		}
		else
		{
			shape.setPoint(i, m_shapeVertexPos[i]);
		}

	}

	if (isConcave)
	{
		//TODO change this so we dont start over from scratch for concave
		CreateConcaveShape();
	}
	else
	{
		shape.setPoint(pointCount, m_shapeVertexPos[0]);
		shape.setFillColor(sf::Color(125, 125, 0, 50));
		ConvexShape cs(shape);
		cs.SetFont(m_labelFont);
		if (!m_unusedConvexShapes.empty())
		{
			int index = m_unusedConvexShapes.front();
			m_convexShapes[index] = cs;
			m_unusedConvexShapes.pop();
		}
		else
			m_convexShapes.push_back(cs);
	}

	m_shapeVertexPos.clear();
	m_shapeVertices.clear();

	m_shoelaceSum = 0;
}

void GridManager::ShowPointer(bool val)
{
	m_drawPointer = val;
}

void GridManager::DeleteCurrentShape()
{
	if (m_currentShape.first == -1)
		return;

	if (m_currentShape.second == Concave)
	{
		m_concaveShapes[m_currentShape.first].SetInUse(false);
		m_unusedConcaveShapes.push(m_currentShape.first);
	}
	else if (m_currentShape.second == Convex)
	{
		m_convexShapes[m_currentShape.first].SetInUse(false);
		m_unusedConvexShapes.push(m_currentShape.first);
	}

}

//TODO: Clean up
void GridManager::SelectShape(const sf::Vector2f& mousePos)
{
	if (m_currentShape.first != -1)
		if (m_currentShape.second == Concave)
			m_concaveShapes[m_currentShape.first].Selected(false, sf::Vector2f(0, 0));
		else
			m_convexShapes[m_currentShape.first].Selected(false, sf::Vector2f(0, 0));


	if (!GetShapeContainingPoint(mousePos))
		return;
	if (m_currentShape.second == Concave)
	{
		m_concaveShapes[m_currentShape.first].Selected(true, mousePos);
	}
	else
	{
		m_convexShapes[m_currentShape.first].Selected(true, mousePos);
	}
}

void GridManager::MarkShape(const sf::Vector2f& mousePos)
{
	if (!GetShapeContainingPoint(mousePos))
		return;
	IShape* shape = 0;

	if (m_currentShape.second == Concave)
		shape = &m_concaveShapes[m_currentShape.first];
	else
		shape = &m_convexShapes[m_currentShape.first];

	//Deselect shape
	
	IShape* shapeA = 0;
	if (m_shapeA.first != -1)
	{
		if (m_shapeA.second == Concave)
			shapeA = &m_concaveShapes[m_shapeA.first];
		else
			shapeA = &m_convexShapes[m_shapeA.first];
	}
	IShape* shapeB = 0;
	if (m_shapeB.first != -1)
	{
		if (m_shapeB.second == Concave)
			shapeB = &m_concaveShapes[m_shapeB.first];
		else
			shapeB = &m_convexShapes[m_shapeB.first];
	}

	if (shape != 0)
	{
		if (shapeA != NULL && m_currentShape == m_shapeA)
		{
			shape->Marked(false);
			shape->Selected(false, mousePos);
			shape->SetText("");
			if (shapeB != NULL)
			{
				m_shapeA = m_shapeB;

				shapeB->SetFillColor(MARKED_A);
				shapeB->SetText("A");

				m_shapeB = std::make_pair(-1, ShapeType::Concave);
			}
			else
				m_shapeA = std::make_pair(-1, ShapeType::Concave);

		}
		else if (shapeB != NULL &&  m_currentShape == m_shapeB)
		{
			shape->Marked(false);
			shape->Selected(false, mousePos);
			shape->SetText("");

			m_shapeB = std::make_pair(-1, ShapeType::Concave);
		}
		else
		{
			if (shapeA == 0)
			{
				m_shapeA = m_currentShape;
				shape->SetFillColor(MARKED_A);
				shape->SetText("A");
				shape->Marked(true);
			}
			else if (shapeB == 0)
			{
				m_shapeB = m_currentShape;
				shape->SetFillColor(MARKED_B);
				shape->SetText("B");
				shape->Marked(true);
			}
			else
			{
				shapeA->Marked(false);
				shapeA->Selected(false, mousePos);
				shapeA->SetText("");

				m_shapeA = m_shapeB;

				shapeB->Marked(true);
				shapeB->SetFillColor(MARKED_A);
				shapeB->SetText("A");

				m_shapeB = m_currentShape;
				shape->Marked(true);
				shape->SetFillColor(MARKED_B);
				shape->SetText("B");

			}
		}
	}

	m_currentShape.first = -1;
}

//See http://masc.cs.gmu.edu/wiki/uploads/ReducedConvolution/iros11-mksum2d.pdf
//They need to change how wordy publications have to be, makes it seem waaay more complex than it is
void GridManager::CreateMinkoswkiSum()
{
	if (m_shapeA.first == -1 || m_shapeB.first == -1)
		return;

	IShape* shapeA = 0;
	if (m_shapeA.second == Concave)
		shapeA = &m_concaveShapes[m_shapeA.first];
	else
		shapeA = &m_convexShapes[m_shapeA.first];
	IShape* shapeB = 0;
	if (m_shapeB.second == Concave)
		shapeB = &m_concaveShapes[m_shapeB.first];
	else
		shapeB = &m_convexShapes[m_shapeB.first];

	m_convolutionEdges = Minkowski::ReducedConvolution(	shapeA->GetVerticies(), shapeA->GetType() == Concave,
														shapeB->GetVerticies(), shapeB->GetType() == Concave);
	//Reduced convolution
	//std::vector<sf::Vector2f> convolutionEdges;

	//Orientable loop extraction

	//Filter boundries from loops and return

	
}
void GridManager::CreateMinkowskiDifference()
{
	if (m_shapeA.first == -1 || m_shapeB.first == -1)
		return;

	IShape* shapeA = 0;
	if (m_shapeA.second == Concave)
		shapeA = &m_concaveShapes[m_shapeA.first];
	else
		shapeA = &m_convexShapes[m_shapeA.first];
	IShape* shapeB = 0;
	if (m_shapeB.second == Concave)
		shapeB = &m_concaveShapes[m_shapeB.first];
	else
		shapeB = &m_convexShapes[m_shapeB.first];

	m_convolutionEdges = Minkowski::ReducedConvolution(shapeA->GetVerticies(), shapeA->GetType() == Concave,
													   sfmath::InvertShape(shapeB->GetVerticies(), ORIGIN_OFFSET), shapeB->GetType() == Concave);
}

bool GridManager::IsEar(const Node& n, const std::vector<Node>& nodes)
{
	bool isEar = true;

	for (unsigned int j = 0; j < nodes.size(); ++j)
	{
		if (nodes[j].index != n.index &&  nodes[j].index != n.next && nodes[j].index != n.prev)
			if (sfmath::PointInTriangle(m_shapeVertexPos[n.index], m_shapeVertexPos[n.next], m_shapeVertexPos[n.prev], m_shapeVertexPos[nodes[j].index]))
			{
				isEar = false;
				break;
			}
	}

	return isEar;
}

//See https://www.geometrictools.com/Documentation/TriangulationByEarClipping.pdf for ear clipping
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


		if (sfmath::IsReflex(nodes[i].value, nodes[nodes[i].prev].value, nodes[nodes[i].next].value))
		{
			reflexVertices[i] = i;
			nodes[i].isReflex = true;
		}
		else
		{
			convexVertices[i] = i;
		}
	}
	/*
	Just in case passing nodes vector starts shwoing bugs, gonna save this.
	std::vector<int> temp;
	temp.reserve(reflexVertices.size() + convexVertices.size());
	temp.insert(temp.end(), reflexVertices.begin(), reflexVertices.end());
	temp.insert(temp.end(), convexVertices.begin(), convexVertices.end());
	*/
	{
		for (unsigned int i = 0; i < pointCount; ++i)
		{
			if (!nodes[i].isReflex)
				if (IsEar(nodes[i], nodes))
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

		if ((next == prev) || (next == ear) || (prev == ear))
			break;

		cs = sf::ConvexShape(3);
		cs.setPoint(0, currentNode.value);
		cs.setPoint(1, nodes[currentNode.prev].value);
		cs.setPoint(2, nodes[currentNode.next].value);
		cs.setOutlineThickness(0);
		shapes.push_back(cs);

		nodes[prev].next = next;
		nodes[next].prev = prev;

		if (nodes[prev].isReflex)
		{
			if (!(nodes[prev].isReflex = sfmath::IsReflex(nodes[prev].value, nodes[nodes[prev].prev].value, nodes[nodes[prev].next].value)))
			{
				reflexVertices[prev] = -1;
				convexVertices[prev] = prev;
			}
		}

		if (nodes[next].isReflex)
		{
			if (!(nodes[next].isReflex = sfmath::IsReflex(nodes[next].value, nodes[nodes[next].prev].value, nodes[nodes[next].next].value)))
			{
				reflexVertices[next] = -1;
				convexVertices[next] = next;
			}
		}

		auto it = ears.begin();
		while(it != ears.end())
		{
			if (!IsEar(nodes[it->second], nodes))
				it = ears.erase(it);
			else
				it++;
		}

		if (!nodes[prev].isReflex)
			if (ears.find(prev) == ears.end() && IsEar(nodes[prev], nodes));
				ears[prev] = prev;

		if (!nodes[next].isReflex)
			if (ears.find(next) == ears.end() && IsEar(nodes[next], nodes));
				ears[next] = next;

		steps++;
	}
	
	ConcaveShape concaveShape(shapes, m_shapeVertexPos);
	concaveShape.SetFont(m_labelFont);
	if (!m_unusedConcaveShapes.empty())
	{
		int index = m_unusedConcaveShapes.front();
		m_concaveShapes[index] = concaveShape;
		m_unusedConcaveShapes.pop();
	}
	else
		m_concaveShapes.push_back(concaveShape);
}

//TODO: BAD FUNCTION
bool GridManager::GetShapeContainingPoint(const sf::Vector2f& point)
{
	bool found = false;

	for (unsigned int i = 0; i < m_concaveShapes.size(); ++i)
	{
		if (m_concaveShapes[i].IsInUse() && m_concaveShapes[i].ContainsPoint(point))
		{
			m_currentShape.first = i;
			m_currentShape.second = Concave;
			found = true;
			break;

		}
	}

	if (!found)
	{
		for (unsigned int i = 0; i < m_convexShapes.size(); ++i)
		{
			if (m_convexShapes[i].IsInUse() && m_convexShapes[i].ContainsPoint(point))
			{
				m_currentShape.first = i;
				m_currentShape.second = Convex;
				found = true;
				break;

			}
		}
	}

	if (!found)
		m_currentShape.first = -1;

	return found;
}

const sf::Color GridManager::MARKED_A = sf::Color(255, 0, 0, 255);
const sf::Color GridManager::MARKED_B = sf::Color(0, 255, 0, 255);