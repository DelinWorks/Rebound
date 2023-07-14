#include "uiToolTip.h"

CUI::ToolTip::ToolTip() {
    scheduleUpdate();
    setBorderLayoutAnchor(BorderLayout::TOP_LEFT);
    setForceRawInput(true);
    setCascadeOpacityEnabled(true);
}

CUI::ToolTip* CUI::ToolTip::create()
{
    ToolTip* ref = new ToolTip();
    if (ref->init())
    {
        ref->label = CUI::Label::create();
        ref->label->hAlignment = ax::TextHAlignment::CENTER;
        ref->label->init(L"", TTFFS);
        ref->label->hAlignment = TextHAlignment::LEFT;
        ref->addChildAsContainer(ref->label)->setCascadeOpacityEnabled(true);
        ref->setBackgroundSprite();
        ref->setMargin(Vec2(10, 10));
        ref->setVisible(false);
        ref->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(ref);
    }
    return ref;
}

void CUI::ToolTip::update(f32 dt)
{
    Vec2 visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 clampedMousePos = ax::Vec2::ZERO;
    clampedMousePos.x = Math::clamp(mousePos.x, visibleSize.x / -2, visibleSize.x / 2 - getContentSize().x);
    bool isClamped = mousePos.y < visibleSize.y / -2 + getContentSize().y / 2;
    clampedMousePos.y = Math::clamp(mousePos.y, visibleSize.y / -2 + getContentSize().y, UINT32_MAX);
    Vec2 curPos = position;
    curPos.x = LERP(curPos.x, clampedMousePos.x, 15 * dt);
    curPos.y = LERP(curPos.y, clampedMousePos.y, 15 * dt);
    auto angle = Vec2(curPos.x + curPos.y * 2, 1280).getAngle(Vec2(clampedMousePos.x + clampedMousePos.y * 2, 1280));
    float curRotation = -AX_RADIANS_TO_DEGREES(angle * (isClamped ? 0 : 4));
    rotation = LERP(rotation, curRotation, 16 * dt);
    rotation = Math::clamp(rotation, -50, 50);
    setRotation(rotation);
    setPosition(clampedMousePos);
    position = curPos;
}

bool CUI::ToolTip::hover(cocos2d::Vec2 mouseLocationInView, cocos2d::Camera* cam)
{
	mousePos = mouseLocationInView - Director::getInstance()->getVisibleSize() / 2 - Vec2(0, 30);
	return false;
}

void CUI::ToolTip::showToolTip(std::wstring tooltip, float time_override)
{
    float time = time_override == -1 ? 3 : time_override;
    stopAllActions();
    setOpacity(0);
    setVisible(true);
    label->setString(tooltip);
    auto seq = ax::Sequence::create(
        FadeTo::create(0.25, 255),
        DelayTime::create(time),
        FadeTo::create(0.25, 0),
        CallFunc::create([&]() { this->setVisible(false); }),
        nullptr
    );
    runAction(seq);
}

void CUI::ToolTip::hideToolTip()
{
    stopAllActions();
    auto seq = ax::Sequence::create(
        FadeTo::create(0.25, 0),
        CallFunc::create([&]() { this->setVisible(false); }),
        nullptr
    );
    runAction(seq);
}

CUI::ToolTip::~ToolTip()
{
    LOG_RELEASE;
}