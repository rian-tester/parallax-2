//
// Created by fitra on 12/21/2023.
//

#ifndef PROJ_ANDROID_PARALLAX_H
#define PROJ_ANDROID_PARALLAX_H

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

class Parallax : public Layer {

public:

    CREATE_FUNC(Parallax);

    static cocos2d::Scene* createScene();

    virtual bool init();



private:

    void initPlayer(); // where to setup player related

    void initControllers(); // where to setup UI and input related

    void initParallaxScene(); // Where to setup all parallax related

    virtual bool onTouchBegan(Touch* touch, Event* event);
    virtual void onTouchMoved(Touch* touch, Event* event);
    virtual void onTouchEnded(Touch *touch, Event* event);

    Sprite* playerSprite;

    Action* girlIdle;
    Action* girlJump;

    Action* getIdleAnimation();
    Action* getJumpAnimation();

    ParallaxNode* paraNode;

    ui::Button* jumpButton;

    // State
    bool jumping;

    // Callbacks
    void clickJumpButton(Ref* sender);



};


#endif //PROJ_ANDROID_PARALLAX_H
