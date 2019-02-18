#include "mathlib.h"

Polygon Mathlib::get_poly2(const Vertex &A, const Vertex &B)
{
	Polygon poly;
	poly.vectors.insert(poly.vectors.begin(), B);
	poly.vectors.insert(poly.vectors.begin(), A);
	return poly;
}

Polygon Mathlib::get_poly3(const Vertex &A, const Vertex &B, const Vertex &C)
{
	Polygon poly;
	poly.vectors.insert(poly.vectors.begin(), C);
	poly.vectors.insert(poly.vectors.begin(), B);
	poly.vectors.insert(poly.vectors.begin(), A);
	return poly;
}

void Mathlib::scale(const CTM *A, const double &sx, const double &sy, const double &sz)
{
	CTM *tmp = new CTM;
	tmp->ctm[0][0] *= sx;
	tmp->ctm[1][1] *= sy;
	tmp->ctm[2][2] *= sz;
	multiplyCTM(A, tmp);
	delete tmp;
}

void Mathlib::rotate(const CTM *A, const int &axis, const double &angle)
{
	CTM *tmp = new CTM;
	double Cos = cos(angle * PI / 180.0);
	double Sin = sin(angle * PI / 180.0);
	switch (axis)
	{
	case 1: //x axis
		tmp->ctm[1][1] = Cos;
		tmp->ctm[1][2] = Sin;
		tmp->ctm[2][1] = -Sin;
		tmp->ctm[2][2] = Cos;
		break;
	case 2: //y axis
		tmp->ctm[0][0] = Cos;
		tmp->ctm[0][2] = -Sin;
		tmp->ctm[2][0] = Sin;
		tmp->ctm[2][2] = Cos;
		break;
	case 3: //z axis
		tmp->ctm[0][0] = Cos;
		tmp->ctm[0][1] = Sin;
		tmp->ctm[1][0] = -Sin;
		tmp->ctm[1][1] = Cos;
		break;
	default:
		break;
	}
	multiplyCTM(A, tmp);
	delete tmp;
}


void Mathlib::translate(const CTM *A, const double &tx, const double &ty, const double &tz)
{
	CTM *tmp = new CTM;
	tmp->ctm[0][3] += tx;
	tmp->ctm[1][3] += ty;
	tmp->ctm[2][3] += tz;
	multiplyCTM(A, tmp);
	delete tmp;
}

