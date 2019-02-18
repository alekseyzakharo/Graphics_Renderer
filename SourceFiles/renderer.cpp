#include "renderer.h"
#define TRUE 0

Renderer::Renderer(Drawable *drawable)
{
	draw = Draw(drawable,&light);

	matrix = new CTM();
	isWire = false;
	
	phong = true;
	gouraud = false;
	flat = false;

	//set ambient light to 0 0 0
	ambRed = 0, ambGreen = 0, ambBlue = 0;
	surface = Color(255, 255, 255); //default initial surface color 1 1 1
	KS = (float)0.3;
	P = (float)0.8;
}

Renderer::Renderer(Drawable *drawable,int argc ,char *argv[])
{
	draw = Draw(drawable,&light);
	if (argc < 2)
		return;
	else
		str = argv[1];

	matrix = new CTM();
	isWire = false;

	phong = true;
	gouraud = false;
	flat = false;

	//set ambient light to 0 0 0
	ambRed = 0, ambGreen = 0, ambBlue = 0;
	surface = Color(255, 255, 255); //default initial surface color 1 1 1
	KS = (float)0.3;
	P = (float)0.8;
}

void Renderer::render_scene()
{
	read_file(str);
	//At point our camera is set up
	computeFrusPlanes();
	cameraSpaceTransform(); //transform all faces into camera space

	//draw all the different possible faces
	drawFlatCS();
	drawGouraudCS();
	drawPhongCS();
	drawWireCS();
}

