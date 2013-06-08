#include "MainEditLayer.h"
#include "SimpleAudioEngine.h"
#include "CCEditBox.h"
#include "EgCommon.h"

#include "FrameEdit.h"
#include "RectEdit.h"
#include "AnimateEdit.h"

#include "tinyxml.h"

using namespace cocos2d;
using namespace CocosDenshion;
USING_NS_CC_EXT;

#define cccontrol_selector(_SELECTOR) (SEL_CCControlHandler)(&_SELECTOR)

const std::string RECT = "rect";
const std::string FRAME = "frame";
const std::string ANIMATE = "animate";

MainEditLayer* MainEditLayer::Self = NULL;

MainEditLayer::MainEditLayer()
{}

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
CCTexture2D* testTex = NULL;
// on "init" you need to initialize your instance
bool MainEditLayer::init()
{
    if ( !CCLayer::init() )
    {
        return false;
    }

    _addEditUiBase();

    this->schedule(schedule_selector(MainEditLayer::updateScene));
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this,0,true);
        
    Self = this;
    
    //init public elements//
    CCSize size = CCDirector::sharedDirector()->getWinSize();
    
    m_useBigSprArr = CCArray::create();
    m_useBigSprArr->retain();
    m_useBigSprList = new IconListComponent(ccp(size.width - UI_BACK_WIDTH, UI_BACK_HEIGHT * 0.85f),
                                                                       CCSize(UI_BACK_WIDTH, UI_BACK_HEIGHT * 0.1f));
    m_cutRectArr = CCArray::create();
    m_cutRectArr->retain();
    m_cutRectList = new IconListComponent(ccp(size.width - UI_BACK_WIDTH, UI_BACK_HEIGHT * 0.85f),
                                                                    CCSize(UI_BACK_WIDTH, UI_BACK_HEIGHT * 0.1f));
    
    m_oneFrameSprArr = CCArray::create();
    m_oneFrameSprArr->retain();
    m_oneFrameSprArrBac = m_oneFrameSprArr;
    m_oneFrameSprList = new IconListComponent(ccp(size.width - UI_BACK_WIDTH, UI_BACK_HEIGHT * 0.85f),
                                                                            CCSize(UI_BACK_WIDTH, UI_BACK_HEIGHT * 0.1f));
    m_allFramesList = new IconListComponent(ccp(size.width - UI_BACK_WIDTH, UI_BACK_HEIGHT * 0.85f),
                                                                       CCSize(UI_BACK_WIDTH, UI_BACK_HEIGHT * 0.1f));
    
    m_showAnimate = new CC2_EGG::Animate();
    //////////////////////
    
    //add this for test//
    CCSprite* dummySpr = CCSprite::create(RES_PATH("test.png"));
    m_bigSprPathMap[dummySpr->getTexture()] = "test.png";
    m_useBigSprArr->addObject(dummySpr);
    CCSprite* detail = CCSprite::createWithTexture(dummySpr->getTexture(), dummySpr->getTextureRect());
    m_useBigSprList->addList(detail);

    
    testTex = dummySpr->getTexture();
    
    CCSprite* dummySpr2 = CCSprite::create(RES_PATH("player_move.png"));
    m_bigSprPathMap[dummySpr2->getTexture()] = "player_move.png";
    m_useBigSprArr->addObject(dummySpr2);
    CCSprite* detail2 = CCSprite::createWithTexture(dummySpr2->getTexture(), dummySpr2->getTextureRect());
    m_useBigSprList->addList(detail2);
    ///////////////////
    
    m_frameRenderTarget = CCRenderTexture::create(size.width - UI_BACK_WIDTH, size.height);
    m_frameRenderTarget->retain();
    
    //init edit scenes

    
    m_editScenenMap[FRAME] = new FrameEditScene();
    m_editScenenMap[FRAME]->initUi();
    m_editScenenMap[RECT] = new RectEditScene();
    m_editScenenMap[RECT]->initUi();
    m_editScenenMap[ANIMATE] = new AnimateEditScene();
    m_editScenenMap[ANIMATE]->initUi();
    
    m_nowEditScene = m_editScenenMap[RECT];
    m_nowEditScene->showOnDestLayer(this);
    
    return true;
}

MainEditLayer::~MainEditLayer()
{
    m_useBigSprArr->release();
    delete m_useBigSprList;
    m_cutRectArr->release();
    delete m_cutRectList;
    m_oneFrameSprArr->release();
    delete m_oneFrameSprList;
    
    m_frameRenderTarget->release();
    m_frameRenderTarget = NULL;
}

