#include "MainEditLayer.h"
#include "SimpleAudioEngine.h"
#include "GUI/CCEditBox/CCEditBox.h"

using namespace cocos2d;
using namespace CocosDenshion;
USING_NS_CC_EXT;

std::string G2U(const char* gb2312)    
{    
    int len = MultiByteToWideChar(CP_ACP, 0, gb2312, -1, NULL, 0);    
    wchar_t* wstr = new wchar_t[len+1];    
    memset(wstr, 0, len+1);    
    MultiByteToWideChar(CP_ACP, 0, gb2312, -1, wstr, len);    
    len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);    
    char* str = new char[len+1];    
    memset(str, 0, len+1);    
    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);    
    if(wstr) delete[] wstr;  
    std::string ret(str);
    delete[] str;
    return ret;    
}  

CCSprite* CreatePureColorSpr(const int& w_, const int& h_, const unsigned int& col_)
{
    GLubyte alpha = (col_ & 0xff000000) >> 24;
    GLubyte r = (col_ & 0x00ff0000) >> 16;
    GLubyte g = (col_ & 0x0000ff00) >> 8;
    GLubyte b = (col_ & 0x000000ff) >> 0;

    GLubyte* pixels;
    int totalBytes = h_ * w_ * 3;//rgb888
    pixels = new GLubyte[totalBytes];

    for (int i = 0; i < h_; ++i)
    {
        for (int j = 0; j < w_*3; j += 3)
        {
            int index = i * w_ *3 + j;
            pixels[index] = r;
            pixels[index + 1] = g;
            pixels[index + 2] = b;
        }
    }

    CCTexture2D *texture = new CCTexture2D();
    texture->initWithData(pixels, kCCTexture2DPixelFormat_RGB888, 1, 1, CCSizeMake(w_, h_));

    CCSprite* spr = CCSprite::createWithTexture(texture);
    spr->setOpacity(alpha);
    texture->release();
    delete [totalBytes] pixels;

    return spr;
}

cocos2d::CCRect MakeRect(const int& x1, const int& y1, const int& x2, const int& y2)
{
    int rx = MIN(x1, x2);
    int ry = MAX(y1, y2);
    int w = abs(x1 - x2);
    int h = abs(y1 - y2);
    return CCRect(rx, ry, w, h);
}

void ScaleAndMoveCCSprite2Rect(cocos2d::CCSprite* spr_, const cocos2d::CCRect& rect_)
{
    float scale = 1.0f;
    float hSub = spr_->getTextureRect().size.height / rect_.size.height;
    float wSub = spr_->getTextureRect().size.width / rect_.size.width;
    if(hSub >= 1.0 && hSub >= wSub)
        scale = rect_.size.height / spr_->getTextureRect().size.height;
    else if(wSub >= 1.00 && wSub >= hSub)
        scale = rect_.size.width / spr_->getTextureRect().size.width;
    spr_->setScale(scale);
    int posX = rect_.origin.x + rect_.size.width/2;
    int posY = rect_.origin.y - rect_.size.height/2;
    spr_->setPosition(ccp(posX, posY));
}

void GetEdge1DPoint(const float& p1, const float& p2, const float& pi, float& outp)
{
    if(pi < p1)
        outp = p1;
    else if(pi > p2)
        outp = p2;
    else
        outp = pi;
}

void GetEdge2DPoint(const CCPoint& p1, const CCPoint& p2, const CCPoint& pi, CCPoint& outp)
{
    GetEdge1DPoint(p1.x, p2.x, pi.x, outp.x);
    GetEdge1DPoint(p1.y, p2.y, pi.y, outp.y);
}

void SetBoxTestByVar(const int& var_, cocos2d::extension::CCEditBox* box_)
{
    const int boxChars = 15;
    char toBox[boxChars] = {0};
    sprintf(toBox, "%d",  var_);
    box_->setText(toBox);
}

CCMenuItemFont* AddFontBtn(const char *value_, CCObject* target_, SEL_MenuHandler selector_, const CCPoint& pos_,  CCMenu* menu_, const int& fontSize_ = 22)
{
    CCMenuItemFont* addBtn = CCMenuItemFont::create(value_, target_, selector_);
    addBtn->setPosition(pos_);
    addBtn->setFontSize(fontSize_);
    menu_->addChild(addBtn);
    return addBtn;
}

//==============================================
GLDrawLayer::GLDrawLayer()
{
    m_drawRectStart = ccp(0, 0);
    m_drawRectEnd = ccp(0, 0);
}

GLDrawLayer::~GLDrawLayer()
{}

void GLDrawLayer::setDrawRectStart(const cocos2d::CCPoint &start_)
{
    m_drawRectStart = start_;
}

void GLDrawLayer::setDrawRectEnd(const cocos2d::CCPoint &end_)
{
    m_drawRectEnd = end_;
}

void GLDrawLayer::setDrawPoint(const cocos2d::CCPoint &point_)
{
    float cx = (m_drawRectEnd.x + m_drawRectStart.x)/2;
    float cy = (m_drawRectEnd.y + m_drawRectStart.y)/2;
    CCPoint offset(point_.x - cx, point_.y - cy);
    m_drawRectStart.x += offset.x;
    m_drawRectStart.y += offset.y;
    m_drawRectEnd.x += offset.x;
    m_drawRectEnd.y += offset.y;
}

cocos2d::CCRect GLDrawLayer::getNowRect()
{
    int rx = MIN(m_drawRectStart.x, m_drawRectEnd.x);
    int ry = MAX(m_drawRectStart.y, m_drawRectEnd.y);
    int w = abs(m_drawRectStart.x - m_drawRectEnd.x);
    int h = abs(m_drawRectStart.y - m_drawRectEnd.y);
    return CCRect(rx, ry, w, h);
}

void GLDrawLayer::draw()
{
    ccDrawColor4B(155, 200, 200, 200);
    glLineWidth(1);
    ccDrawRect(m_drawRectStart, m_drawRectEnd);
}
//==============================================

IconListComponent::IconListComponent(const cocos2d::CCPoint& firstIconPos_, 
                                     const cocos2d::CCSize& iconSize_, 
                                     const int& maxShow_ /* = 8 */)
:m_iconSize(iconSize_), m_firstIconPos(firstIconPos_), m_maxShow(maxShow_),
m_maxPage(0),m_nowPage(0)
{
    m_layer = CreatePureColorSpr(0, 0, 0xffffffff);
    m_layer->retain();

    m_showList = CCArray::create();
    m_showList->retain();

    m_trueList = CCArray::create();
    m_trueList->retain();
}

IconListComponent::~IconListComponent()
{
    m_layer->release();
    m_showList->release();
    m_trueList->release();
}

void IconListComponent::addList(cocos2d::CCSprite* spr_)
{
    CCSize size = CCDirector::sharedDirector()->getWinSize();
    int showIndex = m_trueList->count() - m_maxPage * m_maxShow;
    bool bIsAddPage = false;
    if(showIndex >= m_maxShow)
    {
        showIndex -= m_maxShow;
        bIsAddPage = true;
        ++m_maxPage;
    }
    
    cocos2d::CCRect rect;
    rect.size = m_iconSize;
    rect.origin.x = m_firstIconPos.x;
    rect.origin.y = m_firstIconPos.y - showIndex * m_iconSize.height;

    int color = 0xE0F0F0F0;
    if (showIndex % 2)
    {
        color = 0xE0C0C0C0;
    }

    CCSprite* addIcon = CreatePureColorSpr(rect.size.width, rect.size.height, color);
    addIcon->setPosition(ccp(rect.origin.x + rect.size.width/2, rect.origin.y - rect.size.height/2));
    addIcon->addChild(spr_);
    ScaleAndMoveCCSprite2Rect(spr_, rect);
    spr_->setPosition(ccp(addIcon->getTextureRect().size.width/2, addIcon->getTextureRect().size.height/2));

    m_trueList->addObject(addIcon);

    if (m_nowPage == m_maxPage)
    {
        m_showList->addObject(addIcon);
        m_layer->addChild(addIcon);
    }
    else
    {
       m_nowPage = m_maxPage;
       _goPage(m_maxPage);
    }
}

