#ifndef HEXGEN_H
#define HEXGEN_H

#include <vector>
#include <algorithm>
#include <unordered_set>
#include "CellQueue.h"
#include "StaticVars.h"
#include "Mesh.h"
// structures

struct octCell
{
	bool refineFlag = false;
	bool intersect = false;
};

// general functions
// distance between two vertices
inline double dist(double v[3], double v2[3]);
inline double dist(double x, double y, double z, double v2[3]);

// triangle-triangle intersection detection 0: not intersect 1: intersect
 inline int Coplanar(double p1[3], double q1[3], double r1[3], double p2[3], double q2[3], double r2[3], double N1[3], double N2[3]);
 inline int CounterClockwiseIntersection2D(double p1[2], double q1[2], double r1[2], double p2[2], double q2[2], double r2[2]);
 inline int Intersect2D(double p1[2], double q1[2], double r1[2], double p2[2], double q2[2], double r2[2]);
 inline int Intersect(double p1[3], double q1[3], double r1[3], double p2[3], double q2[3], double r2[3]);

// check if point p intersects with triangle abc in direction dir 0: not intersect 1: intersect e: intersect point
inline double TriArea(double a, double b, double c);
// -1: need another dir; 0: not intersect; 1: intersect
inline int Intersect(double a[3], double b[3], double c[3], double p[3], double dir[3], double* e, double& alpha);
// distance between a point and a triangle in 3d space
inline double PointToTri(double a[3], double b[3], double c[3], double p[3], double* q, double currMin);

// scaled jacobian
inline double Sj(double p0[3], double p1[3], double p2[3], double p3[3], double p4[3], double p5[3], double p6[3], double p7[3]);

// negative scaled jacobian index
// if all the nine Sj > ELEM_THRES, return -1
// otherwise return the number of point that has the lowest Sj
inline void iSj(double p0[3], double p1[3], double p2[3], double p3[3], double p4[3], double p5[3], double p6[3], double p7[3], int* minIdx);

// hexGen
class hexGen
{
public:
	// constructor
	hexGen(int depth);

	// destructor
	~hexGen(void);

	// initialize the octree data structure
	void InitializeOctree(const char* inputFileName, const char* outputFileName);

	// construct the octree data structure
	void ConstructOctree();

	// print generated octree to a file
	void OutputOctree(const char* fileName);

	// read octree from a file
	void ReadOctree(const char* inputFileName);

	// extract dual full hexahedral mesh
	void DualFullHexMeshExtraction(const char* fileName);

	// read dual full hex from a file
	void ReadDualFullHex(const char* inputFileName);

	// remove hex with at least one corner isovalue < 0
	void RemoveOutsideElement(const char* fileName);

	// read dual hex from a file
	void ReadDualHex(const char* inputFileName);

	// filt surface faces and project surface faces to isosurface
	void ProjectToIsoSurface(const char* fileName);

private:
	int voxelSize;// to the power of 2
	int octreeDepth;
	int numVoxels;// total number of voxels
	double BOX_LENGTH, START_POINT[3], BOX_LENGTH_RATIO;// box length and start point determined by triangular mesh

	int leafNum = 0;// number of leaves/elements
	std::vector<octCell> octreeArray;// restore if the octree is already refined
	std::vector<int> cutArray;// restore the leaf octree cell index
	std::vector<int> cutArray1;// used in octree balancing rule

	Mesh triMesh;// triangular structure information
	// different levels triangle number
	std::vector<int> refineTri0, refineTri1, refineTri2, refineTri3, refineTri4;
	std::vector<int> refineTriPt0, refineTriPt1, refineTriPt2, refineTriPt3, refineTriPt4;

	Mesh octreeMesh;// octree structure information

	Mesh hexMesh;
	int(*elementValence)[6][4], (*elementValenceNumber)[6];// element valence

	// here octreeMesh becomes useless, so we use it again to save memory space

	// read raw file and write to origData
	inline void ReadRawData(const char* inputFileName, const char* outputFileName);

	// compute if an octree cell intersects with a surface triangle and local radius
	inline void GetCellValue();

	// compute cell intersect information
	inline void ComputeCellValue(int octreeId, int level, bool& intersect);

	// convert octree index to xyz coordinate
	inline void OctreeidxToXYZ(int octreeId, int& x, int& y, int& z, int level);

	// refine brother octree cells
	inline void RefineBrothers(int octreeId, int level, int* octreeIdx);

	// given local, level-related XYZ, gain the octree cell index
	inline int XYZToOctreeidx(int x, int y, int z, int level);

	// given a parent octree cell, gain its eight children cells
	inline int Child(int octreeId, int level, int i);

	// get strongly balanced octree
	inline void StrongBalancedOctree();

	// check if an octree is shared by eight other octree cells
	inline int IsSharedByEightCells(int x, int y, int z, int level, std::vector<int>& vecEightCell);

	// octreeCellNum = 2^0 + 2^3 + 2^6 + 2^9 + 2^12 + ... + 2^(3 * octreeDepth)
	inline int GetLevel(int octreeId);

	// initiate element valences in the octree structure
	inline void InitiateElementValence();
};
#endif HEXGEN_H