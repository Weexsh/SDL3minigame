#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <string>
#include <cmath>
struct Player{
    SDL_Surface *mSurface;
    SDL_Texture *mTexture;
    SDL_Surface *OutSurface;
    SDL_Texture *OutTexture;
    Uint32 whiteKey;
    float score;
    float bar;
    float Tx,Ty,Tw,Th;
    float OTx,OTy,OTw,OTh;
    Player(SDL_Renderer* r,std::string n){
        Tx=20,Ty=20,Tw=15,Th=15;
        OTx=0,OTy=0,OTw=50,OTh=50;
        bar=1000;
        mSurface=SDL_LoadBMP(n.c_str());
        whiteKey = SDL_MapSurfaceRGBA(mSurface, 0xFF, 0xFF, 0xFF,0xFF);
        SDL_SetSurfaceColorKey(mSurface, 1,whiteKey);
        mTexture=SDL_CreateTextureFromSurface(r,mSurface);
        SDL_DestroySurface(mSurface);

        OutSurface=SDL_LoadBMP("./Outframe.bmp");
        whiteKey = SDL_MapSurfaceRGBA(OutSurface, 0xFF, 0xFF, 0xFF,0xFF);
        SDL_SetSurfaceColorKey(OutSurface, 1,whiteKey);
        OutTexture=SDL_CreateTextureFromSurface(r,OutSurface);
        SDL_DestroySurface(OutSurface);
    }
    Player(){}
    ~Player(){
        SDL_DestroyTexture(mTexture);
        SDL_DestroyTexture(OutTexture);
    }
    void render(SDL_Renderer *r){
        SDL_FRect dst{
            .x=Tx,
            .y=Ty,
            .w=Tw,
            .h=Th
        };
        SDL_FRect Outdst{
            .x=OTx,
            .y=OTy,
            .w=OTw,
            .h=OTh
        };
        SDL_RenderTexture(r, OutTexture, NULL, &Outdst);
        SDL_RenderTexture(r, mTexture, NULL, &dst);   
    }
    void move(float x,float y,float speed){
        Tx=Tx+x*speed;
        Ty=Ty+y*speed;
    }
};

struct scene{
    SDL_Texture* mTexture;
    SDL_Surface* mSurface;

