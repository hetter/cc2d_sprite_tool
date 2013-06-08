#include "FrameEdit.h"

#include "EgCommon.h"
#include "MainEditLayer.h"

using namespace cocos2d;
USING_NS_CC_EXT;

FrameEditScene::FrameEditScene()
{}

FrameEditScene::~FrameEditScene()
{}

void FrameEditScene::initUi()
{
    m_nowUseList = NULL;
    m_nowContactArr = NULL;
    m_oneFrameSprs = NULL;
    m_listMask = NULL;
    m_sprMask = NULL;
    m_nowEditUi = NULL;
    m_touchMoveSpr = NULL;
    
    m_editFrameSpeed = NULL;
    
    m_editUiMap[DETAIL_SET] = CCLayer::create();
    m_editUiMap[DETAIL_SET]->retain();
    m_editUiMap[FRAME_SPR_VIEW] = CCLayer::create();
    m_editUiMap[FRAME_SPR_VIEW]->retain();
    m_editUiMap[MOVE_ELEMENT] = CCLayer::create();
    m_editUiMap[MOVE_ELEMENT]->retain();
    
    _addUi();
    
    //_addUiOnDetailSet(); // To avoid editbox show earlyer(is it a bug?)
    
    m_glDrawLayer = new GLDrawLayer();
    
    m_nowState = MOVE_ELEMENT;
    m_detailClick = MOV_DETAIL;
    
    CCSize size = CCDirector::sharedDirector()->getWinSize();
    

    
    m_inputFrameSpeed = 15.0f;
}

void FrameEditScene::releaseUi()
{
    for (LayerMap::iterator i = m_editUiMap.begin(); i != m_editUiMap.end() ; ++i)
    {
        i->second->release();
    }
    m_editUiMap.clear();
    BaseEditScene::releaseUi();
    delete m_glDrawLayer;
}

void FrameEditScene::showOnDestLayer(cocos2d::CCLayer *dest_, const int& lay_)
{
    BaseEditScene::showOnDestLayer(dest_, lay_);
    m_oneFrameSprs = SMainLayer->getOneFrameSprArray();
    AddSprArrToLayer(m_oneFrameSprs, this);
    if(m_editFrameSpeed == NULL)
    {
        m_editFrameSpeed = AddNumEditBox(0.2f, 0.18f, this, this);
        char toBox[15] = {0};
        sprintf(toBox, "%.2f",  m_inputFrameSpeed);
        m_editFrameSpeed->setText(toBox);
    }
    _go_to_edit(m_nowState);
}

void FrameEditScene::closeOnDestLayer()
{
    BaseEditScene::closeOnDestLayer();
    RemoveSprArrFromLayer(m_oneFrameSprs, this);
    if(m_nowUseList)
        this->removeChild(m_nowUseList->getMainLayer());
    if(m_editFrameSpeed)
    {
        removeChild(m_editFrameSpeed);
        m_editFrameSpeed = NULL;
    }
}