void Renderer::read_file(string str)
{
	ifstream *myFile = new ifstream(str);
	if (!(*myFile).is_open())
		return;
	int axis;
	string line, buffer;
	while (!(*myFile).eof())
	{
		getline((*myFile), line);
		replace(line.begin(), line.end(), ',', ' '); //replace all commas with white space
		replace(line.begin(), line.end(), '\t', ' ');
		trimWhiteSpace(line);

		if (line.empty() || line.compare("\t") == TRUE) //empty line or new line = both empty
			continue;
		if (line.at(0) == '#')
			continue;
		if (line.front() == '{')
		{
			matrixVec.push_back(matrix);
			matrix = new CTM();
			math.copyCTM(matrix, matrixVec.back());
		}
		else if (line.front() == '}')
		{
			delete matrix;
			if (!matrixVec.empty())
			{
				matrix = matrixVec.back();
				matrixVec.pop_back();
			}
		}
		else
		{
			stringstream ss(line);
			vector<string> tokens;
			while (ss >> buffer)
			{
				buffer = parseStr(buffer);
				tokens.push_back(buffer);
			}
			buffer = tokens.front();
			if (buffer.compare("file") == TRUE)
			{
				streamVer.push_back(myFile);

				vecvec.push_back(vec);
				vecNormvec.push_back(vecNorm);
				vec = new vector<Vertex>;
				vecNorm = new vector<Vertex>;

				read_file(tokens.at(1).append(".SIMP"));
				
				delete vec;
				delete vecNorm;
				vec = vecvec.back();
				vecNorm = vecNormvec.back();
				vecvec.pop_back();
				vecNormvec.pop_back();

				myFile = streamVer.back();
				streamVer.pop_back();
			}
			else if (buffer.compare("obj") == TRUE)
			{
				streamVer.push_back(myFile);

				vecvec.push_back(vec);
				vecNormvec.push_back(vecNorm);
				vec = new vector<Vertex>;
				vecNorm = new vector<Vertex>;

				read_file(tokens.at(1).append(".OBJ"));

				delete vec;
				delete vecNorm;
				vec = vecvec.back();
				vecNorm = vecNormvec.back();
				vecvec.pop_back();
				vecNormvec.pop_back();

				myFile = streamVer.back();
				streamVer.pop_back();
			}
			else if (buffer.compare("camera") == TRUE)
			{
				xlow = stof(tokens.at(1));
				ylow = stof(tokens.at(2));
				xhigh = stof(tokens.at(3));
				yhigh = stof(tokens.at(4));
				hither = stof(tokens.at(5));
				yon = stof(tokens.at(6));
				draw.setClippingPlane(yon);
				//set inverse of current matrix as camera matrix
				float x = xhigh - xlow;
				float y = yhigh - ylow;
				if (x == y)
					draw.setCoodinateConvert(xlow, ylow, xhigh, yhigh, (float)0, (float)0);
				else if (x < y)
					draw.setCoodinateConvert(xlow, ylow, xhigh, yhigh, (float)((650-((x/y)*650))/2), (float)0);
				else if (x > y)
					draw.setCoodinateConvert(xlow, ylow, xhigh, yhigh, (float)0, (float)((650 - ((x / y) * 650)) / 2));
				
				cameraMatrix = math.inverseCTM(matrix);
				
			}
			else if (buffer.compare("light") == TRUE)
			{
				Vertex tmp((float)0, (float)0, (float)0); //set tmp 0 0 0
				math.CTM_Vertex3_multiply(matrix, &tmp); // multiply by current transformation matrix to set the light source
				Light li((stof(tokens.at(1)) * 255), (stof(tokens.at(2)) * 255), (stof(tokens.at(3)) * 255), stof(tokens.at(4)), stof(tokens.at(5)), tmp);
				light.push_back(li); //push light source onto stack
			}
			else if (buffer.compare("phong") == TRUE)
			{
				flat = false;
				gouraud = false;
				phong = true;
			}
			else if (buffer.compare("gouraud") == TRUE)
			{
				flat = false;
				gouraud = true;
				phong = false;
			}
			else if (buffer.compare("flat") == TRUE)
			{
				flat = true;
				gouraud = false;
				phong = false;
			}
			else if (buffer.compare("ambient") == TRUE)
			{
				ambRed = stof(tokens.at(1));
				ambGreen = stof(tokens.at(2));
				ambBlue = stof(tokens.at(3));
			}
			else if (buffer.compare("depth") == TRUE) 
			{
				Color depth((short)(stof(tokens.at(3)) * 255), (short)(stof(tokens.at(4)) * 255), (short)(stof(tokens.at(5)) * 255));
				draw.setDepth(stof(tokens.at(1)), stof(tokens.at(2)), depth);
			}
			else if (buffer.compare("surface") == TRUE)
			{
				surface = Color((short)(stof(tokens.at(1)) * 255), (short)(stof(tokens.at(2)) * 255), (short)(stof(tokens.at(3)) * 255));
				KS = stof(tokens.at(4));
				P = stof(tokens.at(5));
			}
			else if (buffer.compare("v") == TRUE) //token(0) = "v", +1 to all switch statements
			{
				Vertex A;
				Color color;
				switch (tokens.size())
				{
				case 4: //v x y z, set color to surface color
						A = Vertex(stof(tokens.at(1)), stof(tokens.at(2)), stof(tokens.at(3)), addAmbient(surface.color));
					break;
				case 5: //v x y z w
						A = Vertex(stof(tokens.at(1)) / stof(tokens.at(4)), stof(tokens.at(2)) / stof(tokens.at(4)), stof(tokens.at(3)) / stof(tokens.at(4)), addAmbient(surface.color));
					break;
				case 7: //v x y z r g b
					color = Color((short)(stof(tokens.at(4)) * 255), (short)(stof(tokens.at(5)) * 255), (short)(stof(tokens.at(6)) * 255));
					A = Vertex(stof(tokens.at(1)), stof(tokens.at(2)), stof(tokens.at(3)), addAmbient(color));
					break;
				case 8: //v x y z w r g b
					color = Color((short)(stof(tokens.at(5)) * 255), (short)(stof(tokens.at(6)) * 255), (short)(stof(tokens.at(7)) * 255));
					A = Vertex(stof(tokens.at(1)) / stof(tokens.at(4)), stof(tokens.at(2)) / stof(tokens.at(4)), stof(tokens.at(3)) / stof(tokens.at(4)), addAmbient(color));
					break;
				}
				math.CTM_Vertex3_multiply(matrix, &A);
				(*vec).push_back(A);
			}
			else if (buffer.compare("vn") == TRUE)
			{
				Vertex A(stof(tokens.at(1)), stof(tokens.at(2)), stof(tokens.at(3)));
				math.CTM_Vertex3_multiply(matrix, &A); 
				(*vecNorm).push_back(A); 
			}
			else if (buffer.compare("f") == TRUE) 
			{
				Polygon poly;
				Vertex Vec, VecNorm;
				int i = 1;					//first token since 0 = "f"
				int n = (int)tokens.size();		//size of tokens		
				if (tokens.at(1).find("/") != string::npos) //found
				{
					if (tokens.at(1).find("//") != string::npos) //string sepertated by "//"
					{
						while (i < n)
						{
							Vec = getVec(parseFront(tokens.at(i))); 
							//VecNorm = getVecNorm(parseNormBack(tokens.at(i))); 
							Vec.norm = new Vertex(getVecNorm(parseNormBack(tokens.at(i))));
							//Vec.norm = new Vertex(VecNorm); //set vec norm for the vertex
							poly.vectors.push_back(Vec); //push vec from left of //
							//poly.vecNorm.push_back(VecNorm); //push vecNorm from right of //
							i++;
						}
					}
					else //we know the string is separated by "/"
					{
						if (count(tokens.at(1).begin(), tokens.at(1).end(), '/') == 1) //at this point we konw that we have 3/3
						{
							while (i < n)
							{
								Vec = getVec(parseFront(tokens.at(i))); 
								poly.vectors.push_back(Vec);

								//do nothing for texture normal at end for now
								i++;
							}
						}
						else // 3/3/3
						{
							while (i < n)
							{
								Vec = getVec(parseFront(tokens.at(i))); //get vec
								//VecNorm = getVecNorm(parseBack(tokens.at(i)));
								
								Vec.norm = new Vertex(getVecNorm(parseBack(tokens.at(i))));

								//do nothing for texture normal in middle for now

								//poly.vectors.push_back(VecNorm);
								poly.vectors.push_back(Vec);
								i++;
							}
						}
					}
				}
				else	//only vertices specified
				{
					while (i < n)
					{
						Vec = getVec(parseFront(tokens.at(i))); //get vec
						poly.vectors.push_back(Vec);
						i++;
					}
				}
				if (poly.vectors.size() > 3) //if polygon has more than 3 vectors
					separatePoly(&poly);
				else
					insertWSPoly(&poly);
					//WSface.push_back(poly); 
			}
			else if (buffer.compare("wire") == TRUE)
				isWire = true;
			else if (buffer.compare("filled") == TRUE)
				isWire = false;
			else if (buffer.compare("scale") == TRUE)
				math.scale(matrix, stod(tokens.at(1)), stod(tokens.at(2)), stod(tokens.at(3)));
			else if (buffer.compare("rotate") == TRUE)
			{
				if (tokens.at(1).compare("X") == TRUE)
					axis = 1;
				else if (tokens.at(1).compare("Y") == TRUE)
					axis = 2;
				else
					axis = 3;
				math.rotate(matrix, axis, stod(tokens.at(2)));
			}
			else if (buffer.compare("translate") == TRUE)
				math.translate(matrix, stod(tokens.at(1)), stod(tokens.at(2)), stod(tokens.at(3)));
			else if (buffer.compare("line") == TRUE) 
			{
				if (!isWire) //do not make 2 vertex polygons if were not in wire mode.
					continue;
				Color colA, colB;
				Vertex A, B;
				Polygon poly;
				switch (tokens.size())
				{
				case 7: // line x y z x y z = 7
						A = Vertex(stof(tokens.at(1)), stof(tokens.at(2)), stof(tokens.at(3)), addAmbient(surface.color));
						B = Vertex(stof(tokens.at(4)), stof(tokens.at(5)), stof(tokens.at(6)), addAmbient(surface.color));
					break;
				case 13: // line x y z r g b x y z r g b = 13
					colA = Color((short)(stof(tokens.at(4)) * 255), (short)(stof(tokens.at(5)) * 255), (short)(stof(tokens.at(6)) * 255));
					colB = Color((short)(stof(tokens.at(10)) * 255), (short)(stof(tokens.at(11)) * 255), (short)(stof(tokens.at(12)) * 255));
					A = Vertex(stof(tokens.at(1)), stof(tokens.at(2)), stof(tokens.at(3)), addAmbient(colA.color));
					B = Vertex(stof(tokens.at(7)), stof(tokens.at(8)), stof(tokens.at(9)), addAmbient(colB.color));
					break;
				}
				//MULTIPLY BY CTM
				math.CTM_Vertex3_multiply(matrix, &A);
				math.CTM_Vertex3_multiply(matrix, &B);
				poly.vectors.push_back(A);
				poly.vectors.push_back(B);
				WSwire.push_back(poly); //ADD LINE TO WIRE VECTOR
			}
			else if (buffer.compare("polygon") == TRUE) 
			{
				Color colA, colB, colC;
				Vertex A, B, C;
				Polygon poly;
				switch (tokens.size())
				{
				case 10: // polygon x y z x y z x y z = 10
						A = Vertex(stof(tokens.at(1)), stof(tokens.at(2)), stof(tokens.at(3)), addAmbient(surface.color));
						B = Vertex(stof(tokens.at(4)), stof(tokens.at(5)), stof(tokens.at(6)), addAmbient(surface.color));
						C = Vertex(stof(tokens.at(7)), stof(tokens.at(8)), stof(tokens.at(9)), addAmbient(surface.color));
					break;
				case 19: // polygon x y z r g b x y z r g b x y z r g b = 19
					colA = Color((short)(stof(tokens.at(4)) * 255), (short)(stof(tokens.at(5)) * 255), (short)(stof(tokens.at(6)) * 255));
					colB = Color((short)(stof(tokens.at(10)) * 255), (short)(stof(tokens.at(11)) * 255), (short)(stof(tokens.at(12)) * 255));
					colC = Color((short)(stof(tokens.at(16)) * 255), (short)(stof(tokens.at(17)) * 255), (short)(stof(tokens.at(18)) * 255));
					A = Vertex(stof(tokens.at(1)), stof(tokens.at(2)), stof(tokens.at(3)), addAmbient(colA.color));
					B = Vertex(stof(tokens.at(7)), stof(tokens.at(8)), stof(tokens.at(9)), addAmbient(colB.color));
					C = Vertex(stof(tokens.at(13)), stof(tokens.at(14)), stof(tokens.at(15)), addAmbient(colC.color));
					break;
				}
				//Multi CTM
				math.CTM_Vertex3_multiply(matrix, &A);
				math.CTM_Vertex3_multiply(matrix, &B);
				math.CTM_Vertex3_multiply(matrix, &C);
				poly.vectors.push_back(A);
				poly.vectors.push_back(B);
				poly.vectors.push_back(C);
				insertWSPoly(&poly); //insert the polygon into the appropriate vector depending on lighting
				//WSface.push_back(poly);
			}
		}
	}
	(*myFile).close();
}


