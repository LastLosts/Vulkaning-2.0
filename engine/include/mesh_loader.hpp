#pragma once

#include "mesh.hpp"

#include <string_view>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

namespace ving
{
static Mesh load_mesh(const VulkanCore &core, std::string_view filepath)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;
    bool success = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.data());

    if (!warn.empty())
        std::cout << "WARN: " << warn << '\n';
    if (!err.empty())
        std::cerr << "ERR: " << err << '\n';
    if (!success)
    {
        std::cerr << "FAILED TO LOAD MESH!\n";
        return {};
    }

    std::vector<Vertex> vertices;
    vertices.reserve(attrib.vertices.size() / 3);
    std::vector<uint32_t> indices;

    for (uint32_t i = 0; i < attrib.vertices.size(); i += 3)
    {
        vertices.push_back(Vertex{{attrib.vertices[i + 0], attrib.vertices[i + 1], attrib.vertices[i + 2]}, 0, {}, 0});
    }
    size_t total_mesh_indices{};
    for (auto &&shape : shapes)
    {
        total_mesh_indices += shape.mesh.indices.size();
    }

    assert(total_mesh_indices > 0);
    indices.reserve(total_mesh_indices);

    bool normals = false, uvs = false;

    for (auto &&shape : shapes)
    {
        for (auto &&index : shape.mesh.indices)
        {
            if (index.vertex_index < 0)
            {
                std::cout << "ERROR: Bad mesh!";
                return {};
            }
            indices.push_back(index.vertex_index);

            if (index.normal_index >= 0)
            {
                vertices[index.vertex_index].normal = {attrib.normals[3 * index.normal_index + 0],
                                                       attrib.normals[3 * index.normal_index + 1],
                                                       attrib.normals[3 * index.normal_index + 2]};
                normals = true;
            }
            if (index.texcoord_index >= 0)
            {
                vertices[index.vertex_index].uv_x = attrib.texcoords[2 * index.texcoord_index + 0];
                vertices[index.vertex_index].uv_y = attrib.texcoords[2 * index.texcoord_index + 1];
                uvs = true;
            }
        }
    }

    if (!normals)
        std::cout << "Warn: No Normals!\n";
    if (!uvs)
        std::cout << "Warn: No Uvs!\n";

    return {core, vertices, indices};
}
} // namespace ving
