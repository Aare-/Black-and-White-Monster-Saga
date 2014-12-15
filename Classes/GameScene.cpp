#include "GameScene.h"

// BoardTileType
std::vector<BoardTileType*> BoardTileType::boardTypes = std::vector<BoardTileType*>();

BoardTileType::BoardTileType(int setId, std::string name, float chance) {
	this->id = setId;
	this->name = name;
	this->chance = chance;	
}

std::string BoardTileType::getName() {
	return this->name;
}

int BoardTileType::getId() {
	return id;
}

int BoardTileType::getValue() {
	return 1;
}

float BoardTileType::getSpawnChance() {
	return chance;
}

// BoardTile
BoardTile* BoardTile::create(BPos pos) {
	BoardTile *pRet = new BoardTile();
	if (pRet && pRet->init(pos)) {
		pRet->autorelease();		

		return pRet;
	} else { 
		delete pRet; 
		pRet = NULL; 
		return NULL; 
	} 
}

bool BoardTile::init(BPos pos) {	
	if (!cocos2d::Node::init())
		return false;	

	pattern = -1;

	myPos = pos;	

	return true;
}

void BoardTile::spawn() {
	float sum = 0.0f;	
	BoardTileType *t = nullptr;	

	for (UINT i = 0; i < BoardTileType::boardTypes.size(); i++) {
		sum += BoardTileType::boardTypes.at(i)->getSpawnChance();
		if (CCRANDOM_0_1() <= BoardTileType::boardTypes.at(i)->getSpawnChance() / sum)
			t = BoardTileType::boardTypes[i];
	}	

	setType(t);		
	setPattern(ClippedPattern::getRandomPattern());

	image->setPosition(cocos2d::Vec2(BoardTilesLayer::tileSize.width * 0.5f, 
									 BoardTilesLayer::tileSize.height * 0.5f + DESIGN_H / 2));
	moveToFinalPos();
}

void BoardTile::moveToFinalPos(){
	float squeeze = 0.6f;
	image->runAction(cocos2d::EaseSineIn::create((cocos2d::MoveTo::create(0.7f, cocos2d::Vec2(BoardTilesLayer::tileSize.width * 0.5f, 
																							  BoardTilesLayer::tileSize.height * 0.5f)))));

	this->runAction(cocos2d::Sequence::create(cocos2d::DelayTime::create(0.6f),
											  cocos2d::EaseSineInOut::create(cocos2d::ScaleTo::create(0.30f, 1.0f, squeeze, 1.0f)),
											  cocos2d::EaseBounceOut::create(cocos2d::ScaleTo::create(0.60f, 1.0f, 1.0f, 1.0f)),
											  NULL));
}

void BoardTile::collect(const cocos2d::Vec2& monsterPos, cocos2d::Layer* effects) {

	auto targetPos = effects->convertToNodeSpace(monsterPos + cocos2d::Vec2(monsterPos.x < DESIGN_W / 2 ? 65.0f : -65.0f, -55.0f));

	cocos2d::Sprite* effectSprite = Sub::getSprite(type->getName() + ".png");		
	effectSprite->setPosition(getPosition() + cocos2d::Vec2(BoardTilesLayer::tileSize.width * 0.5f,
		BoardTilesLayer::tileSize.height * 0.5f));
	effects->addChild(effectSprite);
		
	effectSprite->runAction(
		cocos2d::Sequence::create(cocos2d::EaseSineInOut::create(cocos2d::MoveTo::create(0.4f, targetPos)),
									cocos2d::RemoveSelf::create(true),
									NULL));			

	effectSprite->runAction(cocos2d::Sequence::create(cocos2d::DelayTime::create(0.35f),
														cocos2d::EaseSineIn::create(cocos2d::ScaleTo::create(0.05f, 0.0f)),
														NULL));

	type = NULL;	
}

void BoardTile::collectWithoutEffect() {
	type = NULL;
}

