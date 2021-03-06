#pragma once
#include <vector>
#include <algorithm>
#include <time.h>
#include <math.h>
#include <list>
#include <map>
#include "stdafx.h"
#include "GameData.h"
#include "Dodge.h"
#include <string>
#include "IniManager.h"
#include "DataString.h"
#include "InjectBot.h"
#include "Address_Game.h"
#include <cmath>
#include "GameDefine.h"
#include "SpellData.h"
#include "DataString.h"
#include "VMProtectSDK.h"
#include "AntiDebugger.h"
#include "Combo/SkillData.h"
#include "Combo/CommonsFunc.h"
#include "Combo/Lulu.h"
#include "Combo/Diana.h"
#include "Combo/Cho'Gath.h"
#include "Log.h"
//#include "Get_TickCount.h"
#include "HotKey.h"
#include "OrbWalker/OrbWalker.h"
#include "OrbWalker/OrbT_LHit.h"
#include "OrbWalker/OrbSafe.h"
//#include "OrbWalker/Prediction.h"

#include "SDK/OnEvent.h"

typedef void (*fPtrRESET_DATA_SPELL)(int);

typedef void (*callback_DrawLogicChamp)();
typedef void (*callback_OnBeforeAttack)(KNpc*);
typedef void (*callback_OnAfterAttack) (KNpc*, KNpc*);

bool Get_Exit();
void Set_LUA_Combo(bool nIsCombo = false);
void Set_LUA_Harass(bool nIsHarass = false);
void Set_LUA_ClearLane(bool nIsClearLane = false);
void Set_LUA_BasicAttack(bool nIsLuaBAOnly = false);
void Set_LUA_MoveOnly(bool nIsLuaMoveOnly = false);
int Get_Key_Code();

void RecheckLoginMsg(bool isSuccess = false);
void MOVE_TO(float &posX, float &posY, bool triggerEvent = true);
//void MOVE_TO_RCLICK(float &posX, float &posY);
//void MOVE_TO_EVENT(float &posX, float &posY);			//evtPlayerMoveClick_MouseTriggered
//int  GET_DELAYMOVE_RANDOM();
void  DISABLE_SETECTOR();
DWORD GET_ENEMY_ID_SELECTED();

void BlockMove();
void UnBlockMove();
void Block_RightClick_FromUser();
void UnBlock_RightClick_FromUser();

void SEND_TEXT_SYSTEM(char *);
void SEND_TEXT_SYSTEM(char *szTextMsg, int xxxx);	// sendText nay bat buoc

void SEND_CHAT2PLAYER(char *);

bool IS_CHATTING();

void ClearScreen();

HMODULE GetGameModule();
HMODULE MyGetModuleHandle(const char *title);
float Get_Delay_BasicAttack();
void NormalAttack(KNpc *pNpc, bool triggerEvent = true);

bool IsAllyMinionInRange(float nRange);
bool Is_EnemyMinion_Around_NPC(KNpc* pNpc, float nRange);

int CountEnemyMinionInRange(float nRange);
bool IsEnemyTurretInRange(float nRange);
int CountEnemyPlayerInRange(float nRange);
int Count_EnemyPlayer_Around_NPC(KNpc* pNpc, float nRange);
int Count_EnemyMinion_Around_NPC(KNpc* pNpc, float nRange);
int Count_EnemyTurret_Around_NPC(KNpc* pNpc, float nRange);
int Count_AllyPlayer_Around_NPC(KNpc* pNpc, float nRange);
int Count_AllyMinion_Around_NPC(KNpc* pNpc, float nRange);
KNpc* GET_VISION_WARD(float nRange, bool isMat_Tim = true);
KNpc* SearchNearestAllyTurret(float nRange);
KNpc* SearchNearestAllyPlayer(float nRange);
KNpc* SearchFarestAllyPlayer(float nRange);
KNpc* SearchNearestNPC(float nRange);
KNpc* SearchEnemyMinHP(float nRange);
KNpc* SearchNearestNPC_Minion(float nRange);
KNpc* SearchFarestNPC_Minion(float nRange);
KNpc* SearchFarestNPC_AllyMinion(float nRange);
KNpc* SearchNPC2KillFastest(float nRange, bool nAA = false);
KNpc* SearchEnemyNPC_CC_AroundNPC(KNpc* pNpc, float nRange);
KNpc* Search_EnemyMinion_Around_NPC(KNpc* pNpc, float nRange);
float GetDamageReceive(KNpc* pPlayer, KNpc* pEnemy);
KNpc* SearchNPC2KillFastest_AP(float nRange);
float GetDamageReceive_AP(KNpc* pPlayer, KNpc* pEnemy);

