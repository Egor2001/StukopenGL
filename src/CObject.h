#ifndef SGL_COBJECT_H
#define SGL_COBJECT_H

//general
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <ctime>

#include "SVector.h"
#include "SColor.h" 

//namespace sgl { 

class CObject
{
public:
    struct SObjIndex
    {
        size_t arr[3];
    };

    CObject();
    explicit CObject(std::vector<SVertex>&& move_vertex_buf);

    CObject             (const CObject&) = delete;
    CObject& operator = (const CObject&) = delete;

    CObject             (CObject&&);
    CObject& operator = (CObject&&);

    ~CObject();

    bool parse_from(FILE* obj_file);
    bool write_to  (FILE* obj_file) const;

private:
    std::vector<SVertex>   vertex_buf_;
    std::vector<SObjIndex> index_buf_;
};

CObject::CObject():
    vertex_buf_(),
    index_buf_()
{}

CObject::CObject(std::vector<SVertex>&& move_vertex_buf):
    vertex_buf_(std::move(move_vertex_buf)),
    index_buf_()
{
    for (size_t i = 0; i+2 < vertex_buf_.size(); i += 3)
        index_buf_.push_back(SObjIndex{i+0, i+1, i+2}); 
}

CObject::CObject(CObject&& move_object):
    vertex_buf_(std::move(move_object.vertex_buf_)),
    index_buf_ (std::move(move_object.index_buf_))
{}

CObject& CObject::operator = (CObject&& move_object)
{
    std::swap(vertex_buf_, move_object.vertex_buf_);
    std::swap(index_buf_,  move_object.index_buf_);

    return *this;
}

CObject::~CObject()
{
    vertex_buf_.clear();
    index_buf_ .clear();
}

bool CObject::parse_from(FILE* obj_file)
{
    bool result = false;

    std::vector<SVector> vector_temp_buf;
    std::vector<SVector> normal_temp_buf;

    if (!obj_file)
        result = false;
    else
    {
        const size_t LINE_LEN = 128;
        char cur_line[LINE_LEN] = "";

        int ret = 1;
        while (ret)
        {
            ret = fscanf(obj_file, "%127[^\n]", cur_line);

            char* it = cur_line;
            while(isspace(*it)) ++it;
            
            switch (*it)
            {
                case 'v':
                {
                    ++it;
                    if (isspace(*it))
                    {
                        SVector cur_vector = SVector();
                        cur_vector.w = 1.0f;

                        sscanf(it, "%f %f %f %f", 
                               &cur_vector.x, &cur_vector.y, &cur_vector.z,
                               &cur_vector.w); //w could be ignored

                        vector_temp_buf.push_back(cur_vector);
                    }
                    else if (*it == 'n' && isspace(*(++it)))
                    {
                        SVector cur_normal = SVector();
                        cur_normal.w = 0.0f;

                        sscanf(it, "%f %f %f", 
                               &cur_normal.x, &cur_normal.y, &cur_normal.z);

                        normal_temp_buf.push_back(cur_normal);
                    }
                } 
                break;

                case 'f':
                {
                    ++it;
                    if (isspace(*it))
                    {
                        size_t vector_idx_arr[3] = {};
                        size_t normal_idx_arr[3] = {};

                        sscanf(it, "%lu//%lu %lu//%lu %lu//%lu",
                               vector_idx_arr+0, normal_idx_arr+0, 
                               vector_idx_arr+1, normal_idx_arr+1, 
                               vector_idx_arr+2, normal_idx_arr+2); 
                       
                        SObjIndex cur_index = SObjIndex();
                        for (size_t i = 0; i < 3; ++i)
                        {
                            SVertex cur_vertex = SVertex();

                            size_t cur_vector_idx = vector_idx_arr[i]-1; 
                            size_t cur_normal_idx = normal_idx_arr[i]-1;
                            cur_vertex.point  = vector_temp_buf[cur_vector_idx];
                            cur_vertex.normal = normal_temp_buf[cur_normal_idx];

                            cur_index.arr[i] = vertex_buf_.size();
                            vertex_buf_.push_back(cur_vertex);
                        }

                        index_buf_.push_back(cur_index);
                    }
                }
                break;

                //all symbols like '#', 'o', 'vp', vt' etc 
                //are not supported yet
                default:
                {}
                break;
            }
        }
    }

    return result;
}

bool CObject::write_to(FILE* obj_file) const
{
    bool result = false;

    if (!obj_file)
        result = false;
    else
    {
        fprintf(obj_file, "#.obj from CObject::write_to \n");

        size_t vert_cnt = vertex_buf_.size();
        
        fprintf(obj_file, "#vertex points: \n");
        for (size_t i = 0; i < vert_cnt; ++i)
        {
            SVector cur_point = unitary(vertex_buf_[i].point);
            fprintf(obj_file, "v %.3f %.3f %.3f \n",
                    cur_point.x, cur_point.y, cur_point.z);
        }

        fprintf(obj_file, "#vertex normals: \n");
        for (size_t i = 0; i < vert_cnt; ++i)
        {
            SVector cur_normal = vertex_buf_[i].normal;
            fprintf(obj_file, "vn %.3f %.3f %.3f \n",
                    cur_normal.x, cur_normal.y, cur_normal.z);
        }

        fprintf(obj_file, "#indices: \n");
        for (const auto& index : index_buf_)
        {
            fprintf(obj_file, "f %lu//%lu %lu//%lu %lu//%lu \n",
                    index.arr[0] + 1, index.arr[0] + 1,
                    index.arr[1] + 1, index.arr[1] + 1,
                    index.arr[2] + 1, index.arr[2] + 1);
        }
    }

    return result;
}

int test_CObject()
{
    SVertex tetra[4] = {SVertex{.point=SVector(0.5f, 0.5f, 0.5f),
                                .normal=SVector(0.5f, 0.5f, 0.5f),
                                .color=SColor(1.0f, 1.0f, 1.0f)},
                        SVertex{.point=SVector(0.5f, -0.5f, 0.5f),
                                .normal=SVector(0.5f, -0.5f, 0.5f),
                                .color=SColor(1.0f, 1.0f, 1.0f)},
                        SVertex{.point=SVector(-0.5f, 0.0f, 0.5f),
                                .normal=SVector(-0.5f, 0.0f, 0.5f),
                                .color=SColor(1.0f, 1.0f, 1.0f)},
                        SVertex{.point=SVector(0.0f, 0.0f, 0.0f),
                                .normal=SVector(0.0f, 0.0f, 0.0f),
                                .color=SColor(1.0f, 1.0f, 1.0f)}};

    auto vertex_buf = std::vector<SVertex>({tetra[0], tetra[1], tetra[2],
                                            tetra[0], tetra[1], tetra[3],
                                            tetra[0], tetra[2], tetra[3],
                                            tetra[1], tetra[2], tetra[3]});

    CObject object = CObject(std::move(vertex_buf));

    FILE* obj_file = fopen("test.obj", "w+");

    object.write_to(obj_file);
    rewind(obj_file);

    object.parse_from(obj_file);
    object.write_to(stdout);

    fclose(obj_file); obj_file = nullptr;

    return 0;
}

//} //namespace sgl 

#endif //SGL_COBJECT_H
