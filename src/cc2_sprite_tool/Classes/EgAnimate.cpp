#include "EgAnimate.h"
#include "cocos2d.h"

#if  ENABLE_ANIMATE_RW
#include "MainEditLayer.h"
#include "tinyxml.h"
#endif

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
        m_anmFramesFather->setPosition(ccp(0, 0));
        m_boundBox.size = CCSize(512, 512);
        m_boundBox.origin = CCPoint(ccp(size.width/2 - 256, size.height/2 + 256));
    }
    
    Animate::~Animate()
    {
        if(m_animateFrames)
            m_animateFrames->release();
        m_anmFramesFather->release();
    }
    
    void Animate::copyFrame(const int &index_)
    {
        CCArray* frame = dynamic_cast<CCArray*> (m_animateFrames->objectAtIndex(index_));
        addFrame(frame, m_animateFramesSize[index_], m_animateFramesTime[index_]);
    }

    void Animate::addSprOnNowFrame(cocos2d::CCSprite* spr_)
    {
        CCSprite* newSprite = CopySprite(spr_);
        newSprite->setZOrder(0);
        getFrameSprites(m_nowFrame)->addObject(newSprite);
        m_anmFramesFather->addChild(newSprite);
    }
    
    void Animate::addFrame(cocos2d::CCArray *frame_, const cocos2d::CCSize size_, const float& fTime_)
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
        m_animateFramesTime.push_back(fTime_);
        m_boundBox.size = size_;
    }
    
    void Animate::runWithTargetLayer(cocos2d::CCLayer* dest_)
    {
        if(m_animateFrames && m_animateFrames->count() > 0 && dest_)
        {
            m_destDrawLayer = dest_;
            m_nowFrame = 0;
            m_timeCount = 0.0f;
            m_bIsStop = false;
            m_anmFramesFather->removeAllChildren();
            _addNowFrame();
            m_destDrawLayer->addChild(m_anmFramesFather);
        }
    }

    void Animate::setTargetLayer(cocos2d::CCLayer* dest_)
    {
        m_destDrawLayer = dest_;
        _addNowFrame();
        m_destDrawLayer->addChild(m_anmFramesFather);
    }
    
    void Animate::removeFormTargetLayer()
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
    
    void Animate::update(const float &dTime_)
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
    
    const cocos2d::CCRect& Animate::getBoundBox()
    {
         return m_boundBox;
    }

    cocos2d::CCArray* Animate::getFrameSprites(const int& index_)
    {
        if(index_ < 0 || index_ >= m_animateFrames->count())
            return NULL;
       return dynamic_cast<CCArray*> (m_animateFrames->objectAtIndex(index_));
    }
    
    void Animate::deleteFrame(const int& index_)
    {
        m_animateFrames->removeObjectAtIndex(index_);
        m_animateFramesSize.erase(m_animateFramesSize.begin() + index_);
        m_animateFramesTime.erase(m_animateFramesTime.begin() + index_);
        if (m_animateFrames->count() == 0)
        {
            m_animateFrames->release();
            m_animateFrames = NULL;
        }
        
    }

    void Animate::swapFrame(const int& sw1_, const int& sw2_)
    {
        if(m_destDrawLayer)
        {
            CCLayer* dest = m_destDrawLayer;
            removeFormTargetLayer();
            m_destDrawLayer = dest;
        }

        m_animateFrames->exchangeObjectAtIndex(sw1_, sw2_);
        CCSize tempSize = m_animateFramesSize[sw1_];
        m_animateFramesSize[sw1_] = m_animateFramesSize[sw2_];
        m_animateFramesSize[sw2_] = tempSize;
        float tempTime = m_animateFramesTime[sw1_];
        m_animateFramesTime[sw1_] = m_animateFramesTime[sw2_];
        m_animateFramesTime[sw2_] = tempTime;

        if(m_destDrawLayer)
            runWithTargetLayer(m_destDrawLayer);
    }
    
    void Animate::setAnimateCentre(const cocos2d::CCPoint &acPoint_)
    {
        CCPoint offset = ccpSub(acPoint_, m_anmFramesFather->getPosition());
        for (int i = 0; i != m_animateFrames->count(); ++i)
        {
            CCArray* frame = dynamic_cast<CCArray*>(m_animateFrames->objectAtIndex(i));
            if(frame)
            {
                for (int j = 0; j != frame->count(); ++j)
                {
                    CCSprite* pSpr = dynamic_cast<CCSprite*>(frame->objectAtIndex(j));
                    if(pSpr)
                        pSpr->setPosition(ccpSub(pSpr->getPosition(), offset));
                }
            }
        }
    }

    void Animate::setFSpeedPerSecond(const float& spd_)
    {
        m_changeFrameTime = 1.0f/spd_;
    }

    void Animate::play()
    {
        m_bIsStop = false;
    }

    void Animate::stop()
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
            m_changeFrameTime = 1.0f/m_animateFramesTime[m_nowFrame];
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
    
