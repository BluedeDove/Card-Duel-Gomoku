#pragma once
#include<vector>
#include <ctime>
#include <cstring>
#include<iostream>
#include<cmath>
#include <random>
#include <functional>

#include "camera.cpp"
#include "vector2.cpp"
#include "animation.cpp"
#include "data.h"
#include "pugixml.hpp"

#include <SDL_mixer.h>//史莱姆退场音效

//extern void data::save_game_file();

extern Camera* camera;
extern Game_setting setting;
extern Game_player Player_sets;
extern Atlas Atlas_explosion;
extern Atlas Atlas_Slime;
extern Atlas Atlas_PSlime;
extern Atlas Atlas_null;
extern Atlas board_tex;
extern Atlas select_box;

extern data::Game_Data game_data;

extern Mix_Chunk* sound_explosion;
extern Mix_Chunk* put_slime;

extern TTF_Font* font;
extern TTF_Font* font_s;
extern TTF_Font* font_b;

namespace Game_elements
{


	class Slime
	{
	public:
		Slime() = default;
		~Slime()
		{
			delete now_slime_ani;
		}
		
		void slime_init(Atlas* now_atlas, int player_ID, int tile_ID, const Vector2& position)
		{
			now_pos = position;
			player_number = player_ID;
			delete now_slime_ani;
			now_slime_ani = nullptr;
			now_slime_ani = new Animation;

			now_slime_ani->add_frame(now_atlas);
			now_slime_ani->set_interval(interval_time);
			now_slime_ani->set_position(now_pos);

			explosion.set_position(now_pos);

			explorsion_waiting_time.set_on_timeout([&]() {if_visible = true; Mix_PlayChannel(-1, put_slime, 0); });
			explorsion_waiting_time.set_one_shot(true);
			explorsion_waiting_time.set_aim_time(3);

			now_slime_ani->set_on_finished([&]() {this->paused = true;
			this->paused_time.set_aim_time(25 * Game_time::generateRandomDouble()); });

			explosion.add_frame(&Atlas_explosion);
			explosion.add_frame(&Atlas_null);
			explosion.set_interval(0.1);
			explosion.set_loop(false);

			tile_number = tile_ID;

			initing = false;
		}

		void handleEvent(const SDL_Event& event)
		{
		}

		void on_updata(double delta)
		{
			/*Vector2 shift = { 0,-20 * sin((now_slime_ani.get_time() / (interval_time*7)) * (3.1415926/2)) };

			Vector2 real_pos = now_pos + shift;*/
			if (!initing) {
				if (if_visible) {
					if (!paused) {
						now_slime_ani->set_position(now_pos);

						now_slime_ani->on_update(delta);
					}
					else
					{
						paused_time.on_update(delta);
					}
					//std::cout << "更新史莱姆" << std::endl;
				}
				else
				{
					explosion.on_update(delta);
					explorsion_waiting_time.on_update(delta);
				}
			}
		}

		void render(const Camera& camera)
		{
			if (!initing) {
				if (if_visible) {
					now_slime_ani->on_render(camera);
					//std::cout << "渲染史莱姆" << std::endl;
				}
				else
				{
					explosion.on_render(camera);
				}
			}
		}


	public:
		bool if_visible = true;
		bool initing = true;

	private:
		Vector2 now_pos;
		double interval_time = 0.1;
		int player_number = 0;////////////////////////////////////////////
		int tile_number;
		bool vaild_tile = false;///////////////////////////////////
		Animation* now_slime_ani = nullptr;
		Animation explosion;
		bool paused = false;
		TIMER paused_time;
		TIMER explorsion_waiting_time;
	};

	class Tile//棋盘抽象类
	{
	public:
		/*virtual Tile();
		virtual ~Tile();
		virtual Tile(int number, pugi::xml_node this_node);*/

		~Tile()
		{
			delete slime;
		}

		void init(int number, int playernumber, int tiletype, Vector2 pos, int this_board_num,
			std::function<void(int check_tile_id, int player_id)> board_test,
			std::function<void(int tile_id, int player_number)> change_tile,
			std::function<void()> save_time)
		{
			board_test_fun = board_test;
			change_tile_player = change_tile;
			save_time_fun = save_time;

			this->num = number;
			this->player_number = playernumber;
			this->tile_type = tiletype;
			now_board_num = this_board_num;

			this->pos_real = pos;

			set_slime();
			s_init();

			switch_player.set_pos(pos_real.x, pos_real.y);
			switch_player.set_on_press([&]() {std::cout << "切换" << std::endl; });
			board.set_position(pos);

			select_board.add_frame(&select_box);
			select_board.set_position(pos);
		}