void IconListComponent::swapIndex(const int& trueIndex1_, const int& trueIndex2_)
{
    m_trueList->exchangeObjectAtIndex(trueIndex1_, trueIndex2_);
    CCNode* node1 = (CCNode*)m_trueList->objectAtIndex(trueIndex1_);
    CCNode* node2 = (CCNode*)m_trueList->objectAtIndex(trueIndex2_);
    
    CCPoint temp = node1->getPosition();
    node1->setPosition(node2->getPosition());
    node2->setPosition(temp);

    _goPage(m_nowPage);
}

void IconListComponent::goLeftPage()
{
    if (m_nowPage == 0)
        return;    
    --m_nowPage;
    _goPage(m_nowPage);
}

void IconListComponent::goRightPage()
{
    if (m_nowPage == m_maxPage)
        return;    
    ++m_nowPage;
    _goPage(m_nowPage);
}

int IconListComponent::getTrueIndexByShowIndex(const int& showIndex_)
{
    return (showIndex_ + m_nowPage * m_maxShow);
}

const cocos2d::CCPoint& IconListComponent::getIconPositionByShowIndex(const int& showIndex_)
{
    return ((CCNode*)m_showList->objectAtIndex(showIndex_))->getPosition();
}

bool IconListComponent::deletIconByShowIndex(const int& showIndex_)
{
    if(m_showList->count() > 0 && showIndex_ < m_showList->count())
    {
        CCNode* delNode = (CCNode*) m_showList->objectAtIndex(showIndex_);
        CCPoint lastPoint = delNode->getPosition();
        int trueIndex = getTrueIndexByShowIndex(showIndex_);

        for(int i = trueIndex + 1; i < m_trueList->count(); ++i)
        {
            CCPoint setPoint = lastPoint; 
            CCNode* checkNode = (CCNode*) m_trueList->objectAtIndex(i);
            lastPoint = checkNode->getPosition();
            checkNode->setPosition(setPoint);
        }

        m_layer->removeChild(delNode, false);
        m_showList->removeObjectAtIndex(showIndex_);
        m_trueList->removeObjectAtIndex(trueIndex);

        if (m_maxPage == m_nowPage && m_showList->count() == 0 && m_nowPage != 0)
        {
            --m_maxPage;
            --m_nowPage;
            _goPage(m_nowPage);
        }
        else if (m_maxPage > m_nowPage)
        {
            if (m_trueList->count() == m_maxPage * m_maxShow)
                --m_maxPage;            
            _goPage(m_nowPage);
        }
        

        return true;
    }
    return false;
}

void IconListComponent::_goPage(const int& page_)
{
    m_showList->removeAllObjects();
    m_layer->removeAllChildren();

    int start = page_ * m_maxShow;
    int end = start + m_maxShow;
    if(end > m_trueList->count())
        end = m_trueList->count();
    for (int i = start; i != end; ++i)
    {
        CCSprite* spr = (CCSprite*)m_trueList->objectAtIndex(i);
        m_showList->addObject(spr);
        m_layer->addChild(spr);
    }    
}

//==============================================

#define cccontrol_selector(_SELECTOR) (SEL_CCControlHandler)(&_SELECTOR)

const int UI_BACK_PIC_Z_ORDER = 99999;
const int UI_BACK_ELEMENTS_Z_ORDER = 100000;
const int UI_BACK_WIDTH = 200;
const int UI_BACK_HEIGHT = 768;



MainEditLayer::MainEditLayer()
{}

MainEditLayer::~MainEditLayer()
{
    if(m_nowTouchMask)
        m_nowTouchMask->release();
    m_oneFrameSpritesArr->removeAllObjects();
    delete  m_showAnimate;
    delete m_elementIconList;
}

CCScene* MainEditLayer::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    MainEditLayer *layer = new MainEditLayer;
    layer->init();
    layer->autorelease();
    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool MainEditLayer::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !CCLayer::init() )
    {
        return false;
    }

    /////////////////////////////
    // 3. add your codes below...
    // ask director the window size


    
    CCSize size = CCDirector::sharedDirector()->getWinSize();
    
    CCSprite* pSprite = CCSprite::create("Resources/monster/mon_head/head_0.png");
    pSprite->setPosition( ccp(size.width/2, size.height/2) );
   // this->addChild(pSprite, 0);
    
    CCSprite* pSprite2 = CCSprite::create("Resources/monster/mon_torso/torso_0.png");
    pSprite2->setPosition( ccp(size.width/2 + 100, size.height/2) );
  //  this->addChild(pSprite2, 0);
    
    m_bIsOneFrameFromAnimate = false;
    m_oneFrameSpritesArr = CCArray::createWithObject(pSprite);
    m_oneFrameSpritesArr->addObject(pSprite2);
    m_oneFrameSpritesArr->retain();
    
    m_showAnimate = new CC2_EGG::Animate();
    
    m_nowUseBigSprite = CCSprite::create("Resources/test.png");
    m_nowUseBigSprite->retain();
    m_nowUseBigSprite->setPosition(ccp(size.width/2, size.height/2));

    m_elementsArr = cocos2d::CCArray::create();
    m_elementsArr->retain();
    m_elementIconList = new IconListComponent(ccp(size.width - UI_BACK_WIDTH, UI_BACK_HEIGHT * 0.85f),
                                              CCSize(UI_BACK_WIDTH, UI_BACK_HEIGHT * 0.1f));

    m_animateFrameList = new IconListComponent(ccp(size.width - UI_BACK_WIDTH, UI_BACK_HEIGHT * 0.85f),
        CCSize(UI_BACK_WIDTH, UI_BACK_HEIGHT * 0.1f));
    
    m_frameRenderTarget = CCRenderTexture::create(size.width - UI_BACK_WIDTH, size.height);
    m_frameRenderTarget->retain();
    
    m_glDrawLayer = new GLDrawLayer();
    m_glDrawLayer->retain();

    m_mainUiMenu = CCMenu::create();
    m_mainUiMenu->setPosition( CCPointZero );
    m_mainUiMenu->retain();
    
    _addEditCutRectUi();  

    //this->addChild(m_nowUseBigSprite);    
    
    m_nowTouchMask = NULL;
    m_nowTouchSprite = NULL;
    
    this->schedule(schedule_selector(MainEditLayer::updateScene));
    
    m_nowClickEditMode = SET_RECT_MODE;//MOVE_SPRITE_MODE;
    m_nowSceneEditMode = EDIT_CUT_RECT_MODE;//EDIT_ONE_FRAME_MODE;
    //m_glDrawLayer = NULL;   

    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this,0,true);
    
    return true;
}