void Mathlib::multiplyCTM(const CTM *A, const CTM* B)
{
	CTM *tmp = new CTM;
	tmp->ctm[0][0] = A->ctm[0][0] * B->ctm[0][0] + A->ctm[0][1] * B->ctm[1][0] + A->ctm[0][2] * B->ctm[2][0] + A->ctm[0][3] * B->ctm[3][0];
	tmp->ctm[0][1] = A->ctm[0][0] * B->ctm[0][1] + A->ctm[0][1] * B->ctm[1][1] + A->ctm[0][2] * B->ctm[2][1] + A->ctm[0][3] * B->ctm[3][1];
	tmp->ctm[0][2] = A->ctm[0][0] * B->ctm[0][2] + A->ctm[0][1] * B->ctm[1][2] + A->ctm[0][2] * B->ctm[2][2] + A->ctm[0][3] * B->ctm[3][2];
	tmp->ctm[0][3] = A->ctm[0][0] * B->ctm[0][3] + A->ctm[0][1] * B->ctm[1][3] + A->ctm[0][2] * B->ctm[2][3] + A->ctm[0][3] * B->ctm[3][3];

	tmp->ctm[1][0] = A->ctm[1][0] * B->ctm[0][0] + A->ctm[1][1] * B->ctm[1][0] + A->ctm[1][2] * B->ctm[2][0] + A->ctm[1][3] * B->ctm[3][0];
	tmp->ctm[1][1] = A->ctm[1][0] * B->ctm[0][1] + A->ctm[1][1] * B->ctm[1][1] + A->ctm[1][2] * B->ctm[2][1] + A->ctm[1][3] * B->ctm[3][1];
	tmp->ctm[1][2] = A->ctm[1][0] * B->ctm[0][2] + A->ctm[1][1] * B->ctm[1][2] + A->ctm[1][2] * B->ctm[2][2] + A->ctm[1][3] * B->ctm[3][2];
	tmp->ctm[1][3] = A->ctm[1][0] * B->ctm[0][3] + A->ctm[1][1] * B->ctm[1][3] + A->ctm[1][2] * B->ctm[2][3] + A->ctm[1][3] * B->ctm[3][3];

	tmp->ctm[2][0] = A->ctm[2][0] * B->ctm[0][0] + A->ctm[2][1] * B->ctm[1][0] + A->ctm[2][2] * B->ctm[2][0] + A->ctm[2][3] * B->ctm[3][0];
	tmp->ctm[2][1] = A->ctm[2][0] * B->ctm[0][1] + A->ctm[2][1] * B->ctm[1][1] + A->ctm[2][2] * B->ctm[2][1] + A->ctm[2][3] * B->ctm[3][1];
	tmp->ctm[2][2] = A->ctm[2][0] * B->ctm[0][2] + A->ctm[2][1] * B->ctm[1][2] + A->ctm[2][2] * B->ctm[2][2] + A->ctm[2][3] * B->ctm[3][2];
	tmp->ctm[2][3] = A->ctm[2][0] * B->ctm[0][3] + A->ctm[2][1] * B->ctm[1][3] + A->ctm[2][2] * B->ctm[2][3] + A->ctm[2][3] * B->ctm[3][3];

	tmp->ctm[3][0] = A->ctm[3][0] * B->ctm[0][0] + A->ctm[3][1] * B->ctm[1][0] + A->ctm[3][2] * B->ctm[2][0] + A->ctm[3][3] * B->ctm[3][0];
	tmp->ctm[3][1] = A->ctm[3][0] * B->ctm[0][1] + A->ctm[3][1] * B->ctm[1][1] + A->ctm[3][2] * B->ctm[2][1] + A->ctm[3][3] * B->ctm[3][1];
	tmp->ctm[3][2] = A->ctm[3][0] * B->ctm[0][2] + A->ctm[3][1] * B->ctm[1][2] + A->ctm[3][2] * B->ctm[2][2] + A->ctm[3][3] * B->ctm[3][2];
	tmp->ctm[3][3] = A->ctm[3][0] * B->ctm[0][3] + A->ctm[3][1] * B->ctm[1][3] + A->ctm[3][2] * B->ctm[2][3] + A->ctm[3][3] * B->ctm[3][3];
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
			A->ctm[i][j] = tmp->ctm[i][j];
	}
	delete tmp;
}

