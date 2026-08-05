#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

struct Player{
    SDL_Surface *mSurface;
    SDL_Texture *mTexture;
    float score;
    float bar;
    float Tx,Ty,Tw,Th;
    Player(SDL_Renderer* r){
        Tx=20,Ty=20,Tw=25,Th=33;
        bar=1000;
        mSurface=SDL_LoadBMP("./test.bmp");
        if(!(mSurface)){
            SDL_Log("surface創建失敗: %s",SDL_GetError());
        }
        mTexture=SDL_CreateTextureFromSurface(r,mSurface);
        if(!(mTexture)){
            SDL_Log("texture創建失敗: %s",SDL_GetError());
        }
        SDL_DestroySurface(mSurface);
    }
    Player(){}
    ~Player(){
        SDL_DestroyTexture(mTexture);
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
    void move(float x,float y,float speed){
        Tx=Tx+x*speed;
        Ty=Ty+y*speed;
    }
};

struct scene{
    SDL_Texture* mTexture;
    SDL_Surface* mSurface;

    scene(){}
    scene(SDL_Renderer* r){
        mSurface=SDL_LoadBMP("./background.bmp");
        mTexture=SDL_CreateTextureFromSurface(r,mSurface);
        SDL_DestroySurface(mSurface);
    }
    void render(SDL_Renderer *r){
        SDL_FRect dst{
            .x=0,
            .y=0,
            .w=200,
            .h=238
        };
        SDL_RenderTexture(r, mTexture, NULL, &dst);
    }
    ~scene(){
        SDL_DestroyTexture(mTexture);
    }
};
struct SDLminiGame{
    SDL_Window *mWindow;
    SDL_Renderer *mRenderer;
    Player* player;
    Player* player2;
    scene* back;
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
        player = new Player(mRenderer);
        player2= new Player(mRenderer);
        back = new scene(mRenderer);
    }
    ~SDLminiGame(){
        delete back;
        delete player;
        SDL_DestroyRenderer(mRenderer);
        SDL_DestroyWindow(mWindow);
        SDL_Quit();
    }
    void Tick(){
        Input();
        Render();
    }
    void playerOneInput(){
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
        if(key_states[SDL_SCANCODE_D]&&player->Tx<480)player->move(2,0,speed);
        if(key_states[SDL_SCANCODE_A]&&player->Tx>0)player->move(-2,0,speed);
    }
    void playerTwoInput(){

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
        //float x,y;
        //SDL_GetMouseState(&x,&y);
        //SDL_Log("x=%f,y=%f",x,y);
    }
    void RenderText(){
        SDL_SetRenderDrawColor(mRenderer, 0xFF, 0x00, 0x00, 0xFF);
        SDL_RenderDebugTextFormat(mRenderer,390, 465, "energy: %.0f",player->bar/10.0f);
    }
    void RenderBall(){

    }
    void RenderRect(){
        SDL_FRect rect{
            .x=390,
            .y=480,
            .w=player->bar/10,
            .h=10
        };
        SDL_FRect boardary{
            .x=390,
            .y=478,
            .w=102,
            .h=14
        };
        SDL_SetRenderDrawColor(mRenderer, 0x00, 0x00, 0xFF, 0xFF);
        SDL_RenderFillRect(mRenderer, &rect);
        SDL_SetRenderDrawColor(mRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderRect(mRenderer,&boardary);
    }
    void Render(){
        SDL_SetRenderDrawColor(mRenderer, 0x00, 0x00, 0x00, 0xFF);
        
        SDL_RenderClear(mRenderer);

        back->render(mRenderer);
        RenderBall();
        player->render(mRenderer);
        RenderRect();
        RenderText();
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