#include "RectEdit.h"

#include "EgCommon.h"
#include "MainEditLayer.h"

using namespace cocos2d;
USING_NS_CC_EXT;

RectEditScene::RectEditScene():m_lastSelectindex(-1),m_nowSprMask(NULL),m_nowUseBigSprite(NULL)
{
}

RectEditScene::~RectEditScene()
{
}

void RectEditScene::initUi()
{
    CCSize size = CCDirector::sharedDirector()->getWinSize();

    m_nowUseBigSprite = CCSprite::create();
    m_nowUseBigSprite->retain();
    m_nowUseBigSprite->setPosition(ccp(size.width/2, size.height/2));
    this->addChild(m_nowUseBigSprite);
    
    this->addChild(m_thisMenu, UI_BACK_ELEMENTS_Z_ORDER);
    
    m_glDrawLayer = new GLDrawLayer();
    
    this->addChild(m_glDrawLayer);
    
    AddFontBtn(ZH2U("添加所选区"), this, menu_selector(RectEditScene::addCutRectCallBack),
                       ccp(size.width - UI_BACK_WIDTH * 0.5f,  size.height * 0.95f), m_thisMenu);
    AddFontBtn(ZH2U("切割表"), this, menu_selector(RectEditScene::cutPicCallBack),
                       ccp(size.width - UI_BACK_WIDTH * 0.5f,  size.height * 0.9f), m_thisMenu);
    AddFontBtn(ZH2U("选图表"), this, menu_selector(RectEditScene::selPicCallBack),
                       ccp(size.width - UI_BACK_WIDTH * 0.85f,  size.height * 0.9f), m_thisMenu);
    AddFontBtn(ZH2U("删除"), this, menu_selector(RectEditScene::delRectCallBack),
                       ccp(size.width - UI_BACK_WIDTH * 0.15f,  size.height * 0.9f), m_thisMenu);
    AddFontBtn(ZH2U("->"), this, menu_selector(RectEditScene::rectlistRightCallBack),
                       ccp(size.width - UI_BACK_WIDTH + 0.9f * UI_BACK_WIDTH,  size.height * 0.87f), m_thisMenu);
    AddFontBtn(ZH2U("<-"), this, menu_selector(RectEditScene::rectlistLeftCallBack),
                       ccp(size.width - UI_BACK_WIDTH + 0.1f * UI_BACK_WIDTH,  size.height * 0.87f), m_thisMenu);
    
    
    CCLabelTTF* xlable = CCLabelTTF::create("X:", "微软雅黑", 22);
    xlable->setPosition(ccp(size.width - UI_BACK_WIDTH * 2 + 10.0f,  size.height * 0.1f));
    addChild(xlable, UI_BACK_ELEMENTS_Z_ORDER);
    
    CCLabelTTF* ylable = CCLabelTTF::create("Y:", "微软雅黑", 22);
    ylable->setPosition(ccp(size.width - UI_BACK_WIDTH * 2 + UI_BACK_WIDTH * 0.55f,  size.height * 0.1f));
    addChild(ylable, UI_BACK_ELEMENTS_Z_ORDER);
    
    CCLabelTTF* wlable = CCLabelTTF::create("W:", "微软雅黑", 22);
    wlable->setPosition(ccp(size.width - UI_BACK_WIDTH  * 2+ 10.0f,  size.height * 0.18f));
    addChild(wlable, UI_BACK_ELEMENTS_Z_ORDER);
    
    CCLabelTTF* hlable = CCLabelTTF::create("H:", "微软雅黑", 22);
    hlable->setPosition(ccp(size.width - UI_BACK_WIDTH * 2 + UI_BACK_WIDTH * 0.55f,  size.height * 0.18f));
    addChild(hlable, UI_BACK_ELEMENTS_Z_ORDER);

    
    m_nowState = SELECT_BIGSPR;
    m_nowUseList = NULL;
    m_nowContactArr = NULL;
}

void RectEditScene::releaseUi()
{
    BaseEditScene::releaseUi();
    delete m_glDrawLayer;
    m_glDrawLayer = NULL;
    m_nowUseBigSprite->release();
    m_nowUseBigSprite = NULL;
    m_nowSprMask = NULL;
    m_lastSelectindex = -1;
}

void RectEditScene::showOnDestLayer(cocos2d::CCLayer* dest_, const int& lay_)
{
    BaseEditScene::showOnDestLayer(dest_, lay_);
    _go_to_edit(m_nowState);
    
    CCSize size = CCDirector::sharedDirector()->getWinSize();
    

    m_editRectX = AddNumEditBox(-1.2f, 0.1f, this, this, 0.4);
    

    m_editRectY = AddNumEditBox(-0.73f, 0.1f, this, this, 0.4);
    

    m_editRectW = AddNumEditBox(-1.2f, 0.18f, this, this, 0.4);
    

    m_editRectH = AddNumEditBox(-0.73f, 0.18f, this, this, 0.4);
}

