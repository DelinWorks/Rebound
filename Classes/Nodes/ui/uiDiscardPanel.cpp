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

void CustomUi::DiscardPanel::init(const std::wstring& header, const std::wstring& placeholder_or_text, DiscardButtons buttons, DiscardType type)
{
    setBackgroundDim();

    stack = CustomUi::Container::create();
    stack->setBorderLayout(BorderLayout::CENTER, BorderContext::SCREEN_SPACE);
    stack->setLayout(CustomUi::FlowLayout(
        CustomUi::FlowLayoutSort::SORT_VERTICAL,
        CustomUi::FlowLayoutDirection::STACK_CENTER,
        10
    ));
    stack->setBorderLayoutAnchor();
    stack->setMargin({ 30, 15 });
    stack->setBackgroundSprite();
    addChild(stack);

    _bgDim->setOpacity(0);
    _bgDim->runAction(FadeTo::create(0.08, 100));

    stack->setScale(0);
    stack->runAction(EaseSineOut::create(ScaleTo::create(0.08, 1)));

    auto label = CustomUi::Label::create();
    label->init(header, 16 * (type == DiscardType::INPUT ? 2 : 1));
    //label->_padding = { 100, 0 };
    stack->addChild(TO_CONTAINER(label));

    //auto separator = CustomUi::Separator::create({0, 0});
    //stack->addChild(TO_CONTAINER(separator));

    if (type == DiscardType::INPUT) {
        auto textField = CustomUi::TextField::create();
        textField->init(placeholder_or_text, 16, { 200, 40 });
        stack->addChild(TO_CONTAINER(textField));
    }
    else {
        auto text = CustomUi::Label::create();
        text->init(placeholder_or_text, 16 * 1, { 0, 0 }, 0);
        text->hAlignment = ax::TextHAlignment::CENTER;
        stack->addChild(TO_CONTAINER(text));
    }

    auto buttonStack = CustomUi::Container::create();
    buttonStack->setLayout(CustomUi::FlowLayout(
        CustomUi::FlowLayoutSort::SORT_HORIZONTAL,
        CustomUi::FlowLayoutDirection::STACK_CENTER,
        100
    ));

    //separator = CustomUi::Separator::create({ 0, 10 });
    //stack->addChild(TO_CONTAINER(separator));

    discardCallback = [&](CustomUi::Button* target)
    {
        Modal::popSelf();

        stack->disable();

        _bgDim->setOpacity(100);
        _bgDim->runAction(FadeTo::create(0.08, 0));

        stack->runAction(
            Sequence::create(
                EaseSineIn::create(ScaleTo::create(0.08, 0)),
                CallFunc::create([&] { stack->getParent()->removeFromParent(); }),
                _NOTHING
            )
        );
    };

    auto cancelText = (buttons == DiscardButtons::OKAY_ABORT ? L"ABORT" :
        (buttons == DiscardButtons::YES_NO) ? L"NO" :
        (buttons == DiscardButtons::SUBMIT_CANCEL ? L"CANCEL" : L"OKAY"));

    auto cancel = CustomUi::Button::create();
    cancel->init(cancelText, 16);
    buttonStack->addChild(cancel);

    cancel->_callback = discardCallback;

    if (buttons != DiscardButtons::OKAY) {
        auto okayText = (buttons == DiscardButtons::OKAY_ABORT ? L"OKAY" :
            (buttons == DiscardButtons::YES_NO) ? L"YES" :
            (buttons == DiscardButtons::SUBMIT_CANCEL ? L"SUBMIT" : L""));

        auto okay = eventButton = CustomUi::Button::create();
        okay->init(okayText, 16);
        buttonStack->addChild(okay);
    }

    stack->addChild(TO_CONTAINER(buttonStack));
}

bool CustomUi::DiscardPanel::hover(cocos2d::Vec2 mouseLocationInView, cocos2d::Camera* cam)
{
    if (_modalStack.size() == 0) // Object was popped before
        return false;

    Container::hover(mouseLocationInView, cam);
    if (_modalStack.top() == this) {
        notifyFocused(this, !isUiFocused(), true);
        return true;
    }
    else return false;
}

bool CustomUi::DiscardPanel::press(cocos2d::Vec2 mouseLocationInView, cocos2d::Camera* cam)
{
    Container::press(mouseLocationInView, cam);
    return true;
}

void CustomUi::DiscardPanel::keyPress(EventKeyboard::KeyCode keyCode)
{
    if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
        discardCallback(nullptr);
    if (keyCode == EventKeyboard::KeyCode::KEY_ENTER) {
        if (eventButton && eventButton->_callback)
            eventButton->_callback(nullptr);
        discardCallback(nullptr);
    }
}

void CustomUi::DiscardPanel::keyRelease(EventKeyboard::KeyCode keyCode)
{
}

void CustomUi::DiscardPanel::update(f32 dt)
{
}

Size CustomUi::DiscardPanel::getDynamicContentSize()
{
    return Size(0, 0);
}
