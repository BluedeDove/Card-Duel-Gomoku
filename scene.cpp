#pragma once

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include"camera.cpp"
#include"button.cpp"
#include"setting.cpp"
#include"game_elements.h"
#include"B_showing_list.h"
//#include"All_Function.h"

//extern SDL_Window* window;
//extern SDL_Renderer* renderer;
extern SDL_Texture* cloud_background;
extern SDL_Texture* cloud;
extern SDL_Texture* tex_background;
extern Atlas Atlas_background;
extern Atlas Atlas_game_bg;

extern SDL_Texture* Slime;
extern SDL_Texture* PSlime;

//extern Camera* camera;
extern Game_setting setting;
extern Game_elements::Board_Manager board_manager;
extern Game_player Player_sets;
extern SDL_Renderer* renderer;

extern Mix_Chunk* click_button;

extern TTF_Font* font;
extern TTF_Font* font_s;
extern TTF_Font* font_b;



inline void put_background(const Camera& camera, SDL_Texture* background)
{
    const SDL_FRect rect_background =
    {
        0,0,setting.resolution.resolution.x ,setting.resolution.resolution.y
    };
    camera.render_texture(background, nullptr, &rect_background, 0, nullptr);
}

namespace Game_tools {
    //void put_text(std::string context, double x, double y);//中心坐标
    
}
class Scene {//抽象场景类，利用虚函数规范子类的实现
public:
    virtual ~Scene() {}
    virtual void init() = 0;
    virtual void handleEvent(const SDL_Event& event) = 0;//处理用户输入与游戏逻辑，加上场景用于切换
    virtual void update(double delta) = 0;//时间顺序更新动画
    virtual void render(const Camera& camera) = 0;//根据相机位置调整渲染
    virtual void p_select_num_f(int& p) = 0;
};

//int scene_selete_num = 0;

class MainMenuScene : public Scene {
public:

    /*void change_scene(int num)
    {
        *p_select_num = num;
    }*/

    void init() override
    {
        background_with_atlas.add_frame(&Atlas_background,true);
        background_with_atlas.set_interval(0.08);
        Vector2 tmp = { setting.resolution.resolution.x / 2,setting.resolution.resolution.y / 2 };
        background_with_atlas.set_position(tmp);

        this->start_B = new Button(1, setting.resolution.resolution.x / 2, setting.resolution.resolution.y / 3);
        this->start_B->set_on_press([&]()
            {
                *p_select_num = 2;//开始游戏界面
                board_manager.new_board();
                Mix_PlayChannel(-1, click_button, 0);
                board_manager.select_board(game_data.board_node_sets.size() -1);
            }
        );
        this->start_B->set_text(u8"创建棋盘");
        //this->start_B->B_on = false;//测试用

        this->select_ = new Button(1, setting.resolution.resolution.x / 2, setting.resolution.resolution.y * 2 / 3);
        this->select_->set_on_press([&]()
            {
                *p_select_num = 1;//选择界面
            }
        );
        this->select_->set_text(u8"选择棋盘");
    }
    void handleEvent(const SDL_Event& event) override {
        if (this->start_B->if_press())
        {
            Mix_PlayChannel(-1, click_button, 0);
            this->start_B->pressed();
        }

        if (this->select_->if_press())
        {
            Mix_PlayChannel(-1, click_button, 0);
            this->select_->pressed();
        }
    }

    void update(double delta) override {
        background_with_atlas.on_update(delta);
    }

    void render(const Camera& camera) override {
        {
            background_with_atlas.on_render(camera);

            /*int width_bg, height_bg;
            SDL_QueryTexture(tex_background, nullptr, nullptr, &width_bg, &height_bg);
            const SDL_FRect rect_background =
            {
                (1280 - width_bg) / 2.0f,
                (720 - height_bg) / 2.0f,
                (float)width_bg, (float)height_bg
            };
            camera.render_texture(tex_background, nullptr, &rect_background, 0, nullptr);*/


            this->start_B->render(camera);
            this->select_->render(camera);
        }
    }

