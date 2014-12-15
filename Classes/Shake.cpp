#include "Shake.h"

float fgRangeRand(float min, float max) {
	float rnd = ((float)rand() / (float)RAND_MAX);
	return rnd*(max - min) + min;
}

Shake* Shake::actionWithDuration(float d, float strength){
  return actionWithDuration( d, strength, strength );
}

Shake* Shake::actionWithDuration(float duration, float strengthX, float strengthY) {
	Shake* p_action = new Shake();
	p_action->initWithDuration(duration, strengthX, strengthY);
	p_action->autorelease();

	return p_action;
}

bool Shake::initWithDuration(float duration, float strength_x, float strength_y) {
	if (cocos2d::ActionInterval::initWithDuration(duration)) {
		strX = strength_x;
		strY = strength_y;
		return true;
	}

	return false;
}

void Shake::update(float time) {
	float randx = fgRangeRand( -strX, strX );
	float randy = fgRangeRand( -strY, strY ); 
  
	_target->setPosition(initX + randx, initY + randy);
}

void Shake::startWithTarget(cocos2d::Node *pTarget) {
	cocos2d::ActionInterval::startWithTarget( pTarget );
	
	initX = pTarget->getPosition().x;
	initY = pTarget->getPosition().y;
}

void Shake::stop(void) {	
	_target->setPosition( initX, initY);

	cocos2d::ActionInterval::stop();
}

cocos2d::ActionInterval* Shake::reverse() const {
	return Shake::actionWithDuration(_duration, strX, strY);
}

cocos2d::ActionInterval* Shake::clone() const {
	return Shake::actionWithDuration(_duration, strX, strY);
}