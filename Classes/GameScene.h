#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"
#include "Shake.h"
#include "Sub.h"

#define ADDING_TO_CHAIN_FAILED 0
#define ADDING_TO_CHAIN_SUCCEDED 1
#define REVERT_ADDING_TO_CHAIN 2

#define LEFT_BORDER 0
#define RIGHT_BORDER 1

class BoardTilesLayer;
class Game;

struct BPos {
	int col;
	int row;
};

class BoardTileType {
public:
	static std::vector<BoardTileType*> boardTypes;

	BoardTileType(int setId, std::string name, float chance);
	~BoardTileType(){};

	std::string getName();
	int getId();
	float getSpawnChance();
	int getValue();

protected:
	std::string name;
	int id;
	float chance;		
};

class BoardTile : public cocos2d::Node {
public:	
	BoardTilesLayer* manager;

	BoardTile() : image(nullptr),
				  type(nullptr),
				  manager(nullptr){}
	~BoardTile(){}
	static BoardTile* create(BPos pos);
	bool init(BPos pos);

	void spawn();
	void collect(const cocos2d::Vec2&, cocos2d::Layer*);
	void collectWithoutEffect();
	void destroy(cocos2d::Layer*);
	
	void simulateGravity(std::vector<std::vector<BoardTile*>> board);

	void setType(BoardTileType*);
	BoardTileType* getType();	

	int getPattern();
	void setPattern(int);	

	int getCol();
	int getRow();	

protected:
	BoardTileType* type;
	BPos myPos;
	cocos2d::Sprite* image;
	int pattern;	

	void moveToFinalPos();
};

class Chain{
public:	
	Chain();
	~Chain();	

	int push(BoardTile*);
	void pop();
	int collect(const cocos2d::Vec2&, cocos2d::Layer*);
	void destroy();

	BoardTile* getSecLastItem();

protected:
	std::vector<BoardTile*>* items;	

	bool isChainValid();
};

class Pattern {
public:	

	Pattern(std::string path);
	~Pattern(){};

	cocos2d::Sprite* getPatternSprite();

	void changeDirection();
	void changeSpeed();
	void update(float);

protected:
	float changeDirDelay;
	float changeSpeedDelay;	
	cocos2d::Vec2 movDirection;
	cocos2d::Vec2 movTargetDirection;
	float movSpeed;
	float movTargetSpeed;

	cocos2d::Sprite* patternSprite;	
};

class ClippedPattern : public Pattern {
public:		

	ClippedPattern(cocos2d::Node* parent, std::string path, float chance);
	~ClippedPattern();	

	cocos2d::Layer* getLayer();
	float getChance();	
	
	static int getRandomPattern();	

protected:	
	static std::vector<float> _pChances;

	float _chance;	
	
	cocos2d::ClippingNode* cNode;
	cocos2d::Layer* cLayer;
};

class BoardTilesLayer : public cocos2d::Layer {
public:
	friend class BoardTile;

	Game *game;
	static cocos2d::Size tileSize;

	static BoardTilesLayer* create(Game*);	
	virtual bool init();
	void initTouch();
	void start();

	BoardTilesLayer() : touchListener(NULL), 
						blockI (false) {}
	~BoardTilesLayer(){
		if (touchListener != NULL)
			touchListener->release();
	};
		
	void onUpdate(float);

	void resume();
	void pause();

	void blockInput(bool);

	void registerTile(BoardTile*);
	void unregisterTile(BoardTile*);	

	cocos2d::Layer* monsterLayer;

protected:
	bool blockI;
	cocos2d::EventListenerTouchOneByOne *touchListener;

	std::vector<ClippedPattern> patterns;

	void updatePatternForTile(BoardTile*, int);

	cocos2d::Layer* tilesLayer;
	cocos2d::Layer* chainLayer;

	cocos2d::Layer* effectsLayer;

	Chain chain;
	cocos2d::Node* findChildCollidingWithPos(cocos2d::Vec2);	
};

class MadnessProgress : public cocos2d::Node {
public:
	MadnessProgress() {}
	~MadnessProgress() {}

	CREATE_FUNC(MadnessProgress);
	virtual bool init();

	void start();	
	void onUpdate(float);
	void resetMadness();

	bool isMadnessReached();

protected:
	cocos2d::Sprite* background;
	cocos2d::Sprite* progress;
	float madness;
	float madnessProgressSpeed;

	void updateVisuals();
};

class Game : public cocos2d::Scene {
public:		
	static Game* createScene();
	
	CREATE_FUNC(Game);
	virtual bool init();		
	Game() : gameOverMenu(NULL) {};
	~Game() {
		if (gameOverMenu != NULL)
			gameOverMenu->release();
	}

	void setGameConfig(cocos2d::Map<std::string, cocos2d::String*>*);
	
	void resizeBoardToFitSize(UINT width, UINT height);
	void onEnter();
	void onUpdate(float);
	void onExit();
	void pause();
	void resume();
	void showGameOverOverlay();
	void shakeAndBlock();	

	void handleScoreUpdate(int);	

	cocos2d::Vec2 getMonsterPos();
	void showMonster();
	void hideMonster();

	//callbacks
	void replayGame(cocos2d::Ref*);
	void exitGame(cocos2d::Ref*);

protected:	
	cocos2d::Map<std::string, cocos2d::String*> *config;

	int score;
	bool gameFinished;
	cocos2d::Menu* gameOverMenu;

	cocos2d::Sprite* monster;
	float monsterTargetX, monsterTargetY;

	cocos2d::Sprite* scoreBackground;
	cocos2d::Label* scoreLabel;
	BoardTilesLayer* boardLayer;	
	MadnessProgress* mProgress;

	std::vector<std::vector<BoardTile*>> board;		

	void start();
};

#endif