		void set_slime()
		{
			delete slime;
			slime = nullptr;
			switch (player_number)
			{
			case 0:break;
			case 1:slime = new Slime; slime->slime_init(&Atlas_Slime, player_number, tile_type, pos_real); break;
			case 2:slime = new Slime; slime->slime_init(&Atlas_PSlime, player_number, tile_type, pos_real); break;
			default:break;
			}
		}

		void handleEvent(const SDL_Event& event)
		{
			show_select_box = false;
			if (switch_player.if_hovering())
			{
				show_select_box = true;
			}
			if (switch_player.if_press() && player_number == 0)
			{
				save_time_fun();

				switch_player.pressed();
				player_number = Player_sets.get_player_num();
				Mix_PlayChannel(-1, put_slime, 0);
				camera->shake(2, 0.08);

				set_slime();

				std::cout << "当前玩家" << Player_sets.get_player_num() << std::endl;

				/////////////////////////////////////////////////////////////////////
				board_test_fun(num, Player_sets.get_player_num());

				change_tile_player(num, Player_sets.get_player_num());

				game_data.save_game_file();

				Player_sets.next_player();

				game_data.set_board_now_player(now_board_num, Player_sets.get_player_num());
			}
		}

		virtual void s_init() = 0;
		virtual void on_updata(double delta) = 0;
		virtual void render_board(const Camera& camera) = 0;
		virtual void render_slime(const Camera& camera) = 0;
		virtual void render_over_board(const Camera& camera) = 0;
		

	public:
		//pugi::xml_node tile_node;
		int num;
		int player_number;//识别并渲染对应棋子（用数字方便未来添加多人）
		int tile_type;//给格子分类
		int now_board_num;
		Slime* slime = nullptr;
		Vector2 pos_real;
		Animation board;
		bool show_select_box = false;

		bool final_highlight = false;
		Animation select_board;
		pugi::xml_node this_node;
		Button switch_player = { 0 };
		std::function<void(int check_tile_id, int player_id)> board_test_fun = nullptr;
		std::function<void(int tile_id, int player_number)> change_tile_player = nullptr;
		std::function<void()> save_time_fun = nullptr;
	};

	class Normal_Tile : public Tile
	{
	public:

		void s_init()override
		{
			board.add_frame(&board_tex);
		}

		void on_updata(double delta) override
		{
			if (player_number != 0 && slime != nullptr)
			{
				slime->on_updata(delta);
			}
			board.on_update(delta);
		}

		void render_board(const Camera& camera) override//先渲染棋盘，后渲染史莱姆
		{
			board.on_render(camera);
		}

		void render_slime(const Camera& camera) override
		{
			if (player_number != 0 && slime != nullptr)
			{
				slime->render(camera);
			}
		}

		void render_over_board(const Camera& camera) override
		{
			if (show_select_box || final_highlight)
			{
				select_board.on_render(camera);
			}
		}

	};


	class Board
	{
	public:

		Board() 
		{
		}
		~Board()
		{
			for (Tile* tmp : tile_sets)
			{
				delete tmp;
			}

			for (Tile* tmp : tile_history)
			{
				delete tmp;
			}
		};

		void init(std::string name, bool done, int winner, std::string tiles, int this_board_id
			, std::string tiles_type, int final_hightlight,float time,
			std::function<void(int check_tile_id, int player_id)> game_check)
		{	
			game_check_fun = game_check;

			memory_board_data.board_name = name;
			memory_board_data.game_state = done;
			memory_board_data.winner_id = winner;
			memory_board_data.board_id = this_board_id;
			memory_board_data.memory_tiles = tiles;
			memory_board_data.memory_tiles_type = tiles_type;
			memory_board_data.final_hightlight_num = final_hightlight;

			now_time = time;

			for (int i = 0; i < tiles.size(); i++)
			{
				Tile* p_tile;
				int now_tile_type = (int)(tiles_type[i]) - '0';
				switch (now_tile_type)
				{
				case 0:p_tile = new Normal_Tile; break;
				}
				tile_sets.push_back(p_tile);
			}

			init_tiles();

			std::cout << tiles << std::endl;
		}

