#include "uiDiscardPanel.h"
#include "2d/CCTweenFunction.h"
#include <regex>

CUI::DiscardPanel* CUI::DiscardPanel::create(BorderLayout border, BorderContext context)
{
    CUI::DiscardPanel* ret = new CUI::DiscardPanel();
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

void CUI::DiscardPanel::init(const std::wstring& header, const std::wstring& placeholder_or_text, DiscardButtons buttons, DiscardType type)
{
    setBackgroundDim();

    stack = CUI::Container::create();
    stack->setBorderLayout(BorderLayout::CENTER, BorderContext::SCREEN_SPACE);
    stack->setLayout(CUI::FlowLayout(
        CUI::FlowLayoutSort::SORT_VERTICAL,
        CUI::FlowLayoutDirection::STACK_CENTER,
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

    auto label = CUI::Label::create();
    label->init(header, 16 * (type == DiscardType::INPUT ? 2 : 1));
    //label->_padding = { 100, 0 };
    stack->addChild(TO_CONTAINER(label));

    //auto separator = CUI::Separator::create({0, 0});
    //stack->addChild(TO_CONTAINER(separator));

    if (type == DiscardType::INPUT) {
        auto textField = CUI::TextField::create();
        textField->init(placeholder_or_text, 16, { 200, 40 });
        stack->addChild(TO_CONTAINER(textField));
    }
    else {
        auto text = CUI::Label::create();
        text->init(placeholder_or_text, 16 * 1, { 0, 0 }, 0);
        text->hAlignment = ax::TextHAlignment::CENTER;
        stack->addChild(TO_CONTAINER(text));
    }

    auto buttonStack = CUI::Container::create();
    buttonStack->setLayout(CUI::FlowLayout(
        CUI::FlowLayoutSort::SORT_HORIZONTAL,
        CUI::FlowLayoutDirection::STACK_CENTER,
        100
    ));

    //separator = CUI::Separator::create({ 0, 10 });
    //stack->addChild(TO_CONTAINER(separator));

    discardCallback = [&](CUI::Button* target)
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

    auto cancel = CUI::Button::create();
    cancel->init(cancelText, 16);
    buttonStack->addChild(cancel);

    cancel->_callback = discardCallback;

    if (buttons != DiscardButtons::OKAY) {
        auto okayText = (buttons == DiscardButtons::OKAY_ABORT ? L"OKAY" :
            (buttons == DiscardButtons::YES_NO) ? L"YES" :
            (buttons == DiscardButtons::SUBMIT_CANCEL ? L"SUBMIT" : L""));

        auto okay = eventButton = CUI::Button::create();
        okay->init(okayText, 16);
        buttonStack->addChild(okay);
    }

    stack->addChild(TO_CONTAINER(buttonStack));
}

bool CUI::DiscardPanel::hover(cocos2d::Vec2 mouseLocationInView, cocos2d::Camera* cam)
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

bool CUI::DiscardPanel::press(cocos2d::Vec2 mouseLocationInView, cocos2d::Camera* cam)
{
    Container::press(mouseLocationInView, cam);
    return true;
}

void CUI::DiscardPanel::keyPress(EventKeyboard::KeyCode keyCode)
{
    if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
        discardCallback(nullptr);
    if (keyCode == EventKeyboard::KeyCode::KEY_ENTER) {
        if (eventButton && eventButton->_callback)
            eventButton->_callback(nullptr);
        discardCallback(nullptr);
    }
}

void CUI::DiscardPanel::keyRelease(EventKeyboard::KeyCode keyCode)
{
}

void CUI::DiscardPanel::update(f32 dt)
{
}

Size CUI::DiscardPanel::getDynamicContentSize()
{
    return Size(0, 0);
}
