#include "uiEmptyPanel.h"
#include "2d/TweenFunction.h"
#include <regex>

CUI::EmptyPanel* CUI::EmptyPanel::create(BorderLayout border, BorderContext context)
{
    CUI::EmptyPanel* ret = new CUI::EmptyPanel();
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

void CUI::EmptyPanel::init(const std::wstring& header)
{
    addComponent((new UiRescaleComponent(Director::getInstance()->getVisibleSize()))->enableDesignScaleIgnoring());
    setBackgroundDim();

    stack = CUI::Container::create();
    stack->setBorderLayout(BorderLayout::CENTER, BorderContext::PARENT);
    stack->setLayout(CUI::FlowLayout(
        CUI::FlowLayoutSort::SORT_VERTICAL,
        CUI::FlowLayoutDirection::STACK_CENTER,
        30
    ));

    stack->setBorderLayoutAnchor();
    stack->setMargin({ 10, 5 });
    stack->setBackgroundSprite();
    addChild(stack);

    _bgDim->setOpacity(0);
    _bgDim->runAction(FadeTo::create(0.08, 160));

    stack->setScale(0);
    stack->runAction(EaseSineOut::create(ScaleTo::create(0.08, 1)));

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

    auto label = CUI::Label::create();
    label->init(header, TTFFS * 2);
    label->setUiPadding(V2D(100, 0));
    auto cc1 = TO_CONTAINER(label);
    //cc1->setStaticX();
    stack->addChild(cc1);

    auto huhBtn = CUI::Button::create();
    huhBtn->initIcon("editor_what");
    auto cc3 = TO_CONTAINER(huhBtn);
    cc3->setBorderLayoutAnchor(BorderLayout::CENTER);
    cc3->setTag(YOURE_NOT_WELCOME_HERE);

    auto exitBtn = CUI::Button::create();
    exitBtn->initIcon("editor_x");
    auto cc4 = TO_CONTAINER(exitBtn);
    cc4->setBorderLayoutAnchor(BorderLayout::CENTER);
    cc4->setTag(YOURE_NOT_WELCOME_HERE);
    exitBtn->_callback = discardCallback;

    stack->addChild(cc3);
    stack->addChild(cc4);

    cont = CUI::Container::create();
    //cont->_contentSizeDebugColor = Color3B::GREEN;
    stack->addChild(cont);

    //cc1->setConstraint(ContentSizeConstraint(cc2, V2D::ZERO, false, false, false, V2D{180, 0}));

    cc3->setConstraint(DependencyConstraint(stack, BorderLayout::TOP_LEFT));
    cc4->setConstraint(DependencyConstraint(stack, BorderLayout::TOP_RIGHT));
}

bool CUI::EmptyPanel::hover(V2D mouseLocationInView, Camera* cam)
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

bool CUI::EmptyPanel::press(V2D mouseLocationInView, Camera* cam)
{
    Container::press(mouseLocationInView, cam);
    return true;
}

void CUI::EmptyPanel::keyPress(EventKeyboard::KeyCode keyCode)
{
    if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE || ANY_ENTER(keyCode))
        discardCallback(nullptr);
}

void CUI::EmptyPanel::keyRelease(EventKeyboard::KeyCode keyCode)
{
}

void CUI::EmptyPanel::update(F32 dt)
{
}

Size CUI::EmptyPanel::getDynamicContentSize()
{
    return Size(0, 0);
}

CUI::EmptyPanel::~EmptyPanel()
{
    LOG_RELEASE;
}
