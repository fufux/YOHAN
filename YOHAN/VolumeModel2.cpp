#include "VolumeModel2.h"

#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <sstream>

using namespace std;

VolumeModel2::VolumeModel2(const char* nodeFile, const char* faceFile, const char* eleFile, DATA speed[], const DATA materialPropriety[])
{
	// id
	modelID = 0;

	material.lambda = materialPropriety[0];
	material.mu = materialPropriety[1];
	material.alpha = materialPropriety[2];
	material.beta = materialPropriety[3];
	material.density = materialPropriety[4];
	material.k1 = 0.0102;
	material.k2 = 0.0252;

	// allocation
	tetPool = new std::vector<struct Tetra>();
	facePool = new std::vector<struct Face>();
	pointPool = new std::vector<struct Point>();

	// read file
	ifstream fpnode(nodeFile, ios::in);
	ifstream fpface(faceFile, ios::in);
	ifstream fpele(eleFile, ios::in);

	if (!fpnode || !fpnode.good())
	{
		printf("Can not open node file: %s", nodeFile);
		fetalError();
	}

	if (!fpface || !fpface.good())
	{
		printf("Can not open face file: %s", faceFile);
		fetalError();
	}

	if (!fpele || !fpele.good())
	{
		printf("Can not open ele file: %s", eleFile);
		fetalError();
	}

	int n;		//number of nodes
	int nf;		//number of faces
	int ne;		//number of tet
	int tmp;	//for those values not important

	/* points */

	fpnode >> n >> tmp >> tmp >> tmp;
	for (int i = 0; i < n; i++)
	{
		struct Point p;
		fpnode >> tmp >> p.originalPosition[0] >> p.originalPosition[1] >> p.originalPosition[2];

		// intial
		p.currentPosition[0] = p.originalPosition[0];
		p.currentPosition[1] = p.originalPosition[1];
		p.currentPosition[2] = p.originalPosition[2];

		// speed
		p.velority[0] = speed[0];
		p.velority[1] = speed[1];
		p.velority[2] = speed[2];

		// acceleration
		p.acceleration[0] = 0;
		p.acceleration[1] = 0;
		p.acceleration[2] = 0;

		// visibility
		p.isVisible = false;

		// tetIndex
		p.tetIndexList = new std::list<IndexTetraPoint>();

		// add into list
		pointPool->push_back(p);
	}

	// end of points
	fpnode.close();
	fpnode.clear();

	
	/* face */
	fpface >> nf >> tmp;
	for (int i = 0; i < nf; i++)
	{
		struct Face f;

		// index of point
		fpface >> tmp >> f.index[0] >> f.index[1] >> f.index[2] >> tmp;

		f.index[0] -= 1;
		f.index[1] -= 1;
		f.index[2] -= 1;

		// visibility of point
		pointPool->at(f.index[0]).isVisible = true;
		pointPool->at(f.index[1]).isVisible = true;
		pointPool->at(f.index[2]).isVisible = true;

		// add into list
		facePool->push_back(f);
	}

	// end of faces
	fpface.close();
	fpface.clear();

	/* Tetraedre */
	fpele >> ne >> tmp >> tmp;
	for (int i = 0; i < ne; i++)
	{
		struct Tetra tet;

		// index of point
		fpele >> tmp >> tet.index[0] >> tet.index[1] >> tet.index[2] >> tet.index[3];

		tet.index[0] -= 1;
		tet.index[1] -= 1;
		tet.index[2] -= 1;
		tet.index[3] -= 1;

		// reverse index in Point
		struct IndexTetraPoint itp;
		itp.tetIndex = i;

		itp.indexOfPoint = 0;
		pointPool->at(tet.index[0]).tetIndexList->push_back(itp);
		itp.indexOfPoint = 1;
		pointPool->at(tet.index[1]).tetIndexList->push_back(itp);
		itp.indexOfPoint = 2;
		pointPool->at(tet.index[2]).tetIndexList->push_back(itp);
		itp.indexOfPoint = 3;
		pointPool->at(tet.index[3]).tetIndexList->push_back(itp);

		// beta
		calcul_Tet_Beta(&tet);

		// add into list
		tetPool->push_back(tet);
	}

	// end of tets
	fpele.close();
	fpele.clear();

}