#if  ENABLE_ANIMATE_RW
    void SetAnimateData(Animate*  outAnm_,  TiXmlDocument* inData_)
    {
        TiXmlElement* root = inData_->RootElement();
        
        TiXmlElement* allFrames = root->FirstChildElement("all_frames");
        
        TiXmlNode* frame = allFrames->FirstChild("frame");
        int frameIndex = 0;

        while (NULL != frame)
        {
            CCArray* nowFrame = CCArray::create();
            //nowFrame->retain();
            
            frame->ToElement()->Attribute("index",  &frameIndex);
            
            TiXmlElement* allSprites = frame->FirstChildElement("all_sprites");
            
            TiXmlNode* sprite = allSprites->FirstChild("sprite");
            while(NULL != sprite)
            {
                //SMainLayer->
                CCTexture2D* tex = SMainLayer->getUseSprTex(sprite->ToElement()->Attribute("img_path"));
                
                double rx,ry,rw,rh;
                sprite->ToElement()->Attribute("rect_ox",  &rx);
                sprite->ToElement()->Attribute("rect_oy",  &ry);
                sprite->ToElement()->Attribute("rect_w",  &rw);
                sprite->ToElement()->Attribute("rect_h",  &rh);
                
                CCSprite* realSpr = CCSprite::createWithTexture(tex,  CCRect(rx, ry, rw, rh));
                
                SMainLayer->addCutRect(realSpr->getTexture(), realSpr->getTextureRect());
                
                double px,py,ax,ay,sx,sy,alpha;
                int  isFx,isFy;
                sprite->ToElement()->Attribute("pos_x",  &px);
                sprite->ToElement()->Attribute("pos_y",  &py);
                sprite->ToElement()->Attribute("rot_x",  &ax);
                sprite->ToElement()->Attribute("rot_y",  &ay);
                sprite->ToElement()->Attribute("scale_X",  &sx);
                sprite->ToElement()->Attribute("scale_Y",  &sy);
                sprite->ToElement()->Attribute("alpha",  &alpha);
                sprite->ToElement()->Attribute("is_flipX",  &isFx);
                sprite->ToElement()->Attribute("is_flipY",  &isFy);
                
                realSpr->setPosition(ccp(px,py));
                realSpr->setRotationX(ax);
                realSpr->setRotationY(ay);
                realSpr->setScaleX(sx);
                realSpr->setScaleY(sy);
                realSpr->setOpacity(alpha);
                realSpr->setFlipX(isFx);
                realSpr->setFlipY(isFy);
                
                nowFrame->addObject(realSpr);
                
                sprite = sprite->NextSiblingElement();
            }
            double fpsTime (0);
            frame->ToElement()->Attribute("fps_time",  &fpsTime);
            double sizeW(0), sizeH(0);
            frame->ToElement()->Attribute("bound_box_w", &sizeW);
            frame->ToElement()->Attribute("bound_box_h", &sizeH);
            outAnm_->addFrame(nowFrame, CCSize(sizeW, sizeH), fpsTime);
            SMainLayer->rttFrameToList(nowFrame);
            nowFrame->release();
            
            frame = frame->NextSiblingElement();
        }
        
    }
    
    void GetAnimateData(Animate* inAnm_, TiXmlDocument* outData_)
    {
        TiXmlElement* root = new TiXmlElement("eg_anmaite_data");
        outData_->LinkEndChild(root);
        
        TiXmlElement* allFrames = new TiXmlElement("all_frames");
        root->LinkEndChild(allFrames);
        
        unsigned int frameTotal = inAnm_->m_animateFrames->count();
        
        for (unsigned int i = 0; i < frameTotal; ++i)
        {
            TiXmlElement* frame = new TiXmlElement("frame");
            allFrames->LinkEndChild(frame);

            frame->SetAttribute("index",  i);
            
            TiXmlElement* allSprites = new TiXmlElement("all_sprites");
            frame->LinkEndChild(allSprites);
                        
            CCArray* nowFrame = (CCArray*)inAnm_->m_animateFrames->objectAtIndex(i);
            unsigned int sprTotal = nowFrame->count();
            for (unsigned int j = 0; j < sprTotal; ++j)
            {
                TiXmlElement* sprite = new TiXmlElement("sprite");
                allSprites->LinkEndChild(sprite);
                
                CCSprite* fSpr = (CCSprite*)nowFrame->objectAtIndex(j);
                //SMainLayer->
                sprite->SetAttribute("img_path", SMainLayer->getUseSprPath(fSpr->getTexture()).c_str());//temp temp temp
                sprite->SetAttribute("rect_ox",  fSpr->getTextureRect().origin.x);
                sprite->SetAttribute("rect_oy",  fSpr->getTextureRect().origin.y);
                sprite->SetAttribute("rect_w",  fSpr->getTextureRect().size.width);
                sprite->SetAttribute("rect_h",  fSpr->getTextureRect().size.height);
                
                sprite->SetAttribute("pos_x",  fSpr->getPosition().x);
                sprite->SetAttribute("pos_y",  fSpr->getPosition().y);
                sprite->SetAttribute("rot_x",  fSpr->getRotationX());
                sprite->SetAttribute("rot_y",  fSpr->getRotationY());
                sprite->SetAttribute("scale_X",  fSpr->getScaleX());
                sprite->SetAttribute("scale_Y",  fSpr->getScaleY());
                sprite->SetAttribute("alpha",  fSpr->getOpacity());
                sprite->SetAttribute("is_flipX",  fSpr->isFlipX());
                sprite->SetAttribute("is_flipY",  fSpr->isFlipX());
            }
            frame->SetAttribute("fps_time",  inAnm_->m_animateFramesTime[i]);
            frame->SetAttribute("bound_box_w", inAnm_->m_animateFramesSize[i].width);
            frame->SetAttribute("bound_box_h", inAnm_->m_animateFramesSize[i].height);
        }
    }
#endif
};