bool MainEditLayer::ccTouchBegan(cocos2d::CCTouch *touch_, cocos2d::CCEvent *event_)
{
    CCPoint touchPoint  =  touch_->getLocation();

    if(SET_RECT_MODE == m_nowClickEditMode)
    {
        _selectUseSprRectInGLDrawLayerBegin(touchPoint);
        _checkSpriteTouchAndMask(m_elementIconList->getShowList(), touchPoint, true);

        return true;
    }
    else if(SET_BOUND_BOX_MODE == m_nowClickEditMode)
    {
        _selectGLDrawLayerBegin(touchPoint);
        return true;
    }
    else if(MOVE_ELEMENT_MODE == m_nowClickEditMode)
    {
        if(_checkSpriteTouchAndMask(m_elementIconList->getShowList(), touchPoint, true))
            return true;
        if(m_bIsOneFrameFromAnimate)
        {
            if(_checkSpriteTouchAndMask(m_showAnimate->GetNowFrameSprites(), _changePosWorld2Anm(touchPoint)))
                return true;
        }
        else if(_checkSpriteTouchAndMask(m_oneFrameSpritesArr, touchPoint))
            return true;
    }
    else if(SELECT_ELEMENT_MODE == m_nowClickEditMode)
    {
        _checkSpriteTouchAndMask(m_animateFrameList->getShowList(), touchPoint);
        return false;
    }
    else if(MOVE_SPRITE_MODE == m_nowClickEditMode || CHANGE_R0TATION_MODE == m_nowClickEditMode)
    {
        if(m_bIsOneFrameFromAnimate)
        {
            if(_checkSpriteTouchAndMask(m_showAnimate->GetNowFrameSprites(), _changePosWorld2Anm(touchPoint)))
            {
                _updateSprAttributeEditBox();
                return true;
            }
        }
        else if(_checkSpriteTouchAndMask(m_oneFrameSpritesArr, touchPoint))
        {
            _updateSprAttributeEditBox();
            return true;
        }
    }
    return false;
}

void MainEditLayer::ccTouchMoved(cocos2d::CCTouch *touch_, cocos2d::CCEvent *event_)
{
    if(SET_RECT_MODE == m_nowClickEditMode || SET_BOUND_BOX_MODE == m_nowClickEditMode)
    {
        CCPoint touchPoint  =  touch_->getLocation();
        m_glDrawLayer->setDrawRectEnd(touchPoint);
        return;
    }
    
    if (m_nowTouchSprite)
    {
        CCPoint touchPoint  =  touch_->getLocation();
        CCSize size = CCDirector::sharedDirector()->getWinSize();
        switch (m_nowClickEditMode)
        {
            case MOVE_ELEMENT_MODE:
            {               
                m_nowTouchMask->setPosition(touchPoint);
                if(touchPoint.x > (size.width - UI_BACK_WIDTH))
                    m_backUiPic->setOpacity(100);
                else
                    m_backUiPic->setOpacity(255);
                if(m_bIsOneFrameFromAnimate)
                {
                    if(UINT_MAX != m_showAnimate->GetNowFrameSprites()->indexOfObject(m_nowTouchSprite))
                        touchPoint = _changePosWorld2Anm(touchPoint);
                }
                m_nowTouchSprite->setPosition(touchPoint);
            }
                break;
            case MOVE_SPRITE_MODE:
            {
                if(touchPoint.x > (size.width - UI_BACK_WIDTH))
                    m_backUiPic->setOpacity(100);
                else
                    m_backUiPic->setOpacity(255);

                char toBox[10] = {0};
                sprintf(toBox, "%d",  (int)m_nowTouchSprite->getPositionX());
                m_editNowSprX->setText(toBox);
                memset(toBox, sizeof(char) * 10, 0);
                sprintf(toBox, "%d",  (int)m_nowTouchSprite->getPositionY());
                m_editNowSprY->setText(toBox);

                m_nowTouchMask->setPosition(touchPoint);
                if(m_bIsOneFrameFromAnimate)
                    touchPoint = _changePosWorld2Anm(touchPoint);
                m_nowTouchSprite->setPosition(touchPoint);
            }
                break;
            case CHANGE_R0TATION_MODE:
            {
                float o = touchPoint.x - m_nowTouchSprite->getPosition().x;
                float a = touchPoint.y - m_nowTouchSprite->getPosition().y;
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
                m_nowTouchSprite->setRotation(at);
               // m_nowTouchMask->setRotation(at);
                
                sprintf(toBox, "%d",  (int)m_nowTouchSprite->getRotation());
                m_editNowSprRot->setText(toBox);
            }
                break;
            default:
                break;
        }
        //
        

    }
}

void MainEditLayer::ccTouchEnded(cocos2d::CCTouch *touch_, cocos2d::CCEvent *event_)
{
    CCPoint touchPoint  =  touch_->getLocation();
    CCSize size = CCDirector::sharedDirector()->getWinSize();
    //calc rect origin
    if(SET_BOUND_BOX_MODE == m_nowClickEditMode)
    {
        m_glDrawLayer->setDrawPoint(ccp((size.width - UI_BACK_WIDTH)/2, size.height/2));
        float w = m_glDrawLayer->getNowRect().size.width;
        float h = m_glDrawLayer->getNowRect().size.height;
        
        //SetBoxTestByVar((int)x, m_editFrameRX);
        //SetBoxTestByVar((int)y, m_editFrameRY);
        SetBoxTestByVar((int)h, m_editFrameRH);
        SetBoxTestByVar((int)w, m_editFrameRW);
        
        //m_showAnimate->SetBoundBox(m_glDrawLayer->getNowRect());
    }
    else if(SET_RECT_MODE == m_nowClickEditMode)
    {
        if(m_nowUseBigSprite)
        {
            CCPoint bigMin = ccp(m_nowUseBigSprite->getPosition().x - m_nowUseBigSprite->getTextureRect().size.width/2,
                                 m_nowUseBigSprite->getPosition().y - m_nowUseBigSprite->getTextureRect().size.height/2);
            CCPoint bigMax = ccp(m_nowUseBigSprite->getPosition().x + m_nowUseBigSprite->getTextureRect().size.width/2,
                                 m_nowUseBigSprite->getPosition().y + m_nowUseBigSprite->getTextureRect().size.height/2);
            CCPoint selectEnd;
            GetEdge2DPoint(bigMin, bigMax,  touchPoint,  selectEnd);
            MakeRect(m_nowSelectBegin.x, m_nowSelectBegin.y, selectEnd.x, selectEnd.y);
            m_glDrawLayer->setDrawRectStart(m_nowSelectBegin);
            m_glDrawLayer->setDrawRectEnd(selectEnd);
            m_nowSelectRect = m_glDrawLayer->getNowRect();
            
            m_nowSelectRect.origin.y = bigMax.y - m_nowSelectRect.origin.y;
            m_nowSelectRect.origin.x -= bigMin.x;
        }
        
        if(m_nowTouchSprite)
        {
            m_oneFrameSpritesArr->removeObject(m_nowTouchSprite);
            removeChild(m_nowTouchSprite);
            m_nowTouchSprite = NULL;
        }
    }
    else if(MOVE_ELEMENT_MODE == m_nowClickEditMode || SET_RECT_MODE == m_nowClickEditMode)
    {
       m_backUiPic->setOpacity(255);
       unsigned int removeIndex = 0;

       if(m_bIsOneFrameFromAnimate)
           removeIndex = m_showAnimate->GetNowFrameSprites()->indexOfObject(m_nowTouchSprite);
       else
           removeIndex = m_oneFrameSpritesArr->indexOfObject(m_nowTouchSprite);
       if(UINT_MAX == removeIndex)//doesn't contain the object
       {
           if(touchPoint.x < (size.width - UI_BACK_WIDTH))
           {
               if (m_bIsOneFrameFromAnimate)
               {
                   m_nowTouchSprite->setPosition(_changePosWorld2Anm(touchPoint));
                   m_showAnimate->AddSprOnNowFrame(m_nowTouchSprite);
                   m_nowTouchSprite->setZOrder(0);
                   removeChild(m_nowTouchSprite);
                   m_nowTouchSprite = NULL;
               }
               else
               {
                   m_oneFrameSpritesArr->addObject(m_nowTouchSprite);
                   m_nowTouchSprite->setZOrder(0);
                   m_nowTouchSprite = NULL;
               }
           }
           else
           {
               removeChild(m_nowTouchSprite);
               m_nowTouchSprite = NULL;
           }
       }
       else
       {
           if(touchPoint.x > (size.width - UI_BACK_WIDTH))
           {
               //for(int i = removeIndex; i != m_oneFrameSpritesArr->count(); ++i)
               //{
               //   m_oneFrameSpritesArr->objectAtIndex(i);
               //}
               if (m_bIsOneFrameFromAnimate)
                   m_showAnimate->GetNowFrameSprites()->removeObjectAtIndex(removeIndex);
               else
                   m_oneFrameSpritesArr->removeObjectAtIndex(removeIndex);
               removeChild(m_nowTouchSprite);
               m_nowTouchSprite = NULL;
           }

       }
    }
    else if(MOVE_SPRITE_MODE == m_nowClickEditMode)
    {
        m_backUiPic->setOpacity(255);
        if(touchPoint.x > (size.width - UI_BACK_WIDTH))
        {
            //for(int i = removeIndex; i != m_oneFrameSpritesArr->count(); ++i)
            //{
            //   m_oneFrameSpritesArr->objectAtIndex(i);
            //}
            removeChild(m_nowTouchSprite);
            m_nowTouchSprite = NULL;
            removeChild(m_nowTouchMask);
            m_nowTouchMask = NULL;
        }
    }

}

