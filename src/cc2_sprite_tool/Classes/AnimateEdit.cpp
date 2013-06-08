#include "AnimateEdit.h"

#include "EgCommon.h"
#include "MainEditLayer.h"

using namespace cocos2d;
USING_NS_CC_EXT;

AnimateEditScene::AnimateEditScene()
{}

AnimateEditScene::~AnimateEditScene()
{}

void AnimateEditScene::initUi()
{    
    CCSize size = CCDirector::sharedDirector()->getWinSize();
    
    addChild(m_thisMenu);
    
    AddFontBtn(ZH2U("修改所选桢"), this, menu_selector(AnimateEditScene::editNowFrameCallBack),
                       ccp(size.width - UI_BACK_WIDTH * 0.32f, size.height * 0.975f), m_thisMenu);
    AddFontBtn(ZH2U("复制桢"), this, menu_selector(AnimateEditScene::copyFrameCallBack),
                       ccp(size.width - UI_BACK_WIDTH * 0.83f, size.height * 0.975f), m_thisMenu);
    AddFontBtn(ZH2U("上移"), this, menu_selector(AnimateEditScene::doUpFrameCallBack),
                      ccp(size.width - UI_BACK_WIDTH + 30.0f, size.height * 0.937f), m_thisMenu);
    AddFontBtn(ZH2U("删除"), this, menu_selector(AnimateEditScene::doDelFrameCallBack),
                       ccp(size.width - UI_BACK_WIDTH + 170.0f, size.height * 0.937f), m_thisMenu);
    AddFontBtn(ZH2U("下移"), this, menu_selector(AnimateEditScene::doDownFrameCallBack),
                       ccp(size.width - UI_BACK_WIDTH + 100.0f, size.height * 0.937f), m_thisMenu);
    
    AddFontBtn(ZH2U("->"), this, menu_selector(AnimateEditScene::framelistLeftCallBack),
                       ccp(size.width - UI_BACK_WIDTH + 0.9f * UI_BACK_WIDTH,  size.height * 0.9f), m_thisMenu);
    AddFontBtn(ZH2U("<-"), this, menu_selector(AnimateEditScene::framelistRightCallBack),
                       ccp(size.width - UI_BACK_WIDTH + 0.1f * UI_BACK_WIDTH,  size.height * 0.9f), m_thisMenu);
    
    AddFontBtn(ZH2U("|>"), this, menu_selector(AnimateEditScene::playAnimateCallBack),
                       ccp(size.width/2 -  0.2f * UI_BACK_WIDTH,  size.height * 0.1f), m_thisMenu);
    AddFontBtn(ZH2U("||"), this, menu_selector(AnimateEditScene::stopAnimateCallBack),
                       ccp(size.width/2 + 0.2f * UI_BACK_WIDTH,  size.height * 0.1f), m_thisMenu);
    
    m_lastSelectindex = -1;
    m_showFrame = NULL;
    m_listMask = NULL;
}

void AnimateEditScene::releaseUi()
{
    BaseEditScene::releaseUi();
}

void AnimateEditScene::showOnDestLayer(cocos2d::CCLayer* dest_, const int& lay_)
{
    BaseEditScene::showOnDestLayer(dest_, lay_);
    addChild(SMainLayer->getAllFramesList()->getMainLayer());
    SMainLayer->getShowAnimate()->runWithTargetLayer(this);
}

void AnimateEditScene::closeOnDestLayer()
{
    RemoveSprArrFromLayer(m_showFrame, this);
    m_showFrame = NULL;
    BaseEditScene::closeOnDestLayer();
    removeChild(SMainLayer->getAllFramesList()->getMainLayer());
    SMainLayer->getShowAnimate()->removeFormTargetLayer();
}

void AnimateEditScene::update(const float &dt)
{
    SMainLayer->getShowAnimate()->update(dt);
}

bool AnimateEditScene::ccTouchBegan   (cocos2d::CCTouch* touch_)
{
    CCPoint touchPoint  =  touch_->getLocation();
    CCSprite* touchSpr = CheckSpriteTouch(SMainLayer->getAllFramesList()->getShowList(), touchPoint, m_lastSelectindex);
    
    if(touchSpr)
    {
        if (m_listMask)
            this->removeChild(m_listMask);

        m_listMask = CreateAndAddMask(touchSpr, this);
        SMainLayer->getShowAnimate()->stop();
        SMainLayer->getShowAnimate()->removeFormTargetLayer();
        RemoveSprArrFromLayer(m_showFrame, this);       
        m_showFrame = SMainLayer->getShowAnimate()->getFrameSprites(m_lastSelectindex);
        AddSprArrToLayer(m_showFrame, this);
        return false;
    }
    
    return false;
}

