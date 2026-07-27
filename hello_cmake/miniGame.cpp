#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

struct Player{
    SDL_Surface *mSurface;
    SDL_Texture *mTexture;

    Player(SDL_Renderer* r){
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
    Player(){
        mTexture=nullptr;
    }
    ~Player(){
        SDL_DestroyTexture(mTexture);
    }
    void render(SDL_Renderer *r){
        SDL_FRect dst{
            .x=50,
            .y=50,
            .w=33,
            .h=25
        };
        SDL_RenderTexture(r, mTexture, NULL, &dst);
    }
};
struct SDLminiGame{
    SDL_Window *mWindow;
    SDL_Renderer *mRenderer;
    Player* player;
    bool running = true;
    
    SDLminiGame(){
        
        SDL_Init(SDL_INIT_VIDEO);
        mWindow = SDL_CreateWindow("first miniGame", 500, 500, SDL_WINDOW_OPENGL);
        if(!(mWindow)){
            SDL_Log("window創建失敗: %s",SDL_GetError());
        }
        mRenderer=SDL_CreateRenderer(mWindow,nullptr);
         player = new Player(mRenderer);
    }
    
    ~SDLminiGame(){
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
        
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) { 
                running = false;
            }
        }
    }
    void Render(){
        
        SDL_RenderClear(mRenderer);

        SDL_SetRenderDrawColor(mRenderer, 0x00, 0x00, 0x00, 0xFF);

        player->render(mRenderer);

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