void MainEditLayer::editBoxEditingDidBegin(cocos2d::extension::CCEditBox* editBox)
{}

void MainEditLayer::editBoxEditingDidEnd(cocos2d::extension::CCEditBox* editBox_)
{}

void MainEditLayer::editBoxTextChanged(cocos2d::extension::CCEditBox* editBox_, const std::string& text_)
{
    if (editBox_ == m_editFrameRW || editBox_ == m_editFrameRH)
        _updateRectAttributeEditBox();

    if(m_nowTouchSprite)
    {
        if (editBox_ == m_editNowSprX)
        {
            int posX = 0;
            sscanf(text_.c_str(), "%d",  &posX);

            m_nowTouchMask->setPositionX(float(posX));
            if (m_bIsOneFrameFromAnimate)
                posX += m_showAnimate->GetFatherSpr()->getPositionX();            
            m_nowTouchSprite->setPositionX(float(posX));

        }
        else if (editBox_ == m_editNowSprY)
        {
            int posY = 0;
            sscanf(text_.c_str(), "%d",  &posY);
            
            m_nowTouchSprite->setPositionY(float(posY));
            if (m_bIsOneFrameFromAnimate)
                posY += m_showAnimate->GetFatherSpr()->getPositionY();
            m_nowTouchMask->setPositionY(float(posY));     
        }
        else if (editBox_ == m_editNowSprRot)
        {
            int rot = 0;
            sscanf(text_.c_str(), "%d",  &rot);
            m_nowTouchSprite->setRotation(float(rot));
            // m_nowTouchMask->setRotation(float(rot));
        }
        else if(editBox_ == m_editNowSprLayer)
        {
            int layer = 0;
            sscanf(text_.c_str(), "%d",  &layer);
            m_nowTouchSprite->setZOrder(layer);
        }
        else if (editBox_ == m_editNowSprAlpha)
        {
            int alpha = 255;
            sscanf(text_.c_str(), "%d",  &alpha);
            m_alphaSlider->setValue(alpha);
        }
        else if (editBox_ == m_editNowSprScale)
        {
            int scale = 255;
            sscanf(text_.c_str(), "%d",  &scale);
            m_scaleSlider->setValue(scale);
        }
    }
}

void MainEditLayer::editBoxReturn(cocos2d::extension::CCEditBox* editBox_)
{

}


void MainEditLayer::updateScene(float dt_)
{
    if (wcscmp(m_nowDragFile.c_str(), CCEGLView::sharedOpenGLView()->getDragOnFile()))
    {
        int len = WideCharToMultiByte(CP_UTF8, 0, CCEGLView::sharedOpenGLView()->getDragOnFile(), -1, NULL, 0, NULL, NULL);    
        char* str = new char[len+1];    
        memset(str, 0, len+1);    
        WideCharToMultiByte(CP_ACP, 0, CCEGLView::sharedOpenGLView()->getDragOnFile(), -1, str, len, NULL, NULL); 
        
        CCSprite* dummySpr = CCSprite::create(str);
        delete[] str;        
        m_nowUseBigSprite->setTexture(dummySpr->getTexture());
        m_nowUseBigSprite->setTextureRect(dummySpr->getTextureRect());
        dummySpr->release();

        m_nowDragFile = CCEGLView::sharedOpenGLView()->getDragOnFile();
    }
    

    if( (EDIT_ANIMATE_MODE == m_nowSceneEditMode) && m_showAnimate)
        m_showAnimate->Update(dt_);
}

//---------------------call back------------------------------

void MainEditLayer::changeSceneEditMode(CCObject* pSender_)
{
    m_nowSceneEditMode = (NowSceneEditMode) ((int)m_nowSceneEditMode + 1);
    if (SCENE_MODE_COUNT == m_nowSceneEditMode) {
        m_nowSceneEditMode = EDIT_CUT_RECT_MODE;
    }
    switch(m_nowSceneEditMode)
    {
    case EDIT_CUT_RECT_MODE:
            _addEditCutRectUi();
            break;
    case EDIT_ONE_FRAME_MODE:
            _addEditFrameUi();
            break;
    case EDIT_ANIMATE_MODE:
            _addEditAnimateUi();
            break;
    default:
            break;
    }
}

void MainEditLayer::changeClickEditMode(CCObject* pSender_)
{
    m_nowClickEditMode = (NowClickEditMode) ((int)m_nowClickEditMode + 1);

    if (m_nowClickEditMode == BTN_CHANGE_COUNT_LIMIT)
    {
        m_nowClickEditMode = MOVE_SPRITE_MODE;
    }
    switch(m_nowClickEditMode)
    {
        case MOVE_SPRITE_MODE:
            m_changeCEditBtn->setString(G2U("移动模式").c_str());
            break;
        case CHANGE_R0TATION_MODE:
            m_changeCEditBtn->setString(G2U("旋转模式").c_str());
            break;
        case SET_BOUND_BOX_MODE:
            m_changeCEditBtn->setString(G2U("桢包围盒设置").c_str());
            break;
        default:
            break;
    }
}

void MainEditLayer::flipNowXCallBack(cocos2d::CCObject *pSender_)
{
    if(m_nowTouchSprite)
        m_nowTouchSprite->setFlipX(!m_nowTouchSprite->isFlipX());
}

void MainEditLayer::flipNowYCallBack(cocos2d::CCObject *pSender_)
{
    if(m_nowTouchSprite)
        m_nowTouchSprite->setFlipY(!m_nowTouchSprite->isFlipY());
}

void MainEditLayer::changeAlphaCallBack(cocos2d::CCObject *pSender_, extension::CCControlEvent controlEvent_)
{
    CCControlSlider* pSlider = (CCControlSlider*)pSender_;
    if(m_nowTouchSprite)
    {
        const int boxChars = 15;
        char toBox[boxChars] = {0};
        sprintf(toBox, "%d",  (int)pSlider->getValue());
        m_editNowSprAlpha->setText(toBox);
        m_nowTouchSprite->setOpacity(pSlider->getValue());
        //_updateSprAttributeEditBox();
    }
}

void MainEditLayer::changeScaleCallBack(cocos2d::CCObject *pSender_, extension::CCControlEvent controlEvent_)
{
    CCControlSlider* pSlider = (CCControlSlider*)pSender_;
    if(m_nowTouchSprite)
    {
        const int boxChars = 15;
        char toBox[boxChars] = {0};
        sprintf(toBox, "%d",  (int)pSlider->getValue());
        m_editNowSprScale->setText(toBox);
        m_nowTouchSprite->setScale(pSlider->getValue() / 100.0f);
        //m_nowTouchMask->setScale(m_nowTouchMask->getScale() * pSlider->getValue() / 100.0f);
        //_updateSprAttributeEditBox();
    }
}

