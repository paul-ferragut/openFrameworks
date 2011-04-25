#pragma once

#include "ofConstants.h"
#include "ofUtils.h"
#include "ofColor.h"
//#include "ofPixelUtils.h"



//---------------------------------------
enum ofInterpolationMethod {
	OF_INTERPOLATE_NEAREST_NEIGHBOR =1,
	OF_INTERPOLATE_BILINEAR			=2,
	OF_INTERPOLATE_BICUBIC			=3
};



class ofPixels {

	//friend class ofPixelUtils;

public:

	ofPixels();
	~ofPixels();
	ofPixels(const ofPixels & mom);
	ofPixels& operator=(const ofPixels & mom);

	void allocate(int w, int h, int bitsPerPixel);
	void allocate(int w, int h, ofPixelFormat type);
	void allocate(int w, int h, ofImageType type);
	void set(unsigned char val);
	void setFromPixels(unsigned char * newPixels,int w, int h, ofImageType newType);
	void setFromExternalPixels(unsigned char * newPixels,int w, int h, ofImageType newType);
	void setFromAlignedPixels(unsigned char * newPixels,int w, int h, ofImageType newType, int widthStep);
	void setFromPixels(unsigned char * newPixels,int w, int h, int bitsPerPixel);
	void setFromExternalPixels(unsigned char * newPixels,int w, int h, int bitsPerPixel);
	void setFromAlignedPixels(unsigned char * newPixels,int w, int h, int bitsPerPixel, int widthStep);

	void swapRgb();

	void clear();
	
	unsigned char * getPixels();
	unsigned char * const getPixels() const;

	int getPixelIndex(int x, int y) const;
	ofColor getColor(int x, int y) const;
	void setColor(int x, int y, ofColor color);

	unsigned char& operator[](int pos);

	bool isAllocated() const;

	int getWidth() const;
	int getHeight() const;

	int getBytesPerPixel() const;
	int getBitsPerPixel() const;

	ofImageType getImageType() const;
	int getGlDataType() const;
	
	// functions to operate in-place on pixels
	void crop(int x, int y, int width, int height);
	void rotate90(int nClockwiseRotations);
	void mirror(bool vertically, bool horizontal);
	bool resize(int dstWidth, int dstHeight, ofInterpolationMethod interpMethod=OF_INTERPOLATE_NEAREST_NEIGHBOR);
	
	// not in place
	bool cropFrom(ofPixels &frompix, int x, int y, int width, int height);
	bool resizeFrom(ofPixels& frompix, ofInterpolationMethod interpMethod=OF_INTERPOLATE_NEAREST_NEIGHBOR);

	

private:
	
	
	static float bicubicInterpolate (const int *patch, float x, float y, float x2,float y2, float x3,float y3);
	
	void copyFrom( const ofPixels& mom );
	
	
	unsigned char * pixels;
	int width;
	int height;

	int		bitsPerPixel;		// 8 = gray, 24 = rgb, 32 = rgba
	int		bytesPerPixel;		// 1, 3, 4 bytes per pixels
	GLint	glDataType;			// GL_LUMINANCE, GL_RGB, GL_RGBA
	ofImageType imageType;		// OF_IMAGE_GRAYSCALE, OF_IMAGE_COLOR, OF_IMAGE_COLOR_ALPHA
	bool	bAllocated;
	bool	pixelsOwner;			// if set from external data don't delete it

};

typedef ofPixels& ofPixelsRef;

