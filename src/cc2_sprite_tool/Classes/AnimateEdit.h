#ifndef  EG_ANIMATE_EDIT_SCENE_H_
#define EG_ANIMATE_EDIT_SCENE_H_

#include "BaseEditScene.hpp"

class AnimateEditScene:public BaseEditScene, public cocos2d::extension::CCEditBoxDelegate
{
public:
    AnimateEditScene();
    ~AnimateEditScene();
    
    void initUi();
    void releaseUi();
    
    void showOnDestLayer(cocos2d::CCLayer* dest_, const int& lay_ = 100000);
    void closeOnDestLayer();
    
    //only one click
    bool ccTouchBegan   (cocos2d::CCTouch* touch_);
    void ccTouchMoved  (cocos2d::CCTouch* touch_);
    void ccTouchEnded   (cocos2d::CCTouch* touch_);
    //
    
    void update(const float& dt);
public:
    //callback function//
    void editNowFrameCallBack(CCObject* pSender_);
    void doDelFrameCallBack(CCObject* pSender_);
    void doUpFrameCallBack(CCObject* pSender_);
    void copyFrameCallBack(CCObject* pSender_);
    void doDownFrameCallBack(CCObject* pSender_);
    void framelistLeftCallBack(CCObject* pSender_);
    void framelistRightCallBack(CCObject* pSender_);
    void playAnimateCallBack(CCObject* pSender_);
    void stopAnimateCallBack(CCObject* pSender_);
    ////////////////////
protected:
    int m_lastSelectindex;
    cocos2d::CCArray* m_showFrame;
    cocos2d::CCSprite* m_listMask;
    //extension gui part//
public:
    virtual void editBoxEditingDidBegin(cocos2d::extension::CCEditBox* editBox);
    virtual void editBoxEditingDidEnd(cocos2d::extension::CCEditBox* editBox);
    virtual void editBoxTextChanged(cocos2d::extension::CCEditBox* editBox, const std::string& text);
    virtual void editBoxReturn(cocos2d::extension::CCEditBox* editBox);
protected:
    cocos2d::extension::CCEditBox *m_editFrameSpeed;
};


#endif