#define SDL_MAIN_HANDLED

#include <chrono>
#include <thread>
#include <string>
#include <vector>
#include <algorithm>
#include <thread>
#include <ctime>
#include <random>

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

#include"All_Function.h"

Camera* camera = nullptr;
Scene_Manager SceneM;
Game_elements::Board_Manager board_manager;
data::Game_Data game_data;

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;

bool is_gaming = true;

Game_setting setting;
Game_player Player_sets;
Vector2 mouse_pos;
bool mouse_key_down = false;

SDL_Texture* tex_background = nullptr;
SDL_Texture* button_up1 = nullptr;
SDL_Texture* button_down1 = nullptr;
SDL_Texture* button_up2 = nullptr;
SDL_Texture* button_down2 = nullptr;
SDL_Texture* tile_check = nullptr;
SDL_Texture* cloud_background = nullptr;
SDL_Texture* cloud = nullptr;

SDL_Texture* Slime = nullptr;
SDL_Texture* PSlime = nullptr;

Atlas board_tex;
Atlas Atlas_explosion;
Atlas Atlas_Slime;
Atlas Atlas_PSlime;
Atlas Atlas_null;
Atlas Atlas_background;
Atlas Atlas_game_bg;
Atlas select_box;

Mix_Music* music_bgm = nullptr;
Mix_Chunk* sound_explosion = nullptr;
Mix_Chunk* put_slime = nullptr;
Mix_Chunk* click_button = nullptr;

TTF_Font* font = nullptr;
TTF_Font* font_s = nullptr;
TTF_Font* font_ss = nullptr;
TTF_Font* font_b = nullptr;


void load_resources();							// 加载游戏资源
void unload_resources();						// 卸载游戏资源
void init();									// 游戏程序初始化
void deinit();									// 游戏程序反初始化
void on_update(double delta);					// 逻辑更新
void on_render(const Camera& camera);			// 画面渲染
void mainloop();								// 游戏主循环

void show_text(std::string context,TTF_Font* now_font,
	SDL_Color color = { 255, 255, 255, 180 })//水印
{
	SDL_Surface* text = TTF_RenderUTF8_Blended(now_font, context.c_str(), color);
	SDL_Texture* tex_text = SDL_CreateTextureFromSurface(renderer, text);
	SDL_Rect text_rect = { 0, setting.resolution.resolution.y - (text->h),text->w ,text->h };
	SDL_RenderCopy(renderer, tex_text, nullptr, &text_rect);

	SDL_DestroyTexture(tex_text); SDL_FreeSurface(text);
}

namespace Game_time {

	std::string getCurrentTimeAsString() {
		// 获取当前时间
		std::time_t t = std::time(nullptr);
		// 使用localtime_s获取本地时间
		struct tm timeinfo;
		localtime_s(&timeinfo, &t); // 使用 localtime_s 替代 localtime
		// 使用strftime获取格式化的时间字符串
		char time_str[100];
		std::strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", &timeinfo);
		std::string utf8_str = u8"棋盘";
		return std::string(time_str) + " " + utf8_str;
	}

	double generateRandomDouble() {
		// 创建随机数生成器
		std::random_device rd;  // 用于获取随机数种子
		std::mt19937 gen(rd()); // 根据种子初始化随机数生成器

		// 创建分布，范围是 [0, 1)
		std::uniform_real_distribution<> dis(0.0, 1.0);

		// 返回一个 0 到 1 的随机数
		return dis(gen);
	}
}

