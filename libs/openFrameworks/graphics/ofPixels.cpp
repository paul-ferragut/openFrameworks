#include "ofPixels.h"

ofPixels::ofPixels(){
	bAllocated = false;
	pixels = NULL;
	pixelsOwner = false;
	clear();
}

ofPixels::~ofPixels(){
	clear();
}

ofPixels::ofPixels(const ofPixels & mom){
	copyFrom( mom );
}

ofPixels& ofPixels::operator=(const ofPixels & mom){
	if(&mom==this) return * this;
	copyFrom( mom );
	return *this;
}

void ofPixels::copyFrom(const ofPixels & mom){
	if(mom.isAllocated()){
		allocate(mom.getWidth(),mom.getHeight(),mom.getImageType());
		memcpy(pixels,mom.getPixels(),mom.getWidth()*mom.getHeight()*mom.getBytesPerPixel());
	}
}


static ofImageType getImageTypeFromBits(int bitsPerPixel){
	switch(bitsPerPixel){
	case 8:
		return OF_IMAGE_GRAYSCALE;
		break;
	case 24:
		 return OF_IMAGE_COLOR;
		break;
	case 32:
		 return OF_IMAGE_COLOR_ALPHA;
		break;
	default:
		ofLog(OF_LOG_ERROR,"%i bits per pixel is not a supported image type", bitsPerPixel);
		return OF_IMAGE_UNDEFINED;
	}
}

void ofPixels::allocate(int w, int h, int bitsPerPixel){
	ofImageType type = getImageTypeFromBits(bitsPerPixel);
	allocate(w,h,type);
}

void ofPixels::allocate(int w, int h, ofImageType type){

	if (w < 0 || h < 0) return; 
	
	//we check if we are already allocated at the right size
	if(bAllocated && w==width && h==height && type==imageType){
		return; //we don't need to allocate
	}

	//we do need to allocate, clear the data
	clear();

	imageType = type;
	width= w;
	height = h;
	switch(imageType){
	case OF_IMAGE_GRAYSCALE:
		bytesPerPixel = 1;
		glDataType = GL_LUMINANCE;
		break;
	case OF_IMAGE_COLOR:
		bytesPerPixel = 3;
		glDataType = GL_RGB;
		break;
	case OF_IMAGE_COLOR_ALPHA:
		bytesPerPixel = 4;
		glDataType = GL_RGBA;
		break;
	default:
		ofLog(OF_LOG_ERROR,"ofPixels: format not supported");
		break;
	}

	bitsPerPixel = bytesPerPixel * 8;
	pixels = new unsigned char[w*h*bytesPerPixel];
	memset(pixels, 0, w*h*bytesPerPixel);
	bAllocated = true;
	pixelsOwner = true;

}

void ofPixels::allocate(int w, int h, ofPixelFormat type){

	if (w < 0 || h < 0) return;

	ofImageType imgType;
	switch(type){
	case OF_PIXELS_RGB:
		imgType = OF_IMAGE_COLOR;
		break;
	case OF_PIXELS_RGBA:
	case OF_PIXELS_BGRA:
		imgType = OF_IMAGE_COLOR_ALPHA;
		break;
	case OF_PIXELS_MONO:
		imgType = OF_IMAGE_GRAYSCALE;
		break;
	default:
		ofLog(OF_LOG_ERROR,"ofPixels: format not supported");
		break;

	}
	allocate(w,h,imgType);

}

void ofPixels::set(unsigned char val){
	memset(pixels,val,width*height*bytesPerPixel);
}

void ofPixels::setFromPixels(unsigned char * newPixels,int w, int h, int bitsPerPixel){
	ofImageType type = getImageTypeFromBits(bitsPerPixel);
	setFromPixels(newPixels,w,h,type);
}

void ofPixels::setFromPixels(unsigned char * newPixels,int w, int h, ofImageType newType){
	allocate(w,h,newType);
	memcpy(pixels,newPixels,w*h*bytesPerPixel);
}

void ofPixels::setFromExternalPixels(unsigned char * newPixels,int w, int h, int bitsPerPixel){
	ofImageType type = getImageTypeFromBits(bitsPerPixel);
	setFromExternalPixels(newPixels,w,h,type);
}

