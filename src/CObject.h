#ifndef SGL_COBJECT_H
#define SGL_COBJECT_H

//general
#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <cstdint>
#include <ctime>
#include <cstring>
#include <vector>

#include "math/SVector.h"
#include "math/SColor.h" 
#include "ShaderStructures.h"

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

    const std::vector<SVertex>& vertex_buf() const noexcept
    {
        return vertex_buf_;
    }

    const std::vector<SObjIndex>& index_buf() const noexcept
    {
        return index_buf_;
    }

    bool parse_from(FILE* obj_file, const SColor& color_set = SColor{});
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

bool CObject::parse_from(FILE* obj_file, const SColor& color_set)
{
    bool result = false;

    std::vector<SVectorExt> vector_temp_buf;
    std::vector<SVector>    normal_temp_buf;

    if (!obj_file)
        result = false;
    else
    {
        vertex_buf_.clear();
        index_buf_ .clear();

        const size_t LINE_LEN = 256;
        char cur_line[LINE_LEN] = "";

        int ret = 1;
        while (ret && !feof(obj_file))
        {
            ret = fscanf(obj_file, "%255[^\n]", cur_line);

            if (!ret)
            {
                ret = fscanf(obj_file, "%255[\n\r ]", cur_line);
                continue;
            }

            char* it = cur_line;
            while (isspace(*it)) ++it;
            
            switch (*it)
            {
                case 'v':
                {
                    ++it;
                    if (isspace(*it))
                    {
                        SVectorExt cur_vector;
                        cur_vector.w = 1.0f;

                        sscanf(it, "%f %f %f %f", 
                               &cur_vector.x, &cur_vector.y, &cur_vector.z,
                               &cur_vector.w); //w could be ignored
                        
                        vector_temp_buf.push_back(cur_vector);
                    }
                    else if (*it == 'n' && isspace(*(++it)))
                    {
                        SVector cur_normal;

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
                        auto vector_idx_vec = std::vector<size_t>(); 
                        auto normal_idx_vec = std::vector<size_t>();

                        size_t cur_i = 0;
                        int is_scanned = 2;
                        for (cur_i = 0; *it && (is_scanned == 2); ++cur_i)
                        {
                            while (std::isspace(*it)) ++it;
                            if (!(*it)) 
                                break;

                            size_t cur_vector_idx = 0;
                            size_t cur_normal_idx = 0;
                            is_scanned = sscanf(it, "%lu/%*u/%lu",
                                                &cur_vector_idx,
                                                &cur_normal_idx);

                            if (is_scanned == 2)
                            {
                                vector_idx_vec.push_back(cur_vector_idx);
                                normal_idx_vec.push_back(cur_normal_idx);
                            }
                            else
                                break;

                            while (!std::isspace(*it)) ++it;
                        }

                        size_t cnt_scanned = cur_i;

                        auto cur_index_vec = std::vector<size_t>();
                        for (cur_i = 0; cur_i < cnt_scanned; ++cur_i)
                        {
                            SVertex cur_vertex = SVertex();

                            size_t cur_vector_idx = vector_idx_vec[cur_i]-1; 
                            size_t cur_normal_idx = normal_idx_vec[cur_i]-1;
                            cur_vertex.point  = vector_temp_buf[cur_vector_idx];
                            cur_vertex.normal = normal_temp_buf[cur_normal_idx];
                            cur_vertex.color = color_set;

                            cur_index_vec.push_back(vertex_buf_.size());
                            vertex_buf_.push_back(cur_vertex);
                        }

                        SObjIndex cur_index = SObjIndex();
                        cur_index.arr[0] = cur_index_vec[0];
                        for (size_t i = 1; i+1 < cur_index_vec.size(); ++i)
                        {
                            cur_index.arr[1] = cur_index_vec[i+0];
                            cur_index.arr[2] = cur_index_vec[i+1];

                            index_buf_.push_back(cur_index);
                        }
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

        result = true;
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
            SVector cur_point = narrow(vertex_buf_[i].point);
                                //SVector(vertex_buf_[i].point.x,
                                //        vertex_buf_[i].point.y,
                                //        vertex_buf_[i].point.z);

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
            fprintf(obj_file, "f %lu//%lu %lu//%lu %lu//%lu\n",
                    index.arr[0] + 1, index.arr[0] + 1,
                    index.arr[1] + 1, index.arr[1] + 1,
                    index.arr[2] + 1, index.arr[2] + 1);
        }
    }

    return result;
}

int test_CObject()
{
    /*
    SVertex tetra[4] = {SVertex{.point=SVectorExt(0.5f, 0.5f, 0.5f),
                                .normal=SVector(0.5f, 0.5f, 0.5f),
                                .color=SColor(1.0f, 1.0f, 1.0f)},
                        SVertex{.point=SVectorExt(0.5f, -0.5f, 0.5f),
                                .normal=SVector(0.5f, -0.5f, 0.5f),
                                .color=SColor(1.0f, 1.0f, 1.0f)},
                        SVertex{.point=SVectorExt(-0.5f, 0.0f, 0.5f),
                                .normal=SVector(-0.5f, 0.0f, 0.5f),
                                .color=SColor(1.0f, 1.0f, 1.0f)},
                        SVertex{.point=SVectorExt(0.0f, 0.0f, 0.0f),
                                .normal=SVector(0.0f, 0.0f, 0.0f),
                                .color=SColor(1.0f, 1.0f, 1.0f)}};

    auto vertex_buf = std::vector<SVertex>({tetra[0], tetra[1], tetra[2],
                                            tetra[0], tetra[1], tetra[3],
                                            tetra[0], tetra[2], tetra[3],
                                            tetra[1], tetra[2], tetra[3]});
    */
    CObject object;// = CObject(std::move(vertex_buf));

    FILE* obj_file = fopen("test.obj", "r+");
/*
    object.write_to(obj_file);
    rewind(obj_file);
*/
    object.parse_from(obj_file);
    object.write_to(stdout);

    fclose(obj_file); obj_file = nullptr;

    return 0;
}

//} //namespace sgl 

#endif //SGL_COBJECT_H