void Renderer::separatePoly(Polygon *poly)
{
	int A = 0, B = 1, C = 2;
	int size = (int)(*poly).vectors.size();	
	while (C < size)
	{
		Polygon tmp;
		tmp.ks = (*poly).ks;
		tmp.p = (*poly).p;
		tmp.vectors.push_back((*poly).vectors.at(A));
		tmp.vectors.push_back((*poly).vectors.at(B));
		tmp.vectors.push_back((*poly).vectors.at(C));
		insertWSPoly(&tmp);
		
		B++;
		C++;
	}
}


Color Renderer::addAmbient(const Color &A)
{
	short red = (short)(ambRed*A.red);
	short green = (short)(ambGreen*A.green);
	short blue = (short)(ambBlue*A.blue);
	return Color(red, green, blue);
}

void Renderer::computeFrusPlanes()
{
	Vertex A((float)(xlow*hither), (float)(ylow*hither), (float)(hither));
	Vertex B((float)(xhigh*hither), (float)(ylow*hither), (float)(hither));
	Vertex C((float)(xhigh*hither), (float)(yhigh*hither), (float)(hither));
	Vertex D((float)(xlow*hither), (float)(yhigh*hither), (float)(hither));

	Vertex E((float)(xlow*yon), (float)(ylow*yon), (float)(yon));
	Vertex F((float)(xhigh*yon), (float)(ylow*yon), (float)(yon));
	Vertex G((float)(xhigh*yon), (float)(yhigh*yon), (float)(yon));
	Vertex H((float)(xlow*yon), (float)(yhigh*yon), (float)(yon));


	pl1 = math.getPlane(A, B, C);
	pl2 = math.getPlane(H, D, C);
	pl3 = math.getPlane(H, A, D);
	pl4 = math.getPlane(A, E, F);
	pl5 = math.getPlane(C, B, F);
	pl6 = math.getPlane(H, E, F);
}

void Renderer::cameraSpaceTransform()
{
	isWire = true, gouraud = false, phong = false, flat = false; //set all = false
	Polygon polyWire, polyGouraud, polyPhong, polyFlat;

	//********************SET ISWIRE = TRUE
	int size = (int)WSwire.size();
	for (int i = 0; i < size; i++) 
	{
		polyWire = WSwire.at(i);
		int polSize = (int)polyWire.vectors.size();
		math.PolyMatrixMult(cameraMatrix, &polyWire);
		if (!nearFarCull(&polyWire)) 
		{
			if (polSize == 2)
				frustrumClipLine(&polyWire);
			else
				frustrumClip(&polyWire);
		}	
	}
	isWire = false, gouraud = true;
	//********************SET GOURAUD = TRUE
	size = (int)WSgouraud.size();
	for (int i = 0; i < size; i++) 
	{
		polyGouraud = WSgouraud.at(i);
		int polSize = (int)polyGouraud.vectors.size();
		math.PolyMatrixMult(cameraMatrix, &polyGouraud); 
		if (!nearFarCull(&polyGouraud)) 
		{
			if (polSize == 2)
				frustrumClipLine(&polyGouraud);
			else
				frustrumClip(&polyGouraud);
		}
	}
	gouraud = false, phong = true;
	//********************SET PHONG = TRUE
	size = (int)WSphong.size();
	for (int i = 0; i < size; i++)
	{
		polyPhong = WSphong.at(i);
		int polSize = (int)polyPhong.vectors.size();
		math.PolyMatrixMult(cameraMatrix, &polyPhong); 
		if (!nearFarCull(&polyPhong)) 
		{
			if (polSize == 2)
				frustrumClipLine(&polyPhong);
			else
				frustrumClip(&polyPhong);
		}
	}
	phong = false, flat = true;
	//********************SET FLAT = TRUE
	size = (int)WSflat.size();
	for (int i = 0; i < size; i++) 
	{
		polyFlat = WSflat.at(i);
		int polSize = (int)polyFlat.vectors.size();
		math.PolyMatrixMult(cameraMatrix, &polyFlat); 
		if (!nearFarCull(&polyFlat)) 
		{
			if (polSize == 2)
				frustrumClipLine(&polyFlat);
			else
				frustrumClip(&polyFlat);
		}
	}
	//TRANSFORM THE LIGHT SOURCES
	size = (int)light.size();
	for (int i = 0; i < size; i++)
	{
		math.CTM_Vertex3_multiply(cameraMatrix, &light.at(i).loc);
	}
}

