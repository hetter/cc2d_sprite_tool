#ifndef __MainEditLayer_SCENE_H__
#define __MainEditLayer_SCENE_H__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "EgAnimate.h"

//==============================================

class GLDrawLayer:public cocos2d::CCLayer
{
public:
    GLDrawLayer();
    ~GLDrawLayer();
    virtual void draw();
    void setDrawRectStart(const cocos2d::CCPoint& start_);
    void setDrawRectEnd(const cocos2d::CCPoint& end_);
    void setDrawPoint(const cocos2d::CCPoint& point_);
    const cocos2d::CCPoint& getDrawRectStart() {return m_drawRectStart;}
    const cocos2d::CCPoint& getDrawRectEnd() {return m_drawRectEnd;}
    cocos2d::CCRect getNowRect();
private:
    cocos2d::CCPoint m_drawRectStart;
    cocos2d::CCPoint m_drawRectEnd;
};

//==============================================


class IconListComponent
{
public:
    IconListComponent(const cocos2d::CCPoint& firstIconPos_, const cocos2d::CCSize& iconSize_, const int& MaxShow_ = 8);
    ~IconListComponent();
    
    void goLeftPage();
    void goRightPage();

    void addList(cocos2d::CCSprite* spr_);

    void swapIndex(const int& trueIndex1_, const int& trueIndex2_);

    bool deletIconByShowIndex(const int& showIndex_);

    int getTrueIndexByShowIndex(const int& showIndex_);

    const cocos2d::CCPoint& getIconPositionByShowIndex(const int& showIndex_);
    
    cocos2d::CCArray* getShowList(){return m_showList;}
    cocos2d::CCSprite* getMainLayer(){return m_layer;}

    int getSize() {return m_trueList->count();}
private:
    void _goPage(const int& page_);
private:
    cocos2d::CCSprite* m_layer;
    cocos2d::CCArray*  m_showList;
    cocos2d::CCArray*  m_trueList;
    int m_nowPage;
    int m_maxPage;
    int m_maxShow;
    cocos2d::CCSize m_iconSize;
    cocos2d::CCPoint m_firstIconPos;
};


//==============================================

typedef unsigned int CCControlEvent;

class MainEditLayer : public cocos2d::CCLayer, public cocos2d::extension::CCEditBoxDelegate
{
    
    enum NowClickEditMode
    {
        MOVE_SPRITE_MODE,
        CHANGE_R0TATION_MODE,
        SET_BOUND_BOX_MODE,
        BTN_CHANGE_COUNT_LIMIT,
        SET_RECT_MODE,
        MOVE_ELEMENT_MODE,
        SELECT_ELEMENT_MODE,
        CLICK_MODE_COUNT,
    };
    
    enum NowSceneEditMode
    {
        EDIT_CUT_RECT_MODE,
        EDIT_ONE_FRAME_MODE,
        EDIT_ANIMATE_MODE,
        SCENE_MODE_COUNT,
    };
    
public:
    MainEditLayer();
    ~MainEditLayer();
    // Method 'init' in cocos2d-x returns bool, instead of 'id' in cocos2d-iphone (an object pointer)
    virtual bool init();

    // there's no 'id' in cpp, so we recommend to return the class instance pointer
    static cocos2d::CCScene* scene();
    
    // a selector callback
    void menuCloseCallback(CCObject* pSender);

    // over write from CCStandardTouchDelegate
    bool ccTouchBegan   (cocos2d::CCTouch* touch_, cocos2d:: CCEvent* event_);
    void ccTouchMoved  (cocos2d::CCTouch* touch_, cocos2d:: CCEvent* event_);
    void ccTouchEnded   (cocos2d::CCTouch* touch_, cocos2d:: CCEvent* event_);

public:
    //selector callbacks

    void changeClickEditMode(CCObject* pSender_);

    //edit Frame
    void changeSceneEditMode(CCObject* pSender_);
    void flipNowXCallBack(CCObject* pSender_);
    void flipNowYCallBack(CCObject* pSender_);
    void changeAlphaCallBack(CCObject *pSender_, CCControlEvent controlEvent_);
    void changeScaleCallBack(CCObject *pSender_, CCControlEvent controlEvent_);
    void addFrameCallBack(CCObject* pSender_);
    
    //edit rect
    void addCutRectCallBack(CCObject* pSender_);
    void doCutRectCallBack(CCObject* pSender_);
    void doMoveElementCallBack(CCObject* pSender_);
    void doDelRectCallBack(CCObject* pSender_);
    void rectlistLeftCallBack(CCObject* pSender_);
    void rectlistRightCallBack(CCObject* pSender_);