    void p_select_num_f(int& p) override
    {
        p_select_num = &p;
    }

    
private:
    //Button start_B = { 1, setting.resolution.resolution.x/2, setting.resolution.resolution.y/2 };
    Button* start_B = nullptr;
    Button* select_ = nullptr;
    int* p_select_num = nullptr;
    Animation background_with_atlas;
};

class GameSelectScene : public Scene {
public:

    void init()override//利用比较两个数值的方法来确保执行一次
    {
        //showing_boards.clear();
        showing_boards = nullptr;
        showing_boards = new B_list;

        for (int i = 0; i < game_data.board_node_sets.size(); i++)
        {
            int id_num = i;
            showing_boards->add_button(1, [&]()
                {
                    //board_manager.select_board(id_num);
                    *p_select_num = 2;
                    //set_select_num(2); //2去开始游戏
                    
                    //std::cout << "按钮触发选择" << i << std::endl;

                }, game_data.get_board_name(i));
            //std::cout << board_manager.board_name_sets[i] << std::endl;
        }
        showing_boards->init();
        showing_boards->set_B_id();

        back = new Button(2, 0, 0, font_s);
        back->set_text(u8"返回");
        back->set_pos(setting.resolution.resolution.x / 12, setting.resolution.resolution.y / 8);
        back->set_on_press([&]() {*p_select_num = 0; });
    }

    void handleEvent(const SDL_Event& event) override {
        showing_boards->handleEvent(event);
        for (Button tmp : showing_boards->list)
        {
            if (tmp.if_press())
            {
                Mix_PlayChannel(-1, click_button, 0);
                board_manager.select_board(tmp.get_id());
                tmp.pressed();
            }
        }
        if (back->if_press())
        {
            Mix_PlayChannel(-1, click_button, 0);
            std::cout << "你按下了返回键" << std::endl;
            back->pressed();
        }
    }

    void update(double delta) override {
        //board_manager.on_updata(delta);
    }

    void render(const Camera& camera) override {
        //board_manager.render(camera);
        showing_boards->render(camera);
        back->render(camera);
    }

    /*void set_select_num(int num)
    {
        *p_select_num = num;
    }*/

    void p_select_num_f(int& p)override
    {
        p_select_num = &p;
    }

private:
    int* p_select_num = nullptr;
    B_list* showing_boards = nullptr;//////////////////////////////////////////////////////////////////////////////
    Button* back = nullptr;
};

class GameScene : public Scene {
public:

    void init()override
    {
        Mix_VolumeMusic(16);

        game_background_with_atlas = nullptr;
        check_now_player();

        game_background_with_atlas = new Animation;
        game_background_with_atlas->add_frame(&Atlas_game_bg, true);
        game_background_with_atlas->set_interval(0.1);
        Vector2 tmp = { setting.resolution.resolution.x / 2,setting.resolution.resolution.y / 2 };
        game_background_with_atlas->set_position(tmp);

        board_manager.on_updata(0);
        board_manager.render(nullptr);
        std::cout << "游戏场景初始化" << std::endl;

        back = new Button(2, 0, 0 , font_s);

        back->set_text(u8"返回");
        back->set_pos(setting.resolution.resolution.x / 12, setting.resolution.resolution.y / 8);
        back->set_on_press([&]() {*p_select_num = 0; });

        now_player = Player_sets.get_player_num();

        set_showing_pos();
    }

    void handleEvent(const SDL_Event& event) override {
        board_manager.handleEvent(event);
        if (back->if_press())
        {
            Mix_PlayChannel(-1, click_button, 0);
            std::cout << "你按下了返回键" << std::endl;
            back->pressed();
        }
    }

    void set_showing_pos()
    {
        Vector2 showing_pos = { 7* setting.resolution.resolution.x / 8,
        setting.resolution.resolution.y / 8 };
        int width, height; show_currentplayer = new SDL_FRect;
        SDL_QueryTexture(currentplayer, nullptr, nullptr, &width, &height);
        show_currentplayer->x = showing_pos.x - width / 2;
        show_currentplayer->y = showing_pos.y - height / 2;
        show_currentplayer->w = width;
        show_currentplayer->h = height;

    }

    void update(double delta) override {

        game_background_with_atlas->on_update(delta);

        board_manager.on_updata(delta);

        if (now_player != Player_sets.get_player_num())
        {
            check_now_player();
            now_player = Player_sets.get_player_num();
        }
    }