void VolumeModel2::calcul_Tet_Beta(struct Tetra *tet)
{
	DATA m1x = pointPool->at(tet->index[0]).originalPosition[0];
	DATA m1y = pointPool->at(tet->index[0]).originalPosition[1];
	DATA m1z = pointPool->at(tet->index[0]).originalPosition[2];

	DATA m2x = pointPool->at(tet->index[1]).originalPosition[0];
	DATA m2y = pointPool->at(tet->index[1]).originalPosition[1];
	DATA m2z = pointPool->at(tet->index[1]).originalPosition[2];

	DATA m3x = pointPool->at(tet->index[2]).originalPosition[0];
	DATA m3y = pointPool->at(tet->index[2]).originalPosition[1];
	DATA m3z = pointPool->at(tet->index[2]).originalPosition[2];

	DATA m4x = pointPool->at(tet->index[3]).originalPosition[0];
	DATA m4y = pointPool->at(tet->index[3]).originalPosition[1];
	DATA m4z = pointPool->at(tet->index[3]).originalPosition[2];

	/*
	[ m1x, m2x, m3x, m4x]
	[ m1y, m2y, m3y, m4y]
	[ m1z, m2z, m3z, m4z]
	[   1,   1,   1,   1]
	*/
	DATA det =	m1x*m2y*m3z-m1x*m2y*m4z-m1x*m2z*m3y+m1x*m2z*m4y+
				m1x*m3y*m4z-m1x*m4y*m3z-m1y*m2x*m3z+m1y*m2x*m4z+
				m1y*m2z*m3x-m1y*m2z*m4x-m1y*m3x*m4z+m1y*m4x*m3z+
				m1z*m2x*m3y-m1z*m2x*m4y-m1z*m2y*m3x+m1z*m2y*m4x+
				m1z*m3x*m4y-m1z*m4x*m3y-m2x*m3y*m4z+m2x*m4y*m3z+
				m2y*m3x*m4z-m2y*m4x*m3z-m2z*m3x*m4y+m2z*m4x*m3y;

	tet->beta[0][0] = (m2y*m3z-m2y*m4z-m2z*m3y+m2z*m4y+m3y*m4z-m4y*m3z) / det;
	tet->beta[1][0] = -(m1y*m3z-m1y*m4z-m1z*m3y+m1z*m4y+m3y*m4z-m4y*m3z) / det;
	tet->beta[2][0] = (m1y*m2z-m1y*m4z-m1z*m2y+m1z*m4y+m2y*m4z-m2z*m4y) / det;
	tet->beta[3][0] = -(m1y*m2z-m1y*m3z-m1z*m2y+m1z*m3y+m2y*m3z-m2z*m3y) / det;

	tet->beta[0][1] = -(m2x*m3z-m2x*m4z-m2z*m3x+m2z*m4x+m3x*m4z-m4x*m3z) / det;
	tet->beta[1][1] = (m1x*m3z-m1x*m4z-m1z*m3x+m1z*m4x+m3x*m4z-m4x*m3z) / det;
	tet->beta[2][1] = -(m1x*m2z-m1x*m4z-m1z*m2x+m1z*m4x+m2x*m4z-m2z*m4x) / det;
	tet->beta[3][1] = (m1x*m2z-m1x*m3z-m1z*m2x+m1z*m3x+m2x*m3z-m2z*m3x) / det;

	tet->beta[0][2] = (m2x*m3y-m2x*m4y-m2y*m3x+m2y*m4x+m3x*m4y-m4x*m3y) / det;
	tet->beta[1][2] = -(m1x*m3y-m1x*m4y-m1y*m3x+m1y*m4x+m3x*m4y-m4x*m3y) / det;
	tet->beta[2][2] = (m1x*m2y-m1x*m4y-m1y*m2x+m1y*m4x+m2x*m4y-m2y*m4x) / det;
	tet->beta[3][2] = -(m1x*m2y-m1x*m3y-m1y*m2x+m1y*m3x+m2x*m3y-m2y*m3x) / det;

	tet->beta[0][3] = -(m2x*m3y*m4z-m2x*m4y*m3z-m2y*m3x*m4z+m2y*m4x*m3z+m2z*m3x*m4y-m2z*m4x*m3y) / det;
	tet->beta[1][3] = (m1x*m3y*m4z-m1x*m4y*m3z-m1y*m3x*m4z+m1y*m4x*m3z+m1z*m3x*m4y-m1z*m4x*m3y) / det;
	tet->beta[2][3] = -(m1x*m2y*m4z-m1x*m2z*m4y-m1y*m2x*m4z+m1y*m2z*m4x+m1z*m2x*m4y-m1z*m2y*m4x) / det;
	tet->beta[3][3] = (m1x*m2y*m3z-m1x*m2z*m3y-m1y*m2x*m3z+m1y*m2z*m3x+m1z*m2x*m3y-m1z*m2y*m3x) / det;

	/* Volume */
	if (det < 0)
		tet->vol = -det / 6;
	else
		tet->vol = det / 6;

}

