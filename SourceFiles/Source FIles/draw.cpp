#include "SourceFiles\Header Files\draw.h"

Draw::Draw(Drawable *drawable, vector<Light> *light) //constructor
{
	this->drawable = drawable;
	this->light = light;
	depthBool = false;
}

void Draw::draw_pixel(const int &x, const int &y, const int &z, unsigned int color)
{
	int screenX = math.screenspaceX(x)- WORLD_X_MIN;
	int screenY = math.screenspaceY(y)- WORLD_Y_MIN;
	if (screenX < 1 || screenX > zBufferX || screenY < 1 || screenY > zBufferY)
		return;
	if (z <= zBuffer[screenX - 1][screenY - 1])
	{
		if (depthBool)
		{
			if (z <= near)
				drawable->setPixel(math.screenspaceX(x), math.screenspaceY(y), color);
			else if (z >= far) 
				drawable->setPixel(math.screenspaceX(x), math.screenspaceY(y), depth.color);
			else
			{
				Vertex A((float)x, (float)y, (float)near, color);
				Vertex B((float)x, (float)y, (float)far, depth);
				Vertex Pt((float)x, (float)y, (float)z);
				Color depthCol = math.betweenTwoPoints(A, B, Pt);
				drawable->setPixel(math.screenspaceX(x), math.screenspaceY(y), depthCol.color);
			}
		}
		else
			drawable->setPixel(math.screenspaceX(x), math.screenspaceY(y), color);
		zBuffer[screenX -1][screenY - 1] = (double)z; //set new z value for zBuffer
	}
}

void Draw::set_zBuffer(const int &sizeX, const int &sizeY)
{
	zBuffer = new double*[sizeY];
	for (int i = 0; i < sizeY; i++)
	{
		zBuffer[i] = new double[sizeX];
	}
	for (int i = 0; i < sizeY; i++)
	{
		for (int j = 0; j < sizeX; j++)
		{
			zBuffer[i][j] = clippingPlane;
		}
	}
}


void Draw::draw_polygon_mesh(const Polygon poly)
{
	//under the assumption that all polys at this point have size 3
	int A = 0, B = 1;
	Vertex one, two;
	int size = (int)poly.vectors.size();
	while (B < size)
	{
		draw_line_DDA(poly.vectors.at(A), poly.vectors.at(B));
		A++;
		B++;
	}
}

void Draw::draw_line_DDA(Vertex AA, Vertex BB)
{
	AA.x += (float)50;
	AA.y = (float)(650 - AA.y + 50);
	BB.x += (float)50;
	BB.y = (float)(650 - BB.y + 50);
	Polygon poly = math.get_poly2(AA,BB);	

	bool X_Y_Interchange = math.get_xy_interchange(&poly);
	float m, y, red, green, blue, z;

	math.modify_points2(&poly);
	math.transpose_points(&poly);

	Vertex A = poly.vectors.front();
	Vertex B = poly.vectors.back();

	red = A.color.red;
	green = A.color.green;
	blue = A.color.blue;
	z = A.z;

	float redInc = ((float)B.color.red - (float)A.color.red) / (B.x - A.x);
	float greenInc = ((float)B.color.green - (float)A.color.green) / (B.x - A.x);
	float blueInc = ((float)B.color.blue - (float)A.color.blue) / (B.x - A.x);
	float zInc = (B.z - A.z) / (B.x - A.x);

	m = (B.y - A.y) / (B.x - A.x);
	y = A.y;
	int x = (int)A.x;

	for (x ; x < (int)(B.x); x++)
	{		
		if (X_Y_Interchange)
			draw_pixel((int)round(y), x, (int)z, math.make_color((int)red, (int)green, (int)blue));
		else
			draw_pixel(x, (int)round(y), (int)z, math.make_color((int)red, (int)green, (int)blue));
		y += m;
		red += redInc;
		green += greenInc;
		blue += blueInc;
		z += zInc;
	}
}