    void check_now_player()
    {
        currentplayer = nullptr;
        switch (Player_sets.get_player_num())
        {
        case 1:currentplayer = Slime; break;
        case 2:currentplayer = PSlime; break;
        }
        set_showing_pos();
    }

    void render(const Camera& camera) override {

        game_background_with_atlas->on_render(camera);

        board_manager.render(camera);

        back->render(camera);

        //Game_tools::put_text(u8"当前玩家", show_currentplayer->x, show_currentplayer->y * 1.25);

        camera.render_texture(currentplayer, nullptr, show_currentplayer, 0, nullptr);
        
        if (Player_sets.get_now_winner() != -1)
        {
            std::string context = u8"游戏结束 " + std::to_string(Player_sets.get_now_winner()) + u8"号玩家获胜";
            SDL_Surface* text_tmp = TTF_RenderUTF8_Blended(font_b, context.c_str(), { 255, 255, 255, 180 });
            SDL_Texture* tex_text_tmp = SDL_CreateTextureFromSurface(renderer, text_tmp);
            SDL_Rect text_rect = { setting.resolution.resolution.x/2 - (text_tmp->w / 2), 
                setting.resolution.resolution.y/2 - (text_tmp->h / 2),text_tmp->w ,text_tmp->h };
            SDL_RenderCopy(renderer, tex_text_tmp, nullptr, &text_rect);
            SDL_DestroyTexture(tex_text_tmp); SDL_FreeSurface(text_tmp);
        }
    }

    void p_select_num_f(int& p)override
    {
        p_select_num = &p;
    }

private:
    int* p_select_num = nullptr;
    Button* back = nullptr;
    Animation* game_background_with_atlas = nullptr;
    int now_player;

    SDL_Texture* currentplayer = nullptr;
    SDL_FRect* show_currentplayer = nullptr;
    SDL_FRect* show_currentplayer_text = nullptr;
};

//模板如下

//class GameScene : public Scene {
//public:
//
//    void init()override
//    {
//
//    }
//
//    void handleEvent(const SDL_Event& event) override {
//
//    }
//
//    void update(double delta) override {
//
//    }
//
//    void render(const Camera& camera) override {
//
//    }
//
//    void p_select_num_f(int& p)override
//    {
//        p_select_num = &p;
//    }
//
//private:
//    int* p_select_num = nullptr;
//};

class Scene_Manager {
private:
    Scene* currentScene;
    int current_num = 0;
    Scene* scenes[5] = { new MainMenuScene , new GameSelectScene, new GameScene };
    //利用映射实现便捷管理，增加代码可读性和易于添加新场景
    //0->MainMenuScene; 1->GameScene

public:
    int scene_selete_num = 0;

public:
    Scene_Manager() : currentScene(nullptr) {}

    ~Scene_Manager() {
        /*for (auto& pair : scenes) {
            delete pair.second;
        }*/
    }

    void init()
    {
        Mix_VolumeMusic(32);
        currentScene->init();
        currentScene->p_select_num_f(scene_selete_num);
    }

    void setScene(const int selcet_num) {
        currentScene = scenes[selcet_num];
    }

    void handleEvent(const SDL_Event& event) {
        if (currentScene) {
            currentScene->handleEvent(event);
        }
        currentScene->p_select_num_f(scene_selete_num);
    }

    void update(double delta) {
        if (scene_selete_num != current_num)//切换场景 需要初始化
        {
            current_num = scene_selete_num;
            currentScene = scenes[current_num];
            init();
        }
        else
        {
            if (currentScene) {
                currentScene->update(delta);
            }
        }
    }

    void render(const Camera& camera) {
        if (scene_selete_num != current_num) {
            
        }
        else 
        {
            if (currentScene) {

                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 128);
                SDL_Rect fillRect = {0,0,setting.resolution.resolution.x ,setting.resolution.resolution.y};

                put_background(camera, cloud_background);
                put_background(camera, cloud);
                SDL_RenderFillRect(renderer, &fillRect);
                currentScene->render(camera);
            }
        }
    }
};