void VolumeModel2::calculateInternalForce()
{
	for (std::vector<struct Tetra>::iterator iter = tetPool->begin(); iter != tetPool->end(); ++iter)
	{
		// P and V
		DATA P[3][4], V[3][4];
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				struct Point point = pointPool->at(iter->index[j]);
				P[i][j] = point.currentPosition[i];
				V[i][j] = point.velority[i];
			}
		}

		/* P * Beta and V * Beta */
		DATA P_Beta[3][4], V_Beta[3][4];
		calcul_M34_M44_Mul(P, iter->beta, P_Beta);
		calcul_M34_M44_Mul(V, iter->beta, V_Beta);

		/* Green's strain tensor AND Strain rate tensor */
		DATA strainTensor[3][3];
		DATA strainRateTensor[3][3];
		
		for (int i = 0; i < 3; ++i)
		{
			// Li
			DATA Li[4] = {0,0,0,0};
			Li[i] = 1;

			// Ri = P * Beta * Li
			DATA Ri[3];
			calcul_M34_V4_Mul(P_Beta, Li, Ri);

			// RVi = V * Beta * Li
			DATA RVi[3];
			calcul_M34_V4_Mul(V_Beta, Li, RVi);

			for (int j = 0; j < 3; ++j)
			{
				// Lj
				DATA Lj[4] = {0,0,0,0};
				Lj[j] = 1;

				// Rj = P * Beta * Lj
				DATA Rj[3];
				calcul_M34_V4_Mul(P_Beta, Lj, Rj);

				// RVj = V * Beta * Lj
				DATA RVj[3];
				calcul_M34_V4_Mul(V_Beta, Lj, RVj);

				/* formula 3.2 */
				strainTensor[i][j] = calcul_V3_V3_Dot(Ri, Rj) - (i == j) ? 1 : 0;

				/* formula 3.3 */
				strainRateTensor[i][j] = calcul_V3_V3_Dot(Ri, RVj) + calcul_V3_V3_Dot(RVi, Rj);
			}
		}

		/* Strain Deviation Tensor */
		DATA strainDevTensor[3][3];
		DATA sum = (strainTensor[0][0] + strainTensor[1][1] + strainTensor[2][2]) / 3;
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				/* Formula 3.4 */
				if (i == j)
					strainDevTensor[i][j] = strainTensor[i][j] - sum;
				else
					strainDevTensor[i][j] = strainTensor[i][j];
			}
		}

		/* Magnitude of Strain Derivation Tensor */
		DATA Je = calcul_Jacobin_M33(strainDevTensor);

		DATA strainPlastic[3][3] = {0};

		if (Je > material.k1)
		{
			DATA deltaStrainPlastic[3][3];

			/* formula 3.7 */
			DATA sqrtJe = sqrt(Je);
			calcul_M33_const_Mul(deltaStrainPlastic, (sqrtJe - material.k1) / sqrtJe);

			/* formula 3.8 */
			calcul_M33_M33_Add(strainPlastic, deltaStrainPlastic, strainPlastic);
			DATA coff = material.k2 / calcul_Jacobin_M33(strainPlastic);
			if (coff < 1)
				calcul_M33_const_Mul(strainPlastic, coff);
		}

		/* Elastic strain tensor */
		DATA strainElastic[3][3];
		calcul_M33_M33_Minus(strainTensor, strainPlastic, strainElastic);

		/* Stress tensor */
		
		/* Elastic stress */
		DATA stressElastic[3][3];

		DATA sumElasticStrain = (strainElastic[0][0] + strainElastic[1][1] + strainElastic[2][2]) * material.lambda;
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				/* formula 3.12 */
				if (i == j)
					stressElastic[i][j] = sumElasticStrain + 2 * material.mu * strainElastic[i][j];
				else
					stressElastic[i][j] = 2 * material.mu * strainElastic[i][j];
			}
		}

		/* Viscous stress */
		DATA stressViscous[3][3];

		DATA sumStrainRate = (strainRateTensor[0][0] + strainRateTensor[1][1] + strainRateTensor[2][2]) * material.alpha;
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				/* formula 3.12 */
				if (i == j)
					stressViscous[i][j] = sumStrainRate + 2 * material.beta * strainRateTensor[i][j];
				else
					stressViscous[i][j] = 2 * material.beta * strainRateTensor[i][j];
			}
		}

		// Stress Tensor = Elastic stress + Viscous stress
		calcul_M33_M33_Add(stressViscous, stressElastic, iter->stress);

		/* internal force */

		for (int i = 0; i < 4; i++)
		{
			DATA fi[3] = {0, 0, 0};
			for (int j = 0; j < 4; j++)
			{
				struct Point point = pointPool->at(iter->index[j]);

				DATA coffPj = 0;
				for (int k = 0; k < 3; k++)
				{
					for (int l = 0; l < 3; l++)
					{
						coffPj += iter->beta[j][l] * iter->beta[i][k] * iter->stress[k][l]; // stressElastic[k][l];
					}
				}

				fi[0] += coffPj * point.currentPosition[0];
				fi[1] += coffPj * point.currentPosition[1];
				fi[2] += coffPj * point.currentPosition[2];
			}

			iter->internalForce[i][0] = -0.5 * iter->vol * fi[0];
			iter->internalForce[i][1] = -0.5 * iter->vol * fi[1];
			iter->internalForce[i][2] = -0.5 * iter->vol * fi[2];
		}
	}

	/* Internal Force of each point */
	for (std::vector<struct Point>::iterator iter = pointPool->begin(); iter != pointPool->end(); ++iter)
	{
		DATA f[3] = {0, 0, 0};
		DATA mass = 0;
		for (std::list<IndexTetraPoint>::iterator jter = iter->tetIndexList->begin(); jter != iter->tetIndexList->end(); ++jter)
		{
			DATA* pf = tetPool->at(jter->tetIndex).internalForce[jter->indexOfPoint];
			
			f[0] += pf[0];
			f[1] += pf[1];
			f[2] += pf[2];

			mass += material.density * tetPool->at(jter->tetIndex).vol;
		}

		iter->mass = mass;

		//force = internal force
		iter->force[0] = f[0];
		iter->force[1] = f[1];
		iter->force[2] = f[2];
	}
}

