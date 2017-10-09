#include "image.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <cmath>
#include <iomanip>
#include <iostream>

using namespace std;

/**
 * Image
 **/
Image::Image (int width_, int height_){

    assert(width_ > 0);
    assert(height_ > 0);

    width           = width_;
    height          = height_;
    num_pixels      = width * height;
    sampling_method = IMAGE_SAMPLING_POINT;
    
    data.raw = new uint8_t[num_pixels*4];
	int b = 0; //which byte to write to
	for (int j = 0; j < height; j++){
		for (int i = 0; i < width; i++){
			data.raw[b++] = 0;
			data.raw[b++] = 0;
			data.raw[b++] = 0;
			data.raw[b++] = 0;
		}
	}

    assert(data.raw != NULL);
}

Image::Image (const Image& src){
	
	width           = src.width;
    height          = src.height;
    num_pixels      = width * height;
    sampling_method = IMAGE_SAMPLING_POINT;
    
    data.raw = new uint8_t[num_pixels*4];
    
    //memcpy(data.raw, src.data.raw, num_pixels);
    *data.raw = *src.data.raw;
}

Image::Image (char* fname){

	int numComponents; //(e.g., Y, YA, RGB, or RGBA)
	data.raw = stbi_load(fname, &width, &height, &numComponents, 4);
	
	if (data.raw == NULL){
		printf("Error loading image: %s", fname);
		exit(-1);
	}
	

	num_pixels = width * height;
	sampling_method = IMAGE_SAMPLING_POINT;
	
}

Image::~Image (){
    delete data.raw;
    data.raw = NULL;
}

void Image::Write(char* fname){
	
	int lastc = strlen(fname);

	switch (fname[lastc-1]){
	   case 'g': //jpeg (or jpg) or png
	     if (fname[lastc-2] == 'p' || fname[lastc-2] == 'e') //jpeg or jpg
	        stbi_write_jpg(fname, width, height, 4, data.raw, 95);  //95% jpeg quality
	     else //png
	        stbi_write_png(fname, width, height, 4, data.raw, width*4);
	     break;
	   case 'a': //tga (targa)
	     stbi_write_tga(fname, width, height, 4, data.raw);
	     break;
	   case 'p': //bmp
	   default:
	     stbi_write_bmp(fname, width, height, 4, data.raw);
	}
}

void Image::AddNoise (double factor)
{
	// Algo:
	//	Create a random pixel - this pixel will be the noise
	//	Multiply the random pixel with factor
	//  Add the random pixel with original pixel

	int x,y;
	for (x = 0 ; x < Width() ; x++)
	{
		for (y = 0 ; y < Height() ; y++)
		{
			Pixel p = GetPixel(x, y);
			Pixel p_noise = PixelRandom();
			Pixel noisy_pixel = p + p_noise*factor;

			GetPixel(x,y) = noisy_pixel;
		}
	}

}

void Image::Brighten (double factor)
{
	int x,y;
	for (x = 0 ; x < Width() ; x++)
	{
		for (y = 0 ; y < Height() ; y++)
		{
			Pixel p = GetPixel(x, y);
			Pixel scaled_p = p*factor;
			GetPixel(x,y) = scaled_p;
		}
	}
}


void Image::ChangeContrast (double factor)
{

	float mean_luminance;
	float total_luminance = 0;
	
    for (int x = 0 ; x < Width() ; x++)
    {
        for (int y = 0 ; y < Height() ; y++)
        {
            Pixel p = GetPixel(x, y);
            total_luminance += 0.3*p.r + 0.59*p.g + 0.11*p.b;
        }
    }

    // mean = total / number of pixels
    printf ("total lum %f\n", total_luminance);
    mean_luminance = total_luminance / (Width()*Height());

    for (int x = 0 ; x < Width() ; x++)
    {
        for (int y = 0 ; y < Height() ; y++)
        {
            Pixel p = GetPixel(x, y);

            p.r = mean_luminance*(1-factor) + p.r*factor > 255 ? 255 : mean_luminance*(1-factor) + p.r*factor;
            p.g = mean_luminance*(1-factor) + p.g*factor > 255 ? 255 : mean_luminance*(1-factor) + p.g*factor;
            p.b = mean_luminance*(1-factor) + p.b*factor > 255 ? 255 : mean_luminance*(1-factor) + p.b*factor;
			
           	GetPixel(x,y) = p;
        }
    }
}


void Image::ChangeSaturation(double factor)
{
	float luminance;
	
    for (int x = 0 ; x < Width() ; x++)
    {
        for (int y = 0 ; y < Height() ; y++)
        {
            Pixel p = GetPixel(x, y);
            luminance = 0.3*p.r + 0.59*p.g + 0.11*p.b;
            p.r = luminance*(1-factor) + p.r*factor > 255 ? 255 : luminance*(1-factor) + p.r*factor;
            p.g = luminance*(1-factor) + p.g*factor > 255 ? 255 : luminance*(1-factor) + p.g*factor;
            p.b = luminance*(1-factor) + p.b*factor > 255 ? 255 : luminance*(1-factor) + p.b*factor;
        	GetPixel(x,y) = p;
        }
    }
}