bool IsCanSee(KNpc *pEnemy);
bool IS_ADC(KNpc *pNpc);

bool Can_NormalAttack();
KPos* GetMousePos();
void SetMousePos(KPos *pos);

bool IsPlayingWithBot();
bool IsGameReady();
KNpc* GetPlayer();
float Distance_3D(float x1, float y1, float z1, float x2, float y2, float z2);
float Distance_3D(KPos start, KPos end);
float Distance_2D(float x1, float y1, float x2, float y2);
float Distance_2D(KPos start, KPos end);

void PrintBuffer(int index);
void RESET_BUFFER_SPELL(int index);
void RESET_BUFFER_SPELL_2(int index);
void RESET_BUFFER_SPELL_2_QUA_3s();
void RESET_BUFFER_SPELL_SPECIAL(int index);
void SET_IsEvaded_BUFFER_SPELL_SPECIAL(int index);
void SET_BUFFER_SPELL();

void SET_CONDITION_BUFF_2(KNpc* unit, char * animation);
void SET_BUFFER_SPELL_2(KNpc* unit, KCastSpell * pCast);
void AUTO_DODGE_2();

void PrintBaseTime(int index);
void RESET_BASE_TIME(int index);
void SET_BASE_TIME_SPELL();
float GetTimeGame_OLD();
float GetTimeGame();
float GetTimeGame_Dubi();
void AUTO_DODGE_BASETIME();
void WINAPI PRINT_SKILL();

void WINAPI AUTO_DODGE();

int NghiemHePTBacNhat(KPos &pos, float &a1,float b1,float &c1,float a2,float b2,float &c2);
bool DodgeMissileLine(KPos* pos1,KPos *pos2, float radius, float maxDist, KBuffer_Spell* pBufferSpell = 0, float timeFlying = 350., bool flag = true, fPtrRESET_DATA_SPELL fPtrRESET = RESET_BUFFER_SPELL);
bool WINAPI DodgeMissileLine_45do(KPos* pos1,KPos* pos2, float radius, float maxDist, KBuffer_Spell* pBufferSpell = 0, float timeFlying = 350., bool flag = true, fPtrRESET_DATA_SPELL fPtrRESET = RESET_BUFFER_SPELL);
bool WINAPI DodgeMissileLine_45do_TIEN(KPos* pos1,KPos* pos2, float radius, float maxDist, KBuffer_Spell* pBufferSpell = 0, float timeFlying = 350., bool flag = true, fPtrRESET_DATA_SPELL fPtrRESET = RESET_BUFFER_SPELL);
bool IsNPCMissileInLine(KNpc *pNpc, KPos *pos1,KPos *pos2, float radius, float maxDist, bool flag = true);
bool IsNPCInCircle(KNpc *pNpc, KPos *pos1, float radius);
bool IsINSIDE_TAMGIAC(KPos *pos_diem, KPos *pos_1, KPos *pos_2, KPos *pos_3) ;
bool WINAPI DodgeCircle(KPos* pos1, float radius, KBuffer_Spell* pBufferSpell = 0, float timeFlying = 350, fPtrRESET_DATA_SPELL fPtrRESET = RESET_BUFFER_SPELL, bool flagFlash = false);
bool DodgeCone(KPos* startPoint,KPos* endPoint, float radius_cone, float radius_line, float range, KBuffer_Spell* pBufferSpell = 0, float timeFlying = 150., bool flag = true, fPtrRESET_DATA_SPELL fPtrRESET = RESET_BUFFER_SPELL);

