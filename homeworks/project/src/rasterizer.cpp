// A practical implementation of the rasterization algorithm.
// Copyright (C) 2012  www.scratchapixel.com
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//[/ignore]

#include "geometry.h"
#include <fstream>
#include <chrono>
#include <iostream>

using namespace std;

#include "cow.h"

static const float inchToMm = 25.4;
enum FitResolutionGate { kFill = 0, kOverscan };

const uint32_t ntris = 3156;
const float nearClippingPLane = 1;
const float farClippingPLane = 1000;
float focalLength = 20; // in mm
// 35mm Full Aperture in inches
float filmApertureWidth = 0.980;
float filmApertureHeight = 0.735;
const uint32_t imageWidth = 640;
const uint32_t imageHeight = 480;

int *tileBuffer; // = new int[256];
Vec3<unsigned char> *frameBuffer; // = new Vec3<unsigned char>[imageWidth * imageHeight];
float *depthBuffer; // = new float[imageWidth * imageHeight];
ofstream depthFile;

//[comment]
// Compute screen coordinates based on a physically-based camera model
// http://www.scratchapixel.com/lessons/3d-basic-rendering/3d-viewing-pinhole-camera
//[/comment]
void computeScreenCoordinates(
    const float &filmApertureWidth,
    const float &filmApertureHeight,
    const uint32_t &imageWidth,
    const uint32_t &imageHeight,
    const FitResolutionGate &fitFilm,
    const float &nearClippingPLane,
    const float &focalLength,
    float &top, float &bottom, float &left, float &right
)
{
    float filmAspectRatio = filmApertureWidth / filmApertureHeight;
    float deviceAspectRatio = imageWidth / (float)imageHeight;
    
    top = ((filmApertureHeight * inchToMm / 2) / focalLength) * nearClippingPLane;
    right = ((filmApertureWidth * inchToMm / 2) / focalLength) * nearClippingPLane;

    // field of view (horizontal)
    float fov = 2 * 180 / M_PI * atan((filmApertureWidth * inchToMm / 2) / focalLength);
    //std::cerr << "Field of view " << fov << std::endl;
    
    float xscale = 1;
    float yscale = 1;
    
    switch (fitFilm) {
        default:
        case kFill:
            if (filmAspectRatio > deviceAspectRatio) {
                xscale = deviceAspectRatio / filmAspectRatio;
            }
            else {
                yscale = filmAspectRatio / deviceAspectRatio;
            }
            break;
        case kOverscan:
            if (filmAspectRatio > deviceAspectRatio) {
                yscale = filmAspectRatio / deviceAspectRatio;
            }
            else {
                xscale = deviceAspectRatio / filmAspectRatio;
            }
            break;
    }
    
    right *= xscale;
    top *= yscale;
    
    bottom = -top;
    left = -right;
}

//[comment]
// Compute vertex raster screen coordinates.
// Vertices are defined in world space. They are then converted to camera space,
// then to NDC space (in the range [-1,1]) and then to raster space.
// The z-coordinates of the vertex in raster space is set with the z-coordinate
// of the vertex in camera space.
//[/comment]
void convertToRaster(
    const Vec3f &vertexWorld,
    const Matrix44f &worldToCamera,
    const float &l,
    const float &r,
    const float &t,
    const float &b,
    const float &near,
    const uint32_t &imageWidth,
    const uint32_t &imageHeight,
    Vec3f &vertexRaster
)
{
    Vec3f vertexCamera;

    worldToCamera.multVecMatrix(vertexWorld, vertexCamera);
    
    // convert to screen space
    Vec2f vertexScreen;
    vertexScreen.x = near * vertexCamera.x / -vertexCamera.z;
    vertexScreen.y = near * vertexCamera.y / -vertexCamera.z;
    
    // now convert point from screen space to NDC space (in range [-1,1])
    Vec2f vertexNDC;
    vertexNDC.x = 2 * vertexScreen.x / (r - l) - (r + l) / (r - l);
    vertexNDC.y = 2 * vertexScreen.y / (t - b) - (t + b) / (t - b);

    // convert to raster space
    vertexRaster.x = (vertexNDC.x + 1) / 2 * imageWidth;
    // in raster space y is down so invert direction
    vertexRaster.y = (1 - vertexNDC.y) / 2 * imageHeight;
    vertexRaster.z = -vertexCamera.z;
}

float min3(const float &a, const float &b, const float &c)
{ return std::min(a, std::min(b, c)); }