CTM* Mathlib::inverseCTM(const CTM* A)
{
	CTM* tmp = new CTM;
	double det;
	tmp->ctm[0][0] = getDeterminant(A->ctm[1][1], A->ctm[1][2], A->ctm[1][3], A->ctm[2][1], A->ctm[2][2], A->ctm[2][3], A->ctm[3][1], A->ctm[3][2], A->ctm[3][3]);
	tmp->ctm[0][1] = -getDeterminant(A->ctm[1][0], A->ctm[1][2], A->ctm[1][3], A->ctm[2][0], A->ctm[2][2], A->ctm[2][3], A->ctm[3][0], A->ctm[3][2], A->ctm[3][3]);
	tmp->ctm[0][2] = getDeterminant(A->ctm[1][0], A->ctm[1][1], A->ctm[1][3], A->ctm[2][0], A->ctm[2][1], A->ctm[2][3], A->ctm[3][0], A->ctm[3][1], A->ctm[3][3]);
	tmp->ctm[0][3] = -getDeterminant(A->ctm[1][0], A->ctm[1][1], A->ctm[1][2], A->ctm[2][0], A->ctm[2][1], A->ctm[2][2], A->ctm[3][0], A->ctm[3][1], A->ctm[3][2]);

	tmp->ctm[1][0] = -getDeterminant(A->ctm[0][1], A->ctm[0][2], A->ctm[0][3], A->ctm[2][1], A->ctm[2][2], A->ctm[2][3], A->ctm[3][1], A->ctm[3][2], A->ctm[3][3]);
	tmp->ctm[1][1] = getDeterminant(A->ctm[0][0], A->ctm[0][2], A->ctm[0][3], A->ctm[2][0], A->ctm[2][2], A->ctm[2][3], A->ctm[3][0], A->ctm[3][2], A->ctm[3][3]);
	tmp->ctm[1][2] = -getDeterminant(A->ctm[0][0], A->ctm[0][1], A->ctm[0][3], A->ctm[2][0], A->ctm[2][1], A->ctm[2][3], A->ctm[3][0], A->ctm[3][1], A->ctm[3][3]);
	tmp->ctm[1][3] = getDeterminant(A->ctm[0][0], A->ctm[0][1], A->ctm[0][2], A->ctm[2][0], A->ctm[2][1], A->ctm[2][2], A->ctm[3][0], A->ctm[3][1], A->ctm[3][2]);

	tmp->ctm[2][0] = getDeterminant(A->ctm[0][1], A->ctm[0][2], A->ctm[0][3], A->ctm[1][1], A->ctm[1][2], A->ctm[1][3], A->ctm[3][1], A->ctm[3][2], A->ctm[3][3]);
	tmp->ctm[2][1] = -getDeterminant(A->ctm[0][0], A->ctm[0][2], A->ctm[0][3], A->ctm[1][0], A->ctm[1][2], A->ctm[1][3], A->ctm[3][0], A->ctm[3][2], A->ctm[3][3]);
	tmp->ctm[2][2] = getDeterminant(A->ctm[0][0], A->ctm[0][1], A->ctm[0][3], A->ctm[1][0], A->ctm[1][1], A->ctm[1][3], A->ctm[3][0], A->ctm[3][1], A->ctm[3][3]);
	tmp->ctm[2][3] = -getDeterminant(A->ctm[0][0], A->ctm[0][1], A->ctm[0][2], A->ctm[1][0], A->ctm[1][1], A->ctm[1][2], A->ctm[3][0], A->ctm[3][1], A->ctm[3][2]);

	tmp->ctm[3][0] = -getDeterminant(A->ctm[0][1], A->ctm[0][2], A->ctm[0][3], A->ctm[1][1], A->ctm[1][2], A->ctm[1][3], A->ctm[2][1], A->ctm[2][2], A->ctm[2][3]);
	tmp->ctm[3][1] = getDeterminant(A->ctm[0][0], A->ctm[0][2], A->ctm[0][3], A->ctm[1][0], A->ctm[1][2], A->ctm[1][3], A->ctm[2][0], A->ctm[2][2], A->ctm[2][3]);
	tmp->ctm[3][2] = -getDeterminant(A->ctm[0][0], A->ctm[0][1], A->ctm[0][3], A->ctm[1][0], A->ctm[1][1], A->ctm[1][3], A->ctm[2][0], A->ctm[2][1], A->ctm[2][3]);
	tmp->ctm[3][3] = getDeterminant(A->ctm[0][0], A->ctm[0][1], A->ctm[0][2], A->ctm[1][0], A->ctm[1][1], A->ctm[1][2], A->ctm[2][0], A->ctm[2][1], A->ctm[2][2]);

	det = tmp->ctm[0][0] * A->ctm[0][0] + tmp->ctm[0][1] * A->ctm[0][1] + tmp->ctm[0][2] * A->ctm[0][2] + tmp->ctm[0][3] * A->ctm[0][3];
	transposeCTM(tmp);
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			tmp->ctm[i][j] = tmp->ctm[i][j] / det;
		}
	}
	return tmp;
}

void Mathlib::copyCTM(const CTM* A, const CTM* B)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			A->ctm[i][j] = B->ctm[i][j];
		}
	}
}

double Mathlib::getDeterminant(const double &a, const double &b, const double &c, const double &d, const double &e, const double &f, const double &g, const double &h, const double &i)
{
	return (a*e*i + b*f*g + c*d*h) - (g*e*c + h*f*a + i*d*b);
}

void Mathlib::transposeCTM(const CTM* A)
{
	double tmp;
	for (int i = 0; i < 4; i++)
	{
		for (int j = i; j < 4; j++)
		{
			if (i == j)
				continue;
			else
			{
				tmp = A->ctm[i][j];
				A->ctm[i][j] = A->ctm[j][i];
				A->ctm[j][i] = tmp;
			}
		}
	}
}