void VolumeModel2::calculatePosition()
{
	for (std::vector<struct Point>::iterator iter = pointPool->begin(); iter != pointPool->end(); ++iter)
	{
		// acceleration
		iter->acceleration[0] = iter->force[0] / iter->mass + gravity[0];
		iter->acceleration[1] = iter->force[1] / iter->mass + gravity[1];
		iter->acceleration[2] = iter->force[2] / iter->mass + gravity[2];

		// position
		iter->currentPosition[0] += deltaTime * iter->velority[0] + deltaTime * deltaTime * iter->acceleration[0] / 2;
		iter->currentPosition[1] += deltaTime * iter->velority[1] + deltaTime * deltaTime * iter->acceleration[1] / 2;
		iter->currentPosition[2] += deltaTime * iter->velority[2] + deltaTime * deltaTime * iter->acceleration[2] / 2;

		// velority
		iter->velority[0] += deltaTime * iter->acceleration[0];
		iter->velority[1] += deltaTime * iter->acceleration[1];
		iter->velority[2] += deltaTime * iter->acceleration[2];
	}
}

void VolumeModel2::updateGravity(DATA gra[])
{
	this->gravity[0] = gra[0];
	this->gravity[1] = gra[1];
	this->gravity[2] = gra[2];
}

void VolumeModel2::setDeltaTime(DATA dt)
{
	this->deltaTime = dt;
}

void VolumeModel2::fillForceVector(int pointIndex, DATA force[])
{
	pointPool->at(pointIndex).force[0] += force[0];
	pointPool->at(pointIndex).force[1] += force[1];
	pointPool->at(pointIndex).force[2] += force[2];
}

void VolumeModel2::calcul_M34_M44_Mul(DATA (*A)[4], DATA (*B)[4], DATA (*RES)[4])
{
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			RES[i][j] = 0;
			for (int k = 0; k < 4; ++k)
				RES[i][j] += A[i][k] * B[k][j];
		}
	}
}

void VolumeModel2::calcul_M34_V4_Mul(DATA (*A)[4], DATA* B, DATA* RES)
{
	for (int i = 0; i < 3; i++)
	{
		RES[i] = 0;
		for (int j = 0; j < 4; j++)
			RES[i] += A[i][j] * B[j];
	}
}

DATA VolumeModel2::calcul_V3_V3_Dot(DATA* A, DATA* B)
{
	DATA res = 0;
	for (int i = 0; i < 3; i++)
	{
		res += A[i] * B[i];
	}

	return res;
}