bool IS_INCIRCLE(KNpc *pNpc, KPos *posTam, KBuffer_Spell* pBufferSpell);
int COUNT_NPC_INCIRCLE(KPos *posTam, KBuffer_Spell *pBufferSpell);
bool EVADE_CIRCLE(KPos* posTam, KBuffer_Spell* pBufferSpell, fPtrRESET_DATA_SPELL fPtrRESET = RESET_BUFFER_SPELL);
bool EVADE_CIRCLE_SONGSONG(KPos* posTam, KBuffer_Spell* pBufferSpell, fPtrRESET_DATA_SPELL fPtrRESET = RESET_BUFFER_SPELL);
bool EVADE_LINE(KPos* startPoint, KPos* endPoint, KBuffer_Spell* pBufferSpell, fPtrRESET_DATA_SPELL fPtrRESET = RESET_BUFFER_SPELL, bool nSpecial = false);
bool DODGE_LINE_NEW(KPos* startPoint, KPos* endPoint, KBuffer_Spell* pBufferSpell, fPtrRESET_DATA_SPELL fPtrRESET = RESET_BUFFER_SPELL, bool nSpecial = false);
bool EVADE_THEO_MOUSE(KPos* startPoint, KPos* endPoint, KBuffer_Spell* pBufferSpell, fPtrRESET_DATA_SPELL fPtrRESET = RESET_BUFFER_SPELL, bool nSpecial = false);
bool EVADE_LINE_45DO(KPos* startPoint,KPos* endPoint, KBuffer_Spell* pBufferSpell, int nTienLui, fPtrRESET_DATA_SPELL fPtrRESET = RESET_BUFFER_SPELL, bool nSpecial = false);
bool IS_INLINE(KNpc *pNpc, KPos *startPoint,KPos *endPoint, KBuffer_Spell *pBufferSpell, bool isHasMinion = false);
int COUNT_NPC_INLINE(KPos *startPoint,KPos *endPoint, KBuffer_Spell *pBufferSpell, float nKhoangCach = 10.0);
bool IsShouldDodge(KPos *startPoint,KPos *endPoint, KBuffer_Spell *pBufferSpell);
bool IsShouldDodge_45Do(KPos* startPoint,KPos* endPoint, KBuffer_Spell* pBufferSpell, int nTienLui);
bool IsShouldCircle(KPos* posTam, KBuffer_Spell* pBufferSpell);
int CheckMousePos(KPos startPoint,KPos endPoint, KBuffer_Spell pBufferSpell);
bool GET_antoanPos(KPos startPoint, KPos endPoint, KBuffer_Spell pBufferSpell, KPos &antoanPos, float nDoRong);
KPos* GET_EVADE_EXTRA_THEO_MOUSE(KPos startPoint, KPos endPoint, KBuffer_Spell pBufferSpell);
KPos* GET_EVADE_EXTRA(KPos startPoint, KPos endPoint, KBuffer_Spell pBufferSpell, float nDoDai);

bool WINAPI GIAO_DIEM_VUONG_GOC(KPos& pos_vuonggoc, KPos pos1,KPos pos2, KPos pos3);
bool WINAPI TIM_DIEM_CACH_1DIEM(KPos& pos_diem1, KPos& pos_diem2, KPos pos1,KPos pos2, float radius);
bool WINAPI TIM_2DIEM_CACH_1DIEM(KPos* pos_diem1, KPos* pos_diem2, KPos* pos1, KPos* pos2, float radius);
void WINAPI GiaiPT_Bac2(float nghiem1, float nghiem2, float a, float b, float c);
void WINAPI GiaoDiem_DuongTronVaDuongThang(KPos* pos_diem1, KPos* pos_diem2, KPos* pos1, KPos* pos2, KPos* pos3, float radius);

int CanCastSpell2(int STT_SKILL);
int CanCastSpell4Lua(int STT_SKILL);

bool DO_FLASH(KPos* maxDest);
bool DO_THIEU_DOT(KNpc* pEnemy, bool flag_kocantinhmau = false);
bool DO_IGNITE(KNpc* pEnemy);
bool DO_HOI_MAU_SKILL();
void AutoTHIEU_DOT();
bool DO_RECALL_SKILL();
void PrintCastSpell(KNpc *pNpc);
void StrReplace(char * inputStr, char needReplace, char toReplace);