void Mathlib::CTM_Vertex3_multiply(const CTM *A, Vertex *B)
{
	double a = (double)(*B).x;
	double b = (double)(*B).y;
	double c = (double)(*B).z;
	double d = (double)(*B).w;
	(*B).x = (float)(A->ctm[0][0] * a + A->ctm[0][1] * b + A->ctm[0][2] * c + A->ctm[0][3] * d);
	(*B).y = (float)(A->ctm[1][0] * a + A->ctm[1][1] * b + A->ctm[1][2] * c + A->ctm[1][3] * d);
	(*B).z = (float)(A->ctm[2][0] * a + A->ctm[2][1] * b + A->ctm[2][2] * c + A->ctm[2][3] * d);
	(*B).w = (float)(A->ctm[3][0] * a + A->ctm[3][1] * b + A->ctm[3][2] * c + A->ctm[3][3] * d);
}

void Mathlib::PolyMatrixMult(const CTM *mat, Polygon *poly)
{
	//going into this function we know that there is only 3 vectors in this poly
	int vecSize = (int)(*poly).vectors.size();
	for (int i = 0; i < vecSize; i++)
	{
		CTM_Vertex3_multiply(mat, &(*poly).vectors.at(i));
		if ((*poly).vectors.at(i).norm != NULL)
			CTM_Vertex3_multiply(mat, (*poly).vectors.at(i).norm);
	}
	 
}

//must take input of 3 vectors
void Mathlib::assignNorms(Polygon *poly)
{
	Vertex normal = getNormalizedNormal((*poly).vectors.at(0), (*poly).vectors.at(1), (*poly).vectors.at(2));
	int i = 0, size = (int)(*poly).vectors.size();
	while (i < size)
	{
		if ((*poly).vectors.at(i).norm == NULL)
			(*poly).vectors.at(i).norm = new Vertex(normal);
		normalizeVec((*poly).vectors.at(i).norm); //if normal exists, normalize it
		i++;
	}
}

void Mathlib::assignNormsPhong(Polygon *poly)
{
	Vertex normal = getNormalizedNormal((*poly).vectors.at(0), (*poly).vectors.at(1), (*poly).vectors.at(2));
	int i = 0, size = (int)(*poly).vectors.size();
	while (i < size)
	{
		if ((*poly).vectors.at(i).norm == NULL)
			(*poly).vectors.at(i).norm = &normal;
		i++;
	}
}

Vertex Mathlib::getCenter(const Polygon* poly)
{
	Vertex A = (*poly).vectors.at(0);
	Vertex B = (*poly).vectors.at(1);
	Vertex C = (*poly).vectors.at(2);

	return Vertex((float)((A.x + B.x + C.x) / 3), (float)((A.y + B.y + C.y) / 3), (float)((A.z + B.z + C.z) / 3));
}

Vertex Mathlib::averageNorm(const Polygon *poly)
{
	Vertex *A = (*poly).vectors.at(0).norm;
	Vertex *B = (*poly).vectors.at(1).norm;
	Vertex *C = (*poly).vectors.at(2).norm;
	return Vertex((float)(((*A).x + (*B).x + (*C).x) / 3), (float)(((*A).y + (*B).y + (*C).y) / 3), (float)(((*A).z + (*B).z + (*C).z) / 3));
}

Color Mathlib::averageColor(const Polygon *poly)
{
	Vertex A = (*poly).vectors.at(0);
	Vertex B = (*poly).vectors.at(1);
	Vertex C = (*poly).vectors.at(2);

	float red = (A.color.red + B.color.red + C.color.red) / 3;
	float green = (A.color.green + B.color.green + C.color.green) / 3;
	float blue = (A.color.blue + B.color.blue + C.color.blue) / 3;
	return Color((short)red, (short)green, (short)blue);
}

Vertex Mathlib::getNormalizedNormal(const Vertex &A, const Vertex &B, const Vertex &C)
{
	Vertex V(B.x - A.x, B.y - A.y, B.z - A.z);
	Vertex W(C.x - A.x, C.y - A.y, C.z - A.z);

	Vertex tmp((V.y*W.z) - (V.z*W.y), (V.z*W.x) - (V.x*W.z), (V.x*W.y) - (V.y*W.x)); //cross multiple vectors

	float N = sqrt(tmp.x*tmp.x) + sqrt(tmp.y*tmp.y) + sqrt(tmp.z*tmp.z); 
	tmp.x = tmp.x / N;
	tmp.y = tmp.y / N;
	tmp.z = tmp.z / N;
	return tmp;
}

