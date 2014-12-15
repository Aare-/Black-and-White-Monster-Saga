#ifndef _MAINMENUSCENE_H_
#define _MAINMENUSCENE_H_

#include "cocos2d.h"
#include "Sub.h"
#include "GameScene.h"

class MainMenu : public cocos2d::Scene {
public:
	static MainMenu* createScene();

	CREATE_FUNC(MainMenu);
	virtual bool init();
	MainMenu() : gameScene(NULL), 
				 backgroundPattern(NULL),
				 mainMenu(NULL),
				 hiscoreMenu(NULL),
				 aboutMenu(NULL),
				 activeMenu(NULL){}
	~MainMenu();

	void onEnter();
	void onExit();
	void onUpdate(float);

	void playPressed(cocos2d::Ref*);
	void aboutPressed(cocos2d::Ref*);
	void hiscoresPressed(cocos2d::Ref*);
	void backFromAbout(cocos2d::Ref*);

protected:
	Game* gameScene;
	Pattern* backgroundPattern;
	
	cocos2d::Sprite *monsterHead;	

	cocos2d::Menu *mainMenu;	
	cocos2d::Menu *aboutMenu;
	cocos2d::Menu *hiscoreMenu;

	cocos2d::Menu *activeMenu;

	void createMainMenu();
	void createAboutMenu();
	void createHiscoresMenu();

	cocos2d::Label* hiscore[10];

};

#endif