void BoardTile::destroy(cocos2d::Layer* effects) {
	if (effects != NULL) {
		cocos2d::Sprite* effectSprite = Sub::getSprite(type->getName() + ".png");		
		effectSprite->setPosition(getPosition() + cocos2d::Vec2(BoardTilesLayer::tileSize.width * 0.5f, 
																BoardTilesLayer::tileSize.height * 0.5f));
		effects->addChild(effectSprite);

		effectSprite->runAction(
			cocos2d::Sequence::create(
				cocos2d::EaseCircleActionIn::create(
					cocos2d::MoveTo::create(1.0f, cocos2d::Vec2(effectSprite->getPosition().x + CCRANDOM_MINUS1_1() * 50.0f , -300.0f - CCRANDOM_0_1() * 20.0f))),
				cocos2d::RemoveSelf::create(true),
				NULL));		
	}

	type = NULL;
}

BoardTileType* BoardTile::getType() {
	return type;
}

void BoardTile::setType(BoardTileType* type) {
	this->type = type;	

	if (image) {
		CCLOG("not null");
		image->removeFromParent();
	} else {
		CCLOG("null");
		
	}

	image = Sub::getSprite(type->getName() + ".png");	
	image->setPosition(cocos2d::Vec2(BoardTilesLayer::tileSize.width * 0.5f, 
									 BoardTilesLayer::tileSize.height * 0.5f));

	addChild(image);
}

void BoardTile::simulateGravity(std::vector<std::vector<BoardTile*>> board) {
	int searchRow = myPos.row - 1;
	while (searchRow >= 0){
		if (board[myPos.col][searchRow]->getType() != NULL){			
			setType(board[myPos.col][searchRow]->getType());
			setPattern(board[myPos.col][searchRow]->getPattern());

			int posDiff = getRow() - board[myPos.col][searchRow]->getRow();

			image->setPosition(cocos2d::Vec2(BoardTilesLayer::tileSize.width * 0.5f, (posDiff + 0.5f) * BoardTilesLayer::tileSize.height));
			moveToFinalPos();

			board[myPos.col][searchRow]->collectWithoutEffect();

			break;
		}

		searchRow--;
	}

	if (searchRow < 0)
		spawn();	
}

int BoardTile::getCol() {
	return myPos.col;
}

int BoardTile::getRow() {
	return myPos.row;
}

int BoardTile::getPattern() {
	return pattern;
}

void BoardTile::setPattern(int newPattern) {			
	if (manager != NULL)
		manager->updatePatternForTile(this, newPattern);
	pattern = newPattern;
}

// Chain
Chain::Chain() {		
	items = new std::vector<BoardTile*>();		
}

Chain::~Chain() {
	delete items;	
}

int Chain::push(BoardTile* item) {
	if (item->getType() == NULL) 
		return ADDING_TO_CHAIN_FAILED;

	if (item->getNumberOfRunningActions() > 0)
		return ADDING_TO_CHAIN_FAILED;

	if (items->size() == 0)
		items->push_back(item);		
	else {
		BoardTile* lastItem = items->back();		

		//if (lastItem->getType() != item->getType()) 
		//	return ADDING_TO_CHAIN_FAILED;

		if (items->size() > 1 && item == items->at(items->size() - 2)) {
			pop();
			return REVERT_ADDING_TO_CHAIN;
		}		

		int xDiff = abs(item->getCol() - lastItem->getCol());
		int yDiff = abs(item->getRow() - lastItem->getRow());
		if (!((xDiff == 1 && yDiff == 0) || (xDiff == 0 && yDiff == 1) || (xDiff == 1 && yDiff == 1)))
			return ADDING_TO_CHAIN_FAILED;

		for (UINT i = 0; i < items->size(); i++)
			if (items->at(i) == item)
				return ADDING_TO_CHAIN_FAILED;

		items->push_back(item);
	}

	return ADDING_TO_CHAIN_SUCCEDED;
}

void Chain::pop() {
	items->pop_back();
}

