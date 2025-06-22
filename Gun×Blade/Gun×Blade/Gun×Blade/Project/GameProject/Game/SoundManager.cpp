#include "SoundManager.h"

SoundManager::SoundManager()
	:Base(eSound)
{
	//BGM
	SOUND("TitleBGM")->Load("BGM/MusMus-BGM-063.wav");
	SOUND("TitleBGM")->Volume(0.3);
	SOUND("FieldBGM1")->Load("BGM/Grenade.wav");
	SOUND("FieldBGM1")->Volume(0.3);
	SOUND("BossBGM")->Load("BGM/MusMus-BGM-122.wav");
	SOUND("BossBGM")->Volume(0.3);

	//SE
//プレイヤー関連
	SOUND("walkL")->Load("SE/Player/WalkL.wav");
	SOUND("walkL")->Volume(1.0);

	SOUND("walkR")->Load("SE/Player/WalkR.wav");
	SOUND("walkR")->Volume(1.0);

	SOUND("runL")->Load("SE/Player/RunL.wav");
	SOUND("runL")->Volume(1.0);

	SOUND("runR")->Load("SE/Player/RunR.wav");
	SOUND("runR")->Volume(1.0);

	SOUND("attack")->Load("SE/Player/attack.wav");
	SOUND("attack")->Volume(1.0);
	//突き刺し
	SOUND("stab")->Load("SE/Player/stab.wav");
	SOUND("stab")->Volume(1.0);

	SOUND("bom")->Load("SE/Player/bom.wav");
	SOUND("bom")->Volume(1.0);

	SOUND("gaugeattack")->Load("SE/Player/gaugeattack.wav");
	SOUND("gaugeattack")->Volume(1.0);

	SOUND("parry")->Load("SE/Player/parry.wav");
	SOUND("parry")->Volume(1.0);
	//構える
	SOUND("gunstand")->Load("SE/Player/gunstand.wav");
	SOUND("gunstand")->Volume(1.0);

	SOUND("shot")->Load("SE/Player/shot.wav");
	SOUND("shot")->Volume(1.0);

	//薬莢が落ちる（複数）
	SOUND("reloads")->Load("SE/Player/reloads.wav");
	SOUND("reloads")->Volume(1.0);

	//ジャスト回避成功
	SOUND("slowstrat")->Load("SE/Player/slowstrat.wav");
	SOUND("slowstrat")->Volume(1.0);

//敵関連

	SOUND("enemyattck")->Load("SE/Enemy/enemyattck.wav");
	SOUND("enemyattck")->Volume(1.0);
	//軽めの攻撃音
	SOUND("bossattack")->Load("SE/Boss/bossattack.wav");
	SOUND("bossattack")->Volume(1.0);
	//重めの攻撃音
	SOUND("bossattack2")->Load("SE/Boss/bossattack2.wav");
	SOUND("bossattack2")->Volume(1.0);
	//咆哮
	SOUND("roar")->Load("SE/Boss/roar.wav");
	SOUND("roar")->Volume(1.0);
	//ボス突進時駆け出し音
	SOUND("rushstart")->Load("SE/Boss/rushstart.wav");
	SOUND("rushstart")->Volume(1.0);
	//突進ヒット
	SOUND("rushhit")->Load("SE/Boss/rushhit.wav");
	SOUND("rushhit")->Volume(1.0);
	//柱崩れる
	SOUND("collapse")->Load("SE/Boss/collapse.wav");
	SOUND("collapse")->Volume(1.0);
	//死亡時の炎
	SOUND("death")->Load("SE/Enemy/death.wav");
	SOUND("death")->Volume(1.0);

//タイトル関連
	//決定音
	SOUND("select")->Load("SE/Select/select.wav");
	SOUND("select")->Volume(1.0);
	//チュートリアル次に進む
	SOUND("next")->Load("SE/Select/next.wav");
	SOUND("next")->Volume(1.0);
	//選択肢移動
	SOUND("cursor")->Load("SE/Select/cursor.wav");
	SOUND("cursor")->Volume(1.0);
	//クリア
	SOUND("gameclear")->Load("SE/Select/gameclear.wav");
	SOUND("gameclear")->Volume(1.0);

}

void SoundManager::Play(const char* name)
{
	SOUND(name)->Play();
}

void SoundManager::Loop(const char* name)
{
	SOUND(name)->Play(true);
}

void SoundManager::Stop(const char* name)
{
	SOUND(name)->Stop();
}