bool FrameEditScene::ccTouchBegan(cocos2d::CCTouch* touch_)
{
    //m_touchMoveSpr = NULL;
    m_bIsAddSprToFrame = false;
    
    CCPoint touchPoint  =  touch_->getLocation();
    
    CCSprite* touchSpr = NULL;
    if(m_nowUseList)
        touchSpr = CheckSpriteTouch(m_nowUseList->getShowList(), touchPoint, m_lastSelectindex);
    
    switch (m_nowState)
    {
        case MOVE_ELEMENT:
        {            
            CCSprite* pSpr = NULL;
            if(touchSpr)
            {
                pSpr = dynamic_cast<CCSprite*> (m_nowContactArr->objectAtIndex(m_lastSelectindex));
                //create tag
                m_bIsAddSprToFrame = true;
                m_touchMoveSpr = CCSprite::createWithTexture(pSpr->getTexture(), pSpr->getTextureRect());
                m_touchMoveSpr->setPosition(touchSpr->getPosition());
                addChild(m_touchMoveSpr, UI_BACK_ELEMENTS_Z_ORDER + 100);
                //if (m_sprMask)
                //    this->removeChild(m_sprMask);
                //m_sprMask = CreateAndAddMask(m_touchMoveSpr, this);
                
                if (m_listMask)
                    this->removeChild(m_listMask);
                m_listMask = CreateAndAddMask(touchSpr, this);
                return true;
            }
        }
            break;
        case FRAME_SPR_VIEW:
        {
            CCSprite* pSpr = NULL;
            if(touchSpr)
            {
                pSpr = dynamic_cast<CCSprite*> (m_nowContactArr->objectAtIndex(m_lastSelectindex));
                m_touchMoveSpr = pSpr;
                if (m_sprMask)
                    this->removeChild(m_sprMask);
                m_sprMask = CreateAndAddMask(m_touchMoveSpr, this);                
                if (m_listMask)
                    this->removeChild(m_listMask);
                m_listMask = CreateAndAddMask(touchSpr, this);
                return false;
            }
        }
        case DETAIL_SET:
            if (m_detailClick == ROT_DETAIL)
            {
                //todo;
            }
            else if(m_detailClick == BOX_DETAIL)
            {
                if(m_glDrawLayer)
                {
                    this->removeChild(m_glDrawLayer);
                    m_glDrawLayer->setDrawRectStart(touchPoint);
                    m_glDrawLayer->setDrawRectEnd(touchPoint);
                    addChild(m_glDrawLayer, UI_BACK_PIC_Z_ORDER);
                }
                return true;
            }
            break;
        default:
            break;
    }
    
    int temp = -1;
    touchSpr = CheckSpriteTouch(m_oneFrameSprs, touchPoint, temp);
    
    if(touchSpr)
    {
        m_touchMoveSpr = touchSpr;
        if (m_sprMask)
            this->removeChild(m_sprMask);
        m_sprMask = CreateAndAddMask(touchSpr, this);
        return true;
    }
    
    return false;
}

void FrameEditScene::ccTouchMoved(cocos2d::CCTouch* touch_)
{
    CCPoint touchPoint  =  touch_->getLocation();
    CCPoint touchOffset  =  ccpSub(touchPoint, touch_->getPreviousLocation());
    CCSize size = CCDirector::sharedDirector()->getWinSize();
    
    if(DETAIL_SET == m_nowState)
    {
        if (ROT_DETAIL == m_detailClick)
        {
            if(!m_touchMoveSpr)
                return;
            
            float o = touchPoint.x - m_touchMoveSpr->getPosition().x;
            float a = touchPoint.y - m_touchMoveSpr->getPosition().y;
            float at = (float) CC_RADIANS_TO_DEGREES( atanf( o/a) );
            if( a < 0 )
                // at += 180;
            {
                if(  o < 0 )
                    at = 180 + fabs(at);
                else
                    at = 180 - fabs(at);
            }
            char toBox[10] = {0};
            m_touchMoveSpr->setRotation(at);
            sprintf(toBox, "%d",  (int)m_touchMoveSpr->getRotation());
            m_editNowSprRot->setText(toBox);
            
            return;
        }
        else if(BOX_DETAIL == m_detailClick)
        {
            m_glDrawLayer->setDrawRectEnd(touchPoint);
            return;
        }
        else if(MOV_DETAIL == m_detailClick )
        {
            //todo
        }
    }
    
    if(m_touchMoveSpr)
    {
        CCPoint newPos = ccpAdd(m_touchMoveSpr->getPosition(), touchOffset);
        m_touchMoveSpr->setPosition(newPos);
        if(!m_bIsAddSprToFrame)
            m_sprMask->setPosition(newPos);
        
        //delete work
        if (m_touchMoveSpr->getPosition().x > size.width - UI_BACK_WIDTH)
        {
            SMainLayer->getBackUiPic()->setOpacity(50);//change alpha to prompt delete work
        }
        else
        {
            SMainLayer->getBackUiPic()->setOpacity(255);
        }
    }
}