int Chain::collect(const cocos2d::Vec2 &monsterPos, cocos2d::Layer* effects) {
	int score = 0;

	if (items->size() >= 3)
		if (isChainValid()){
			while (items->size() > 0) {
				score += items->back()->getType()->getValue();
				items->back()->collect(monsterPos, effects);
				items->pop_back();
			}
		} else {
			while (items->size() > 0) {				
				items->back()->destroy(effects);
				items->pop_back();
			}
			score = -1;
		}		
	else {
		items->clear();		
	}

	return score > 0 ? score * score : score;
}

void Chain::destroy() {
	while (items->size() > 0) {
		items->back()->collectWithoutEffect();
		items->pop_back();
	}
}

bool Chain::isChainValid() {
	int type = items->at(0)->getType()->getId();

	for (auto item : *items) {
		if (type != item->getType()->getId())
			return false;
	}

	return true;
}

BoardTile* Chain::getSecLastItem(){
	if (items->size() <= 1)
		return NULL;
	return (BoardTile*) items->at(items->size() - 2);
}

//Pattern
Pattern::Pattern(std::string path){
	patternSprite = cocos2d::Sprite::create(path, cocos2d::Rect(0, 0, std::max(DESIGN_W, DESIGN_H), std::max(DESIGN_W, DESIGN_H)));
	patternSprite->getTexture()->setTexParameters({ GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT });
	patternSprite->setPosition(cocos2d::Vec2(DESIGN_W / 2, DESIGN_H / 2));

	changeDirection();
	changeSpeed();

	movDirection = movTargetDirection;
	movSpeed = movTargetSpeed;
}

void Pattern::changeDirection() {	
	movTargetDirection = cocos2d::Vec2(1.0f, 0.0f).rotateByAngle(cocos2d::Vec2::ZERO, M_PI * 2 * CCRANDOM_0_1());
	changeDirDelay = CCRANDOM_0_1() * 4.5f + 0.5f;
}

void Pattern::changeSpeed() {
	movTargetSpeed = CCRANDOM_0_1() * 30.0f + 30.0f;
	changeSpeedDelay = CCRANDOM_0_1() * 4.5f + 0.5f;
}

cocos2d::Sprite* Pattern::getPatternSprite() {
	return patternSprite;
}

void Pattern::update(float delta) {
	movDirection += (movTargetDirection - movDirection) * 0.05f;
	movSpeed += (movTargetSpeed - movSpeed) * 0.05f;

	cocos2d::Rect rect = getPatternSprite()->getTextureRect();
	getPatternSprite()
		->setTextureRect(cocos2d::Rect(rect.origin.x + movDirection.x * movSpeed * delta,
									   rect.origin.y + movDirection.y * movSpeed * delta,
									   rect.size.width,
									   rect.size.height));

	changeDirDelay -= delta;
	changeSpeedDelay -= delta;
	if (changeDirDelay < 0.0f)
		changeDirection();
	if (changeSpeedDelay < 0.0f)
		changeSpeed();
}

// ClippedPattern
std::vector<float> ClippedPattern::_pChances;

int ClippedPattern::getRandomPattern() {
	int result = 0;
	int id = 0;
	float c = 0;	

	for (auto pattern : _pChances) {
		c += pattern;
		if (CCRANDOM_0_1() <= pattern / c)
			result = id;
		id++;
	}	

	return result;
}

ClippedPattern::ClippedPattern(cocos2d::Node *parent, std::string path, float chance) : Pattern(path) {		

	ClippedPattern::_pChances.push_back(chance);
	_chance = chance;
	
	cLayer = cocos2d::Layer::create();
	cNode = cocos2d::ClippingNode::create(cLayer);		

	cNode->addChild(getPatternSprite());	

	cNode->setAlphaThreshold(0.75f);	
	parent->addChild(cNode);			
}

ClippedPattern::~ClippedPattern(){}

float ClippedPattern::getChance() {
	return _chance;
}

cocos2d::Layer* ClippedPattern::getLayer() {
	return cLayer;
}

// BoardTilesLayer
BoardTilesLayer* BoardTilesLayer::create(Game* _game) {
	BoardTilesLayer *pRet = new BoardTilesLayer();
	if (pRet && pRet->init()) {
		pRet->autorelease();
		pRet->game = _game;

		return pRet;
	} else {
		delete pRet;
		pRet = NULL;
		return NULL;
	}
}