void Draw::draw_line_DDA_phong(Vertex AA, Vertex BB, const float &ks, const float &p)
{
	AA.x += (float)50;
	AA.y = (float)(650 - AA.y + 50);
	BB.x += (float)50;
	BB.y = (float)(650 - BB.y + 50);
	Polygon poly = math.get_poly2(AA, BB);

	float Nx, Ny, Nz;
	float Nmx, Nmy, Nmz;

	float m, y, red, green, blue, z;

	math.modify_points2(&poly);
	math.transpose_points(&poly);

	Vertex A = poly.vectors.front();
	Vertex B = poly.vectors.back();

	red = A.color.red;
	green = A.color.green;
	blue = A.color.blue;
	z = A.z;

	float redInc = ((float)B.color.red - (float)A.color.red) / (B.x - A.x);
	float greenInc = ((float)B.color.green - (float)A.color.green) / (B.x - A.x);
	float blueInc = ((float)B.color.blue - (float)A.color.blue) / (B.x - A.x);
	float zInc = (B.z - A.z) / (B.x - A.x);

	if ((B.x - A.x) == 0)
	{
		Nmx = 0;
		Nmy = 0;
		Nmz = 0;
	}
	else
	{
		Nmx = ((*B.norm).x - (*A.norm).x) / (B.x - A.x);
		Nmy = ((*B.norm).y - (*A.norm).y) / (B.x - A.x);
		Nmz = ((*B.norm).z - (*A.norm).z) / (B.x - A.x);

	}

	Nx = (*A.norm).x;
	Ny = (*A.norm).y;
	Nz = (*A.norm).z;

	if ((B.x - A.x) == 0)
		m = 0;
	else
		m = (B.y - A.y) / (B.x - A.x);
	y = A.y;
	int x = (int)A.x;

	for (x; x < (int)(B.x); x++)
	{

		Vertex CS = getCS((float)x, round(y), (float)z);
		CS.color = Color((short)red, (short)green, (short)blue);
		CS.norm = new Vertex(Nx, Ny, Nz);
		math.normalizeVec(CS.norm);

		getLight(&CS, ks, p);
		draw_pixel(x, (int)round(y), (int)z, CS.color.color);

		y += m;
		red += redInc;
		green += greenInc;
		blue += blueInc;
		z += zInc;

		Nx += Nmx;
		Ny += Nmy;
		Nz += Nmz;
	}
	if (A.x == B.x)
	{
		Vertex CS = getCS((float)x, round(y), (float)z);
		CS.color = Color((short)red, (short)green, (short)blue);
		CS.norm = new Vertex(Nx, Ny, Nz);
		math.normalizeVec(CS.norm);

		getLight(&CS, ks, p);
		draw_pixel(x, (int)round(y), (int)z, CS.color.color);
	}
}

void Draw::draw_polygon_filled(Polygon poly)
{
	int size = (int)poly.vectors.size();
	if (size < 2)
		return;
	else if (size == 2)
		draw_line_DDA(poly.vectors.back(), poly.vectors.front());
	else if (poly.vectors.size() == 3)
		draw_triangle(&poly);

}

void Draw::draw_polygon_filled_phong(Polygon *poly)
{
	int size = (int)(*poly).vectors.size();
	if (size < 2)
		return;
	else if (size == 2)
		draw_line_DDA_phong((*poly).vectors.back(), (*poly).vectors.front(), (*poly).ks, (*poly).p);
	else if ((*poly).vectors.size() == 3)
		draw_triangle_phong(poly);

}