void FrameEditScene::ccTouchEnded(cocos2d::CCTouch* touch_)
{
    SMainLayer->getBackUiPic()->setOpacity(255);//recover color
    CCSize size = CCDirector::sharedDirector()->getWinSize();
    
    if(DETAIL_SET == m_nowState)
    {
        if(ROT_DETAIL == m_detailClick)
        {
            return;
        }
        else if(BOX_DETAIL == m_detailClick)
        {
            m_glDrawLayer->setDrawPoint(ccp((size.width - UI_BACK_WIDTH)/2, size.height/2));
            float w = m_glDrawLayer->getNowRect().size.width;
            float h = m_glDrawLayer->getNowRect().size.height;
            
            SetBoxTextByVar((int)h, m_editFrameRH);
            SetBoxTextByVar((int)w, m_editFrameRW);
            return;
        }
        else if(MOV_DETAIL == m_detailClick )
        {
            //
        }

    }
    
    if(m_touchMoveSpr)
    {
        //////delete work///////
        if (m_touchMoveSpr->getPosition().x > size.width - UI_BACK_WIDTH)
        {
            this->removeChild(m_touchMoveSpr);
            if(!m_bIsAddSprToFrame)
            {
                int removeIndex = SMainLayer->getOneFrameSprArray()->indexOfObject(m_touchMoveSpr);
                SMainLayer->getOneFrameSprArray()->removeObject(m_touchMoveSpr);
                SMainLayer->getOneFrameSprList()->deletIconByTrueIndex(removeIndex);
                m_touchMoveSpr = NULL;
            }
        }
        ///////////////////////
        else
        {
            if (m_bIsAddSprToFrame)
            {
                m_touchMoveSpr->setZOrder(0);
                SMainLayer->getOneFrameSprArray()->addObject(m_touchMoveSpr);
                SMainLayer->getOneFrameSprList()->addList(CCSprite::createWithTexture(m_touchMoveSpr->getTexture(), m_touchMoveSpr->getTextureRect()));
                m_bIsAddSprToFrame = false;
            }
            _updateDetailSet();
        }
        
    }
}


void FrameEditScene::editBoxEditingDidBegin(cocos2d::extension::CCEditBox* editBox)
{}

void FrameEditScene::editBoxEditingDidEnd(cocos2d::extension::CCEditBox* editBox_)
{}

void FrameEditScene::editBoxTextChanged(cocos2d::extension::CCEditBox* editBox_, const std::string& text_)
{
    
    //detail set
    if(m_touchMoveSpr)
    {
        if (editBox_ == m_editNowSprX)
        {
            int posX = 0;
            sscanf(text_.c_str(), "%d",  &posX);
            m_sprMask->setPositionX(float(posX));
            m_touchMoveSpr->setPositionX(float(posX));
        }
        else if (editBox_ == m_editNowSprY)
        {
            int posY = 0;
            sscanf(text_.c_str(), "%d",  &posY);            
            m_touchMoveSpr->setPositionY(float(posY));
            m_sprMask->setPositionY(float(posY));
        }
        else if (editBox_ == m_editNowSprRot)
        {
            int rot = 0;
            sscanf(text_.c_str(), "%d",  &rot);
            m_touchMoveSpr->setRotation(float(rot));
        }
        else if(editBox_ == m_editNowSprLayer)
        {
            int layer = 0;
            sscanf(text_.c_str(), "%d",  &layer);
            m_touchMoveSpr->setZOrder(layer);
        }
        else if (editBox_ == m_editNowSprAlpha)
        {
            int alpha = 255;
            sscanf(text_.c_str(), "%d",  &alpha);
            m_sliderAlpha->setValue(alpha);
        }
        else if (editBox_ == m_editNowSprScale)
        {
            int scale = 255;
            sscanf(text_.c_str(), "%d",  &scale);
            m_sliderScale->setValue(scale);
        }
        else if(editBox_ == m_editFrameRH || editBox_ == m_editFrameRW)
        {
            int rw(512), rh(512);
            sscanf(m_editFrameRW->getText(), "%d", &rw);
            sscanf(m_editFrameRH->getText(), "%d", &rh);
            CCSize size = CCDirector::sharedDirector()->getWinSize();
            CCPoint start = ccp((size.width - UI_BACK_WIDTH)/2 + rw/2, size.height/2 + rh/2);
            m_glDrawLayer->setDrawRectStart(start);
            CCPoint end = ccp((size.width - UI_BACK_WIDTH)/2 - rw/2, size.height/2 - rh/2);
            m_glDrawLayer->setDrawRectEnd(end);
        }
        else if (editBox_ == m_editFrameSpeed)
        {
            sscanf(text_.c_str(), "%f",  &m_inputFrameSpeed);
        }
    }
}

