#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h" // ABS
#include "game/world.h"
#include "user_interface.h"
#include "pi_time.h"
#include "game/input.h"

#include "raylib.h"

enum GameScene
{
    GS_INVALID,
    GS_TITLE_SCREEN,
    GS_GAME
};

enum GameMenu
{
    GM_NONE,
    GM_MAIN_MENU,
    GM_LEVEL_MENU,
    GM_JOIN_MENU,
    GM_CONNECTING_MENU,
    GM_HOST_MENU,
    GM_OPTIONS_MENU,
    GM_PLAYER_MENU
};

struct State
{
    GameScene last_scene = GS_INVALID;
    GameScene current_scene = GS_TITLE_SCREEN;
    bool scene_changed = true;
    GameMenu last_menu = GM_NONE;
    GameMenu current_menu = GM_MAIN_MENU;
    bool menu_changed = true;
    bool should_quit = false;

    struct {
        struct {
            char ip[16];
            char port[6];
        } join;

        struct {
            char port[6];
        } host;
    } fields;

    struct {
        bool world_initialised = false;
    } game;

} state = {};

void DoMainMenu()
{
    UI::Button solo_play_button = UI::Button("Solo Play");
    UI::Button join_game_button = UI::Button("Join Multiplayer");
    UI::Button host_game_button = UI::Button("Host Game");
    UI::Button options_button = UI::Button("Options");
    UI::Button exit_button = UI::Button("Exit");

    UI::Begin();
    {
        UI::Add(&solo_play_button.base); UI::EndRow();
        UI::Add(&join_game_button.base);
        UI::Add(&host_game_button.base); UI::EndRow();
        UI::Add(&options_button.base); UI::EndRow();
        UI::Add(&exit_button.base); UI::EndRow();
    }
    UI::End();

    if (solo_play_button.IsReleased())
        state.current_menu = GM_LEVEL_MENU;

    if (join_game_button.IsReleased())
        state.current_menu = GM_JOIN_MENU;

    if (host_game_button.IsReleased())
        state.current_menu = GM_HOST_MENU;

    if (options_button.IsReleased())
        state.current_menu = GM_OPTIONS_MENU;

    if (exit_button.IsReleased())
        state.should_quit = true;

    solo_play_button.Draw();
    join_game_button.Draw();
    host_game_button.Draw();
    options_button.Draw();
    exit_button.Draw();
}

void DoJoinMenu()
{
    if(state.menu_changed)
    {
        const char default_ip[] = "localhost";
        const char default_port[] = "25565";
        memcpy(state.fields.join.ip, default_ip, sizeof(default_ip));
        memcpy(state.fields.join.port, default_port, sizeof(default_port));
    }

    UI::TextField ip_field = UI::TextField(state.fields.join.ip, sizeof(state.fields.join.ip));
    UI::TextField port_field = UI::TextField(state.fields.join.port, sizeof(state.fields.join.port));
    UI::Button join_button = UI::Button("Join");
    UI::Button close_button = UI::Button("Close");

    UI::Begin();
    {
        UI::Add(&ip_field.base);
        UI::Add(&port_field.base);
        UI::Add(&join_button.base); UI::EndRow();
        UI::AddSpace(); UI::EndRow();
        UI::Add(&close_button.base); UI::EndRow();
    }
    UI::End();

    ip_field.Update();
    port_field.Update();

    if(join_button.IsReleased())
        state.current_menu = GM_CONNECTING_MENU;

    if(close_button.IsReleased())
        state.current_menu = GM_MAIN_MENU;



    ip_field.Draw();
    port_field.Draw();
    join_button.Draw();
    close_button.Draw();
}

void DoConnectingMenu()
{
    if(state.menu_changed)
    {

    }

    ClearBackground(DARKGREEN);
}

void DoHostMenu()
{
    if(state.menu_changed)
    {
        const char default_port[] = "25565";
        memcpy(state.fields.host.port, default_port, sizeof(default_port));
    }

    UI::TextField port_field = UI::TextField(state.fields.host.port, sizeof(state.fields.host.port));
    UI::Button host_button = UI::Button("Host");
    UI::Button close_button = UI::Button("Close");

    UI::Begin();
    {
        UI::Add(&port_field.base);
        UI::Add(&host_button.base); UI::EndRow();
        UI::AddSpace(); UI::EndRow();
        UI::Add(&close_button.base); UI::EndRow();
    }
    UI::End();

    port_field.Update();

    if(host_button.IsReleased())
    {
        // nothing done yet
    }

    if(close_button.IsReleased())
        state.current_menu = GM_MAIN_MENU;

    port_field.Draw();
    host_button.Draw();
    close_button.Draw();
}