		void set_tile(int tile_id, int player_number)
		{
			memory_board_data.memory_tiles[tile_id] = (char)(player_number + '0');
			std::string tmp = memory_board_data.memory_tiles;
			game_data.set_board_tiles(memory_board_data.board_id, tmp);
		}

		Vector2 figure_pos_out(int num)//之后再优化
		{
			Vector2 tmp;

			int num_shift = num;
			double each_block = setting.resolution.resolution.y / 19;
			double x = num_shift % 15 * each_block + setting.resolution.resolution.x / 2 - 7.5 * each_block;
			double y = ((int)(num_shift / 15) + 2) * each_block;
			tmp.x = x; tmp.y = y;
			return tmp;
		}



		enum direction {
			left,
			right,
			up,
			down
		};

		Vector2 get_r_pos(int id, direction d)
		{
			Vector2 tmp = figure_pos_out(id);
			switch (d)
			{
			case left:tmp.x -= 1; break;
			case right:tmp.x += 1; break;
			case up:tmp.y -= 1; break;
			case down:tmp.y += 1; break;
			}
			return tmp;
		}

		bool check_pos(Vector2 pos)
		{
			bool x = pos.x >= 0 && pos.x < 15;
			bool y = pos.y >= 0 && pos.y < 15;
			return x && y;
		}

		void save_time()
		{
			game_data.set_board_now_time(memory_board_data.board_id, now_time);
		}

		void init_tiles()
		{
			for (int i = 0; i < memory_board_data.memory_tiles.size(); i++)
			{
				int player_number = memory_board_data.memory_tiles[i] - '0';
				int tiles_type = memory_board_data.memory_tiles_type[i] - '0';
				tile_sets[i]->init(i, player_number, tiles_type, figure_pos_out(i),
					memory_board_data.board_id,
					game_check_fun, 
					[&](int tile_id, int player_number) {set_tile(tile_id, player_number); },
					[&]() {save_time(); });
				
				//std::cout << figure_pos_out(i).x << figure_pos_out(i).y << std::endl;
			}
			initing = false;
		}

		//void load_board(std::string name) {}

		void handleEvent(const SDL_Event& event)
		{
			if (!initing && !check_once) {
				if (!memory_board_data.game_state) {
					for (Tile* tmp : tile_sets)
					{
						tmp->handleEvent(event);
					}
				}
				else
				{
					game_check_fun(memory_board_data.final_hightlight_num,
						memory_board_data.winner_id);
					check_once = true;
				}
			}
		}

		void on_updata(double delta) 
		{
			if (memory_board_data.winner_id == -1)
			{
				now_time += delta;
			}

			if (!initing) {
				for (Tile* tmp : tile_sets)
				{
					tmp->on_updata(delta);
				}

			}
			else
			{

			}
		}

		void show_text(std::string context, Vector2 pos, TTF_Font* now_font,
			SDL_Color color = { 255, 255, 255, 255 })
		{
			SDL_Surface* text = TTF_RenderUTF8_Blended(now_font, context.c_str(), color);
			SDL_Texture* tex_text = SDL_CreateTextureFromSurface(renderer, text);
			SDL_Rect text_rect = { pos.x - (text->w / 2), pos.y - (text->h / 2),text->w ,text->h };
			SDL_RenderCopy(renderer, tex_text, nullptr, &text_rect);

			SDL_DestroyTexture(tex_text); SDL_FreeSurface(text);
		}

		void render(const Camera& camera) 
		{
			std::string str = std::to_string(now_time);
			size_t pos = str.find('.');
			if (pos != std::string::npos && pos + 3 < str.size()) {
				str.erase(pos + 3);
			}
			str = u8"时间: " + str + u8" 秒";
			show_text(str, text_time_pos, font_s, { 255,255,255,200 });//显示时间

			std::string text_player = u8"玩家" + std::to_string(Player_sets.get_player_num());
			show_text(text_player, text_player_pos, font_s,{ 255,255,255,200 });

			if (!initing) {
				for (Tile* tmp : tile_sets)
				{
					tmp->render_board(camera);
				}

				for (Tile* tmp : tile_sets)
				{
					tmp->render_over_board(camera);
				}

				for (Tile* tmp : tile_sets)
				{
					tmp->render_slime(camera);
				}
			}
		}

	public:
		bool initing = true;

		bool check_once = false;

