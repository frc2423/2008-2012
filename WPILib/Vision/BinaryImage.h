#ifndef BINARYIMAGE_H_
#define BINARYIMAGE_H_

#include "MonoImage.h"
#include "VisionAPI.h"

#include <vector>
#include <algorithm>
using namespace std;

class BinaryImage : public MonoImage
{
public:
	BinaryImage();
	int GetNumberParticles();
	ParticleAnalysisReport GetParticleAnalysisReport(int index);
	vector<ParticleAnalysisReport>* GetOrderedParticleAnalysisReports();
	void write(const char *fileName);
private:
	ParticleAnalysisReport* particleArray;
	int ParticleAnalysis(Image* image, int particleNumber, ParticleAnalysisReport* par);
};
#endif
