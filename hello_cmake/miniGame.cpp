#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

struct Player{
    SDL_Surface *mSurface;
    SDL_Texture *mTexture;
    float bar;
    float Tx,Ty,Tw,Th;;
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
    scene* back;
    bool running = true;
    bool fullscreen=false;

    SDLminiGame(){
        SDL_Init(SDL_INIT_VIDEO);
        mWindow = SDL_CreateWindow("first miniGame", 500, 500, SDL_WINDOW_OPENGL);
        if(!(mWindow)){
            SDL_Log("window創建失敗: %s",SDL_GetError());
        }
        mRenderer=SDL_CreateRenderer(mWindow,nullptr);
        player = new Player(mRenderer);
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
    void Input(){
        const bool *key_states = SDL_GetKeyboardState(NULL);
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
        float speed = 1;
        if(key_states[SDL_SCANCODE_LSHIFT]&&player->bar>0){
            speed = 2;
            player->bar-=10;
        }
        if(!(key_states[SDL_SCANCODE_LSHIFT])&&(player->bar<1000)){
            player->bar+=10;
        }
        
        if(key_states[SDL_SCANCODE_W])player->move(0,-2,speed);
        if(key_states[SDL_SCANCODE_S])player->move(0,2,speed);
        if(key_states[SDL_SCANCODE_D])player->move(2,0,speed);
        if(key_states[SDL_SCANCODE_A])player->move(-2,0,speed);
    }
    void RenderText(){
        SDL_SetRenderScale(mRenderer, 4, 4);
        SDL_SetRenderDrawColor(mRenderer, 0xFF, 0x00, 0x00, 0xFF);
        SDL_RenderDebugTextFormat(mRenderer,20, 20, "energy: %.0f",player->bar/10.0f);
        SDL_SetRenderScale(mRenderer,1,1);
    }
    void Render(){
        SDL_SetRenderDrawColor(mRenderer, 0x00, 0x00, 0x00, 0xFF);
        
        SDL_RenderClear(mRenderer);

        back->render(mRenderer); 

        player->render(mRenderer);

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