void Mathlib::normalizeVec(Vertex *A)
{
	float N = sqrt(A->x*A->x) + sqrt(A->y*A->y) + sqrt(A->z*A->z);
	float Epsilon = 0.9999999;
	if (N < Epsilon || N > 1) //test againts epsilon
	{
		A->x = A->x / N;
		if (abs(A->x) < 0.0000000001)
			A->x = 0;
		A->y = A->y / N;
		if (abs(A->y) < 0.0000000001)
			A->y = 0;
		A->z = A->z / N;
		if (abs(A->z) < 0.0000000001)
			A->z = 0;
	}
}

//calculate planes based on 3 vertex points
Vertex Mathlib::getPlane(const Vertex &A, const Vertex &B, const Vertex &C)
{
	Vertex U(A.x - B.x, A.y - B.y, A.z - B.z);
	Vertex V(C.x - B.x, C.y - B.y, C.z - B.z);

	Vertex tmp((U.y*V.z) - (U.z*V.y), (U.z*V.x) - (U.x*V.z), (U.x*V.y) - (U.y*V.x)); //cross multiple vectors
	tmp.w = tmp.x *A.x + tmp.y*A.y + tmp.z*A.z;
	return tmp;
}

//return the point where a line passes through a plane
Vertex Mathlib::getPlaneIntersect(const Vertex &plane, Vertex &A, const Vertex &B)
{
	Vertex diff(B.x - A.x, B.y - A.y, B.z - A.z);
	float T = diff.x*plane.x + diff.y*plane.y + diff.z*plane.z;
	float notT = plane.w - (A.x*plane.x + A.y*plane.y + A.z*plane.z);
	T = notT / T;
	if (T < 0 || T > 1)
		return Vertex((float)0, (float)0, (float)0); //does not intersect the line
	A.x += (T*diff.x);
	A.y += (T*diff.y);
	A.z += (T*diff.z);
	return A;
}

Color Mathlib::colInFace(const Vertex &point, const Vertex &A, const Vertex &B, const Vertex &C)
{
	Vertex AB(B.x - A.x, B.y - A.y, B.z - A.z);
	Vertex AC(C.x - A.x, C.y - A.y, C.z - A.z);
	Vertex BC(C.x - B.x, C.y - B.y, C.z - B.z);

	Vertex AP(point.x - A.x, point.y - A.y, point.z - A.z);
	Vertex BP(point.x - B.x, point.y - B.y, point.z - B.z);
	Vertex CP(point.x - C.x, point.y - C.y, point.z - C.z);

	float ABlen = sqrt((AB.x*AB.x) + (AB.y*AB.y) + (AB.z*AB.z));
	float AClen = sqrt((AC.x*AC.x) + (AC.y*AC.y) + (AC.z*AC.z));
	float BClen = sqrt((BC.x*BC.x) + (BC.y*BC.y) + (BC.z*BC.z));

	float APlen = sqrt((AP.x*AP.x) + (AP.y*AP.y) + (AP.z*AP.z));
	float BPlen = sqrt((BP.x*BP.x) + (BP.y*BP.y) + (BP.z*BP.z));
	float CPlen = sqrt((CP.x*CP.x) + (CP.y*CP.y) + (CP.z*CP.z));

	float Ap = (ABlen + AClen + BClen) / 2;
	float A1p = (ABlen + APlen + BPlen) / 2;
	float A2p = (BPlen + CPlen + BClen) / 2;
	float A3p = (APlen + CPlen + AClen) / 2;

	float AreaA = sqrt(Ap*(Ap - ABlen)*(Ap - AClen)*(Ap - BClen));
	float AreaA1 = sqrt(A1p*(A1p - ABlen)*(A1p - APlen)*(A1p - BPlen));
	float AreaA2 = sqrt(A2p*(A2p - BPlen)*(A2p - CPlen)*(A2p - BClen));
	float AreaA3 = sqrt(A3p*(A3p - APlen)*(A3p - CPlen)*(A3p - AClen));

	float A1percent = AreaA1 / AreaA;
	float A2percent = AreaA2 / AreaA;
	float A3percent = AreaA3 / AreaA;

	Color AA((short)(A2percent*A.color.red), (short)(A2percent*A.color.green), (short)(A2percent*A.color.blue));
	Color BB((short)(A3percent*B.color.red), (short)(A3percent*B.color.green), (short)(A3percent*B.color.blue));
	Color CC((short)(A1percent*C.color.red), (short)(A1percent*C.color.green), (short)(A1percent*C.color.blue));

	return Color(AA.red + BB.red + CC.red, AA.green + BB.green + CC.green, AA.blue + BB.blue + CC.blue);
}