bool compare_nocase(const char* first, const char* second);
bool compare_case (const char* first, const char* second);
class CTextEncryptMgr
{
private:
	CTextEncryptMgr();
	~CTextEncryptMgr();
	std::vector<char*> m_TextDecrypted;
	static CTextEncryptMgr* s_Instance;
	char* FindTextDecrypted(std::string &str2Find);
public:
	static CTextEncryptMgr* GetInstance();
	char* GetTextDecrypted(int *inputArr);
};
char* ArrInt2Str(int * inputArr);

wchar_t* C2W(const char *);
char* W2C(const wchar_t *);
void HackZoom_Tang();
void HackZoom_Giam();
void MOD_SKIN(DWORD szNameHero, int skinID, KNpc* pNPC = 0);
bool IsWall(KPos* pPos);
bool IsWall(Vec3D* vPos);

float Get_AA_Delay(KNpc* pNPC = 0);
float Get_AA_Animate_Delay(KNpc* pNPC = 0);
DWORD Get_BasicAttack(KNpc* pNPC = 0);

//----------------------------------------
DWORD GET_PMOVE_NEW(KNpc* pNpc);
Vec3D GET_START_POS(KNpc* pNpc);
Vec3D GET_END_POS(KNpc* pNpc);

float GET_CDBA_NEW(KNpc* pNpc);
bool IS_OBJ_DELETED(KNpc* Obj, char* szFuck = "default");			//dung ham nay hay hon, neu result=true ---> doi tuong da~ bi xoa roi, ko ton tai nua
bool IS_HERO(KNpc* pNpc);
bool IS_MINION(KNpc* pNpc);
bool IS_TURRET(KNpc* pNpc);
bool IS_NEXUS(KNpc* pNpc);
bool IS_INHIBITOR(KNpc* pNpc);
int	 OBJECT_TYPE(KNpc* obj); // 0x8000 = missile, 0x1000 = Hero, 0x800 = Minion, 0x2000 = Turret ....
bool IS_OBJECT_TYPE(KNpc* obj, DWORD objType);

bool IS_MOVE_NEW(KNpc* pNpc);
bool IS_CAN_SEE(KNpc* pNpc);	
bool IS_DEAD(KNpc* pNpc);		
bool IS_ALIVE(KNpc* pNpc);			//[NEW] ---> dung` khi Obj ton tai, con song ...
bool IS_TARGET_ABLE(KNpc* pNpc);	//[NEW] ---> dung` khi quat' dc Obj

DWORD GET_BARSOURCE(KNpc* pNpc);

Vec3D GET_REAL_FACING_POS(KNpc* pNpc);

//----------------------------------------

void Search_All_Champ(int (&nResult)[10]);
void Search_All_Unit_Around_NPC(KNpc* pNPC, float nRange, DWORD (&nResult)[300]);
void Search_All_Object_Around_NPC(KNpc* pNPC, float nRange, DWORD (&nResult)[500]);
int  GetPingGame();
bool IsMoving(KNpc* pNpc);
bool IsDashing(KNpc* pNpc);
bool IsDashGravity(KNpc* pNpc);

int GetOffsetSpellBook();
DWORD GetObjectByID(DWORD nID);
DWORD GetObjectByNetworkID(DWORD nNetworkID);

float GetBoundingRadius(KNpc* pNpc);

bool  IsValidObj(KNpc* pUnit);		//---- co the dung cho AllObject
bool  IsValidTarget(KNpc* pUnit, float nRange = 25000, bool checkTeam = true, Vec3D* from = 0);
bool  IsUnitInList(KNpc* pUnit);	//---- chi dung cho unitAttackAble

float GetDmg_TurretHitMinion(KNpc* turret, KNpc* minion);
float calcDamage(KNpc* enemy, int dmgType, float dmgRaw);
KPathOutWayPoint* CreatPathWays(KNpc* unit, Vec3D& start, Vec3D& destination, bool smoothPath = false);