void ofPixels::setFromExternalPixels(unsigned char * newPixels,int w, int h, ofImageType newType){
	clear();
	imageType = newType;
	width= w;
	height = h;
	switch(imageType){
	case OF_IMAGE_GRAYSCALE:
		bytesPerPixel = 1;
		glDataType = GL_LUMINANCE;
		break;
	case OF_IMAGE_COLOR:
		bytesPerPixel = 3;
		glDataType = GL_RGB;
		break;
	case OF_IMAGE_COLOR_ALPHA:
		bytesPerPixel = 4;
		glDataType = GL_RGBA;
		break;
	default:
		ofLog(OF_LOG_ERROR, "format not supported");
		break;
	}

	bitsPerPixel = bytesPerPixel * 8;
	pixels = newPixels;
	pixelsOwner = false;
	bAllocated = true;
}

void ofPixels::setFromAlignedPixels(unsigned char * newPixels,int w, int h, int bitsPerPixel, int widthStep){
	ofImageType type = getImageTypeFromBits(bitsPerPixel);
	setFromAlignedPixels(newPixels,w,h,type,widthStep);
}

void ofPixels::setFromAlignedPixels(unsigned char * newPixels,int w, int h, ofImageType newType, int widthStep){
	allocate(w,h,newType);
	if(widthStep==width*bytesPerPixel){
		memcpy(pixels,newPixels,w*h*bytesPerPixel);
	}else{
		for( int i = 0; i < height; i++ ) {
			memcpy( pixels + (i*width*bytesPerPixel),
					newPixels + (i*widthStep),
					width*bytesPerPixel );
		}
	}

}


void ofPixels::swapRgb(){
	if (bitsPerPixel != 8){
		int sizePixels		= width*height;
		int cnt				= 0;
		unsigned char * pixels_ptr = pixels;

		while (cnt < sizePixels){
			std::swap(pixels_ptr[0],pixels_ptr[2]);
			cnt++;
			pixels_ptr+=bytesPerPixel;
		}
	}
}


void ofPixels::clear(){
	
	if(pixels){
		if(pixelsOwner) delete[] pixels;
		pixels = NULL;
	}
	width			= 0;
	height			= 0;
	bytesPerPixel	= 0;
	bitsPerPixel	= 0;
	bAllocated		= false;
	glDataType		= GL_LUMINANCE;
	imageType		= OF_IMAGE_UNDEFINED;
}

unsigned char * ofPixels::getPixels(){
	return pixels;
}

unsigned char * const ofPixels::getPixels() const{
	return pixels;
}

int ofPixels::getPixelIndex(int x, int y) const {
	if( !bAllocated ){
		return 0;
	}else{
		return ( x + y * width ) * bytesPerPixel;
	}
}

ofColor ofPixels::getColor(int x, int y) const {
	ofColor c;
	int index = getPixelIndex(x, y);

	if( bytesPerPixel == 1 ){
		c.set( pixels[index] );
	}else if( bytesPerPixel == 3 ){
		c.set( pixels[index], pixels[index+1], pixels[index+2] );
	}else if( bytesPerPixel == 4 ){
		c.set( pixels[index], pixels[index+1], pixels[index+2], pixels[index+3] );
	}

	return c;
}

void ofPixels::setColor(int x, int y, ofColor color) {
	int index = getPixelIndex(x, y);

	if( bytesPerPixel == 1 ){
		pixels[index] = color.getBrightness();
	}else if( bytesPerPixel == 3 ){
		pixels[index] = color.r;
		pixels[index+1] = color.g;
		pixels[index+2] = color.b;
	}else if( bytesPerPixel == 4 ){
		pixels[index] = color.r;
		pixels[index+1] = color.g;
		pixels[index+2] = color.b;
		pixels[index+3] = color.a;
	}
}

unsigned char & ofPixels::operator[](int pos){
	return pixels[pos];
}

bool ofPixels::isAllocated() const{
	return bAllocated;
}

int ofPixels::getWidth() const{
	return width;
}

int ofPixels::getHeight() const{
	return height;
}

int ofPixels::getBytesPerPixel() const{
	return bytesPerPixel;
}

int ofPixels::getBitsPerPixel() const{
	return bitsPerPixel;
}

ofImageType ofPixels::getImageType() const{
	return imageType;
}

int ofPixels::getGlDataType() const{
	return glDataType;
}



