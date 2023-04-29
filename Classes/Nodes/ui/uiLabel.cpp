#include "uiLabel.h"

CustomUi::Label* CustomUi::Label::create()
{
    CustomUi::Label* ret = new CustomUi::Label();
    if (((Node*)ret)->init())
    {
        ret->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(ret);
    }
    return ret;
}

void CustomUi::Label::init(std::wstring _text, i32 _fontsize, Size _size, float _wrap)
{
    init(
        _text,
        "fonts/bitsy-font-with-arabic.ttf"sv,
        _fontsize,
        _size,
        _wrap
    );
}

void CustomUi::Label::init(std::wstring& _text, std::string_view _fontname, i32 _fontsize, Size _size, float _wrap)
{
    desc.fontName = _fontname;
    desc.fontSize = _fontsize;
    addComponent((new UiRescaleComponent(Director::getInstance()->getVisibleSize()))->enableDesignScaleIgnoring());
    field = ax::Label::create();
    addChild(field);
    text = _text;
    size = _size;
    wrap = _wrap;
}

void CustomUi::Label::enableOutline()
{
    hasOutline = true;
}

void CustomUi::Label::update(f32 dt) {
    auto dSize = getDynamicContentSize();
    setContentSize(dSize * (_UiScale * field->getScale()) + _padding);
}

bool CustomUi::Label::hover(ax::Vec2 mouseLocationInView, Camera* cam)
{
    return false;
}

void CustomUi::Label::focus()
{
}

void CustomUi::Label::defocus()
{
}

void CustomUi::Label::onEnable()
{
    auto fade = FadeTo::create(0.1f, 255);
    auto tint = TintTo::create(0.1f, Color3B::WHITE);
    field->runAction(tint);
}

void CustomUi::Label::onDisable()
{
    defocus();
    auto fade = FadeTo::create(0.1f, 100);
    auto tint = TintTo::create(0.1f, Color3B::GRAY);
    field->runAction(tint);
}

bool CustomUi::Label::press(ax::Vec2 mouseLocationInView, Camera* cam)
{
    return false;
}

bool CustomUi::Label::release(cocos2d::Vec2 mouseLocationInView, Camera* cam)
{
    return false;
}

Size CustomUi::Label::getDynamicContentSize()
{
    auto& dSize = field->getContentSize();
    if (size.x == 0)
        return dSize / _UiScale;
    auto calc = dSize.x / _UiScale * (size.x / dSize.x);
    return Size(size.x > dSize.x ? dSize.x / _UiScale : calc, dSize.y / _UiScale);
}

void CustomUi::Label::onFontScaleUpdate(float scale)
{
    field->initWithTTF(ShapingEngine::render(text), desc.fontName, desc.fontSize * _PmtFontScale * scale, {0, 0}, hAlignment, vAlignment);
    if (wrap != 0) {
        field->setDimensions(wrap, field->getContentSize().y);
        field->setOverflow(ax::Label::Overflow::RESIZE_HEIGHT);
    }
    field->setHorizontalAlignment(hAlignment);
    field->setVerticalAlignment(vAlignment);
    if (hasOutline)
        field->enableOutline(Color4B::BLACK, 2);
    field->getFontAtlas()->setAliasTexParameters();
}

void CustomUi::Label::updatePositionAndSize()
{
    auto& dSize = field->getContentSize();
    if (dSize.x > size.x && size.x != 0)
        field->setScale(size.x / dSize.x / _UiScale);
    else 
        field->setScale(1.0 / _UiScale);
}

void CustomUi::Label::notifyLayout()
{
    GUI::notifyLayout();
    updatePositionAndSize();
}

void CustomUi::Label::setString(std::wstring _text)
{
    field->setString(ShapingEngine::render(text = _text));
    updatePositionAndSize();
}

void CustomUi::Label::setString(std::string _text)
{
    text = Strings::widen(_text);
    setString(text);
}