Vertex* Mathlib::nomalInFace(const Vertex &point, const Vertex &A, const Vertex &B, const Vertex &C)
{
	if (A.norm == NULL && B.norm == NULL && C.norm == NULL)
		return NULL;
	if (A.norm != NULL && B.norm != NULL && C.norm != NULL)
	{
		Vertex AB(B.x - A.x, B.y - A.y, B.z - A.z);
		Vertex AC(C.x - A.x, C.y - A.y, C.z - A.z);
		Vertex BC(C.x - B.x, C.y - B.y, C.z - B.z);

		Vertex AP(point.x - A.x, point.y - A.y, point.z - A.z);
		Vertex BP(point.x - B.x, point.y - B.y, point.z - B.z);
		Vertex CP(point.x - C.x, point.y - C.y, point.z - C.z);

		float ABlen = sqrt((AB.x*AB.x) + (AB.y*AB.y) + (AB.z*AB.z));
		float AClen = sqrt((AC.x*AC.x) + (AC.y*AC.y) + (AC.z*AC.z));
		float BClen = sqrt((BC.x*BC.x) + (BC.y*BC.y) + (BC.z*BC.z));

		float APlen = sqrt((AP.x*AP.x) + (AP.y*AP.y) + (AP.z*AP.z));
		float BPlen = sqrt((BP.x*BP.x) + (BP.y*BP.y) + (BP.z*BP.z));
		float CPlen = sqrt((CP.x*CP.x) + (CP.y*CP.y) + (CP.z*CP.z));

		float Ap = (ABlen + AClen + BClen) / 2;
		float A1p = (ABlen + APlen + BPlen) / 2;
		float A2p = (BPlen + CPlen + BClen) / 2;
		float A3p = (APlen + CPlen + AClen) / 2;

		float AreaA = sqrt(Ap*(Ap - ABlen)*(Ap - AClen)*(Ap - BClen));
		float AreaA1 = sqrt(A1p*(A1p - ABlen)*(A1p - APlen)*(A1p - BPlen));
		float AreaA2 = sqrt(A2p*(A2p - BPlen)*(A2p - CPlen)*(A2p - BClen));
		float AreaA3 = sqrt(A3p*(A3p - APlen)*(A3p - CPlen)*(A3p - AClen));

		float A1percent = AreaA1 / AreaA;
		float A2percent = AreaA2 / AreaA;
		float A3percent = AreaA3 / AreaA;

		Vertex AA(A2percent*A.x, A2percent*A.y, A2percent*A.z, A2percent*A.w);
		Vertex BB(A3percent*B.x, A3percent*B.y, A3percent*B.z, A2percent*A.w);
		Vertex CC(A1percent*C.x, A1percent*C.y, A1percent*C.z, A2percent*A.w);

		return &Vertex(AA.x + BB.x + CC.x, AA.y + BB.y + CC.y, AA.z + BB.z + CC.z, AA.w + BB.w + CC.w);
	}
	else
		return NULL;
}


Color Mathlib::betweenTwoPoints(const Vertex &A, const Vertex &B, const Vertex &Pt)
{	
	//from A -> B
	float xABdif = B.x - A.x;
	float yABdif = B.y - A.y;
	float zABdif = B.z - A.z;
	
	float ABDist = sqrt((xABdif*xABdif) + (yABdif*yABdif) + (zABdif*zABdif));

	float xAPtdif = Pt.x - A.x;
	float yAPtdif = Pt.y - A.y;
	float zAPtdif = Pt.z - A.z;

	float ACDist = sqrt((xAPtdif*xAPtdif) + (yAPtdif*yAPtdif) + (zAPtdif*zAPtdif));

	float Aper = ACDist / ABDist;
	float Bper = 1 - Aper;

	return Color((short)((B.color.red*Aper) + (A.color.red*Bper)), (short)((B.color.green*Aper) + (A.color.green*Bper)), (short)((B.color.blue*Aper) + (A.color.blue*Bper)));
}