void DoLevelMenu()
{
    UI::Button lvl1_button = UI::Button("Level 1");
    UI::Button lvl2_button = UI::Button("Level 2");
    UI::Button lvl3_button = UI::Button("Level 3");
    UI::Button close_button = UI::Button("Close");

    UI::Begin();
    {
        UI::Add(&lvl1_button.base);
        UI::Add(&lvl2_button.base);
        UI::Add(&lvl3_button.base); UI::EndRow();
        UI::Add(&close_button.base); UI::EndRow();
    }
    UI::End();

    if (close_button.IsReleased())
        state.current_menu = GM_MAIN_MENU;
    if (lvl1_button.IsReleased()) {
        Game::ActualLevel = Game::plains;
        state.current_menu = GM_PLAYER_MENU;
    }
    if (lvl2_button.IsReleased()) {
        Game::ActualLevel = Game::plains;
        state.current_menu = GM_PLAYER_MENU;
    }
    if (lvl3_button.IsReleased()) {
        Game::ActualLevel = Game::plains;
        state.current_menu = GM_PLAYER_MENU;
    }

    lvl1_button.Draw();
    lvl2_button.Draw();
    lvl3_button.Draw();
    close_button.Draw();
}
void DoPlayerMenu()
{
    UI::Button player1_button = UI::Button("Rouge");
    UI::Button player2_button = UI::Button("Sniper");
    UI::Button player3_button = UI::Button("Healer");
    UI::Button player4_button = UI::Button("Warrior");
    UI::Button close_button = UI::Button("Return");

    UI::Begin();
    {
        UI::Add(&player1_button.base);
        UI::Add(&player2_button.base);
        UI::Add(&player3_button.base);
        UI::Add(&player4_button.base); UI::EndRow();
        UI::Add(&close_button.base); UI::EndRow();
    }
    UI::End();

    if (close_button.IsReleased())
        state.current_menu = GM_LEVEL_MENU;
    if (player1_button.IsReleased()) {
        Game::ActualPlayer = Game::PLAYER_TYPE_ROUGE;
        state.current_menu = GM_NONE;
        state.current_scene = GS_GAME;
    }
    if (player2_button.IsReleased()) {
        Game::ActualPlayer = Game::PLAYER_TYPE_SNIPER;
        state.current_menu = GM_NONE;
        state.current_scene = GS_GAME;
    }
    if (player3_button.IsReleased()) {
        Game::ActualPlayer = Game::PLAYER_TYPE_HEALER;
        state.current_menu = GM_NONE;
        state.current_scene = GS_GAME;
    }
    if (player4_button.IsReleased()) {
        Game::ActualPlayer = Game::PLAYER_TYPE_WARRIOR;
        state.current_menu = GM_NONE;
        state.current_scene = GS_GAME;
    }

    player1_button.Draw();
    player2_button.Draw();
    player3_button.Draw();
    player4_button.Draw();
    close_button.Draw();
}
void DoOptionsMenu()
{
    UI::Button close_button = UI::Button("Close");

    UI::Begin();
    {
        UI::Add(&close_button.base); UI::EndRow();
    }
    UI::End();

    if(close_button.IsReleased())
        state.current_menu = GM_MAIN_MENU;

    close_button.Draw();
}

void DoTitleScreenScene()
{
    if(state.current_menu == GM_NONE)
        state.current_menu = GM_MAIN_MENU;

    UI::SetPosition(GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f);
    UI::SetOrigin(0.5f, 0.5f);

    ClearBackground(Color{20, 20, 20});
    switch(state.current_menu)
    {
        case GM_MAIN_MENU: DoMainMenu(); break;
        case GM_JOIN_MENU: DoJoinMenu(); break;
        case GM_CONNECTING_MENU: DoConnectingMenu(); break;
        case GM_HOST_MENU: DoHostMenu(); break;
        case GM_LEVEL_MENU: DoLevelMenu(); break;
        case GM_OPTIONS_MENU: DoOptionsMenu(); break;
        case GM_PLAYER_MENU: DoPlayerMenu(); break;
        default: break;
    }
}

