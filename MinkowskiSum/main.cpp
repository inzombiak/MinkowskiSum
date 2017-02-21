#include <iostream>

#include "GridManager.h"

const int WINDOW_HEIGHT = 800;
const int WINDOW_WIDTH = 800;

int main()
{
	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Minkowski Sum!");
	GridManager gm;
	gm.GenerateGrid(WINDOW_WIDTH, WINDOW_HEIGHT, 41, 41);
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
			{
				//Place points
<<<<<<< HEAD
<<<<<<< HEAD
				gm.AddVertex();
=======
				if (m_state == DrawingShape)
					gm.AddVertex();
				else if (m_state == SelectingShape)
					gm.SelectShape(sf::Vector2f(event.mouseButton.x, event.mouseButton.y));
>>>>>>> parent of 0dbf6c8... Added Convolution, still need to do loops and sum
=======
				gm.AddVertex();
>>>>>>> parent of 5923412... Added convex shape picking, now to add minkowski sum
			}
			if (event.type == sf::Event::MouseMoved)
			{
				gm.MoveDot(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
			}
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space)
			{
<<<<<<< HEAD
<<<<<<< HEAD
				gm.CreateShape();
=======
				if (m_state == SelectingShape)
					gm.DropShape();
			}
			if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::Space)
				{
					if (m_state == DrawingShape)
						gm.CreateShape();
				}
				else if (event.key.code == sf::Keyboard::Delete || event.key.code == sf::Keyboard::BackSpace)
				{
					if (m_state == SelectingShape)
						gm.DeleteCurrentShape();
				}
				else if (event.key.code == sf::Keyboard::Q)
				{
					if (m_state == SelectingShape)
					{
						m_state = DrawingShape;
						gm.ShowPointer(true);
					}
					else
					{
						m_state = SelectingShape;
						gm.ShowPointer(false);
					}

				}
>>>>>>> parent of 0dbf6c8... Added Convolution, still need to do loops and sum
=======
				gm.CreateShape();
>>>>>>> parent of 5923412... Added convex shape picking, now to add minkowski sum
			}
		}

		window.clear(sf::Color::White);
		gm.Draw(window);
		window.display();
	}

	return 0;
}