void Renderer::drawWireCS()
{
	Polygon poly;
	Vertex *A, *B, *C;
	int size = (int)CSwire.size();
	for (int i = 0; i < size; i++)
	{
		poly = CSwire.at(i);
		int polSize = (int)poly.vectors.size();
		if (polSize == 2)
		{
			A = &poly.vectors.at(0), B = &poly.vectors.at(1);

			getViewCoords2(A, B); //Convert to screen space, Leave the z as it is as we need it for the z-buffer for drawing
			convertScreenCoords(A);
			convertScreenCoords(B);
		}
		else
		{
			A = &poly.vectors.at(0), B = &poly.vectors.at(1), C = &poly.vectors.at(2);
			if (A->z == 0 || B->z == 0 || C->z == 0)
				continue;
			getViewCoords3(A, B, C);	//convert to screen space, Leave the z as it is as we need it for the z-buffer for drawing
			convertScreenCoords(A);
			convertScreenCoords(B);
			convertScreenCoords(C);
		}
		draw.draw_polygon_mesh(poly);
	}
}

void Renderer::drawFlatCS()
{
	Polygon poly;
	Vertex *A, *B, *C, center, avgNorm;
	int size = (int)CSflat.size();
	for (int i = 0; i < size; i++)
	{
		poly = CSflat.at(i);
		math.assignNorms(&poly); 

		center = math.getCenter(&poly);
		center.norm = new Vertex(math.averageNorm(&poly));

		math.normalizeVec(center.norm);
		center.color = math.averageColor(&poly); //get average color of all vertex's
		
		draw.getLight(&center, poly.ks, poly.p); //this gets the lighing calculation for center of the polygon

		//at this point every vertex has a normal associated with it
		A = &poly.vectors.at(0), B = &poly.vectors.at(1), C = &poly.vectors.at(2);
		(*A).color = center.color, (*B).color = center.color, (*C).color = center.color;

		if (A->z == 0 || B->z == 0 || C->z == 0)
			continue;

		getViewCoords3(A, B, C);	//convert to screen space, Leave the z as it is as we need it for the z-buffer for drawing

		convertScreenCoords(A);
		convertScreenCoords(B);
		convertScreenCoords(C);

		draw.draw_polygon_filled(poly);
	}
}

void Renderer::drawGouraudCS()
{
	Polygon poly;
	Vertex *A, *B, *C;
	int size = (int)CSgouraud.size();
	for (int i = 0; i < size; i++)
	{
		poly = CSgouraud.at(i);
		math.assignNorms(&poly);
		//at this point every vertex has a normal associated with it
		A = &poly.vectors.at(0), B = &poly.vectors.at(1), C = &poly.vectors.at(2);

		draw.getLight(A, poly.ks, poly.p); //Assign color calculation to the vertexes
		draw.getLight(B, poly.ks, poly.p);
		draw.getLight(C, poly.ks, poly.p);

		if (A->z == 0 || B->z == 0 || C->z == 0)
			continue;

		getViewCoords3(A, B, C);	//convert to screen space, Leave the z as it is as we need it for the z-buffer for drawing

		convertScreenCoords(A);
		convertScreenCoords(B);
		convertScreenCoords(C);

		draw.draw_polygon_filled(poly);
	}
}

void Renderer::drawPhongCS()
{
	Polygon poly;
	Vertex *A, *B, *C;
	int size = (int)CSphong.size();
	for (int i = 0; i <size; i++)
	{
		poly = CSphong.at(i);
		math.assignNormsPhong(&poly);
		//at this point every vertex has a normal associated with it
	
		A = &poly.vectors.at(0), B = &poly.vectors.at(1), C = &poly.vectors.at(2);
		if (A->z == 0 || B->z == 0 || C->z == 0)
			continue;
		
		getViewCoords3(A, B, C);	//convert to screen space, Leave the z as it is as we need it for the z-buffer for drawing
		
		convertScreenCoords(A);
		convertScreenCoords(B);
		convertScreenCoords(C);

		//MAKE SURE ALL OF THE VERTEXES HAS A NORMAL ASSOCIATED WITH IT, REWRITE DRAWING FUNCTION TO BLERP THE NORMAL WITH IT
		draw.draw_polygon_filled_phong(&poly);
	}
}

void Renderer::getViewCoords3(Vertex *A, Vertex *B, Vertex *C)
{
	(*A).x = (*A).x / (*A).z;
	(*A).y = (*A).y / (*A).z;
	(*B).x = (*B).x / (*B).z;
	(*B).y = (*B).y / (*B).z;
	(*C).x = (*C).x / (*C).z;
	(*C).y = (*C).y / (*C).z;
}

void Renderer::getViewCoords2(Vertex *A, Vertex *B)
{
	(*A).x = (*A).x / (*A).z;
	(*A).y = (*A).y / (*A).z;
	(*B).x = (*B).x / (*B).z;
	(*B).y = (*B).y / (*B).z;
}


bool Renderer::nearFarCull(Polygon * poly)
{
	bool cull = true;
	int size = (int)(*poly).vectors.size();
	for (int i = 0; i < size; i++)
	{
		if ((*poly).vectors.at(i).z > hither || (*poly).vectors.at(i).z < yon)
			cull = false;
	}
	return cull;
}