bool BoardTilesLayer::init(){
	if (!Layer::init())
		return false;
	
	tilesLayer = cocos2d::Layer::create();
	chainLayer = cocos2d::Layer::create();
	monsterLayer = cocos2d::Layer::create();
	effectsLayer = cocos2d::Layer::create();	

	addChild(tilesLayer);
	addChild(chainLayer);
	addChild(monsterLayer);
	addChild(effectsLayer);
	
	patterns.push_back(ClippedPattern(tilesLayer, "pattern.jpg", 1.0f));
	patterns.push_back(ClippedPattern(tilesLayer, "pattern.jpg", 1.0f));
	patterns.push_back(ClippedPattern(tilesLayer, "pattern.jpg", 1.0f));	

	initTouch();	

	return true;
}

void BoardTilesLayer::initTouch() {
	touchListener = cocos2d::EventListenerTouchOneByOne::create();	
	touchListener->retain();

	touchListener->onTouchBegan = [&](cocos2d::Touch* touch, cocos2d::Event* event) {
		if (blockI) return false;

		cocos2d::Node* child = findChildCollidingWithPos(touch->getLocation());

		if (child != NULL)
			if (chain.push((BoardTile*)child) == ADDING_TO_CHAIN_SUCCEDED) {
				cocos2d::Sprite* dot = Sub::getSprite("dot_line.png");
				dot->setPosition(cocos2d::Vec2(child->getPosition().x + 50, child->getPosition().y + 50));
				chainLayer->addChild(dot);
			}

		return true;
	};

	touchListener->onTouchMoved = [&](cocos2d::Touch* touch, cocos2d::Event* event) {
		if (blockI) return;

		cocos2d::Node* child = findChildCollidingWithPos(touch->getLocation());

		if (child != NULL) {
			int result = chain.push((BoardTile*)child);
			if (result == ADDING_TO_CHAIN_SUCCEDED) {
				cocos2d::Sprite* dot = Sub::getSprite("dot_line.png");
				dot->setPosition(cocos2d::Vec2(child->getPosition().x + 50, child->getPosition().y + 50));

				chainLayer->addChild(dot);

				BoardTile* secLast = chain.getSecLastItem();
				if (secLast != NULL) {
					int diffX = abs(secLast->getCol() - ((BoardTile*)child)->getCol());
					int diffY = abs(secLast->getRow() - ((BoardTile*)child)->getRow());

					cocos2d::Sprite* line;
					float rotation = CC_RADIANS_TO_DEGREES(cocos2d::Vec2(child->getPosition().x - secLast->getPosition().x,
						child->getPosition().y - secLast->getPosition().y).getAngle());
					if (diffX == diffY)
						line = Sub::getSprite("line_2.png");
					else {
						line = Sub::getSprite("line_1.png");
						rotation += 90.0f;
					}

					line->setPosition(
						cocos2d::Vec2((secLast->getPosition().x + child->getPosition().x) * 0.5f + BoardTilesLayer::tileSize.width * 0.5f,
									  (secLast->getPosition().y + child->getPosition().y) * 0.5f + BoardTilesLayer::tileSize.height * 0.5f));
					line->setRotation(rotation);

					chainLayer->addChild(line);
				}
			}
			else if (result == REVERT_ADDING_TO_CHAIN) {
				chainLayer->removeChild(chainLayer->getChildren().back());
				chainLayer->removeChild(chainLayer->getChildren().back());
			}
		}
	};

	touchListener->onTouchEnded = [&](cocos2d::Touch* touch, cocos2d::Event* event) {
		chainLayer->removeAllChildren();
		
		game->showMonster();

		int addScore = chain.collect(game->getMonsterPos(), effectsLayer);		
		game->handleScoreUpdate(addScore);
	};	

	touchListener->setSwallowTouches(true);	
}

