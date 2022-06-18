#include "alonebody.h"
#include <clocale>

void Prim::toPly(FILE* f)
{
	fprintf(f,"%d %d %d %d\n",
			AloneFile::palette[color*3],
			AloneFile::palette[color*3+1],
			AloneFile::palette[color*3+2],
			0);
}

void PrimLine::toPly(FILE* f)
{
    fprintf(f,"%d %d %d %d %d %d\n",
			AloneFile::palette[color*3],
			AloneFile::palette[color*3+1],
			AloneFile::palette[color*3+2],
			2, ptA_index, ptB_index);
}

void PrimPoly::toPly(FILE* f)
{
	fprintf(f,"%d %d %d %d ",
			AloneFile::palette[color*3],
			AloneFile::palette[color*3+1],
			AloneFile::palette[color*3+2],
			nbPts);
	for (int i=0;i<nbPts;i++)
	{
		fprintf(f,"%d ",allPt_index[i]);
	}
	fprintf(f,"\n");
}


AloneBody::AloneBody(AloneFile *file) : file(file),modelFlags(0),numOfPoints(0),
	allPoints(nullptr),numOfBones(0),allBones(nullptr),numOfPrim(0),allPrims(nullptr)
{
}

bool AloneBody::load()
{
	printf("Loading file %s\n",file->mPAKFilename);
	u8 * ptr = file->mDecomprData;

	//from renderModel
	modelFlags = READ_LE_S16(ptr);
	ptr+=2;
	ptr+=12;
	ptr+=READ_LE_S16(ptr) + 2;

	//from computeModel
	numOfPoints = *(short int*)ptr;
	ptr+=2;

	ASSERT(numOfPoints<NUM_MAX_POINT_IN_POINT_BUFFER);
	allPoints=new Pt3d[numOfPoints];
	printf("sizeof Pt3d: %lu\n",sizeof(Pt3d));

	memcpy(allPoints,ptr,numOfPoints*3*2);
	ptr+=numOfPoints*3*2;

	if (modelFlags & 0x02)
	{
		numOfBones = *(short int*)ptr;
		ptr+=2;
		ptr+=2*numOfBones;

		/*ASSERT(numOfBones<NUM_MAX_BONES);
		allBones=new Bone[numOfBones];

		memcpy(allBones,ptr,numOfBones*2);
		ptr+=numOfBones*2;*/

		//just skip bones for now
		if (modelFlags & 8)
			ptr+=0x18*numOfBones;
		else
			ptr+=0x10*numOfBones;
		numOfBones = 0;
	}else{
		numOfBones = 0;
	}
	numOfPrim = *(short int*)ptr;
	ptr+=2;
	allPrims=new Prim*[numOfPrim];
	for (int i = 0;i<numOfPrim;i++)
	{
		u8 primType = *(u8*)ptr;
		ptr++;
		if (primType==0) {
			PrimLine * prime = new PrimLine();
			prime->primtype = primType;
			ptr++;
			prime->color = *(u8*)ptr;
			ptr++;
			prime->ptA_index = (*(s16*)ptr)/6;
			ptr+=2;
			prime->ptB_index = (*(s16*)ptr)/6;
			ptr+=2;
			allPrims[i] = prime;
		} else if (primType==1) {
			PrimPoly * prime = new PrimPoly();
			prime->primtype = primType;
			prime->nbPts = *(u8*)ptr;
			ptr++;
			prime->polytype = *(u8*)ptr;
			ptr++;
			prime->color = *(u8*)ptr;
			ptr++;
			prime->allPt_index = new s16[prime->nbPts];
			for (int j = 0;j<prime->nbPts;j++)
			{
				prime->allPt_index[j] = (*(s16*)ptr)/6;
				ptr+=2;
			}
			allPrims[i] = prime;
		} else {
			std::cout<<"Primitive type "<<primType<<"not supported for now, ending export..."<<std::endl;
			numOfPrim=i; //stop for now...
			break;
		}
	}


	char fname[256+9]="toto.ply";
	sprintf(fname,"%s_%05d.ply",file->mPAKFilename,file->mIndex);

	exportPly(fname);
	return true;
}

bool AloneBody::exportPly(char* filename)
{
	setlocale(LC_ALL,"C");
	printf("Export ply into %s\n",filename);
	FILE* fileHandle;
	fileHandle = fopen(filename,"w");
	fprintf(fileHandle,"ply\n");
	fprintf(fileHandle,"format ascii 1.0\n");
	fprintf(fileHandle,"element vertex %d\n",numOfPoints);
	fprintf(fileHandle,"property float32 x\n");
	fprintf(fileHandle,"property float32 y\n");
	fprintf(fileHandle,"property float32 z\n");
	fprintf(fileHandle,"element face %d\n", numOfPrim);
	fprintf(fileHandle,"property uchar red\n");
	fprintf(fileHandle,"property uchar green\n");
	fprintf(fileHandle,"property uchar blue\n");
	fprintf(fileHandle,"property list uchar int vertex_index\n");
	fprintf(fileHandle,"end_header\n");
	for (int i=0;i<numOfPoints;i++)
	{
		fprintf(fileHandle,"%f %f %f\n", allPoints[i].x/1000.0, -allPoints[i].z/1000.0, -allPoints[i].y/1000.0);
	}
	for (int i=0;i<numOfPrim;i++)
	{
		allPrims[i]->toPly(fileHandle);
	}
	fclose(fileHandle);
	return true;
}
