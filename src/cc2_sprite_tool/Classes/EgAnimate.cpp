#include "EgAnimate.h"
#include "cocos2d.h"

USING_NS_CC;

namespace CC2_EGG
{
    cocos2d::CCSprite* CopySprite(cocos2d::CCSprite* src_)
    {
        CCSprite* newSprite = CCSprite::createWithTexture(src_->getTexture(), src_->getTextureRect());
        newSprite->setPosition(src_->getPosition());
        newSprite->setRotation(src_->getRotation());
        newSprite->setFlipX(src_->isFlipX());
        newSprite->setFlipY(src_->isFlipY());
        newSprite->setOpacity(src_->getOpacity());
        newSprite->setScale(src_->getScale());
        newSprite->setZOrder(src_->getZOrder());  
        return newSprite;
    }

    Animate::Animate():
    m_destDrawLayer(NULL), m_timeCount(0.0f), m_changeFrameTime(0.05f),m_nowFrame(0), m_bIsStop(false),m_animateFrames(NULL)
    {
        CCSize size = CCDirector::sharedDirector()->getWinSize();
        m_anmFramesFather = CCSprite::create();
        m_anmFramesFather->retain();
        m_anmFramesFather->setPosition(ccp(size.width/2, size.height/2));
        m_boundBox.size = CCSize(512, 512);
        m_boundBox.origin = CCPoint(ccp(size.width/2 - 256, size.height/2 + 256));
    }
    
    Animate::~Animate()
    {
        if(m_animateFrames)
            m_animateFrames->release();
        m_anmFramesFather->release();
    }

    void Animate::AddSprOnNowFrame(cocos2d::CCSprite* spr_)
    {
        CCSprite* newSprite = CopySprite(spr_);
        newSprite->setZOrder(0);
        GetNowFrameSprites()->addObject(newSprite);        
        m_anmFramesFather->addChild(newSprite);
    }
    
    void Animate::AddFrame(cocos2d::CCArray *frame_, const cocos2d::CCSize size_)
    {
        if(!m_animateFrames)
        {
            m_animateFrames = CCArray::create();
            m_animateFrames->retain();
        }
        

        CCArray* addFrame = CCArray::create();
        for (int i = 0; i < frame_->count(); ++i)
        {
            //copy sprite
            CCSprite* oldSprite = dynamic_cast<CCSprite*>(frame_->objectAtIndex(i));
            CCSprite* newSprite = CopySprite(oldSprite);
            //
            
            //set new position
            newSprite->setPositionX( oldSprite->getPosition().x - m_anmFramesFather->getPositionX() );//change rect origin
            newSprite->setPositionY( oldSprite->getPosition().y - m_anmFramesFather->getPositionY() );
            
            addFrame->addObject(newSprite);
        }
        addFrame->retain();
        m_animateFrames->addObject(addFrame);
        m_animateFramesSize.push_back(size_);

        m_boundBox.size = size_;
    }
    
    void Animate::RunWithTargetLayer(cocos2d::CCLayer* dest_)
    {
        m_destDrawLayer = dest_;
        m_nowFrame = 0;
        m_timeCount = 0.0f;
        m_bIsStop = false;
        m_anmFramesFather->removeAllChildren();
        _addNowFrame();
        m_destDrawLayer->addChild(m_anmFramesFather);
    }

    void Animate::SetTargetLayer(cocos2d::CCLayer* dest_)
    {
        m_destDrawLayer = dest_;
        _addNowFrame();
        m_destDrawLayer->addChild(m_anmFramesFather);
    }
    
    void Animate::RemoveFormTargetLayer()
    {
        if(m_animateFrames && m_animateFrames->count() > 0 && m_destDrawLayer)
        {
            _removeNowFrame();
            m_destDrawLayer->removeChild(m_anmFramesFather);
            m_destDrawLayer = NULL;
            m_nowFrame = 0;
            m_timeCount = 0.0f;
            m_bIsStop = true;
        }
    }
    
    void Animate::Update(const float &dTime_)
    {
        if(NULL == m_destDrawLayer || m_bIsStop || 0 == m_animateFrames || 0 == m_animateFrames->count())
            return;
        m_timeCount += dTime_;
        if (m_timeCount >= m_changeFrameTime)
        {
            m_timeCount = 0.0f;
            _removeNowFrame();
            if(++m_nowFrame == m_animateFrames->count())
                m_nowFrame = 0;            
            _addNowFrame();
            
        }
    }
    
    const cocos2d::CCRect& Animate::GetBoundBox()
    {
         return m_boundBox;
    }

    cocos2d::CCArray* Animate::GetNowFrameSprites()
    {
       return dynamic_cast<CCArray*> (m_animateFrames->objectAtIndex(m_nowFrame));
    }
    
    void Animate::DeleteFrame(const int& index_)
    {
        m_animateFrames->removeObjectAtIndex(index_);
        m_animateFramesSize.erase(m_animateFramesSize.begin() + index_);
        if (m_animateFrames->count() == 0)
        {
            m_animateFrames->release();
            m_animateFrames = NULL;
        }
        
    }

    void Animate::SwapFrame(const int& sw1_, const int& sw2_)
    {
        if(m_destDrawLayer)
        {
            CCLayer* dest = m_destDrawLayer;
            RemoveFormTargetLayer();
            m_destDrawLayer = dest;
        }

        m_animateFrames->exchangeObjectAtIndex(sw1_, sw2_);
        CCSize temp = m_animateFramesSize[sw1_];
        m_animateFramesSize[sw1_] = m_animateFramesSize[sw2_];
        m_animateFramesSize[sw2_] = temp;
        if(m_destDrawLayer)
            RunWithTargetLayer(m_destDrawLayer);
    }

    void Animate::SetFSpeedPerSecond(const float& spd_)
    {
        m_changeFrameTime = 1.0f/spd_;
    }

    void Animate::Play()
    {
        m_bIsStop = false;
    }

    void Animate::Stop()
    {
        m_bIsStop = true;
    }

    //-------private function---------
    void Animate::_addNowFrame()
    {
        if(m_animateFrames)
        {
            CCArray* addFrame = dynamic_cast<CCArray*> (m_animateFrames->objectAtIndex(m_nowFrame));
            for(int i = 0;  i != addFrame->count(); ++i)
                m_anmFramesFather->addChild(dynamic_cast<CCSprite*>(addFrame->objectAtIndex(i)));
            m_boundBox.size = m_animateFramesSize[m_nowFrame];
        }
    }
    
    void Animate::_removeNowFrame()
    {
         if(m_animateFrames)
         {
             CCArray* removeFrame = dynamic_cast<CCArray*> (m_animateFrames->objectAtIndex(m_nowFrame));
             for(int i = 0;  i != removeFrame->count(); ++i)
                 m_anmFramesFather->removeChild(dynamic_cast<CCSprite*>(removeFrame->objectAtIndex(i)), false);
         }
    }
};