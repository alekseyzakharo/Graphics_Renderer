#ifndef MATHLIB_H
#define MATHLIB_H

#include <stdlib.h>
#include <vector>
#include <iostream>

#define PI 3.14159265

#define WORLD_X_MIN 50
#define WORLD_X_MAX 700
#define WORLD_Y_MIN 50
#define WORLD_Y_MAX 700

#define WIRED 0
#define FILLED 1

using namespace std;

struct Color
{
	unsigned short red;
	unsigned short green;
	unsigned short blue;
	unsigned int color;
	Color::Color(){}; //default
	Color::Color(unsigned int color)
	{
		red = 0xff & (color >> 16);
		green = 0xff & (color >> 8);
		blue = 0xff & (color);
		this->color = color;
	};
	Color::Color(const unsigned short red, const unsigned short green, const unsigned short blue)
	{
		this->red = red;
		this->green = green;
		this->blue = blue;
		color = (0xff << 24) + ((red & 0xff) << 16) + ((green & 0xff) << 8) + (blue & 0xff);
	};
	Color & operator= (const Color * other)
	{
		if (this != other)
		{
			this->red = other->red;
			this->green = other->green;
			this->blue = other->blue;
			this->color = other->color;
		}
		return *this;
	}
};

struct Vertex
{
	float x, y, z, w;
	Color color;
	Vertex *norm = NULL;

	Vertex::Vertex(){}; //default

	Vertex::Vertex(const float &x, const float &y, const float &z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		w = 1;
	};

	Vertex::Vertex(const float &x, const float &y, const float &z, const unsigned int &color)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		w = 1;
		this->color = Color(color);
	};

	Vertex::Vertex(const float &x, const float &y, const float &z, const float &w)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
		this->color = Color(color);
	};

	Vertex::Vertex(const float &x, const float &y, const float &z, const Color &color)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		w = 1;
		this->color = color;
	};

	Vertex & operator= (const Vertex * other)
	{
		if (other == NULL)
			*this = NULL;
		else if (this != other)
		{
			this->x = other->x;
			this->y = other->y;
			this->z = other->z;
			this->w = other->w;
			this->color = Color(other->color.color);
			if (norm != NULL)
				this->norm = other->norm;

		}
		return *this;
	};

	void operator* (const float &num)
	{
		x *= num;
		y *= num;
		z *= num;
	};

	Vertex & operator- (const Vertex * other)
	{
		if (this != other)
		{
			x -= (*other).x;
			y -= (*other).y;
			z -= (*other).z;
		}
		return *this;
	};
};

struct Polygon
{
	std::vector<Vertex> vectors;
	float ks, p;
	Polygon & operator= (const Polygon * other)
	{
		Vertex tmp;
		this->ks = other->ks;
		this->p = other->p;
		if (this != other)
		{
			int vecSize = (int)(*other).vectors.size();
			for (int i = 0; i < vecSize; i++)
			{
				tmp = (*other).vectors.at(i);
				tmp.norm = new Vertex((*(*other).vectors.at(i).norm));
				vectors.push_back(tmp);
			}
		}
		return *this;
	};
};

struct CTM
{
	double **ctm;
	CTM::CTM()
	{
		ctm = new double*[4];
		for (int i = 0; i < 4; i++)
			ctm[i] = new double[4];

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if (i == j)
					ctm[i][j] = 1;
				else
					ctm[i][j] = 0;
			}
		}
	};
	CTM::~CTM()
	{
		for (int i = 0; i < 4; i++)
		{
			delete[] ctm[i];
		}
		delete[] ctm;
	};
};

struct Light
{
	Vertex loc;
	float A, B;
	float red, green, blue;
	Light::Light(const float &red, const float &green, const float &blue, const float &A, const float &B, const Vertex &loc)
	{
		this->red = red;
		this->green = green;
		this->blue = blue;
		this->A = A;
		this->B = B;
		this->loc = loc;
	};
};

class Mathlib
{
public:
	Mathlib(){};

	float xlo, xhi, ylo, yhi, PlusX, PlusY;

	Polygon get_poly2(const Vertex &A, const Vertex &B);
	Polygon get_poly3(const Vertex &A, const Vertex &B, const Vertex &C);
	
	void scale(const CTM *A, const double &sx, const double &sy, const double &sz);
	void rotate(const CTM *A, const int &axis, const double &angle);
	void translate(const CTM *A, const double &tx, const double &ty, const double &tz);

	void multiplyCTM(const CTM *A, const CTM* B);
	//Inverse 4x4 Matrix using Cramers Rule
	CTM* inverseCTM(const CTM* A);
	void copyCTM(const CTM* A, const CTM* B);

	void CTM_Vertex3_multiply(const CTM *A, Vertex *B);
	void PolyMatrixMult(const CTM *mat, Polygon *poly);
	
	//assign normals to every vertex in a polygon
	void assignNorms(Polygon *poly);

	void assignNormsPhong(Polygon *poly);

	//return center point of polygon
	Vertex getCenter(const Polygon* poly);

	//return the average normal of the normals associated with the polygon
	Vertex averageNorm(const Polygon *poly);

	//return average color
	Color averageColor(const Polygon *poly);

	//retuns normalzlied normal to triangle
	Vertex getNormalizedNormal(const Vertex &A, const Vertex &B, const Vertex &C);

	//normalized vector
	void normalizeVec(Vertex *A);

	//return the equation plane based on 3 vertex points
	Vertex getPlane(const Vertex &A, const Vertex &B, const Vertex &C);

	//return point where line A->B intersects plane
	Vertex getPlaneIntersect(const Vertex &plane, Vertex &A, const Vertex &B);

	//get color of point in a triangle plane
	Color colInFace(const Vertex &point, const Vertex &A, const Vertex &B, const Vertex &C);

	//get the blerped normal vector in the face of the poly
	Vertex* nomalInFace(const Vertex &point, const Vertex &A, const Vertex &B, const Vertex &C);

	//get color of Pt which sits between A and B
	Color betweenTwoPoints(const Vertex &A, const Vertex &B, const Vertex &Pt);

	//get normal between 2 points
	Vertex* normBetweenTwoPoints(const Vertex &A, const Vertex &B, const Vertex &Pt);
	
	//return dot product of 2 vertexes
	float EucliDotProd(const Vertex &A, const Vertex &B);

	//vector from A to B
	Vertex getVector(const Vertex &A, const Vertex &B);

	//distance between 2 points
	float getDist(const Vertex &A, const Vertex &B);
	//returns normalized vector from A-->B
	Vertex normalizeVec(const Vertex &A, const Vertex &B);

	void transpose_points(Polygon *poly);
	void modify_points2(Polygon *poly);
	
	bool order_triangle(Polygon *poly); 
	bool get_xy_interchange(Polygon *poly);

	unsigned int make_color(const int &red, const int &green, const int &blue);

	int screenspaceX(const int &x); //turn x into screen coord for QT
	int screenspaceY(const int &y); //turn y into screen coord for QT
private:
	//return the determinant of a 3x3 matrix
	double getDeterminant(const double &a, const double &b, const double &c, const double &d, const double &e, const double &f, const double &g, const double &h, const double &i);
	//transpose 4x4 matrix
	void transposeCTM(const CTM* A);
};
#endif