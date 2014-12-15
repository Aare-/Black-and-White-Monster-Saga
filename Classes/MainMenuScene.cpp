#include "MainMenuScene.h"


MainMenu* MainMenu::createScene() {
	return MainMenu::create();
} 

bool MainMenu::init() {
	if (!cocos2d::Scene::init())
		return false;

	gameScene = Game::createScene();
	gameScene->retain();

	backgroundPattern = new Pattern("pattern.jpg");
	addChild(backgroundPattern->getPatternSprite());

	monsterHead = Sub::getSprite("monsterhead_closed.png");
	addChild(monsterHead);

	return true;
}

MainMenu::~MainMenu() {
	if (gameScene != NULL)
		gameScene->release();	
	if (backgroundPattern != NULL)
		delete backgroundPattern;	
}

void MainMenu::onEnter(){
	cocos2d::Scene::onEnter();	

	schedule(schedule_selector(MainMenu::onUpdate));
	createMainMenu();

	backgroundPattern->getPatternSprite()->setOpacity(0);
	backgroundPattern->getPatternSprite()->runAction(cocos2d::EaseSineOut::create(cocos2d::FadeIn::create(0.5f)));

	monsterHead->setPosition(DESIGN_W / 2, DESIGN_H + monsterHead->getContentSize().height * 2);
	monsterHead->runAction(cocos2d::Sequence::create(
		cocos2d::DelayTime::create(0.5f),
		cocos2d::EaseBackOut::create(cocos2d::MoveTo::create(0.8f, cocos2d::Vec2(DESIGN_W / 2, DESIGN_H - monsterHead->getContentSize().height * 0.5f - 100))),
		NULL
		));

	activeMenu->setOpacity(0);
	activeMenu->runAction(cocos2d::Sequence::create(
		cocos2d::DelayTime::create(0.5f),
		cocos2d::EaseSineIn::create(cocos2d::FadeIn::create(0.5f)),
		NULL
	));
}

void MainMenu::onExit() {
	cocos2d::Scene::onExit();

	unschedule(schedule_selector(MainMenu::onUpdate));	

	if (activeMenu != NULL) {
		activeMenu->removeFromParent();
		activeMenu = NULL;
	}
}

void MainMenu::onUpdate(float delta) {
	backgroundPattern->update(delta);
}

void MainMenu::playPressed(Ref* sender) {
	auto *map = new cocos2d::Map<std::string, cocos2d::String*>();

	map->insert("item_types", cocos2d::String::create("4"));
	map->insert("board_width", cocos2d::String::create("6"));
	map->insert("board_height", cocos2d::String::create("9"));

	map->insert("board_tile_width", cocos2d::String::create("100"));
	map->insert("board_tile_height", cocos2d::String::create("100"));

	gameScene->setGameConfig(map);
	cocos2d::Director::getInstance()->pushScene(gameScene);
}

void MainMenu::aboutPressed(cocos2d::Ref* sender) {	
	createAboutMenu();	
}

void MainMenu::hiscoresPressed(cocos2d::Ref* sender) {		
	createHiscoresMenu();
}

void MainMenu::backFromAbout(cocos2d::Ref* sender) {	
	createMainMenu();
}

void MainMenu::createMainMenu() {
	if (activeMenu != NULL) {
		activeMenu->removeFromParent();
		activeMenu = NULL;
	}

	monsterHead->setVisible(true);

	auto playButton 
		= cocos2d::MenuItemImage::create("play.png", "play_pressed.png",
										 CC_CALLBACK_1(MainMenu::playPressed, this));
	playButton->setPosition(cocos2d::Vec2(DESIGN_W * 0.5f, 200));

	auto aboutButton 
		= cocos2d::MenuItemImage::create("about.png", "about_pressed.png",
										 CC_CALLBACK_1(MainMenu::aboutPressed, this));
	aboutButton->setPosition(cocos2d::Vec2((DESIGN_W - playButton->getContentSize().width) * 0.25f, 200));

	auto hiscoresButton 
		= cocos2d::MenuItemImage::create("hiscores.png", "hiscores_pressed.png",
										 CC_CALLBACK_1(MainMenu::hiscoresPressed, this));
	hiscoresButton->setPosition(cocos2d::Vec2(DESIGN_W - (DESIGN_W - playButton->getContentSize().width) * 0.25f, 200));

	mainMenu = cocos2d::Menu::create(playButton, aboutButton, hiscoresButton, NULL);
	mainMenu->setPosition(cocos2d::Vec2::ZERO);
	addChild(mainMenu);

	activeMenu = mainMenu;
}

void MainMenu::createAboutMenu() {
	if (activeMenu != NULL) {
		activeMenu->removeFromParent();
		activeMenu = NULL;
	}

	monsterHead->setVisible(false);

	auto backButton = cocos2d::MenuItemImage::create("replay.png", "replay_pressed.png", CC_CALLBACK_1(MainMenu::backFromAbout, this));
	backButton->setPosition(DESIGN_W / 2, 200);

	auto creditsSprite = cocos2d::Sprite::create("credits.png");
	creditsSprite->setPosition(DESIGN_W / 2, DESIGN_H / 2);

	aboutMenu = cocos2d::Menu::create(cocos2d::MenuItemSprite::create(creditsSprite, creditsSprite),
										backButton,
										NULL);

	aboutMenu->setPosition(cocos2d::Vec2::ZERO);
	addChild(aboutMenu);

	activeMenu = aboutMenu;
}

void MainMenu::createHiscoresMenu() {
	if (activeMenu != NULL) {
		activeMenu->removeFromParent();
		activeMenu = NULL;
	}

	monsterHead->setVisible(false);

	int hiscores[10];
	PlayerProgress::getHiscores(hiscores);	

	auto backButton = cocos2d::MenuItemImage::create("replay.png", "replay_pressed.png", CC_CALLBACK_1(MainMenu::backFromAbout, this));
	backButton->setPosition(DESIGN_W / 2, 200);

	hiscoreMenu = cocos2d::Menu::create(backButton, NULL);

	for (int i = 0; i < 10; i++){
		auto hiscore = cocos2d::Label::create("", "Arial", 30);
		hiscore->setPosition(DESIGN_W / 2, DESIGN_H - 125 - 50 * i);
		hiscore->setString(std::to_string(hiscores[i]));
		hiscore->setColor({ 0, 0, 0 });

		auto background = cocos2d::Sprite::create();
		background->setTextureRect(cocos2d::Rect(0, 0, hiscore->getContentSize().width + 20, hiscore->getContentSize().height));
		background->setPosition(hiscore->getPosition());

		hiscoreMenu->addChild(cocos2d::MenuItemSprite::create(background, background));
		hiscoreMenu->addChild(cocos2d::MenuItemLabel::create(hiscore));
	}

	hiscoreMenu->setPosition(cocos2d::Vec2::ZERO);
	addChild(hiscoreMenu);

	activeMenu = hiscoreMenu;
}