void FrameEditScene::editBoxReturn(cocos2d::extension::CCEditBox* editBox_)
{}
//======callback function======
void FrameEditScene::goToMoveElmCallBack(cocos2d::CCObject* pSender_)
{
    _go_to_edit(MOVE_ELEMENT);
}

void FrameEditScene::goToDetailSetCallBack(cocos2d::CCObject* pSender_)
{
    _go_to_edit(DETAIL_SET);
}

void FrameEditScene::goToFrameSprViewCallBack(cocos2d::CCObject* pSender_)
{
    _go_to_edit(FRAME_SPR_VIEW);
}

void FrameEditScene::setMovDetailCallBack(cocos2d::CCObject* pSender_)
{
    m_detailClick = MOV_DETAIL;
}

void FrameEditScene::setRotDetailCallBack(cocos2d::CCObject* pSender_)
{
    m_detailClick = ROT_DETAIL;
}

void FrameEditScene::setBoxDetailCallBack(cocos2d::CCObject* pSender_)
{
    m_detailClick = BOX_DETAIL;
}

void FrameEditScene::flipNowXCallBack(CCObject* pSender_)
{
    if(m_touchMoveSpr)
        m_touchMoveSpr->setFlipX(!m_touchMoveSpr->isFlipX());
}

void FrameEditScene::flipNowYCallBack(CCObject* pSender_)
{
    if(m_touchMoveSpr)
        m_touchMoveSpr->setFlipY(!m_touchMoveSpr->isFlipY());
}

void FrameEditScene::changeAlphaCallBack(CCObject *pSender_, CCControlEvent controlEvent_)
{
    CCControlSlider* pSlider = (CCControlSlider*)pSender_;
    if(m_touchMoveSpr)
    {
        const int boxChars = 15;
        char toBox[boxChars] = {0};
        sprintf(toBox, "%d",  (int)pSlider->getValue());
        m_editNowSprAlpha->setText(toBox);
        m_touchMoveSpr->setOpacity(pSlider->getValue());
    }
}

void FrameEditScene::changeScaleCallBack(CCObject *pSender_, CCControlEvent controlEvent_)
{
    CCControlSlider* pSlider = (CCControlSlider*)pSender_;
    if(m_touchMoveSpr)
    {
        const int boxChars = 15;
        char toBox[boxChars] = {0};
        sprintf(toBox, "%d",  (int)pSlider->getValue());
        m_editNowSprScale->setText(toBox);
        m_touchMoveSpr->setScale(pSlider->getValue() / 100.0f);
    }
}

void FrameEditScene::addFrameCallBack(CCObject* pSender_)
{
    SMainLayer->rttFrameToList(m_oneFrameSprs);
    SMainLayer->_changeEditScene("animate");    
    SMainLayer->getShowAnimate()->addFrame(m_oneFrameSprs, m_glDrawLayer->getNowRect().size, m_inputFrameSpeed);
}

void FrameEditScene::refreshCallBack(CCObject* pSender_)
{
    RemoveSprArrFromLayer(m_oneFrameSprs, this);
    SMainLayer->setOneFrameSprArray(SMainLayer->getOneFrameSprArrayBac());
    m_oneFrameSprs = SMainLayer->getOneFrameSprArray();
    AddSprArrToLayer(m_oneFrameSprs, this);
    SMainLayer->_changeEditScene("frame");
}

