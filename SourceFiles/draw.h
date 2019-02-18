#ifndef DRAW_H
#define DRAW_H

#include "drawable.h"
#include "pageturner.h"
#include "mathlib.h"

#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

class Draw
{
public:
	Draw(){}; 
	Draw(Drawable *drawable, vector<Light> *light); //constructor
	Mathlib math;

	void draw_line_DDA(Vertex A, Vertex B);
	void draw_triangle(Polygon *poly);

	void draw_line_DDA_phong(Vertex AA, Vertex BB, const float &ks, const float &p);
	void draw_triangle_phong(Polygon *poly);

	void draw_polygon_filled(Polygon poly);
	void draw_polygon_mesh(const Polygon poly);

	void draw_polygon_filled_phong(Polygon *poly);

	void setClippingPlane(const float &clip);

	void setDepth(const float &ne, const float &fa, const Color &dep);

	void getLight(Vertex *A, const float &ks, const float &p);

	void setCoodinateConvert(const float &xlo, const float &ylo, const float &xhi, const float &yhi, const float &PlusX, const float &PlusY);
private:
	Drawable *drawable;
	float xlo, xhi, ylo, yhi, PlusX, PlusY;

	vector<Light> *light;

	bool depthBool;
	float near, far;
	Color depth;

	double **zBuffer;
	double	zBufferX, zBufferY, clippingPlane; //size of the zBuffer, used for zBuffer refresh
	
	Vertex getCS(const float &xx, const float &yy, const float &zz);

	//line drawers;
	void draw_pixel(const int &x, const int &y, const int &z, unsigned int color);
	void set_zBuffer(const int &sizeX, const int &sizeY);
};
#endif