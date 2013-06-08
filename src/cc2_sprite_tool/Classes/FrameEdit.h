#ifndef  EG_FRAME_EDIT_SCENE_H_
#define EG_FRAME_EDIT_SCENE_H_

#include "BaseEditScene.hpp"

class IconListComponent;
class GLDrawLayer;

class FrameEditScene:public BaseEditScene,public cocos2d::extension::CCEditBoxDelegate
{
public:
    FrameEditScene();
    ~FrameEditScene();
    
    void initUi();
    void releaseUi();
    
    void showOnDestLayer(cocos2d::CCLayer* dest_, const int& lay_ = 100000);
    void closeOnDestLayer();
    
    //only one click
    bool ccTouchBegan   (cocos2d::CCTouch* touch_);
    void ccTouchMoved  (cocos2d::CCTouch* touch_);
    void ccTouchEnded   (cocos2d::CCTouch* touch_);
public:
    //callback function//
    void goToMoveElmCallBack(cocos2d::CCObject* pSender_);
    void goToDetailSetCallBack(cocos2d::CCObject* pSender_);
    void goToFrameSprViewCallBack(cocos2d::CCObject* pSender_);
    
    //use for component
    void delRectCallBack(cocos2d::CCObject* pSender_);
    void listLeftCallBack(cocos2d::CCObject* pSender_);
    void listRightCallBack(cocos2d::CCObject* pSender_);
    
    void setMovDetailCallBack(cocos2d::CCObject* pSender_);
    void setRotDetailCallBack(cocos2d::CCObject* pSender_);
    void setBoxDetailCallBack(cocos2d::CCObject* pSender_);
    void flipNowXCallBack(CCObject* pSender_);
    void flipNowYCallBack(CCObject* pSender_);
    void changeAlphaCallBack(CCObject *pSender_, cocos2d::extension::CCControlEvent);
    void changeScaleCallBack(CCObject *pSender_, cocos2d::extension::CCControlEvent);
    void addFrameCallBack(CCObject* pSender_);
    void refreshCallBack(CCObject* pSender_);
    ////////////////////
protected:
    enum EDIT_STATE
    {
        MOVE_ELEMENT,
        DETAIL_SET,
        FRAME_SPR_VIEW,
        STATE_COUNT
    };
    
    enum DETAIL_CLICK_STATE
    {
        MOV_DETAIL,
        ROT_DETAIL,
        BOX_DETAIL,
        DETAIL_COUNT
    };
    
    typedef std::map<EDIT_STATE,  cocos2d::CCLayer*> LayerMap;
protected:
    void _go_to_edit(const EDIT_STATE& state_);
    void _addUi();
protected:
    //component of List
    IconListComponent*  m_nowUseList;
    cocos2d::CCArray*    m_nowContactArr;
    
    cocos2d::CCSprite*   m_listMask;
    cocos2d::CCSprite*   m_sprMask;
    
    cocos2d::CCArray*    m_oneFrameSprs;
    
    //use for touch//
    cocos2d::CCSprite*   m_touchMoveSpr;
    bool m_bIsAddSprToFrame;
    ////////////////
    
    LayerMap m_editUiMap;
    CCLayer* m_nowEditUi;
    
    GLDrawLayer*  m_glDrawLayer;
    
    cocos2d::CCRect m_nowSelectRect;
    
    int m_lastSelectindex;
    
    cocos2d::CCMenuItemFont* m_detailBtn;
    DETAIL_CLICK_STATE m_detailClick;
    EDIT_STATE m_nowState;
    
//extension gui part//
public:
    virtual void editBoxEditingDidBegin(cocos2d::extension::CCEditBox* editBox);
    virtual void editBoxEditingDidEnd(cocos2d::extension::CCEditBox* editBox);
    virtual void editBoxTextChanged(cocos2d::extension::CCEditBox* editBox, const std::string& text);
    virtual void editBoxReturn(cocos2d::extension::CCEditBox* editBox);
protected:
    void _addUiOnDetailSet();
    void _removeUiOnDetailSet();    
    void _updateDetailSet();
protected:
    cocos2d::extension::CCEditBox *m_editNowSprX, *m_editNowSprY, *m_editNowSprRot,
                                                    *m_editNowSprLayer, *m_editNowSprAlpha, *m_editNowSprScale;
    cocos2d::extension::CCEditBox *m_editFrameRX, *m_editFrameRY, *m_editFrameRW, *m_editFrameRH;
    cocos2d::extension::CCEditBox *m_editFrameSpeed;
    float                                           m_inputFrameSpeed;
    cocos2d::extension::CCControlSlider *m_sliderAlpha, *m_sliderScale;
////////////////////
};


#endif