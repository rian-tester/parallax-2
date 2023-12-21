//
// Created by fitra on 12/21/2023.
// all code written by me
// except for parallax effect I found an example in the internet (https://rezghob.com/parallax/) and modified the code as per my need
// credit to the owner
//

#include "Parallax.h"
#include "HelloWorldScene.h"


USING_NS_CC;

cocos2d::Scene* Parallax::createScene()
{
    // create a scene
    auto scene = Scene::create();


    auto  layer = Parallax::create();
    scene->addChild(layer);

    return scene;
}

bool Parallax::init()
{
    if(!Layer::init())
    {
        return false;
    }

    // Screen coordinates
    cocos2d::Rect visibleRect = Director::getInstance()->getOpenGLView()->getVisibleRect();
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    Size screenSize = Director::getInstance()->getWinSize();

    // add title
    auto label = Label::createWithTTF("Parallax", "fonts/Marker Felt.ttf", 25);
    label->setPosition(Vec2(visibleRect.origin.x+visibleRect.size.width/2, visibleRect.origin.y+visibleRect.size.height/2).x,
                       Vec2(visibleRect.origin.x+visibleRect.size.width/2, visibleRect.origin.y+visibleRect.size.height).y - 30);

    this->addChild(label, 99);

    // add label for jump button
    auto jumpLabel = Label::createWithTTF("Jump", "fonts/Marker Felt.ttf", 19);
    jumpLabel->setPosition(Point(screenSize.width * 0.15f,  screenSize.height  * 0.35f));
    this->addChild(jumpLabel, 100);


    //add the menu item for back to main menu
    label = Label::createWithTTF("Go To MainMenu", "fonts/Marker Felt.ttf", 19);
    auto menuItem = MenuItemLabel::create(label);
    menuItem->setCallback([&](cocos2d::Ref *sender) {
        Director::getInstance()->replaceScene(HelloWorld::createScene());
    });

    auto menu = Menu::create(menuItem, nullptr);
    menu->setPosition(Vec2::ZERO);
    menuItem->setPosition(Vec2(visibleRect.origin.x+visibleRect.size.width - 80, visibleRect.origin.y + 25));
    this->addChild(menu, 100);

    // Register native Android back button
    auto touchListener = EventListenerKeyboard::create();
    touchListener->onKeyReleased =  [](cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event)
    {
        if(keyCode == EventKeyboard::KeyCode::KEY_BACK)
        {
            // Quit the game
            Director::getInstance()->end();
        }
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

    // Continue init
    this->initPlayer();
    this->initControllers();
    this->initParallaxScene();

    return true;
}

void Parallax::initPlayer()
{
    // Screen coordinates
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Point origin = Director::getInstance()->getVisibleOrigin();

    // Setup player sprite
    girlIdle = getIdleAnimation();
    playerSprite = Sprite::create();
    playerSprite->setScale(0.4f);
    playerSprite->setPosition(Point(visibleSize.width / 2 + origin.x, visibleSize.height /5 + origin.y));
    playerSprite->runAction(girlIdle);

    this->addChild(playerSprite, 1);

}

void Parallax::initControllers()
{
    // Screen coordinates
    Size screenSize = Director::getInstance()->getWinSize();

    // Jump Button Setup
    jumpButton = ui::Button::create("startButton.png", "startButtonDn.png", "startButtonDn.png", ui::AbstractCheckButton::TextureResType::PLIST);
    jumpButton->setPosition(Point(screenSize.width * 0.15f,  screenSize.height  * 0.25f));
    jumpButton->addTouchEventListener( [&] (Ref* sender, ui::Widget::TouchEventType type) {
        switch (type) {
            case ui::Widget::TouchEventType::BEGAN:
                this->clickJumpButton(sender);
                break;
            case ui::Widget::TouchEventType::ENDED:
                break;
            default:
                break;
        }
    });

    this->addChild(jumpButton, 99);


    // Register touch for scroll
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(Parallax::onTouchBegan, this);
    listener->onTouchEnded = CC_CALLBACK_2(Parallax::onTouchEnded, this);
    listener->onTouchMoved = CC_CALLBACK_2(Parallax::onTouchMoved, this);

    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

}

Action* Parallax::getIdleAnimation()
{
    // Looping from  spritesheet
    auto girlIdleAnimation = Animation::create();
    for(int index = 1; index < 17; index++)
    {
        char name[100] = {0};
        sprintf(name, "Idle (%i).png", index);
        auto girlIdleSpriteFrame = SpriteFrameCache::getInstance()->getSpriteFrameByName(name);
        girlIdleAnimation->addSpriteFrame(girlIdleSpriteFrame);
    }

    // Creating Animation
    girlIdleAnimation->setDelayPerUnit(0.15f / 2.f);
    girlIdleAnimation->setRestoreOriginalFrame(true);
    auto girlAnimate = Animate::create(girlIdleAnimation);
    girlIdle = RepeatForever::create(girlAnimate);

    return girlIdle;
}

Action* Parallax::getJumpAnimation()
{
    // Looping from spritesheet
    auto girlJumpAnimation = Animation::create();
    for(int index =1; index < 31; index++)
    {
        char name[100] =  {0};
        sprintf(name, "Jump (%i).png", index);
        auto girlJumpSpriteFrame = SpriteFrameCache::getInstance()->getSpriteFrameByName(name);
        girlJumpAnimation->addSpriteFrame(girlJumpSpriteFrame);
    }

    // Creating Animation
    girlJumpAnimation->setDelayPerUnit(0.5f / 3.f);
    girlJumpAnimation->setRestoreOriginalFrame(true);
    auto girlAnimate = Animate::create(girlJumpAnimation);
    girlJump = RepeatForever::create(girlAnimate);

    return girlJump;

}

void Parallax::clickJumpButton(Ref* sender)
{
    // Prepare callback function to reset jump
    auto calb = CallFunc::create( [this] () {
        playerSprite->stopAllActions();
        playerSprite->runAction(getIdleAnimation());
        jumping = false;
    });

    // Only jumping if state jumping is false
    if (!jumping)
    {
        playerSprite->stopAllActions();
        playerSprite->runAction(getJumpAnimation());
        jumping = true;
        auto action = JumpBy::create(2, Point(0,0), 75, 1);
        auto seq = Sequence::create(action, calb, nullptr);
        playerSprite->runAction(seq);
    }

}

void Parallax::initParallaxScene()
{
    // Screen coordinates
    auto visibleSize = Director::getInstance()->getVisibleSize();   // Screen's visible size
    Vec2 origin = Director::getInstance()->getVisibleOrigin();      // Screen's visible origin
    float x = origin.x + visibleSize.width /2;                   // X for the center of the screen
    float y = origin.y + visibleSize.height /2;                  // y for the center of the screen

   // Create one sprite for each parallax layer
   auto layer1 = Sprite::create("scene/layer1.png");
   auto layer2 = Sprite::create("scene/layer2_extra.png");
   auto layer3 = Sprite::create("scene/layer3_extra.png");
   auto layer4 = Sprite::create("scene/layer4_extra.png");

   // Set sprites scale
    layer1->setScale(1.75f);
    layer2->setScale(1.75f);
    layer3->setScale(1.65f);
    layer4->setScale(1.65f);

    // setup parallax node
    paraNode = ParallaxNode::create();

    paraNode->addChild(layer1, -4, Vec2(0.2, 0), Vec2(x, y));
    paraNode->addChild(layer2, -3, Vec2(0.5, 0), Vec2(x, y));
    paraNode->addChild(layer3, -2, Vec2(0.8, 0), Vec2(x, y * 1.1f));
    paraNode->addChild(layer4, 5, Vec2(1.0, 0), Vec2(x, y * 0.5f));

    this->addChild(paraNode, 0, 1);

}

bool Parallax::onTouchBegan(Touch* touch, Event* event)
{
    // Leave empty until needed later
    return true;
}

void Parallax::onTouchMoved(Touch* touch, Event* event)
{
    // Cache touch delta
    Vec2 touchDelta = touch->getDelta();
    log("X touch delta : %f " , touchDelta.x);

    // Get X scroll delta
    auto pos = paraNode->getPosition();

    // Calculate Parallax' new position
    pos.x += 1.5 * touchDelta.x;

    // Make sure the sprites stay in the screen
    if(pos.x > 100)
        pos.x = 100;
    if(pos.x < -100)
        pos.x = -100;

    // Update parallax position
    paraNode->setPosition(pos);

}

void Parallax::onTouchEnded(Touch* touch, Event* event)
{
    // Create MoveTo action
    auto moveTo = MoveTo::create(1.0f, Vec2(0, paraNode->getPosition().y));

    // Create the EaseIn action to apply an easing effect to the MoveTo action
    auto easeIn = EaseIn::create(moveTo, 2.0f);

    // Run the action on the Parallax node
    paraNode->runAction(easeIn);
}
