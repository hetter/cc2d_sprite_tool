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

const std::string CUTRECT = "rect";
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
#ifdef _WIN32
    CCMenuItemFont::setFontName("微软雅黑");
#endif

    m_bIsSaveAnmFile = false;
    m_bIsLoadAnmFile = false;

    _addEditUiBase();

    this->schedule(schedule_selector(MainEditLayer::updateScene));
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this,0,true);
        
    Self = this;
    
    //init public elements//
    CCSize size = CCDirector::sharedDirector()->getWinSize();
    
    m_useBigSprArr = CCArray::create();
    m_useBigSprArr->retain();
    m_useBigSprList = new IconListComponent(ccp(size.width - UI_BACK_WIDTH, UI_BACK_HEIGHT * 0.75f),
                                                                       CCSize(UI_BACK_WIDTH, UI_BACK_HEIGHT * 0.1f), 6);
    m_cutRectArr = CCArray::create();
    m_cutRectArr->retain();
    m_cutRectList = new IconListComponent(ccp(size.width - UI_BACK_WIDTH, UI_BACK_HEIGHT * 0.75f),
                                                                    CCSize(UI_BACK_WIDTH, UI_BACK_HEIGHT * 0.1f), 6);
    
    m_oneFrameSprArr = CCArray::create();
    m_oneFrameSprArr->retain();
    m_oneFrameSprArrBac = m_oneFrameSprArr;
    m_oneFrameSprList = new IconListComponent(ccp(size.width - UI_BACK_WIDTH, UI_BACK_HEIGHT * 0.75f),
                                                                            CCSize(UI_BACK_WIDTH, UI_BACK_HEIGHT * 0.1f), 5);
    m_allFramesList = new IconListComponent(ccp(size.width - UI_BACK_WIDTH, UI_BACK_HEIGHT * 0.75f),
                                                                       CCSize(UI_BACK_WIDTH, UI_BACK_HEIGHT * 0.1f), 6);
    
    m_showAnimate = new CC2_EGG::Animate();
    //////////////////////
    
    //add this for test//
    //CCSprite* dummySpr = CCSprite::create(RES_PATH("test.png"));
    //m_bigSprPathMap[dummySpr->getTexture()] = "test.png";
    //m_useBigSprArr->addObject(dummySpr);
    //CCSprite* detail = CCSprite::createWithTexture(dummySpr->getTexture(), dummySpr->getTextureRect());
    //m_useBigSprList->addList(detail);

    //
    //testTex = dummySpr->getTexture();
    //
    //CCSprite* dummySpr2 = CCSprite::create(RES_PATH("player_move.png"));
    //m_bigSprPathMap[dummySpr2->getTexture()] = "player_move.png";
    //m_useBigSprArr->addObject(dummySpr2);
    //CCSprite* detail2 = CCSprite::createWithTexture(dummySpr2->getTexture(), dummySpr2->getTextureRect());
    //m_useBigSprList->addList(detail2);
    ///////////////////
    
    m_frameRenderTarget = CCRenderTexture::create(size.width - UI_BACK_WIDTH, size.height);
    m_frameRenderTarget->retain();
    
    //init edit scenes

    
    m_editScenenMap[FRAME] = new FrameEditScene();
    m_editScenenMap[FRAME]->initUi();
    m_editScenenMap[CUTRECT] = new RectEditScene();
    m_editScenenMap[CUTRECT]->initUi();
    m_editScenenMap[ANIMATE] = new AnimateEditScene();
    m_editScenenMap[ANIMATE]->initUi();
    
    m_nowEditScene = m_editScenenMap[CUTRECT];
    m_nowEditScene->showOnDestLayer(this);

    glClearColor(.5f, .5f, .5f, 0.0f);

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
    
    //CCSprite* foo = CCSprite::create(RES_PATH(path_.c_str()));
    CCSprite* foo = CCSprite::create(path_.c_str());
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
        std::string str = W2C(CCEGLView::sharedOpenGLView()->getDragOnFile());
        
        //CCSprite* dummySpr = CCSprite::create(str.c_str());
        //m_useBigSprArr->addObject(dummySpr);
        //CCSprite* detail = CCSprite::createWithTexture(dummySpr->getTexture(), dummySpr->getTextureRect());
        //m_useBigSprList->addList(detail);        
        getUseSprTex(str);
        //dummySpr->release();
        
        m_nowDragFile = CCEGLView::sharedOpenGLView()->getDragOnFile();
    }

