#include <axmol.h>
#include "renderer/backend/Backend.h"

using namespace ax;

struct UV {
    float U;
    float V;
};

struct TileTexCoords {
    UV tl;
    UV tr;
    UV bl;
    UV br;

    bool is90 = false;
    bool isH = false;
    bool isV = false;
    
    void rotate90() {
        is90 = !is90;

        float tmp = 0;

        tmp = tl.U;
        tl.U = tl.V;
        tl.V = tmp;

        tmp = tr.U;
        tr.U = tr.V;
        tr.V = tmp;

        tmp = bl.U;
        bl.U = bl.V;
        bl.V = tmp;

        tmp = br.U;
        br.U = br.V;
        br.V = tmp;
    }

    void flipH() {
        isH = !isH;

        UV tmp;

        tmp = tl;
        tl = tr;
        tr = tmp;

        tmp = bl;
        bl = br;
        br = tmp;
    }

    void flipV() {
        isV = !isV;

        UV tmp;

        tmp = tl;
        tl = bl;
        bl = tmp;

        tmp = tr;
        tr = br;
        br = tmp;
    }

    void rotateClockwise() {
        rotate90();
        flipV();
        if (!is90)
            flipH();
    }
};

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

                    TileTexCoords coord{ { 0,0 }, { 1,0 }, { 0,1 }, { 1,1 } };

                    vertices.insert(vertices.end(), {
                        x, y,           0,  tc.r, tc.g, tc.b, tc.a,  coord.tl.U, coord.tl.V,
                        x + sx, y,      0,  tc.r, tc.g, tc.b, tc.a,  coord.tr.U, coord.tr.V,
                        x, y + sy,      0,  tc.r, tc.g, tc.b, tc.a,  coord.bl.U, coord.bl.V,
                        x + sx, y + sy, 0,  tc.r, tc.g, tc.b, tc.a,  coord.br.U, coord.br.V,
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
