#ifndef CONSTANT_H
#define CONSTANT_H

namespace Const {
	struct Rouge_Data
	{
		float JUMP_HEIGHT = 12.0f;
		int BASE_HEALTH = 50;
		float BASE_SPEED = 110.0f;
	};
	struct Sniper_Data
	{
		float JUMP_HEIGHT = 13.5f;
		int BASE_HEALTH = 70;
		float BASE_SPEED = 90.0f;
	};
	struct Healer_Data
	{
		float JUMP_HEIGHT = 12.0f;
		int BASE_HEALTH = 60;
		float BASE_SPEED = 100.0f;
	};
	struct Warrior_Data
	{
		float JUMP_HEIGHT = 14.0f;
		int BASE_HEALTH = 30;
		float BASE_SPEED = 120.0f;
	};
	struct Player_Data {
		Rouge_Data ROUGE;
		Sniper_Data SNIPER;
		Healer_Data HEALER;
		Warrior_Data WARRIOR;
		float SHOT_COOLDOWN = 0.6f;
		float DAMAGE_COOLDOWN = 1.0f;
		float DASH_SPEED = 10.0f;
		float DASH_COOLDOWN = 1.0f;
		float DASH_BRAKESPEED = 100.0f;
		float FALL_SPEED = 30.0f;
	};
	struct Entity_Data {
		float MAX_FRAME_TIME = 0.25f;
		float MOVING_TILE_MOVE_SPEED = 20.0f;
		int DAMAGING_TILE_DAMAGE = 30;
		int DESTROY_TILE_HEALTH = 50;
		float BULLET_LIFETIME = 2.0f;
		int BULLET_DAMAGE = 10;
		float BULLET_VELOCITY_X = 200.0f;
		float BULLET_VELOCITY_Y = -100.0f;
		float INTERACTIVE_STATE_COOLDOWN = 0.25f;
	};
	struct Enemy_Data {
		float VELOCITY_X = 10.0f;
		int DAMAGE = 15;
		float SHOT_COOLDOWN = 2.0f;
		float JUMP_HEIGHT = 12.0f;
		float JUMP_COOLDOWN = 4.0f;
		float MOVE_SPEED = 50.0f;
		float FALL_SPEED = 30.0f;
	};
	extern Player_Data PLAYER;
	extern Entity_Data ENTITY;
	extern Enemy_Data ENEMY;




}
#endif