#endif

    if(m_nowOpenDlgFile[0] != 0)
    {
        if(m_bIsSaveAnmFile)
        {
            _saveAnmFile();
            m_bIsSaveAnmFile = false;
        }
        else if(m_bIsLoadAnmFile)
        {
            _loadAnmFile();
            m_bIsLoadAnmFile = false;
        }

        memset(m_nowOpenDlgFile, 0, FILE_PATH_LENTH);
        m_nowOpenDlgFile[0] = '\0';
    }

    m_nowEditScene->update(dt_);

#ifdef _WIN32

    UINT message(0);
    WPARAM param(0);

    CCEGLView::sharedOpenGLView()->getWin32MsgAndParam(message, param);

    if(message == WM_KEYDOWN)
    {
        if(param == 0x33)// num 3
        {
            _changeEditScene(ANIMATE);
        }
        else if(param == 0x32)//num 2
        {
            _changeEditScene(FRAME);
        }
        else if(param == 0x31)//num 1
        {
            _changeEditScene(CUTRECT);
        }
    }

    CCEGLView::sharedOpenGLView()->resetWin32MsgAndParam();
#endif
}

//button callback function
void MainEditLayer::changeStateCutCallBack(CCObject* pSender_)
{
    _changeEditScene(CUTRECT);
}

void MainEditLayer::changeStateFrameCallBack(CCObject* pSender_)
{
    _changeEditScene(FRAME);
}

void MainEditLayer::changeStateAnimateCallBack(CCObject* pSender_)
{
    _changeEditScene(ANIMATE);
}

BOOL OpenFileDialog(wchar_t* pathPtr_)
{
    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    // must !
    ofn.lpstrFile = pathPtr_;
    ofn.nMaxFile = MainEditLayer::FILE_PATH_LENTH;
    //
    //ofn.lpstrFilter = L"act";

    ofn.lpstrFile[0] = '\0';
    //no extention file!    ofn.lpstrFilter="Any file(*.*)\0*.*\0ddfs\0ddfs*\0";
    return(GetOpenFileName((LPOPENFILENAME)&ofn));
}

void MainEditLayer::loadCallBack(CCObject* pSender_)
{
    OpenFileDialog(m_nowOpenDlgFile);
    m_bIsLoadAnmFile = true;
}

void MainEditLayer::saveCallBack(CCObject* pSender_)
{
    OpenFileDialog(m_nowOpenDlgFile);
    m_bIsSaveAnmFile = true;
}

void MainEditLayer::backWhiteCallBack(CCObject* pSender_)
{
    glClearColor(1, 1, 1, 0);
}

void MainEditLayer::backBlackCallBack(CCObject* pSender_)
{
    glClearColor(0, 0, 0, 0);
}

void MainEditLayer::backGrayCallBack(CCObject* pSender_)
{
    glClearColor(0.5, 0.5, 0.5, 0);
}

