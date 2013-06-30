#ifndef  EG_RECT_EDIT_SCENE_H_
#define EG_RECT_EDIT_SCENE_H_

#include "BaseEditScene.hpp"

class IconListComponent;
class GLDrawLayer;

class RectEditScene:public BaseEditScene, public cocos2d::extension::CCEditBoxDelegate
{
public:
    RectEditScene();
    ~RectEditScene();
    
    void initUi();
    void releaseUi();
    
    void showOnDestLayer(cocos2d::CCLayer* dest_, const int& lay_ = 100000);
    void closeOnDestLayer();

    void update(const float& dt_);
    
    //only one click
    bool ccTouchBegan   (cocos2d::CCTouch* touch_);
    void ccTouchMoved  (cocos2d::CCTouch* touch_);
    void ccTouchEnded   (cocos2d::CCTouch* touch_);

public:
    //btn callBacks
    void addCutRectCallBack(cocos2d::CCObject* pSender_);
    void cutPicCallBack(cocos2d::CCObject* pSender_);
    void selPicCallBack(cocos2d::CCObject* pSender_);
    void delRectCallBack(cocos2d::CCObject* pSender_);
    void rectlistLeftCallBack(cocos2d::CCObject* pSender_);
    void rectlistRightCallBack(cocos2d::CCObject* pSender_);
    
protected:
    enum EDIT_STATE
    {
        SELECT_BIGSPR,
        CUT_RECT,
        STATE_COUNT
    };
    
protected:
    void _go_to_edit(const EDIT_STATE& state_);
    void _modifyRectSet();
protected:    
    //switch
    bool m_bIsMoveSelBox;
    bool m_bIsSetSelBoxEnd;

    //component of List    
    IconListComponent*  m_nowUseList;
    cocos2d::CCArray*    m_nowContactArr;
    
    cocos2d::CCSprite*   m_nowUseBigSprite;
    cocos2d::CCSprite*   m_nowSprMask;
    
    GLDrawLayer*           m_glDrawLayer;
    
    cocos2d::CCRect m_nowSelectRect;
    
    int m_lastSelectindex;
    
    cocos2d::CCMenuItemFont* m_changeEditBtn;
    
    EDIT_STATE m_nowState;
    //extension gui part//
public:
    virtual void editBoxEditingDidBegin(cocos2d::extension::CCEditBox* editBox);
    virtual void editBoxEditingDidEnd(cocos2d::extension::CCEditBox* editBox);
    virtual void editBoxTextChanged(cocos2d::extension::CCEditBox* editBox, const std::string& text);
    virtual void editBoxReturn(cocos2d::extension::CCEditBox* editBox);
protected:
    void _updateRectSelect();
protected:
    cocos2d::extension::CCEditBox *m_editRectX, *m_editRectY, *m_editRectW, *m_editRectH;
    ////////////////////
};


#endif