void FrameEditScene::delRectCallBack(cocos2d::CCObject* pSender_)
{
    if(m_nowUseList->deletIconByShowIndex(m_lastSelectindex))
    {
        int trueIndex = m_nowUseList->getTrueIndexByShowIndex(m_lastSelectindex);
        if(m_nowState == FRAME_SPR_VIEW)
            removeChild(dynamic_cast<CCSprite*> (m_nowContactArr->objectAtIndex(trueIndex)));
            
        m_nowContactArr->removeObjectAtIndex(trueIndex);
        if (m_listMask)
        {
            this->removeChild(m_listMask);
            m_listMask = NULL;
        }
    }
}

void FrameEditScene::listLeftCallBack(cocos2d::CCObject* pSender_)
{
    m_nowUseList->goLeftPage();
}

void FrameEditScene::listRightCallBack(cocos2d::CCObject* pSender_)
{
    m_nowUseList->goRightPage();
}


//======private function======
void FrameEditScene::_go_to_edit(const FrameEditScene::EDIT_STATE& state_)
{
    if(m_nowEditUi)
        removeChild(m_nowEditUi);
    
    if(m_nowUseList)
        this->removeChild(m_nowUseList->getMainLayer());
    
    if(m_listMask)
        this->removeChild(m_listMask);
    
    m_lastSelectindex = -1;
    
    m_nowState = state_;
    
    switch (m_nowState)
    {
        case MOVE_ELEMENT:
            m_nowUseList = SMainLayer->getCutRectList();
            m_nowContactArr = SMainLayer->getCutRectArray();
            this->addChild(m_nowUseList->getMainLayer(), UI_BACK_ELEMENTS_Z_ORDER);
            m_nowEditUi = m_editUiMap[MOVE_ELEMENT]; 
            break;
        case DETAIL_SET:
            m_nowUseList = NULL;
            m_nowContactArr = NULL;
            _removeUiOnDetailSet();
            m_nowEditUi = m_editUiMap[DETAIL_SET];
            _addUiOnDetailSet();
            _updateDetailSet();
            break;
        case FRAME_SPR_VIEW:
            m_nowUseList = SMainLayer->getOneFrameSprList();
            m_nowContactArr = SMainLayer->getOneFrameSprArray();
            this->addChild(m_nowUseList->getMainLayer(), UI_BACK_ELEMENTS_Z_ORDER);
            m_nowEditUi = m_editUiMap[FRAME_SPR_VIEW];
            break;
        default:
            break;
    }
    
    addChild(m_nowEditUi);
}

