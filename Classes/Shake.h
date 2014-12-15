#ifndef __SHAKE_H__
#define __SHAKE_H__

#include "cocos2d.h"

class Shake : public cocos2d::ActionInterval {
public:
	Shake() : strX(0), strY(0), initX(0), initY(0){};
	~Shake(){};
  
	static Shake* actionWithDuration(float delta, float strength);  
	static Shake* actionWithDuration(float d, float strength_x, float strength_y );

	bool initWithDuration(float d, float strength_x, float strength_y );

	virtual void startWithTarget(cocos2d::Node *pTarget);
	virtual void update(float time);
	virtual void stop(void);
	
	cocos2d::ActionInterval* reverse() const override;
	cocos2d::ActionInterval* clone() const override;

protected:  
	float initX, initY;
    float strX, strY;
};

#endif //__SHAKE_H__