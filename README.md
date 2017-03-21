# MinkowskiSum
Minkowski Sum/Difference calculator. Ended up also implementing ear clipping for concave polygons.
The ear clipping is done in the "CreateConcaveShape" function.
I ended up spending more time on the ear clipping for concave polygons than I did on the Minkowski.

Note: A lot of the functions are not optimized. I'll try to mark them at some point, but in general be
aware that this is not an optimal implementation, just an implementation.

Note 2: I had a lot of difficulty understanding the paper by Baram, mostly what the collision/intersection tests are for (probably
because M-sum is for collision detection itself so seems odd to use another collision detection method to build somethign thats used for collision detection).
I've managed to get this program to work for a lot of cases, but there are still bugs. I'll come back to it later and try to fix them.

I'll probably add buttons later, but until then

## Controls:
 * Q - switches between drawing and selecting mode. When drawing a little red dot will follow the cursor
 * Drawing Mode:
   * Left Click - place vertex
   * Space - links the verices in the order they were placed. Note: Theres no testing for non-simple polygons yet, so KISS
 * Selecting Mode:
   * Left Click and Drag - move a shape around
   * Left Click, Hold + Delete/Backspace - delete shape
   * Ctrl + Left Click - marks a shape. If 2 shapes are selected B replaces A and the new shape becomes B
   * Keyboard/Numpad "+" - calculates Minkowski sum (A + B)
   * Keyboard/Numpad "-" - calculates Minkowski difference (A - B)

## Requirements:

SFML for rendering.

## Refrences:
 * Ear Clipping
   * Eberly, David. "Triangulation by ear clipping." Geometric Tools (2008). [(link)](https://www.geometrictools.com/Documentation/TriangulationByEarClipping.pdf)
 * Minkowski Sum/Difference
   * Behar, Evan, and Jyh-Ming Lien. "Fast and robust 2d minkowski sum using reduced convolution." Intelligent Robots and Systems (IROS), 2011 IEEE/RSJ International Conference on. IEEE, 2011.[(link)](http://www.ams.sunysb.edu/~jsbm/fwcg10/papers/29.pdf)
   * Baram, Alon. "Polygonal Minkowski sums via convolution: theory and practice." Tel-Aviv University, 2013. [(link)](http://acg.cs.tau.ac.il/tau-members-area/general%20publications/m.sc.-theses/thesis-lienchapter.pdf)
   * Wein, Ron. "Exact and efficient construction of planar Minkowski sums using the convolution method." European symposium on algorithms. Springer Berlin Heidelberg, 2006. [(link)](https://pdfs.semanticscholar.org/b049/3b89b18d785ca81427404ec78d7ce6602ceb.pdf)
