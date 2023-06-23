#include "LerpPropertyActionComponent.h"

void LerpPropertyActionComponent::onAdd() {
    //std::cout << "Action Started: " << getOwner()->_ID << std::endl;
}

void LerpPropertyActionComponent::update(f32 dt) {
    switch (type)
    {
    case 0:
        *ref_float = actionNode->getPositionX();
        break;
    case 1:
        *ref_int = actionNode->getPositionX();
        break;
    case 2:
        *ref_uint8 = actionNode->getPositionX();
        break;
    default:
        break;
    }
}

LerpPropertyActionComponent::LerpPropertyActionComponent(Node* _owner) {
    actionNode = Node::create();
    _owner->addChild(actionNode);
    setName(__func__);
    setEnabled(true);
    setOwner(_owner);
    owner = _owner;
}

LerpPropertyActionComponent* LerpPropertyActionComponent::initFloat(f32* _ref, f32 _duration, f32 _start, f32 _end, f32 _finish) {
    type = 0;
    ref_float = _ref;
    actionNode->setPositionX(start = _start);
    auto actionTo = MoveTo::create(_duration, Vec2(end = _end, _finish));
    auto finishFunc = CallFunc::create([&]() {
        *ref_float = actionNode->getPositionX();
    *ref_float = actionNode->getPositionY();
    owner->removeComponent(this);
    owner->removeChild(actionNode);
    this->release();
        });
    action = Sequence::create(actionTo, finishFunc, nullptr);
    actionNode->runAction(action);
    return this;
}