    //edit animate
    void editNowFrameCallBack(CCObject* pSender_);
    void doDelFrameCallBack(CCObject* pSender_);
    void doUpFrameCallBack(CCObject* pSender_);
    void doDownFrameCallBack(CCObject* pSender_);
    void framelistLeftCallBack(CCObject* pSender_);
    void framelistRightCallBack(CCObject* pSender_);

    //top ui
    void changeStateCutCallBack(CCObject* pSender_);
    void changeStateFrameCallBack(CCObject* pSender_);
    void changeStateAnimateCallBack(CCObject* pSender_);
    void saveCallBack(CCObject* pSender_);
    void loadCallBack(CCObject* pSender_);
    //



    void updateScene(float dt_);
    
private:
    cocos2d::CCPoint _changePosWorld2Anm(const cocos2d::CCPoint& pos_);

    void _addEditFrameUi();
    void _addEditAnimateUi();
    void _addEditCutRectUi();
    void _addEditUiBase();
    
    void _addShowList(cocos2d::CCSprite* spr_, cocos2d::CCArray* list_, cocos2d::CCSprite* lay_);
    bool _checkSpriteTouchAndMask(cocos2d::CCArray* sprArray, const cocos2d::CCPoint& touch_, bool bIsRecord_ = false);
    void _selectUseSprRectInGLDrawLayerBegin(const cocos2d::CCPoint& touch_);
    void _selectGLDrawLayerBegin(const cocos2d::CCPoint& touch_);
    cocos2d::extension::CCEditBox*  _addNumEditBox(const float& ui_x_rate_, const float& ui_y_rate_);
    cocos2d::extension::CCControlSlider* _addSliderGroup(const float& ui_x_rate_, const float& ui_y_rate_,
                                                                                         const float& min_, const float&  max_, const float&  now_,
                                                                                         cocos2d::extension::SEL_CCControlHandler hander_);
    void _updateRenderTarget();
private:
    //list part//

    //component of List(show element)
    cocos2d::CCArray*  m_elementsArr;
    IconListComponent* m_elementIconList;
    
    //component of List(show frame)
    IconListComponent* m_animateFrameList;
    cocos2d::CCRenderTexture*  m_frameRenderTarget;
    
    cocos2d::CCSprite* m_backUiPic;
    
    bool m_bIsOneFrameFromAnimate;
    cocos2d::CCArray*  m_oneFrameSpritesArr;

    cocos2d::CCSprite* m_nowTouchSprite;
    cocos2d::CCSprite* m_nowUseBigSprite;
    cocos2d::CCSprite* m_nowTouchMask;
    cocos2d::CCMenuItemFont* m_changeCEditBtn;

    CC2_EGG::Animate* m_showAnimate;
    NowClickEditMode m_nowClickEditMode;
    NowSceneEditMode m_nowSceneEditMode;
    
    GLDrawLayer*        m_glDrawLayer;
    cocos2d::CCPoint    m_nowSelectBegin;
    cocos2d::CCRect     m_nowSelectRect;
    cocos2d::CCMenu*    m_mainUiMenu;

    unsigned int m_lastSelectindex;
    
    std::wstring m_nowDragFile;
//------------------extension gui part-----------------
public:
    virtual void editBoxEditingDidBegin(cocos2d::extension::CCEditBox* editBox);
    virtual void editBoxEditingDidEnd(cocos2d::extension::CCEditBox* editBox);
    virtual void editBoxTextChanged(cocos2d::extension::CCEditBox* editBox, const std::string& text);
    virtual void editBoxReturn(cocos2d::extension::CCEditBox* editBox);
private:
    void _updateSprAttributeEditBox();
    void _updateRectAttributeEditBox();
private:
    cocos2d::extension::CCEditBox *m_editNowSprX, *m_editNowSprY, *m_editNowSprRot, *m_editNowSprLayer,
                                  *m_editNowSprAlpha, *m_editNowSprScale;
    cocos2d::extension::CCEditBox *m_editFrameRX, *m_editFrameRY, *m_editFrameRW, *m_editFrameRH;
    cocos2d::extension::CCEditBox *m_editFrameSpeed;
    cocos2d::extension::CCControlSlider *m_alphaSlider, *m_scaleSlider;
};

#endif // __MainEditLayer_SCENE_H__