void RectEditScene::closeOnDestLayer()
{
    removeChild(m_editRectH);
    removeChild(m_editRectW);
    removeChild(m_editRectX);
    removeChild(m_editRectY);
    
    BaseEditScene::closeOnDestLayer();
    this->removeChild(m_nowUseList->getMainLayer());
}

bool RectEditScene::ccTouchBegan(cocos2d::CCTouch* touch_)
{
    CCSize size = CCDirector::sharedDirector()->getWinSize();
    
    CCPoint touchPoint  =  touch_->getLocation();
    CCSprite* touchSpr = CheckSpriteTouch(m_nowUseList->getShowList(), touchPoint, m_lastSelectindex);
    CCSprite* pSpr = NULL;
    if(touchSpr)
        pSpr = dynamic_cast<CCSprite*> (m_nowContactArr->objectAtIndex(m_lastSelectindex));
    
    if(touchSpr)
    {
        if (m_nowSprMask)
            this->removeChild(m_nowSprMask);
        m_nowSprMask = CreateAndAddMask(touchSpr, this);
        
        if(pSpr && m_nowState == SELECT_BIGSPR)
        {
            m_nowUseBigSprite->setTexture(pSpr->getTexture());
            m_nowUseBigSprite->setTextureRect(pSpr->getTextureRect());
        }
        
        m_glDrawLayer->setDrawRectStart(ccp(size.width/2, size.height/2));
        m_glDrawLayer->setDrawRectEnd(ccp(size.width/2, size.height/2));
        
        return false;
    }
    
    CCPoint bigMin = ccp(m_nowUseBigSprite->getPosition().x - m_nowUseBigSprite->getTextureRect().size.width/2,
                         m_nowUseBigSprite->getPosition().y - m_nowUseBigSprite->getTextureRect().size.height/2);
    CCPoint bigMax = ccp(m_nowUseBigSprite->getPosition().x + m_nowUseBigSprite->getTextureRect().size.width/2,
                         m_nowUseBigSprite->getPosition().y + m_nowUseBigSprite->getTextureRect().size.height/2);
    CCPoint selBegin;
    GetEdge2DPoint(bigMin, bigMax,  touchPoint,  selBegin);
    m_glDrawLayer->setDrawRectStart(selBegin);
    m_glDrawLayer->setDrawRectEnd(selBegin);
    
    return true;
}

void RectEditScene::ccTouchMoved(cocos2d::CCTouch* touch_)
{
    CCPoint touchPoint  =  touch_->getLocation();
    CCPoint touchOffset  =  ccpSub(touchPoint, touch_->getPreviousLocation());
     m_glDrawLayer->setDrawRectEnd(touchPoint);
    _updateRectSelect();
    //m_glDrawLayer->move(touchOffset);
    
}

void RectEditScene::ccTouchEnded(cocos2d::CCTouch* touch_)
{
    CCPoint touchPoint  =  touch_->getLocation();
    if(m_nowUseBigSprite)
    {
        CCPoint bigMin = ccp(m_nowUseBigSprite->getPosition().x - m_nowUseBigSprite->getTextureRect().size.width/2,
                             m_nowUseBigSprite->getPosition().y - m_nowUseBigSprite->getTextureRect().size.height/2);
        CCPoint bigMax = ccp(m_nowUseBigSprite->getPosition().x + m_nowUseBigSprite->getTextureRect().size.width/2,
                             m_nowUseBigSprite->getPosition().y + m_nowUseBigSprite->getTextureRect().size.height/2);
        CCPoint selectEnd;
        GetEdge2DPoint(bigMin, bigMax,  touchPoint,  selectEnd);
        MakeRect(m_glDrawLayer->getDrawRectStart().x, m_glDrawLayer->getDrawRectStart().y, selectEnd.x, selectEnd.y);

        m_glDrawLayer->setDrawRectEnd(selectEnd);
        m_nowSelectRect = m_glDrawLayer->getNowRect();
        
        m_nowSelectRect.origin.y = bigMax.y - m_nowSelectRect.origin.y;
        m_nowSelectRect.origin.x -= bigMin.x;
    }
}

void RectEditScene::editBoxEditingDidBegin(cocos2d::extension::CCEditBox* editBox)
{}

void RectEditScene::editBoxEditingDidEnd(cocos2d::extension::CCEditBox* editBox_)
{}

void RectEditScene::editBoxTextChanged(cocos2d::extension::CCEditBox* editBox_, const std::string& text_)
{
    CCRect rect = m_glDrawLayer->getNowRect();
    editBox_->getText();
    float var(0);
    sscanf(text_.c_str(), "%f",  &var);
    
    if (editBox_ == m_editRectW)
    {
        rect.size.width = var;
        m_glDrawLayer->setDrawRect(rect);
    }
    else if (editBox_ == m_editRectH)
    {
        rect.size.height = var;
        m_glDrawLayer->setDrawRect(rect);
    }
    else if (editBox_ == m_editRectX)
    {
        rect.origin.x = m_nowUseBigSprite->getPositionX() - m_nowUseBigSprite->getTextureRect().size.width/2 +  var;
        m_glDrawLayer->setDrawRect(rect);
    }
    else if (editBox_ == m_editRectY)
    {
        rect.origin.y = m_nowUseBigSprite->getPositionY() + m_nowUseBigSprite->getTextureRect().size.height/2 -  var;
        m_glDrawLayer->setDrawRect(rect);
    }
}

