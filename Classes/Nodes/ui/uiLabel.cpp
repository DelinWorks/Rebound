#include "uiLabel.h"

CUI::Label* CUI::Label::create()
{
    CUI::Label* ret = new CUI::Label();
    if (((Node*)ret)->init())
    {
        ret->setCascadeOpacityEnabled(true);
        ret->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(ret);
    }
    return ret;
}

void CUI::Label::init(std::wstring _text, i32 _fontsize, Size _size, float _wrap)
{
    init(
        _text,
        "fonts/bitsy-font-with-arabic.ttf"sv,
        _fontsize,
        _size,
        _wrap
    );
}

void CUI::Label::init(std::wstring& _text, std::string_view _fontname, i32 _fontsize, Size _size, float _wrap)
{
    if (_rescalingAllowed)
        addComponent((new UiRescaleComponent(Director::getInstance()->getVisibleSize()))->enableDesignScaleIgnoring());
    desc.fontName = _fontname;
    desc.fontSize = _fontsize;
    field = ax::Label::create();
    SELF addChild(field);
    text = _text;
    size = _size;
    wrap = _wrap;
    onFontScaleUpdate(_UiScale);
    update(0);
}

void CUI::Label::enableOutline()
{
    hasOutline = true;
}

void CUI::Label::update(f32 dt) {
    auto dSize = getDynamicContentSize();
    setContentSize(dSize * (_UiScale * field->getScale()) + getUiPadding());
}

bool CUI::Label::hover(ax::Vec2 mouseLocationInView, Camera* cam)
{
    return false;
}

void CUI::Label::focus()
{
}

void CUI::Label::defocus()
{
}

void CUI::Label::onEnable()
{
    auto fade = FadeTo::create(0.1f, 255);
    auto tint = TintTo::create(0.1f, Color3B::WHITE);
    field->runAction(tint);
}

void CUI::Label::onDisable()
{
    defocus();
    auto fade = FadeTo::create(0.1f, 100);
    auto tint = TintTo::create(0.1f, Color3B::GRAY);
    field->runAction(tint);
}

bool CUI::Label::press(ax::Vec2 mouseLocationInView, Camera* cam)
{
    return false;
}

bool CUI::Label::release(cocos2d::Vec2 mouseLocationInView, Camera* cam)
{
    return false;
}

Size CUI::Label::getDynamicContentSize()
{
    auto dSize = field->getContentSize() - Vec2(0, (_ForceOutline ? _PmtFontOutline * 1 : 0));
    if (size.x == 0)
        return dSize / _UiScale;
    auto calc = dSize.x / _UiScale * (size.x / dSize.x);
    return Size(size.x > dSize.x ? dSize.x / _UiScale : calc, dSize.y / _UiScale);
}

void CUI::Label::onFontScaleUpdate(float scale)
{
    field->initWithTTF(ShapingEngine::render(text), desc.fontName, desc.fontSize * _PmtFontScale * scale, {0, 0}, hAlignment, vAlignment);
    if (wrap != 0) {
        field->setDimensions(wrap, field->getContentSize().y);
        field->setOverflow(ax::Label::Overflow::RESIZE_HEIGHT);
    }
    field->setHorizontalAlignment(hAlignment);
    field->setVerticalAlignment(vAlignment);
    if (hasOutline || _ForceOutline)
        field->enableOutline(Color4B::BLACK, _PmtFontOutline * scale);
    field->getFontAtlas()->setAliasTexParameters();
    field->updateContent();
}

void CUI::Label::updatePositionAndSize()
{
    auto dSize = field->getContentSize() * Vec2(1.0, 1.0 / (_ForceOutline ? _PmtFontOutline * 2 : 1));
    if (dSize.x > size.x && size.x != 0)
        field->setScale(size.x / dSize.x / _UiScale);
    else 
        field->setScale(1.0 / _UiScale);
}

void CUI::Label::notifyLayout()
{
    GUI::notifyLayout();
    updatePositionAndSize();
}

void CUI::Label::setString(std::wstring _text)
{
    field->setString(ShapingEngine::Helper::narrow(text = _text));
    updatePositionAndSize();
}

void CUI::Label::setString(std::string _text)
{
    text = Strings::widen(_text);
    setString(text);
}

CUI::Label::~Label()
{
    LOG_RELEASE;
}
