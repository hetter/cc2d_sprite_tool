#ifndef EG_BASE_SCENE_H_
#define EG_BASE_SCENE_H_

#include "cocos2d.h"
#include "cocos-ext.h"

class BaseEditScene:public cocos2d::CCLayer
{
public:
    BaseEditScene() :
    m_destLayer(NULL)
    {
        m_thisMenu = cocos2d::CCMenu::create();
        m_thisMenu->retain();
        m_thisMenu->setPosition(cocos2d::CCPointZero);
    }
    
    ~BaseEditScene()
    {
        m_thisMenu->release();
    }
    
    virtual void initUi() = 0;
    
    virtual void update(const float& dt)
    {}
    
    virtual void releaseUi()
    {
        m_thisMenu->removeAllChildren();
    }
    
    virtual void showOnDestLayer(cocos2d::CCLayer* dest_, const int& lay_ = 100000)
    {
        m_destLayer = dest_;
        dest_->addChild(this, lay_);
    }
    
    virtual void closeOnDestLayer()
    {
        if(m_destLayer)
        {
            m_destLayer->removeChild(this);
            m_destLayer = NULL;
        }
    }
    
 ///////////event base function///////////
    //only one click
    virtual bool ccTouchBegan   (cocos2d::CCTouch* touch_)  {return true;}
    virtual void ccTouchMoved  (cocos2d::CCTouch* touch_)  {}
    virtual void ccTouchEnded   (cocos2d::CCTouch* touch_)  {}
////////////////////////////////////////

protected:
    cocos2d::CCLayer*   m_destLayer;
    cocos2d::CCMenu*   m_thisMenu;
};


#endif