Vertex* Mathlib::normBetweenTwoPoints(const Vertex &A, const Vertex &B, const Vertex &Pt)
{
	if (A.norm == NULL && B.norm == NULL)
		return NULL;
	else if (A.norm == NULL)
		return B.norm;
	else if (B.norm == NULL)
		return A.norm;
	//neither is null

	float xABdif = B.x - A.x;
	float yABdif = B.y - A.y;
	float zABdif = B.z - A.z;
	float ABDist = sqrt((xABdif*xABdif) + (yABdif*yABdif) + (zABdif*zABdif));
	float xAPtdif = Pt.x - A.x;
	float yAPtdif = Pt.y - A.y;
	float zAPtdif = Pt.z - A.z;
	float ACDist = sqrt((xAPtdif*xAPtdif) + (yAPtdif*yAPtdif) + (zAPtdif*zAPtdif));
	float Aper = ACDist / ABDist;
	float Bper = 1 - Aper;
	return &Vertex(((B.x*Aper) + (A.x*Bper)), ((B.y*Aper) + (A.y*Bper)), ((B.z*Aper) + (A.z*Bper)), ((B.w*Aper) + (A.w*Bper)));
}


float Mathlib::EucliDotProd(const Vertex &A, const Vertex &B)
{
	return (A.x*B.x) + (A.y*B.y) + (A.z*B.z);
}

//From A --> B
Vertex Mathlib::getVector(const Vertex &A, const Vertex &B)
{
	return Vertex(B.x - A.x, B.y - A.y, B.z - A.z);
}

float Mathlib::getDist(const Vertex &A, const Vertex &B)
{
	return sqrt(((A.x - B.x)*(A.x - B.x)) + ((A.y - B.y)*(A.y - B.y)) + ((A.z - B.z)*(A.z - B.z)));
}

Vertex Mathlib::normalizeVec(const Vertex &A, const Vertex &B)
{
	Vertex tmp = getVector(A, B);
	float abs = sqrt(tmp.x*tmp.x) + sqrt(tmp.y*tmp.y) + sqrt(tmp.z*tmp.z);
	tmp.x = tmp.x / abs;
	tmp.y = tmp.y/  abs;
	tmp.z = tmp.z / abs;
	return tmp;
}

void Mathlib::transpose_points(Polygon *poly)
{
	Vertex A = poly->vectors.front();
	Vertex B = poly->vectors.back();
	poly->vectors.clear();
	if (abs(B.y-A.y) > abs(B.x-A.x))
	{
		swap(A.x, A.y);
		swap(B.x, B.y);
	}
	if (A.x > B.x)
	{
		swap(B.x, A.x);
		swap(B.y, A.y);
		Color tmp(A.color);
		A.color = B.color;
		B.color = tmp;
	}
	poly->vectors.insert(poly->vectors.begin(), B);
	poly->vectors.insert(poly->vectors.begin(), A);
}


void Mathlib::modify_points2(Polygon *poly)
{
	Vertex A = poly->vectors.front();
	Vertex B = poly->vectors.back();
	poly->vectors.clear();
	
	A.x = A.x - WORLD_X_MIN;
	A.y = WORLD_Y_MAX - A.y;
	B.x = B.x - WORLD_X_MIN;
	B.y = WORLD_Y_MAX - B.y;
	
	poly->vectors.insert(poly->vectors.begin(), B);
	poly->vectors.insert(poly->vectors.begin(), A);
}

