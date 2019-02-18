#ifndef RENDERER_H
#define RENDERER_H

#include "drawable.h"
#include "pageturner.h"
#include "mathlib.h"
#include "draw.h"
#include <algorithm>

class Renderer
{
public:
	Renderer(Drawable *drawable); //constructor
	Renderer(Drawable *drawable,int argc, char *argv[]);

	void render_scene(); //read file and render it
	
private:
	Draw draw;
	Mathlib math;

	string str;

	Color surface;
	bool isWire, phong, gouraud, flat;

	float KS, P;
	float ambRed, ambGreen, ambBlue;
	float xlow, ylow, xhigh, yhigh, hither, yon; //camera perspective
	float near, far; //used for depth color

	vector<ifstream*> streamVer;
	vector<CTM*> matrixVec;

	vector<Vertex> *vec, *vecNorm;

	vector<vector<Vertex>*> vecvec;
	vector<vector<Vertex>*> vecNormvec;

	vector<Light> light; 
	vector<Polygon> WSflat, WSgouraud, WSphong, WSwire;
	vector<Polygon> CSflat, CSgouraud, CSphong, CSwire;

	Vertex pl1, pl2, pl3, pl4, pl5, pl6; //planes for the frustrum

	CTM *matrix;
	CTM *cameraMatrix;

	void read_file(string str);
	void separatePoly(Polygon *poly); //called if polygon has more than 3 vectors
	
	Color addAmbient(const Color &A);

	void computeFrusPlanes(); //this functions computes the 6 planes of the frustrum **********TESTED

	//polygons must have size 3 past this point
	void cameraSpaceTransform(); // after all WS faces are entered, we transform to camera space
	//this function converts camera space coords into screen space coords and draws them

	void drawFlatCS();
	void drawGouraudCS();
	void drawPhongCS();
	void drawWireCS();

	//set z=1 and x & y accordingly
	void getViewCoords3(Vertex *A, Vertex *B, Vertex *C);
	void getViewCoords2(Vertex *A, Vertex *B);
	
	bool nearFarCull(Polygon * poly);
	//clip poly of size 3 againts frustrum
	void frustrumClip(Polygon * poly);
	//clip line againts frustrum
	void frustrumClipLine(Polygon *poly);
	bool inFrustrum(Vertex *A);
	
	//these 2 functions will clip faces
	void oneInTwoOut(const Vertex &inA, const Vertex &outB, const Vertex &outC);
	void twoInOneOut(const Vertex &inA, const Vertex &inB, const Vertex &outC);

	//returns the number of the frustrum plane which the line between the vertex intersects
	int getPlaneIntersect(Vertex  A, Vertex B);

	bool isIntersect(const Vertex *A);

	//returns point of hwere plane gets intersected, knowing which plane needs to be intersected
	Vertex pointIntersect(Vertex A, Vertex B, const int &frustPlane);

	//returns the points inside frustum when 2 planes get intersected
	Polygon clipPoly(Vertex A, Vertex B, Vertex C, Vertex D, const int &planeA, const int &planeB);

	//when polygon intersects 3 planes of frusrum
	void threePlaneClip(Polygon* poly, const Vertex &plane, const Vertex &point, Vertex *A, Vertex *B, Vertex *C, const int &line);

	//get the lines 2 plane lines which the poly intersects
	void getClipLines(const int &line, const bool &larger, int * L1, int *L2);

	//returns point where face of poly intersects edege of 2 frustrum planes
	Vertex linePlaneIntersect(const Vertex &face, const int &lineOfIntersect);

	void oppositeSideIntersect(Polygon *poly, const int &type, const Vertex &A, const Vertex &B, const Vertex &C, const Vertex &D);
	void convertScreenCoords(Vertex *A);

	Vertex getVec(const int &iterator);
	Vertex getVecNorm(const int &iterator);

	//insert the vertex into the right vector
	void insertWSPoly(Polygon *A);
	void insertCSPoly(Polygon *A);

	//parsing functions
	void trimWhiteSpace(string& str);
	string parseStr(string str); //parse string
	int parseFront(string str); // will parse front number regardless if its followed by "/" or "//"
	int parseNormBack(string str); // will parse back with double "//" 
	int parseBack(string str); // will parse back with single "/"
	int parseMid(string str); // will parse mid with "/" on each side
};
#endif