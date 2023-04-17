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

void CustomUi::Label::init(std::wstring _text, i32 _fontsize, Size _size)
{
    init(
        _text,
        "fonts/bitsy-font-with-arabic.ttf"sv,
        _fontsize,
        _size
    );
}

void CustomUi::Label::init(std::wstring _text, std::string_view _fontname, i32 _fontsize, Size _size)
{
    desc.fontName = _fontname;
    desc.fontSize = _fontsize;
    addComponent((new UiRescaleComponent(Director::getInstance()->getVisibleSize()))->enableDesignScaleIgnoring());
    field = ax::Label::createWithTTF(ShapingEngine::render(_text), _fontname, _fontsize * _UiScale);
    addChild(field);
    size = _size;
}

void CustomUi::Label::update(f32 dt) {
    auto dSize = getDynamicContentSize();
    setContentSize(dSize);
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
    auto calc = dSize.x / _UiScale * (size.x / dSize.x);
    return Size(size.x > dSize.x ? dSize.x / _UiScale : calc, dSize.y / _UiScale);
}

void CustomUi::Label::onFontScaleUpdate(float scale)
{
    field->initWithTTF(field->getString(), desc.fontName, desc.fontSize * _PmtFontScale * scale);
    field->enableOutline(Color4B::BLACK, 8);
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
    field->setString(Strings::narrow(_text));
    updatePositionAndSize();
}