void Draw::draw_triangle_phong(Polygon *poly)
{
	
	bool is_left = math.order_triangle(poly);

	Vertex min = poly->vectors.back();
	min.norm = poly->vectors.back().norm;
	poly->vectors.pop_back();
	Vertex mid = poly->vectors.back();
	mid.norm = poly->vectors.back().norm;
	poly->vectors.pop_back();
	Vertex max = poly->vectors.back();
	max.norm = poly->vectors.back().norm;
	poly->vectors.pop_back();

	int x1, x2;
	int ystart = (int)max.y;
	int yend = (int)min.y;
	float m_left, m_right, b1, b2;

	float left_redInc, left_greenInc, left_blueInc;
	float right_redInc, right_greenInc, right_blueInc;

	float leftNormXinc, leftNormYinc, leftNormZinc;
	float rightNormXinc, rightNormYinc, rightNormZinc;

	float Lr, Lg, Lb, Rr, Rg, Rb;
	float left_Z, right_Z, left_ZInc, right_ZInc;

	Vertex *leftNorm, *rightNorm;

	leftNorm = new Vertex(*max.norm); 
	rightNorm = new Vertex(*max.norm);
	
	Lr = max.color.red;
	Lg = max.color.green;
	Lb = max.color.blue;

	Rr = Lr;
	Rg = Lg;
	Rb = Lb;

	left_Z = max.z;
	right_Z = left_Z;
	if (is_left)
	{
		if (mid.x - max.x == 0)
			m_left = 0;
		else
			m_left = (mid.y - max.y) / (mid.x - max.x);
		if (max.y - mid.y == 0)
			left_ZInc = 0;
		else
			left_ZInc = (mid.z - max.z) / (max.y - mid.y);
		if (min.x - max.x == 0)
			m_right = 0;
		else
			m_right = (min.y - max.y) / (min.x - max.x);
		if (max.y - min.y)
			right_ZInc = 0;
		else
			right_ZInc = (min.z - max.z) / (max.y - min.y);
		b1 = max.y - (m_left * max.x); //left dx/dy
		b2 = max.y - (m_right * max.x); //right dx/dy

		left_redInc = (mid.color.red - max.color.red) / (max.y - mid.y);
		left_greenInc = (mid.color.green - max.color.green) / (max.y - mid.y);
		left_blueInc = (mid.color.blue - max.color.blue) / (max.y - mid.y);

		right_redInc = (min.color.red - max.color.red) / (max.y - min.y);
		right_greenInc = (min.color.green - max.color.green) / (max.y - min.y);
		right_blueInc = (min.color.blue - max.color.blue) / (max.y - min.y);

		leftNormXinc = ((*mid.norm).x - (*max.norm).x) / (max.y - mid.y);
		leftNormYinc = ((*mid.norm).y - (*max.norm).y) / (max.y - mid.y);
		leftNormZinc = ((*mid.norm).z - (*max.norm).z) / (max.y - mid.y);

		rightNormXinc = ((*min.norm).x - (*max.norm).x) / (max.y - min.y);
		rightNormYinc = ((*min.norm).y - (*max.norm).y) / (max.y - min.y);
		rightNormZinc = ((*min.norm).z - (*max.norm).z) / (max.y - min.y);
	}
	else
	{
		if (min.x - max.x == 0)
			m_left = 0;
		else
			m_left = (min.y - max.y) / (min.x - max.x);
		if (max.y - min.y == 0)
			left_ZInc = 0;
		else
			left_ZInc = (min.z - max.z) / (max.y - min.y);
		if (mid.x - max.x == 0)
			m_right = 0;
		else
			m_right = (mid.y - max.y) / (mid.x - max.x);
		if (max.y - mid.y == 0)
			right_ZInc = 0;
		else
			right_ZInc = (mid.z - max.z) / (max.y - mid.y);
		b1 = max.y - (m_left * max.x); //left dx/dy
		b2 = max.y - (m_right * max.x); //right dx/dy

		left_redInc = (min.color.red - max.color.red) / (max.y - min.y);
		left_greenInc = (min.color.green - max.color.green) / (max.y - min.y);
		left_blueInc = (min.color.blue - max.color.blue) / (max.y - min.y);

		right_redInc = (mid.color.red - max.color.red) / (max.y - mid.y);
		right_greenInc = (mid.color.green - max.color.green) / (max.y - mid.y);
		right_blueInc = (mid.color.blue - max.color.blue) / (max.y - mid.y);

		leftNormXinc = ((*min.norm).x - (*max.norm).x) / (max.y - min.y);
		leftNormYinc = ((*min.norm).y - (*max.norm).y) / (max.y - min.y);
		leftNormZinc = ((*min.norm).z - (*max.norm).z) / (max.y - min.y);

		rightNormXinc = ((*mid.norm).x - (*max.norm).x) / (max.y - mid.y);
		rightNormYinc = ((*mid.norm).y - (*max.norm).y) / (max.y - mid.y);
		rightNormZinc = ((*mid.norm).z - (*max.norm).z) / (max.y - mid.y);
	}

	while (ystart > yend)
	{
		if (ystart <= mid.y)
		{
			if (is_left)
			{
				if (min.x - mid.x == 0)
					m_left = 0;
				else
					m_left = (min.y - mid.y) / (min.x - mid.x);
				if (mid.y - min.y == 0)
					left_ZInc = 0;
				else
					left_ZInc = (min.z - mid.z) / (mid.y - min.y);
				b1 = min.y - (m_left * min.x);
				//set left color
				Lr = mid.color.red; Lg = mid.color.green; Lb = mid.color.blue;
				//set norm to mid norm
				leftNorm = mid.norm;

				left_redInc = (min.color.red - mid.color.red) / (mid.y - min.y);
				left_greenInc = (min.color.green - mid.color.green) / (mid.y - min.y);
				left_blueInc = (min.color.blue - mid.color.blue) / (mid.y - min.y);

				leftNormXinc = ((*min.norm).x - (*mid.norm).x) / (mid.y - min.y);
				leftNormYinc = ((*min.norm).y - (*mid.norm).y) / (mid.y - min.y);
				leftNormZinc = ((*min.norm).z - (*mid.norm).z) / (mid.y - min.y);

				left_Z = mid.z;
			}
			else
			{
				if (min.x - mid.x == 0)
					m_right = 0;
				else
					m_right = (min.y - mid.y) / (min.x - mid.x);
				if (mid.y - min.y == 0)
					right_ZInc = 0;
				else
					right_ZInc = (min.z - mid.z) / (mid.y - min.y);
				b2 = min.y - (m_right * min.x); //right dx/dy

				//set right color
				Rr = mid.color.red; Rg = mid.color.green; Rb = mid.color.blue;
				//set norm to right
				rightNorm = mid.norm;
				
				right_redInc = (min.color.red - mid.color.red) / (mid.y - min.y);
				right_greenInc = (min.color.green - mid.color.green) / (mid.y - min.y);
				right_blueInc = (min.color.blue - mid.color.blue) / (mid.y - min.y);

				rightNormXinc = ((*min.norm).x - (*mid.norm).x) / (mid.y - min.y);
				rightNormYinc = ((*min.norm).y - (*mid.norm).y) / (mid.y - min.y);
				rightNormZinc = ((*min.norm).z - (*mid.norm).z) / (mid.y - min.y);

				right_Z = mid.z;
			}
		}
		if (m_left == 0)
		{
			if (is_left)
				x1 = (int)mid.x;
			else
				x1 = (int)min.x;
		}
		else
			x1 = (int)round(((ystart - b1) / m_left));
		if (m_right == 0)
		{
			if (is_left)
				x2 = (int)max.x;
			else
				x2 = (int)mid.x;
		}
		else
			x2 = (int)round((ystart - b2) / m_right);

		Vertex A((float)x1, (float)ystart, left_Z, math.make_color((int)Lr, (int)Lg, (int)Lb));
		Vertex B((float)x2, (float)ystart, right_Z, math.make_color((int)Rr, (int)Rg, (int)Rb));
		A.norm = new Vertex(*leftNorm);
		B.norm = new Vertex(*rightNorm);

		draw_line_DDA_phong(A, B,(*poly).ks, (*poly).p); //draw the line

		Lr += left_redInc; Lg += left_greenInc; Lb += left_blueInc; //increment left color
		Rr += right_redInc; Rg += right_greenInc; Rb += right_blueInc; //increment right color

		left_Z += left_ZInc; right_Z += right_ZInc; //increment left z/ right Z
		
		(*leftNorm).x += leftNormXinc;
		(*leftNorm).y += leftNormYinc;
		(*leftNorm).z += leftNormZinc;
		(*rightNorm).x += rightNormXinc;
		(*rightNorm).y += rightNormYinc;
		(*rightNorm).z += rightNormZinc;

		ystart--;
	}
}