void DoPauseMenu()
{
    UI::Button options_button = UI::Button("Options");
    UI::Button return_button = UI::Button("Return");
    UI::Button exit_button = UI::Button("Exit");

    UI::Begin();
    {
        UI::Add(&options_button.base); UI::EndRow();
        UI::Add(&return_button.base);
        UI::Add(&exit_button.base); UI::EndRow();
    }
    UI::End();

    if(options_button.IsReleased())
        state.current_menu = GM_OPTIONS_MENU;

    if(return_button.IsReleased())
        state.current_menu = GM_NONE;

    if(exit_button.IsReleased())
    {
        state.current_scene = GS_TITLE_SCREEN;
        state.current_menu = GM_NONE;
    }

    options_button.Draw();
    return_button.Draw();
    exit_button.Draw();
}

void DoGameScene()
{
    static Game::World world;
    static Game::Entity *player;
    static Game::Input input;

    if(!state.game.world_initialised)
    {
        input = Game::Input();
        world.Clear();
        world.SetLevel(Game::ActualLevel);
        Texture2D character = LoadTexture(RESOURCE_PATH "/character.png");
        player = world.CreatePlayer(Game::ActualLevel.spawnpoint.x, Game::ActualLevel.spawnpoint.y,character,Game::ActualPlayer);

        state.game.world_initialised = true;
    }


    input.Update();

    float expected_delta_time = 1.0f / (float)GetFPS();
    float delta_time = GetFrameTime() > expected_delta_time ? expected_delta_time : GetFrameTime();

    /*
    if(input.shoot)
    {
        static float cooldown = 0;
        if(cooldown <= 0) {
            Game::ReferenceFrame rframe;
            rframe.position = player->rF.position + glm::vec2(player->width, player->height)*0.5f;
            rframe.velocity = glm::vec2(player->playerData.facing,1) * glm::vec2(200,50) + glm::vec2(player->rF.velocity.x, 0);
            rframe.angularVelocity = -360;
            rframe.rotation = 0;
            world.CreateBullet(rframe,5,Game::bulletTexture);
            cooldown = 0.5f;
        }
        cooldown -= delta_time;
    }
    */

    world.Update(&input, delta_time);

    Camera2D camera = {};
    camera.offset = Vector2{ GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };
    camera.zoom = 3.0f;
    camera.target.x = player->position.x;
    camera.target.y = player->position.y;
    camera.zoom = 4.0f;

    BeginMode2D(camera);
    ClearBackground(Color{25, 30, 40});
    world.Draw();
    EndMode2D();

    if(IsKeyPressed(KEY_ESCAPE))
    {
        if(state.current_menu == GM_NONE)
            state.current_menu = GM_MAIN_MENU;
        else
            state.current_menu = GM_NONE;
    }

    if(state.current_menu != GM_NONE)
    {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Color{0, 0, 0, 150});

        UI::SetPosition(GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f);
        UI::SetOrigin(0.5f, 0.5f);

        switch(state.current_menu)
        {
            case GM_MAIN_MENU: DoPauseMenu(); break;
            case GM_OPTIONS_MENU: DoOptionsMenu(); break;
            default: break;
        }
    }
}

int main(int, char**)
{
    SetTraceLogLevel(LOG_INFO);

    InitWindow(960, 540, "PI");
    SetTargetFPS(60);
    SetExitKey(KEY_END);

    Game::InitGameContent();
    UI::Init();

    while(!state.should_quit)
    {
        state.scene_changed = (state.current_scene != state.last_scene);
        state.last_scene = state.current_scene;
        state.menu_changed = (state.current_menu != state.last_menu);
        state.last_menu = state.current_menu;

        BeginDrawing();
        switch(state.current_scene)
        {
            case GS_TITLE_SCREEN: DoTitleScreenScene(); break;
            case GS_GAME: DoGameScene(); break;
            default: state.should_quit = true; break;
        }
        EndDrawing();

        if(WindowShouldClose()) state.should_quit = true;
    }

    CloseWindow();

    return 0;
}