void Renderer::frustrumClipLine(Polygon *poly)
{
	//we know that this polygon only has 2 points
	Vertex *A = &(*poly).vectors.at(0);
	Vertex *B = &(*poly).vectors.at(1);
	Vertex clipPt;
	Polygon pol;
	int planeIntersect;
	if (inFrustrum(A))
	{
		if (inFrustrum(B)) //v2
			insertCSPoly(poly); //both inside add to camera space poly vec
		else
		{
			planeIntersect = getPlaneIntersect(*A, *B);
			clipPt = pointIntersect(*A, *B,planeIntersect);
			clipPt.color = math.betweenTwoPoints(*A, *B, clipPt);
			pol.vectors.push_back(*A);
			pol.vectors.push_back(clipPt);
			insertCSPoly(&pol);
		}
	}
	else
	{
		if (inFrustrum(B)) //v2
		{
			planeIntersect = getPlaneIntersect(*B, *A);
			clipPt = pointIntersect(*B, *A, planeIntersect);
			clipPt.color = math.betweenTwoPoints(*B, *A, clipPt);
			pol.vectors.push_back(*B);
			pol.vectors.push_back(clipPt);
			insertCSPoly(&pol);
		}
		else
			return;//both out do nothing
	}
}

void Renderer::frustrumClip(Polygon * poly)
{
	Vertex *A = &(*poly).vectors.at(0);
	Vertex *B = &(*poly).vectors.at(1);
	Vertex *C = &(*poly).vectors.at(2);
	if (inFrustrum(A)) //v1
	{
		if (inFrustrum(B)) //v2
		{
			if (inFrustrum(C)) //(v1, v2, v3) = in
				insertCSPoly(poly); //all 3 vectors inside add to camera stack
			else //(v1 v2)=in (v3)=out
				twoInOneOut(*A, *B, *C);
		}
		else //!v2
		{
			if (inFrustrum(C)) //(v1,v3)= in (v2)=out
				twoInOneOut(*A, *C, *B);
			else //v1=in v2,v3=out
				oneInTwoOut(*A, *B, *C);
		}
	}
	else
	{
		if (inFrustrum(B))
		{
			if (inFrustrum(C)) //(v2, v3) = in (v1)=out
				twoInOneOut(*B, *C, *A);
			else //(v2)=in (v1,v3)=out
				oneInTwoOut(*B, *A, *C);
		}
		else //!v2
		{
			if (inFrustrum(C)) //(v3)= in (v1,v2)=out
				oneInTwoOut(*C, *A, *B);
			else //ALL OUT
				return; //do nothing
		}
	}
}

//clipping function where one verte is inside frustrum and 2 are out
void Renderer::oneInTwoOut(const Vertex &inA, const Vertex &outB, const Vertex &outC)
{
	int planeA, planeB;
	Vertex clipA, clipB;
	Polygon multiPlaneIntersect;
	planeA = getPlaneIntersect(inA, outC);
	planeB = getPlaneIntersect(inA, outB);
	clipA = pointIntersect(inA, outC, planeA);
	clipB = pointIntersect(inA, outB, planeB);
	clipA.norm = math.normBetweenTwoPoints(inA, outB, clipA); //get the norm
	clipB.norm = math.normBetweenTwoPoints(inA, outC, clipB); //get the norm
	if (planeA == planeB) //A->C and B->C intersect same plane
	{
		clipA.color = math.colInFace(clipA, inA, outB, outC);
		clipB.color = math.colInFace(clipB, inA, outB, outC);
		insertCSPoly(&math.get_poly3(inA, clipB, clipA));
	}
	else
	{
		multiPlaneIntersect = clipPoly(inA, outB, inA, outC, planeA, planeB);
		switch ((int)multiPlaneIntersect.vectors.size())
		{
		case 1:
			multiPlaneIntersect.vectors.at(0).norm = math.nomalInFace(multiPlaneIntersect.vectors.at(0), inA, outB, outC);

			insertCSPoly(&math.get_poly3(inA, clipA, multiPlaneIntersect.vectors.at(0)));
			insertCSPoly(&math.get_poly3(inA, multiPlaneIntersect.vectors.at(0), clipB));
			break;
		case 2:
			multiPlaneIntersect.vectors.at(0).norm = math.nomalInFace(multiPlaneIntersect.vectors.at(0), inA, outB, outC);
			multiPlaneIntersect.vectors.at(1).norm = math.nomalInFace(multiPlaneIntersect.vectors.at(1), inA, outB, outC);

			insertCSPoly(&math.get_poly3(inA, clipA, multiPlaneIntersect.vectors.at(0)));
			insertCSPoly(&math.get_poly3(inA, multiPlaneIntersect.vectors.at(0), multiPlaneIntersect.vectors.at(1)));
			insertCSPoly(&math.get_poly3(inA, multiPlaneIntersect.vectors.at(1), clipB));
			break;
		}
	}
}


//helper function where 2 points are inside frustrum and 1 point is out
void Renderer::twoInOneOut(const Vertex &inA, const Vertex &inB, const Vertex &outC)
{
	int planeA, planeB;
	Vertex clipA, clipB;
	Polygon multiPlaneIntersect;
	planeA = getPlaneIntersect(inA, outC);
	planeB = getPlaneIntersect(inB, outC);
	clipA = pointIntersect(inA, outC, planeA);
	clipB = pointIntersect(inB, outC, planeB);
	clipA.norm = math.normBetweenTwoPoints(inA, outC, clipA); //get the norm
	clipB.norm = math.normBetweenTwoPoints(inB, outC, clipB); //get the norm
	if (planeA == planeB) //A->C and B->C intersect same plane
	{
		clipA.color = math.colInFace(clipA, inA, inB, outC);
		clipB.color = math.colInFace(clipB, inA, inB, outC);
		insertCSPoly(&math.get_poly3(inA, clipB, clipA));
		insertCSPoly(&math.get_poly3(inA, inB, clipB));
	}
	else
	{
		multiPlaneIntersect = clipPoly(inA, outC, inB, outC, planeA, planeB);
		switch ((int)multiPlaneIntersect.vectors.size())
		{
		case 1:
			multiPlaneIntersect.vectors.at(0).norm = math.nomalInFace(multiPlaneIntersect.vectors.at(0), inA, inB, outC);

			insertCSPoly(&math.get_poly3(inA, clipA, multiPlaneIntersect.vectors.at(0)));
			insertCSPoly(&math.get_poly3(inA, multiPlaneIntersect.vectors.at(0), clipB));
			insertCSPoly(&math.get_poly3(inA, clipB, inB));
			break;
		case 2:
			multiPlaneIntersect.vectors.at(0).norm = math.nomalInFace(multiPlaneIntersect.vectors.at(0), inA, inB, outC);
			multiPlaneIntersect.vectors.at(1).norm = math.nomalInFace(multiPlaneIntersect.vectors.at(1), inA, inB, outC);

			insertCSPoly(&math.get_poly3(inA, clipA, multiPlaneIntersect.vectors.at(0)));
			insertCSPoly(&math.get_poly3(inA, multiPlaneIntersect.vectors.at(0), multiPlaneIntersect.vectors.at(1)));
			insertCSPoly(&math.get_poly3(inA, multiPlaneIntersect.vectors.at(1), clipB));
			insertCSPoly(&math.get_poly3(inA, clipB, inB));
			break;
		}
	}
}