void Draw::draw_triangle(Polygon *poly)
{
	bool is_left = math.order_triangle(poly);

	Vertex min = poly->vectors.back(); 
	poly->vectors.pop_back();
	Vertex mid = poly->vectors.back();
	poly->vectors.pop_back();
	Vertex max = poly->vectors.back();
	poly->vectors.pop_back();

	int x1, x2;
	int ystart = (int)max.y;
	int yend = (int)min.y;
	float m_left, m_right, b1, b2;

	float left_redInc, left_greenInc, left_blueInc;
	float right_redInc, right_greenInc, right_blueInc;
	float Lr, Lg, Lb, Rr, Rg, Rb;
	float left_Z, right_Z, left_ZInc, right_ZInc;

	Lr = max.color.red;
	Lg = max.color.green;
	Lb = max.color.blue;

	Rr = Lr;
	Rg = Lg;
	Rb = Lb;

	left_Z = max.z;
	right_Z = left_Z;
	if (is_left)
	{
		if (mid.x - max.x == 0)
			m_left = 0;
		else			
			m_left = (mid.y - max.y) / (mid.x - max.x);
		if (max.y - mid.y == 0)
			left_ZInc = 0;
		else
			left_ZInc = (mid.z - max.z) / (max.y - mid.y);
		if (min.x - max.x == 0)
			m_right = 0;
		else
			m_right = (min.y - max.y) / (min.x - max.x);
		if (max.y - min.y)
			right_ZInc = 0;
		else
			right_ZInc = (min.z - max.z) / (max.y - min.y);
		b1 = max.y - (m_left * max.x); //left dx/dy
		b2 = max.y - (m_right * max.x); //right dx/dy

		left_redInc = (mid.color.red - max.color.red)/(max.y-mid.y);
		left_greenInc = (mid.color.green - max.color.green) / (max.y - mid.y);
		left_blueInc = (mid.color.blue - max.color.blue) / (max.y - mid.y);

		right_redInc = (min.color.red - max.color.red) / (max.y - min.y);
		right_greenInc = (min.color.green - max.color.green) / (max.y - min.y);
		right_blueInc = (min.color.blue - max.color.blue) / (max.y - min.y);
	}
	else
	{
		if (min.x - max.x == 0)
			m_left = 0;
		else
			m_left = (min.y - max.y) / (min.x - max.x);
		if (max.y - min.y == 0)
			left_ZInc = 0;
		else
			left_ZInc = (min.z - max.z) / (max.y - min.y);
		if (mid.x - max.x == 0)
			m_right = 0;
		else
			m_right = (mid.y - max.y) / (mid.x - max.x);
		if (max.y - mid.y == 0)
			right_ZInc = 0;
		else
			right_ZInc = (mid.z - max.z) / (max.y - mid.y);
		b1 = max.y - (m_left * max.x); //left dx/dy
		b2 = max.y - (m_right * max.x); //right dx/dy

		left_redInc = (min.color.red - max.color.red) / (max.y - min.y);
		left_greenInc = (min.color.green - max.color.green) / (max.y - min.y);
		left_blueInc = (min.color.blue - max.color.blue) / (max.y - min.y);

		right_redInc = (mid.color.red - max.color.red) / (max.y - mid.y);
		right_greenInc = (mid.color.green - max.color.green) / (max.y - mid.y);
		right_blueInc = (mid.color.blue - max.color.blue) / (max.y - mid.y);
	}

	while (ystart > yend)
	{
		if (ystart <= mid.y)
		{
			if (is_left)
			{
				if (min.x - mid.x == 0)
					m_left = 0;
				else
					m_left = (min.y - mid.y) / (min.x - mid.x);
				if (mid.y - min.y == 0)
					left_ZInc = 0;
				else
					left_ZInc = (min.z - mid.z) / (mid.y - min.y);
				b1 = min.y - (m_left * min.x);
				//set left color
				Lr = mid.color.red; Lg = mid.color.green; Lb = mid.color.blue;

				left_redInc = (min.color.red - mid.color.red) / (mid.y - min.y);
				left_greenInc = (min.color.green - mid.color.green) / (mid.y - min.y);
				left_blueInc = (min.color.blue - mid.color.blue) / (mid.y - min.y);

				left_Z = mid.z;
			}
			else
			{
				if (min.x - mid.x == 0)
					m_right = 0;
				else
					m_right = (min.y - mid.y) / (min.x - mid.x);
				if (mid.y - min.y == 0)
					right_ZInc = 0;
				else
					right_ZInc = (min.z - mid.z) / (mid.y - min.y);
				b2 = min.y - (m_right * min.x); //right dx/dy

				//set right color
				Rr = mid.color.red; Rg = mid.color.green; Rb = mid.color.blue;
			
				right_redInc = (min.color.red - mid.color.red) / (mid.y - min.y);
				right_greenInc = (min.color.green - mid.color.green) / (mid.y - min.y);
				right_blueInc = (min.color.blue - mid.color.blue) / (mid.y - min.y);

				right_Z = mid.z;
			}
		}
		if (m_left == 0)
		{
			if (is_left)
				x1 = (int)mid.x;
			else
				x1 = (int)min.x;
		}
		else
			x1 = (int)round(((ystart - b1) / m_left));
		if (m_right == 0)
		{
			if (is_left)
				x2 = (int)max.x;
			else
				x2 = (int)mid.x;
		}
		else
			x2 = (int)round((ystart - b2) / m_right);

		Vertex A((float)x1, (float)ystart, left_Z, math.make_color((int)Lr, (int)Lg, (int)Lb));
		Vertex B((float)x2, (float)ystart, right_Z, math.make_color((int)Rr, (int)Rg, (int)Rb));

		draw_line_DDA(A, B); //draw the line
		Lr += left_redInc; Lg += left_greenInc; Lb += left_blueInc; //increment left color
		Rr += right_redInc; Rg += right_greenInc; Rb += right_blueInc; //increment right color
		left_Z += left_ZInc; right_Z += right_ZInc; //increment left z/ right Z
		ystart--;
	}
}