DATA VolumeModel2::calcul_Jacobin_M33(DATA (*A)[3])
{
	DATA Jac = 0;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			Jac += A[i][j] * A[i][j];
		}
	}
	return Jac;
}

void VolumeModel2::calcul_M33_M33_Add(DATA (*A)[3], DATA (*B)[3], DATA (*RES)[3])
{
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			RES[i][j] = A[i][j] + B[i][j];
		}
	}
}

void VolumeModel2::calcul_M33_M33_Minus(DATA (*A)[3], DATA (*B)[3], DATA (*RES)[3])
{
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			RES[i][j] = A[i][j] - B[i][j];
		}
	}
}

void VolumeModel2::calcul_M33_const_Mul(DATA (*A)[3], DATA b)
{
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			A[i][j] *= b;
		}
	}
}


// temporal
void VolumeModel2::fillForceList(std::list<SceneForce> *fl)
{
	for (std::vector<struct Tetra>::iterator iter = tetPool->begin(); iter != tetPool->end(); ++iter)
	{
		for (int i = 0; i < 4; i++)
		{
			int pindex = iter->index[i];
			struct Point p = pointPool->at(pindex);

			if (p.currentPosition[1] < 0)	// y < 0
			{
				SceneForce sf;

				sf.vmeshID = 0;	// only one object in the scene
				sf.vertexID = pindex;

				//F = m * -v / deltaTime 
				//+ penality (assume that z = v * dt + 0.5 * (F/m-g) * dt2, F = ((z - v * t) / 0.5 / dt2 + g) * m)
				
				DATA m = p.mass;
				//DATA penalityForce = ((-pinfo[2] - pinfo[5] * deltaTime) * 2 / (deltaTime * deltaTime) + 9.8) * m * 10 ;	// coeffient
				
				sf.intensity[0] = m * 9.8 * -p.velority[0] * 0.3;
				sf.intensity[1] = m * -p.velority[1] / deltaTime / 2;
				sf.intensity[2] = m * 9.8 * -p.velority[2] * 0.3;

				fl->push_back(sf);		

			}
		}
	}
}

void VolumeModel2::output(FILE* sceneFile, char* objectFileDir, int frameID)
{
	// file name
	char nodeFileName[256], faceFileName[256], eleFileName[256];
	std::stringstream sstream, sstream1, sstream2;

	sstream << objectFileDir << "/" << modelID << ".bnode";
	strcpy_s(nodeFileName, 256, sstream.str().c_str());
	sstream.clear();

	sstream1 << objectFileDir << "/" << modelID << ".bface";
	strcpy_s(faceFileName, 256, sstream1.str().c_str());

	sstream1.clear();
	sstream2 << objectFileDir << "/" << modelID << ".bele";
	strcpy_s(eleFileName, 256, sstream2.str().c_str());

	sstream2.clear();

	// output
	// node
	ofstream fp(nodeFileName, ios::out | ios::binary);
	int size = pointPool->size();
	fp.write ((char*)&size, sizeof(int));

	for (std::vector<struct Point>::iterator iter = pointPool->begin(); iter != pointPool->end(); ++iter)
	{
		fp.write ((char*)iter->currentPosition, sizeof(DATA) * 3);
	}
	fp.flush();
	fp.close();
	fp.clear();

	// face
	if (facePool->size() != oldFaceCount)
	{
		oldFaceCount = facePool->size();
		strcpy_s(oldFaceFileName, 256, faceFileName);

		fp.open(faceFileName, ios::out | ios::binary);
		fp.write ((char*)&oldFaceCount, sizeof(int));
		for (std::vector<struct Face>::iterator iter = facePool->begin(); iter != facePool->end(); ++iter)
		{
			fp.write((char*)iter->index, sizeof(int) * 3);
		}
		fp.flush();
		fp.close();
		fp.clear();
	}

	// tet
	if (tetPool->size() != oldTetCount)
	{
		oldTetCount = tetPool->size();
		strcpy_s(oldTetFileName, 256, eleFileName);

		fp.open(eleFileName, ios::out | ios::binary);
		fp.write ((char*)&oldTetCount, sizeof(int));
		for (std::vector<Tetra>::iterator iter = tetPool->begin(); iter != tetPool->end(); ++iter)
		{
			fp.write ((char*)iter->index, sizeof(int) * 4);
		}
		fp.flush();
		fp.close();
		fp.clear();
	}

	fprintf(sceneFile, "\t\t<object nodefile=\"%s\" facefile=\"%s\" elefile=\"%s\" />\n", nodeFileName, oldFaceFileName, oldTetFileName);
}
