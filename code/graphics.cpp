#include "pch.hpp"
#include "core.hpp"
#include "assets.hpp"
#include "engine.hpp"
#include "graphics.hpp"

#include <sstream>

namespace Graphics
{
    

    Tileset::Tileset(const std::string& name, int w, int h)
        :Asset(name), w(w), h(h),
        pixels(new Color[(int)(w * h)]),
        texture(Engine::CreateTexture(w, h))
    {
        memset(pixels, 255, w * h * sizeof(Color));
        update();
    }
    Tileset::~Tileset()
    {
        delete[]pixels;
        Engine::DestroyTexture(texture);
    }

    void Tileset::update(const Rect & rect )
    {
        ASSERT(rect.x >= 0 && rect.w+rect.x <= w 
            && rect.y >= 0 && rect.h+rect.y <= h,
            "Engine: Invalid rect" );

        if( rect.w == 0 || rect.h == 0 )
        {
            Color * data = Engine::LockTexture( texture, { 0,0,w,h } );
            memcpy(data, pixels, w * h * sizeof(Color));
        }
        else
        {
            Color * data = Engine::LockTexture( texture, rect );
            for(int y = 0; y < rect.h; y++)
                for(int x = 0; x < rect.w; x++)
                    data[y * w  + x] = pixels[(rect.y+y) * w  + rect.x + x];
        }   

        Engine::UnlockTexture(texture);

    }
    
    ///////////////////////////////////////////////////////////////////////////////////////////////

    Sprite::Sprite(const std::string & name, int w, int h)
        :Asset(name), w(w), h(h)
    {
        //default animation is nothing
        animframes[DEFAULT_ANIMATION] = {
            { -1.0f, {0,0,w,h} }
        };
    }
     void Sprite::reload()
    {
        m_tilesetcache = Assets::Load<Tileset>(sheet);
    }

    void Sprite::draw(const Rect & src, const Rect & dest)
    {
        Engine::DrawTexture(m_tilesetcache->texture, src, dest);
    }
  
    /*--------------------------- SpriteInstance ------------------------------------
        Sprites are instanced. The sprite description 
    */
    SpriteInstance::SpriteInstance(const std::string & sprite)
        :sprite(sprite),
        m_spritecache(0),
        m_iframe(0), 
        m_timer(0), 
        animation(DEFAULT_ANIMATION)
    {
        m_spritecache = Assets::Load<Sprite>(sprite);
        rect = { 0, 0, m_spritecache->w, m_spritecache->h};
    }
     
    void SpriteInstance::draw()
    {
        if(!m_spritecache) return;
        m_timer += Engine::GetTimeDeltaMs();
        const std::vector<Frame>& frames = m_spritecache->animframes[animation];
        
        if (frames[m_iframe].duration != -1 && m_timer > frames[m_iframe].duration)
        {
            m_timer = 0;
            m_iframe++;
            m_iframe %= frames.size();
        }
        m_spritecache->draw(frames[m_iframe].region, rect);
    }

    ///////////////////////////////////////////////////////////////////////////////

    Font::Font(const std::string& name, int w, int h, int charW, int charH, char start)
         :Tileset(name, w, h), 
         charW(charW), charH(charH), 
         start(start)
    { 
    }    

    void Font::blit(int destTexture, const std::string & text, const Rect & dest)
    {

        int c, sx,sy;
        const int srcW = w / charW;  
        const int destW = dest.w / charW;     
        const int destH = dest.h / charH;     
        int scrolly = 0;
        int dx = 0, dy = 0;
        for(int i = 0; i < text.size(); i++)
        {
            int s =  text[i];
            if(s == '\n')
            {
                i++;
                if(i== text.size()) break;
                scrolly++;
                s =  text[i];
                dx = 0;
                dy++;
            }
            if(dx == destW)
            {
                dx = 0;
                dy++;
            }
            c =  s - start;
            sx = (c % srcW);
            sy = (c / srcW) ;            
            Rect src = { 
                sx * charW, sy * charH, 
                charW, charH 
            } ;
            Rect pos = { 
                dest.x + dx * charW, 
                dest.y + dy * charH, 
                charW, charH 
            };   
            if(dy > destH) 
                break;
            Engine::Blit(texture, destTexture, src, pos);
            dx++;
        }
    }


    ///////////////////////////////////////////////////////////////////////////////
    
    TextBox::TextBox(int tw, int th, const std::string & text)
        :text(text),
        font(DEFAULT_FONT),
        m_texture (0),
        x(0), y(0), 
        w(0), h(0),
        tw(tw), th(th),
        scrolly(0),filled(false),
        textColor({255,255,255,255}),
        fillColor({0,0,0,0}),
        visible(true),
        borderX(0),
        borderY(0)
    {
    }
    TextBox::~TextBox()
    {
        Engine::DestroyTexture(m_texture);
    }

    void TextBox::draw()
    {
        if (!visible) return;
        if(!m_fontcache) return;   
        Engine::DrawTexture(m_texture, {0,0,w,h}, {x,y,w,h});
    }

    void TextBox::refresh()
    {
        Engine::ClearTexture(m_texture, fillColor);
        m_fontcache->blit(m_texture, text, { borderX, borderY , w, h });
        for(int y =0 ; y < m_fontcache->h; y++ )
            for(int x =0 ; x < m_fontcache->w; x++ )
            {
                //color only text 
                Color & color = m_fontcache->pixels[y * m_fontcache->w + x];
                if(color == fillColor)
                    color = textColor;
            }

    }
    void TextBox::reload()
    {
        m_fontcache = Assets::Load<Font>(font);
        if(!m_fontcache) return;
        //only do if texture w/h changes
        if(m_texture) Engine::DestroyTexture(m_texture);
        w = tw * m_fontcache->charW + borderX*2;
        h = th * m_fontcache->charH + borderY*2;
        m_texture = Engine::CreateTexture(w, h, true);
        if(!filled)
            Engine::SetTextureBlendMode(m_texture, BLEND_ADD);
        refresh();
    }
} // namespace Graphics