#include <axmol.h>
#include "renderer/backend/Backend.h"

using namespace ax;

class TileMeshCreator {
public:
    static void updateMeshVertexData(const std::vector<float>& vertices, Mesh* mesh) {
        mesh->getVertexBuffer()->updateData((void*)&vertices[0], vertices.size() * sizeof(vertices[0]));
    }

    static MeshRenderer* createMeshRenderer(std::string_view texturePath, Mesh* mesh) {
        auto mat = MeshMaterial::createBuiltInMaterial(MeshMaterial::MaterialType::QUAD_TEXTURE, false);
        auto texture = Director::getInstance()->getTextureCache()->addImage(texturePath);
        texture->setAliasTexParameters();
        mat->setTexture(texture, ax::NTextureData::Usage::Diffuse);
        mat->setForce2DQueue(true);
        mesh->setMaterial(mat);
        auto renderer = MeshRenderer::create();
        renderer->addMesh(mesh);
        return renderer;
    }

    static Mesh* buildTiledMesh(std::vector<float>& vertices, uint64_t* tiles, ax::Vec2 tile_size, ax::Vec2 tex_size) {
        int perVertexSizeInFloat = 9;
        IndexArray indices;
        vertices.clear();
        //vertices.reserve(32 * 32 * 9 * 4 /* tiles.x * tiles.y * vertex_size * vertices */);
        indices.clear(CustomCommand::IndexFormat::U_SHORT);
            for (u8 y1 = 32; y1 > 0; y1--)
                for (u8 x1 = 0; x1 < 32; x1++)
                {
                    unsigned short startindex = vertices.size() / perVertexSizeInFloat;
                    float x = x1 * tile_size.x;
                    float y = y1 * tile_size.y - tile_size.y;
                    float sx = tile_size.x;
                    float sy = tile_size.y;
                    Color4F tc = Color4F::WHITE;

                    vertices.insert(vertices.end(), {
                        x, y,           0,  tc.r, tc.g, tc.b, tc.a,  0,0,
                        x + sx, y,      0,  tc.r, tc.g, tc.b, tc.a,  1,0,
                        x, y + sy,      0,  tc.r, tc.g, tc.b, tc.a,  0,1,
                        x + sx, y + sy, 0,  tc.r, tc.g, tc.b, tc.a,  1,1,
                        });

                    indices.insert<uint16_t>(indices.size(),
                        ilist_u16_t{ startindex, uint16_t(startindex + 3), uint16_t(startindex + 2),
                        uint16_t(startindex + 1), uint16_t(startindex + 3), startindex });
                }

        std::vector<MeshVertexAttrib> attribs;
        MeshVertexAttrib att;

        att.type = backend::VertexFormat::FLOAT3;
        att.vertexAttrib = shaderinfos::VertexKey::VERTEX_ATTRIB_POSITION;
        attribs.push_back(att);

        att.type = backend::VertexFormat::FLOAT4;
        att.vertexAttrib = shaderinfos::VertexKey::VERTEX_ATTRIB_COLOR;
        attribs.push_back(att);

        att.type = backend::VertexFormat::FLOAT2;
        att.vertexAttrib = shaderinfos::VertexKey::VERTEX_ATTRIB_TEX_COORD;
        attribs.push_back(att);

        auto mesh = Mesh::create(vertices, perVertexSizeInFloat, indices, attribs);
            
        return mesh;
    }
};