void ofPixels::crop(int x, int y, int _width, int _height){

	if (_width < 0 || _height < 0){
		return;
	}
	
	if (bAllocated == true){
		
		// TODO: check if we need to crop / reallocate
		
		//int width = width;
		//int height = height;
		//int bytesPerPixel = bytesPerPixel;
		
		int newWidth = _width;
		int newHeight = _height;
		unsigned char * newPixels = new unsigned char [newWidth*newHeight*bytesPerPixel];
		memset(newPixels, 0, newWidth*newHeight*bytesPerPixel);
		
		// this prevents having to do a check for bounds in the for loop;
		int minX = MAX(x, 0);
		int maxX = MIN(x+_width, width);
		int minY = MAX(y, 0);
		int maxY = MIN(y+_height, height);
		
		// TODO: point math can help speed this up:
		for (int i = minX; i < maxX; i++){
			for (int j = minY; j < maxY; j++){
				
				int newPixel = (j-y) * newWidth + (i-x);
				int oldPixel = (j) * width + (i);
				
				for (int k = 0; k < bytesPerPixel; k++){
					newPixels[newPixel*bytesPerPixel + k] = pixels[oldPixel*bytesPerPixel + k];
				}
			}
		}
		
		delete [] pixels;
		pixels = newPixels;
		width = newWidth;
		height = newHeight;
	}
	
	
}
void ofPixels::rotate90(int nClockwiseRotations){
	
	if (bAllocated == false){
		return;
	}
	
	// first, figure out which type of rotation we have
	int rotation = nClockwiseRotations;
	while (rotation < 0){
		rotation+=4;
	}
	rotation %= 4;
	
	// if it's 0, do nothing.  if it's 2, do it by a mirror operation.
	if (rotation == 0) {
		return;
		// do nothing!
	} else if (rotation == 2) {
		mirror(true, true);
		return;
	}
	
	// otherwise, we will need to do some new allocaiton.
	//int width = width;
	//int height = height;
	//int bytesPerPixel = bytesPerPixel;
	unsigned char * oldPixels = pixels;
	int newWidth = height;
	int newHeight = width;
	unsigned char * newPixels = new unsigned char [newWidth*newHeight*bytesPerPixel];
	
	if(rotation == 1){
		for (int i = 0; i < width; i++){
			for (int j = 0; j < height; j++){
				
				int pixela = (j*width + i);
				int pixelb = ((i) * newWidth + (height - j - 1));
				for (int k = 0; k < bytesPerPixel; k++){
					newPixels[pixelb*bytesPerPixel + k] = oldPixels[pixela*bytesPerPixel + k];
				}
				
			}
		}
	} else if(rotation == 3){
		for (int i = 0; i < width; i++){
			for (int j = 0; j < height; j++){
				
				int pixela = (j*width + i);
				int pixelb = ((width-i-1) * newWidth + j);
				for (int k = 0; k < bytesPerPixel; k++){
					newPixels[pixelb*bytesPerPixel + k] = oldPixels[pixela*bytesPerPixel + k];
				}
			}
		}
	}
	
	delete [] pixels;
	pixels = newPixels;
	width = newWidth;
	height = newHeight;

}


void ofPixels::mirror(bool vertically, bool horizontal){
	
	
	if (!vertically && !horizontal){
		return;
	}
	
	//int width = width;
	//int height = height;
	//int bytesPerPixel = bytesPerPixel;
	unsigned char * oldPixels = pixels;
	unsigned char tempVal;
	
	if (! (vertically && horizontal)){
		int wToDo = horizontal ? width/2 : width;
		int hToDo = vertically ? height/2 : height;
		
		for (int i = 0; i < wToDo; i++){
			for (int j = 0; j < hToDo; j++){
				
				int pixelb = (vertically ? (height - j - 1) : j) * width + (horizontal ? (width - i - 1) : i);
				int pixela = j*width + i;
				for (int k = 0; k < bytesPerPixel; k++){
					
					tempVal = oldPixels[pixela*bytesPerPixel + k];
					oldPixels[pixela*bytesPerPixel + k] = oldPixels[pixelb*bytesPerPixel + k];
					oldPixels[pixelb*bytesPerPixel + k] = tempVal;
					
				}
			}
		}
	} else {
		// I couldn't think of a good way to do this in place.  I'm sure there is.
		mirror(true, false);
		mirror(false, true);
	}
	
	
}


bool ofPixels::resize(int dstWidth, int dstHeight, ofInterpolationMethod interpMethod){
	
	if ((dstWidth<=0) || (dstHeight<=0) || !(isAllocated())) return false;
	
	ofPixels dstPixels;
	dstPixels.allocate(dstWidth,dstHeight,getImageType());
	
	if(!resizeFrom(dstPixels,interpMethod)) return false;
	
	delete [] pixels;
	pixels = dstPixels.getPixels();
	width  = dstWidth;
	height = dstHeight;
	dstPixels.pixelsOwner = false;
	return true;
	
}