cocos2d::Node* BoardTilesLayer::findChildCollidingWithPos(cocos2d::Vec2 pos) {	
	cocos2d::Vec2 centerPoint 
		= cocos2d::Vec2(0.0f + BoardTilesLayer::tileSize.width * 0.5f, 
						BoardTilesLayer::tileSize.height * 0.5f);

	for (auto pattern : patterns)
		for (auto child : pattern.getLayer()->getChildren())	
			if ((centerPoint - convertToNodeSpace(child->convertToNodeSpace(pos))).length() < BoardTilesLayer::tileSize.width * 0.4f)
				return child;	

	return NULL;
}

void BoardTilesLayer::registerTile(BoardTile* item) {	
	item->manager = this;
}

void BoardTilesLayer::unregisterTile(BoardTile* item) {
	item->removeFromParent();
	item->manager = NULL;
}

void BoardTilesLayer::updatePatternForTile(BoardTile* bt, int next) {		
	bt->retain();
	bt->removeFromParent();

	if (next >= 0)
		patterns.at(next).getLayer()->addChild(bt);

	bt->release();
}

void BoardTilesLayer::blockInput(bool block){
	blockI = block;
}

void BoardTilesLayer::start(){	
	chain.destroy();
	chainLayer->removeAllChildren();
}

void BoardTilesLayer::resume() {
	_eventDispatcher->addEventListenerWithFixedPriority(touchListener, 1);
	schedule(schedule_selector(BoardTilesLayer::onUpdate));
}

void BoardTilesLayer::pause() {
	unschedule(schedule_selector(BoardTilesLayer::onUpdate));
	_eventDispatcher->removeEventListener(touchListener);
}

void BoardTilesLayer::onUpdate(float delta) {
	for (auto pattern : patterns)		
		pattern.update(delta);	
}

cocos2d::Size BoardTilesLayer::tileSize = cocos2d::Size(100, 100);

//Madness Progress
bool MadnessProgress::init() {
	if (!cocos2d::Node::init())
		return false;

	progress = cocos2d::Sprite::create();
	progress->setPosition(DESIGN_W / 2, DESIGN_H / 2);
	progress->setTextureRect(cocos2d::Rect(0, 0, DESIGN_W, DESIGN_H));
	addChild(progress);

	background = cocos2d::Sprite::create();
	background->setPosition(DESIGN_W / 2, DESIGN_H / 2);
	background->setColor({ 0, 0, 0 });
	background->setTextureRect(cocos2d::Rect(0, 0, DESIGN_W - 20, DESIGN_H - 20));
	addChild(background);	

	return true;
}

void MadnessProgress::start() {
	madness = 0.0f;
	madnessProgressSpeed = 10.0f;	
}

void MadnessProgress::onUpdate(float delta) {
	madness += delta / madnessProgressSpeed;	
	madness = std::max(0.0f, std::min(1.0f, madness));

	updateVisuals();
}

void MadnessProgress::resetMadness() {
	madness = 0.0f;
	madnessProgressSpeed -= 0.1f;
	madnessProgressSpeed = std::max(3.0f, madnessProgressSpeed);

	updateVisuals();
}

void MadnessProgress::updateVisuals() {
	progress->setPosition(DESIGN_W / 2, DESIGN_H / 2 * madness);
	progress->setTextureRect(cocos2d::Rect(0, 0, DESIGN_W, DESIGN_H * madness));
}

bool MadnessProgress::isMadnessReached(){
	return madness >= 1.0f;
}

// Game
Game* Game::createScene() {	
	return Game::create();
}

