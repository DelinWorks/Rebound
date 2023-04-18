#include "uiDiscardPanel.h"
#include "2d/CCTweenFunction.h"
#include <regex>

CustomUi::DiscardPanel* CustomUi::DiscardPanel::create(BorderLayout border, BorderContext context)
{
    CustomUi::DiscardPanel* ret = new CustomUi::DiscardPanel();
    if (((Node*)ret)->init())
    {
        ret->setBorderLayout(border, context);
        ret->setBorderLayoutAnchor();
        ret->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(ret);
    }
    return ret;
}

void CustomUi::DiscardPanel::init(const std::wstring& header, const std::wstring& placeholder)
{
    setBackgroundSprite();
    setBackgroundDim();

    auto parentStack = CustomUi::Container::create();
    parentStack->setBorderLayout(BorderLayout::CENTER, BorderContext::SCREEN_SPACE);
    parentStack->setLayout(CustomUi::FlowLayout(
        CustomUi::FlowLayoutSort::SORT_VERTICAL,
        CustomUi::FlowLayoutDirection::STACK_CENTER,
        100
    ));
    parentStack->setBorderLayoutAnchor();
    parentStack->setMargin({ 30, 30 });
    addChild(parentStack);

    auto label = CustomUi::Label::create();
    label->init(header, 16 * 2);
    //label->_padding = { 100, 0 };
    parentStack->addChild(label);

    auto textField = CustomUi::TextField::create();
    textField->init(placeholder, 16, { 200, 40 });
    parentStack->addChild(textField);

    auto separator = CustomUi::Separator::create();
    separator->setContentSize({ 1, 1 });
    parentStack->addChild(separator);

    auto buttonStack = CustomUi::Container::create();
    buttonStack->setLayout(CustomUi::FlowLayout(
        CustomUi::FlowLayoutSort::SORT_HORIZONTAL,
        CustomUi::FlowLayoutDirection::STACK_CENTER,
        100
    ));

    auto cancel = CustomUi::Button::create();
    cancel->init(L"ABORT", 16, { 0, 40 });
    buttonStack->addChild(cancel);

    cancel->_callback = [&](CustomUi::Button* target)
    {
        this->removeFromParent();
    };

    auto okay = CustomUi::Button::create();
    okay->init(L"OKAY", 16, { 0, 40 });
    buttonStack->addChild(okay);

    parentStack->addChild(buttonStack);
}

bool CustomUi::DiscardPanel::hover(cocos2d::Vec2 mouseLocationInView, cocos2d::Camera* cam)
{
    Container::hover(mouseLocationInView, cam);
    return true;
}

bool CustomUi::DiscardPanel::press(cocos2d::Vec2 mouseLocationInView, cocos2d::Camera* cam)
{
    Container::press(mouseLocationInView, cam);
    return true;
}

void CustomUi::DiscardPanel::update(f32 dt)
{
}

Size CustomUi::DiscardPanel::getDynamicContentSize()
{
    return Size(0, 0);
}