void MainEditLayer::addCutRectCallBack(cocos2d::CCObject *pSender_)
{
    if(m_nowClickEditMode != SET_RECT_MODE)
        return; 
    CCSprite* add = CCSprite::createWithTexture(m_nowUseBigSprite->getTexture(), m_nowSelectRect);
    m_elementsArr->addObject(add);
    
    CCSprite* detail = CCSprite::createWithTexture(m_nowUseBigSprite->getTexture(), m_nowSelectRect);
    m_elementIconList->addList(detail);
}

void MainEditLayer::addFrameCallBack(cocos2d::CCObject *pSender_)
{
    m_showAnimate->AddFrame(m_oneFrameSpritesArr, m_glDrawLayer->getNowRect().size);
    m_nowSceneEditMode = EDIT_ANIMATE_MODE;
    
    _addEditAnimateUi();
    _updateRenderTarget();
    CCImage* pImage = m_frameRenderTarget->newCCImage();
    CCTexture2D* tex = new CCTexture2D();
    tex->initWithImage(pImage);
    CC_SAFE_DELETE(pImage);
    CCSprite* detail = CCSprite::createWithTexture(tex, m_frameRenderTarget->getSprite()->getTextureRect());
    m_animateFrameList->addList(detail);
}

void MainEditLayer::doMoveElementCallBack(cocos2d::CCObject *pSender_)
{
    if(m_nowClickEditMode == MOVE_ELEMENT_MODE)
        return;
    if(m_glDrawLayer)
        this->removeChild(m_glDrawLayer);
    removeChild(m_nowUseBigSprite);
    if (m_bIsOneFrameFromAnimate)
    {
        m_showAnimate->RemoveFormTargetLayer();
        m_showAnimate->SetTargetLayer(this);
    } 
    else
    {
        for (int i = 0; i < m_oneFrameSpritesArr->count(); ++i)
        {
            CCSprite* pSprite = dynamic_cast<CCSprite*>(m_oneFrameSpritesArr->objectAtIndex(i));
            addChild(pSprite);
        }
    }

    m_nowClickEditMode = MOVE_ELEMENT_MODE;
}

void MainEditLayer::doCutRectCallBack(cocos2d::CCObject *pSender_)
{
    if(m_nowClickEditMode == SET_RECT_MODE)
        return;

    if (m_nowTouchMask)
    {
        this->removeChild(m_nowTouchMask);
        m_nowTouchMask->release();
        m_nowTouchMask = NULL;
        m_nowTouchSprite = NULL;
    }
    
    addChild(m_nowUseBigSprite);

    if (m_bIsOneFrameFromAnimate)
        removeChild(m_showAnimate->GetFatherSpr());
    else
    {
        for (int i = 0; i < m_oneFrameSpritesArr->count(); ++i)
        {
            CCSprite* pSprite = dynamic_cast<CCSprite*>(m_oneFrameSpritesArr->objectAtIndex(i));
            removeChild(pSprite);
        }
    }

    m_nowClickEditMode = SET_RECT_MODE;
}

void MainEditLayer::doDelRectCallBack(cocos2d::CCObject *pSender_)
{
    if(m_elementIconList->deletIconByShowIndex(m_lastSelectindex))
        m_elementsArr->removeObjectAtIndex(m_elementIconList->getTrueIndexByShowIndex(m_lastSelectindex));
}

void MainEditLayer::changeStateCutCallBack(CCObject* pSender_)
{
    m_nowSceneEditMode = EDIT_CUT_RECT_MODE;
    _addEditCutRectUi();
}

void MainEditLayer::changeStateFrameCallBack(CCObject* pSender_)
{
    m_nowSceneEditMode = EDIT_ONE_FRAME_MODE;
    _addEditFrameUi();
}

void MainEditLayer::changeStateAnimateCallBack(CCObject* pSender_)
{
     m_nowSceneEditMode = EDIT_ANIMATE_MODE;
    _addEditAnimateUi();
}

void MainEditLayer::loadCallBack(CCObject* pSender_)
{

}

void MainEditLayer::saveCallBack(CCObject* pSender_)
{
}

void MainEditLayer::doDownFrameCallBack(CCObject* pSender_)
{
    int trueIndex = m_animateFrameList->getTrueIndexByShowIndex(m_lastSelectindex);
    if( trueIndex >= 0 && trueIndex < (m_animateFrameList->getSize() - 1) )
    {
        m_showAnimate->SwapFrame(trueIndex, trueIndex + 1);
        m_animateFrameList->swapIndex(trueIndex, trueIndex + 1);
        ++m_lastSelectindex;
        m_nowTouchMask->setPosition(m_animateFrameList->getIconPositionByShowIndex(m_lastSelectindex));
    }
}

void MainEditLayer::doUpFrameCallBack(CCObject* pSender_)
{
    int trueIndex = m_animateFrameList->getTrueIndexByShowIndex(m_lastSelectindex);
    if( trueIndex > 0 && trueIndex < m_animateFrameList->getSize() )
    {
        m_showAnimate->SwapFrame(trueIndex, trueIndex - 1);
        m_animateFrameList->swapIndex(trueIndex, trueIndex - 1);
        --m_lastSelectindex;
        m_nowTouchMask->setPosition(m_animateFrameList->getIconPositionByShowIndex(m_lastSelectindex));
    }
}

void MainEditLayer::doDelFrameCallBack(CCObject* pSender_)
{
    if(m_animateFrameList->deletIconByShowIndex(m_lastSelectindex))
    {
        m_showAnimate->RemoveFormTargetLayer();
        m_showAnimate->DeleteFrame(m_animateFrameList->getTrueIndexByShowIndex(m_lastSelectindex));
        m_showAnimate->RunWithTargetLayer(this);
    }
}

void MainEditLayer::editNowFrameCallBack(CCObject* pSender_)
{
    CCArray* editFrame = m_showAnimate->GetNowFrameSprites();
    
    if(editFrame && editFrame->count() > 0)
    {        
        //m_oneFrameSpritesArr = editFrame;
        m_nowSceneEditMode = EDIT_ONE_FRAME_MODE;
        m_bIsOneFrameFromAnimate = true;
        _addEditFrameUi();
    }
}

void MainEditLayer::rectlistLeftCallBack(CCObject* pSender_)
{
    m_elementIconList->goLeftPage();
}

void MainEditLayer::rectlistRightCallBack(CCObject* pSender_)
{
    m_elementIconList->goRightPage();
}

void MainEditLayer::framelistLeftCallBack(CCObject* pSender_)
{
    m_animateFrameList->goLeftPage();
}

void MainEditLayer::framelistRightCallBack(CCObject* pSender_)
{
    m_animateFrameList->goRightPage();
}