bool ofPixels::cropFrom(ofPixels &frompix, int x, int y, int _width, int _height){
	
	if (!(isAllocated()) || !(frompix.isAllocated()) || getBytesPerPixel() != frompix.getBytesPerPixel()) return false;
	
	int srcWidth      = frompix.getWidth();
	int srcHeight     = frompix.getHeight();
	int dstWidth	  = getWidth();
	int dstHeight	  = getHeight();
	int bytesPerPixel = getBytesPerPixel();
	
	if (_width < 0 || _height < 0){
		return false;
	}
	
	if (frompix.bAllocated == true){
		
		//int newWidth = frompix.width;
		//int newHeight = frompix.height;
		//int bytesPerPixel = frompix.bytesPerPixel;
		
		if ((width != _width) || (height != _height) || (imageType != frompix.imageType)){
			delete [] pixels;
			allocate(_width, _height, frompix.imageType);
		}
		
		int newWidth = _width;
		unsigned char * newPixels = pixels;
		
		// this prevents having to do a check for bounds in the for loop;
		int minX = MAX(x, 0);
		int maxX = MIN(x+_width, width);
		int minY = MAX(y, 0);
		int maxY = MIN(y+_height, height);
		
		// TODO: point math can help speed this up:
		for (int i = minX; i < maxX; i++){
			for (int j = minY; j < maxY; j++){
				
				int newPixel = (j-y) * newWidth + (i-x);
				int oldPixel = (j) * width + (i);
				
				for (int k = 0; k < bytesPerPixel; k++){
					newPixels[newPixel*bytesPerPixel + k] = frompix.pixels[oldPixel*bytesPerPixel + k];
				}
			}
		}
		
		
	}
	
	
}

bool ofPixels::resizeFrom(ofPixels& frompix, ofInterpolationMethod interpMethod){
	
	if (!(isAllocated()) || !(frompix.isAllocated()) || getBytesPerPixel() != frompix.getBytesPerPixel()) return false;
	
	int srcWidth      = frompix.getWidth();
	int srcHeight     = frompix.getHeight();
	int dstWidth	  = getWidth();
	int dstHeight	  = getHeight();
	int bytesPerPixel = getBytesPerPixel();
	
	
	unsigned char* dstPixels = getPixels();
	
	switch (interpMethod){
			
			//----------------------------------------
		case OF_INTERPOLATE_NEAREST_NEIGHBOR:{
			int dstIndex = 0;
			float srcxFactor = (float)srcWidth/dstWidth;
			float srcyFactor = (float)srcHeight/dstHeight;
			float srcy = 0.5;
			for (int dsty=0; dsty<dstHeight; dsty++){
				float srcx = 0.5;
				int srcIndex = int(srcy)*srcWidth;
				for (int dstx=0; dstx<dstWidth; dstx++){
					int pixelIndex = int(srcIndex + srcx) * bytesPerPixel;
					for (int k=0; k<bytesPerPixel; k++){
						dstPixels[dstIndex] = pixels[pixelIndex];
						dstIndex++;
						pixelIndex++;
					}
					srcx+=srcxFactor;
				}
				srcy+=srcyFactor;
			}
		}break;
			
			//----------------------------------------
		case OF_INTERPOLATE_BILINEAR:
			// not implemented yet
			
			//----------------------------------------
		case OF_INTERPOLATE_BICUBIC:
			float px1, py1;
			float px2, py2;
			float px3, py3;
			
			unsigned char srcColor;
			unsigned char interpCol;
			int patchRow;
			int patchIndex;
			int patch[16];
			
			int srcRowBytes = srcWidth*bytesPerPixel;
			int loIndex = (srcRowBytes)+1;
			int hiIndex = (srcWidth*srcHeight*bytesPerPixel)-(srcRowBytes)-1;
			
			for (int dsty=0; dsty<dstHeight; dsty++){
				for (int dstx=0; dstx<dstWidth; dstx++){
					
					int   dstIndex0 = (dsty*dstWidth + dstx) * bytesPerPixel;
					float srcxf = srcWidth  * (float)dstx/(float)dstWidth;
					float srcyf = srcHeight * (float)dsty/(float)dstHeight;
					int   srcx = (int) MIN(srcWidth-1,   srcxf);
					int   srcy = (int) MIN(srcHeight-1,  srcyf);
					int   srcIndex0 = (srcy*srcWidth + srcx) * bytesPerPixel;
					
					px1 = srcxf - srcx;
					py1 = srcyf - srcy;
					px2 = px1 * px1;
					px3 = px2 * px1;
					py2 = py1 * py1;
					py3 = py2 * py1;
					
					for (int k=0; k<bytesPerPixel; k++){
						int   dstIndex = dstIndex0+k;
						int   srcIndex = srcIndex0+k;
						
						for (int dy=0; dy<4; dy++) {
							patchRow = srcIndex + ((dy-1)*srcRowBytes);
							for (int dx=0; dx<4; dx++) {
								patchIndex = patchRow + (dx-1)*bytesPerPixel;
								if ((patchIndex >= loIndex) && (patchIndex < hiIndex)) {
									srcColor = pixels[patchIndex];
								}
								patch[dx*4 + dy] = srcColor;
							}
						}
						
						interpCol = (unsigned char) bicubicInterpolate (patch, px1,py1, px2,py2, px3,py3);
						dstPixels[dstIndex] = interpCol;
					}
					
				}
			}
			break;
	}
	
	return true;
}

