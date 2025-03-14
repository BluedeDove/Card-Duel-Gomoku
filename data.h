#pragma once

#include<string>
#include<iostream>
#include "pugixml.hpp"

extern Game_player Player_sets;

namespace Game_time
{
	std::string getCurrentTimeAsString();
	double generateRandomDouble();
}

namespace data {

	using namespace pugi;
	using namespace std;

    class Game_Data {
    public:                       

		void init()
		{
			saves = nullptr;
			saves = new xml_document;
			check_xml_file(game_filename, game_name);
		}

		bool check_xml_file(string name, string game_name)
		{
			const char* filename = name.c_str();
			xml_parse_result result = saves->load_file(filename);

			if (result.status == pugi::status_ok)
			{
				return true;
			}
			else
			{
				xml_node decl = saves->append_child(pugi::node_declaration);
				decl.append_attribute("version") = "1.0";
				decl.append_attribute("encoding") = "UTF-8";

				xml_node game = saves->append_child(u8"game");
				game.append_attribute(u8"name") = game_name.c_str();
				//game.append_attribute


				saves->save_file(filename);
				std::cout << "创建新的文件" << std::endl;
				cout << result.description() << endl;
				return false;
			}
		}

		void save_game_file()
		{
			const char* filename = game_filename.c_str();
			if (saves->save_file(filename))
			{
				cout << "文件保存成功" << endl;
			}
		}

		std::vector<pugi::xml_node> open_board_sets()
		{
			board_node_sets.clear();

			xml_node board_node = saves->child(u8"Boards");
			//pugi::xml_node find_it = board_node.find_node(u8"Boards");

			if (board_node)
			{
				std::cout << "非空棋盘" << std::endl;
				for (pugi::xml_node node : saves->children(u8"Boards"))
				{
					board_node_sets.push_back(node);

				}
				std::cout << "已加载棋盘：" << board_node_sets.size() << std::endl;
			}
			else
			{
				std::cout << "空棋盘" << std::endl;
				new_board();
			}
			return board_node_sets;
		}

		void new_board()
		{
			xml_node board_node = saves->append_child(u8"Boards");
			pugi::xml_node now_tmp = board_node;
			std::string name = Game_time::getCurrentTimeAsString();

			pugi::xml_node name_node = now_tmp.append_child(u8"name");
			name_node.text() = name.c_str();

			pugi::xml_node done_node = now_tmp.append_child(u8"game_state");//false未结束
			done_node.text() = false;

			pugi::xml_node winner_node = now_tmp.append_child(u8"winner_id");
			winner_node.text() = -1;

			pugi::xml_node now_player = now_tmp.append_child(u8"now_player");
			now_player.text() = 1;

			pugi::xml_node now_time = now_tmp.append_child(u8"now_time");
			now_time.text() = 0;

			pugi::xml_node tiles = now_tmp.append_child(u8"tiles");
			string menory_tiles = "0";
			for (int i = 0; i < 225 -1; i++)
			{
				menory_tiles += "0";
			}
			tiles.text() = menory_tiles.c_str();

			pugi::xml_node tiles_type = now_tmp.append_child(u8"tiles_type");
			string menory_tiles_type = "0";
			for (int i = 0; i < 225 - 1; i++)
			{
				menory_tiles_type += "0";
			}
			tiles_type.text() = menory_tiles_type.c_str();

			pugi::xml_node final_highlight = now_tmp.append_child(u8"final_highlight");
			int menory_final_highlight = -1;
			final_highlight.text() = menory_final_highlight;

			save_game_file();

			board_node_sets.push_back(board_node);

			Player_sets.set_nowplayer(1);
			Player_sets.set_now_winner(-1);

			std::cout << "创建棋盘" << std::endl;
		}

		//////////////////////////////////////////////////////////////////////////////////////////////////////////

		std::string get_board_name(int board_id)
		{
			return board_node_sets[board_id].child(u8"name").text().as_string();
		}

		std::string get_board_tiles(int board_id)
		{
			return board_node_sets[board_id].child(u8"tiles").text().as_string();
		}

		std::string get_board_tiles_type(int board_id)
		{
			return board_node_sets[board_id].child(u8"tiles_type").text().as_string();
		}

		int get_board_final_highlight(int board_id)
		{
			return board_node_sets[board_id].child(u8"final_highlight").text().as_int();
		}

		bool get_board_game_state(int board_id)
		{
			return board_node_sets[board_id].child(u8"game_state").text().as_bool();
		}

		int get_board_winner(int board_id)
		{
			return board_node_sets[board_id].child(u8"winner_id").text().as_int();
		}

		int get_board_now_player(int board_id)
		{
			return board_node_sets[board_id].child(u8"now_player").text().as_int();
		}

		float get_board_now_time(int board_id)
		{
			return board_node_sets[board_id].child(u8"now_time").text().as_float();
		}

		//////////////////////////////////////////////////////////////////////////////////////////////////////////

		void set_board_name(int board_id , std::string target)
		{
			board_node_sets[board_id].child(u8"name").text() = target.c_str();
		}

		void set_board_tiles(int board_id, std::string target)
		{
			board_node_sets[board_id].child(u8"tiles").text() = target.c_str();
		}

		void set_board_tiles_type(int board_id, std::string target)
		{
			board_node_sets[board_id].child(u8"tiles_type").text() = target.c_str();
		}

		void set_board_final_highlight(int board_id, int target)
		{
			board_node_sets[board_id].child(u8"final_highlight").text() = target;
		}

		void set_board_game_state(int board_id, bool target)
		{
			board_node_sets[board_id].child(u8"game_state").text() = target;
		}

		void set_board_winner(int board_id, int target)
		{
			board_node_sets[board_id].child(u8"winner_id").text() = target;
		}

		void set_board_now_player(int board_id, int target)
		{
			board_node_sets[board_id].child(u8"now_player").text() = target;
		}

		void set_board_now_time(int board_id, float target)
		{
			board_node_sets[board_id].child(u8"now_time").text() = target;
		}

	public:
        std::string game_filename = u8"saves/all_data.xml";
		std::string game_name = u8"Card Duel Gomoku";
        pugi::xml_document* saves = nullptr;
		std::vector<pugi::xml_node> board_node_sets;
    };

}