void MainEditLayer::menuCloseCallback(CCObject* pSender_)
{
    CCDirector::sharedDirector()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

//------------------private------------------------
void MainEditLayer::_addEditCutRectUi()
{
    m_nowClickEditMode = SET_RECT_MODE;
    _addEditUiBase();
    addChild(m_nowUseBigSprite);
    CCSize size = CCDirector::sharedDirector()->getWinSize();
    
    addChild(m_elementIconList->getMainLayer(), UI_BACK_ELEMENTS_Z_ORDER);

    AddFontBtn(G2U("添加所选区").c_str(), this, menu_selector(MainEditLayer::addCutRectCallBack), ccp(size.width - UI_BACK_WIDTH + 140.0f,  size.height * 0.95f), m_mainUiMenu);
    AddFontBtn(G2U("切图").c_str(), this, menu_selector(MainEditLayer::doCutRectCallBack), ccp(size.width - UI_BACK_WIDTH + 30.0f,  size.height * 0.9f), m_mainUiMenu);
    AddFontBtn(G2U("删除").c_str(), this, menu_selector(MainEditLayer::doDelRectCallBack), ccp(size.width - UI_BACK_WIDTH + 170.0f,  size.height * 0.9f), m_mainUiMenu);
    AddFontBtn(G2U("拖拽").c_str(), this, menu_selector(MainEditLayer::doMoveElementCallBack), ccp(size.width - UI_BACK_WIDTH + 100.0f,  size.height * 0.9f), m_mainUiMenu);
    AddFontBtn(G2U("->").c_str(), this, menu_selector(MainEditLayer::rectlistRightCallBack), ccp(size.width - UI_BACK_WIDTH + 0.9f * UI_BACK_WIDTH,  size.height * 0.87f), m_mainUiMenu);
    AddFontBtn(G2U("<-").c_str(), this, menu_selector(MainEditLayer::rectlistLeftCallBack), ccp(size.width - UI_BACK_WIDTH + 0.1f * UI_BACK_WIDTH,  size.height * 0.87f), m_mainUiMenu);
}


void MainEditLayer::_addEditFrameUi()
{
    m_nowClickEditMode = MOVE_SPRITE_MODE;
    
    _addEditUiBase();
    m_showAnimate->RemoveFormTargetLayer();
    
    if (m_bIsOneFrameFromAnimate)
    {
        m_showAnimate->SetTargetLayer(this);
    }    
    else
    {
        for (int i = 0; i < m_oneFrameSpritesArr->count(); ++i)
        {
            CCSprite* pSprite = dynamic_cast<CCSprite*>(m_oneFrameSpritesArr->objectAtIndex(i));
            CCPoint look = pSprite->getPosition();
            this->addChild(pSprite);
        }
    }

    
    CCSize size = CCDirector::sharedDirector()->getWinSize();
    int uiBackWidth = UI_BACK_WIDTH;
    int uiBackHeight = UI_BACK_HEIGHT;
    
    float nextGap = 0.08f;
    float nowHight = 0.96f;
    
    m_changeCEditBtn = AddFontBtn(G2U("移动模式").c_str(), this, menu_selector(MainEditLayer::changeClickEditMode), ccp(size.width - uiBackWidth + 130.0f,  size.height * nowHight), m_mainUiMenu);
    
    nowHight -= nextGap;
    
    CCLabelTTF* xlable = CCLabelTTF::create("X:", "微软雅黑", 22);
    xlable->setPosition(ccp(size.width - uiBackWidth + 10.0f,  size.height * nowHight));
    this->addChild(xlable, UI_BACK_ELEMENTS_Z_ORDER);
    m_editNowSprX = _addNumEditBox(-0.2f, nowHight);
    
    CCLabelTTF* ylable = CCLabelTTF::create("Y:", "微软雅黑", 22);
    ylable->setPosition(ccp(size.width - uiBackWidth + uiBackWidth * 0.55f,  size.height * nowHight));
    this->addChild(ylable, UI_BACK_ELEMENTS_Z_ORDER);
    m_editNowSprY = _addNumEditBox(0.28f, nowHight);
    
    nowHight -= nextGap;
    
    CCLabelTTF* rlable = CCLabelTTF::create(G2U("角:").c_str(), "微软雅黑", 20);
    rlable->setPosition(ccp(size.width - uiBackWidth + 11.0f,  size.height * nowHight));
    this->addChild(rlable, UI_BACK_ELEMENTS_Z_ORDER);    
    m_editNowSprRot = _addNumEditBox(-0.2f, nowHight);
    
    CCLabelTTF* layerlable = CCLabelTTF::create(G2U("层:").c_str(), "微软雅黑", 20);
    layerlable->setPosition(ccp(size.width - uiBackWidth + uiBackWidth * 0.55f,  size.height * nowHight));
    this->addChild(layerlable, UI_BACK_ELEMENTS_Z_ORDER);    
    m_editNowSprLayer = _addNumEditBox(0.28f, nowHight);
    
    nowHight -= nextGap;
    
    CCMenuItemFont* flipXBtn = CCMenuItemFont::create(G2U("X翻转").c_str(), this, menu_selector(MainEditLayer::flipNowXCallBack));
    flipXBtn->setPosition(ccp(size.width - uiBackWidth + 60.0f,  size.height * nowHight));
    flipXBtn->setFontSize(22);
    m_mainUiMenu->addChild(flipXBtn);
    
    CCMenuItemFont* flipYBtn = CCMenuItemFont::create(G2U("Y翻转").c_str(), this, menu_selector(MainEditLayer::flipNowYCallBack));
    flipYBtn->setPosition(ccp(size.width - uiBackWidth + 160.0f,  size.height * nowHight));
    flipYBtn->setFontSize(22);
    m_mainUiMenu->addChild(flipYBtn);
    
    nowHight -= nextGap;
    
    CCLabelTTF* alphaLable = CCLabelTTF::create(G2U("透明度:").c_str(), "微软雅黑", 22);
    alphaLable->setPosition(ccp(size.width - uiBackWidth + 50.0f,  size.height * nowHight));
    m_editNowSprAlpha = _addNumEditBox(0.12f, nowHight);
    this->addChild(alphaLable, UI_BACK_ELEMENTS_Z_ORDER);

    nowHight -= 0.03f;    

    m_alphaSlider = _addSliderGroup(0.5f, nowHight, 0, 255, 255, cccontrol_selector(MainEditLayer::changeAlphaCallBack));
    
    nowHight -= nextGap;
    
    CCLabelTTF* scaleLable = CCLabelTTF::create(G2U("拉伸:").c_str(), "微软雅黑", 22);
    scaleLable->setPosition(ccp(size.width - uiBackWidth + 35.0f,  size.height * nowHight));
    m_editNowSprScale = _addNumEditBox(0.0f, nowHight);
    this->addChild(scaleLable, UI_BACK_ELEMENTS_Z_ORDER);
    
    nowHight -= 0.03f;
    
    m_scaleSlider = _addSliderGroup(0.5f, nowHight, 10.0f, 300.0f, 100.0f, cccontrol_selector(MainEditLayer::changeScaleCallBack));

    nowHight -= nextGap;
    
    //CCLabelTTF* rxlable = CCLabelTTF::create("RX:", "微软雅黑", 22);
   // rxlable->setPosition(ccp(size.width - uiBackWidth + 10.0f,  size.height * nowHight));
    //this->addChild(rxlable, UI_BACK_ELEMENTS_Z_ORDER);
    //m_editFrameRX = _addNumEditBox(-0.2f, nowHight);
    
   // CCLabelTTF* rylable = CCLabelTTF::create("RY:", "微软雅黑", 22);
   // rylable->setPosition(ccp(size.width - uiBackWidth + uiBackWidth * 0.55f,  size.height * nowHight));
   // this->addChild(rylable, UI_BACK_ELEMENTS_Z_ORDER);
    //m_editFrameRY = _addNumEditBox(0.28f, nowHight);
    
   // nowHight -= nextGap;
    
    CCLabelTTF* wlable = CCLabelTTF::create("RW:", "微软雅黑", 22);
    wlable->setPosition(ccp(size.width - uiBackWidth + 10.0f,  size.height * nowHight));
    this->addChild(wlable, UI_BACK_ELEMENTS_Z_ORDER);
    m_editFrameRW = _addNumEditBox(-0.2f, nowHight);
    
    CCLabelTTF* hlable = CCLabelTTF::create("RH:", "微软雅黑", 22);
    hlable->setPosition(ccp(size.width - uiBackWidth + uiBackWidth * 0.55f,  size.height * nowHight));
    this->addChild(hlable, UI_BACK_ELEMENTS_Z_ORDER);
    m_editFrameRH = _addNumEditBox(0.28f, nowHight);
    
    nowHight -= nextGap;

    AddFontBtn(G2U("添加动画桢").c_str(), this, menu_selector(MainEditLayer::addFrameCallBack), ccp(size.width - uiBackWidth + 100.0f, size.height *  nowHight), m_mainUiMenu);

    const int boxChars = 15;
    char toBox[boxChars];
    sprintf(toBox, "%d",  (int)m_showAnimate->GetBoundBox().size.width);
    m_editFrameRW->setText(toBox);
    memset(toBox, sizeof(char) * boxChars, 0);
    sprintf(toBox, "%d",  (int)m_showAnimate->GetBoundBox().size.height);
    m_editFrameRH->setText(toBox);

    _updateRectAttributeEditBox();
    
    addChild(m_glDrawLayer, UI_BACK_PIC_Z_ORDER);
}

void MainEditLayer::_addEditAnimateUi()
{
    if(m_bIsOneFrameFromAnimate)
    {
        m_bIsOneFrameFromAnimate = false;
        for (int i = 0; i < m_oneFrameSpritesArr->count(); ++i)
        {
            CCSprite* pSprite = dynamic_cast<CCSprite*>(m_oneFrameSpritesArr->objectAtIndex(i));            
        }
    }

    
    _addEditUiBase();
    m_showAnimate->RunWithTargetLayer(this);
    addChild(m_animateFrameList->getMainLayer(), UI_BACK_ELEMENTS_Z_ORDER);
    m_nowClickEditMode = SELECT_ELEMENT_MODE;

    CCSize size = CCDirector::sharedDirector()->getWinSize();

    AddFontBtn(G2U("修改所选桢").c_str(), this, menu_selector(MainEditLayer::editNowFrameCallBack), ccp(size.width - UI_BACK_WIDTH + 140.0f, size.height * 0.975f), m_mainUiMenu);
    AddFontBtn(G2U("上移").c_str(), this, menu_selector(MainEditLayer::doUpFrameCallBack), ccp(size.width - UI_BACK_WIDTH + 30.0f, size.height * 0.937f), m_mainUiMenu);
    AddFontBtn(G2U("删除").c_str(), this, menu_selector(MainEditLayer::doDelFrameCallBack), ccp(size.width - UI_BACK_WIDTH + 170.0f, size.height * 0.937f), m_mainUiMenu);
    AddFontBtn(G2U("下移").c_str(), this, menu_selector(MainEditLayer::doDownFrameCallBack), ccp(size.width - UI_BACK_WIDTH + 100.0f, size.height * 0.937f), m_mainUiMenu);

    CCLabelTTF* slable = CCLabelTTF::create(G2U("帧速:").c_str(), "微软雅黑", 22);
    slable->setPosition(ccp(size.width - UI_BACK_WIDTH + 50.0f,  size.height * 0.89f));
    this->addChild(slable, UI_BACK_ELEMENTS_Z_ORDER);
    m_editFrameSpeed = _addNumEditBox(0.12f, 0.89f);

    AddFontBtn(G2U("->").c_str(), this, menu_selector(MainEditLayer::framelistLeftCallBack), ccp(size.width - UI_BACK_WIDTH + 0.9f * UI_BACK_WIDTH,  size.height * 0.87f), m_mainUiMenu);
    AddFontBtn(G2U("<-").c_str(), this, menu_selector(MainEditLayer::framelistRightCallBack), ccp(size.width - UI_BACK_WIDTH + 0.1f * UI_BACK_WIDTH,  size.height * 0.87f), m_mainUiMenu);
}

void MainEditLayer::_addEditUiBase()
{
    this->removeAllChildren();
    
    //m_glDrawLayer = NULL;
    m_nowTouchSprite = NULL;
    m_lastSelectindex = 0;
    
    CCSize size = CCDirector::sharedDirector()->getWinSize();
    int uiBackWidth = UI_BACK_WIDTH;
    int uiBackHeight = UI_BACK_HEIGHT;
    m_backUiPic = CreatePureColorSpr(uiBackWidth, uiBackHeight, 0xff0f0fff);
    m_backUiPic->setPosition(ccp(size.width - uiBackWidth/2, size.height/2));
    this->addChild(m_backUiPic, UI_BACK_PIC_Z_ORDER);
    
    CCMenuItemImage *pCloseItem = CCMenuItemImage::create(
                                                          "Resources/CloseNormal.png",
                                                          "Resources/CloseSelected.png",
                                                          this,
                                                          menu_selector(MainEditLayer::menuCloseCallback) );
    pCloseItem->setPosition( ccp(CCDirector::sharedDirector()->getWinSize().width - 20, 20) );
    if(m_mainUiMenu)
        m_mainUiMenu->release();
    m_mainUiMenu = CCMenu::create();
    m_mainUiMenu->retain();
    
    m_mainUiMenu->setPosition( CCPointZero );
    this->addChild(m_mainUiMenu, UI_BACK_ELEMENTS_Z_ORDER);
    m_mainUiMenu->addChild(pCloseItem);
    
    CCMenuItemFont::setFontSize(22);
    CCMenuItemFont::setFontName("微软雅黑");
    CCMenuItemFont* changeBtn = CCMenuItemFont::create(G2U("切换").c_str(), this, menu_selector(MainEditLayer::changeSceneEditMode));
    changeBtn->setPosition(ccp(size.width - uiBackWidth + 30.0f,  size.height * 0.98f));
    m_mainUiMenu->addChild(changeBtn);

    //----------------------create top ui---------------------------
    CCSprite* topUiPic = CreatePureColorSpr(size.width - uiBackWidth, size.height * 0.05f, 0xffAfAfff);
    topUiPic->setPosition(ccp((size.width - uiBackWidth)/2, size.height - (size.height * 0.05f)/2));
    this->addChild(topUiPic, UI_BACK_PIC_Z_ORDER);

    AddFontBtn(G2U("切图编辑").c_str(), this, menu_selector(MainEditLayer::changeStateCutCallBack), ccp(30.0f + size.width * 0.05f,  size.height * 0.975f), m_mainUiMenu);
    AddFontBtn(G2U("桢编辑").c_str(), this, menu_selector(MainEditLayer::changeStateFrameCallBack), ccp(30.0f + size.width * 0.2f,  size.height * 0.975f), m_mainUiMenu);
    AddFontBtn(G2U("动画编辑").c_str(), this, menu_selector(MainEditLayer::changeStateAnimateCallBack), ccp(30.0f + size.width * 0.35f,  size.height * 0.975f), m_mainUiMenu);
    AddFontBtn(G2U("保存").c_str(), this, menu_selector(MainEditLayer::saveCallBack), ccp(30.0f + size.width *0.55f,  size.height * 0.975f), m_mainUiMenu);
    AddFontBtn(G2U("读取").c_str(), this, menu_selector(MainEditLayer::loadCallBack), ccp(30.0f + size.width *0.7f,  size.height * 0.975f), m_mainUiMenu);
    //--------------------------------------------------------------
}

void MainEditLayer::_addShowList(CCSprite* spr_, CCArray* list_, CCSprite* lay_)
{
    cocos2d::CCRect rect;
    CCSize size = CCDirector::sharedDirector()->getWinSize();
    
    int showIndex = list_->count();
    
    rect.size.width = UI_BACK_WIDTH;
    rect.size.height = UI_BACK_HEIGHT * 0.1f;
    rect.origin.x = size.width - UI_BACK_WIDTH;
    rect.origin.y = UI_BACK_HEIGHT * 0.85f - showIndex * UI_BACK_HEIGHT * 0.1f;
    
    int color = 0xE0F0F0F0;
    if (showIndex % 2)
    {
        color = 0xE0C0C0C0;
    }

    CCSprite* addIcon = CreatePureColorSpr(rect.size.width, rect.size.height, color);
    addIcon->setPosition(ccp(rect.origin.x + rect.size.width/2, rect.origin.y - rect.size.height/2));
    addIcon->addChild(spr_);
    ScaleAndMoveCCSprite2Rect(spr_, rect);
    spr_->setPosition(ccp(addIcon->getTextureRect().size.width/2, addIcon->getTextureRect().size.height/2));
    
    
    
    list_->addObject(addIcon);

    lay_->addChild(addIcon);
    
}

bool MainEditLayer::_checkSpriteTouchAndMask(CCArray* sprArray_, const CCPoint& touch_, bool bIsRecord_)
{
    for (int i = 0; i < sprArray_->count(); ++i)
    {
        CCSprite* pSprite = dynamic_cast<CCSprite*>(sprArray_->objectAtIndex(i));
        if (pSprite)
        {
            CCPoint pPoint =  pSprite->getPosition();
            CCSize  pSize = pSprite->getContentSize();
            CCRect  pRect = CCRectMake(pPoint.x - pSize.width / 2, pPoint.y - pSize.height / 2, pSize.width, pSize.height);
            if(pRect.containsPoint(touch_))
            {
                m_nowTouchSprite = pSprite;

                if (m_nowTouchMask)
                {
                    this->removeChild(m_nowTouchMask);
                    m_nowTouchMask->release();
                }
                
                m_nowTouchMask = CreatePureColorSpr(pSize.width, pSize.height, 0x200000ff);

                m_nowTouchMask->retain();
                m_nowTouchMask->setPosition(pPoint);
                addChild(m_nowTouchMask, UI_BACK_ELEMENTS_Z_ORDER + 1);
                
                m_lastSelectindex = i;
                if(bIsRecord_)
                {
                    CCSprite* useSprite = dynamic_cast<CCSprite*>(m_elementsArr->objectAtIndex(i));
                    m_nowTouchSprite = CCSprite::createWithTexture(useSprite->getTexture(), useSprite->getTextureRect());
                    m_nowTouchSprite->setPosition(touch_);
                    addChild(m_nowTouchSprite, UI_BACK_ELEMENTS_Z_ORDER + 1);
                }
                return true;
            }
        }
    }
    return false;
}

void MainEditLayer::_selectUseSprRectInGLDrawLayerBegin(const cocos2d::CCPoint& touch_)
{

    _selectGLDrawLayerBegin(touch_);
    //calc rect origin
    if(m_nowUseBigSprite)
    {
        CCPoint bigMin = ccp(m_nowUseBigSprite->getPosition().x - m_nowUseBigSprite->getTextureRect().size.width/2,
                             m_nowUseBigSprite->getPosition().y - m_nowUseBigSprite->getTextureRect().size.height/2);
        CCPoint bigMax = ccp(m_nowUseBigSprite->getPosition().x + m_nowUseBigSprite->getTextureRect().size.width/2,
                             m_nowUseBigSprite->getPosition().y + m_nowUseBigSprite->getTextureRect().size.height/2);
        GetEdge2DPoint(bigMin, bigMax,  touch_,  m_nowSelectBegin);
    }
}

void MainEditLayer::_selectGLDrawLayerBegin(const cocos2d::CCPoint &touch_)
{
    if(m_glDrawLayer)
    {
        this->removeChild(m_glDrawLayer);
        m_glDrawLayer->setDrawRectStart(touch_);
        m_glDrawLayer->setDrawRectEnd(touch_);
        addChild(m_glDrawLayer, UI_BACK_PIC_Z_ORDER);
    }
}

cocos2d::CCPoint MainEditLayer::_changePosWorld2Anm(const cocos2d::CCPoint& pos_)
{
   const CCPoint& anmPos = m_showAnimate->GetFatherSpr()->getPosition();
   return ccpSub(pos_, anmPos);
}

void MainEditLayer::_updateRenderTarget()
{
    addChild(m_frameRenderTarget);
    m_frameRenderTarget->clear(0, 0, 0, 0);
    m_frameRenderTarget->begin();
    for (int i = 0; i < m_oneFrameSpritesArr->count(); ++i)
    {
        CCSprite* pSprite = dynamic_cast<CCSprite*>(m_oneFrameSpritesArr->objectAtIndex(i));
        if(pSprite)
            pSprite->visit();
    }
    m_frameRenderTarget->end();
    removeChild(m_frameRenderTarget);
}

cocos2d::extension::CCEditBox* MainEditLayer::_addNumEditBox(const float& ui_x_rate_, const float& ui_y_rate_)
{
    CCSize size = CCDirector::sharedDirector()->getWinSize();
    CCEditBox* editBox = CCEditBox::create(CCSizeMake(UI_BACK_WIDTH * 0.35f,  UI_BACK_HEIGHT * 0.05f),
                                                                   CCScale9Sprite::create("Resources/green_edit.png"));
    editBox->setOpacity(200);
    editBox->setPosition(ccp(size.width - UI_BACK_WIDTH/2 + UI_BACK_WIDTH * ui_x_rate_, size.height* ui_y_rate_));
    this->addChild(editBox, UI_BACK_ELEMENTS_Z_ORDER);
    editBox->setInputMode(kEditBoxInputModeNumeric);
    editBox->setDelegate(this);
    return editBox;
}
cocos2d::extension::CCControlSlider* MainEditLayer::_addSliderGroup(const float& ui_x_rate_, const float& ui_y_rate_,
                                                                                                            const float& min_, const float&  max_, const float&  now_,
                                                                                                            SEL_CCControlHandler hander_)
{
    CCSize size = CCDirector::sharedDirector()->getWinSize();
    
    CCControlSlider *slider = CCControlSlider::create("Resources/sliderTrack.png", "Resources/sliderProgress.png", "Resources/sliderThumb.png");
    slider->setAnchorPoint(ccp(0.5f, 1.0f));
    slider->setMinimumValue(min_); // Sets the min value of range
    slider->setMaximumValue(max_); // Sets the max value of range
    slider->setValue(now_);
    slider->setScaleX(0.75f);
    slider->setPosition(ccp(size.width - UI_BACK_WIDTH + UI_BACK_WIDTH * ui_x_rate_,  size.height * ui_y_rate_));
    slider->addTargetWithActionForControlEvents(this, hander_, CCControlEventValueChanged);
    this->addChild(slider, UI_BACK_ELEMENTS_Z_ORDER);
    
    return slider;
}

void MainEditLayer::_updateSprAttributeEditBox()
{
    const int boxChars = 15;
    char toBox[boxChars] = {0};
    sprintf(toBox, "%d",  (int)m_nowTouchSprite->getPositionX());
    m_editNowSprX->setText(toBox);
    memset(toBox, sizeof(char) * boxChars, 0);
    sprintf(toBox, "%d",  (int)m_nowTouchSprite->getPositionY());
    m_editNowSprY->setText(toBox);
    memset(toBox, sizeof(char) * boxChars, 0);
    sprintf(toBox, "%d",  (int)m_nowTouchSprite->getRotation());
    m_editNowSprRot->setText(toBox);
    memset(toBox, sizeof(char) * boxChars, 0);
    sprintf(toBox, "%d",  (int)m_nowTouchSprite->getZOrder());
    m_editNowSprLayer->setText(toBox);

    m_alphaSlider->setValue(m_nowTouchSprite->getOpacity());
    m_scaleSlider->setValue(m_nowTouchSprite->getScale() * 100);
}

void MainEditLayer::_updateRectAttributeEditBox()
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