void AnimateEditScene::ccTouchMoved  (cocos2d::CCTouch* touch_)
{
}

void AnimateEditScene::ccTouchEnded   (cocos2d::CCTouch* touch_)
{
}

//callback function//
void AnimateEditScene::editNowFrameCallBack(CCObject* pSender_)
{
    CCArray* frame = SMainLayer->getShowAnimate()->getFrameSprites(m_lastSelectindex);
    if(frame)
    {
        if(SMainLayer->getShowAnimate()->getAllFrames()->indexOfObject(SMainLayer->getOneFrameSprArray()) == CC_INVALID_INDEX)
            SMainLayer->setOneFrameSprArrayBac();
        SMainLayer->setOneFrameSprArray(frame);
        SMainLayer->_changeEditScene("frame");
    }
}

void AnimateEditScene::doDelFrameCallBack(CCObject* pSender_)
{
    if(SMainLayer->getAllFramesList()->deletIconByShowIndex(m_lastSelectindex))
    {
        SMainLayer->getShowAnimate()->removeFormTargetLayer();
        SMainLayer->getShowAnimate()->deleteFrame(SMainLayer->getAllFramesList()->getTrueIndexByShowIndex(m_lastSelectindex));
        RemoveSprArrFromLayer(m_showFrame, this);
        m_showFrame = NULL;
        m_lastSelectindex = -1;
        if(m_listMask)
        {
            this->removeChild(m_listMask);
            m_listMask = NULL;
        }
        SMainLayer->getShowAnimate()->runWithTargetLayer(this);
    }
}

void AnimateEditScene::copyFrameCallBack(cocos2d::CCObject *pSender_)
{
   // SMainLayer->getShowAnimate()->removeFormTargetLayer();
    if(m_lastSelectindex >= 0 && m_lastSelectindex <  SMainLayer->getAllFramesList()->getSize())
    {
        SMainLayer->getShowAnimate()->copyFrame(m_lastSelectindex);
        SMainLayer->getAllFramesList()->copyIndex(m_lastSelectindex);
    }
 //   SMainLayer->getShowAnimate()->runWithTargetLayer(this);
}

void AnimateEditScene::doUpFrameCallBack(CCObject* pSender_)
{
    int trueIndex = SMainLayer->getAllFramesList()->getTrueIndexByShowIndex(m_lastSelectindex);
    if( trueIndex > 0 && trueIndex < SMainLayer->getAllFramesList()->getSize() )
    {
        SMainLayer->getShowAnimate()->swapFrame(trueIndex, trueIndex - 1);
        SMainLayer->getAllFramesList()->swapIndex(trueIndex, trueIndex - 1);
        --m_lastSelectindex;
        if(m_listMask)
            m_listMask->setPosition(SMainLayer->getAllFramesList()->getIconPositionByShowIndex(m_lastSelectindex));
    }
}

void AnimateEditScene::doDownFrameCallBack(CCObject* pSender_)
{
    int trueIndex = SMainLayer->getAllFramesList()->getTrueIndexByShowIndex(m_lastSelectindex);
    if( trueIndex >= 0 && trueIndex < (SMainLayer->getAllFramesList()->getSize() - 1) )
    {
        SMainLayer->getShowAnimate()->swapFrame(trueIndex, trueIndex + 1);
        SMainLayer->getAllFramesList()->swapIndex(trueIndex, trueIndex + 1);
        ++m_lastSelectindex;
        if(m_listMask)
            m_listMask->setPosition(SMainLayer->getAllFramesList()->getIconPositionByShowIndex(m_lastSelectindex));
    }
}

void AnimateEditScene::framelistLeftCallBack(CCObject* pSender_)
{
    SMainLayer->getAllFramesList()->goLeftPage();
}

void AnimateEditScene::framelistRightCallBack(CCObject* pSender_)
{
    SMainLayer->getAllFramesList()->goRightPage();
}

void AnimateEditScene::playAnimateCallBack(CCObject* pSender_)
{
    RemoveSprArrFromLayer(m_showFrame, this);
    m_showFrame = NULL;
    SMainLayer->getShowAnimate()->removeFormTargetLayer();
    SMainLayer->getShowAnimate()->runWithTargetLayer(this);
    SMainLayer->getShowAnimate()->play();
}

void AnimateEditScene::stopAnimateCallBack(CCObject* pSender_)
{
    SMainLayer->getShowAnimate()->stop();
}
////////////////////