bool Game::init() {
	if (!cocos2d::Scene::init())
		return false;				

	config = NULL;	

	//create game
	mProgress = MadnessProgress::create();
	mProgress->setPosition(cocos2d::Vec2(0, 0));
	addChild(mProgress);

	boardLayer = BoardTilesLayer::create(this);	
	addChild(boardLayer);	

	scoreBackground = cocos2d::Sprite::create();
	scoreBackground->setPosition(cocos2d::Vec2(DESIGN_W / 2, DESIGN_H - 50));	
	addChild(scoreBackground);

	scoreLabel = cocos2d::Label::create(std::to_string(score), "Arial", 35);
	scoreLabel->setColor({ 0, 0, 0 });
	scoreLabel->setAlignment(cocos2d::TextHAlignment::CENTER);	
	scoreLabel->setPosition(cocos2d::Vec2(DESIGN_W / 2, DESIGN_H - 50));
	addChild(scoreLabel);

	monster = cocos2d::Sprite::create("monsterhead.png");
	monster->setVisible(false);
	boardLayer->monsterLayer->addChild(monster);

	//creating menu
	auto replayButton = cocos2d::MenuItemImage::create("replay.png", "replay_pressed.png",
													   CC_CALLBACK_1(Game::replayGame, this));
	replayButton->setPosition(cocos2d::Vec2(DESIGN_W * 0.25f, 200));

	auto exitButton = cocos2d::MenuItemImage::create("exit.png", "exit_pressed.png",
														CC_CALLBACK_1(Game::exitGame, this));
	exitButton->setPosition(cocos2d::Vec2(DESIGN_W * 0.75f, 200));

	auto score = cocos2d::MenuItemLabel::create(cocos2d::Label::create("SCORE: ", "Arial", 50));
	score->setPosition(cocos2d::Vec2(DESIGN_W / 2, DESIGN_H - 200));

	gameOverMenu = cocos2d::Menu::create(replayButton, exitButton, score, NULL);
	gameOverMenu->setPosition(cocos2d::Vec2::ZERO);
	gameOverMenu->retain();

	return true;
}

void Game::replayGame(cocos2d::Ref* ref) {
	scoreBackground->runAction(cocos2d::FadeOut::create(0.3f));
	gameOverMenu->runAction(cocos2d::Sequence::create(
		cocos2d::FadeOut::create(0.5f),
		cocos2d::RemoveSelf::create(),
		cocos2d::CallFunc::create([&](){ 		
			start();
			resume();
		}),
		NULL
		));
}

void Game::exitGame(cocos2d::Ref* ref) {
	scoreBackground->runAction(cocos2d::FadeOut::create(0.3f));
	gameOverMenu->runAction(cocos2d::Sequence::create(
		cocos2d::FadeOut::create(0.5f),
		cocos2d::RemoveSelf::create(),
		cocos2d::CallFunc::create([&](){ cocos2d::Director::getInstance()->popScene(); }),
		NULL
		));
}

void Game::setGameConfig(cocos2d::Map<std::string, cocos2d::String*>* newConfig) {	

	std::string possibleBoardTileTypes[] = {"ball", "cross", "bean", "ring", "box" };
	this->config = newConfig;	
	
	int itemTypes = newConfig->at("item_types")->intValue();

	for (auto item : BoardTileType::boardTypes)
		delete item;

	BoardTileType::boardTypes.clear();
	for (int i = 0; i < itemTypes; i++)
		BoardTileType::boardTypes.push_back(new BoardTileType(i, possibleBoardTileTypes[i], 1.0f));	

	BoardTilesLayer::tileSize.setSize(newConfig->at("board_tile_width")->floatValue(),
									  newConfig->at("board_tile_height")->floatValue());

	resizeBoardToFitSize(newConfig->at("board_width")->intValue(),
						 newConfig->at("board_height")->intValue());
	
	boardLayer->setPosition(cocos2d::Vec2((DESIGN_W - newConfig->at("board_width")->intValue() *
													  newConfig->at("board_tile_width")->floatValue()) * 0.5f, 6));
}

void Game::start() {
	CCASSERT(config != NULL, "You must set config before launching the game");

	gameFinished = false;

	score = 0;
	scoreLabel->setString(std::to_string(score));	
	scoreBackground->setTextureRect(cocos2d::Rect(0, 0, scoreLabel->getContentSize().width + 20, scoreLabel->getContentSize().height));
	
	monster->setScale(1.0f);
	monster->setVisible(false);

	boardLayer->setVisible(true);
	boardLayer->start();
	mProgress->setVisible(true);
	
	scoreBackground->setPosition(cocos2d::Vec2(DESIGN_W / 2, DESIGN_H - 30));		
	scoreLabel->setPosition(cocos2d::Vec2(DESIGN_W / 2, DESIGN_H - 30));	
	scoreBackground->setOpacity(255);

	mProgress->start();
	boardLayer->blockInput(false);
	
	for (auto col : board)
		for (auto row : col)
			row->spawn();
}