cocos2d::CCTexture2D* MainEditLayer::getUseSprTex(const std::string& path_)
{
    for(Texture2DPathMap::iterator i = m_bigSprPathMap.begin(); i != m_bigSprPathMap.end(); ++i)
    {
        if(i->second == path_)
            return i->first;
    }
    
    CCSprite* foo = CCSprite::create(RES_PATH(path_.c_str()));
    m_useBigSprArr->addObject(foo);
    CCSprite* detail = CCSprite::createWithTexture(foo->getTexture(), foo->getTextureRect());
    m_useBigSprList->addList(detail);
    
    CCTexture2D* tex = foo->getTexture();
    tex->retain();
    m_bigSprPathMap[tex] = path_;
    return tex;
}

void MainEditLayer::addCutRect(cocos2d::CCTexture2D* tex_, const cocos2d::CCRect& rect_)
{
    for (int i = 0; i < m_cutRectArr->count(); ++i)
    {
        CCSprite* spr = (CCSprite*) (m_cutRectArr->objectAtIndex(i));
        if (spr->getTexture() == tex_ && spr->getTextureRect().equals(rect_))
        {
            return;
        }
    }
    
    m_cutRectArr->addObject(CCSprite::createWithTexture(tex_, rect_));
    m_cutRectList->addList(CCSprite::createWithTexture(tex_, rect_));
}

void MainEditLayer::rttFrameToList(cocos2d::CCArray* frame_)
{
    addChild(m_frameRenderTarget);
    m_frameRenderTarget->clear(0, 0, 0, 0);
    m_frameRenderTarget->begin();
    for (int i = 0; i < frame_->count(); ++i)
    {
        CCSprite* pSprite = dynamic_cast<CCSprite*>(frame_->objectAtIndex(i));
        if(pSprite)
            pSprite->visit();
    }
    m_frameRenderTarget->end();
    removeChild(m_frameRenderTarget);
    
    CCImage* pImage = m_frameRenderTarget->newCCImage();
    CCTexture2D* tex = new CCTexture2D();
    tex->initWithImage(pImage);
    CC_SAFE_DELETE(pImage);
    CCSprite* detail = CCSprite::createWithTexture(tex, m_frameRenderTarget->getSprite()->getTextureRect());
    getAllFramesList()->addList(detail);
}

bool MainEditLayer::ccTouchBegan(cocos2d::CCTouch *touch_, cocos2d::CCEvent *event_)
{
    return m_nowEditScene->ccTouchBegan(touch_);
}

void MainEditLayer::ccTouchMoved(cocos2d::CCTouch *touch_, cocos2d::CCEvent *event_)
{
    m_nowEditScene->ccTouchMoved(touch_);
}

void MainEditLayer::ccTouchEnded(cocos2d::CCTouch *touch_, cocos2d::CCEvent *event_)
{
    m_nowEditScene->ccTouchEnded(touch_);
}

void MainEditLayer::updateScene(float dt_)
{
#ifdef _WIN32
    if (wcscmp(m_nowDragFile.c_str(), CCEGLView::sharedOpenGLView()->getDragOnFile()))
    {
        int len = WideCharToMultiByte(CP_UTF8, 0, CCEGLView::sharedOpenGLView()->getDragOnFile(), -1, NULL, 0, NULL, NULL);
        char* str = new char[len+1];
        memset(str, 0, len+1);
        WideCharToMultiByte(CP_ACP, 0, CCEGLView::sharedOpenGLView()->getDragOnFile(), -1, str, len, NULL, NULL);
        
        CCSprite* dummySpr = CCSprite::create(str);
        delete[] str;
        m_useBigSprArr->addObject(dummySpr);
        CCSprite* detail = CCSprite::createWithTexture(dummySpr->getTexture(), dummySpr->getTextureRect());
        m_useBigSprList->addList(detail);        
        
        //dummySpr->release();
        
        m_nowDragFile = CCEGLView::sharedOpenGLView()->getDragOnFile();
    }
#endif
    m_nowEditScene->update(dt_);
}

//button callback function
void MainEditLayer::changeStateCutCallBack(CCObject* pSender_)
{
    _changeEditScene(RECT);
}

void MainEditLayer::changeStateFrameCallBack(CCObject* pSender_)
{
    _changeEditScene(FRAME);
}

void MainEditLayer::changeStateAnimateCallBack(CCObject* pSender_)
{
    _changeEditScene(ANIMATE);
}