float max3(const float &a, const float &b, const float &c)
{ return std::max(a, std::max(b, c)); }

float edgeFunction(const Vec3f &a, const Vec3f &b, const Vec3f &c)
{ return (c[0] - a[0]) * (b[1] - a[1]) - (c[1] - a[1]) * (b[0] - a[0]); }

const Matrix44f worldToCamera = {0.707107, -0.331295, 0.624695, 0, 0, 0.883452, 0.468521, 0, -0.707107, -0.331295, 0.624695, 0, -1.63871, -5.747777, -40.400412, 1};

void generateTileDisplayList (uint32_t tile_x, uint32_t tile_y)
{

    // compute screen coordinates
    float t, b, l, r;
    
    computeScreenCoordinates(
        filmApertureWidth, filmApertureHeight,
        imageWidth, imageHeight,
        kOverscan,
        nearClippingPLane,
        focalLength,
        t, b, l, r);

    for (uint32_t i = 0; i < ntris; ++i) {              // find out if current triangle is touched by the tile
        const Vec3f &v0 = vertices[nvertices[i * 3]];
        const Vec3f &v1 = vertices[nvertices[i * 3 + 1]];
        const Vec3f &v2 = vertices[nvertices[i * 3 + 2]];

        // [comment]
        // Convert the vertices of the triangle to raster space
        // [/comment]
        Vec3f v0Raster, v1Raster, v2Raster;
        convertToRaster(v0, worldToCamera, l, r, t, b, nearClippingPLane, imageWidth, imageHeight, v0Raster);
        convertToRaster(v1, worldToCamera, l, r, t, b, nearClippingPLane, imageWidth, imageHeight, v1Raster);
        convertToRaster(v2, worldToCamera, l, r, t, b, nearClippingPLane, imageWidth, imageHeight, v2Raster);
        
        // [comment]
        // Precompute reciprocal of vertex z-coordinate
        // [/comment]
        v0Raster.z = 1 / v0Raster.z,
        v1Raster.z = 1 / v1Raster.z,
        v2Raster.z = 1 / v2Raster.z;
        
        // Find bounding box
        float xmin = min3(v0Raster.x, v1Raster.x, v2Raster.x);
        float ymin = min3(v0Raster.y, v1Raster.y, v2Raster.y);
        float xmax = max3(v0Raster.x, v1Raster.x, v2Raster.x);
        float ymax = max3(v0Raster.y, v1Raster.y, v2Raster.y);

        uint32_t tile_x_xmin = tile_x*16;
        uint32_t tile_x_xmax = tile_x*16+16-1;

        uint32_t tile_y_xmin = tile_y*16;
        uint32_t tile_y_xmax = tile_y*16+16-1;
        
        // the triangle is out of screen or out of tile
        if (xmin > imageWidth - 1 || xmax < 0         || ymin > imageHeight - 1  || ymax < 0 || 
            xmin > tile_x_xmax    || xmax < tile_x_xmin || ymin > tile_y_xmax   || ymax < tile_y_xmin) continue;

        // be careful xmin/xmax/ymin/ymax can be negative. Don't cast to uint32_t
        uint32_t x0 = std::max(int32_t(tile_x_xmin), (int32_t)(std::floor(xmin)));
        uint32_t x1 = std::min(int32_t(tile_x_xmax), (int32_t)(std::floor(xmax)));
        uint32_t y0 = std::max(int32_t(tile_y_xmin), (int32_t)(std::floor(ymin)));
        uint32_t y1 = std::min(int32_t(tile_y_xmax), (int32_t)(std::floor(ymax)));

        //printf("tile id %d %d\n", tile_x, tile_y);
        //printf("Bounding box %d %d %d %d\n", x0, y0, x1, y1);

        float area = edgeFunction(v0Raster, v1Raster, v2Raster);

        for (uint32_t y = y0; y <= y1; ++y) {
            for (uint32_t x = x0; x <= x1; ++x) {
                Vec3f pixelSample(x + 0.5, y + 0.5, 0);
                float w0 = edgeFunction(v1Raster, v2Raster, pixelSample);
                float w1 = edgeFunction(v2Raster, v0Raster, pixelSample);
                float w2 = edgeFunction(v0Raster, v1Raster, pixelSample);
                if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
                    w0 /= area;
                    w1 /= area;
                    w2 /= area;
                    float oneOverZ = v0Raster.z * w0 + v1Raster.z * w1 + v2Raster.z * w2;
                    float z = 1 / oneOverZ;
                    // [comment]
                    // Depth-buffer test
                    // [/comment]
                    if (z < depthBuffer[y * imageWidth + x]) {
                        depthBuffer[y * imageWidth + x] = z;
                        //int tilebuffer_x = x - (16*tile_x);
                        //int tileBuffer_y = y - (16*tile_y);
                        //if (tilebuffer_x >= 16 || tileBuffer_y >= 16 || tilebuffer_x <= -1 || tileBuffer_y <= -1)
                        //    printf("BAD data ********** \n");
                        //tileBuffer[tileBuffer_y * 16 + tilebuffer_x] = i;
                        tileBuffer[y * imageWidth + x] = i;
                    }
                }
            }
        }
    }

}