		struct Memory_player
		{
			int ID;
		};

		struct Memory_board_data
		{
			std::string board_name;
			std::string create_time;
			bool game_state = false;
			int winner_id = -1;
			int timer_counter = 0;
			int board_id = -1;
			std::string memory_tiles;
			std::string memory_tiles_type;
			int final_hightlight_num;
		}memory_board_data;

		std::vector<Tile*> tile_sets;

		std::vector<Tile*> tile_history;

		Vector2 text_time_pos = { 7 * setting.resolution.resolution.x / 8,7 * setting.resolution.resolution.y / 8 };
		Vector2 text_player_pos = { 7 * setting.resolution.resolution.x / 8,2 * setting.resolution.resolution.y / 8 };

		float now_time = 0;

		std::function<void(int check_tile_id, int player_id)> game_check_fun = nullptr;
		//Tile tile_sets[15][15];//从0开始数 标准棋盘大小（未来可能会变）记得按照固定顺序写入和读取
	};

	class Board_Manager
	{
	public:
		//pugi::xml_document* this_game_file = nullptr;
		pugi::xml_node board_node;
		Board* currentBoard = nullptr;
		std::vector<std::string> board_name_sets;
		bool currentstate = false;

		int select_board_number = -1;
		int select_board_number_long_term = -1;

	public:

		Board_Manager() = default;
		~Board_Manager() = default;

		void init()
		{
			currentstate = false;

			game_data.open_board_sets();

		}

		void handleEvent(const SDL_Event& event)
		{
			if (select_board_number == -1 && currentBoard != nullptr) {
				this->currentBoard->handleEvent(event);
			}
			else if (select_board_number != -1)
			{

			}
		}

		void check_game_done(int check_tile_id, int player_id)////////游戏逻辑部分
		{
			int x = check_tile_id % 15;
			int y = check_tile_id / 15;
			std::vector<Tile*> line;

			// 垂直判断（正向和逆向）
			line.clear();
			line.push_back(currentBoard->tile_sets[check_tile_id]);
			int up_down = 1;
			for (int i = 1; i <= 4; i++) {
				if (y + i < 15 && player_id == currentBoard->tile_sets[check_tile_id + 15 * i]->player_number) {
					line.push_back(currentBoard->tile_sets[check_tile_id + 15 * i]);
					up_down++;
				}
				else {
					break;
				}
			}
			for (int i = 1; i <= 4; i++) {
				if (y - i >= 0 && player_id == currentBoard->tile_sets[check_tile_id - 15 * i]->player_number) {
					line.push_back(currentBoard->tile_sets[check_tile_id - 15 * i]);
					up_down++;
				}
				else {
					break;
				}
			}
			if (up_down >= 5) {
				game_done(player_id, line, check_tile_id);
			}

			// 水平判断（正向和逆向）
			line.clear();
			line.push_back(currentBoard->tile_sets[check_tile_id]);
			int left_right = 1;
			for (int i = 1; i <= 4; i++) {
				if (x + i < 15 && player_id == currentBoard->tile_sets[check_tile_id + i]->player_number) {
					line.push_back(currentBoard->tile_sets[check_tile_id + i]);
					left_right++;
				}
				else {
					break;
				}
			}
			for (int i = 1; i <= 4; i++) {
				if (x - i >= 0 && player_id == currentBoard->tile_sets[check_tile_id - i]->player_number) {
					line.push_back(currentBoard->tile_sets[check_tile_id - i]);
					left_right++;
				}
				else {
					break;
				}
			}
			if (left_right >= 5) {
				game_done(player_id, line, check_tile_id);
			}

			// 对角线判断（正向和逆向）
			line.clear();
			line.push_back(currentBoard->tile_sets[check_tile_id]);
			int dia = 1;
			for (int i = 1; i <= 4; i++) {
				if (x + i < 15 && y + i < 15 && player_id == currentBoard->tile_sets[check_tile_id + i + 15 * i]->player_number) {
					line.push_back(currentBoard->tile_sets[check_tile_id + i + 15 * i]);
					dia++;
				}
				else {
					break;
				}
			}
			for (int i = 1; i <= 4; i++) {
				if (x - i >= 0 && y - i >= 0 && player_id == currentBoard->tile_sets[check_tile_id - i - 15 * i]->player_number) {
					line.push_back(currentBoard->tile_sets[check_tile_id - i - 15 * i]);
					dia++;
				}
				else {
					break;
				}
			}
			if (dia >= 5) {
				game_done(player_id, line, check_tile_id);
			}

			// 反对角线判断（正向和逆向）
			line.clear();
			line.push_back(currentBoard->tile_sets[check_tile_id]);
			int redia = 1;
			for (int i = 1; i <= 4; i++) {
				if (x + i < 15 && y - i >= 0 && player_id == currentBoard->tile_sets[check_tile_id + i - 15 * i]->player_number) {
					line.push_back(currentBoard->tile_sets[check_tile_id + i - 15 * i]);
					redia++;
				}
				else {
					break;
				}
			}
			for (int i = 1; i <= 4; i++) {
				if (x - i >= 0 && y + i < 15 && player_id == currentBoard->tile_sets[check_tile_id - i + 15 * i]->player_number) {
					line.push_back(currentBoard->tile_sets[check_tile_id - i + 15 * i]);
					redia++;
				}
				else {
					break;
				}
			}
			if (redia >= 5) {
				game_done(player_id, line, check_tile_id);
			}
		}


