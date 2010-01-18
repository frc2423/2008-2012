#include "BinaryImage.h"
#include "VisionAPI.h"
#include "Utility.h"

//Normalizes to [-1,1]
double NormalizeFromRange(double position, int range){
	return(((position*2.0)/(double)range)-1.0);
}

BinaryImage::BinaryImage() : MonoImage()
{
}

/**
 * Get then number of particles for the image.
 * @returns the number of particles found for the image.
 */
int BinaryImage::GetNumberParticles()
{
	int numParticles = 0;
	int success = imaqCountParticles(image, 1, &numParticles);
	wpi_imaqAssert(success, "IMAQ Error counting particles");
	return numParticles;
}

/**
 * Get a single particle analysis report.
 * Get one (of possibly many) particle analysis reports for an image.
 * @param index which particle analysis report to return.
 * @returns the selected particle analysis report
 */
ParticleAnalysisReport BinaryImage::GetParticleAnalysisReport(int index)
{
	ParticleAnalysisReport par;
	ParticleAnalysis(image, index, &par);
	return par;
}


/**
 * The compare helper function for sort.
 * This function compares two particle analysis reports as a helper for the sort function.
 * @param particle1 The first particle to compare
 * @param particle2 the second particle to compare
 * @returns true if particle1 is greater than particle2
 */
bool compareParticleSizes(ParticleAnalysisReport particle1, ParticleAnalysisReport particle2)
{
	return particle1.particleToImagePercent > particle2.particleToImagePercent; //we want descending sort order
}

/**
 * Get an ordered vector of particles for the image.
 * Create a vector of particle analysis reports sorted by size for an image.
 * The vector contains the actual report structures.
 * @returns a pointer to the vector of particle analysis reports. The caller must delete the
 * vector when finished using it.
 */
vector<ParticleAnalysisReport>* BinaryImage::GetOrderedParticleAnalysisReports()
{
	vector<ParticleAnalysisReport>* particles = new vector<ParticleAnalysisReport>;
	int particleCount = GetNumberParticles();
	for(int particleIndex = 0; particleIndex < particleCount; particleIndex++)
	{
		particles->push_back(GetParticleAnalysisReport(particleIndex));
	}
	sort(particles->begin(), particles->end(), compareParticleSizes);
	return particles;
}

/**
 * Write a binary image to flash.
 * Writes the binary image to flash on the cRIO for later inspection.
 * @param fileName the name of the image file written to the flash.
 */
void BinaryImage::write(const char *fileName)
{
	RGBValue colorTable[256];
	Priv_SetWriteFileAllowed(1);
	memset(colorTable, 0, sizeof(colorTable));
	colorTable[0].R = 0;
	colorTable[1].R = 255;
	colorTable[0].G = colorTable[1].G = 0;
	colorTable[0].B = colorTable[1].B = 0;
	colorTable[0].alpha = colorTable[1].alpha = 0;
	imaqWriteFile(image, fileName, colorTable);
	return;
}

/**
 * Measure a single parameter for an image.
 * Get the measurement for a single parameter about an image by calling the imaqMeasureParticle
 * function for the selected parameter.
 * @param image the image to measure
 * @param particleNumber which particle in the set of particles
 * @param whatToMeasure the imaq MeasurementType (what to measure)
 * @returns the value of the measurement
 */
double particleMeasurement(Image *image, int particleNumber, MeasurementType whatToMeasure)
{
	double returnDouble;
	int success;
	success = imaqMeasureParticle(image, particleNumber, 0, whatToMeasure, &returnDouble);
	wpi_imaqAssert(success, "Error measuring particle");
	if ( !success )
		return 0.0;
	else
		return returnDouble;
}

/**
 * Fill in the particle analysis report.
 * For a given image, fill in the particle analysis report structure.
 * @param imageParam the image to use
 * @param particleNumber the particle index in the set of particles for the image
 * @param par the particle analysis report to be filled in
 */
int BinaryImage::ParticleAnalysis(Image* imageParam, int particleNumber, ParticleAnalysisReport* par)
{
	int success = 0;

	success = imaqGetImageSize(imageParam, &par->imageWidth, &par->imageHeight);
	wpi_imaqAssert(success, "Error getting image size");
	
	par->particleIndex = particleNumber;		
	
	success = imaqCountParticles(imageParam, TRUE, NULL);
	wpi_imaqAssert(success, "Error counting particles");
	
	par->center_mass_x = (int) particleMeasurement(imageParam, particleNumber, IMAQ_MT_CENTER_OF_MASS_X);
	par->center_mass_y = (int) particleMeasurement(imageParam, particleNumber, IMAQ_MT_CENTER_OF_MASS_Y);
	par->particleArea = (int) particleMeasurement(imageParam, particleNumber, IMAQ_MT_AREA);	
	par->boundingRect.top = (int) particleMeasurement(imageParam, particleNumber, IMAQ_MT_BOUNDING_RECT_TOP);
	par->boundingRect.left = (int) particleMeasurement(imageParam, particleNumber, IMAQ_MT_BOUNDING_RECT_LEFT);
	par->boundingRect.height = (int) particleMeasurement(imageParam, particleNumber, IMAQ_MT_BOUNDING_RECT_HEIGHT);
	par->boundingRect.width = (int) particleMeasurement(imageParam, particleNumber, IMAQ_MT_BOUNDING_RECT_WIDTH);
	par->particleToImagePercent = particleMeasurement(imageParam, particleNumber, IMAQ_MT_AREA_BY_IMAGE_AREA);
	par->particleQuality = particleMeasurement(imageParam, particleNumber, IMAQ_MT_AREA_BY_PARTICLE_AND_HOLES_AREA);

	/* normalized position (-1 to 1) */
	par->center_mass_x_normalized = NormalizeFromRange(par->center_mass_x, par->imageWidth);
	par->center_mass_y_normalized = NormalizeFromRange(par->center_mass_y, par->imageHeight);

	return success;
}
