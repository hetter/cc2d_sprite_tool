#ifndef __MainEditLayer_SCENE_H__
#define __MainEditLayer_SCENE_H__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "EgAnimate.h"

class BaseEditScene;
class IconListComponent;

class MainEditLayer : public cocos2d::CCLayer
{
public:
    static MainEditLayer* Self;
public:
    MainEditLayer();
    ~MainEditLayer();
    // Method 'init' in cocos2d-x returns bool, instead of 'id' in cocos2d-iphone (an object pointer)
    virtual bool init();

    // there's no 'id' in cpp, so we recommend to return the class instance pointer
    static cocos2d::CCScene* scene();

    // over write from CCStandardTouchDelegate
    bool ccTouchBegan   (cocos2d::CCTouch* touch_, cocos2d:: CCEvent* event_);
    void ccTouchMoved  (cocos2d::CCTouch* touch_, cocos2d:: CCEvent* event_);
    void ccTouchEnded   (cocos2d::CCTouch* touch_, cocos2d:: CCEvent* event_);
private:
    BaseEditScene* m_nowEditScene;
    std::map<std::string, BaseEditScene*> m_editScenenMap;
    
    std::wstring m_nowDragFile;
    
   
 //===============public elements==================
public:
    cocos2d::CCArray*      getUseBigSprArray()         {return m_useBigSprArr;}
    IconListComponent*    getUseBigSprList()           {return m_useBigSprList;}
    cocos2d::CCArray*      getCutRectArray()            {return m_cutRectArr;}
    IconListComponent*    getCutRectList()              {return m_cutRectList;}
    cocos2d::CCArray*      getOneFrameSprArray()   {return m_oneFrameSprArr;}
    void                            setOneFrameSprArray(cocos2d::CCArray* frm_)   {m_oneFrameSprArr = frm_;}
    cocos2d::CCArray*      getOneFrameSprArrayBac()   {return m_oneFrameSprArrBac;}
    void                            setOneFrameSprArrayBac()   {m_oneFrameSprArrBac = m_oneFrameSprArr;}
    IconListComponent*    getOneFrameSprList()     {return m_oneFrameSprList;}
    cocos2d::CCArray*      geAllFramesArray()          {return m_allFramesArr;}
    IconListComponent*    getAllFramesList()           {return m_allFramesList;}
    

    
    cocos2d::CCSprite*      getBackUiPic() {return m_backUiPic;}
    
    CC2_EGG::Animate*      getShowAnimate() {return m_showAnimate;}
    
    std::string getUseSprPath(cocos2d::CCTexture2D* tex_)
    {
        if(m_bigSprPathMap.find(tex_) != m_bigSprPathMap.end())
            return m_bigSprPathMap[tex_];
        return "";
    }
    
    cocos2d::CCTexture2D* getUseSprTex(const std::string& path_);
    
    void addCutRect(cocos2d::CCTexture2D* tex_, const cocos2d::CCRect& rect_);
    
    void rttFrameToList(cocos2d::CCArray* frame_);
private:
    cocos2d::CCArray*        m_useBigSprArr;//store of big sprite
    IconListComponent*      m_useBigSprList;
    cocos2d::CCArray*        m_cutRectArr;//store of cut rect sprite
    IconListComponent*      m_cutRectList;
    cocos2d::CCArray*        m_oneFrameSprArr;//store of now frame all sprite
    cocos2d::CCArray*        m_oneFrameSprArrBac;//store of now frame all sprite Bac

    IconListComponent*      m_oneFrameSprList;
    cocos2d::CCArray*        m_allFramesArr;//store of all frame
    IconListComponent*      m_allFramesList;
    cocos2d::CCSprite*        m_backUiPic;
    CC2_EGG::Animate*       m_showAnimate;
    
    typedef std::map<cocos2d::CCTexture2D*, std::string> Texture2DPathMap;
    Texture2DPathMap m_bigSprPathMap;
    
    cocos2d::CCRenderTexture*  m_frameRenderTarget;//rtt the animate frame to add list icon
//===========================================
public:
    //call back function
    void updateScene(float dt_);
    
    //top ui
    void changeStateCutCallBack(CCObject* pSender_);
    void changeStateFrameCallBack(CCObject* pSender_);
    void changeStateAnimateCallBack(CCObject* pSender_);
    void saveCallBack(CCObject* pSender_);
    void loadCallBack(CCObject* pSender_);
    
    // a selector callback
    void menuCloseCallback(CCObject* pSender);
    
private:
    void _addEditUiBase();
public:
    void _changeEditScene(const std::string& id_);
};

#define SMainLayer (MainEditLayer::Self)

#endif // __MainEditLayer_SCENE_H__