void MainEditLayer::backRandomCallBack(CCObject* pSender_)
{
    glClearColor(CCRANDOM_0_1(), CCRANDOM_0_1(), CCRANDOM_0_1(), 0);
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
    
    //CCMenuItemImage *pCloseItem = CCMenuItemImage::create(
    //                                                      RES_PATH("CloseNormal.png"),
    //                                                      RES_PATH("CloseSelected.png"),
    //                                                      this,
    //                                                      menu_selector(MainEditLayer::menuCloseCallback) );
    //pCloseItem->setPosition( ccp(CCDirector::sharedDirector()->getWinSize().width - 20, 20) );

    CCMenu* mainUiMenu = CCMenu::create();
    mainUiMenu->retain();
    
    mainUiMenu->setPosition( CCPointZero );
    this->addChild(mainUiMenu, UI_BACK_ELEMENTS_Z_ORDER);
    //mainUiMenu->addChild(pCloseItem);
    
    
    //----------------------create top ui---------------------------
    CCSprite* topUiPic = CreatePureColorSpr(size.width - uiBackWidth, size.height * 0.05f, 0xffAfAfff);
    topUiPic->setPosition(ccp((size.width - uiBackWidth)/2, size.height - (size.height * 0.05f)/2));
    this->addChild(topUiPic, UI_BACK_PIC_Z_ORDER);
    
    AddFontBtn(ZH2U("切图编辑"), this, menu_selector(MainEditLayer::changeStateCutCallBack), ccp(30.0f + size.width * 0.4f,  size.height * 0.975f), mainUiMenu);
    AddFontBtn(ZH2U("桢编辑"), this, menu_selector(MainEditLayer::changeStateFrameCallBack), ccp(30.0f + size.width * 0.475f,  size.height * 0.975f), mainUiMenu);
    AddFontBtn(ZH2U("动画编辑"), this, menu_selector(MainEditLayer::changeStateAnimateCallBack), ccp(30.0f + size.width * 0.55f,  size.height * 0.975f), mainUiMenu);
    AddFontBtn(ZH2U("保存"), this, menu_selector(MainEditLayer::saveCallBack), ccp(30.0f + size.width * 0.615f,  size.height * 0.975f), mainUiMenu);
    AddFontBtn(ZH2U("读取"), this, menu_selector(MainEditLayer::loadCallBack), ccp(30.0f + size.width * 0.665f,  size.height * 0.975f), mainUiMenu);
    
    CCMenuItemImage *whiteItem = CCMenuItemImage::create(
        RES_PATH("white.png"),
        RES_PATH("white.png"),
        this,
        menu_selector(MainEditLayer::backWhiteCallBack));
    whiteItem->setPosition(ccp(32.0f, size.height * 0.975f));
    mainUiMenu->addChild(whiteItem);
    CCMenuItemImage *blackItem = CCMenuItemImage::create(
        RES_PATH("black.png"),
        RES_PATH("black.png"),
        this,
        menu_selector(MainEditLayer::backBlackCallBack));
    blackItem->setPosition(ccp(64.0f, size.height * 0.975f));
    mainUiMenu->addChild(blackItem);
    CCMenuItemImage *grayItem = CCMenuItemImage::create(
        RES_PATH("gray.png"),
        RES_PATH("gray.png"),
        this,
        menu_selector(MainEditLayer::backGrayCallBack));
    grayItem->setPosition(ccp(96.0f, size.height * 0.975f));
    mainUiMenu->addChild(grayItem);

    CCMenuItemImage *ranItem = CCMenuItemImage::create(
        RES_PATH("random.png"),
        RES_PATH("random.png"),
        this,
        menu_selector(MainEditLayer::backRandomCallBack));
    ranItem->setPosition(ccp(128.0f, size.height * 0.975f));
    mainUiMenu->addChild(ranItem);

    //--------------------------------------------------------------
}

void MainEditLayer::_changeEditScene(const std::string& id_)
{
    assert(m_editScenenMap.find(id_) != m_editScenenMap.end());    
    
    m_nowEditScene->closeOnDestLayer();
    m_editScenenMap[id_]->showOnDestLayer(this);
    m_nowEditScene = m_editScenenMap[id_];
}

void MainEditLayer::_saveAnmFile()
{

    std::string documentPath = "";//CCFileUtils::sharedFileUtils()->getWriteablePath();

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
    std::string savePath = W2C(m_nowOpenDlgFile);

    //savePath = documentPath + "test.dat";

    fp = fopen(savePath.c_str(), "w");
    if(NULL == fp)
        return;

    fwrite(str.c_str(), strlen(str.c_str()), 1, fp);

    fclose(fp);
}

void MainEditLayer::_loadAnmFile()
{
    if(m_editScenenMap[FRAME] == m_nowEditScene)
        ((FrameEditScene*)m_editScenenMap[FRAME])->refresh(true);
    else
        ((FrameEditScene*)m_editScenenMap[FRAME])->refresh(false);

    getAllFramesList()->clean();

    std::string documentPath = "";//CCFileUtils::sharedFileUtils()->getWriteablePath();
    std::string loadPath = W2C(m_nowOpenDlgFile);//documentPath + "test.dat";
    FILE *fp = NULL;
    fp = fopen(loadPath.c_str(), "r");
    if(NULL == fp)
        return;

    int pos = loadPath.find_last_of("\\");
    if(pos == -1)
        m_anmFilePath = "";
    else
        m_anmFilePath = loadPath.substr(0, pos + 1); 
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
    m_showAnimate->removeFormTargetLayer();
    if(m_showAnimate)
        delete m_showAnimate;
    m_showAnimate = new CC2_EGG::Animate();

    CC2_EGG::SetAnimateData(m_showAnimate, &myDocument);


}
