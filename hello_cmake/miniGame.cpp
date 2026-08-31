#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <string>
#include <cmath>
#include <random>
struct Player{
    SDL_Surface *mSurface;
    SDL_Texture *mTexture;
    SDL_Surface *OutSurface;
    SDL_Texture *OutTexture;
    int score;
    Uint32 whiteKey;
    float bar;
    float Tx,Ty,Tw,Th;
    float OTx,OTy,OTw,OTh;
    Player(SDL_Renderer* r,std::string n){
        Tw=15,Th=15;
        OTw=50,OTh=50;
        score = 0;
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
    void move(float vx,float vy){
        Tx=Tx+vx;
        Ty=Ty+vy;
    }
    void RingMove(float vx,float vy){
        OTx=OTx+vx;
        OTy=OTy+vy;
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
    float Tx,Ty,Tw,Th;
    Ball(){}
    Ball(SDL_Renderer* r){
        Tw=20,Th=20;
        mSurface=SDL_LoadBMP("./ball.bmp");
        whiteKey = SDL_MapSurfaceRGBA(mSurface, 0xFF, 0xFF, 0xFF,0xFF);
        SDL_SetSurfaceColorKey(mSurface, 1,whiteKey);
        mTexture=SDL_CreateTextureFromSurface(r,mSurface);  
        SDL_DestroySurface(mSurface);
    }
    void render(SDL_Renderer *r){
        SDL_FRect dst{
            .x=Tx,
            .y=Ty,
            .w=Tw,
            .h=Th
        };
        SDL_RenderTexture(r, mTexture, NULL, &dst);
        
    }
    void Move(float x,float y,float speed){
        Tx=Tx+x*speed;
        Ty=Ty+y*speed;
    }
    int begin(){
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> distrib(1,2);
        if(distrib(gen)%2==0)return 1;
        else return -1;
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
    float xBall,yBall,sBall;
    float vxP1,vyP1;
    float vxP2,vyP2;
    unsigned int scoreTime,currentTime,touchTimeOne,touchTimeTwo;
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

        ball->Tx=240;
        ball->Ty=240;

        xBall=0;
        yBall=ball->begin();
        sBall=4;

        player->score=0;
        player2->score=0;
        touchTimeOne=0;
        touchTimeTwo=0;
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
        vxP1=0,vyP1=0;
        vxP2=0,vyP2=0;
        BallInclude();
        Input();
        Update();
        
        Render();
        float x,y;
        SDL_GetMouseState(&x,&y);
        //SDL_Log("x=%f,y=%f",x,y);
    }
    void BallInclude(){
        BallTouchWall();
        BallTouchPlayer2();
        BallTouchPlayer1();
        currentTime=SDL_GetTicks();
        if(currentTime>scoreTime+2000) sBall=5;
        if(Score()){
            scoreTime=SDL_GetTicks();
            Reset();
        }
    }
    bool Score(){
        float BallMidx,BallMidy;
        BallMidx=ball->Tx+10, BallMidy=ball->Ty+10;
        if(BallMidy>486&&BallMidx<327&&BallMidx>171){
            player->score+=1;
            return 1;
        }
        if(BallMidy<14&&BallMidx<327&&BallMidx>171){
            player2->score+=1;
            return 1;
        }
        return 0;
    } 
    void BallTouchWall(){
        if(ball->Tx<37 || ball->Tx>440) xBall=-xBall;
        else if(ball->Ty<4 || ball->Ty>476) yBall=-yBall;
    }
    void BallTouchPlayer2(){
        float BallMidx,BallMidy;
        float P2Midx,P2Midy;
        float DtoP2;
        float V1n,V2n;
        float diffX,diffY;
        float nx,ny;
        float afterTouchV1n,afterTouchV2n;
        float overlap;
        BallMidx=ball->Tx+10, BallMidy=ball->Ty+10;
        P2Midx=player2->OTx+25, P2Midy=player2->OTy+25;
        DtoP2=std::sqrt(((BallMidx-P2Midx)*(BallMidx-P2Midx))+((BallMidy-P2Midy)*(BallMidy-P2Midy)));
        if(DtoP2<35){
            touchTimeTwo=SDL_GetTicks();
            overlap=35-DtoP2;
            diffX=BallMidx-P2Midx;
            diffY=BallMidy-P2Midy;
            nx=diffX/DtoP2;
            ny=diffY/DtoP2;
            V1n=vxP2*nx+vyP2*ny;
            V2n=xBall*nx+yBall*ny;
            if((V1n-V2n)>0){
                player2->Tx-=nx*(overlap/2);
                player2->Ty-=ny*(overlap/2);
                player2->OTx-=nx*(overlap/2);
                player2->OTy-=ny*(overlap/2);
                ball->Tx+=nx*(overlap/2);
                ball->Ty+=ny*(overlap/2);      
                afterTouchV1n=V1n+30.0f*(V2n-V1n);
                afterTouchV2n=-1.5f*V2n+V1n;
                vxP2+=(afterTouchV1n-V1n)*nx;
                vyP2+=(afterTouchV1n-V1n)*ny;
                player2->RingMove(vxP2,vyP2);
                xBall+=(afterTouchV2n-V2n)*nx;
                yBall+=(afterTouchV2n-V2n)*ny;
            }
        }
    }
    void BallTouchPlayer1(){
        float BallMidx,BallMidy;
        float P1Midx,P1Midy;
        float DtoP1,DtoP2;
        float V1n,V2n;
        float diffX,diffY;
        float nx,ny;
        float afterTouchV1n,afterTouchV2n;
        float overlap;
        BallMidx=ball->Tx+10, BallMidy=ball->Ty+10;
        P1Midx=player->OTx+25, P1Midy=player->OTy+25;
        DtoP1=std::sqrt(((BallMidx-P1Midx)*(BallMidx-P1Midx))+((BallMidy-P1Midy)*(BallMidy-P1Midy)));
        if(DtoP1<35){
            touchTimeOne=SDL_GetTicks();
            overlap=35-DtoP1;
            diffX=BallMidx-P1Midx;
            diffY=BallMidy-P1Midy;
            nx=diffX/DtoP1;
            ny=diffY/DtoP1;
            V1n=vxP1*nx+vyP1*ny;
            V2n=xBall*nx+yBall*ny;
            if((V1n-V2n)>0){
                player->Tx-=nx*(overlap/2);
                player->Ty-=ny*(overlap/2);
                player->OTx-=nx*(overlap/2);
                player->OTy-=ny*(overlap/2);
                ball->Tx+=nx*(overlap/2);
                ball->Ty+=ny*(overlap/2);           
                afterTouchV1n=V1n+30.0f*(V2n-V1n);
                afterTouchV2n=-1.5f*V2n+V1n;
                vxP1+=(afterTouchV1n-V1n)*nx;
                vyP1+=(afterTouchV1n-V1n)*ny;
                player->RingMove(vxP1,vyP1);
                xBall+=(afterTouchV2n-V2n)*nx;
                yBall+=(afterTouchV2n-V2n)*ny;
            }
        }
    }
    void Reset(){ 
        xBall=0;
        yBall=ball->begin();
        sBall=0;

        ball->Tx=240;
        ball->Ty=240;
    }
    void playerOneInput(){
        float midx,midy;
        float midOx,midOy;
        float distance;
        float totalP;
        float Px,Py;
        float speed;
        const bool *key_states = SDL_GetKeyboardState(NULL);
        if(currentTime>touchTimeOne+500){
            if(key_states[SDL_SCANCODE_W]&&player->Ty>0)vyP1=-2;
            if(key_states[SDL_SCANCODE_S]&&player->Ty<480)vyP1=2;
            if(key_states[SDL_SCANCODE_D]&&player->Tx<444.5)vxP1=2;
            if(key_states[SDL_SCANCODE_A]&&player->Tx>40.5)vxP1=-2;
            if(key_states[SDL_SCANCODE_LSHIFT]&&player->bar>0){
                speed=2;
                vxP1=speed*vxP1;
                vyP1=speed*vyP1;
                player->bar-=10;
            }
            if(!(key_states[SDL_SCANCODE_LSHIFT])&&(player->bar<1000)){
                player->bar+=10;
            } 
        }
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
    void Update(){
        ball->Move(xBall,yBall,sBall);
        player->move(vxP1,vyP1);
        player2->move(vxP2,vyP2);
    }
    void playerTwoInput(){
        float midx,midy;
        float midOx,midOy;
        float distance;
        float totalP;
        float Px,Py;
        const bool *key_states = SDL_GetKeyboardState(NULL);
        float speed;
        if(currentTime>touchTimeTwo+500){
            if(key_states[SDL_SCANCODE_I]&&player2->Ty>0)vyP2=-2;
            if(key_states[SDL_SCANCODE_K]&&player2->Ty<480)vyP2=2;
            if(key_states[SDL_SCANCODE_L]&&player2->Tx<444.5)vxP2=2;
            if(key_states[SDL_SCANCODE_J]&&player2->Tx>40.5)vxP2=-2;
            if(key_states[SDL_SCANCODE_RSHIFT]&&player2->bar>0){
                speed=2;
                vxP2=speed*vxP2;
                vyP2=speed*vyP2;
                player2->bar-=10;
            }
            if(!(key_states[SDL_SCANCODE_RSHIFT])&&(player2->bar<1000)){
                player2->bar+=10;
            } 
        }
        
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
        playerOneInput();
        playerTwoInput();

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
    }
    void RenderText(){
        SDL_SetRenderDrawColor(mRenderer, 0xFF, 0x00, 0x00, 0xFF);
        SDL_RenderDebugTextFormat(mRenderer,390, 465, "point:%d",player2->score);
        SDL_RenderDebugTextFormat(mRenderer, 65,  25, "point:%d",player->score);
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
        RenderText();
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