		void new_board()
		{
			game_data.new_board();
		}

		void game_done(int winner_id,std::vector<Tile*> line, int hight_light)
		{
			//std::string final_high = "XXXXX";
			
			int count = 0;
			for (Tile* tmp : line)
			{
				tmp->final_highlight = true;
				if (tmp->slime != nullptr) {
					tmp->slime->if_visible = false;
					std::cout << "执行毁灭动画" << tmp->slime->if_visible << std::endl;
				}
				//final_high[count++] = std::to_string(tmp->num);
				//final_high += std::to_string(tmp->num);

				//std::cout << final_high << std::endl;
			}

			Mix_PlayChannel(-1, sound_explosion, 0);

			currentstate = true;

			int this_node_num = select_board_number_long_term;

			game_data.set_board_final_highlight(this_node_num, hight_light);

			game_data.set_board_game_state(this_node_num, true);
			currentBoard->memory_board_data.game_state = true;

			game_data.set_board_winner(this_node_num, winner_id);
			currentBoard->memory_board_data.winner_id = winner_id;
			Player_sets.set_now_winner(winner_id);

			game_data.save_game_file();

			std::cout << "游戏结束" << winner_id << "获胜" << std::endl;
			////////实现获胜动画

		}

		void select_board(int num)//从零开始，注意对齐
		{
			select_board_number = num;
			select_board_number_long_term = select_board_number;
			/*currentBoard = &board_sets[num];
			std::cout << "选择 " << currentBoard->name << " 棋盘" << std::endl;*/
		}

		void board_init()
		{
			delete currentBoard;
			currentBoard = nullptr;
			currentBoard = new Board;

			std::string name = game_data.get_board_name(select_board_number);
			std::string tiles = game_data.get_board_tiles(select_board_number);
			std::string tiles_type = game_data.get_board_tiles_type(select_board_number);
			int final_highlight = game_data.get_board_final_highlight(select_board_number);
			bool if_done = game_data.get_board_game_state(select_board_number);
			int winner_id = game_data.get_board_winner(select_board_number);
			int now_player_number_read = game_data.get_board_now_player(select_board_number);
			Player_sets.set_nowplayer(now_player_number_read);
			Player_sets.set_now_winner(winner_id);
			float game_time = game_data.get_board_now_time(select_board_number);

			currentBoard->init(name, if_done, winner_id, tiles,
				select_board_number, tiles_type, final_highlight,game_time,
				[&](int check_tile_id, int player_id) {this->check_game_done(check_tile_id, player_id); });

			std::cout << "选择 " << name.c_str() << " 棋盘" << std::endl;
		}

		void on_updata(double delta)
		{
			if (select_board_number == -1 && currentBoard != nullptr) {
				this->currentBoard->on_updata(delta);
			}
			else if (select_board_number != -1)
			{
				board_init();
				select_board_number = -1;
			}
		}

		void render(const Camera& camera)
		{

			if (select_board_number == -1 && currentBoard != nullptr) {
				this->currentBoard->render(camera);
			}
			else if(select_board_number != -1)
			{
				//select_board_number = -1;
				std::cout << "更新指针，暂停渲染" << std::endl;
			}
			//this->currentBoard->render(camera);
		}

	};







	class Card
	{

	};

	class Card_sets
	{

	};


	class Card_Manager
	{

	};

}