void Game::onEnter() {	
	cocos2d::Scene::onEnter();		

	start();
	resume();	
}

void Game::onExit() {
	cocos2d::Scene::onExit();
	pause();	
}

void Game::pause() {
	unschedule(schedule_selector(Game::onUpdate));
	boardLayer->pause();
}

void Game::resume() {
	schedule(schedule_selector(Game::onUpdate));
	boardLayer->resume();
}

void Game::showGameOverOverlay() {
	monster->stopAllActions();
	monster->setVisible(true);
	monster->setRotation(0.0f);
	monster->setPosition(DESIGN_W / 2, -monster->getContentSize().height);

	monster->runAction(cocos2d::Sequence::create(
			cocos2d::EaseBackOut::create(cocos2d::MoveTo::create(1.0f, cocos2d::Vec2(DESIGN_W / 2, DESIGN_H / 2))),
			cocos2d::EaseCircleActionOut::create(cocos2d::ScaleTo::create(0.5f, 6.0f)),			
			NULL
		));

	cocos2d::Sprite* blackOverlay = cocos2d::Sprite::create();
	blackOverlay->setPosition(DESIGN_W / 2, DESIGN_H / 2);
	blackOverlay->setTextureRect(cocos2d::Rect(0, 0, DESIGN_H, DESIGN_H));
	blackOverlay->setColor({ 0, 0, 0 });
	blackOverlay->setOpacity(0);
	addChild(blackOverlay);

	
	blackOverlay->runAction(cocos2d::Sequence::create(
		cocos2d::DelayTime::create(1.0f),
		cocos2d::FadeIn::create(0.3f),
		cocos2d::RemoveSelf::create(),
		cocos2d::CallFunc::create([&](){
			boardLayer->setVisible(false);
			mProgress->setVisible(false);

			scoreBackground->setPosition(cocos2d::Vec2(DESIGN_W / 2, DESIGN_H - 270));
			scoreLabel->setPosition(cocos2d::Vec2(DESIGN_W / 2, DESIGN_H - 270));

			PlayerProgress::tryToInsertNewHiscore(score);
			addChild(gameOverMenu);
			monster->setVisible(false);
			gameOverMenu->setOpacity(0);
			gameOverMenu->runAction(cocos2d::FadeIn::create(0.5f));
		}),
		NULL
		));
	
}

void Game::onUpdate(float delta){
	mProgress->onUpdate(delta);

	if (!gameFinished && mProgress->isMadnessReached()){
		gameFinished = true;
		pause();
		showGameOverOverlay();
	}
}

void Game::resizeBoardToFitSize(UINT width, UINT height) {
	UINT i;
	for (i = 0; i < width; i++) {		
		if (board.size() <= i) {			
			board.push_back(std::vector<BoardTile*>());
		}

		UINT j;
		for (j = 0; j < height; j++) {
			BoardTile* item = BoardTile::create({ i, j });						
			item->retain();
			board.at(i).push_back(item);

			boardLayer->registerTile(item);
		}

		while (board.at(i).size() > j) {
			boardLayer->unregisterTile(board.at(i).back());

			board.at(i).back()->release();
			board.at(i).pop_back();
		}
	}

	while (board.size() > i) {
		for (auto tile : board.back()) {
			boardLayer->unregisterTile(tile);
			tile->release();
		}
		
		board.pop_back();
	}

	for (i = 0; i < width; i++)
		for (UINT j = 0; j < height; j++)
			board.at(i).at(j)->setPosition(cocos2d::Vec2(i * BoardTilesLayer::tileSize.width,
														 (height - j - 1) * BoardTilesLayer::tileSize.height));
}

