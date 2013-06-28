#ifndef  EGG_COMMON_H_
#define EGG_COMMON_H_

#include "cocos2d.h"
#include "cocos-ext.h"

//==============================================

class GLDrawLayer:public cocos2d::CCLayer
{
public:
    GLDrawLayer();
    ~GLDrawLayer();
    virtual void draw();
    void move(const cocos2d::CCPoint& offset_);
    void setDrawRectStart(const cocos2d::CCPoint& start_);
    void setDrawRectEnd(const cocos2d::CCPoint& end_);
    void setDrawRect(const cocos2d::CCRect& rect_);
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
    
    void addList(cocos2d::CCSprite* spr_, bool bIsDetail = true);
    
    void swapIndex(const int& trueIndex1_, const int& trueIndex2_);
    
    void copyIndex(const int& index_);
    
    void updateIcon(const int& index_);
    
    bool deletIconByShowIndex(const int& showIndex_);
    
    bool deletIconByTrueIndex(const int& trueIndex_);
    
    int getTrueIndexByShowIndex(const int& showIndex_);
    int getShowIndexByTrueIndex(const int& trueIndex_);
    
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

//==================================================

#ifdef _WIN32
std::string G2U(const char* gb2312);
#define ZH2U(zh_) (G2U(zh_).c_str())
#endif

#ifdef  __APPLE__
#define ZH2U(zh_) (zh_)
#endif

#ifdef _WIN32
const char* const SOURCE_ROOT = "Resources/";
#endif

#ifdef __APPLE__
const char* const SOURCE_ROOT = "";
#endif

const int UI_BACK_PIC_Z_ORDER = 99999;
const int UI_BACK_ELEMENTS_Z_ORDER = 100000;
const int UI_BACK_WIDTH = 200;
const int UI_BACK_HEIGHT = 768;

std::string GetResPath(const char* res_);
#define RES_PATH(rs_) (GetResPath(rs_).c_str())

cocos2d::CCSprite* CreatePureColorSpr(const int& w_, const int& h_, const unsigned int& col_);

cocos2d::CCRect MakeRect(const int& x1, const int& y1, const int& x2, const int& y2);

void ScaleAndMoveCCSprite2Rect(cocos2d::CCSprite* spr_, const cocos2d::CCRect& rect_);

void GetEdge1DPoint(const float& p1, const float& p2, const float& pi, float& outp);
void GetEdge2DPoint(const cocos2d::CCPoint& p1, const cocos2d::CCPoint& p2, const cocos2d::CCPoint& pi, cocos2d::CCPoint& outp);
void SetBoxTextByVar(const int& var_, cocos2d::extension::CCEditBox* box_);

cocos2d::CCMenuItemFont* AddFontBtn(const char *value_, cocos2d::CCObject* target_, cocos2d::SEL_MenuHandler selector_,
                                                               const cocos2d::CCPoint& pos_, cocos2d::CCMenu* menu_, const int& fontSize_ = 22);

cocos2d::CCSprite* CheckSpriteTouch(cocos2d::CCArray* sprArray_, const cocos2d::CCPoint& touch_,
                                                           int& outIndex_, bool isNewSpr_ = false);


cocos2d::CCSprite* CreateAndAddMask(cocos2d::CCSprite* ref_, cocos2d::CCLayer* lay_);

void AddSprArrToLayer(cocos2d::CCArray* sprArray_, cocos2d::CCLayer* lay_);
void RemoveSprArrFromLayer(cocos2d::CCArray* sprArray_, cocos2d::CCLayer* lay_);

cocos2d::extension::CCEditBox* AddNumEditBox(const float& ui_x_rate_, const float& ui_y_rate_,
                                                                            cocos2d::extension::CCEditBoxDelegate* delegate_, cocos2d::CCLayer* lay_ = NULL,
                                                                            const float& sizeW_ = 0.35f, const float& sizeH_ = 0.05f);

cocos2d::extension::CCControlSlider* AddSliderGroup(const float& ui_x_rate_, const float& ui_y_rate_,
                                                                                    const float& min_, const float&  max_, const float&  now_,
                                                                                    cocos2d::extension::SEL_CCControlHandler hander_,
                                                                                    cocos2d::CCObject* target_, cocos2d::CCLayer* lay_);

#endif//endif EGG_COMMON_H_