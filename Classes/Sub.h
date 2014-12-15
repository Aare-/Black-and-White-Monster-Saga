#ifndef __SUB_H__
#define __SUB_H__

#include "cocos2d.h"

#define DESIGN_W 640
#define DESIGN_H 960
#define DEBUG false

class Sub {
public:
	static cocos2d::Sprite* getSprite(std::string file_name) {		
		cocos2d::SpriteFrameCache *sfc = cocos2d::SpriteFrameCache::sharedSpriteFrameCache();
		cocos2d::SpriteFrame *frame = sfc->getSpriteFrameByName(file_name);		

		if (frame != NULL)
			return cocos2d::Sprite::createWithSpriteFrame(frame);
		else
			return cocos2d::Sprite::create(file_name);
	}
};

class PlayerProgress {
public:
	static void getHiscores(int *hiscoresArr) {		
		for (int i = 0; i < 10; i++){
			auto key = "player_score" + std::to_string(i);
			hiscoresArr[i] = cocos2d::UserDefault::getInstance()->getIntegerForKey(key.c_str());
		}		
	}

	static void tryToInsertNewHiscore(int newScore) {
		int hiscores[10];
		getHiscores(hiscores);

		int prevVal = hiscores[0];
		for (int i = 0; i < 10; i++) {
			auto key = "player_score" + std::to_string(i);
			
			
			if (newScore > hiscores[i])
				std::swap(hiscores[i], newScore);				
			cocos2d::UserDefault::getInstance()->setIntegerForKey(key.c_str(), hiscores[i]);

			CCLOG(key.c_str());
		}	
		cocos2d::UserDefault::getInstance()->flush();
	}

	
};

#endif