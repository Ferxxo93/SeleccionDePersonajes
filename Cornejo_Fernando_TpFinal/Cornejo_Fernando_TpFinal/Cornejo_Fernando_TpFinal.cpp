#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <SDL_image.h>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>

const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;

struct Character {
    std::string name;
};//se crea la lista con un struct llamado character de tipo string de nombres

std::vector<Character> loadCharacters(const std::string& filename)//creamos este vector que carga los personajes desde un archivo 
    {
    std::vector<Character> characters;
    std::ifstream file(filename);
    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty()) {
            characters.push_back({ line });
        }
    }//el while es para leer cada linea que tenga dentro del archivo
    return characters;//una vez que lee todo lo que hay dentro del archivo y se hayan agregado, devuelve este vector completo
}

SDL_Texture* loadTexture(const std::string& path, SDL_Renderer* renderer) {
    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface) {
        std::cerr << "Failed to load image: " << IMG_GetError() << std::endl;
        return nullptr;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}//creamos una textura para que pueda cargar imagenes y lo pueda renderizar

void renderText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, int x, int y) {
    SDL_Color color = { 0, 0, 0};
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dstrect = { x, y, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, NULL, &dstrect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}//creamos una textura para las fuentes de los textos los colores y donde se van a ubicar

int main(int argc, char* args[]) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    TTF_Init();
    IMG_Init(IMG_INIT_PNG);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    //inicializamos todas las herramientas de SDL que vamos a usar
    SDL_Window* window = SDL_CreateWindow("Character Management", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    TTF_Font* font = TTF_OpenFont("DejaVuSansMono.ttf", 35);
    Mix_Chunk* moveSound = Mix_LoadWAV("paddleHit.wav");
    Mix_Chunk* removeSound = Mix_LoadWAV("wallHit.wav");
    SDL_Texture* triangleTexture = loadTexture("triangle.png", renderer);
    SDL_Texture* backgroundTexture = loadTexture("LogoUade.png", renderer);
    //creamos la pantalla, la renderizamos y las texturas que creamos antes, ahora lo que vamos hacer es darle forma, poniendole el sonido, la fuente y el tamano de las letras
    std::vector<Character> characters = loadCharacters("characters.txt");
    int selectedCharacter = 0;//inicializamos el vector characters que va a leer todo lo que tenga el archivo txt nombrado characters que va a estar dentro de la carpeta

    bool quit = false;
    SDL_Event e;

    while (!quit) //le damos accion a los botones del teclado
        {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                case SDLK_UP:
                    if (selectedCharacter > 0) 
                        {
                        selectedCharacter--;
                        Mix_PlayChannel(-1, moveSound, 0);
                        }
                    break;
                case SDLK_DOWN:
                    if (selectedCharacter < characters.size() - 1) 
                        {
                        selectedCharacter++;
                        Mix_PlayChannel(-1, moveSound, 0);
                        }
                    break;
                case SDLK_SPACE:
                    if (!characters.empty()) {
                        characters.erase(characters.begin() + selectedCharacter);
                        Mix_PlayChannel(-1, removeSound, 0);
                        if (selectedCharacter >= characters.size() && selectedCharacter > 0) {
                            selectedCharacter--;
                        }
                    }//si apretamos la espaciadora lo que va hacer es  ruido creado previamente y remover al nombre seleccionado y mover el de abajo a la posicion que estaba el que se removio
                    break;
                case SDLK_ESCAPE:
                    quit = true;
                    break;
                }
            }
        }//creamos un evento en un while para poder asignarle acciones a las teclas, en este caso si apretamos esc vamos a salir del juego

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);//renderizamos para que aparezca la imagen de fondo

        int y = 100;
        for (int i = 0; i < characters.size(); ++i)//le damos movimiento al triangulo del menu 
            {
            renderText(renderer, font, characters[i].name, 100, y);
            if (i == selectedCharacter)
            {
                SDL_Rect dstrect = { 60, y, 20, 20 }; //ajustamos la posicion del triangulo
                SDL_RenderCopy(renderer, triangleTexture, NULL, &dstrect);//renderizamos el triangulo para que aparezca al costado y pueda moverse arriba y abajo
            }
            y += 50; // Ajusta el espaciado entre líneas según sea necesario para el tamaño de fuente más grande
        }

        renderText(renderer, font, "UP/DOWN: Select | SPACE: Remove | ESC: Quit", 50, SCREEN_HEIGHT - 50);//renderizamos el texto de los botones abajo de la pantalla

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(backgroundTexture);
    SDL_DestroyTexture(triangleTexture);
    TTF_CloseFont(font);
    Mix_FreeChunk(moveSound);
    Mix_FreeChunk(removeSound);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    Mix_CloseAudio();
    IMG_Quit();
    SDL_Quit();

    return 0;
}//liberamos todo lo que utilizamos
