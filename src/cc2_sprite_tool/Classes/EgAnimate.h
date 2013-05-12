//
//  sprite_toolAppDelegate.h
//  sprite_tool
//
//  Created by dummy on 13-4-11.
//  Copyright __MyCompanyName__ 2013年. All rights reserved.
//

#ifndef  _W_EGG_DELEGATE_H_
#define  _W_EGG_DELEGATE_H_

#include <vector>
#include "cocoa\CCGeometry.h"

namespace cocos2d
{
    class CCArray;
    class CCLayer;
    class CCSprite;
};

namespace CC2_EGG
{
    cocos2d::CCSprite* CopySprite(cocos2d::CCSprite* src_);

    class Animate
    {
    public:
        Animate();
        ~Animate();
        
        void AddFrame(cocos2d::CCArray* frame_, const cocos2d::CCSize size_);
        void AddSprOnNowFrame(cocos2d::CCSprite* spr_);
        void DeleteFrame(const int& index_);
        void SwapFrame(const int& sw1_, const int& sw2_);
        void RunWithTargetLayer(cocos2d::CCLayer* dest_);
        void SetTargetLayer(cocos2d::CCLayer* dest_);

        void RemoveFormTargetLayer();
        void Update(const float& dTime_);

        void Play();
        void Stop();
        void SetFSpeedPerSecond(const float& spd_ = 20.0f);
        
        cocos2d::CCArray* GetNowFrameSprites();
        const cocos2d::CCRect& GetBoundBox();

        cocos2d::CCSprite* GetFatherSpr() {return m_anmFramesFather;}
    private:
        cocos2d::CCArray* m_animateFrames;//a ccarray fill with sprites ccarray...
        typedef std::vector<cocos2d::CCSize> CCSizeArray;
        CCSizeArray  m_animateFramesSize;
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
};



#endif // _APP_DELEGATE_H_

