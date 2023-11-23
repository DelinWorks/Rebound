#pragma once

#include "axmol.h"
#include "custom_ui.h"
#include "uiContainer4Edge.h"
#include "uiButton.h"
#include "uiLabel.h"

namespace CUI
{
    class List : public Container {
    public:
        List(V2D _prefferedSize, bool rescalingAllowed = true);
        ~List();

        static CUI::List* create(V2D _prefferedSize, bool rescalingAllowed = true);

        void addElement(Container* container, int extendCoeff = 0);
        void removeElement(U32 index);
        void moveElement(U32 index, U32 newIndex);

        void update(F32 dt) override;

        virtual void mouseScroll(EventMouse* event) override;

        KeyboardModifierState _keyboardState;
        std::function<void(KeyboardModifierState&, EventKeyboard::KeyCode)> _keyPressCallback;
        void keyPress(EventKeyboard::KeyCode keyCode);
        void keyRelease(EventKeyboard::KeyCode keyCode);
        bool hover(V2D mouseLocationInView, cocos2d::Camera* cam);
        bool press(V2D mouseLocationInView, cocos2d::Camera* cam);
        bool release(V2D mouseLocationInView, cocos2d::Camera* cam);

        void calculateContentBoundaries() override;
        void updateLayoutManagers(bool recursive = false) override;

        bool scrollIgnoreFirst = true;
        U16 scrollToPromise = UINT16_MAX;
        void scrollToIndex(U16 index);

        CUI::Label* setEmptyText(std::wstring _text);

        const std::vector<Container*>& getElements() { return elements; };

        V2D prefferredListSize = V2D::ZERO;
        V2D ePos = V2D::ZERO;
    private:
        EventPassClippingNode* clipping;
        Container* elementCont;
        Container* scrollCont;
        CUI::Button* scrollKnob;
        float dtScroll = 0.0f;
        CUI::Button* upB;
        CUI::Button* downB;
        float vel = 0.0;
        V2D elemContPos = V2D::ZERO;
        float deltaScroll = 0.0;
        float deltaScroll2 = 0.0;
        std::vector<Container*> elements;
        bool isListDirty = false;
        bool isElementListDirty = false;
        ChangeValue<bool> scrollEnableState;
    };
}
