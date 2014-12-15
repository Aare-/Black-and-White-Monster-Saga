#include "GameOverScene.h"

GameOver* GameOver::createScene() {
	return GameOver::create();
} 

bool GameOver::init() {
	if (!cocos2d::Scene::init())
		return false;

	return true;
}

GameOver::~GameOver() {

}