void FrameEditScene::_addUi()
{
    CCSize size = CCDirector::sharedDirector()->getWinSize();
    
    float nowHight = 0.96f;
    float nextGap = 0.08f;
    
    this->addChild(m_thisMenu, UI_BACK_ELEMENTS_Z_ORDER);

    AddFontBtn(ZH2U("切割表"), this, menu_selector(FrameEditScene::goToMoveElmCallBack),
                       ccp(size.width - UI_BACK_WIDTH * 0.85f,  size.height * nowHight), m_thisMenu, 20);
    AddFontBtn(ZH2U("细节单"), this, menu_selector(FrameEditScene::goToDetailSetCallBack),
                       ccp(size.width - UI_BACK_WIDTH * 0.5f,  size.height * nowHight), m_thisMenu, 20);
    AddFontBtn(ZH2U("桢图表"), this, menu_selector(FrameEditScene::goToFrameSprViewCallBack),
                       ccp(size.width - UI_BACK_WIDTH * 0.15f,  size.height * nowHight), m_thisMenu, 20);
    
    nowHight -= nextGap;
    
    CCMenu* tempMenu = CCMenu::create();
    tempMenu->setPosition(cocos2d::CCPointZero);
    m_editUiMap[MOVE_ELEMENT]->addChild(tempMenu);
    
    AddFontBtn(ZH2U("删除"), this, menu_selector(FrameEditScene::delRectCallBack),
               ccp(size.width - UI_BACK_WIDTH * 0.5f,  size.height * nowHight), tempMenu);
    AddFontBtn(ZH2U("->"), this, menu_selector(FrameEditScene::listRightCallBack),
               ccp(size.width - UI_BACK_WIDTH * 0.2f,  size.height * nowHight), tempMenu);
    AddFontBtn(ZH2U("<-"), this, menu_selector(FrameEditScene::listLeftCallBack),
               ccp(size.width - UI_BACK_WIDTH * 0.8f,  size.height * nowHight), tempMenu);

    tempMenu = CCMenu::create();
    tempMenu->setPosition(cocos2d::CCPointZero);
    m_editUiMap[FRAME_SPR_VIEW]->addChild(tempMenu);
    AddFontBtn(ZH2U("删除"), this, menu_selector(FrameEditScene::delRectCallBack),
               ccp(size.width - UI_BACK_WIDTH * 0.5f,  size.height * nowHight), tempMenu);
    AddFontBtn(ZH2U("->"), this, menu_selector(FrameEditScene::listRightCallBack),
               ccp(size.width - UI_BACK_WIDTH * 0.2f,  size.height * nowHight), tempMenu);
    AddFontBtn(ZH2U("<-"), this, menu_selector(FrameEditScene::listLeftCallBack),
               ccp(size.width - UI_BACK_WIDTH * 0.8f,  size.height * nowHight), tempMenu);
    
    CCLabelTTF* fslable = CCLabelTTF::create(ZH2U("帧速:"), "微软雅黑", 20);
    fslable->setPosition(ccp(size.width - UI_BACK_WIDTH * 0.75f, size.height * 0.18f));
    addChild(fslable, UI_BACK_ELEMENTS_Z_ORDER);
    //m_editFrameSpeed = AddNumEditBox(0.2f, 0.18f, this, this);//change add pos (is it bug?)
    

    AddFontBtn(ZH2U("重载编辑桢"), this, menu_selector(FrameEditScene::refreshCallBack),
               ccp(size.width - UI_BACK_WIDTH * 0.5f, size.height * 0.14f), m_thisMenu);
    
    AddFontBtn(ZH2U("添加动画桢"), this, menu_selector(FrameEditScene::addFrameCallBack),
               ccp(size.width - UI_BACK_WIDTH * 0.5f, size.height * 0.08f), m_thisMenu);
    //addChild(m_glDrawLayer, UI_BACK_PIC_Z_ORDER);
}

