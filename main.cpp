//general
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <ctime>

//double including all tested files to check collisions
#include "src/SVector.h"
#include "src/SVector.h"
#include "src/SMatrix.h"
#include "src/SMatrix.h"
#include "src/SColor.h"
#include "src/SColor.h" 
#include "src/CRasterizer.h"
#include "src/CRasterizer.h"
#include "src/CScreen.h"
#include "src/CScreen.h"
#include "src/CBuffer.h"
#include "src/CBuffer.h"
#include "src/CObject.h"
#include "src/CObject.h"
#include "src/CCamera.h"
#include "src/CCamera.h"

int main()
{
    CCamera cam = CCamera(SVector( 1.0f,  1.0f,  1.0f),
                          SVector(-1.0f, -1.0f, -1.0f));

    SMatrix mtx = cam.get_matrix();
    for (size_t y = 0; y < 4; ++y)
        printf("%.3f %.3f %.3f %.3f \n", 
                mtx[y][0], mtx[y][1], mtx[y][2], mtx[y][3]);

    float coord = 1.0f - sqrtf(3.0f)/3.0f;
    SVector vec = SVector(coord, coord, coord);
    printf("initial vec: %.3f %.3f %.3f %.3f \n", 
            vec.x, vec.y, vec.z, vec.w);
    
    vec = mtx*vec;

    printf("camera vec: %.3f %.3f %.3f %.3f \n", 
            vec.x, vec.y, vec.z, vec.w);

    return 0;
}