void load_resources()
{
	tex_background = IMG_LoadTexture(renderer, "resources/background_title.png");
	button_up1 = IMG_LoadTexture(renderer, "resources/button_up_1.png");
	button_down1 = IMG_LoadTexture(renderer, "resources/button_down_.png");
	button_up2 = IMG_LoadTexture(renderer, "resources/button_up_2.png");
	button_down2 = IMG_LoadTexture(renderer, "resources/button_down_2.png");
	tile_check = IMG_LoadTexture(renderer, "resources/Slime_1.png");
	cloud_background = IMG_LoadTexture(renderer, "resources/Clouds 7/1.png");
	cloud = IMG_LoadTexture(renderer, "resources/Clouds 7/4.png");
	Slime = IMG_LoadTexture(renderer, u8"resources/Slime_1@3x.png");
	PSlime = IMG_LoadTexture(renderer, u8"resources/PSlime_1@3x.png");


	board_tex.load(renderer, "resources/board_%d.png", 1);
	Atlas_explosion.load(renderer, "resources/explosion_%d.png", 5);
	Atlas_Slime.load(renderer, "resources/Slime_%d.png", 7);
	Atlas_PSlime.load(renderer, "resources/PSlime_%d.png", 7);
	Atlas_null.load(renderer, "resources/null_%d.png", 1);
	Atlas_background.load(renderer, u8"resources/background/图层 %d@0.5x.png", 120);///太大了 要删改
	Atlas_game_bg.load(renderer, u8"resources/game_background/图层 %d@0.5x.png", 32);
	select_box.load(renderer, u8"resources/box%d.png", 1);

	music_bgm = Mix_LoadMUS(u8"resources/流派未確定 - 幻想世界.wav");
	sound_explosion = Mix_LoadWAV("resources/explosion.wav");
	put_slime = Mix_LoadWAV("resources/put_slime.mp3");
	click_button = Mix_LoadWAV("resources/click_button.wav");

	font = TTF_OpenFont("resources/IPix.ttf", 56);
	font_s = TTF_OpenFont("resources/IPix.ttf", 38);
	font_ss = TTF_OpenFont("resources/IPix.ttf", 20);
	font_b = TTF_OpenFont("resources/IPix.ttf", 120);
}

void on_updata(double delta)//程序的心脏
{
	SceneM.update(delta);
	camera->on_update(delta);
}

void on_render(const Camera& camera)
{
	SceneM.render(camera);
}

void init()//完成资源加载等内容
{
	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
	Mix_Init(MIX_INIT_MP3);
	TTF_Init();

	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
	Mix_AllocateChannels(32);

	window = SDL_CreateWindow(u8"Card Duel Gomoku",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		setting.resolution.resolution.x, setting.resolution.resolution.y, SDL_WINDOW_SHOWN);//后面设置分辨率要用

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	//SDL_ShowCursor(SDL_DISABLE);
	camera = new Camera(renderer);

	load_resources();
	SceneM.setScene(0);
	SceneM.init();

	game_data.init();

	//board_manager.init(data::saves.child(u8"Card Duel Gomoku"));
	board_manager.init();
	
	Mix_VolumeMusic(32);
	Mix_PlayMusic(music_bgm, -1);
}


void mainloop()//主循环（游戏每帧内的逻辑）
{
	using namespace std::chrono;

	SDL_Event event;

	const nanoseconds frame_duration(1000000000 / setting.fps);
	steady_clock::time_point last_tick = steady_clock::now();


	while (is_gaming)
	{
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				is_gaming = false;
				break;
			case SDL_MOUSEMOTION:
			{
				mouse_pos.x = (double)event.motion.x;
				mouse_pos.y = (double)event.motion.y;
			}
			break;
			case SDL_MOUSEBUTTONDOWN:
				mouse_key_down = true;
				break;
			case SDL_MOUSEBUTTONUP:
				mouse_key_down = false;
				break;
			}

			//std::cout << event.type << std::endl;
			//std::cout << event.wheel.y << std::endl;

			SceneM.handleEvent(event);
			//std::cout << Player_sets.get_now_winner() << std::endl;
			//Player_sets.set_now_winner();
		}

		steady_clock::time_point frame_start = steady_clock::now();
		duration<double> delta = duration<double>(frame_start - last_tick);

		on_updata(delta.count());

		on_render(*camera);

		show_text(u8"程序设计课作业 刘承浩", font_ss);

		//show_text(u8"早期版本0.3v", font_ss);

		SDL_RenderPresent(renderer);

		last_tick = frame_start;
		nanoseconds sleep_duration = frame_duration - (steady_clock::now() - frame_start);
		if (sleep_duration > nanoseconds(0))
			std::this_thread::sleep_for(sleep_duration);

	}
}

void unload_resources()
{
	SDL_DestroyTexture(tex_background);
	SDL_DestroyTexture(button_up1);
	SDL_DestroyTexture(button_down1);

	Mix_FreeMusic(music_bgm);
	Mix_FreeChunk(sound_explosion);
}


void deinit()//退出游戏后资源卸载逻辑
{
	delete camera;

	unload_resources();

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	TTF_Quit();
	Mix_Quit();
	IMG_Quit();
	SDL_Quit();
}

int main()
{
	init();
	mainloop();
	deinit();

	//system("pause");
	return 0;
}