void Draw::setClippingPlane(const float &clip)
{
	clippingPlane = clip;
	zBufferX = WORLD_X_MAX - WORLD_X_MIN;
	zBufferY = WORLD_Y_MAX - WORLD_Y_MIN;
	set_zBuffer((int)zBufferX, (int)zBufferY);
}

void Draw::setDepth(const float &ne, const float &fa, const Color &dep)
{
	depthBool = true;
	near = ne;
	far = fa;
	depth = dep;
}

void Draw::getLight(Vertex *A, const float &ks, const float &p)
{
	float kr = (float)((float)(*A).color.red / 255);
	float kg = (float)((float)(*A).color.green / 255);
	float kb = (float)((float)(*A).color.blue / 255);

	float Ir = 0.0, Ig = 0.0, Ib = 0.0;

	float fatti, dist, NL, VR, KS;
	Vertex L, *N, V;
	int size = (int)(*light).size();
	for (int i = 0; i < size; i++)
	{
		Light *tmp = &(*light).at(i);

		dist = math.getDist((*A), (*tmp).loc);			//distance between light and point
		fatti = 1 / ((*tmp).A+((*tmp).B*dist));

		L = math.normalizeVec((*A),(*tmp).loc);			//normazlied L
		N = new Vertex(*(*A).norm);						//normalized N

		math.normalizeVec(N);

		V = math.normalizeVec((*A), Vertex(0.0, 0.0, 0.0)); 
		
		NL = math.EucliDotProd((*N), L);
		
		(*N)*(2 * math.EucliDotProd((*N), L));
		(*N).x -= L.x;
		(*N).y -= L.y;
		(*N).z -= L.z;
		VR = math.EucliDotProd((*N), V);
		
		KS = ks*pow(VR, p);								//compute this only once

		if (NL < 0  )
			NL *= -1;
	
		Ir += ((float)(*tmp).red/255)*fatti*(kr*NL + KS);
		Ig += ((float)(*tmp).green/255)*fatti*(kg*NL + KS);
		Ib += ((float)(*tmp).blue/255)*fatti*(kb*NL + KS);
	}
	(*A).color.red += (short)(Ir*255);
	(*A).color.green += (short)(Ig * 255);
	(*A).color.blue += (short)(Ib * 255);
	if ((*A).color.red > 255)
		(*A).color.red = 255;
	if ((*A).color.red < 0)
		(*A).color.red = 0;
	if ((*A).color.green > 255)
		(*A).color.green = 255;
	if ((*A).color.green < 0)
		(*A).color.green = 0;
	if ((*A).color.blue > 255)
		(*A).color.blue = 255;
	if ((*A).color.blue < 0)
		(*A).color.blue = 0;
}

void Draw::setCoodinateConvert(const float &xlo, const float &ylo, const float &xhi, const float &yhi, const float &PlusX, const float &PlusY)
{
	this->xlo = xlo;
	this->ylo = ylo;
	this->xhi = xhi;
	this->yhi = yhi;
	this->PlusX = PlusX;
	this->PlusY = PlusY;
}

Vertex Draw::getCS(const float &xx, const float &yy, const float &zz)
{
	float xRange = 650 - (PlusX * 2);
	float yRange = 650 - (PlusY * 2);

	float x = xx - PlusX;
	float y = yy - PlusY;

	float xPer = x / xRange;
	float yPer = y / yRange;

	float screenX = (xPer*(xhi - xlo)) + xlo;
	float screenY = (yPer*(yhi - ylo)) + xlo;

	return Vertex(screenX*zz, screenY*zz, zz);
}