void FrameEditScene::_addUiOnDetailSet()
{
    CCSize size = CCDirector::sharedDirector()->getWinSize();
    
    //reset
    float nextGap =0.08f;
    float nowHight = 0.92f;
    CCMenu* tempMenu = CCMenu::create();
    tempMenu->setPosition(cocos2d::CCPointZero);
    m_editUiMap[DETAIL_SET]->addChild(tempMenu);
    
    CCLabelTTF* mouselable = CCLabelTTF::create(ZH2U("鼠标控制："), "微软雅黑", 22);
    mouselable->setPosition(ccp(size.width - UI_BACK_WIDTH * 0.5f,  size.height * nowHight));
    m_editUiMap[DETAIL_SET]->addChild(mouselable, UI_BACK_ELEMENTS_Z_ORDER);
    
    nowHight -= 0.03f;
    
    AddFontBtn(ZH2U("移动"), this, menu_selector(FrameEditScene::setMovDetailCallBack),
                       ccp(size.width - UI_BACK_WIDTH * 0.8f,  size.height * nowHight), tempMenu, 20);
    AddFontBtn(ZH2U("旋转"), this, menu_selector(FrameEditScene::setRotDetailCallBack),
                       ccp(size.width - UI_BACK_WIDTH * 0.5f,  size.height * nowHight), tempMenu, 20);
    AddFontBtn(ZH2U("包围盒"), this, menu_selector(FrameEditScene::setBoxDetailCallBack),
                       ccp(size.width - UI_BACK_WIDTH * 0.2f,  size.height * nowHight), tempMenu, 20);
    
    nowHight -= nextGap;
    
    CCLabelTTF* xlable = CCLabelTTF::create("X:", "微软雅黑", 22);
    xlable->setPosition(ccp(size.width - UI_BACK_WIDTH + 10.0f,  size.height * nowHight));
    m_editUiMap[DETAIL_SET]->addChild(xlable, UI_BACK_ELEMENTS_Z_ORDER);
    m_editNowSprX = AddNumEditBox(-0.2f, nowHight, this, m_editUiMap[DETAIL_SET]);
    
    CCLabelTTF* ylable = CCLabelTTF::create("Y:", "微软雅黑", 22);
    ylable->setPosition(ccp(size.width - UI_BACK_WIDTH + UI_BACK_WIDTH * 0.55f,  size.height * nowHight));
    m_editUiMap[DETAIL_SET]->addChild(ylable, UI_BACK_ELEMENTS_Z_ORDER);
    m_editNowSprY = AddNumEditBox(0.28f, nowHight, this, m_editUiMap[DETAIL_SET]);
    
    nowHight -= nextGap;
    
    CCLabelTTF* rlable = CCLabelTTF::create(ZH2U("角:"), "微软雅黑", 20);
    rlable->setPosition(ccp(size.width - UI_BACK_WIDTH + 11.0f,  size.height * nowHight));
    m_editUiMap[DETAIL_SET]->addChild(rlable, UI_BACK_ELEMENTS_Z_ORDER);
    m_editNowSprRot = AddNumEditBox(-0.2f, nowHight, this, m_editUiMap[DETAIL_SET]);
    
    CCLabelTTF* layerlable = CCLabelTTF::create(ZH2U("层:"), "微软雅黑", 20);
    layerlable->setPosition(ccp(size.width - UI_BACK_WIDTH + UI_BACK_WIDTH * 0.55f,  size.height * nowHight));
    m_editUiMap[DETAIL_SET]->addChild(layerlable, UI_BACK_ELEMENTS_Z_ORDER);
    m_editNowSprLayer = AddNumEditBox(0.28f, nowHight, this, m_editUiMap[DETAIL_SET]);
    
    nowHight -= nextGap;
    
    CCMenuItemFont* flipXBtn = CCMenuItemFont::create(ZH2U("X翻转"), this, menu_selector(FrameEditScene::flipNowXCallBack));
    flipXBtn->setPosition(ccp(size.width - UI_BACK_WIDTH + 60.0f,  size.height * nowHight));
    flipXBtn->setFontSize(22);
    tempMenu->addChild(flipXBtn);
    
    CCMenuItemFont* flipYBtn = CCMenuItemFont::create(ZH2U("Y翻转"), this, menu_selector(FrameEditScene::flipNowYCallBack));
    flipYBtn->setPosition(ccp(size.width - UI_BACK_WIDTH + 160.0f,  size.height * nowHight));
    flipYBtn->setFontSize(22);
    tempMenu->addChild(flipYBtn);
    
    nowHight -= nextGap;
    
    CCLabelTTF* alphaLable = CCLabelTTF::create(ZH2U("透明度:"), "微软雅黑", 22);
    alphaLable->setPosition(ccp(size.width - UI_BACK_WIDTH + 50.0f,  size.height * nowHight));
    m_editUiMap[DETAIL_SET]->addChild(alphaLable, UI_BACK_ELEMENTS_Z_ORDER);
    m_editNowSprAlpha = AddNumEditBox(0.12f, nowHight, this, m_editUiMap[DETAIL_SET]);
    
    nowHight -= 0.03f;
    
    m_sliderAlpha = AddSliderGroup(0.5f, nowHight, 0, 255, 255,
                                                        cccontrol_selector(FrameEditScene::changeAlphaCallBack),
                                                        this, m_editUiMap[DETAIL_SET]);
    
    nowHight -= nextGap;
    
    CCLabelTTF* scaleLable = CCLabelTTF::create(ZH2U("拉伸:"), "微软雅黑", 22);
    scaleLable->setPosition(ccp(size.width - UI_BACK_WIDTH + 35.0f,  size.height * nowHight));
    m_editUiMap[DETAIL_SET]->addChild(scaleLable, UI_BACK_ELEMENTS_Z_ORDER);
    CCLabelTTF* rateLable = CCLabelTTF::create(ZH2U("%"), "微软雅黑", 22);
    rateLable->setPosition(ccp(size.width - UI_BACK_WIDTH + 145.0f,  size.height * nowHight));
    m_editUiMap[DETAIL_SET]->addChild(rateLable, UI_BACK_ELEMENTS_Z_ORDER);
    
    m_editNowSprScale = AddNumEditBox(0.0f, nowHight, this, m_editUiMap[DETAIL_SET]);
    
    nowHight -= 0.03f;
    
    m_sliderScale = AddSliderGroup(0.5f, nowHight, 10.0f, 300.0f, 100.0f,
                                                      cccontrol_selector(FrameEditScene::changeScaleCallBack),
                                                      this, m_editUiMap[DETAIL_SET]);
    
    nowHight -= nextGap;
    
    CCLabelTTF* wlable = CCLabelTTF::create("RW:", "微软雅黑", 22);
    wlable->setPosition(ccp(size.width - UI_BACK_WIDTH + 10.0f,  size.height * nowHight));
    m_editUiMap[DETAIL_SET]->addChild(wlable, UI_BACK_ELEMENTS_Z_ORDER);
    m_editFrameRW = AddNumEditBox(-0.2f, nowHight, this, m_editUiMap[DETAIL_SET]);
    
    CCLabelTTF* hlable = CCLabelTTF::create("RH:", "微软雅黑", 22);
    hlable->setPosition(ccp(size.width - UI_BACK_WIDTH + UI_BACK_WIDTH * 0.55f,  size.height * nowHight));
    m_editUiMap[DETAIL_SET]->addChild(hlable, UI_BACK_ELEMENTS_Z_ORDER);
    m_editFrameRH = AddNumEditBox(0.28f, nowHight, this, m_editUiMap[DETAIL_SET]);
    
    nowHight -= nextGap;
    
    const int boxChars = 15;
    char toBox[boxChars];
    sprintf(toBox, "%d",  (int)SMainLayer->getShowAnimate()->getBoundBox().size.width);
    m_editFrameRW->setText(toBox);
    memset(toBox, sizeof(char) * boxChars, 0);
    sprintf(toBox, "%d",  (int)SMainLayer->getShowAnimate()->getBoundBox().size.height);
    m_editFrameRH->setText(toBox);
}

void FrameEditScene::_removeUiOnDetailSet()
{
    m_editUiMap[DETAIL_SET]->removeAllChildren();
}

void FrameEditScene::_updateDetailSet()
{
    if(!m_touchMoveSpr || m_nowState != DETAIL_SET)
        return;
    
    const int boxChars = 15;
    char toBox[boxChars] = {0};
    
    sprintf(toBox, "%d",  (int)m_touchMoveSpr->getPositionX());
    m_editNowSprX->setText(toBox);
    memset(toBox, sizeof(char) * boxChars, 0);
    sprintf(toBox, "%d",  (int)m_touchMoveSpr->getPositionY());
    m_editNowSprY->setText(toBox);
    memset(toBox, sizeof(char) * boxChars, 0);
    sprintf(toBox, "%d",  (int)m_touchMoveSpr->getRotation());
    m_editNowSprRot->setText(toBox);
    memset(toBox, sizeof(char) * boxChars, 0);
    sprintf(toBox, "%d",  (int)m_touchMoveSpr->getZOrder());
    m_editNowSprLayer->setText(toBox);
    
    m_sliderAlpha->setValue(m_touchMoveSpr->getOpacity());
    m_sliderScale->setValue(m_touchMoveSpr->getScale() * 100);
}