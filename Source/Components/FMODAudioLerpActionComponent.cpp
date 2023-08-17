#include "FMODAudioLerpActionComponent.h"

void FMODAudioLerpActionComponent::onAdd()
{
}

void FMODAudioLerpActionComponent::update(F32 dt) {
    ref->setVolume(actionNode->getPositionX());
}

FMODAudioLerpActionComponent::FMODAudioLerpActionComponent(Node* _owner) {
    actionNode = Node::create();
    _owner->addChild(actionNode);
    setName(__func__);
    setEnabled(true);
    setOwner(actionNode);
    owner = _owner;
}

FMODAudioLerpActionComponent* FMODAudioLerpActionComponent::initComponent(FMOD::Channel* sound, F32 _duration, F32 _start, F32 _end) {
    ref = sound;
    getOwner()->setPositionX(start = _start);
    auto actionTo = MoveTo::create(_duration, Vec2(end = _end, 0));
    auto ease = EaseQuadraticActionIn::create(actionTo);
    auto finish = CallFunc::create([&]() {
        ref->setVolume(getOwner()->getPositionX());
    actionNode->removeComponent(this);
    owner->removeChild(actionNode);
    delete this;
        });
    action = Sequence::create(actionTo, finish, nullptr);
    getOwner()->runAction(action);
    actionNode->addComponent(this);
    return this;
}
