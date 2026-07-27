#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
struct Player{
    SDL_Surface *mSurface;
    SDL_Texture *mTexture;

    Player(SDL_Renderer* r){
        mSurface=SDL_LoadBMP("./test.bmp");
        mTexture=SDL_CreateTextureFromSurface(r,mSurface);
    }
    Player(){}
    ~Player(){
        SDL_DestroySurface(mSurface);
        SDL_DestroyTexture(mTexture);
    }
    void render(SDL_Renderer *r){
        SDL_FRect dst{
            .x=50,
            .y=50,
            .w=30,
            .h=30
        };
        SDL_RenderTexture(r, mTexture, NULL, &dst);
    }
};
struct SDLminiGame{
    SDL_Window *mWindow;
    SDL_Renderer *mRenderer;
    Player player{mRenderer};

    SDLminiGame(){
        SDL_Init(SDL_INIT_VIDEO);
        mWindow = SDL_CreateWindow("first miniGame", 500, 500, SDL_WINDOW_OPENGL);
        
        if(mWindow==nullptr){
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create window: %s\n", SDL_GetError());
        }
    }
    
    ~SDLminiGame(){
        SDL_DestroyWindow(mWindow);
        SDL_DestroyRenderer(mRenderer);
        SDL_Quit();
    }
    
    void Input(){
        bool running = true;
        const bool *key_states=SDL_GetKeyboardState(NULL);

        while (running) {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_EVENT_QUIT) { 
                    SDL_Log("errror");
                    running = false;
                }
            }
        }
    }
    void Render(){
        SDL_SetRenderDrawColor(mRenderer, 0x00, 0xFF, 0xFF, 0xFF);

        SDL_RenderClear(mRenderer);

        SDL_RenderPresent(mRenderer);

    }

    void MainRun(){
        Input();
        Render();
        player.render(mRenderer);
    }
        
};
int main(int argc,char*argv[]){
    bool SDL_Init(SDL_InitFlags flags);

    SDLminiGame Game; 
    
    Game.MainRun();
    
    return 0;
}