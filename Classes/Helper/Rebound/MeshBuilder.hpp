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

    UV saved_tl;
    UV saved_tr;
    UV saved_bl;
    UV saved_br;

    bool is90 = false;
    bool isH = false;
    bool isV = false;

    bool isSaved = false;
    
    void save() {
        if (!isSaved) {
            isSaved = true;
            saved_tl = tl;
            saved_tr = tr;
            saved_bl = bl;
            saved_br = br;
        }
    }

    void reset() {
        save();
        tl = saved_tl;
        tr = saved_tr;
        bl = saved_bl;
        br = saved_br;
        is90 = isH = isV = false;
    }

    void rotate90() {
        is90 = !is90;
        save();
        std::swap(br, bl);
        std::swap(tr, br);
        std::swap(tl, tr);
    }

    void flipH() {
        isH = !isH;
        save();
        std::swap(tl, tr);
        std::swap(bl, br);
    }

    void flipV() {
        isV = !isV;
        save();
        std::swap(tl, bl);
        std::swap(tr, br);
    }

    i8 rotation = 0;

    void cw() {
        reset();

        rotation++;

        if (rotation > 3)
            rotation = 0;

        if (rotation == 1) {
            rotate90();
            flipH();
            flipV();
        }
        else if (rotation == 2) {
            flipV();
            flipH();
        }
        else if (rotation == 3) {
            rotate90();
        }
    }

    void ccw() {
        reset();

        rotation--;

        if (rotation < 0)
            rotation = 3;

        if (rotation == 1) {
            rotate90();
            flipH();
            flipV();
        }
        else if (rotation == 2) {
            flipV();
            flipH();
        }
        else if (rotation == 3) {
            rotate90();
        }
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
        vertices.reserve(32 * 32 * 9 * 4 /* tiles.x * tiles.y * vertex_size * vertices */);
        indices.clear(CustomCommand::IndexFormat::U_SHORT);
        int index = 0;
            for (u8 y1 = 32; y1 > 0; y1--)
                for (u8 x1 = 0; x1 < 32; x1++)
                {
                    float column = (tiles[index] % (int)(tex_size.x / tile_size.x)) * tile_size.x;
                    float row = floor(tiles[index] / (tex_size.y / tile_size.y)) * tile_size.y;
                    float columnM = (column + tile_size.x);
                    float rowM = (row + tile_size.y);

                    column /= tex_size.x;
                    row /= tex_size.y;
                    columnM /= tex_size.x;
                    rowM /= tex_size.y;

                    row = 1.0 - row;
                    rowM = 1.0 - rowM;

                    unsigned short startindex = vertices.size() / perVertexSizeInFloat;
                    float x = x1 * tile_size.x;
                    float y = y1 * tile_size.y - tile_size.y;
                    float sx = tile_size.x;
                    float sy = tile_size.y;
                    Color4F tc = Color4F::WHITE;

                    TileTexCoords coord{
                        { column,  rowM },
                        { columnM, rowM },
                        { column,  row  },
                        { columnM, row  },
                    };

                    if (Random::float01() >= 0.5)
                        coord.rotate90();
                    if (Random::float01() >= 0.5)
                        coord.flipH();
                    if (Random::float01() >= 0.5)
                        coord.flipV();

                    vertices.insert(vertices.end(), {
                        x, y,           0,  tc.r, tc.g, tc.b, tc.a,  coord.tl.U, coord.tl.V,
                        x + sx, y,      0,  tc.r, tc.g, tc.b, tc.a,  coord.tr.U, coord.tr.V,
                        x, y + sy,      0,  tc.r, tc.g, tc.b, tc.a,  coord.bl.U, coord.bl.V,
                        x + sx, y + sy, 0,  tc.r, tc.g, tc.b, tc.a,  coord.br.U, coord.br.V,
                        });

                    indices.insert<uint16_t>(indices.size(),
                        ilist_u16_t{ startindex, uint16_t(startindex + 3), uint16_t(startindex + 2),
                        uint16_t(startindex + 1), uint16_t(startindex + 3), startindex });

                    index++;
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