void Game::handleScoreUpdate(int addScore) {
	if (addScore > 0) {
		score += addScore;
		scoreLabel->setString(std::to_string(score));
		scoreBackground->setTextureRect(cocos2d::Rect(0, 0, scoreLabel->getContentSize().width + 20, scoreLabel->getContentSize().height));
	}

	if (addScore > 0) {
		mProgress->resetMadness();
	} else {
		hideMonster();

		if (addScore < 0)
			shakeAndBlock();
	}

	bool finish = false;
	for (UINT j = 0; !finish; j++) {
		for (UINT i = 0; i < board.size(); i++) {
			int rowPos = board.at(i).size() - 1 - j;
			if (rowPos >= 0) {
				BoardTile *item = board.at(i)[rowPos];
				if (item->getType() == NULL)
					item->simulateGravity(board);
			} else {
				finish = true;
				break;
			}
		}
	}
}

void Game::shakeAndBlock() {
	boardLayer->blockInput(true);

	boardLayer->runAction(
		cocos2d::Sequence::create(
		Shake::actionWithDuration(1.0f, 15.0f),
		cocos2d::CallFunc::create([&]() {
			boardLayer->blockInput(false);
		}), 
		NULL));

}

void Game::showMonster() {		

	int numberOfActions = monster->getNumberOfRunningActions() > 0;
	int border;

	monster->stopAllActions();
	
	if (numberOfActions == 0) {
		border = CCRANDOM_0_1() < 0.5f ? 0 : 1;
		monsterTargetY = CCRANDOM_0_1() * DESIGN_H * 0.7f + DESIGN_H * 0.15f;
	} else {
		border = monster->getPosition().x < DESIGN_W / 2 ? LEFT_BORDER : RIGHT_BORDER;
	}
	
	monster->setVisible(true);

	if (border == 0) {
		monsterTargetX = 0;
		if (numberOfActions == 0) {
			monster->setRotation(-30.0f);
			monster->setPosition(0.0f - monster->getContentSize().width,
				monsterTargetY + monster->getContentSize().height);
		}

		monster->runAction(cocos2d::EaseBackOut::create(cocos2d::MoveTo::create(0.25f, cocos2d::Vec2(30.0f, monsterTargetY))));		
		monster->runAction(cocos2d::Sequence::create(cocos2d::DelayTime::create(0.6f), cocos2d::CallFunc::create([&](){ hideMonster(); }), NULL));
	} else {
		monsterTargetX = DESIGN_W;
		if (numberOfActions == 0) {
			monster->setRotation(30.0f);
			monster->setPosition(DESIGN_W + monster->getContentSize().width,
				monsterTargetY + monster->getContentSize().height);
		}

		monster->runAction(cocos2d::EaseBackOut::create(cocos2d::MoveTo::create(0.25f, cocos2d::Vec2(DESIGN_W - 30.0f, monsterTargetY))));
		monster->runAction(cocos2d::Sequence::create(cocos2d::DelayTime::create(0.6f), cocos2d::CallFunc::create([&](){ hideMonster(); }), NULL));
	}	
}

void Game::hideMonster() {
	monster->stopAllActions();

	if (monster->getPosition().x < DESIGN_W / 2) {
		monster->runAction(cocos2d::Sequence::create(
			cocos2d::EaseQuadraticActionOut::create(cocos2d::MoveBy::create(0.8f, cocos2d::Vec2(-monster->getContentSize().width, 0))),
			cocos2d::CallFunc::create([&](){ monster->setVisible(false); }),
			NULL
			));

	} else{
		monster->runAction(cocos2d::Sequence::create(
			cocos2d::EaseQuadraticActionOut::create(cocos2d::MoveBy::create(0.8f, cocos2d::Vec2(monster->getContentSize().width, 0))),
			cocos2d::CallFunc::create([&](){ monster->setVisible(false); }),
			NULL
			));
	}
}

cocos2d::Vec2 Game::getMonsterPos() {
	return cocos2d::Vec2(monsterTargetX, monsterTargetY);
}