#ifndef  _APP_DELEGATE_H_
#define  _APP_DELEGATE_H_

#include "cocos2d.h"
#include "GameScene.h"
#include "MainMenuScene.h"
#include "Sub.h"

class  AppDelegate : private cocos2d::Application {		
public:	

	AppDelegate();
	virtual ~AppDelegate();
    
    virtual bool applicationDidFinishLaunching();    
    virtual void applicationDidEnterBackground();    
    virtual void applicationWillEnterForeground();	

protected:
	MainMenu* mainMenuScene;
};

#endif