//test the line againts all frustrum planes and return the number of the one that it intersercts
int Renderer::getPlaneIntersect(Vertex  A,  Vertex B)
{
	Vertex tmp;
	int i = 1;
	tmp = math.getPlaneIntersect(pl1, A, B);
	if (isIntersect(&tmp))
		return i;
	i++;
	tmp = math.getPlaneIntersect(pl2, A, B);
	if (isIntersect(&tmp))
		return i;
	i++; 
	tmp = math.getPlaneIntersect(pl3, A, B);
	if (isIntersect(&tmp))
		return i;
	i++; 
	tmp = math.getPlaneIntersect(pl4, A, B);
	if (isIntersect(&tmp))
		return i;
	i++; 
	tmp = math.getPlaneIntersect(pl5, A, B);
	if (isIntersect(&tmp))
		return i;
	i++; 
	tmp = math.getPlaneIntersect(pl6, A, B);
	if (isIntersect(&tmp))
		return i;
	return -1; //this should not happen.
}


//helper function to determine if a line passes through a plane
bool Renderer::isIntersect(const Vertex *A)
{
	if ((*A).x == 0 && (*A).y == 0 && (*A).z == 0)
		return false;
	return true;
}

//returns point of where vertex intersects plane
Vertex Renderer::pointIntersect(Vertex A, Vertex B, const int &frustPlane)
{
	switch (frustPlane)
	{
	case 1:
		return math.getPlaneIntersect(pl1, A, B);
		break;
	case 2:
		return math.getPlaneIntersect(pl2, A, B);
		break;
	case 3:
		return math.getPlaneIntersect(pl3, A, B);
		break;
	case 4:
		return math.getPlaneIntersect(pl4, A, B);
		break;
	case 5:
		return math.getPlaneIntersect(pl5, A, B);
		break;
	case 6:
		return math.getPlaneIntersect(pl6, A, B);
		break;
	default:
		break;
	}
	//Will never get here
	return Vertex((float)0, (float)0, (float)0);
}


Polygon Renderer::clipPoly(Vertex A,  Vertex B,  Vertex C,  Vertex D, const int &planeA, const int &planeB)
{
	Polygon poly;
	Vertex tmp;
	Vertex plane = math.getPlane(A, B, D);
	bool isOppsite = false;
	int line;
	if (planeA == 1)
	{
		if (planeB == 2)
			line = 1;
		else if (planeB == 3)
			line = 2;
		else if (planeB == 4)
			line = 3;
		else if (planeB == 5)
			line = 4;
		else //planeB == 6
		{
			isOppsite = true;
			oppositeSideIntersect(&poly, 1, A, B, C, D);
		}
	}
	else if (planeA == 2)
	{
		if (planeB == 1)
			line = 1;
		else if (planeB == 3)
			line = 6;
		else if (planeB == 4)
		{
			isOppsite = true;
			oppositeSideIntersect(&poly, 2, A, B, C, D);
		}
		else if (planeB == 5)
			line = 5;
		else //else planeB = 6
			line = 10;
	}
	else if (planeA == 3)
	{
		if (planeB == 1)
			line = 2;
		else if (planeB == 2)
			line = 6;
		else if (planeB == 4)
			line = 7;
		else if (planeB == 5)
		{
			isOppsite = true;
			oppositeSideIntersect(&poly, 3, C, D, A, B);
		}
		else //planeB == 6
			line = 11;
	}
	else if (planeA == 4)
	{
		if (planeB == 1)
			line = 3;
		else if (planeB == 2)
		{
			isOppsite = true;
			oppositeSideIntersect(&poly, 2, C, D, A, B);
		}
		else if (planeB == 3)
			line = 7;
		else if (planeB == 5)
			line = 8;
		else //planeB == 6
			line = 12;
	}
	else if (planeA == 5)
	{
		if (planeB == 1)
			line = 4;
		else if (planeB == 2)
			line = 5;
		else if (planeB == 3)
		{
			isOppsite = true;
			oppositeSideIntersect(&poly, 3, A, B, C, D);
		}
		else if (planeB == 4)
			line = 8;
		else //planeB == 6
			line = 9;
	}
	else
	{
		if (planeB == 1)
		{
			isOppsite = true;
			oppositeSideIntersect(&poly, 1, C, D, A, B);
		}
		else if (planeB == 2)
			line = 10;
		else if (planeB == 3)
			line = 11;
		else if (planeB == 4)
			line = 12;
		else //planeB == 5
			line = 9;
	}
	if (!isOppsite)
	{
		tmp = linePlaneIntersect(plane, line);
		//using barycentric coordinates we get the right color to the new point.
		tmp.color = math.colInFace(tmp, A, B, D);
		if (inFrustrum(&tmp))
			poly.vectors.push_back(tmp);
		else
			threePlaneClip(&poly, plane, tmp, &A, &B, &D, line);
	}
	return poly;
}

void Renderer::threePlaneClip(Polygon* poly, const Vertex &plane, const Vertex &point, Vertex *A, Vertex *B, Vertex *C , const int &line)
{
	int L1, L2;
	bool larger;
	if (line == 1 || line == 3 || line == 10 || line == 12)
	{
		if ((point.x / point.z) > xhigh)
			larger = true;
		else //(tmp.x/tmp.z) < xlow
			larger = false;
	}
	else if (line == 2 || line == 4 || line == 11 || line == 9)
	{
		if ((point.y / point.z) > yhigh)
			larger = true;
		else //(tmp.y/tmp.z) < ylow
			larger = false;
	}
	else //line == 5 || line == 6 || line == 7 || line == 8
	{
		if (point.z < hither)
			larger = true;
		else //tmp.z > yon
			larger = false;
	}
	getClipLines(line, true, &L1, &L2);
	Vertex one = linePlaneIntersect(plane, L1);
	Vertex two = linePlaneIntersect(plane, L2);
	//get the color

	one.color = math.colInFace(one, *A, *B, *C);
	two.color = math.colInFace(two, *A, *B, *C);

	(*poly).vectors.push_back(one);
	(*poly).vectors.push_back(two);
}

