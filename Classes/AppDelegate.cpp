#include "AppDelegate.h"

USING_NS_CC;

AppDelegate::AppDelegate() {}

AppDelegate::~AppDelegate() {}

bool AppDelegate::applicationDidFinishLaunching() {	
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();

    if(!glview) {
        glview = GLView::create("Black and White Saga");		
		glview->setFrameSize(DESIGN_W , DESIGN_H );
        director->setOpenGLView(glview);				
    }

	//load resources
	cocos2d::SpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("board.plist");

	srand(time(0));	
	Size screenSize = glview->getFrameSize();

	glview->setDesignResolutionSize(DESIGN_W, DESIGN_H, ResolutionPolicy::SHOW_ALL);
	director->setContentScaleFactor(1);
    
	director->setDisplayStats(DEBUG);
    director->setAnimationInterval(1.0 / 60);
    
	mainMenuScene = MainMenu::createScene();	
	director->runWithScene(mainMenuScene);

    return true;
}

void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();
}

void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();
}
