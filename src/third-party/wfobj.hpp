#pragma once
#include <cstdio>
#include <fstream>
#include <vector>
#include "../SVertex.h"
#include "../SIndex.h"

typedef struct
{
    float x, y;
} vec2;

struct Mesh
{
    using uint = unsigned;
    std::vector<SVertex> verts;
    std::vector<SIndex> inds;
};

inline Mesh import_obj(char const *filename)
{
    std::ifstream in(filename);
    Mesh out;
    if(!in.is_open())
        throw std::invalid_argument("can not find file " + std::string(filename));

    std::vector<SVector> pos;
    std::vector<vec2> tex;
    std::vector<SVector> norm;

    std::string line;
    while(std::getline(in, line))
    {
        char const * const cstr = line.c_str();
        if(cstr[0] == 'v')
        {
            SVector v;
            vec2 vt;
            if(sscanf(cstr + 1, " %f %f %f", &v.x, &v.y, &v.z) == 3)
                pos.push_back(v);
            else if(sscanf(cstr + 1, "t %f %f", &vt.x, &vt.y) == 2)
                tex.push_back(vt);
            else if(sscanf(cstr + 1, "n %f %f %f", &v.x, &v.y, &v.z) == 3)
                norm.push_back(v);
        }
        else if(cstr[0] == 'f')
        {
            SIndex idx = {};
            auto const vsize = out.verts.size();

            char const *cptr = cstr + 2;
            int eaten;
            while(sscanf(cptr, "%u/%u/%u%n", idx.arr, idx.arr + 1, idx.arr + 2, &eaten) == 3)
            {
                out.verts.push_back(SVertex
                        {.point = ::extend(pos[idx.arr[0] - 1]), 
                         .normal = norm[idx.arr[2] - 1],
                         .color = SColor{1.0f, 1.0f, 1.0f},
                         .tex_u = tex[idx.arr[1] - 1].x, 
                         .tex_v = tex[idx.arr[1] - 1].y 
                         });
                cptr += eaten;
            }

            auto const vcount = out.verts.size() - vsize;
            for(auto i = 1u; i < vcount - 1; i++)
            {
                SIndex cur_idx;
                cur_idx.arr[0] = vsize;
                cur_idx.arr[1] = vsize + i;
                cur_idx.arr[2] = vsize + i + 1;

                out.inds.push_back(cur_idx);
            }
        }
    }
    return out;
}