int CountMinionInLine(Vec3D pointstart, Vec3D pointend, float width, float range);
KNpc * GetBestMininonLineFarm(float width, float range);
KNpc * GetBestMininonCircleFarm(float width, float range);
bool IsSpellMinionCollision(KNpc* pPlayer, KNpc* target, float range, float width);
bool AFacingB(KNpc* A, KNpc* B, float angle);
bool Face2Face(KNpc* A, KNpc* B,float angle);
//=========================================================================================================
//=== For Prediction ======================================================================================
class PredictionInput
{
private:
	Vec3D _from;
	Vec3D _rangeCheckFrom;

public:
	bool Aoe;// = false;
	bool Collision;// = false;
	int  CountCollision;
	CollisionableObjects CollisionObjects[5];
	float Delay;
	float Radius;// = 1.0f;
	float Range;//  = 22000.0f;
	float Speed;//  = 22000.0f;
	SkillshotType Type;// = SkillshotLine;
	KNpc* Unit;// = GetPlayer();
	bool USE_BOUNDING_RADIUS;// = true;

	PredictionInput()
	{
		Aoe = false;
		Collision = false;
		CountCollision = 1;
		CollisionObjects[0] = Minions;
		CollisionObjects[1] = YasuoWall;
		CollisionObjects[2] = None_1;
		CollisionObjects[3] = None_2;
		CollisionObjects[4] = None_3;
		Delay  = 0.25f;
		Radius = 1.0f;	  //50.0f
		Range  = FLT_MAX; //20000.0f;
		Speed  = FLT_MAX; //20000.0f;
		Type   = SkillshotLine;
		Unit   = GetPlayer();
		USE_BOUNDING_RADIUS = true;
	}

	Vec3D GetFrom()
	{
		Vec3D defaut;
		if(GetPlayer()) defaut.x = GetPlayer()->ServerPosition.x, defaut.y = 0, defaut.z = GetPlayer()->ServerPosition.z;
		return _from.IsValid() ? _from : defaut;
	}
	void SetFrom(Vec3D value) { _from.x = value.x;_from.y = 0; _from.z = value.z;}

	Vec3D GetRangeCheckFrom()
	{
		Vec3D defaut;
		if(GetPlayer()) defaut.x = GetPlayer()->ServerPosition.x, defaut.y = 0, defaut.z = GetPlayer()->ServerPosition.z;
		return _rangeCheckFrom.IsValid()
			? _rangeCheckFrom
			: (GetFrom().IsValid() ? GetFrom() : defaut);
		//return _rangeCheckFrom;
	}
	void SetRangeCheckFrom(Vec3D value) { _rangeCheckFrom.x = value.x; _rangeCheckFrom.y = 0; _rangeCheckFrom.z = value.z;}

	float RealRadius()
	{
		return USE_BOUNDING_RADIUS ? Radius + GetBoundingRadius(Unit) - 5.02f : Radius;		// giam 5.02f BoundingRadius cho chuan hon ti
	}
};

//-----------------------------------------------------------------------------------------------
class PredictionOutput
{
private:
	Vec3D _castPosition;
	Vec3D _unitPosition;

public:
	std::vector<KNpc*> AoeTargetsHit;
	std::vector<KNpc*> CollisionObjects;
	HitChance Hitchance;// = HitChance.Impossible;
	int _aoeTargetsHitCount;

	PredictionInput Input;

	PredictionOutput()
	{
		Hitchance = Impossible;
		_aoeTargetsHitCount = 0;
	}

	int AoeTargetsHitCount()
	{
		return max(_aoeTargetsHitCount, (int)AoeTargetsHit.size());
		//return _aoeTargetsHitCount;
	}

	Vec3D GetCastPosition()
	{
		Vec3D defaut;
		if(Input.Unit) defaut.x = Input.Unit->ServerPosition.x, defaut.y = 0, defaut.z = Input.Unit->ServerPosition.z;
		return _castPosition.IsValid() ? _castPosition : defaut;
	}
	void SetCastPosition(Vec3D value){_castPosition.x = value.x; _castPosition.y = 0; _castPosition.z = value.z;}