    scene(){}
    scene(SDL_Renderer* r,std::string name){
        mSurface=SDL_LoadBMP(name.c_str());
        mTexture=SDL_CreateTextureFromSurface(r,mSurface);
        SDL_DestroySurface(mSurface);
    }
    void render(SDL_Renderer *r){
        SDL_FRect dst{
            .x=35,
            .y=0,
            .w=430,
            .h=500
        };
        SDL_RenderTexture(r, mTexture, NULL, &dst);
    }
    ~scene(){
        SDL_DestroyTexture(mTexture);
    }
};
struct Ball{
    SDL_Texture* mTexture;
    SDL_Surface* mSurface;
    Uint32 whiteKey;
    Ball(){}
    Ball(SDL_Renderer* r){
        mSurface=SDL_LoadBMP("./ball.bmp");
        whiteKey = SDL_MapSurfaceRGBA(mSurface, 0xFF, 0xFF, 0xFF,0xFF);
        SDL_SetSurfaceColorKey(mSurface, 1,whiteKey);
        mTexture=SDL_CreateTextureFromSurface(r,mSurface);  
        SDL_DestroySurface(mSurface);
    }
    void render(SDL_Renderer *r){
        SDL_FRect dst{
            .x=240,
            .y=240,
            .w=20,
            .h=20
        };
        SDL_RenderTexture(r, mTexture, NULL, &dst);
    }
    ~Ball(){
        SDL_DestroyTexture(mTexture);
    }
};
struct SDLminiGame{
    SDL_Window *mWindow;
    SDL_Renderer *mRenderer;
    Player* player;
    Player* player2;
    scene* back;
    Ball* ball;
    bool running = true;
    bool fullscreen=false;
    int windowX,windowY;
    SDLminiGame(){
        SDL_Init(SDL_INIT_VIDEO);
        windowX=500;
        windowY=500;
        mWindow = SDL_CreateWindow("first miniGame", windowX,windowY, SDL_WINDOW_OPENGL);
        if(!(mWindow)){
            SDL_Log("window創建失敗: %s",SDL_GetError());
        }
        mRenderer=SDL_CreateRenderer(mWindow,nullptr);
        SDL_SetWindowResizable(mWindow,true); 
        SDL_SetRenderLogicalPresentation(mRenderer, 500, 500,SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);
        player = new Player(mRenderer,"./test.bmp");
        player2= new Player(mRenderer,"./test1.bmp");
        back = new scene(mRenderer,"./background.bmp");
        ball = new Ball(mRenderer);
        player->Tx=240;
        player->Ty=400;
        player->OTx=222.5;
        player->OTy=382.5;

        player2->Tx=240;
        player2->Ty=100;
        player2->OTx=222.5;
        player2->OTy=82.5;
    }
    ~SDLminiGame(){
        delete back;
        delete player;
        delete player2;
        delete ball;
        SDL_DestroyRenderer(mRenderer);
        SDL_DestroyWindow(mWindow);
        SDL_Quit();
    }
    void Tick(){
        Input();
        Render();
    }
    void playerOneInput(){
        float midx,midy;
        float Xhitbox,Yhitbox;
        float midOx,midOy;
        float distance;
        float totalP;
        float Px,Py;
        const bool *key_states = SDL_GetKeyboardState(NULL);
        float speed = 1;
        if(key_states[SDL_SCANCODE_LSHIFT]&&player->bar>0){
            speed += 1;
            player->bar-=10;
        }
        if(!(key_states[SDL_SCANCODE_LSHIFT])&&(player->bar<1000)){
            player->bar+=10;
        }  

        if(key_states[SDL_SCANCODE_W]&&player->Ty>0)player->move(0,-2,speed);
        if(key_states[SDL_SCANCODE_S]&&player->Ty<480)player->move(0,2,speed);
        if(key_states[SDL_SCANCODE_D]&&player->Tx<444.5)player->move(2,0,speed);
        if(key_states[SDL_SCANCODE_A]&&player->Tx>40.5)player->move(-2,0,speed);
        
        midx=player->Tx+7.5;
        midy=player->Ty+7.5;
        midOx=player->OTx+25;
        midOy=player->OTy+25;
        distance=std::sqrt((midx-midOx)*(midx-midOx)+(midy-midOy)*(midy-midOy));
        if(distance>17.5){
            totalP=distance-17.5;
            Px=midx-midOx;
            Py=midy-midOy;
            player->OTx+=totalP*(Px/distance);
            player->OTy+=totalP*(Py/distance);
        }
        
    }
    void playerTwoInput(){
        float midx,midy;
        float Xhitbox,Yhitbox;
        float midOx,midOy;
        float distance;
        float totalP;
        float Px,Py;
        const bool *key_states = SDL_GetKeyboardState(NULL);
        float speed = 1;
        if(key_states[SDL_SCANCODE_RSHIFT]&&player2->bar>0){
            speed += 1;
            player2->bar-=10;
        }
        if(!(key_states[SDL_SCANCODE_RSHIFT])&&(player2->bar<1000)){
            player2->bar+=10;
        }  
        if(key_states[SDL_SCANCODE_I]&&player2->Ty>0)player2->move(0,-2,speed);
        if(key_states[SDL_SCANCODE_K]&&player2->Ty<480)player2->move(0,2,speed);
        if(key_states[SDL_SCANCODE_L]&&player2->Tx<444.5)player2->move(2,0,speed);
        if(key_states[SDL_SCANCODE_J]&&player2->Tx>40.5)player2->move(-2,0,speed);
        midx=player2->Tx+7.5;
        midy=player2->Ty+7.5;
        midOx=player2->OTx+25;
        midOy=player2->OTy+25;
        distance=std::sqrt((midx-midOx)*(midx-midOx)+(midy-midOy)*(midy-midOy));
        if(distance>17.5){
            totalP=distance-17.5;
            Px=midx-midOx;
            Py=midy-midOy;
            player2->OTx+=totalP*(Px/distance);
            player2->OTy+=totalP*(Py/distance);
        }
    }
    void Input(){
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) { 
                running = false;
            }else if(event.type==SDL_EVENT_KEY_DOWN){
                if(event.key.key==SDLK_F11){
                    fullscreen=!fullscreen;
                    SDL_SetWindowFullscreen(mWindow,fullscreen);
                }
            }
        }
        playerOneInput();
        playerTwoInput();
    }
    void RenderText(){
        SDL_SetRenderDrawColor(mRenderer, 0xFF, 0x00, 0x00, 0xFF);
        SDL_RenderDebugTextFormat(mRenderer,390, 465, "energy: %.0f",player->bar/10.0f);
    }
    void RenderRect(){
        SDL_FRect PlayerOneRect{
            .x=480,
            .y=480,
            .w=10,
            .h=-player->bar/10
        };
        SDL_FRect PlayerOneBoardary{
            .x=478,
            .y=379,
            .w=14,
            .h=102
        };
        SDL_FRect PlayerTwoRect{
            .x=10,
            .y=120,
            .w=10,
            .h=-player2->bar/10
        };
        SDL_FRect PlayerTwoBoardary{
            .x=8,
            .y=19,
            .w=14,
            .h=102
        };
        SDL_SetRenderDrawColor(mRenderer, 0x00, 0x00, 0xFF, 0xFF);
        SDL_RenderFillRect(mRenderer, &PlayerOneRect);
        SDL_RenderFillRect(mRenderer, &PlayerTwoRect);
        SDL_SetRenderDrawColor(mRenderer, 0x50, 0x50, 0x50, 0xFF);
        SDL_RenderRect(mRenderer,&PlayerOneBoardary);
        SDL_RenderRect(mRenderer,&PlayerTwoBoardary);
    }
    void Render(){
        SDL_SetRenderDrawColor(mRenderer, 0x00, 0x48, 0x00, 0xFF);
        
        SDL_RenderClear(mRenderer);

        back->render(mRenderer);
        ball->render(mRenderer);
        player->render(mRenderer);
        player2->render(mRenderer);
        RenderRect();
        SDL_RenderPresent(mRenderer);
    }

    void MainRun(){
        while(running){
            SDL_Delay(16);
            Tick();
        }
    }
};
int main(int argc,char*argv[]){
    SDLminiGame Game; 
    Game.MainRun();
    return 0;
}