//=============================================
float ofPixels::bicubicInterpolate (const int *patch, float x,float y, float x2,float y2, float x3,float y3) {
	// adapted from http://www.paulinternet.nl/?page=bicubic
	// Note that this code can produce values outside of 0...255, due to cubic overshoot.
	// The ofClamp() prevents this from happening.
	
	int p00 = patch[ 0];
	int p10 = patch[ 4];
	int p20 = patch[ 8];
	int p30 = patch[12];
	
	int p01 = patch[ 1];
	int p11 = patch[ 5];
	int p21 = patch[ 9];
	int p31 = patch[13];
	
	int p02 = patch[ 2];
	int p12 = patch[ 6];
	int p22 = patch[10];
	int p32 = patch[14];
	
	int p03 = patch[ 3];
	int p13 = patch[ 7];
	int p23 = patch[11];
	int p33 = patch[15];
	
	int a00 =    p11;
	int a01 =   -p10 +   p12;
	int a02 =  2*p10 - 2*p11 +   p12 -   p13;
	int a03 =   -p10 +   p11 -   p12 +   p13;
	int a10 =   -p01 +   p21;
	int a11 =    p00 -   p02 -   p20 +   p22;
	int a12 = -2*p00 + 2*p01 -   p02 +   p03 + 2*p20 - 2*p21 +   p22 -   p23;
	int a13 =    p00 -   p01 +   p02 -   p03 -   p20 +   p21 -   p22 +   p23;
	int a20 =  2*p01 - 2*p11 +   p21 -   p31;
	int a21 = -2*p00 + 2*p02 + 2*p10 - 2*p12 -   p20 +   p22 +   p30 -   p32;
	int a22 =  4*p00 - 4*p01 + 2*p02 - 2*p03 - 4*p10 + 4*p11 - 2*p12 + 2*p13 + 2*p20 - 2*p21 + p22 - p23 - 2*p30 + 2*p31 - p32 + p33;
	int a23 = -2*p00 + 2*p01 - 2*p02 + 2*p03 + 2*p10 - 2*p11 + 2*p12 - 2*p13 -   p20 +   p21 - p22 + p23 +   p30 -   p31 + p32 - p33;
	int a30 =   -p01 +   p11 -   p21 +   p31;
	int a31 =    p00 -   p02 -   p10 +   p12 +   p20 -   p22 -   p30 +   p32;
	int a32 = -2*p00 + 2*p01 -   p02 +   p03 + 2*p10 - 2*p11 +   p12 -   p13 - 2*p20 + 2*p21 - p22 + p23 + 2*p30 - 2*p31 + p32 - p33;
	int a33 =    p00 -   p01 +   p02 -   p03 -   p10 +   p11 -   p12 +   p13 +   p20 -   p21 + p22 - p23 -   p30 +   p31 - p32 + p33;
	
	float out =
    a00      + a01 * y      + a02 * y2      + a03 * y3 +
    a10 * x  + a11 * x  * y + a12 * x  * y2 + a13 * x  * y3 +
    a20 * x2 + a21 * x2 * y + a22 * x2 * y2 + a23 * x2 * y3 +
    a30 * x3 + a31 * x3 * y + a32 * x3 * y2 + a33 * x3 * y3;
	
	return MIN(255, MAX(out, 0));
}