void Renderer::getClipLines(const int &line, const bool &larger,  int * L1, int *L2)
{
	switch (line)
	{
	case 1:
		if (larger)
			*L1 = 4, *L2 = 5;
		else
			*L1 = 2, *L2 = 6;
		break;
	case 2:
		if (larger)
			*L1 = 1, *L2 = 6;
		else
			*L1 = 3, *L2 = 7;
		break;
	case 3:
		if (larger)
			*L1 = 4, *L2 = 8;
		else
			*L1 = 2, *L2 = 7;
		break;
	case 4:
		if (larger)
			*L1 = 1, *L2 = 5;
		else
			*L1 = 3, *L2 = 8;
		break;
	case 5:
		if (larger)
			*L1 = 9, *L2 = 10;
		else
			*L1 = 1, *L2 = 4;
		break;
	case 6:
		if (larger)
			*L1 = 10, *L2 = 11;
		else
			*L1 = 1, *L2 = 2;
		break;
	case 7:
		if (larger)
			*L1 = 11, *L2 = 12;
		else
			*L1 = 2, *L2 = 3;
		break;
	case 8:
		if (larger)
			*L1 = 9, *L2 = 12;
		else
			*L1 = 3, *L2 = 4;
		break;
	case 9:
		if (larger)
			*L1 = 5, *L2 = 10;
		else
			*L1 = 8, *L2 = 12;
		break;
	case 10:
		if (larger)
			*L1 = 5, *L2 = 9;
		else
			*L1 = 6, *L2 = 1;
		break;
	case 11:
		if (larger)
			*L1 = 6, *L2 = 10;
		else
			*L1 = 7, *L2 = 12;
		break;
	case 12:
		if (larger)
			*L1 = 8, *L2 = 9;
		else
			*L1 = 7, *L2 = 11;
		break;
	}
}

//returns the point where 3 planes intersect
Vertex Renderer::linePlaneIntersect(const Vertex &face, const int &lineOfIntersect)
{
	Vertex A((float)(xlow*hither), (float)(yhigh*hither), (float)hither);
	Vertex B((float)(xhigh*hither), (float)(yhigh*hither), (float)hither);
	Vertex C((float)(xhigh*hither), (float)(ylow*hither), (float)hither);
	Vertex D((float)(xlow*hither), (float)(ylow*hither), (float)hither);
	Vertex E((float)(xlow*yon), (float)(yhigh*yon), (float)yon);
	Vertex F((float)(xhigh*yon), (float)(yhigh*yon), (float)yon);
	Vertex G((float)(xhigh*yon), (float)(ylow*yon), (float)yon);
	Vertex H((float)(xlow*yon), (float)(ylow*yon), (float)yon);
	
	Vertex tmp;
	switch (lineOfIntersect)
	{
	case 1:
		tmp = math.getPlaneIntersect(face, A, B);
		break;
	case 2:
		tmp = math.getPlaneIntersect(face, A, C);
		break;
	case 3:
		tmp = math.getPlaneIntersect(face, C, D);
		break;
	case 4:
		tmp = math.getPlaneIntersect(face, B, D);
		break;
	case 5:
		tmp = math.getPlaneIntersect(face, B, F);
		break;
	case 6:
		tmp = math.getPlaneIntersect(face, A, E);
		break;
	case 7:
		tmp = math.getPlaneIntersect(face, C, H);
		break;
	case 8:
		tmp = math.getPlaneIntersect(face, D, G);
		break;
	case 9:
		tmp = math.getPlaneIntersect(face, F, G);
		break;
	case 10:
		tmp = math.getPlaneIntersect(face, E, F);
		break;
	case 11:
		tmp = math.getPlaneIntersect(face, E, H);
		break;
	case 12:
		tmp = math.getPlaneIntersect(face, H, G);
		break;
	}
	return tmp;
}

//A always either front top or right, B always bak bottom or left, ONLY 3 Cases
void Renderer::oppositeSideIntersect(Polygon *poly, const int &type, const Vertex &A, const Vertex &B, const Vertex &C, const Vertex &D)
{
	//A and B are the same vector
	Vertex plane = math.getPlane(B, C, D);
	int L1, L2;
	if (B.z <= 1)
		return;
	Vertex tmp1, tmp2;
	float x1 = B.x / B.z;
	float y1 = B.y / B.z;
	float z1 = B.z;
	float x2 = D.x / D.z;
	float y2 = D.y / D.z;
	float z2 = D.z;
	switch (type)
	{
	case 1: //either plane 1 & 6 or 6 & 1, either way A always plane 1
		if (x1 >= xlow && x1 <= xhigh && y1 >= yhigh &&  x2 >= xlow && x2 <= xhigh && y2 >= yhigh) //planeline 1 & 10
		{
			L1 = 1;
			L2 = 10;
		}
		else if (x1 >= xlow && x1 <= xhigh && y1 <= ylow && x2 >= xlow && x2 <= xhigh && y2 <= ylow) //planeline 3 & 12
		{
			L1 = 3;
			L2 = 12;
		}
		else if (y1 >= ylow && y1 <= yhigh && x1 <= xlow && y2 >= ylow && y2 <= yhigh && x2 <= xlow) //planeline 2 & 11
		{
			L1 = 2;
			L2 = 11;
		}
		else if (y1 >= ylow && y1 <= yhigh && x1 >= xhigh && y2 >= ylow && y2 <= yhigh && x2 >= xhigh) //planeline 4 & 9
		{
			L1 = 4;
			L2 = 9;
		}
		break;
	case 2: //either plane 2 & 4 or 4 & 2, either way A always plane 2
		if (z1 >= hither && z1 <= yon && y1 >= xhigh && z2 >= hither && z2 <= yon && y2 >= xhigh)
		{
			L1 = 5;
			L2 = 8;
		}
		else if (z1 >= hither && z1 <= yon && y1 <= xlow && z2 >= hither && z2 <= yon && y2 <= xlow)
		{
			L1 = 6;
			L2 = 7;
		}
		else if (z1 < hither && z2 < hither && x1 >= xlow && x1 <= xhigh && x2 >= xlow && x2 <= xhigh)
		{
			L1 = 1;
			L2 = 3;
		}
		else if (z1 > yon && z2 > yon && x1 >= xlow && x1 <= xhigh && x2 >=xlow && x2 <= xhigh)
		{
			L1 = 10;
			L2 = 12;
		}
		break;
	case 3: //either plane 3 & 5 or 5 & 3, either way A always plane 5
		if (z1 >= hither && z1 <= yon && y1 >= yhigh && z2 >= hither && z2 <= yon && y2 >= yhigh)
		{
			L1 = 5;
			L2 = 6;
		}
		else if (z1 >= hither && z1 <= yon && y1 <= ylow && z2 >= hither && z2 <= yon && y2 <= ylow)
		{
			L1 = 5;
			L2 = 6;
		}
		else if (z1 < hither && z2 < hither && y1 >= ylow && y1 <= yhigh && y2 >= ylow && y2 <= yhigh)
		{
			L1 = 2;
			L2 = 4;
		}
		else if (z1 > yon && z2 > yon && y1 >= ylow && y1 <= yhigh && y2 >= ylow && y2 <= yhigh)
		{
			L1 = 9;
			L2 = 11;
		}
		break;
	}
	tmp1 = linePlaneIntersect(plane, L1);
	tmp2 = linePlaneIntersect(plane, L2);
	tmp1.color = math.colInFace(tmp1, B, C, D);
	tmp2.color = math.colInFace(tmp1, B, C, D);
	(*poly).vectors.push_back(tmp1);
	(*poly).vectors.push_back(tmp2);
}

