//
//  sprite_toolAppDelegate.h
//  sprite_tool
//
//  Created by dummy on 13-4-11.
//  Copyright __MyCompanyName__ 2013年. All rights reserved.
//

#ifndef  _W_EGG_DELEGATE_H_
#define  _W_EGG_DELEGATE_H_

#define  ENABLE_ANIMATE_RW 1

#include <vector>
#include "CCGeometry.h"

namespace cocos2d
{
    class CCArray;
    class CCLayer;
    class CCSprite;
};

#if ENABLE_ANIMATE_RW
class TiXmlDocument;
#endif

namespace CC2_EGG
{
    cocos2d::CCSprite* CopySprite(cocos2d::CCSprite* src_);
    
    class Animate
    {
#if   ENABLE_ANIMATE_RW
        friend  void SetAnimateData(Animate*  outAnm_,  TiXmlDocument* inData_);
        friend  void GetAnimateData(Animate* inAnm_,  TiXmlDocument* outData_);
#endif
    public:
        Animate();
        ~Animate();
        
        void addFrame(cocos2d::CCArray* frame_, const cocos2d::CCSize size_, const float& fTime_ = 15.0f);
        void addSprOnNowFrame(cocos2d::CCSprite* spr_);
        void deleteFrame(const int& index_);
        void swapFrame(const int& sw1_, const int& sw2_);
        void runWithTargetLayer(cocos2d::CCLayer* dest_);
        void setTargetLayer(cocos2d::CCLayer* dest_);
        
        void copyFrame(const int& index_);

        void removeFormTargetLayer();
        void update(const float& dTime_);

        void play();
        void stop();
        void setFSpeedPerSecond(const float& spd_ = 20.0f);
        
        //only for edit//
        void setAnimateCentre(const cocos2d::CCPoint& acPoint_);
        ///////////////
        
        cocos2d::CCArray* getFrameSprites(const int& index_);
        cocos2d::CCArray* getAllFrames() {return m_animateFrames;}
        
        const cocos2d::CCRect& getBoundBox();

        cocos2d::CCSprite* getFatherSpr() {return m_anmFramesFather;}

        //add for tools//
        float getFrameTime(const int& index_)
        {
            return m_animateFramesTime[index_];
        }
        cocos2d::CCSize getFrameBox(const int& index_)
        {
            return m_animateFramesSize[index_];
        }
        void setFrameBox(const int& index_, const cocos2d::CCSize& size_)
        {
            m_animateFramesSize[index_] = size_;
        }
        void setFrameTime(const int& index_, const float& time_)
        {
            m_animateFramesTime[index_] = time_;
        }
        ////////////////
    private:
        cocos2d::CCArray* m_animateFrames;//a ccarray fill with sprites ccarray...
        typedef std::vector<cocos2d::CCSize> CCSizeArray;
        CCSizeArray  m_animateFramesSize;
        std::vector<float> m_animateFramesTime;
        cocos2d::CCSprite* m_anmFramesFather;
        unsigned int m_nowFrame;
        cocos2d::CCLayer* m_destDrawLayer;
        cocos2d::CCRect m_boundBox;

        float m_changeFrameTime;
        float m_timeCount;
        bool m_bIsStop;
    private:
        void _addNowFrame();
        void _removeNowFrame();
    };
    
#if  ENABLE_ANIMATE_RW
    void SetAnimateData(Animate*  outAnm_,  TiXmlDocument* inData_);
    void GetAnimateData(Animate* inAnm_,  TiXmlDocument* outData_);
#endif
};



#endif // _APP_DELEGATE_H_

