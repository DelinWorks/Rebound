#include "GameplayScene.h"

#define LERP MathUtil::lerp

using namespace GameUtils::CocosExt::CustomComponents;

USING_NS_CC;

#define SET_UNIFORM(ps, name, value)  do { \
decltype(value) __v = value; \
auto __loc = (ps)->getUniformLocation(name); \
(ps)->setUniform(__loc, &__v, sizeof(__v)); \
} while(false) 

Scene* GameplayScene::createScene()
{
    return GameplayScene::create();
}

bool GameplayScene::init()
{
    if (!Scene::init())
    {
        return false;
    }

    REGISTER_SCENE(GameplayScene);

    scheduleUpdate();

    auto visibleSize = Director::getInstance()->getVisibleSize();

    _defaultCamera->setPosition({ 0,0 });
    //_defaultCamera->setZoom(3);
    //_defaultCamera->setBackgroundBrush(CameraBackgroundBrush::createColorBrush(Color4F(1, 1, 1, 1), 0));

    renderer = MeshRenderer::create();

    int perVertexSizeInFloat = 7;  // 3+2+2

    IndexArray indices;
    std::vector<float> vertices;
    indices.clear(CustomCommand::IndexFormat::U_INT);

    FastRNG r;

    auto InsertGrass = [perVertexSizeInFloat, &vertices, &indices, &r](float x, float y, float s)
    {
        s /= 2;
        float frac = 1.0 / 3.0;

        float rndSpd = 1;
        float rndYvl = 0;
        for (float i = 0; i < 1; i += frac)
        {
            unsigned int startindex = vertices.size() / perVertexSizeInFloat;

            vertices.insert(vertices.end(), {
                x - s,LERP(y - s, y + s, i),0,           0,i,        i,        rndSpd,
                x + s,LERP(y - s, y + s, i),0,           1,i,        i,        rndSpd,
                x - s,LERP(y - s, y + s, i + frac),0,    0,i + frac, i + frac, rndSpd,
                x + s,LERP(y - s, y + s, i + frac),0,    1,i + frac, i + frac, rndSpd,
                });

            indices.insert<uint32_t>(indices.size(),
                ilist_u32_t{ startindex, startindex + 3, startindex + 2, startindex + 1, startindex + 3, startindex });
        }
    };

    float yPosSpwn = visibleSize.y / 2;

    for (size_t i = 0; i < 1024; i++)
    {
        if (yPosSpwn <= visibleSize.y / -2)
            yPosSpwn = visibleSize.y / 2;
        yPosSpwn -= 10;
        InsertGrass(r.rangef(visibleSize.x / -2, visibleSize.x / 2), yPosSpwn, 32);
    }

    std::vector<MeshVertexAttrib> attribs;
    MeshVertexAttrib att;

    att.type = backend::VertexFormat::FLOAT3;
    att.vertexAttrib = shaderinfos::VertexKey::VERTEX_ATTRIB_POSITION;
    attribs.push_back(att);

    att.type = backend::VertexFormat::FLOAT2;
    att.vertexAttrib = shaderinfos::VertexKey::VERTEX_ATTRIB_TEX_COORD;
    attribs.push_back(att);

    att.type = backend::VertexFormat::FLOAT2;
    att.vertexAttrib = shaderinfos::VertexKey::VERTEX_ATTRIB_TEX_COORD1;
    attribs.push_back(att);

    auto mesh = Mesh::create(vertices, perVertexSizeInFloat, indices, attribs);

    auto programState = GameUtils::CocosExt::createGPUProgram("grass_animation.fsh", "grass_animation.vsh");

    ps1 = programState;

    auto mat = MeshMaterial::createWithProgramState(programState);
    mesh->setMaterial(mat);

    auto texture = Director::getInstance()->getTextureCache()->addImage("grass.png");
    texture->setAliasTexParameters();

    mesh->setTexture(texture);

    renderer->addMesh(mesh);

    addChild(renderer);

    slider = ui::Slider::create();
    slider->loadBarTexture("Slider_Back.png"); // what the slider looks like
    slider->loadSlidBallTextures("SliderNode_Normal.png", "SliderNode_Press.png", "SliderNode_Disable.png");
    slider->loadProgressBarTexture("Slider_PressBar.png");

    slider->addEventListener([&](Ref* sender, ui::Slider::EventType type) {
        switch (type)
        {
        case ui::Slider::EventType::ON_PERCENTAGE_CHANGED:
            SET_UNIFORM(ps1, "u_wind", LERP(-100, 100, slider->getPercent() / 100.0));
            break;
        default:
            break;
        }
        });

    this->addChild(slider);

    SET_UNIFORM(ps1, "u_sway", (float)20.0);
    SET_UNIFORM(ps1, "u_infl", (float)10.0);

    SET_UNIFORM(ps1, "u_bottomColor", Vec4(0.074f, 0.419f, 0.0f, 1.0f));
    SET_UNIFORM(ps1, "u_topColor", Vec4(0.243f, 0.862f, 0.0f, 1.0f));

    p = Player::createPlayer();
    addChild(p);
    p->attachCamera(getDefaultCamera());
    p->setInputState(true);

    return true;
}

void GameplayScene::awake()
{
    if (Node::isAwake()) {

    }
}

void GameplayScene::update(f32 dt)
{
    awake();

    p->update(dt);

    elapsedDt += dt;

    SET_UNIFORM(ps1, "u_time", (float)elapsedDt);

}

void GameplayScene::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event)
{
    p->onKeyPressed(keyCode);
}

void GameplayScene::onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event)
{
    p->onKeyReleased(keyCode);
}

void GameplayScene::onKeyHold(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event)
{
}

void GameplayScene::onMouseDown(cocos2d::Event* event)
{
    p->onMouseDown(event);
}

void GameplayScene::onMouseUp(cocos2d::Event* event)
{
    p->onMouseUp(event);
}

void GameplayScene::onMouseMove(cocos2d::Event* event)
{
}

void GameplayScene::onMouseScroll(cocos2d::Event* event)
{
}

bool GameplayScene::onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event)
{
    return false;
}

void GameplayScene::onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event)
{
}

void GameplayScene::onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event)
{
}

void GameplayScene::onTouchCancelled(cocos2d::Touch* touch, cocos2d::Event* event)
{
}