	Vec3D GetUnitPosition()
	{
		Vec3D defaut;
		if(Input.Unit) defaut.x = Input.Unit->ServerPosition.x, defaut.y = 0, defaut.z = Input.Unit->ServerPosition.z;
		return _unitPosition.IsValid() ? _unitPosition : defaut;
	}
	void SetUnitPosition(Vec3D value){_unitPosition.x = value.x; _unitPosition.y = 0; _unitPosition.z = value.z;}
};

//-----------------------------------------------------------------------------------------------
class StoredPath
{
public:
	std::vector<Vec3D> Path;
	float Tick;

	Vec3D GetEndPoint()
	{
		return Path.back();
	}
	Vec3D GetStartPoint()
	{
		return Path.front();
	}
	float GetTime()
	{
		return ((float)GetTickCount() - Tick) / 1000;
	}
	int GetWaypointCount()
	{
		return (int)Path.size();
	}
};


class GameObjectIssueOrderEventArgs
{
public:
	int		Order;
	Vec3D	TargetPosition;
	KNpc*	Target;
	bool	AttackMove;
	bool	Process;

	GameObjectIssueOrderEventArgs()
	{
		Order		= 2;
		//Vec3D	TargetPosition;
		Target		= 0;
		AttackMove	= false;
		Process		= true;

	}
};













//============================================================================================================|
//================================ CHI DUNG CHECK LOI~ THOI, KO CHO VAO CODE DC ==============================:
//////#ifndef _MSC_VER
//////#define NOEXCEPT noexcept
//////#else
//////#define NOEXCEPT
//////#endif
//////enum AircraftError
//////{
//////	WingsOnFire = 1,
//////	WingBroken = 2,
//////	NoRunway = 3,
//////	Crahed = 4
//////};
//////class AircraftException : public std::exception
//////{
//////public:
//////	AircraftException(const char* errMessage, AircraftError errorType) :m_ErrMessage(errMessage),
//////		m_ErrorType(errorType){}
//////	// overriden what() method from exception class
//////	const char* what() const NOEXCEPT override{ return m_ErrMessage; }
//////	AircraftError GetError() { return m_ErrorType; }
//////private:
//////	const char* m_ErrMessage;
//////	AircraftError m_ErrorType;
//////};
//////throw AircraftException("CRASHED", AircraftError::Crahed);
//////catch (AircraftException& e){..x..}

//////class myexception: public std::exception			//<---Dung` dc
//////{
//////	virtual const char* what() const throw()
//////	{
//////		return "My exception happened-111";
//////	}
//////} myex;


/*
#include <cstdlib>
#include <string>
#include <stdexcept>
#include <iostream>
#include <sstream>
using namespace std;
template<class EX>
class traced_error : virtual public std::exception, virtual public EX
{
public:
	traced_error(const std::string& file, int line, const EX& ex)
		:   EX(ex),
		line_(line),
		file_(file)
	{       
	}

	const char* what() const
	{
		std::stringstream ss;
		static std::string msg;
		ss << "." << file_ << " . " << line_ << "." << EX::what();
		msg = ss.str().c_str();
		return msg.c_str();
	}

	int line_;
	std::string file_;
};

template<class EX> traced_error<EX> make_traced_error(const std::string& file, int line, const EX& ex)
{
	return traced_error<EX>(file, line, ex);
}


class my_exception : virtual public std::exception
{
public:
	my_exception() {};

	const char* what() const
	{
		return ".";
	}
};

#define throwx(EX) (throw make_traced_error(__FILE__,__LINE__,EX))
*/


//////#include <sstream>
//////class my_exception : public std::runtime_error {
//////	std::string msg;
//////public:
//////	my_exception(const std::string &arg, const char *file, int line) :
//////	  /////std::runtime_error(std::string(file)+":"+std::to_st(line)+":"+arg)
//////
//////	  std::runtime_error(arg)
//////	  {
//////		  std::ostringstream out;
//////		  out << file << ":" << line << ": " << arg;
//////		  msg = out.str();
//////	  }
//////
//////	  ~my_exception() throw() {}
//////	  const char *what() const throw() {
//////		  return msg.c_str();
//////	  }
//////};
//////#define throw_line(arg) throw my_exception(arg, __FILE__, __LINE__);
//////
//////void f() {
//////	throw_line("Oh no!");
//////}