void MainEditLayer::loadCallBack(CCObject* pSender_)
{
    std::string documentPath = CCFileUtils::sharedFileUtils()->getWriteablePath();
    std::string loadPath = documentPath + "test.dat";
    FILE *fp = NULL;
    fp = fopen(loadPath.c_str(), "r");
    if(NULL == fp)
        return;
    

    char check = 0;
    std::string mystream;
    while(fread(&check, 1, 1, fp))
    {
        char app[2] = {0};
        app[0] = check;
        mystream.append(app);
    }
    
    fclose(fp);

    TiXmlDocument myDocument("my_doc_name");
    myDocument.Parse(mystream.c_str());
    
    if(m_showAnimate)
        delete m_showAnimate;
    m_showAnimate = new CC2_EGG::Animate();
    
    CC2_EGG::SetAnimateData(m_showAnimate, &myDocument);
}

void MainEditLayer::saveCallBack(CCObject* pSender_)
{
    
    std::string documentPath = CCFileUtils::sharedFileUtils()->getWriteablePath();
    
    // Create a TiXmlDocument
    TiXmlDocument myDocument = TiXmlDocument("my_doc_name");
    CC2_EGG::GetAnimateData(m_showAnimate, &myDocument);
    // Declare a printer
    TiXmlPrinter printer;
    // attach it to the document you want to convert in to a std::string
    myDocument.Accept(&printer);
    // Create a std::string and copy your document data in to the string
    std::string str = printer.CStr();
    
    FILE *fp = NULL;
    std::string savePath;

    savePath = documentPath + "test.dat";
    
    fp = fopen(savePath.c_str(), "w");
    if(NULL == fp)
        return;
    
    fwrite(str.c_str(), strlen(str.c_str()), 1, fp);

    fclose(fp);
}

void MainEditLayer::menuCloseCallback(CCObject* pSender_)
{
    CCDirector::sharedDirector()->end();
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

//==================private function=======================

void MainEditLayer::_addEditUiBase()
{
    this->removeAllChildren();
    
    CCSize size = CCDirector::sharedDirector()->getWinSize();
    int uiBackWidth = UI_BACK_WIDTH;
    int uiBackHeight = UI_BACK_HEIGHT;
    m_backUiPic = CreatePureColorSpr(uiBackWidth, uiBackHeight, 0xff0f0fff);
    m_backUiPic->setPosition(ccp(size.width - uiBackWidth/2, size.height/2));
    this->addChild(m_backUiPic, UI_BACK_PIC_Z_ORDER);
    
    CCMenuItemImage *pCloseItem = CCMenuItemImage::create(
                                                          RES_PATH("CloseNormal.png"),
                                                          RES_PATH("CloseSelected.png"),
                                                          this,
                                                          menu_selector(MainEditLayer::menuCloseCallback) );
    pCloseItem->setPosition( ccp(CCDirector::sharedDirector()->getWinSize().width - 20, 20) );

    CCMenu* mainUiMenu = CCMenu::create();
    mainUiMenu->retain();
    
    mainUiMenu->setPosition( CCPointZero );
    this->addChild(mainUiMenu, UI_BACK_ELEMENTS_Z_ORDER);
    mainUiMenu->addChild(pCloseItem);
    
    
    //----------------------create top ui---------------------------
    CCSprite* topUiPic = CreatePureColorSpr(size.width - uiBackWidth, size.height * 0.05f, 0xffAfAfff);
    topUiPic->setPosition(ccp((size.width - uiBackWidth)/2, size.height - (size.height * 0.05f)/2));
    this->addChild(topUiPic, UI_BACK_PIC_Z_ORDER);
    
    AddFontBtn(ZH2U("切图编辑"), this, menu_selector(MainEditLayer::changeStateCutCallBack), ccp(30.0f + size.width * 0.05f,  size.height * 0.975f), mainUiMenu);
    AddFontBtn(ZH2U("桢编辑"), this, menu_selector(MainEditLayer::changeStateFrameCallBack), ccp(30.0f + size.width * 0.2f,  size.height * 0.975f), mainUiMenu);
    AddFontBtn(ZH2U("动画编辑"), this, menu_selector(MainEditLayer::changeStateAnimateCallBack), ccp(30.0f + size.width * 0.35f,  size.height * 0.975f), mainUiMenu);
    AddFontBtn(ZH2U("保存"), this, menu_selector(MainEditLayer::saveCallBack), ccp(30.0f + size.width *0.55f,  size.height * 0.975f), mainUiMenu);
    AddFontBtn(ZH2U("读取"), this, menu_selector(MainEditLayer::loadCallBack), ccp(30.0f + size.width *0.7f,  size.height * 0.975f), mainUiMenu);
    //--------------------------------------------------------------
}

void MainEditLayer::_changeEditScene(const std::string& id_)
{
    assert(m_editScenenMap.find(id_) != m_editScenenMap.end());    
    
    m_nowEditScene->closeOnDestLayer();
    m_editScenenMap[id_]->showOnDestLayer(this);
    m_nowEditScene = m_editScenenMap[id_];
}