Image* Image::Crop(int x, int y, int w, int h)
{

	// new image
    Image* image_cropped = new Image(w,h);

	// check for overflow
	w = x+w > Width() ? Width() - x : w;
	h = y+h > Height() ? Height() - y : h;
    
    //Copy and crop image
    for (int i = 0 ; i < image_cropped->Width() ; i++)
    {
        for (int j = 0 ; j < image_cropped->Height() ; j++)
        {
            Pixel p = GetPixel(x + i, y + j);
            image_cropped->GetPixel(i,j) = p;
        }
    }
    
	return image_cropped;
}


void Image::ExtractChannel(int channel)
{

	assert(channel < 3);

	int w = Width();
	int h = Height();


	for (int x = 0 ; x < w ; x++)
    {
        for (int y = 0 ; y < h ; y++)
        {
        	if (channel == 0)			// red
            {	
            	GetPixel(x, y).g = 0;
            	GetPixel(x, y).b = 0;
            } else if (channel == 1)	// green
            {
                GetPixel(x, y).r = 0;
                GetPixel(x, y).b = 0;
            } else if (channel == 2)	// blue
            {
                GetPixel(x, y).r = 0;
                GetPixel(x, y).g = 0;
            }
        }
    }
}


void Image::Quantize (int nbits)
{
    
    for (int x = 0; x < Width(); x++)
    {
        for (int y = 0; y < Height(); y++)
        {
            GetPixel(x,y) = PixelQuant(GetPixel(x, y), nbits); 
        }
    }
}

void Image::RandomDither (int nbits)
{
    for (int x = 0; x < Width(); x++)
    {
        for (int y = 0; y < Height(); y++)
        {
            Pixel p = GetPixel(x, y) + ComponentRandom(); // + .5;
            GetPixel(x,y) = PixelQuant(p, nbits);
        }
    }
}


static int Bayer4[4][4] =
{
    {15,  7, 13,  5},
    { 3, 11,  1,  9},
    {12,  4, 14,  6},
    { 0,  8,  2, 10}
};


void Image::OrderedDither(int nbits)
{
    for (int y = 0; y < Height(); y++)
    {
        for (int x = 0; x < Width(); x++)
        {
            Pixel p = GetPixel(x, y) + Bayer4[x % 4][y % 4];
            GetPixel(x,y) = PixelQuant(p, nbits);
        }
    }
}

/* Error-diffusion parameters */
const double
    ALPHA = 7.0 / 16.0,
    BETA  = 3.0 / 16.0,
    GAMMA = 5.0 / 16.0,
    DELTA = 1.0 / 16.0;

void Image::FloydSteinbergDither(int nbits)
{
	
	float error_r, error_g, error_b;

    for (int x = 0; x < Width(); x++)
    {
        for (int y = 0; y < Height(); y++)
        {	
        	//Pixel newPixel = PixelQuant(GetPixel(x, y), nbits);
            error_r = GetPixel(x,y).r - PixelQuant(GetPixel(x, y), nbits).r; 
            error_g = GetPixel(x,y).g - PixelQuant(GetPixel(x, y), nbits).g; 
            error_b = GetPixel(x,y).b - PixelQuant(GetPixel(x, y), nbits).b; 
            //cout << "error : " << error << endl;

            GetPixel(x,y) = PixelQuant(GetPixel(x, y), nbits);

            Pixel p;

            if (ValidCoord(x, y+1)) {
            	p = GetPixel(x, y+1);   
            	p.r   += ALPHA * error_r;  
            	p.g   += ALPHA * error_g;  
            	p.b   += ALPHA * error_b;  
            	GetPixel(x, y+1) = p;
        	}

            if (ValidCoord(x+1, y-1)) {
            	p = GetPixel(x+1, y-1); //p 	+= BETA * error;
            	p.r   += BETA * error_r;  
            	p.g   += BETA * error_g;  
            	p.b   += BETA * error_b;  
            	GetPixel(x+1, y-1) = p;
            }

            if (ValidCoord(x+1, y)) {
            	p = GetPixel(x+1, y); 	//p   += GAMMA * error;
            	p.r   += GAMMA * error_r;  
            	p.g   += GAMMA * error_g;  
            	p.b   += GAMMA * error_b;  
            	GetPixel(x+1, y) = p;
            }

            if (ValidCoord(x+1, y+1)) {
	            p = GetPixel(x+1, y+1); //p 	+= DELTA * error;
            	p.r   += DELTA * error_r;  
            	p.g   += DELTA * error_g;  
            	p.b   += DELTA * error_b;  
            	GetPixel(x+1, y+1) = p;
            }
        }
    }
}

void createFilter(double gKernel[][5], double sigma)
{
    // set standard deviation to 1.0
    //double sigma = 1.0;
    int w = 2;
    double r;
    double s = 2.0 * sigma * sigma;
 
    // sum is for normalization
    double sum = 0.0;
 
    // generate 5x5 kernel
    for (int x = -w; x <= w; x++)
    {
        for(int y = -w; y <= w; y++)
        {
            r = sqrt(x*x + y*y);
            gKernel[x + w][y + w] = (exp(-(r*r)/s))/(M_PI * s);
            sum += gKernel[x + w][y + w];
        }
    }
 
    // normalize the Kernel
    for(int i = 0; i < 2*w+1; ++i)
        for(int j = 0; j < 2*w+1; ++j)
            gKernel[i][j] /= sum;
 
}