void printDepth() {
    depthFile.open ("deppth.txt");
    for (int i =0; i<imageWidth*imageHeight; i++) {
        depthFile << tileBuffer[i] << '\n';
    }
}


void textureAndShading (uint32_t tile_x, uint32_t tile_y)
{

    // compute screen coordinates
    float t, b, l, r;
    
    computeScreenCoordinates(
        filmApertureWidth, filmApertureHeight,
        imageWidth, imageHeight,
        kOverscan,
        nearClippingPLane,
        focalLength,
        t, b, l, r);

    uint32_t tile_x_inpixel = tile_x*16;
    uint32_t tile_y_inpixel = tile_y*16;

    for (uint32_t y = tile_y_inpixel; y < tile_y_inpixel+16; ++y) {
        for (uint32_t x = tile_x_inpixel; x < tile_x_inpixel+16; ++x) {

            int index_x_tilebuffer = x - (tile_x*16);
            int index_y_tilebuffer = y - (tile_y*16);

            int triNum = tileBuffer[y * imageWidth + x];

            if (triNum == -1) continue;

            const Vec3f &v0 = vertices[nvertices[triNum]];
            const Vec3f &v1 = vertices[nvertices[triNum + 1]];
            const Vec3f &v2 = vertices[nvertices[triNum + 2]];

            // TODO
            /*
            *   Read the triagle color and attributes using triNum and shade the pixel
            */

            frameBuffer[y * imageWidth + x].x = 125; 
            frameBuffer[y * imageWidth + x].y = 125; 
            frameBuffer[y * imageWidth + x].z = 125; 
        }
    }
}


int main(int argc, char **argv)
{

    Matrix44f cameraToWorld = worldToCamera.inverse();

    // compute screen coordinates
    float t, b, l, r;
    
    computeScreenCoordinates(
        filmApertureWidth, filmApertureHeight,
        imageWidth, imageHeight,
        kOverscan,
        nearClippingPLane,
        focalLength,
        t, b, l, r);
    
    // define the frame-buffer and the depth-buffer. Initialize depth buffer
    // to far clipping plane.
    frameBuffer = new Vec3<unsigned char>[imageWidth * imageHeight];
    for (uint32_t i = 0; i < imageWidth * imageHeight; ++i) frameBuffer[i] = Vec3<unsigned char>(255);
    depthBuffer = new float[imageWidth * imageHeight];
    for (uint32_t i = 0; i < imageWidth * imageHeight; ++i) depthBuffer[i] = farClippingPLane;
    tileBuffer = new int[imageWidth * imageHeight];
    for (int i = 0; i < imageWidth * imageHeight; ++i) tileBuffer[i] = -1;

    auto t_start = std::chrono::high_resolution_clock::now();
    
    for (uint32_t tile_x = 0; tile_x < 40; tile_x++) {        // for every tile on screen
        for (uint32_t tile_y = 0; tile_y < 30; tile_y++) {
            generateTileDisplayList(tile_x, tile_y);            // generate a display list
            textureAndShading(tile_x, tile_y);                  // shading
        }
    }
    
    auto t_end = std::chrono::high_resolution_clock::now();
    auto passedTime = std::chrono::duration<double, std::milli>(t_end - t_start).count();
    std::cerr << "Wall passed time:  " << passedTime << " ms" << std::endl;
    
    // [comment]
    // Store the result of the framebuffer to a PPM file (Photoshop reads PPM files).
    // [/comment]
    std::ofstream ofs;
    ofs.open("./output.ppm");
    ofs << "P6\n" << imageWidth << " " << imageHeight << "\n255\n";
    ofs.write((char*)frameBuffer, imageWidth * imageWidth * 3);
    ofs.close();
    
    //printDepth();

    delete [] frameBuffer;
    delete [] depthBuffer;
    
    return 0;
}
