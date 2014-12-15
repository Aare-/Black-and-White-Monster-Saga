#ifndef _GAMEOVERSCENE_H_
#define _GAMEOVERSCENE_H_

#include "cocos2d.h"
#include "Sub.h"

class GameOver : public cocos2d::Scene {
public:
	static GameOver* createScene();

	CREATE_FUNC(GameOver);
	virtual bool init();
	~GameOver();

protected:

};

#endif