// Ref: http://lodev.org/cgtutor/filtering.html
void Image::Blur(int n)
{

	int filterHeight = 5;
	int filterWidth = 5;

	double filter[5][5];
    createFilter(filter, n);

    for(int i = 0; i < 5; ++i)
    {
        for (int j = 0; j < 5; ++j)
            cout<<filter[i][j]<<"\t";
        cout<<endl;
    }


	double factor = 1.0 / 1.0;
	double bias = 0.0;

	int w = Width();
	int h = Height();

  	//apply the filter
  	for(int x = 0; x < w; x++)
  	{
  		for(int y = 0; y < h; y++)
  		{
    		double red = 0.0, green = 0.0, blue = 0.0;

    		//multiply every value of the filter with corresponding image pixel
    		for(int filterY = 0; filterY < filterHeight; filterY++)
    		{
    			for(int filterX = 0; filterX < filterWidth; filterX++)
    			{
    				int imageX = (x - filterWidth / 2 + filterX + w) % w;
      				int imageY = (y - filterHeight / 2 + filterY + h) % h;

      				red 	+= GetPixel(imageX, imageY).r * filter[filterY][filterX];
      				green 	+= GetPixel(imageX, imageY).g * filter[filterY][filterX];
      				blue 	+= GetPixel(imageX, imageY).b * filter[filterY][filterX];
    			}
    		}

    		//truncate values smaller than zero and larger than 255
    		GetPixel(x,y).r = ComponentClamp(int(factor * red + bias));
    		GetPixel(x,y).g = ComponentClamp(int(factor * green + bias));
    		GetPixel(x,y).b = ComponentClamp(int(factor * blue + bias));
  		}
  	}	
}

void Image::Sharpen(int n)
{
	// Save the original image

	int w = Width();
	int h = Height();

	Image* oldimage = new Image(Width(),Height());

  	for(int x = 0; x < w; x++)
  	{
  		for(int y = 0; y < h; y++)
  		{
  			oldimage->GetPixel(x,y) = GetPixel(x,y);
  		}
  	}

	// First blur the image
	Blur(n);

	// Extrapolate from the blurred image

  	for(int x = 0; x < w; x++)
  	{
  		for(int y = 0; y < h; y++)
  		{
  			GetPixel(x,y).r = (1-n)*GetPixel(x,y).r + n*oldimage->GetPixel(x,y).r;
  			GetPixel(x,y).g = (1-n)*GetPixel(x,y).g + n*oldimage->GetPixel(x,y).g;
  			GetPixel(x,y).b = (1-n)*GetPixel(x,y).b + n*oldimage->GetPixel(x,y).b;
  		}
  	}
}

// Ref: http://lodev.org/cgtutor/filtering.html
// Convolution code adopted from above ref
void Image::EdgeDetect()
{
	int filterHeight = 3;
	int filterWidth = 3;

	double filter[3][3] =
		{
			-1, -1, -1,
			-1, +8, -1,
			-1, -1, -1,
		};

	int w = Width();
	int h = Height();

  	//apply the filter
  	for(int x = 0; x < w; x++)
  	{
  		for(int y = 0; y < h; y++)
  		{
    		double red = 0.0, green = 0.0, blue = 0.0;

    		//multiply every value of the filter with corresponding image pixel
    		for(int filterY = 0; filterY < filterHeight; filterY++)
    		{
    			for(int filterX = 0; filterX < filterWidth; filterX++)
    			{
    				int imageX = (x - filterWidth / 2 + filterX + w) % w;
      				int imageY = (y - filterHeight / 2 + filterY + h) % h;

      				red 	+= GetPixel(imageX, imageY).r * filter[filterY][filterX];
      				green 	+= GetPixel(imageX, imageY).g * filter[filterY][filterX];
      				blue 	+= GetPixel(imageX, imageY).b * filter[filterY][filterX];
    			}
    		}
    		//truncate values smaller than zero and larger than 255
    		GetPixel(x,y).r = ComponentClamp(int(red));
    		GetPixel(x,y).g = ComponentClamp(int(green));
    		GetPixel(x,y).b = ComponentClamp(int(blue));
  		}
  	}
}

Image* Image::Scale(double sx, double sy)
{
	/* WORK HERE */
	return NULL;
}

Image* Image::Rotate(double angle)
{
	/* WORK HERE */
	return NULL;
}

void Image::Fun()
{
	/* WORK HERE */
}

/**
 * Image Sample
 **/
void Image::SetSamplingMethod(int method)
{
    assert((method >= 0) && (method < IMAGE_N_SAMPLING_METHODS));
    sampling_method = method;
}


Pixel Image::Sample (double u, double v){
    /* WORK HERE */
	return Pixel();
}