bool Mathlib::order_triangle(Polygon *poly)
{
	Vertex C = poly->vectors.back();
	C.norm = new Vertex((*poly->vectors.back().norm));
	poly->vectors.pop_back();
	Vertex B = poly->vectors.back();
	B.norm = new Vertex((*poly->vectors.back().norm));
	poly->vectors.pop_back();
	Vertex A = poly->vectors.back();
	A.norm = new Vertex((*poly->vectors.back().norm));
	poly->vectors.pop_back();
	float max_x, max_y, mid_x, mid_y, min_x, min_y, max_z, mid_z, min_z;
	Color maxColor, midColor, minColor;
	bool is_left;
	Vertex maxNorm, midNorm, minNorm;
	if (A.y >= B.y && A.y >= C.y)
	{
		max_x = A.x;
		max_y = A.y;
		max_z = A.z;
		maxNorm = new Vertex(*A.norm);
		maxColor = A.color;
		if (B.y < C.y)
		{
			min_x = B.x;
			min_y = B.y;
			min_z = B.z;
			minNorm = new Vertex(*B.norm);
			minColor = B.color;
			mid_x = C.x;
			mid_y = C.y;
			mid_z = C.z;
			midNorm = new Vertex(*C.norm);
			midColor = C.color;
			if (C.x < B.x)
				is_left = true;
			else
				is_left = false;
		}
		else
		{
			min_x = C.x;
			min_y = C.y;
			min_z = C.z;
			minNorm = new Vertex(*C.norm);
			minColor = C.color;
			mid_x = B.x;
			mid_y = B.y;
			mid_z = B.z;
			midNorm = new Vertex(*B.norm);
			midColor = B.color;
			if (B.x < C.x || B.x >= C.x && B.x < A.x)
				is_left = true;
			else
				is_left = false;
		}
	}
	else if (B.y > A.y && B.y > C.y)
	{
		max_x = B.x;
		max_y = B.y;
		max_z = B.z;
		maxNorm = new Vertex(*B.norm);
		maxColor = B.color;
		if (A.y < C.y)
		{
			min_x = A.x;
			min_y = A.y;
			min_z = A.z;
			minNorm = new Vertex(*A.norm);
			minColor = A.color;
			mid_x = C.x;
			mid_y = C.y;
			mid_z = C.z;
			midNorm = new Vertex(*C.norm);
			midColor = C.color;
			if (C.x < A.x)
				is_left = true;
			else
				is_left = false;
		}
		else
		{
			min_x = C.x;
			min_y = C.y;
			min_z = C.z;
			minNorm = new Vertex(*C.norm);
			minColor = C.color;
			mid_x = A.x;
			mid_y = A.y;
			mid_z = A.z;
			midNorm = new Vertex(*A.norm);
			midColor = A.color;
			if (A.x < C.x || A.x >= C.x && A.x < B.x)
				is_left = true;
			else
				is_left = false;
		}
	}
	else
	{
		max_x = C.x;
		max_y = C.y;
		max_z = C.z;
		maxNorm = new Vertex(*C.norm);
		maxColor = C.color;
		if (A.y < B.y)
		{
			min_x = A.x;
			min_y = A.y;
			min_z = A.z;
			minNorm = new Vertex(*A.norm);
			minColor = A.color;
			mid_x = B.x;
			mid_y = B.y;
			mid_z = B.z;
			midNorm = new Vertex(*B.norm);
			midColor = B.color;
			if (B.x < A.x || B.x >= A.x && B.x < C.x)
				is_left = true;
			else
				is_left = false;
		}
		else
		{
			min_x = B.x;
			min_y = B.y;
			min_z = B.z;
			minNorm = new Vertex(*B.norm);
			minColor = B.color;
			mid_x = A.x;
			mid_y = A.y;
			mid_z = A.z;
			midNorm = new Vertex(*A.norm);
			midColor = A.color;
			if (A.x < B.x)
				is_left = true;
			else
				is_left = false;
		}
	}
	Vertex maximum(max_x, max_y, max_z, maxColor);
	maximum.norm = new Vertex(maxNorm);
	Vertex middle(mid_x, mid_y, mid_z, midColor);
	middle.norm = new Vertex(midNorm);
	Vertex minimum(min_x, min_y, min_z, minColor);
	minimum.norm = new Vertex(minNorm);
	poly->vectors.push_back(maximum); //first vectore is largest
	poly->vectors.push_back(middle); //then mid
	poly->vectors.push_back(minimum); //then smallest
	return is_left;
}

unsigned int Mathlib::make_color(const int &red, const int &green, const int &blue)
{
	return (0xff << 24) + ((red & 0xff) << 16) + ((green & 0xff) << 8) + (blue & 0xff);
}

bool Mathlib::get_xy_interchange(Polygon *poly)
{
	Vertex A = poly->vectors.front();
	Vertex B = poly->vectors.back();
	return abs(B.y - A.y) > abs(B.x - A.x);
}

int Mathlib::screenspaceX(const int &x)
{
	return x + WORLD_X_MIN;
}

int Mathlib::screenspaceY(const int &y)
{	
	return WORLD_Y_MAX - y;
}