//check if a point is inside of a frustrum
bool Renderer::inFrustrum(Vertex *A)
{
	if ((*A).z < hither || (*A).z > yon)
		return false;
	float xtmp = (*A).x / (*A).z;
	float ytmp = (*A).y / (*A).z;
	if (xtmp < xlow || xtmp > xhigh)
		return false;
	if (ytmp < ylow || ytmp > yhigh)
		return false;
	return true;
}

//this function turns screen space into 650*650 coordinates
void Renderer::convertScreenCoords(Vertex *A)
{
	float scX = (WORLD_X_MAX - WORLD_X_MIN);
	float scY = (WORLD_Y_MAX - WORLD_Y_MIN);

	float deltaX = xhigh - xlow;
	float deltaY = yhigh - ylow;

	float x = ((*A).x - xlow) / (xhigh - xlow); //percent of x
	float y = ((*A).y - ylow) / (yhigh - ylow); //percent of y

	float xPer, yPer;
	if (deltaX == deltaY)
	{
		(*A).x = x * (WORLD_X_MAX - WORLD_X_MIN);
		(*A).y = y * (WORLD_Y_MAX - WORLD_Y_MIN);
	}
	else if (deltaX < deltaY)
	{
		xPer = deltaX / deltaY;
		scX = scX * xPer;
		(*A).x = (x * scX) + (((WORLD_X_MAX - WORLD_X_MIN) - scX) / 2);
		(*A).y = y * (WORLD_Y_MAX - WORLD_Y_MIN);
	}
	else //deltaY < deltaX
	{
		yPer = deltaY / deltaX;
		scY = scY * yPer;
		(*A).x = x * (WORLD_X_MAX - WORLD_X_MIN);
		(*A).y = (y * scY) + (((WORLD_Y_MAX - WORLD_Y_MIN) - scY) / 2);
	}
}

void Renderer::insertWSPoly(Polygon *A)
{
	(*A).p = P;
	(*A).ks = KS;
	if (isWire)
		WSwire.push_back(*A);
	else if (phong)
		WSphong.push_back(*A);
	else if (gouraud)
		WSgouraud.push_back(*A);
	else if (flat)
		WSflat.push_back(*A);
}

void Renderer::insertCSPoly(Polygon *A)
{
	if (isWire)
		CSwire.push_back(*A);
	else if (phong)
		CSphong.push_back(*A);
	else if (gouraud)
		CSgouraud.push_back(*A);
	else if (flat)
		CSflat.push_back(*A);
}

void Renderer::trimWhiteSpace(string& str) 
{
	while (str.compare(0, 1, " ") == 0)
		str.erase(str.begin()); 
	while (str.size()>0 && str.compare(str.size() - 1, 1, " ") == 0)
		str.erase(str.end() - 1);
}

Vertex Renderer::getVec(const int &iterator)
{
	if (iterator < 0)
		return (*vec).at((*vec).size() + iterator);
	return (*vec).at(iterator - 1);
}

Vertex Renderer::getVecNorm(const int const &iterator )
{
	if (iterator < 0)
		return (*vecNorm).at((*vecNorm).size() + iterator);
	return (*vecNorm).at(iterator - 1);
}

int Renderer::parseFront(string str)
{
	string tmp = str.substr(0, str.find("/"));
	return stoi(tmp);
}

int Renderer::parseNormBack(string str)
{
	string tmp = str.substr(str.find("/") + 2, str.length());
	return stoi(tmp);
}

int Renderer::parseBack(string str) 
{
	string tmp = str.substr(str.find_last_of("/") + 1, str.length());
	return stoi(tmp);
}

int Renderer::parseMid(string str)
{
	string tmp = str.substr(str.find_first_of("/") + 1, str.find_last_of("/"));
	return stoi(tmp);
}

string Renderer::parseStr(string str)
{
	for (int i = 0; i < str.length(); i++)
	{
		if (str.at(0) >= '0' && str.at(0) <= '9')
			break;
		else if (str.at(0) >= 'A' && str.at(0) <= 'Z')
			break;
		else if (str.at(0) >= 'a' && str.at(0) <= 'z')
			break;
		else if (str.at(0) == '-')
			break;
		str.erase(0, 1);
	}
	for (int i = (int)(str.length()) - 1; i > 0; i--)
	{
		if (str.at(str.length() - 1) >= '0' && str.at(str.length() - 1) <= '9')
			break;
		else if (str.at(str.length() - 1) >= 'A' && str.at(str.length() - 1) <= 'Z')
			break;
		else if (str.at(str.length() - 1) >= 'a' && str.at(str.length() - 1) <= 'z')
			break;
		str.erase(str.length() - 1, 1);
	}
	return str;
}