void RectEditScene::editBoxReturn(cocos2d::extension::CCEditBox* editBox_)
{}
//-----------------btn call back-----------------

void RectEditScene::addCutRectCallBack(cocos2d::CCObject *pSender_)
{
    if(m_nowState != CUT_RECT)
        _go_to_edit(CUT_RECT);
    
    CCSprite* add = CCSprite::createWithTexture(m_nowUseBigSprite->getTexture(), m_nowSelectRect);
    SMainLayer->getCutRectArray()->addObject(add);
    
    CCSprite* detail = CCSprite::createWithTexture(m_nowUseBigSprite->getTexture(), m_nowSelectRect);
    SMainLayer->getCutRectList()->addList(detail);
}

void RectEditScene::selPicCallBack(cocos2d::CCObject *pSender_)
{
    _go_to_edit(SELECT_BIGSPR);
}

void RectEditScene::cutPicCallBack(cocos2d::CCObject *pSender_)
{
    _go_to_edit(CUT_RECT);
}

void RectEditScene::delRectCallBack(cocos2d::CCObject *pSender_)
{
    if(m_nowUseList->deletIconByShowIndex(m_lastSelectindex))
    {
        m_nowContactArr->removeObjectAtIndex(m_nowUseList->getTrueIndexByShowIndex(m_lastSelectindex));
        if (m_nowSprMask)
        {
            this->removeChild(m_nowSprMask);
            m_nowSprMask = NULL;
        }
    }
}

void RectEditScene::rectlistLeftCallBack(cocos2d::CCObject *pSender_)
{
    m_nowUseList->goLeftPage();
}

void RectEditScene::rectlistRightCallBack(cocos2d::CCObject *pSender_)
{
    m_nowUseList->goRightPage();
}


//------------------private function----------------

void RectEditScene::_go_to_edit(const RectEditScene::EDIT_STATE& state_)
{
    m_nowState = state_;
    if (m_nowSprMask)
        this->removeChild(m_nowSprMask);
    m_lastSelectindex = -1;
    switch (m_nowState)
    {
        case SELECT_BIGSPR:
            if(m_nowUseList)
                this->removeChild(m_nowUseList->getMainLayer());
            m_nowUseList = SMainLayer->getUseBigSprList();
            m_nowContactArr = SMainLayer->getUseBigSprArray();
            this->addChild(m_nowUseList->getMainLayer(), UI_BACK_ELEMENTS_Z_ORDER);
            break;
        case CUT_RECT:
            if(m_nowUseList)
                this->removeChild(m_nowUseList->getMainLayer());
            m_nowUseList = SMainLayer->getCutRectList();
            m_nowContactArr = SMainLayer->getCutRectArray();
            this->addChild(m_nowUseList->getMainLayer(), UI_BACK_ELEMENTS_Z_ORDER);
            break;
        default:
            break;
    }
}

void RectEditScene::_updateRectSelect()
{
    CCRect dlRect = m_glDrawLayer->getNowRect();
    float w = dlRect.size.width;
    float h = dlRect.size.height;
    float x = dlRect.origin.x -
                  (m_nowUseBigSprite->getPositionX() - m_nowUseBigSprite->getTextureRect().size.width/2);
    float y = m_nowUseBigSprite->getPositionY() + m_nowUseBigSprite->getTextureRect().size.height/2 -
                  dlRect.origin.y;
    
    if(w > m_nowUseBigSprite->getTextureRect().size.width)
        w = m_nowUseBigSprite->getTextureRect().size.width;
    
    if(h > m_nowUseBigSprite->getTextureRect().size.height)
        h = m_nowUseBigSprite->getTextureRect().size.height;

    if(x < 0)
        x = 0;
    else if(x > m_nowUseBigSprite->getTextureRect().size.width)
        x = x > m_nowUseBigSprite->getTextureRect().size.width;
    if(y < 0)
        y = 0;
    else if(y > m_nowUseBigSprite->getTextureRect().size.height)
        y = y > m_nowUseBigSprite->getTextureRect().size.height;
    const int boxChars = 15;
    
    char toBox[boxChars] = {0};
    sprintf(toBox, "%d",  (int)w);
    m_editRectW->setText(toBox);
    sprintf(toBox, "%d",  (int)h);
    m_editRectH->setText(toBox);
    sprintf(toBox, "%d",  (int)x);
    m_editRectX->setText(toBox);
    sprintf(toBox, "%d",  (int)y);
    m_editRectY->setText(toBox);
}
