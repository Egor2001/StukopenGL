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

int main()
{
    float ang = float(M_PI / 12.0f);

    SVector vec_x = SVector(cosf(ang), -sinf(ang), 0.0f, 0.0f);
    SVector vec_y = SVector(sinf(ang),  cosf(ang), 0.0f, 0.0f);
    SVector vec_z = SVector(0.0f,       0.0f,      1.0f, 0.0f);

    SMatrix mtx = sgl_new_basis_mtx(vec_x, vec_y, vec_z); 
    mtx = mtx*mtx*mtx;

    SVector vec = SVector(sqrtf(2.0f), sqrtf(2.0f), 2.0f);

    printf("initial vec: %.3f %.3f %.3f %.3f \n", 
            vec.x, vec.y, vec.z, vec.w);
    vec = mtx*vec;
    printf("rotated vec: %.3f %.3f %.3f %.3f \n", 
            vec.x, vec.y, vec.z, vec.w);

    return 0;
}

