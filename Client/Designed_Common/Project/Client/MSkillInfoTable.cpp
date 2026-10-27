//----------------------------------------------------------------------
// MSkillInfoTable.h
//----------------------------------------------------------------------
// ���� �׸��� SkillInfo.inf�� ���ؼ� �������� �޴� ���̹Ƿ�
// �Ű澵 �ʿ䰡 ����.
//		Name(��� �̸�)
//		LearnLevel(��� Blood Bible)
//		MP(��� MP)
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "MSkillManager.h"

#ifdef __INIT_INFO__
#include "MActionInfoTable.h"
#endif

//----------------------------------------------------------------------
//
//						MSkillInfoTable
//
//----------------------------------------------------------------------
MSkillInfoTable::MSkillInfoTable() {
  // �Blood Bible��ŭ..
  CTypeTable<SKILLINFO_NODE>::Init(MIN_RESULT_ACTIONINFO);

#ifndef __GAME_CLIENT__

  //----------------------------------------------------------------------
  // �ϴ� ��� skill�� ���� defaultBlood Bible
  //----------------------------------------------------------------------
  for (int i = 0; i < MIN_RESULT_ACTIONINFO; i++) {
#ifdef __INIT_INFO__
    m_pTypeInfo[i].Set(0, (*g_pActionInfoTable)[i].GetName(), 0, 0, 0,
                       "Not Defined");
#else
    m_pTypeInfo[i].Set(0, "Not defined", 0, 0, 0, "Not Defined");
#endif
    m_pTypeInfo[i].UnSetPassive();
    m_pTypeInfo[i].UnSetActive();
    m_pTypeInfo[i].SetSkillStep(SKILL_STEP_NULL);
    m_pTypeInfo[i].SetLearnLevel(-1); // �ʱ�ȭ
    m_pTypeInfo[i].SetSlayerSkill();
  }

  int sid = 0;

  //----------------------------------------------------------------------
  // �˰迭
  //----------------------------------------------------------------------
  m_pTypeInfo[SKILL_DOUBLE_IMPACT].Set(sid++, "Double Impact", 400, 100, 0,
                                       "Double Impact");
  m_pTypeInfo[SKILL_DOUBLE_IMPACT].SetMP(5);
  m_pTypeInfo[SKILL_DOUBLE_IMPACT].SetLearnLevel(5);
  m_pTypeInfo[SKILL_DOUBLE_IMPACT].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_DOUBLE_IMPACT].AddNextSkill(SKILL_TRIPLE_SLASHER);

  m_pTypeInfo[SKILL_TRIPLE_SLASHER].Set(sid++, "Triple Slasher", 200, 130, 1,
                                        "Triple Slasher");
  m_pTypeInfo[SKILL_TRIPLE_SLASHER].SetMP(8);
  m_pTypeInfo[SKILL_TRIPLE_SLASHER].SetLearnLevel(10);
  m_pTypeInfo[SKILL_TRIPLE_SLASHER].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_TRIPLE_SLASHER].AddNextSkill(SKILL_SWORD_MASTERY);

  m_pTypeInfo[SKILL_SWORD_MASTERY].Set(sid++, "Sword Mastery", 200, 130, 154,
                                       "Sword Mastery");
  m_pTypeInfo[SKILL_SWORD_MASTERY].SetMP(8);
  m_pTypeInfo[SKILL_SWORD_MASTERY].SetPassive();
  m_pTypeInfo[SKILL_SWORD_MASTERY].SetLearnLevel(15);
  m_pTypeInfo[SKILL_SWORD_MASTERY].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_SWORD_MASTERY].AddNextSkill(SKILL_DANCING_SWORD);

  m_pTypeInfo[SKILL_DANCING_SWORD].Set(sid++, "Dancing Sword", 700, 190, 4,
                                       "Dancing Sword");
  m_pTypeInfo[SKILL_DANCING_SWORD].SetMP(12);
  m_pTypeInfo[SKILL_DANCING_SWORD].SetLearnLevel(20);
  m_pTypeInfo[SKILL_DANCING_SWORD].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_DANCING_SWORD].AddNextSkill(SKILL_THUNDER_SPARK);
  //	m_pTypeInfo[SKILL_DANCING_SWORD].AddNextSkill( SKILL_THUNDER_SPARK );

  // 2004,4,30,sobeit add start
  //	m_pTypeInfo[SKILL_FABULOUS_SOUL].Set(sid++, "Fabulous Soul", 600, 160,
  //382,"�к淯�� �ҿ�"); 	m_pTypeInfo[SKILL_FABULOUS_SOUL].SetMP( 0 );
  //	m_pTypeInfo[SKILL_FABULOUS_SOUL].SetPassive();
  //	m_pTypeInfo[SKILL_FABULOUS_SOUL].SetLearnLevel( 23 );
  //	m_pTypeInfo[SKILL_FABULOUS_SOUL].SetSkillStep( SKILL_STEP_APPRENTICE );
  //	m_pTypeInfo[SKILL_FABULOUS_SOUL].AddNextSkill( SKILL_THUNDER_SPARK );
  // 2004,4,30,sobeit add end

  m_pTypeInfo[SKILL_THUNDER_SPARK].Set(sid++, "Thunder Spark", 600, 160, 2,
                                       "Thunder Spark");
  m_pTypeInfo[SKILL_THUNDER_SPARK].SetMP(10);
  m_pTypeInfo[SKILL_THUNDER_SPARK].SetLearnLevel(25);
  m_pTypeInfo[SKILL_THUNDER_SPARK].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_THUNDER_SPARK].AddNextSkill(SKILL_SHIELD_MASTERY);

  m_pTypeInfo[SKILL_SHIELD_MASTERY].Set(sid++, "Shield Mastery", 200, 130, 155,
                                        "Shield Mastery");
  m_pTypeInfo[SKILL_SHIELD_MASTERY].SetMP(8);
  m_pTypeInfo[SKILL_SHIELD_MASTERY].SetPassive();
  m_pTypeInfo[SKILL_SHIELD_MASTERY].SetLearnLevel(30);
  m_pTypeInfo[SKILL_SHIELD_MASTERY].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_SHIELD_MASTERY].AddNextSkill(SKILL_CROSS_COUNTER);

  m_pTypeInfo[SKILL_CROSS_COUNTER].Set(sid++, "Cross Counter", 400, 210, 5,
                                       "Cross Counter");
  m_pTypeInfo[SKILL_CROSS_COUNTER].SetMP(15);
  m_pTypeInfo[SKILL_CROSS_COUNTER].SetLearnLevel(35);
  m_pTypeInfo[SKILL_CROSS_COUNTER].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_CROSS_COUNTER].AddNextSkill(SKILL_RAINBOW_SLASHER);

  m_pTypeInfo[SKILL_RAINBOW_SLASHER].Set(sid++, "Rainbow Slasher", 200, 160, 3,
                                         "Rainbow Slasher");
  m_pTypeInfo[SKILL_RAINBOW_SLASHER].SetMP(10);
  m_pTypeInfo[SKILL_RAINBOW_SLASHER].SetLearnLevel(40);
  m_pTypeInfo[SKILL_RAINBOW_SLASHER].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_RAINBOW_SLASHER].AddNextSkill(SKILL_FLASH_SLIDING);

  m_pTypeInfo[SKILL_FLASH_SLIDING].Set(sid++, "Flash Sliding", 300, 240, 6,
                                       "Flash Sliding");
  m_pTypeInfo[SKILL_FLASH_SLIDING].SetMP(20);
  m_pTypeInfo[SKILL_FLASH_SLIDING].SetLearnLevel(45);
  m_pTypeInfo[SKILL_FLASH_SLIDING].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_FLASH_SLIDING].AddNextSkill(SKILL_SNAKE_COMBO);

  m_pTypeInfo[SKILL_SNAKE_COMBO].Set(sid++, "Snake Combo", 700, 190, 8,
                                     "Snake Combo");
  m_pTypeInfo[SKILL_SNAKE_COMBO].SetMP(12);
  m_pTypeInfo[SKILL_SNAKE_COMBO].SetLearnLevel(50);
  m_pTypeInfo[SKILL_SNAKE_COMBO].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_SNAKE_COMBO].AddNextSkill(SKILL_SWORD_WAVE);

  m_pTypeInfo[SKILL_SWORD_WAVE].Set(sid++, "Sword Wave", 500, 240, 9,
                                    "Sword Wave");
  m_pTypeInfo[SKILL_SWORD_WAVE].SetMP(20);
  m_pTypeInfo[SKILL_SWORD_WAVE].SetLearnLevel(55);
  m_pTypeInfo[SKILL_SWORD_WAVE].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_SWORD_WAVE].AddNextSkill(SKILL_SWORD_RAY);

  m_pTypeInfo[SKILL_SWORD_RAY].Set(sid++, "Sword Ray", 0, 0, 9, "Sword Ray");
  m_pTypeInfo[SKILL_SWORD_RAY].SetMP(25);
  m_pTypeInfo[SKILL_SWORD_RAY].SetLearnLevel(61);
  m_pTypeInfo[SKILL_SWORD_RAY].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_SWORD_RAY].AddNextSkill(SKILL_LIGHTNING_HAND);

  m_pTypeInfo[SKILL_LIGHTNING_HAND].Set(sid++, "Lightning Hand", 300, 270, 7,
                                        "Lightning Hand");
  m_pTypeInfo[SKILL_LIGHTNING_HAND].SetMP(25);
  m_pTypeInfo[SKILL_LIGHTNING_HAND].SetLearnLevel(65);
  m_pTypeInfo[SKILL_LIGHTNING_HAND].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_LIGHTNING_HAND].AddNextSkill(SKILL_REDIANCE);

  m_pTypeInfo[SKILL_REDIANCE].Set(sid++, "Radiance", 300, 270, 322, "Radiance");
  m_pTypeInfo[SKILL_REDIANCE].SetMP(25);
  m_pTypeInfo[SKILL_REDIANCE].SetLearnLevel(68);
  m_pTypeInfo[SKILL_REDIANCE].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_REDIANCE].AddNextSkill(SKILL_WIND_DIVIDER);

  m_pTypeInfo[SKILL_WIND_DIVIDER].Set(sid++, "Wind Divider", 500, 240, 121,
                                      "Wind Divider");
  m_pTypeInfo[SKILL_WIND_DIVIDER].SetMP(20);
  m_pTypeInfo[SKILL_WIND_DIVIDER].SetLearnLevel(70);
  m_pTypeInfo[SKILL_WIND_DIVIDER].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_WIND_DIVIDER].AddNextSkill(SKILL_EXPANSION);

  m_pTypeInfo[SKILL_EXPANSION].Set(sid++, "EXPANSION", 700, 270, 11, "EXPANSION");
  m_pTypeInfo[SKILL_EXPANSION].SetMP(25);
  m_pTypeInfo[SKILL_EXPANSION].SetLearnLevel(75);
  m_pTypeInfo[SKILL_EXPANSION].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_EXPANSION].AddNextSkill(SKILL_THUNDER_BOLT);

  m_pTypeInfo[SKILL_THUNDER_BOLT].Set(sid++, "Thunder Bolt", 700, 270, 10,
                                      "Thunder Bolt");
  m_pTypeInfo[SKILL_THUNDER_BOLT].SetMP(25);
  m_pTypeInfo[SKILL_THUNDER_BOLT].SetLearnLevel(80);
  m_pTypeInfo[SKILL_THUNDER_BOLT].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_THUNDER_BOLT].AddNextSkill(SKILL_MENTAL_SWORD);

  m_pTypeInfo[SKILL_MENTAL_SWORD].Set(sid++, "Mental Sword", 700, 270, 156,
                                      "Mental Sword");
  m_pTypeInfo[SKILL_MENTAL_SWORD].SetMP(25);
  m_pTypeInfo[SKILL_MENTAL_SWORD].SetLearnLevel(85);
  m_pTypeInfo[SKILL_MENTAL_SWORD].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_MENTAL_SWORD].AddNextSkill(SKILL_LARSLASH);

  m_pTypeInfo[SKILL_LARSLASH].Set(sid++, "Lar Slash", 700, 270, 323,
                                  "Lar Slash");
  m_pTypeInfo[SKILL_LARSLASH].SetMP(25);
  m_pTypeInfo[SKILL_LARSLASH].SetLearnLevel(88);
  m_pTypeInfo[SKILL_LARSLASH].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_LARSLASH].AddNextSkill(SKILL_THUNDER_STORM);

  m_pTypeInfo[SKILL_THUNDER_STORM].Set(sid++, "Thunder Storm", 700, 270, 157,
                                       "��� ����");
  m_pTypeInfo[SKILL_THUNDER_STORM].SetMP(25);
  m_pTypeInfo[SKILL_THUNDER_STORM].SetLearnLevel(90);
  m_pTypeInfo[SKILL_THUNDER_STORM].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_THUNDER_STORM].AddNextSkill(SKILL_SHARP_SHIELD);

  m_pTypeInfo[SKILL_SHARP_SHIELD].Set(sid++, "Sharp Shield", 700, 270, 216,
                                      "Sharp Shield");
  m_pTypeInfo[SKILL_SHARP_SHIELD].SetMP(25);
  m_pTypeInfo[SKILL_SHARP_SHIELD].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_SHARP_SHIELD].SetLearnLevel(100);
  m_pTypeInfo[SKILL_SHARP_SHIELD].AddNextSkill(SKILL_WIDE_LIGHTNING);

  m_pTypeInfo[SKILL_WIDE_LIGHTNING].Set(sid++, "Wide Lightning", 700, 270, 217,
                                        "Wide Lightning");
  m_pTypeInfo[SKILL_WIDE_LIGHTNING].SetMP(25);
  m_pTypeInfo[SKILL_WIDE_LIGHTNING].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_WIDE_LIGHTNING].SetLearnLevel(100);
  m_pTypeInfo[SKILL_WIDE_LIGHTNING].AddNextSkill(SKILL_BLITZ_SLIDING);

  m_pTypeInfo[SKILL_BLITZ_SLIDING].Set(sid++, "Blitz Sliding", 700, 270, 313,
                                       "Blitz Sliding");
  m_pTypeInfo[SKILL_BLITZ_SLIDING].SetMP(25);
  m_pTypeInfo[SKILL_BLITZ_SLIDING].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_BLITZ_SLIDING].SetLearnLevel(110);
  m_pTypeInfo[SKILL_BLITZ_SLIDING].AddNextSkill(SKILL_HIT_CONVERT);

  m_pTypeInfo[SKILL_HIT_CONVERT].Set(sid++, "Hit Convert", 700, 270, 254,
                                     "Hit Convert");
  m_pTypeInfo[SKILL_HIT_CONVERT].SetMP(40);
  m_pTypeInfo[SKILL_HIT_CONVERT].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_HIT_CONVERT].SetLearnLevel(120);
  m_pTypeInfo[SKILL_HIT_CONVERT].AddNextSkill(SKILL_SWORD_OF_THOR);

  // 2004, 9, 8, sobeit add start - �� 130 ��ų
  m_pTypeInfo[SKILL_SWORD_OF_THOR].Set(sid++, "Sword of Thor", 700, 270, 387,
                                       "Sword of Thor");
  m_pTypeInfo[SKILL_SWORD_OF_THOR].SetMP(40);
  m_pTypeInfo[SKILL_SWORD_OF_THOR].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_SWORD_OF_THOR].SetLearnLevel(130);
  m_pTypeInfo[SKILL_SWORD_OF_THOR].AddNextSkill(SKILL_INFINITY_THUNDERBOLT);
  // 2004, 9, 8, sobeit add end - �� 130 ��ų

  // 2004, 11, 11, sobeit add start - �� 140 ��ų
  m_pTypeInfo[SKILL_INFINITY_THUNDERBOLT].Set(
      sid++, "Infinity Thunderbolt", 700, 270, 458, "Infinity Thunderbolt");
  m_pTypeInfo[SKILL_INFINITY_THUNDERBOLT].SetMP(40);
  m_pTypeInfo[SKILL_INFINITY_THUNDERBOLT].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_INFINITY_THUNDERBOLT].SetLearnLevel(140);
  m_pTypeInfo[SKILL_INFINITY_THUNDERBOLT].AddNextSkill(SKILL_BIKE_CRASH);
  // 2004, 11, 11, sobeit add end - �� 140 ��ų

  m_pTypeInfo[SKILL_BIKE_CRASH].Set(sid++, "Bike Crash", 700, 270, 470,
                                    "Bike Crash");
  m_pTypeInfo[SKILL_BIKE_CRASH].SetMP(40);
  m_pTypeInfo[SKILL_BIKE_CRASH].SetSkillStep(
      SKILL_STEP_SLAYER_SWORD_ADVANCEMENT);
  m_pTypeInfo[SKILL_BIKE_CRASH].SetLearnLevel(150);
  m_pTypeInfo[SKILL_BIKE_CRASH].AddNextSkill(SKILL_SHINE_SWORD);

  m_pTypeInfo[SKILL_SHINE_SWORD].Set(sid++, "Shine Sword", 700, 270, 497,
                                     "Shine Sword");
  m_pTypeInfo[SKILL_SHINE_SWORD].SetMP(50);
  m_pTypeInfo[SKILL_SHINE_SWORD].SetSkillStep(
      SKILL_STEP_SLAYER_SWORD_ADVANCEMENT);
  m_pTypeInfo[SKILL_SHINE_SWORD].SetLearnLevel(160);
  m_pTypeInfo[SKILL_SHINE_SWORD].AddNextSkill(SKILL_SOUL_CHAIN);

  // 2006.02.20 ������� Sjheon Add
  //	m_pTypeInfo[SKILL_SWORD_COMBO].Set(sid++, "Sword Combo", 700, 270,
  //470,"�� �޺�"); 	m_pTypeInfo[SKILL_SWORD_COMBO].SetSkillStep(
  //SKILL_STEP_SLAYER_SWORD_ADVANCEMENT) ;// SKILL_STEP_SLAYER_SWORD_ADVANCEMENT
  //); 	m_pTypeInfo[SKILL_SWORD_COMBO].SetMP( 50 );
  //	m_pTypeInfo[SKILL_SWORD_COMBO].SetLearnLevel( 150 );
  //	m_pTypeInfo[SKILL_SWORD_COMBO].AddNextSkill(SKILL_SOUL_CHAIN) ; //
  //SKILL_BLADE_COMBO) ; // SKILL_DESTINIES );
  // 2006.02.20 ������� Sjheon End

  //----------------------------------------------------------------------
  // �� �迭
  //----------------------------------------------------------------------
  sid = 0;
  m_pTypeInfo[SKILL_SINGLE_BLOW].Set(sid++, "Single Blow", 0, 0, 13,
                                     "Single Blow");
  m_pTypeInfo[SKILL_SINGLE_BLOW].SetMP(8);
  m_pTypeInfo[SKILL_SINGLE_BLOW].SetLearnLevel(5);
  m_pTypeInfo[SKILL_SINGLE_BLOW].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_SINGLE_BLOW].AddNextSkill(SKILL_SPIRAL_SLAY);

  m_pTypeInfo[SKILL_SPIRAL_SLAY].Set(sid++, "Spiral Slay", 0, 0, 14,
                                     "Spiral Slay");
  m_pTypeInfo[SKILL_SPIRAL_SLAY].SetMP(10);
  m_pTypeInfo[SKILL_SPIRAL_SLAY].SetLearnLevel(10);
  m_pTypeInfo[SKILL_SPIRAL_SLAY].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_SPIRAL_SLAY].AddNextSkill(SKILL_CONCENTRATION);

  m_pTypeInfo[SKILL_CONCENTRATION].Set(sid++, "CONCENTRATION", 200, 130, 150,
                                       "CONCENTRATION");
  m_pTypeInfo[SKILL_CONCENTRATION].SetMP(8);
  m_pTypeInfo[SKILL_CONCENTRATION].SetPassive();
  m_pTypeInfo[SKILL_CONCENTRATION].SetLearnLevel(15);
  m_pTypeInfo[SKILL_CONCENTRATION].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_CONCENTRATION].AddNextSkill(SKILL_GHOST_BLADE);

  m_pTypeInfo[SKILL_GHOST_BLADE].Set(sid++, "Ghost Blade", 0, 0, 17,
                                     "Ghost Blade");
  m_pTypeInfo[SKILL_GHOST_BLADE].SetMP(12);
  m_pTypeInfo[SKILL_GHOST_BLADE].SetLearnLevel(20);
  m_pTypeInfo[SKILL_GHOST_BLADE].SetSkillStep(SKILL_STEP_APPRENTICE);
  //	m_pTypeInfo[SKILL_GHOST_BLADE].AddNextSkill( SKILL_TRIPLE_BREAK );
  m_pTypeInfo[SKILL_GHOST_BLADE].AddNextSkill(SKILL_TRIPLE_BREAK);

  // 2004,4,30,sobeit add start
  //	m_pTypeInfo[SKILL_WILL_OF_IRON].Set(sid++, "Will Of Iron", 600, 160,
  //382,"�� ���� ���̾�"); 	m_pTypeInfo[SKILL_WILL_OF_IRON].SetMP( 0 );
  //	m_pTypeInfo[SKILL_WILL_OF_IRON].SetPassive();
  //	m_pTypeInfo[SKILL_WILL_OF_IRON].SetLearnLevel( 23 );
  //	m_pTypeInfo[SKILL_WILL_OF_IRON].SetSkillStep( SKILL_STEP_APPRENTICE );
  //	m_pTypeInfo[SKILL_WILL_OF_IRON].AddNextSkill( SKILL_TRIPLE_BREAK );
  // 2004,4,30,sobeit add end

  m_pTypeInfo[SKILL_TRIPLE_BREAK].Set(sid++, "Triple Break", 0, 0, 15,
                                      "Triple Break");
  m_pTypeInfo[SKILL_TRIPLE_BREAK].SetMP(12);
  m_pTypeInfo[SKILL_TRIPLE_BREAK].SetLearnLevel(25);
  m_pTypeInfo[SKILL_TRIPLE_BREAK].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_TRIPLE_BREAK].AddNextSkill(SKILL_EVASION);

  m_pTypeInfo[SKILL_EVASION].Set(sid++, "EVASION", 200, 130, 151, "EVASION");
  m_pTypeInfo[SKILL_EVASION].SetMP(8);
  m_pTypeInfo[SKILL_EVASION].SetPassive();
  m_pTypeInfo[SKILL_EVASION].SetLearnLevel(30);
  m_pTypeInfo[SKILL_EVASION].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_EVASION].AddNextSkill(SKILL_WILD_SMASH);

  m_pTypeInfo[SKILL_WILD_SMASH].Set(sid++, "Wild Smash", 0, 0, 16,
                                    "Wild Smash");
  m_pTypeInfo[SKILL_WILD_SMASH].SetMP(12);
  m_pTypeInfo[SKILL_WILD_SMASH].SetLearnLevel(35);
  m_pTypeInfo[SKILL_WILD_SMASH].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_WILD_SMASH].AddNextSkill(SKILL_SHADOW_WALK);

  m_pTypeInfo[SKILL_SHADOW_WALK].Set(sid++, "Shadow Walk", 0, 0, 19,
                                     "Shadow Walk");
  m_pTypeInfo[SKILL_SHADOW_WALK].SetMP(20);
  m_pTypeInfo[SKILL_SHADOW_WALK].SetLearnLevel(45);
  m_pTypeInfo[SKILL_SHADOW_WALK].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_SHADOW_WALK].AddNextSkill(SKILL_POTENTIAL_EXPLOSION);

  m_pTypeInfo[SKILL_POTENTIAL_EXPLOSION].Set(sid++, "Potential Explosion", 0, 0,
                                             18, "Potential Explosion");
  m_pTypeInfo[SKILL_POTENTIAL_EXPLOSION].SetMP(25);
  m_pTypeInfo[SKILL_POTENTIAL_EXPLOSION].SetLearnLevel(50);
  m_pTypeInfo[SKILL_POTENTIAL_EXPLOSION].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_POTENTIAL_EXPLOSION].AddNextSkill(SKILL_HURRICANE_COMBO);

  m_pTypeInfo[SKILL_HURRICANE_COMBO].Set(sid++, "Hurricane Combo", 0, 0, 21,
                                         "Hurricane Combo");
  m_pTypeInfo[SKILL_HURRICANE_COMBO].SetMP(15);
  m_pTypeInfo[SKILL_HURRICANE_COMBO].SetLearnLevel(55);
  m_pTypeInfo[SKILL_HURRICANE_COMBO].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_HURRICANE_COMBO].AddNextSkill(SKILL_TORNADO_SEVER);

  m_pTypeInfo[SKILL_TORNADO_SEVER].Set(sid++, "Tornado Sever", 0, 0, 22,
                                       "Tornado Sever");
  m_pTypeInfo[SKILL_TORNADO_SEVER].SetMP(20);
  m_pTypeInfo[SKILL_TORNADO_SEVER].SetLearnLevel(60);
  m_pTypeInfo[SKILL_TORNADO_SEVER].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_TORNADO_SEVER].AddNextSkill(SKILL_MULTI_AMPUTATE);

  m_pTypeInfo[SKILL_MULTI_AMPUTATE].Set(sid++, "Multi Amputate", 0, 0, 247,
                                        "Multi Amputate");
  m_pTypeInfo[SKILL_MULTI_AMPUTATE].SetMP(20);
  m_pTypeInfo[SKILL_MULTI_AMPUTATE].SetLearnLevel(61);
  m_pTypeInfo[SKILL_MULTI_AMPUTATE].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_MULTI_AMPUTATE].AddNextSkill(SKILL_CHARGING_POWER);

  m_pTypeInfo[SKILL_CHARGING_POWER].Set(sid++, "Charging Power", 0, 0, 20,
                                        "Charging Power");
  m_pTypeInfo[SKILL_CHARGING_POWER].SetMP(20);
  m_pTypeInfo[SKILL_CHARGING_POWER].SetLearnLevel(65);
  m_pTypeInfo[SKILL_CHARGING_POWER].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_CHARGING_POWER].AddNextSkill(SKILL_MOONLIGHT_SEVER);

  m_pTypeInfo[SKILL_MOONLIGHT_SEVER].Set(sid++, "Moonlight Sever", 0, 0, 24,
                                         "Moonlight Sever");
  m_pTypeInfo[SKILL_MOONLIGHT_SEVER].SetMP(25);
  m_pTypeInfo[SKILL_MOONLIGHT_SEVER].SetLearnLevel(70);
  m_pTypeInfo[SKILL_MOONLIGHT_SEVER].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_MOONLIGHT_SEVER].AddNextSkill(SKILL_EARTHQUAKE);

  m_pTypeInfo[SKILL_EARTHQUAKE].Set(sid++, "Earthquake", 0, 0, 118,
                                    "Earthquake");
  m_pTypeInfo[SKILL_EARTHQUAKE].SetMP(20);
  m_pTypeInfo[SKILL_EARTHQUAKE].SetLearnLevel(75);
  m_pTypeInfo[SKILL_EARTHQUAKE].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_EARTHQUAKE].AddNextSkill(SKILL_BERSERKER);

  m_pTypeInfo[SKILL_BERSERKER].Set(sid++, "BERSERKER", 0, 0, 23, "BERSERKER");
  m_pTypeInfo[SKILL_BERSERKER].SetMP(25);
  m_pTypeInfo[SKILL_BERSERKER].SetLearnLevel(80);
  m_pTypeInfo[SKILL_BERSERKER].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_BERSERKER].AddNextSkill(SKILL_SHADOW_DANCING);

  m_pTypeInfo[SKILL_SHADOW_DANCING].Set(sid++, "Shadow Dancing", 0, 0, 152,
                                        "Shadow Dancing");
  m_pTypeInfo[SKILL_SHADOW_DANCING].SetMP(35);
  m_pTypeInfo[SKILL_SHADOW_DANCING].SetLearnLevel(85);
  m_pTypeInfo[SKILL_SHADOW_DANCING].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_SHADOW_DANCING].AddNextSkill(SKILL_TYPHOON);

  m_pTypeInfo[SKILL_TYPHOON].Set(sid++, "Typhoon", 0, 0, 153, "Typhoon");
  m_pTypeInfo[SKILL_TYPHOON].SetMP(35);
  m_pTypeInfo[SKILL_TYPHOON].SetLearnLevel(90);
  m_pTypeInfo[SKILL_TYPHOON].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_TYPHOON].AddNextSkill(SKILL_AIR_SHIELD);

  m_pTypeInfo[SKILL_AIR_SHIELD].Set(sid++, "Air Shield", 0, 0, 218,
                                    "Air Shield");
  m_pTypeInfo[SKILL_AIR_SHIELD].SetMP(35);
  m_pTypeInfo[SKILL_AIR_SHIELD].SetLearnLevel(95);
  m_pTypeInfo[SKILL_AIR_SHIELD].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_AIR_SHIELD].AddNextSkill(SKILL_POWER_OF_LAND);

  m_pTypeInfo[SKILL_POWER_OF_LAND].Set(sid++, "Power of Land", 0, 0, 219,
                                       "Power of Land");
  m_pTypeInfo[SKILL_POWER_OF_LAND].SetMP(35);
  m_pTypeInfo[SKILL_POWER_OF_LAND].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_POWER_OF_LAND].SetLearnLevel(100);
  m_pTypeInfo[SKILL_POWER_OF_LAND].AddNextSkill(SKILL_BLAZE_WALK);

  m_pTypeInfo[SKILL_BLAZE_WALK].Set(sid++, "Blaze Walk", 0, 0, 314,
                                    "Blaze Walk");
  m_pTypeInfo[SKILL_BLAZE_WALK].SetMP(35);
  m_pTypeInfo[SKILL_BLAZE_WALK].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_BLAZE_WALK].SetLearnLevel(110);
  m_pTypeInfo[SKILL_BLAZE_WALK].AddNextSkill(SKILL_WILD_TYPHOON);

  m_pTypeInfo[SKILL_WILD_TYPHOON].Set(sid++, "Wild Typhoon", 0, 0, 255,
                                      "Wild Typhoon");
  m_pTypeInfo[SKILL_WILD_TYPHOON].SetMP(40);
  m_pTypeInfo[SKILL_WILD_TYPHOON].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_WILD_TYPHOON].SetLearnLevel(120);
  m_pTypeInfo[SKILL_WILD_TYPHOON].AddNextSkill(SKILL_BURNING_SOUL_CHARGING);

  // 2004, 9, 8, sobeit add start - �� 130 ��ų
  m_pTypeInfo[SKILL_BURNING_SOUL_CHARGING].Set(sid++, "Burning Sol", 700, 270,
                                               388, "Burning Sol");
  m_pTypeInfo[SKILL_BURNING_SOUL_CHARGING].SetMP(40);
  m_pTypeInfo[SKILL_BURNING_SOUL_CHARGING].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_BURNING_SOUL_CHARGING].SetLearnLevel(130);
  m_pTypeInfo[SKILL_BURNING_SOUL_CHARGING].AddNextSkill(SKILL_SPIT_STREAM);
  // 2004, 9, 8, sobeit add end - �� 130 ��ų

  // 2004, 11, 11, sobeit add start - �� 140 ��ų
  m_pTypeInfo[SKILL_SPIT_STREAM].Set(sid++, "Spit Stream", 700, 270, 459,
                                     "Spit Stream");
  m_pTypeInfo[SKILL_SPIT_STREAM].SetMP(40);
  m_pTypeInfo[SKILL_SPIT_STREAM].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_SPIT_STREAM].SetLearnLevel(140);
  m_pTypeInfo[SKILL_SPIT_STREAM].AddNextSkill(SKILL_DRAGON_TORNADO);
  // 2004, 11, 11, sobeit add end - �� 130 ��ų

  // by csm �ּ���
  m_pTypeInfo[SKILL_DRAGON_TORNADO].Set(sid++, "Dragon Tornado", 700, 270, 469,
                                        "Dragon Tornado");
  m_pTypeInfo[SKILL_DRAGON_TORNADO].SetMP(40);
  m_pTypeInfo[SKILL_DRAGON_TORNADO].SetSkillStep(
      SKILL_STEP_SLAYER_BLADE_ADVANCEMENT);
  m_pTypeInfo[SKILL_DRAGON_TORNADO].SetLearnLevel(150);
  m_pTypeInfo[SKILL_DRAGON_TORNADO].AddNextSkill(SKILL_BOMB_CRASH_WALK);

  // 2006.10.24 sjheon Add
  m_pTypeInfo[SKILL_BOMB_CRASH_WALK].Set(sid++, "Bomb Crash Walk", 700, 270,
                                         498, "Bomb Crash Walk");
  m_pTypeInfo[SKILL_BOMB_CRASH_WALK].SetMP(50);
  m_pTypeInfo[SKILL_BOMB_CRASH_WALK].SetSkillStep(
      SKILL_STEP_SLAYER_BLADE_ADVANCEMENT);
  m_pTypeInfo[SKILL_BOMB_CRASH_WALK].SetLearnLevel(160);
  m_pTypeInfo[SKILL_BOMB_CRASH_WALK].AddNextSkill(SKILL_SOUL_CHAIN);
  // 2006.10.24 sjheon End

  // 2006.02.20 ������� Sjheon Add

  //	m_pTypeInfo[SKILL_BLADE_COMBO].Set(sid++, "Blade Combo", 700, 270,
  //469,"�� �޺�"); 	m_pTypeInfo[SKILL_BLADE_COMBO].SetSkillStep(
  //SKILL_STEP_SLAYER_BLADE_ADVANCEMENT ); 	m_pTypeInfo[SKILL_BLADE_COMBO].SetMP(
  //50 ); 	m_pTypeInfo[SKILL_BLADE_COMBO].SetLearnLevel( 150 );
  //	m_pTypeInfo[SKILL_BLADE_COMBO].AddNextSkill(SKILL_SOUL_CHAIN) ; //
  //SKILL_DESTINIES );

  // 2005.07.15 ������� Sjheon End

  //----------------------------------------------------------------------
  // ��
  //----------------------------------------------------------------------
  sid = 0;
  m_pTypeInfo[SKILL_FAST_RELOAD].Set(sid++, "Fast Reload", 0, 0, 26,
                                     "Fast Reload");
  m_pTypeInfo[SKILL_FAST_RELOAD].SetMP(0);
  m_pTypeInfo[SKILL_FAST_RELOAD].SetLearnLevel(5);
  m_pTypeInfo[SKILL_FAST_RELOAD].SetPassive();
  m_pTypeInfo[SKILL_FAST_RELOAD].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_FAST_RELOAD].AddNextSkill(SKILL_DOUBLE_SHOT);

  m_pTypeInfo[SKILL_DOUBLE_SHOT].Set(sid++, "Double Shot", 0, 0, 27, "Double Shot");
  m_pTypeInfo[SKILL_DOUBLE_SHOT].SetMP(0);
  m_pTypeInfo[SKILL_DOUBLE_SHOT].SetLearnLevel(10);
  m_pTypeInfo[SKILL_DOUBLE_SHOT].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_DOUBLE_SHOT].AddNextSkill(SKILL_HEART_CATALYST);

  //	m_pTypeInfo[SKILL_SMG_MASTERY].Set(sid++, "SMG Mastery", 0, 0,
  //28,"SMG�����͸�"); 	m_pTypeInfo[SKILL_SMG_MASTERY].SetMP( 0 );
  //	m_pTypeInfo[SKILL_SMG_MASTERY].SetLearnLevel( 15 );
  //	m_pTypeInfo[SKILL_SMG_MASTERY].SetPassive();
  //	m_pTypeInfo[SKILL_SMG_MASTERY].SetSkillStep( SKILL_STEP_APPRENTICE );
  //	m_pTypeInfo[SKILL_SMG_MASTERY].AddNextSkill( SKILL_TRIPLE_SHOT );

  m_pTypeInfo[SKILL_HEART_CATALYST].Set(sid++, "Heart Catalyst", 0, 0, 329,
                                        "Heart Catalyst");
  m_pTypeInfo[SKILL_HEART_CATALYST].SetMP(0);
  m_pTypeInfo[SKILL_HEART_CATALYST].SetLearnLevel(15);
  m_pTypeInfo[SKILL_HEART_CATALYST].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_HEART_CATALYST].AddNextSkill(SKILL_TRIPLE_SHOT);

  m_pTypeInfo[SKILL_TRIPLE_SHOT].Set(sid++, "Triple Shot", 0, 0, 27,
                                     "Triple Shot");
  m_pTypeInfo[SKILL_TRIPLE_SHOT].SetMP(0);
  m_pTypeInfo[SKILL_TRIPLE_SHOT].SetLearnLevel(20);
  m_pTypeInfo[SKILL_TRIPLE_SHOT].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_TRIPLE_SHOT].AddNextSkill(SKILL_MULTI_SHOT);

  m_pTypeInfo[SKILL_MULTI_SHOT].Set(sid++, "Multi Shot", 0, 0, 29, "Multi Shot");
  m_pTypeInfo[SKILL_MULTI_SHOT].SetMP(0);
  m_pTypeInfo[SKILL_MULTI_SHOT].SetLearnLevel(25);
  m_pTypeInfo[SKILL_MULTI_SHOT].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_MULTI_SHOT].AddNextSkill(SKILL_HEAD_SHOT);

  m_pTypeInfo[SKILL_HEAD_SHOT].Set(sid++, "Head Shot", 0, 0, 30, "Head Shot");
  m_pTypeInfo[SKILL_HEAD_SHOT].SetMP(0);
  m_pTypeInfo[SKILL_HEAD_SHOT].SetLearnLevel(30);
  m_pTypeInfo[SKILL_HEAD_SHOT].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_HEAD_SHOT].AddNextSkill(SKILL_ARMS_MASTERY1);

  m_pTypeInfo[SKILL_ARMS_MASTERY1].Set(sid++, "Arms Mastery 1", 0, 0, 325,
                                       "Arms Mastery 1");
  m_pTypeInfo[SKILL_ARMS_MASTERY1].SetMP(0);
  m_pTypeInfo[SKILL_ARMS_MASTERY1].SetLearnLevel(35);
  m_pTypeInfo[SKILL_ARMS_MASTERY1].SetPassive();
  m_pTypeInfo[SKILL_ARMS_MASTERY1].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_ARMS_MASTERY1].AddNextSkill(SKILL_QUICK_FIRE);

  //	m_pTypeInfo[SKILL_AR_MASTERY].Set(sid++, "AR Mastery", 0, 0,
  //31,"AR�����͸�"); 	m_pTypeInfo[SKILL_AR_MASTERY].SetMP( 0 );
  //	m_pTypeInfo[SKILL_AR_MASTERY].SetLearnLevel( 35 );
  //	m_pTypeInfo[SKILL_AR_MASTERY].SetPassive();
  //	m_pTypeInfo[SKILL_AR_MASTERY].SetSkillStep( SKILL_STEP_APPRENTICE );
  //	m_pTypeInfo[SKILL_AR_MASTERY].AddNextSkill( SKILL_QUICK_FIRE );

  m_pTypeInfo[SKILL_QUICK_FIRE].Set(sid++, "Quick Fire", 0, 0, 27, "Quick Fire");
  m_pTypeInfo[SKILL_QUICK_FIRE].SetMP(0);
  m_pTypeInfo[SKILL_QUICK_FIRE].SetLearnLevel(40);
  m_pTypeInfo[SKILL_QUICK_FIRE].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_QUICK_FIRE].AddNextSkill(SKILL_SNIPPING);

  m_pTypeInfo[SKILL_SNIPPING].Set(sid++, "Snipping", 0, 0, 33, "Snipping");
  m_pTypeInfo[SKILL_SNIPPING].SetMP(0);
  m_pTypeInfo[SKILL_SNIPPING].SetLearnLevel(45);
  m_pTypeInfo[SKILL_SNIPPING].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_SNIPPING].AddNextSkill(SKILL_LIVENESS);

  m_pTypeInfo[SKILL_LIVENESS].Set(sid++, "Liveness", 0, 0, 176, "Liveness");
  m_pTypeInfo[SKILL_LIVENESS].SetPassive();
  m_pTypeInfo[SKILL_LIVENESS].SetLearnLevel(45);
  m_pTypeInfo[SKILL_LIVENESS].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_LIVENESS].AddNextSkill(SKILL_PIERCING);

  m_pTypeInfo[SKILL_PIERCING].Set(sid++, "Piercing", 0, 0, 32, "Piercing");
  m_pTypeInfo[SKILL_PIERCING].SetMP(0);
  m_pTypeInfo[SKILL_PIERCING].SetLearnLevel(47);
  m_pTypeInfo[SKILL_PIERCING].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_PIERCING].AddNextSkill(SKILL_VIVID_MAGAZINE);

  m_pTypeInfo[SKILL_VIVID_MAGAZINE].Set(sid++, "Vivid Magazine", 0, 0, 335,
                                        "Vivid Magazine");
  m_pTypeInfo[SKILL_VIVID_MAGAZINE].SetMP(0);
  m_pTypeInfo[SKILL_VIVID_MAGAZINE].SetLearnLevel(50);
  m_pTypeInfo[SKILL_VIVID_MAGAZINE].SetPassive();
  m_pTypeInfo[SKILL_VIVID_MAGAZINE].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_VIVID_MAGAZINE].AddNextSkill(SKILL_THROW_BOMB);

  //	m_pTypeInfo[SKILL_SG_MASTERY].Set(sid++, "SG Mastery", 0, 0,
  //34,"SG�����͸�"); 	m_pTypeInfo[SKILL_SG_MASTERY].SetMP( 0 );
  //	m_pTypeInfo[SKILL_SG_MASTERY].SetLearnLevel( 50 );
  //	m_pTypeInfo[SKILL_SG_MASTERY].SetPassive();
  //	m_pTypeInfo[SKILL_SG_MASTERY].SetSkillStep( SKILL_STEP_APPRENTICE );
  //	m_pTypeInfo[SKILL_SG_MASTERY].AddNextSkill( SKILL_THROW_BOMB );

  m_pTypeInfo[SKILL_THROW_BOMB].Set(sid++, "Throw Bomb", 0, 0, 36, "Throw Bomb");
  m_pTypeInfo[SKILL_THROW_BOMB].SetMP(10);
  m_pTypeInfo[SKILL_THROW_BOMB].SetLearnLevel(53);
  // m_pTypeInfo[SKILL_THROW_BOMB].SetPassive();
  m_pTypeInfo[SKILL_THROW_BOMB].SetActive();
  m_pTypeInfo[SKILL_THROW_BOMB].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_THROW_BOMB].AddNextSkill(SKILL_CONCEALMENT);

  m_pTypeInfo[SKILL_CONCEALMENT].Set(sid++, "Concealment", 0, 0, 249,
                                     "Concealment");
  m_pTypeInfo[SKILL_CONCEALMENT].SetMP(10);
  m_pTypeInfo[SKILL_CONCEALMENT].SetLearnLevel(55);
  m_pTypeInfo[SKILL_CONCEALMENT].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_CONCEALMENT].AddNextSkill(SKILL_DETECT_MINE);

  m_pTypeInfo[SKILL_DETECT_MINE].Set(sid++, "Revealer", 0, 0, 35, "Revealer");
  m_pTypeInfo[SKILL_DETECT_MINE].SetMP(20);
  m_pTypeInfo[SKILL_DETECT_MINE].SetLearnLevel(57);
  m_pTypeInfo[SKILL_DETECT_MINE].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_DETECT_MINE].AddNextSkill(SKILL_TRIDENT);
  m_pTypeInfo[SKILL_DETECT_MINE].SetActive();

  m_pTypeInfo[SKILL_TRIDENT].Set(sid++, "Trident", 0, 0, 321, "Trident");
  m_pTypeInfo[SKILL_TRIDENT].SetMP(10);
  m_pTypeInfo[SKILL_TRIDENT].SetLearnLevel(60);
  m_pTypeInfo[SKILL_TRIDENT].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_TRIDENT].AddNextSkill(SKILL_ARMS_MASTERY2);

  //	m_pTypeInfo[SKILL_MAKE_BOMB].Set(sid++, "Make Bomb", 0, 0,
  //37,"ũ������Ʈ��"); 	m_pTypeInfo[SKILL_MAKE_BOMB].SetMP( 10 );
  //	m_pTypeInfo[SKILL_MAKE_BOMB].SetLearnLevel( 60 );
  //	m_pTypeInfo[SKILL_MAKE_BOMB].SetActive();
  //	m_pTypeInfo[SKILL_MAKE_BOMB].SetSkillStep( SKILL_STEP_APPRENTICE );
  //	m_pTypeInfo[SKILL_MAKE_BOMB].AddNextSkill( SKILL_ARMS_MASTERY2 );

  m_pTypeInfo[SKILL_ARMS_MASTERY2].Set(sid++, "Arms Mastery 2", 0, 0, 324,
                                       "Arms Mastery 2");
  m_pTypeInfo[SKILL_ARMS_MASTERY2].SetMP(0);
  m_pTypeInfo[SKILL_ARMS_MASTERY2].SetLearnLevel(65);
  m_pTypeInfo[SKILL_ARMS_MASTERY2].SetPassive();
  m_pTypeInfo[SKILL_ARMS_MASTERY2].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_ARMS_MASTERY2].AddNextSkill(SKILL_INSTALL_MINE);

  //	m_pTypeInfo[SKILL_SR_MASTERY].Set(sid++, "SR Mastery", 0, 0,
  //38,"SR�����͸�"); 	m_pTypeInfo[SKILL_SR_MASTERY].SetMP( 0 );
  //	m_pTypeInfo[SKILL_SR_MASTERY].SetLearnLevel( 65 );
  //	m_pTypeInfo[SKILL_SR_MASTERY].SetPassive();
  //	m_pTypeInfo[SKILL_SR_MASTERY].SetSkillStep( SKILL_STEP_APPRENTICE );
  //	m_pTypeInfo[SKILL_SR_MASTERY].AddNextSkill( SKILL_INSTALL_MINE );

  m_pTypeInfo[SKILL_INSTALL_MINE].Set(sid++, "Install Mine", 0, 0, 39,
                                      "Install Mine");
  m_pTypeInfo[SKILL_INSTALL_MINE].SetMP(20);
  m_pTypeInfo[SKILL_INSTALL_MINE].SetLearnLevel(70);
  m_pTypeInfo[SKILL_INSTALL_MINE].SetActive();
  m_pTypeInfo[SKILL_INSTALL_MINE].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_INSTALL_MINE].AddNextSkill(SKILL_MAKE_MINE);

  m_pTypeInfo[SKILL_MAKE_MINE].Set(sid++, "Create Mine", 0, 0, 40,
                                   "Create Mine");
  m_pTypeInfo[SKILL_MAKE_MINE].SetMP(20);
  m_pTypeInfo[SKILL_MAKE_MINE].SetLearnLevel(75);
  m_pTypeInfo[SKILL_MAKE_MINE].SetActive();
  m_pTypeInfo[SKILL_MAKE_MINE].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_MAKE_MINE].AddNextSkill(SKILL_INSTALL_TRAP);

  m_pTypeInfo[SKILL_INSTALL_TRAP].Set(sid++, "Install Trap", 0, 0, 331,
                                      "Install Trap");
  m_pTypeInfo[SKILL_INSTALL_TRAP].SetMP(20);
  m_pTypeInfo[SKILL_INSTALL_TRAP].SetLearnLevel(75);
  m_pTypeInfo[SKILL_INSTALL_TRAP].SetActive();
  m_pTypeInfo[SKILL_INSTALL_TRAP].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_INSTALL_TRAP].AddNextSkill(SKILL_OBSERVING_EYE);

  m_pTypeInfo[SKILL_OBSERVING_EYE].Set(sid++, "Observing Eye", 0, 0, 159,
                                       "Observing Eye");
  m_pTypeInfo[SKILL_OBSERVING_EYE].SetMP(20);
  m_pTypeInfo[SKILL_OBSERVING_EYE].SetLearnLevel(85);
  m_pTypeInfo[SKILL_OBSERVING_EYE].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_OBSERVING_EYE].AddNextSkill(SKILL_MOLE_SHOT);

  m_pTypeInfo[SKILL_MOLE_SHOT].Set(sid++, "Mole Shot", 0, 0, 333, "Mole Shot");
  m_pTypeInfo[SKILL_MOLE_SHOT].SetMP(20);
  m_pTypeInfo[SKILL_MOLE_SHOT].SetLearnLevel(85);
  m_pTypeInfo[SKILL_MOLE_SHOT].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_MOLE_SHOT].AddNextSkill(SKILL_BULLET_OF_LIGHT);

  m_pTypeInfo[SKILL_BULLET_OF_LIGHT].Set(sid++, "Bullet of Light", 0, 0, 224,
                                         "Bullet of Light");
  m_pTypeInfo[SKILL_BULLET_OF_LIGHT].SetMP(20);
  m_pTypeInfo[SKILL_BULLET_OF_LIGHT].SetLearnLevel(85);
  m_pTypeInfo[SKILL_BULLET_OF_LIGHT].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_BULLET_OF_LIGHT].AddNextSkill(SKILL_GUN_SHOT_GUIDANCE);

  m_pTypeInfo[SKILL_GUN_SHOT_GUIDANCE].Set(sid++, "Gunshot Guidance", 0, 0, 225,
                                           "Gunshot Guidance");
  m_pTypeInfo[SKILL_GUN_SHOT_GUIDANCE].SetMP(20);
  m_pTypeInfo[SKILL_GUN_SHOT_GUIDANCE].SetLearnLevel(95);
  m_pTypeInfo[SKILL_GUN_SHOT_GUIDANCE].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_GUN_SHOT_GUIDANCE].AddNextSkill(SKILL_JABBING_VEIN);

  m_pTypeInfo[SKILL_JABBING_VEIN].Set(sid++, "Jabbing Vein", 0, 0, 315,
                                      "Jabbing Vein");
  m_pTypeInfo[SKILL_JABBING_VEIN].SetMP(20);
  m_pTypeInfo[SKILL_JABBING_VEIN].SetLearnLevel(110);
  m_pTypeInfo[SKILL_JABBING_VEIN].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_JABBING_VEIN].AddNextSkill(SKILL_ULTIMATE_BLOW);

  m_pTypeInfo[SKILL_ULTIMATE_BLOW].Set(sid++, "Ultimate Blow", 0, 0, 256,
                                       "Ultimate Blow");
  m_pTypeInfo[SKILL_ULTIMATE_BLOW].SetMP(25);
  m_pTypeInfo[SKILL_ULTIMATE_BLOW].SetLearnLevel(120);
  m_pTypeInfo[SKILL_ULTIMATE_BLOW].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_ULTIMATE_BLOW].AddNextSkill(SKILL_INSTALL_TURRET);

  // 2004, 9, 8, sobeit add start - �� 130 ��ų
  m_pTypeInfo[SKILL_INSTALL_TURRET].Set(sid++, "Install Turret", 700, 270, 389,
                                        "Install Turret");
  m_pTypeInfo[SKILL_INSTALL_TURRET].SetMP(40);
  m_pTypeInfo[SKILL_INSTALL_TURRET].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_INSTALL_TURRET].SetLearnLevel(130);
  m_pTypeInfo[SKILL_INSTALL_TURRET].AddNextSkill(SKILL_TURRET_FIRE);
  // 2004, 9, 8, sobeit add end - �� 130 ��ų

  m_pTypeInfo[SKILL_TURRET_FIRE].Set(sid++, "Install Turret Moved", 700, 270,
                                     389, "Install Turret Moved");
  m_pTypeInfo[SKILL_TURRET_FIRE].SetMP(40);
  m_pTypeInfo[SKILL_TURRET_FIRE].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_TURRET_FIRE].SetLearnLevel(130);
  m_pTypeInfo[SKILL_TURRET_FIRE].AddNextSkill(SKILL_PLASMA_ROCKET_LAUNCHER);

  // 2004, 11, 11, sobeit add start - �� 140 ��ų
  m_pTypeInfo[SKILL_PLASMA_ROCKET_LAUNCHER].Set(
      sid++, "Plasma Rocket Luncher", 700, 270, 460, "Plasma Rocket Luncher");
  m_pTypeInfo[SKILL_PLASMA_ROCKET_LAUNCHER].SetMP(40);
  m_pTypeInfo[SKILL_PLASMA_ROCKET_LAUNCHER].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_PLASMA_ROCKET_LAUNCHER].SetLearnLevel(140);
  m_pTypeInfo[SKILL_PLASMA_ROCKET_LAUNCHER].AddNextSkill(SKILL_HARPOON_BOMB);
  // 2004, 11, 11, sobeit add end - �� 140 ��ų

  m_pTypeInfo[SKILL_HARPOON_BOMB].Set(sid++, "Harpoon Bomb", 700, 270, 471,
                                      "Harpoon Bomb");
  m_pTypeInfo[SKILL_HARPOON_BOMB].SetMP(40);
  m_pTypeInfo[SKILL_HARPOON_BOMB].SetSkillStep(
      SKILL_STEP_SLAYER_GUN_BOMB_ADVANCEMENT);
  m_pTypeInfo[SKILL_HARPOON_BOMB].SetLearnLevel(150);
  m_pTypeInfo[SKILL_HARPOON_BOMB].AddNextSkill(SKILL_SATELLITE_BOMB);

  m_pTypeInfo[SKILL_SATELLITE_BOMB].Set(sid++, "Satellite Bomb", 700, 270, 500,
                                        "Satellite Bomb");
  m_pTypeInfo[SKILL_SATELLITE_BOMB].SetMP(40);
  m_pTypeInfo[SKILL_SATELLITE_BOMB].SetSkillStep(
      SKILL_STEP_SLAYER_GUN_BOMB_ADVANCEMENT);
  m_pTypeInfo[SKILL_SATELLITE_BOMB].SetLearnLevel(160);
  m_pTypeInfo[SKILL_SATELLITE_BOMB].AddNextSkill(SKILL_SOUL_CHAIN);

  //----------------------------------------------------------------------
  // Heal
  //----------------------------------------------------------------------
  sid = 0;
  m_pTypeInfo[MAGIC_CURE_LIGHT_WOUNDS].Set(sid++, "Cure Light Wounds", 0, 0, 41,
                                           "Cure Light Wounds");
  m_pTypeInfo[MAGIC_CURE_LIGHT_WOUNDS].SetMP(6);
  m_pTypeInfo[MAGIC_CURE_LIGHT_WOUNDS].SetLearnLevel(5);
  m_pTypeInfo[MAGIC_CURE_LIGHT_WOUNDS].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[MAGIC_CURE_LIGHT_WOUNDS].AddNextSkill(SKILL_HOLY_ARROW);

  m_pTypeInfo[SKILL_HOLY_ARROW].Set(sid++, "HOLY ARROW", 0, 0, 214,
                                    "HOLY ARROW");
  m_pTypeInfo[SKILL_HOLY_ARROW].SetMP(4);
  m_pTypeInfo[SKILL_HOLY_ARROW].SetLearnLevel(8);
  m_pTypeInfo[SKILL_HOLY_ARROW].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_HOLY_ARROW].AddNextSkill(MAGIC_CURE_POISON);

  m_pTypeInfo[MAGIC_CURE_POISON].Set(sid++, "Cure Poison", 0, 0, 42,
                                     "Cure Poison");
  m_pTypeInfo[MAGIC_CURE_POISON].SetMP(4);
  m_pTypeInfo[MAGIC_CURE_POISON].SetLearnLevel(10);
  m_pTypeInfo[MAGIC_CURE_POISON].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[MAGIC_CURE_POISON].AddNextSkill(MAGIC_PROTECTION_FROM_POISON);

  m_pTypeInfo[MAGIC_PROTECTION_FROM_POISON].Set(
      sid++, "Protection From Poison", 0, 0, 43, "Protection From Poison");
  m_pTypeInfo[MAGIC_PROTECTION_FROM_POISON].SetMP(6);
  m_pTypeInfo[MAGIC_PROTECTION_FROM_POISON].SetLearnLevel(15);
  m_pTypeInfo[MAGIC_PROTECTION_FROM_POISON].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[MAGIC_PROTECTION_FROM_POISON].AddNextSkill(
      MAGIC_CAUSE_LIGHT_WOUNDS);

  m_pTypeInfo[MAGIC_CAUSE_LIGHT_WOUNDS].Set(sid++, "Cause Light Wounds", 0, 0,
                                            44, "Cause Light Wounds");
  m_pTypeInfo[MAGIC_CAUSE_LIGHT_WOUNDS].SetMP(4);
  m_pTypeInfo[MAGIC_CAUSE_LIGHT_WOUNDS].SetLearnLevel(20);
  m_pTypeInfo[MAGIC_CAUSE_LIGHT_WOUNDS].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[MAGIC_CAUSE_LIGHT_WOUNDS].AddNextSkill(MAGIC_CURE_SERIOUS_WOUNDS);

  m_pTypeInfo[MAGIC_CURE_SERIOUS_WOUNDS].Set(sid++, "Cure Serious Wounds", 0, 0,
                                             45, "Cure Serious Wounds");
  m_pTypeInfo[MAGIC_CURE_SERIOUS_WOUNDS].SetMP(4);
  m_pTypeInfo[MAGIC_CURE_SERIOUS_WOUNDS].SetLearnLevel(25);
  m_pTypeInfo[MAGIC_CURE_SERIOUS_WOUNDS].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[MAGIC_CURE_SERIOUS_WOUNDS].AddNextSkill(MAGIC_REMOVE_CURSE);

  m_pTypeInfo[MAGIC_REMOVE_CURSE].Set(sid++, "Remove Curse", 0, 0, 46,
                                      "Remove Curse");
  m_pTypeInfo[MAGIC_REMOVE_CURSE].SetMP(4);
  m_pTypeInfo[MAGIC_REMOVE_CURSE].SetLearnLevel(30);
  m_pTypeInfo[MAGIC_REMOVE_CURSE].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[MAGIC_REMOVE_CURSE].AddNextSkill(MAGIC_PROTECTION_FROM_CURSE);

  m_pTypeInfo[MAGIC_PROTECTION_FROM_CURSE].Set(
      sid++, "Protection From Curse", 0, 0, 47, "Protection From Curse"); // 207);
  m_pTypeInfo[MAGIC_PROTECTION_FROM_CURSE].SetMP(12);
  m_pTypeInfo[MAGIC_PROTECTION_FROM_CURSE].SetLearnLevel(35);
  m_pTypeInfo[MAGIC_PROTECTION_FROM_CURSE].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[MAGIC_PROTECTION_FROM_CURSE].AddNextSkill(MAGIC_RESURRECT);

  m_pTypeInfo[MAGIC_RESURRECT].Set(sid++, "Resurrect", 0, 0, 126, "Resurrect");
  m_pTypeInfo[MAGIC_RESURRECT].SetMP(4);
  m_pTypeInfo[MAGIC_RESURRECT].SetLearnLevel(40);
  m_pTypeInfo[MAGIC_RESURRECT].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[MAGIC_RESURRECT].AddNextSkill(MAGIC_CAUSE_SERIOUS_WOUNDS);

  m_pTypeInfo[MAGIC_CAUSE_SERIOUS_WOUNDS].Set(sid++, "Cause Serious Wounds", 0,
                                              0, 48, "Cause Serious Wounds");
  m_pTypeInfo[MAGIC_CAUSE_SERIOUS_WOUNDS].SetMP(4);
  m_pTypeInfo[MAGIC_CAUSE_SERIOUS_WOUNDS].SetLearnLevel(45);
  m_pTypeInfo[MAGIC_CAUSE_SERIOUS_WOUNDS].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[MAGIC_CAUSE_SERIOUS_WOUNDS].AddNextSkill(
      SKILL_CREATE_HOLY_POTION);

  m_pTypeInfo[SKILL_CREATE_HOLY_POTION].Set(sid++, "Create Holy Potion", 0, 0,
                                            327, "Create Holy Potion");
  m_pTypeInfo[SKILL_CREATE_HOLY_POTION].SetMP(4);
  m_pTypeInfo[SKILL_CREATE_HOLY_POTION].SetLearnLevel(47);
  m_pTypeInfo[SKILL_CREATE_HOLY_POTION].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_CREATE_HOLY_POTION].AddNextSkill(
      MAGIC_CURE_CRITICAL_WOUNDS);

  m_pTypeInfo[MAGIC_CURE_CRITICAL_WOUNDS].Set(sid++, "Cure Critical Wounds", 0,
                                              0, 49, "Cure Critical Wounds");
  m_pTypeInfo[MAGIC_CURE_CRITICAL_WOUNDS].SetMP(4);
  m_pTypeInfo[MAGIC_CURE_CRITICAL_WOUNDS].SetLearnLevel(50);
  m_pTypeInfo[MAGIC_CURE_CRITICAL_WOUNDS].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[MAGIC_CURE_CRITICAL_WOUNDS].AddNextSkill(MAGIC_PRAYER);

  m_pTypeInfo[MAGIC_PRAYER].Set(sid++, "Prayer", 0, 0, 125, "Prayer");
  m_pTypeInfo[MAGIC_PRAYER].SetMP(12);
  m_pTypeInfo[MAGIC_PRAYER].SetLearnLevel(55);
  m_pTypeInfo[MAGIC_PRAYER].SetPassive();
  m_pTypeInfo[MAGIC_PRAYER].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[MAGIC_PRAYER].AddNextSkill(SKILL_DENIAL_MAGIC);

  m_pTypeInfo[SKILL_DENIAL_MAGIC].Set(sid++, "Denial Magic", 0, 0, 248,
                                      "Denial Magic");
  m_pTypeInfo[SKILL_DENIAL_MAGIC].SetMP(12);
  m_pTypeInfo[SKILL_DENIAL_MAGIC].SetLearnLevel(65);
  m_pTypeInfo[SKILL_DENIAL_MAGIC].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_DENIAL_MAGIC].AddNextSkill(MAGIC_PROTECTION_FROM_ACID);

  m_pTypeInfo[MAGIC_PROTECTION_FROM_ACID].Set(sid++, "Protection From Acid", 0,
                                              0, 50, "Protection From Acid");
  m_pTypeInfo[MAGIC_PROTECTION_FROM_ACID].SetMP(12);
  m_pTypeInfo[MAGIC_PROTECTION_FROM_ACID].SetLearnLevel(60);
  m_pTypeInfo[MAGIC_PROTECTION_FROM_ACID].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[MAGIC_PROTECTION_FROM_ACID].AddNextSkill(MAGIC_SACRIFICE);

  m_pTypeInfo[MAGIC_SACRIFICE].Set(sid++, "Sacrifice", 0, 0, 51,
                                   "Sacrifice");
  m_pTypeInfo[MAGIC_SACRIFICE].SetMP(12);
  m_pTypeInfo[MAGIC_SACRIFICE].SetLearnLevel(65);
  m_pTypeInfo[MAGIC_SACRIFICE].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[MAGIC_SACRIFICE].AddNextSkill(MAGIC_CAUSE_CRITICAL_WOUNDS);

  m_pTypeInfo[MAGIC_CAUSE_CRITICAL_WOUNDS].Set(sid++, "Cause Critical Wounds",
                                               0, 0, 52, "Cause Critical Wounds");
  m_pTypeInfo[MAGIC_CAUSE_CRITICAL_WOUNDS].SetMP(4);
  m_pTypeInfo[MAGIC_CAUSE_CRITICAL_WOUNDS].SetLearnLevel(70);
  m_pTypeInfo[MAGIC_CAUSE_CRITICAL_WOUNDS].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[MAGIC_CAUSE_CRITICAL_WOUNDS].AddNextSkill(
      SKILL_PROTECTION_FROM_BLOOD);

  m_pTypeInfo[SKILL_PROTECTION_FROM_BLOOD].Set(
      sid++, "Protection From Blood", 0, 0, 334, "Protection From Blood");
  m_pTypeInfo[SKILL_PROTECTION_FROM_BLOOD].SetMP(4);
  m_pTypeInfo[SKILL_PROTECTION_FROM_BLOOD].SetLearnLevel(73);
  m_pTypeInfo[SKILL_PROTECTION_FROM_BLOOD].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_PROTECTION_FROM_BLOOD].AddNextSkill(MAGIC_CURE_ALL);

  m_pTypeInfo[MAGIC_CURE_ALL].Set(sid++, "Cure All", 0, 0, 53, "Cure All");
  m_pTypeInfo[MAGIC_CURE_ALL].SetMP(30);
  m_pTypeInfo[MAGIC_CURE_ALL].SetLearnLevel(75);
  m_pTypeInfo[MAGIC_CURE_ALL].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[MAGIC_CURE_ALL].AddNextSkill(SKILL_VIGOR_DROP);

  m_pTypeInfo[SKILL_VIGOR_DROP].Set(sid++, "Vigor Drop", 0, 0, 54, "Vigor Drop");
  m_pTypeInfo[SKILL_VIGOR_DROP].SetMP(30);
  m_pTypeInfo[SKILL_VIGOR_DROP].SetLearnLevel(80);
  m_pTypeInfo[SKILL_VIGOR_DROP].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_VIGOR_DROP].AddNextSkill(SKILL_PEACE);

  m_pTypeInfo[SKILL_PEACE].Set(sid++, "Peace", 0, 0, 160, "Peace");
  m_pTypeInfo[SKILL_PEACE].SetMP(30);
  m_pTypeInfo[SKILL_PEACE].SetLearnLevel(85);
  m_pTypeInfo[SKILL_PEACE].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_PEACE].AddNextSkill(SKILL_ETERNITY);

  m_pTypeInfo[SKILL_ETERNITY].Set(sid++, "Eternity", 0, 0, 328, "Eternity");
  m_pTypeInfo[SKILL_ETERNITY].SetMP(30);
  m_pTypeInfo[SKILL_ETERNITY].SetLearnLevel(88);
  m_pTypeInfo[SKILL_ETERNITY].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_ETERNITY].AddNextSkill(SKILL_ACTIVATION);

  m_pTypeInfo[SKILL_ACTIVATION].Set(sid++, "Activation", 0, 0, 162,
                                    "Activation");
  m_pTypeInfo[SKILL_ACTIVATION].SetMP(25);
  m_pTypeInfo[SKILL_ACTIVATION].SetLearnLevel(90);
  m_pTypeInfo[SKILL_ACTIVATION].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_ACTIVATION].AddNextSkill(MAGIC_REGENERATION);

  m_pTypeInfo[MAGIC_REGENERATION].Set(sid++, "Regeneration", 0, 0, 220,
                                      "Regeneration");
  m_pTypeInfo[MAGIC_REGENERATION].SetMP(30);
  m_pTypeInfo[MAGIC_REGENERATION].SetLearnLevel(95);
  m_pTypeInfo[MAGIC_REGENERATION].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[MAGIC_REGENERATION].AddNextSkill(SKILL_TURN_UNDEAD);

  m_pTypeInfo[SKILL_TURN_UNDEAD].Set(sid++, "Turn Undead", 0, 0, 221,
                                     "Turn Undead");
  m_pTypeInfo[SKILL_TURN_UNDEAD].SetMP(30);
  m_pTypeInfo[SKILL_TURN_UNDEAD].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_TURN_UNDEAD].SetLearnLevel(100);
  m_pTypeInfo[SKILL_TURN_UNDEAD].AddNextSkill(SKILL_GREAT_HEAL);

  m_pTypeInfo[SKILL_GREAT_HEAL].Set(sid++, "Great Heal", 0, 0, 316,
                                    "Great Heal");
  m_pTypeInfo[SKILL_GREAT_HEAL].SetMP(30);
  m_pTypeInfo[SKILL_GREAT_HEAL].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_GREAT_HEAL].SetLearnLevel(110);
  m_pTypeInfo[SKILL_GREAT_HEAL].AddNextSkill(SKILL_ILLENDUE);

  m_pTypeInfo[SKILL_ILLENDUE].Set(sid++, "Illendue", 0, 0, 257, "Illendue");
  m_pTypeInfo[SKILL_ILLENDUE].SetMP(40);
  m_pTypeInfo[SKILL_ILLENDUE].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_ILLENDUE].SetLearnLevel(120);
  m_pTypeInfo[SKILL_ILLENDUE].AddNextSkill(SKILL_WHITSUNTIDE);

  // 2004, 9, 8, sobeit add start - �� 130 ��ų
  m_pTypeInfo[SKILL_WHITSUNTIDE].Set(sid++, "whitsuntide", 700, 270, 390,
                                     "whitsuntide");
  m_pTypeInfo[SKILL_WHITSUNTIDE].SetMP(40);
  m_pTypeInfo[SKILL_WHITSUNTIDE].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_WHITSUNTIDE].SetLearnLevel(130);
  m_pTypeInfo[SKILL_WHITSUNTIDE].AddNextSkill(SKILL_BOMBING_STAR);
  // 2004, 9, 8, sobeit add end - �� 130 ��ų

  // 2004, 11, 11, sobeit add start - �� 140 ��ų
  m_pTypeInfo[SKILL_BOMBING_STAR].Set(sid++, "Bombing Star", 700, 270, 461,
                                      "Bombing Star");
  m_pTypeInfo[SKILL_BOMBING_STAR].SetMP(40);
  m_pTypeInfo[SKILL_BOMBING_STAR].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_BOMBING_STAR].SetLearnLevel(140);
#if __CONTENTS(__SECOND_RARE_SKILL)
  m_pTypeInfo[SKILL_BOMBING_STAR].AddDelayGroup(SKILL_GENESIS);
#endif //__SECOND_RARE_SKILL
  m_pTypeInfo[SKILL_BOMBING_STAR].AddNextSkill(SKILL_PASSING_HEAL);
  // 2004, 11, 11, sobeit add end - �� 140 ��ų

  m_pTypeInfo[SKILL_PASSING_HEAL].Set(sid++, "Passing Heal", 700, 270, 472,
                                      "Passing Heal");
  m_pTypeInfo[SKILL_PASSING_HEAL].SetMP(40);
  m_pTypeInfo[SKILL_PASSING_HEAL].SetSkillStep(
      SKILL_STEP_SLAYER_HEAL_ADVANCEMENT);
  m_pTypeInfo[SKILL_PASSING_HEAL].SetLearnLevel(150);
  m_pTypeInfo[SKILL_PASSING_HEAL].AddNextSkill(SKILL_ILLUSION_INVERSION);

  m_pTypeInfo[SKILL_ILLUSION_INVERSION].Set(sid++, "Illusion Inversion", 700,
                                            270, 501, "Illusion Inversion");
  m_pTypeInfo[SKILL_ILLUSION_INVERSION].SetMP(40);
  m_pTypeInfo[SKILL_ILLUSION_INVERSION].SetSkillStep(
      SKILL_STEP_SLAYER_HEAL_ADVANCEMENT);
  m_pTypeInfo[SKILL_ILLUSION_INVERSION].SetLearnLevel(160);
  m_pTypeInfo[SKILL_ILLUSION_INVERSION].AddNextSkill(SKILL_SOUL_CHAIN);

  //----------------------------------------------------------------------
  // Enchant
  //----------------------------------------------------------------------
  sid = 0;
  m_pTypeInfo[MAGIC_CREATE_HOLY_WATER].Set(sid++, "Create Holy Water", 0, 0, 57,
                                           "Create Holy Water");
  m_pTypeInfo[MAGIC_CREATE_HOLY_WATER].SetMP(6);
  m_pTypeInfo[MAGIC_CREATE_HOLY_WATER].SetLearnLevel(5);
  m_pTypeInfo[MAGIC_CREATE_HOLY_WATER].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[MAGIC_CREATE_HOLY_WATER].AddNextSkill(SKILL_LIGHT_BALL);

  m_pTypeInfo[SKILL_LIGHT_BALL].Set(sid++, "Light Ball", 0, 0, 215,
                                    "Light Ball");
  m_pTypeInfo[SKILL_LIGHT_BALL].SetMP(6);
  m_pTypeInfo[SKILL_LIGHT_BALL].SetLearnLevel(8);
  m_pTypeInfo[SKILL_LIGHT_BALL].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_LIGHT_BALL].AddNextSkill(MAGIC_LIGHT);

  m_pTypeInfo[MAGIC_LIGHT].Set(sid++, "Light", 0, 0, 58, "Light");
  m_pTypeInfo[MAGIC_LIGHT].SetMP(4);
  m_pTypeInfo[MAGIC_LIGHT].SetLearnLevel(10);
  m_pTypeInfo[MAGIC_LIGHT].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[MAGIC_LIGHT].AddNextSkill(MAGIC_DETECT_HIDDEN);

  m_pTypeInfo[MAGIC_DETECT_HIDDEN].Set(sid++, "Detect Hidden", 0, 0, 59,
                                       "Detect Hidden");
  m_pTypeInfo[MAGIC_DETECT_HIDDEN].SetMP(4);
  m_pTypeInfo[MAGIC_DETECT_HIDDEN].SetLearnLevel(15);
  m_pTypeInfo[MAGIC_DETECT_HIDDEN].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[MAGIC_DETECT_HIDDEN].AddNextSkill(MAGIC_AURA_BALL);

  m_pTypeInfo[MAGIC_AURA_BALL].Set(sid++, "Aura Ball", 0, 0, 60, "Aura Ball");
  m_pTypeInfo[MAGIC_AURA_BALL].SetMP(6);
  m_pTypeInfo[MAGIC_AURA_BALL].SetLearnLevel(20);
  m_pTypeInfo[MAGIC_AURA_BALL].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[MAGIC_AURA_BALL].AddNextSkill(MAGIC_BLESS);

  m_pTypeInfo[MAGIC_BLESS].Set(sid++, "Bless", 0, 0, 61, "Bless");
  m_pTypeInfo[MAGIC_BLESS].SetMP(6);
  m_pTypeInfo[MAGIC_BLESS].SetLearnLevel(25);
  m_pTypeInfo[MAGIC_BLESS].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[MAGIC_BLESS].AddNextSkill(MAGIC_CONTINUAL_LIGHT);

  m_pTypeInfo[MAGIC_CONTINUAL_LIGHT].Set(sid++, "Continual Light", 0, 0, 62,
                                         "Continual Light");
  m_pTypeInfo[MAGIC_CONTINUAL_LIGHT].SetMP(10);
  m_pTypeInfo[MAGIC_CONTINUAL_LIGHT].SetLearnLevel(30);
  m_pTypeInfo[MAGIC_CONTINUAL_LIGHT].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[MAGIC_CONTINUAL_LIGHT].AddNextSkill(MAGIC_FLARE);

  m_pTypeInfo[MAGIC_FLARE].Set(sid++, "Flare", 0, 0, 63, "Flare");
  m_pTypeInfo[MAGIC_FLARE].SetMP(10);
  m_pTypeInfo[MAGIC_FLARE].SetLearnLevel(35);
  m_pTypeInfo[MAGIC_FLARE].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[MAGIC_FLARE].AddNextSkill(MAGIC_PURIFY);

  m_pTypeInfo[MAGIC_PURIFY].Set(sid++, "Purify", 0, 0, 64, "Purify");
  m_pTypeInfo[MAGIC_PURIFY].SetMP(10);
  m_pTypeInfo[MAGIC_PURIFY].SetLearnLevel(40);
  m_pTypeInfo[MAGIC_PURIFY].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[MAGIC_PURIFY].AddNextSkill(SKILL_MERCY_GROUND);

  m_pTypeInfo[SKILL_MERCY_GROUND].Set(sid++, "Mercy Ground", 0, 0, 332,
                                      "Mercy Ground");
  m_pTypeInfo[SKILL_MERCY_GROUND].SetMP(14);
  m_pTypeInfo[SKILL_MERCY_GROUND].SetLearnLevel(40);
  m_pTypeInfo[SKILL_MERCY_GROUND].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_MERCY_GROUND].AddNextSkill(MAGIC_AURA_RING);

  m_pTypeInfo[MAGIC_AURA_RING].Set(sid++, "Aura Ring", 0, 0, 65,
                                   "Aura Ring"); // 212);
  m_pTypeInfo[MAGIC_AURA_RING].SetMP(4);
  m_pTypeInfo[MAGIC_AURA_RING].SetLearnLevel(45);
  m_pTypeInfo[MAGIC_AURA_RING].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[MAGIC_AURA_RING].AddNextSkill(MAGIC_STRIKING);

  m_pTypeInfo[MAGIC_STRIKING].Set(sid++, "Striking", 0, 0, 66, "Striking");
  m_pTypeInfo[MAGIC_STRIKING].SetMP(6);
  m_pTypeInfo[MAGIC_STRIKING].SetLearnLevel(50);
  m_pTypeInfo[MAGIC_STRIKING].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[MAGIC_STRIKING].AddNextSkill(MAGIC_MEDITATION);

  m_pTypeInfo[MAGIC_MEDITATION].Set(sid++, "Meditation", 0, 0, 124,
                                    "Meditation");
  m_pTypeInfo[MAGIC_MEDITATION].SetMP(10);
  m_pTypeInfo[MAGIC_MEDITATION].SetLearnLevel(55);
  m_pTypeInfo[MAGIC_MEDITATION].SetPassive();
  m_pTypeInfo[MAGIC_MEDITATION].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[MAGIC_MEDITATION].AddNextSkill(MAGIC_DETECT_INVISIBILITY);

  m_pTypeInfo[MAGIC_DETECT_INVISIBILITY].Set(sid++, "Detect Invisibility", 0, 0,
                                             67, "Detect Invisibility");
  m_pTypeInfo[MAGIC_DETECT_INVISIBILITY].SetMP(10);
  m_pTypeInfo[MAGIC_DETECT_INVISIBILITY].SetLearnLevel(60);
  m_pTypeInfo[MAGIC_DETECT_INVISIBILITY].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[MAGIC_DETECT_INVISIBILITY].AddNextSkill(SKILL_REQUITAL);

  m_pTypeInfo[SKILL_REQUITAL].Set(sid++, "Requital", 0, 0, 251, "Requital");
  m_pTypeInfo[SKILL_REQUITAL].SetMP(10);
  m_pTypeInfo[SKILL_REQUITAL].SetLearnLevel(61);
  m_pTypeInfo[SKILL_REQUITAL].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_REQUITAL].AddNextSkill(MAGIC_AURA_SHIELD);

  m_pTypeInfo[MAGIC_AURA_SHIELD].Set(sid++, "Aura Shield", 0, 0, 69,
                                     "Aura Shield");
  m_pTypeInfo[MAGIC_AURA_SHIELD].SetMP(12);
  m_pTypeInfo[MAGIC_AURA_SHIELD].SetLearnLevel(65);
  m_pTypeInfo[MAGIC_AURA_SHIELD].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[MAGIC_AURA_SHIELD].AddNextSkill(MAGIC_VISIBLE);

  m_pTypeInfo[MAGIC_VISIBLE].Set(sid++, "Visible", 0, 0, 71, "Visible");
  m_pTypeInfo[MAGIC_VISIBLE].SetMP(12);
  m_pTypeInfo[MAGIC_VISIBLE].SetLearnLevel(70);
  m_pTypeInfo[MAGIC_VISIBLE].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[MAGIC_VISIBLE].AddNextSkill(SKILL_ENERGY_DROP);

  m_pTypeInfo[SKILL_ENERGY_DROP].Set(sid++, "Aura Storm", 0, 0, 54,
                                     "Aura Storm");
  m_pTypeInfo[SKILL_ENERGY_DROP].SetMP(12);
  m_pTypeInfo[SKILL_ENERGY_DROP].SetLearnLevel(75);
  m_pTypeInfo[SKILL_ENERGY_DROP].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_ENERGY_DROP].AddNextSkill(SKILL_SANCTUARY);

  m_pTypeInfo[SKILL_SANCTUARY].Set(sid++, "Sanctuary", 0, 0, 163, "Sanctuary");
  m_pTypeInfo[SKILL_SANCTUARY].SetMP(12);
  m_pTypeInfo[SKILL_SANCTUARY].SetLearnLevel(85);
  m_pTypeInfo[SKILL_SANCTUARY].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_SANCTUARY].AddNextSkill(SKILL_HOLY_ARMOR);

  m_pTypeInfo[SKILL_HOLY_ARMOR].Set(sid++, "Holy Armor", 0, 0, 330,
                                    "Holy Armor");
  m_pTypeInfo[SKILL_HOLY_ARMOR].SetMP(38);
  m_pTypeInfo[SKILL_HOLY_ARMOR].SetLearnLevel(90);
  m_pTypeInfo[SKILL_HOLY_ARMOR].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_HOLY_ARMOR].AddNextSkill(SKILL_REBUKE);
  // 20080226 Blood Bible ���÷����� �׿��� Blood Bible�ؾ� ��
  //	m_pTypeInfo[SKILL_REFLECTION].Set(sid++, "Reflection", 0, 0,
  //165,"���÷���"); 	m_pTypeInfo[SKILL_REFLECTION].SetMP( 12 );
  //	m_pTypeInfo[SKILL_REFLECTION].SetLearnLevel( 90 );
  //	m_pTypeInfo[SKILL_REFLECTION].SetSkillStep( SKILL_STEP_APPRENTICE );
  //	m_pTypeInfo[SKILL_REFLECTION].AddNextSkill( SKILL_REBUKE );

  m_pTypeInfo[SKILL_REBUKE].Set(sid++, "Rebuke", 0, 0, 222, "Rebuke");
  m_pTypeInfo[SKILL_REBUKE].SetMP(12);
  m_pTypeInfo[SKILL_REBUKE].SetLearnLevel(95);
  m_pTypeInfo[SKILL_REBUKE].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_REBUKE].AddNextSkill(SKILL_SPIRIT_GUARD);

  m_pTypeInfo[SKILL_SPIRIT_GUARD].Set(sid++, "Spirit Guard", 0, 0, 223,
                                      "Spirit Guard");
  m_pTypeInfo[SKILL_SPIRIT_GUARD].SetMP(12);
  m_pTypeInfo[SKILL_SPIRIT_GUARD].SetLearnLevel(100);
  m_pTypeInfo[SKILL_SPIRIT_GUARD].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_SPIRIT_GUARD].AddNextSkill(SKILL_DIVINE_GUIDANCE);

  m_pTypeInfo[SKILL_DIVINE_GUIDANCE].Set(sid++, "Divine Guidance", 0, 0, 317,
                                         "Divine Guidance");
  m_pTypeInfo[SKILL_DIVINE_GUIDANCE].SetMP(12);
  m_pTypeInfo[SKILL_DIVINE_GUIDANCE].SetLearnLevel(110);
  m_pTypeInfo[SKILL_DIVINE_GUIDANCE].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_DIVINE_GUIDANCE].AddNextSkill(SKILL_LIGHTNESS);

  m_pTypeInfo[SKILL_LIGHTNESS].Set(sid++, "Lightness", 0, 0, 258, "Lightness");
  m_pTypeInfo[SKILL_LIGHTNESS].SetMP(40);
  m_pTypeInfo[SKILL_LIGHTNESS].SetLearnLevel(100);
  m_pTypeInfo[SKILL_LIGHTNESS].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_LIGHTNESS].AddNextSkill(SKILL_SWEEP_VICE_1);

  // 2004, 9, 8, sobeit add start - ��æ 130 ��ų
  m_pTypeInfo[SKILL_SWEEP_VICE_1].Set(sid++, "Sweep vice", 700, 270, 391,
                                      "Sweep vice");
  m_pTypeInfo[SKILL_SWEEP_VICE_1].SetMP(40);
  m_pTypeInfo[SKILL_SWEEP_VICE_1].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_SWEEP_VICE_1].SetLearnLevel(130);
  m_pTypeInfo[SKILL_SWEEP_VICE_1].AddNextSkill(SKILL_INTIMATE_GRAIL);
  // 2004, 9, 8, sobeit add end - ��æ 130 ��ų

  // 2004, 11, 11, sobeit add start - ��æ 140 ��ų
  m_pTypeInfo[SKILL_INTIMATE_GRAIL].Set(sid++, "Intimate Grail", 700, 270, 462,
                                        "Intimate Grail");
  m_pTypeInfo[SKILL_INTIMATE_GRAIL].SetMP(40);
  m_pTypeInfo[SKILL_INTIMATE_GRAIL].SetSkillStep(SKILL_STEP_APPRENTICE);
  m_pTypeInfo[SKILL_INTIMATE_GRAIL].SetLearnLevel(140);
  m_pTypeInfo[SKILL_INTIMATE_GRAIL].AddNextSkill(SKILL_ROTTEN_APPLE);
  // 2004, 11, 11, sobeit add end - ��æ 140 ��ų

  m_pTypeInfo[SKILL_ROTTEN_APPLE].Set(sid++, "Rotten Apple", 700, 270, 473,
                                      "Rotten Apple");
  m_pTypeInfo[SKILL_ROTTEN_APPLE].SetMP(40);
  m_pTypeInfo[SKILL_ROTTEN_APPLE].SetSkillStep(
      SKILL_STEP_SLAYER_ENCHANT_ADVANCEMENT);
  m_pTypeInfo[SKILL_ROTTEN_APPLE].SetLearnLevel(150);
  m_pTypeInfo[SKILL_ROTTEN_APPLE].AddNextSkill(SKILL_HEAVEN_GROUND);
  // 2005, 1, 3, sobeit add end -

  m_pTypeInfo[SKILL_HEAVEN_GROUND].Set(sid++, "Heaven Ground", 700, 270, 502,
                                       "Heaven Ground");
  m_pTypeInfo[SKILL_HEAVEN_GROUND].SetMP(40);
  m_pTypeInfo[SKILL_HEAVEN_GROUND].SetSkillStep(
      SKILL_STEP_SLAYER_ENCHANT_ADVANCEMENT);
  m_pTypeInfo[SKILL_HEAVEN_GROUND].SetLearnLevel(160);
  m_pTypeInfo[SKILL_HEAVEN_GROUND].AddNextSkill(SKILL_SOUL_CHAIN);

  //----------------------------------------------------------------------
  // Vampire
  //----------------------------------------------------------------------
  sid = 0;
  m_pTypeInfo[MAGIC_HIDE].Set(sid++, "Hide", 0, 0, 96, "Hide");
  m_pTypeInfo[MAGIC_HIDE].SetVampireSkill();
  m_pTypeInfo[MAGIC_HIDE].SetSkillStep(SKILL_STEP_VAMPIRE_INNATE);
  m_pTypeInfo[MAGIC_HIDE].SetMP(8);
  m_pTypeInfo[MAGIC_HIDE].SetLearnLevel(3);
  m_pTypeInfo[MAGIC_HIDE].AddNextSkill(SKILL_HANDS_OF_WISDOM);

  m_pTypeInfo[SKILL_HANDS_OF_WISDOM].Set(sid++, "Hands of Wisdom", 0, 0, 213,
                                         "Hands of Wisdom");
  m_pTypeInfo[SKILL_HANDS_OF_WISDOM].SetVampireSkill();
  m_pTypeInfo[SKILL_HANDS_OF_WISDOM].SetSkillStep(SKILL_STEP_VAMPIRE_INNATE);
  m_pTypeInfo[SKILL_HANDS_OF_WISDOM].SetMP(15);
  m_pTypeInfo[SKILL_HANDS_OF_WISDOM].SetLearnLevel(5);
  m_pTypeInfo[SKILL_HANDS_OF_WISDOM].AddNextSkill(MAGIC_POISONOUS_HANDS);

  m_pTypeInfo[MAGIC_POISONOUS_HANDS].Set(sid++, "Poisonous Hands", 0, 0, 74,
                                         "Poisonous Hands");
  m_pTypeInfo[MAGIC_POISONOUS_HANDS].SetVampireSkill();
  m_pTypeInfo[MAGIC_POISONOUS_HANDS].SetSkillStep(SKILL_STEP_VAMPIRE_POISON);
  m_pTypeInfo[MAGIC_POISONOUS_HANDS].SetMP(15);
  m_pTypeInfo[MAGIC_POISONOUS_HANDS].SetLearnLevel(6);
  m_pTypeInfo[MAGIC_POISONOUS_HANDS].AddNextSkill(MAGIC_ACID_TOUCH);

  m_pTypeInfo[MAGIC_ACID_TOUCH].Set(sid++, "Acid Touch", 0, 0, 79,
                                    "Acid Touch");
  m_pTypeInfo[MAGIC_ACID_TOUCH].SetVampireSkill();
  m_pTypeInfo[MAGIC_ACID_TOUCH].SetSkillStep(SKILL_STEP_VAMPIRE_ACID);
  m_pTypeInfo[MAGIC_ACID_TOUCH].SetMP(20);
  m_pTypeInfo[MAGIC_ACID_TOUCH].SetLearnLevel(9);
  m_pTypeInfo[MAGIC_ACID_TOUCH].AddNextSkill(MAGIC_GREEN_POISON);

  m_pTypeInfo[MAGIC_GREEN_POISON].Set(sid++, "Green Poison", 0, 0, 75,
                                      "Green Poison");
  m_pTypeInfo[MAGIC_GREEN_POISON].SetVampireSkill();
  m_pTypeInfo[MAGIC_GREEN_POISON].SetSkillStep(SKILL_STEP_VAMPIRE_POISON);
  m_pTypeInfo[MAGIC_GREEN_POISON].SetMP(25);
  m_pTypeInfo[MAGIC_GREEN_POISON].SetLearnLevel(12);
  m_pTypeInfo[MAGIC_GREEN_POISON].AddNextSkill(MAGIC_DARKNESS);

  m_pTypeInfo[MAGIC_DARKNESS].Set(sid++, "Darkness", 0, 0, 97, "Darkness");
  m_pTypeInfo[MAGIC_DARKNESS].SetVampireSkill();
  m_pTypeInfo[MAGIC_DARKNESS].SetSkillStep(SKILL_STEP_VAMPIRE_INNATE);
  m_pTypeInfo[MAGIC_DARKNESS].SetMP(10);
  m_pTypeInfo[MAGIC_DARKNESS].SetLearnLevel(15);
  m_pTypeInfo[MAGIC_DARKNESS].AddNextSkill(MAGIC_YELLOW_POISON);

  m_pTypeInfo[MAGIC_YELLOW_POISON].Set(sid++, "Yellow Poison", 0, 0, 76,
                                       "Yellow Poison");
  m_pTypeInfo[MAGIC_YELLOW_POISON].SetVampireSkill();
  m_pTypeInfo[MAGIC_YELLOW_POISON].SetSkillStep(SKILL_STEP_VAMPIRE_POISON);
  m_pTypeInfo[MAGIC_YELLOW_POISON].SetMP(30);
  m_pTypeInfo[MAGIC_YELLOW_POISON].SetLearnLevel(18);
  m_pTypeInfo[MAGIC_YELLOW_POISON].AddNextSkill(MAGIC_TRANSFORM_TO_BAT);

  m_pTypeInfo[MAGIC_TRANSFORM_TO_BAT].Set(sid++, "Transform to Bat", 0, 0, 99,
                                          "Transform to Bat");
  m_pTypeInfo[MAGIC_TRANSFORM_TO_BAT].SetVampireSkill();
  m_pTypeInfo[MAGIC_TRANSFORM_TO_BAT].SetSkillStep(SKILL_STEP_VAMPIRE_INNATE);
  m_pTypeInfo[MAGIC_TRANSFORM_TO_BAT].SetMP(45);
  m_pTypeInfo[MAGIC_TRANSFORM_TO_BAT].SetLearnLevel(45);
  m_pTypeInfo[MAGIC_TRANSFORM_TO_BAT].AddNextSkill(MAGIC_ACID_BOLT);

  m_pTypeInfo[MAGIC_ACID_BOLT].Set(sid++, "Acid Bolt", 0, 0, 80, "Acid Bolt");
  m_pTypeInfo[MAGIC_ACID_BOLT].SetVampireSkill();
  m_pTypeInfo[MAGIC_ACID_BOLT].SetSkillStep(SKILL_STEP_VAMPIRE_ACID);
  m_pTypeInfo[MAGIC_ACID_BOLT].SetMP(30);
  m_pTypeInfo[MAGIC_ACID_BOLT].SetLearnLevel(21);
  m_pTypeInfo[MAGIC_ACID_BOLT].AddNextSkill(MAGIC_GREEN_STALKER);

  m_pTypeInfo[MAGIC_GREEN_STALKER].Set(sid++, "Green Stalker", 0, 0, 77,
                                       "Green Stalker");
  m_pTypeInfo[MAGIC_GREEN_STALKER].SetVampireSkill();
  m_pTypeInfo[MAGIC_GREEN_STALKER].SetSkillStep(SKILL_STEP_VAMPIRE_POISON);
  m_pTypeInfo[MAGIC_GREEN_STALKER].SetMP(35);
  m_pTypeInfo[MAGIC_GREEN_STALKER].SetLearnLevel(24);
  m_pTypeInfo[MAGIC_GREEN_STALKER].AddNextSkill(MAGIC_BLOODY_TUNNEL);

  m_pTypeInfo[MAGIC_BLOODY_TUNNEL].Set(sid++, "Bloody Tunnel", 0, 0, 117,
                                       "Bloody Tunnel");
  m_pTypeInfo[MAGIC_BLOODY_TUNNEL].SetVampireSkill();
  m_pTypeInfo[MAGIC_BLOODY_TUNNEL].SetSkillStep(SKILL_STEP_VAMPIRE_BLOOD);
  m_pTypeInfo[MAGIC_BLOODY_TUNNEL].SetMP(0);
  m_pTypeInfo[MAGIC_BLOODY_TUNNEL].SetLearnLevel(31);
  m_pTypeInfo[MAGIC_BLOODY_TUNNEL].AddNextSkill(MAGIC_PARALYZE);

  m_pTypeInfo[MAGIC_PARALYZE].Set(sid++, "Paralyze", 0, 0, 83, "Paralyze");
  m_pTypeInfo[MAGIC_PARALYZE].SetVampireSkill();
  m_pTypeInfo[MAGIC_PARALYZE].SetSkillStep(SKILL_STEP_VAMPIRE_CURSE);
  m_pTypeInfo[MAGIC_PARALYZE].SetMP(30);
  m_pTypeInfo[MAGIC_PARALYZE].SetLearnLevel(27);
  m_pTypeInfo[MAGIC_PARALYZE].AddNextSkill(MAGIC_BLOODY_MARK);

  m_pTypeInfo[MAGIC_BLOODY_MARK].Set(sid++, "Bloody Mark", 0, 0, 116,
                                     "Bloody Mark");
  m_pTypeInfo[MAGIC_BLOODY_MARK].SetVampireSkill();
  m_pTypeInfo[MAGIC_BLOODY_MARK].SetSkillStep(SKILL_STEP_VAMPIRE_BLOOD);
  m_pTypeInfo[MAGIC_BLOODY_MARK].SetMP(0);
  m_pTypeInfo[MAGIC_BLOODY_MARK].SetLearnLevel(34);
  m_pTypeInfo[MAGIC_BLOODY_MARK].AddNextSkill(MAGIC_DARKBLUE_POISON);

  m_pTypeInfo[MAGIC_DARKBLUE_POISON].Set(sid++, "DarkBlue Poison", 0, 0, 78,
                                         "DarkBlue Poison");
  m_pTypeInfo[MAGIC_DARKBLUE_POISON].SetVampireSkill();
  m_pTypeInfo[MAGIC_DARKBLUE_POISON].SetSkillStep(SKILL_STEP_VAMPIRE_POISON);
  m_pTypeInfo[MAGIC_DARKBLUE_POISON].SetMP(43);
  m_pTypeInfo[MAGIC_DARKBLUE_POISON].SetLearnLevel(30);
  m_pTypeInfo[MAGIC_DARKBLUE_POISON].AddNextSkill(MAGIC_TRANSFORM_TO_WOLF);

  m_pTypeInfo[MAGIC_TRANSFORM_TO_WOLF].Set(sid++, "Transform to Wolf", 0, 0, 98,
                                           "Transform to Wolf");
  m_pTypeInfo[MAGIC_TRANSFORM_TO_WOLF].SetVampireSkill();
  m_pTypeInfo[MAGIC_TRANSFORM_TO_WOLF].SetSkillStep(SKILL_STEP_VAMPIRE_INNATE);
  m_pTypeInfo[MAGIC_TRANSFORM_TO_WOLF].SetMP(45);
  m_pTypeInfo[MAGIC_TRANSFORM_TO_WOLF].SetLearnLevel(39);
  m_pTypeInfo[MAGIC_TRANSFORM_TO_WOLF].AddNextSkill(SKILL_POISON_STRIKE);

  m_pTypeInfo[SKILL_POISON_STRIKE].Set(sid++, "Poison Strike", 0, 0, 130,
                                       "Poison Strike");
  m_pTypeInfo[SKILL_POISON_STRIKE].SetVampireSkill();
  m_pTypeInfo[SKILL_POISON_STRIKE].SetSkillStep(SKILL_STEP_VAMPIRE_POISON);
  m_pTypeInfo[SKILL_POISON_STRIKE].SetMP(30);
  m_pTypeInfo[SKILL_POISON_STRIKE].SetLearnLevel(36);
  m_pTypeInfo[SKILL_POISON_STRIKE].AddNextSkill(MAGIC_DOOM);

  m_pTypeInfo[MAGIC_DOOM].Set(sid++, "Doom", 0, 0, 84, "Doom");
  m_pTypeInfo[MAGIC_DOOM].SetVampireSkill();
  m_pTypeInfo[MAGIC_DOOM].SetSkillStep(SKILL_STEP_VAMPIRE_CURSE);
  m_pTypeInfo[MAGIC_DOOM].SetMP(30);
  m_pTypeInfo[MAGIC_DOOM].SetLearnLevel(36);
  m_pTypeInfo[MAGIC_DOOM].AddNextSkill(MAGIC_ACID_BALL);

  m_pTypeInfo[MAGIC_ACID_BALL].Set(sid++, "Acid Ball", 0, 0, 81, "Acid Ball");
  m_pTypeInfo[MAGIC_ACID_BALL].SetVampireSkill();
  m_pTypeInfo[MAGIC_ACID_BALL].SetSkillStep(SKILL_STEP_VAMPIRE_ACID);
  m_pTypeInfo[MAGIC_ACID_BALL].SetMP(45);
  m_pTypeInfo[MAGIC_ACID_BALL].SetLearnLevel(42);
  m_pTypeInfo[MAGIC_ACID_BALL].AddNextSkill(MAGIC_INVISIBILITY);

  m_pTypeInfo[MAGIC_INVISIBILITY].Set(sid++, "Invisibllity", 0, 0, 100,
                                      "Invisibllity");
  m_pTypeInfo[MAGIC_INVISIBILITY].SetVampireSkill();
  m_pTypeInfo[MAGIC_INVISIBILITY].SetSkillStep(SKILL_STEP_VAMPIRE_INNATE);
  m_pTypeInfo[MAGIC_INVISIBILITY].SetMP(45);
  m_pTypeInfo[MAGIC_INVISIBILITY].SetLearnLevel(42);
  m_pTypeInfo[MAGIC_INVISIBILITY].AddNextSkill(MAGIC_RAPID_GLIDING);

  m_pTypeInfo[MAGIC_RAPID_GLIDING].Set(sid++, "Rapid Gliding", 0, 0, 244,
                                       "Rapid Gliding");
  m_pTypeInfo[MAGIC_RAPID_GLIDING].SetVampireSkill();
  m_pTypeInfo[MAGIC_RAPID_GLIDING].SetSkillStep(SKILL_STEP_VAMPIRE_INNATE);
  m_pTypeInfo[MAGIC_RAPID_GLIDING].SetMP(33);
  m_pTypeInfo[MAGIC_RAPID_GLIDING].SetLearnLevel(47);
  m_pTypeInfo[MAGIC_RAPID_GLIDING].AddNextSkill(SKILL_NAIL_MASTERY);

  m_pTypeInfo[SKILL_NAIL_MASTERY].Set(sid++, "Nail Mastery", 0, 0, 246,
                                      "Nail Mastery");
  m_pTypeInfo[SKILL_NAIL_MASTERY].SetSkillStep(SKILL_STEP_VAMPIRE_INNATE);
  m_pTypeInfo[SKILL_NAIL_MASTERY].SetVampireSkill();
  m_pTypeInfo[SKILL_NAIL_MASTERY].SetPassive();
  m_pTypeInfo[SKILL_NAIL_MASTERY].SetLearnLevel(57);
  m_pTypeInfo[SKILL_NAIL_MASTERY].AddNextSkill(SKILL_TRANSFUSION);

  m_pTypeInfo[SKILL_TRANSFUSION].Set(sid++, "Transfusion", 0, 0, 167,
                                     "Transfusion");
  m_pTypeInfo[SKILL_TRANSFUSION].SetVampireSkill();
  m_pTypeInfo[SKILL_TRANSFUSION].SetSkillStep(SKILL_STEP_VAMPIRE_BLOOD);
  m_pTypeInfo[SKILL_TRANSFUSION].SetMP(55);
  m_pTypeInfo[SKILL_TRANSFUSION].SetLearnLevel(50);
  m_pTypeInfo[SKILL_TRANSFUSION].AddNextSkill(SKILL_ACID_STRIKE);

  m_pTypeInfo[SKILL_ACID_STRIKE].Set(sid++, "Acid Strike", 0, 0, 132,
                                     "Acid Strike");
  m_pTypeInfo[SKILL_ACID_STRIKE].SetVampireSkill();
  m_pTypeInfo[SKILL_ACID_STRIKE].SetSkillStep(SKILL_STEP_VAMPIRE_ACID);
  m_pTypeInfo[SKILL_ACID_STRIKE].SetMP(55);
  m_pTypeInfo[SKILL_ACID_STRIKE].SetLearnLevel(65);
  m_pTypeInfo[SKILL_ACID_STRIKE].AddNextSkill(MAGIC_ACID_SWAMP);

  m_pTypeInfo[MAGIC_ACID_SWAMP].Set(sid++, "Acid Swamp", 0, 0, 82,
                                    "Acid Swamp");
  m_pTypeInfo[MAGIC_ACID_SWAMP].SetVampireSkill();
  m_pTypeInfo[MAGIC_ACID_SWAMP].SetSkillStep(SKILL_STEP_VAMPIRE_ACID);
  m_pTypeInfo[MAGIC_ACID_SWAMP].SetMP(55);
  m_pTypeInfo[MAGIC_ACID_SWAMP].SetLearnLevel(65);
  m_pTypeInfo[MAGIC_ACID_SWAMP].AddNextSkill(SKILL_POISON_STORM);

  m_pTypeInfo[SKILL_POISON_STORM].Set(sid++, "Poison Storm", 0, 0, 131,
                                      "Poison Storm");
  m_pTypeInfo[SKILL_POISON_STORM].SetVampireSkill();
  m_pTypeInfo[SKILL_POISON_STORM].SetSkillStep(SKILL_STEP_VAMPIRE_POISON);
  m_pTypeInfo[SKILL_POISON_STORM].SetMP(30);
  m_pTypeInfo[SKILL_POISON_STORM].SetLearnLevel(36);
  m_pTypeInfo[SKILL_POISON_STORM].AddNextSkill(MAGIC_SEDUCTION);

  m_pTypeInfo[MAGIC_SEDUCTION].Set(sid++, "Seduction", 0, 0, 119, "Seduction");
  m_pTypeInfo[MAGIC_SEDUCTION].SetVampireSkill();
  m_pTypeInfo[MAGIC_SEDUCTION].SetSkillStep(SKILL_STEP_VAMPIRE_CURSE);
  m_pTypeInfo[MAGIC_SEDUCTION].SetMP(55);
  m_pTypeInfo[MAGIC_SEDUCTION].SetLearnLevel(48);
  m_pTypeInfo[MAGIC_SEDUCTION].AddNextSkill(MAGIC_BLOODY_NAIL);

  m_pTypeInfo[SKILL_BLOODY_WAVE].Set(sid++, "Bloody Wave", 0, 0, 174,
                                     "Bloody Wave");
  m_pTypeInfo[SKILL_BLOODY_WAVE].SetVampireSkill();
  m_pTypeInfo[SKILL_BLOODY_WAVE].SetSkillStep(SKILL_STEP_VAMPIRE_BLOOD);
  m_pTypeInfo[SKILL_BLOODY_WAVE].SetMP(90);
  m_pTypeInfo[SKILL_BLOODY_WAVE].SetLearnLevel(64);
  m_pTypeInfo[SKILL_BLOODY_WAVE].AddNextSkill(SKILL_BLOODY_STRIKE);

  m_pTypeInfo[SKILL_BLOODY_STRIKE].Set(sid++, "Bloody Strike", 0, 0, 250,
                                       "Bloody Strike");
  m_pTypeInfo[SKILL_BLOODY_STRIKE].SetVampireSkill();
  m_pTypeInfo[SKILL_BLOODY_STRIKE].SetSkillStep(SKILL_STEP_VAMPIRE_BLOOD);
  m_pTypeInfo[SKILL_BLOODY_STRIKE].SetMP(76);
  m_pTypeInfo[SKILL_BLOODY_STRIKE].SetLearnLevel(66);
  m_pTypeInfo[SKILL_BLOODY_STRIKE].AddNextSkill(SKILL_ACID_STORM);

  m_pTypeInfo[MAGIC_BLOODY_NAIL].Set(sid++, "Bloody Nail", 0, 0, 127,
                                     "Bloody Nail");
  m_pTypeInfo[MAGIC_BLOODY_NAIL].SetVampireSkill();
  m_pTypeInfo[MAGIC_BLOODY_NAIL].SetSkillStep(SKILL_STEP_VAMPIRE_BLOOD);
  m_pTypeInfo[MAGIC_BLOODY_NAIL].SetMP(60);
  m_pTypeInfo[MAGIC_BLOODY_NAIL].SetLearnLevel(75);
  m_pTypeInfo[MAGIC_BLOODY_NAIL].AddNextSkill(MAGIC_BLOODY_KNIFE);

  m_pTypeInfo[SKILL_ACID_STORM].Set(sid++, "Acid Storm", 0, 0, 133,
                                    "Acid Storm");
  m_pTypeInfo[SKILL_ACID_STORM].SetVampireSkill();
  m_pTypeInfo[SKILL_ACID_STORM].SetSkillStep(SKILL_STEP_VAMPIRE_ACID);
  m_pTypeInfo[SKILL_ACID_STORM].SetMP(55);
  m_pTypeInfo[SKILL_ACID_STORM].SetLearnLevel(65);
  m_pTypeInfo[SKILL_ACID_STORM].AddNextSkill(SKILL_EXTREME);

  m_pTypeInfo[MAGIC_BLOODY_KNIFE].Set(sid++, "Bloody Knife", 0, 0, 128,
                                      "Bloody Knife");
  m_pTypeInfo[MAGIC_BLOODY_KNIFE].SetVampireSkill();
  m_pTypeInfo[MAGIC_BLOODY_KNIFE].SetSkillStep(SKILL_STEP_VAMPIRE_BLOOD);
  m_pTypeInfo[MAGIC_BLOODY_KNIFE].SetMP(63);
  m_pTypeInfo[MAGIC_BLOODY_KNIFE].SetLearnLevel(58);
  m_pTypeInfo[MAGIC_BLOODY_KNIFE].AddNextSkill(SKILL_BLOODY_WAVE);

  m_pTypeInfo[SKILL_EXTREME].Set(sid++, "Dark Revenge", 0, 0, 134,
                                 "Dark Revenge");
  m_pTypeInfo[SKILL_EXTREME].SetVampireSkill();
  m_pTypeInfo[SKILL_EXTREME].SetSkillStep(SKILL_STEP_VAMPIRE_INNATE);
  m_pTypeInfo[SKILL_EXTREME].SetMP(63);
  m_pTypeInfo[SKILL_EXTREME].SetLearnLevel(63);
  m_pTypeInfo[SKILL_EXTREME].AddNextSkill(MAGIC_BLOODY_BALL);

  m_pTypeInfo[MAGIC_BLOODY_BALL].Set(sid++, "Bloody Ball", 0, 0, 129,
                                     "Bloody Ball");
  m_pTypeInfo[MAGIC_BLOODY_BALL].SetVampireSkill();
  m_pTypeInfo[MAGIC_BLOODY_BALL].SetSkillStep(SKILL_STEP_VAMPIRE_BLOOD);
  m_pTypeInfo[MAGIC_BLOODY_BALL].SetMP(65);
  m_pTypeInfo[MAGIC_BLOODY_BALL].SetLearnLevel(66);
  m_pTypeInfo[MAGIC_BLOODY_BALL].AddNextSkill(MAGIC_SUMMON_CASKET);

  m_pTypeInfo[MAGIC_SUMMON_CASKET].Set(sid++, "Summon Casket", 0, 0, 120,
                                       "Summon Casket");
  m_pTypeInfo[MAGIC_SUMMON_CASKET].SetVampireSkill();
  m_pTypeInfo[MAGIC_SUMMON_CASKET].SetSkillStep(SKILL_STEP_VAMPIRE_SUMMON);
  m_pTypeInfo[MAGIC_SUMMON_CASKET].SetMP(70);
  m_pTypeInfo[MAGIC_SUMMON_CASKET].SetLearnLevel(55);
  m_pTypeInfo[MAGIC_SUMMON_CASKET].AddNextSkill(SKILL_STONE_SKIN);

  //	m_pTypeInfo[SKILL_CHARM].Set(sid++, "Kidnap", 0, 0, 135,"Kidnap");
  //	m_pTypeInfo[SKILL_CHARM].SetVampireSkill();
  //	m_pTypeInfo[SKILL_CHARM].SetSkillStep( SKILL_STEP_VAMPIRE_INNATE );
  //	m_pTypeInfo[SKILL_CHARM].SetMP( 30 );
  //	m_pTypeInfo[SKILL_CHARM].SetLearnLevel( 73 );
  //	m_pTypeInfo[SKILL_CHARM].AddNextSkill( MAGIC_DEATH );

  m_pTypeInfo[SKILL_STONE_SKIN].Set(sid++, "Stone Skin", 0, 0, 338,
                                    "Stone Skin");
  m_pTypeInfo[SKILL_STONE_SKIN].SetVampireSkill();
  m_pTypeInfo[SKILL_STONE_SKIN].SetSkillStep(SKILL_STEP_VAMPIRE_INNATE);
  m_pTypeInfo[SKILL_STONE_SKIN].SetMP(10);
  m_pTypeInfo[SKILL_STONE_SKIN].SetLearnLevel(68);
  m_pTypeInfo[SKILL_STONE_SKIN].AddNextSkill(SKILL_TRANSFORM_TO_WERWOLF);

  m_pTypeInfo[SKILL_TRANSFORM_TO_WERWOLF].Set(sid++, "Transform To Werwolf", 0,
                                              0, 336, "Transform To Werwolf");
  m_pTypeInfo[SKILL_TRANSFORM_TO_WERWOLF].SetVampireSkill();
  m_pTypeInfo[SKILL_TRANSFORM_TO_WERWOLF].SetSkillStep(
      SKILL_STEP_VAMPIRE_INNATE);
  m_pTypeInfo[SKILL_TRANSFORM_TO_WERWOLF].SetMP(10);
  m_pTypeInfo[SKILL_TRANSFORM_TO_WERWOLF].SetLearnLevel(72);
  m_pTypeInfo[SKILL_TRANSFORM_TO_WERWOLF].AddNextSkill(SKILL_GRAY_DARKNESS);

  m_pTypeInfo[SKILL_GRAY_DARKNESS].Set(sid++, "Gray Darkness", 0, 0, 339,
                                       "Gray Darkness");
  m_pTypeInfo[SKILL_GRAY_DARKNESS].SetVampireSkill();
  m_pTypeInfo[SKILL_GRAY_DARKNESS].SetSkillStep(SKILL_STEP_VAMPIRE_INNATE);
  m_pTypeInfo[SKILL_GRAY_DARKNESS].SetMP(10);
  m_pTypeInfo[SKILL_GRAY_DARKNESS].SetLearnLevel(78);
  m_pTypeInfo[SKILL_GRAY_DARKNESS].AddNextSkill(SKILL_ACID_ERUPTION);

  m_pTypeInfo[SKILL_ACID_ERUPTION].Set(sid++, "Acid Eruption", 0, 0, 340,
                                       "Acid Eruption");
  m_pTypeInfo[SKILL_ACID_ERUPTION].SetVampireSkill();
  m_pTypeInfo[SKILL_ACID_ERUPTION].SetSkillStep(SKILL_STEP_VAMPIRE_ACID);
  m_pTypeInfo[SKILL_ACID_ERUPTION].SetMP(60);
  m_pTypeInfo[SKILL_ACID_ERUPTION].SetLearnLevel(80);
  m_pTypeInfo[SKILL_ACID_ERUPTION].AddNextSkill(SKILL_TALON_OF_CROW);

  m_pTypeInfo[SKILL_TALON_OF_CROW].Set(sid++, "Talon Of Crow", 0, 0, 337,
                                       "Talon Of Crow");
  m_pTypeInfo[SKILL_TALON_OF_CROW].SetVampireSkill();
  m_pTypeInfo[SKILL_TALON_OF_CROW].SetSkillStep(SKILL_STEP_VAMPIRE_INNATE);
  m_pTypeInfo[SKILL_TALON_OF_CROW].SetMP(20);
  m_pTypeInfo[SKILL_TALON_OF_CROW].SetLearnLevel(86);
  m_pTypeInfo[SKILL_TALON_OF_CROW].AddNextSkill(MAGIC_HOWL);

  m_pTypeInfo[MAGIC_HOWL].Set(sid++, "Howl", 0, 0, 122, "Howl");
  m_pTypeInfo[MAGIC_HOWL].SetVampireSkill();
  m_pTypeInfo[MAGIC_HOWL].SetSkillStep(SKILL_STEP_VAMPIRE_INNATE);
  m_pTypeInfo[MAGIC_HOWL].SetMP(10);
  m_pTypeInfo[MAGIC_HOWL].SetLearnLevel(40);
  m_pTypeInfo[MAGIC_HOWL].AddNextSkill(MAGIC_DEATH);

  m_pTypeInfo[MAGIC_DEATH].Set(sid++, "Death", 0, 0, 86, "Death");
  m_pTypeInfo[MAGIC_DEATH].SetVampireSkill();
  m_pTypeInfo[MAGIC_DEATH].SetSkillStep(SKILL_STEP_VAMPIRE_CURSE);
  m_pTypeInfo[MAGIC_DEATH].SetMP(55);
  m_pTypeInfo[MAGIC_DEATH].SetLearnLevel(79);
  m_pTypeInfo[MAGIC_DEATH].AddNextSkill(MAGIC_HALLUCINATION);

  m_pTypeInfo[MAGIC_HALLUCINATION].Set(sid++, "Hallucination", 0, 0, 85,
                                       "Hallucination");
  m_pTypeInfo[MAGIC_HALLUCINATION].SetVampireSkill();
  m_pTypeInfo[MAGIC_HALLUCINATION].SetSkillStep(SKILL_STEP_VAMPIRE_CURSE);
  m_pTypeInfo[MAGIC_HALLUCINATION].SetMP(40);
  m_pTypeInfo[MAGIC_HALLUCINATION].SetLearnLevel(120);
  m_pTypeInfo[MAGIC_HALLUCINATION].AddNextSkill(MAGIC_BLOODY_SPEAR);

  m_pTypeInfo[MAGIC_BLOODY_SPEAR].Set(sid++, "Bloody Spear", 0, 0, 169,
                                      "Bloody Spear");
  m_pTypeInfo[MAGIC_BLOODY_SPEAR].SetVampireSkill();
  m_pTypeInfo[MAGIC_BLOODY_SPEAR].SetSkillStep(SKILL_STEP_VAMPIRE_BLOOD);
  m_pTypeInfo[MAGIC_BLOODY_SPEAR].SetMP(90);
  m_pTypeInfo[MAGIC_BLOODY_SPEAR].SetLearnLevel(94);
  m_pTypeInfo[MAGIC_BLOODY_SPEAR].AddNextSkill(MAGIC_BLOODY_WALL);

  m_pTypeInfo[MAGIC_BLOODY_WALL].Set(sid++, "Bloody Wall", 0, 0, 168,
                                     "Bloody Wall");
  m_pTypeInfo[MAGIC_BLOODY_WALL].SetVampireSkill();
  m_pTypeInfo[MAGIC_BLOODY_WALL].SetSkillStep(SKILL_STEP_VAMPIRE_BLOOD);
  m_pTypeInfo[MAGIC_BLOODY_WALL].SetMP(75);
  m_pTypeInfo[MAGIC_BLOODY_WALL].SetLearnLevel(95);
  m_pTypeInfo[MAGIC_BLOODY_WALL].AddNextSkill(SKILL_MEPHISTO);

  m_pTypeInfo[SKILL_MEPHISTO].Set(sid++, "Mephistopheles", 0, 0, 171,
                                  "Mephistopheles");
  m_pTypeInfo[SKILL_MEPHISTO].SetVampireSkill();
  m_pTypeInfo[SKILL_MEPHISTO].SetSkillStep(SKILL_STEP_VAMPIRE_SUMMON);
  m_pTypeInfo[SKILL_MEPHISTO].SetMP(90);
  m_pTypeInfo[SKILL_MEPHISTO].SetLearnLevel(96);
  m_pTypeInfo[SKILL_MEPHISTO].AddNextSkill(MAGIC_BLOODY_BREAKER);

  m_pTypeInfo[MAGIC_BLOODY_BREAKER].Set(0, "Bloody Breaker", 0, 0, 243,
                                        "Bloody Breaker");
  m_pTypeInfo[MAGIC_BLOODY_BREAKER].SetSkillStep(SKILL_STEP_VAMPIRE_BLOOD);
  m_pTypeInfo[MAGIC_BLOODY_BREAKER].SetVampireSkill();
  m_pTypeInfo[MAGIC_BLOODY_BREAKER].SetMP(60);
  m_pTypeInfo[MAGIC_BLOODY_BREAKER].SetLearnLevel(99);
  //	m_pTypeInfo[MAGIC_BLOODY_BREAKER].AddNextSkill( SKILL_BLOODY_STRIKE );
  m_pTypeInfo[MAGIC_BLOODY_BREAKER].AddNextSkill(MAGIC_BLOODY_ZENITH);

  m_pTypeInfo[MAGIC_BLOODY_ZENITH].Set(0, "Bloody Zenith", 0, 0, 318,
                                       "Bloody Zenith");
  m_pTypeInfo[MAGIC_BLOODY_ZENITH].SetSkillStep(SKILL_STEP_VAMPIRE_BLOOD);
  m_pTypeInfo[MAGIC_BLOODY_ZENITH].SetVampireSkill();
  m_pTypeInfo[MAGIC_BLOODY_ZENITH].SetMP(60);
  m_pTypeInfo[MAGIC_BLOODY_ZENITH].SetLearnLevel(110);
  m_pTypeInfo[MAGIC_BLOODY_ZENITH].AddNextSkill(MAGIC_METEOR);

  //	m_pTypeInfo[SKILL_BLOODY_STRIKE].Set(sid++, "Bloody Strike", 0, 0,
  //136,"������ ��Ʈ����ũ");
  //	m_pTypeInfo[SKILL_BLOODY_STRIKE].SetVampireSkill();
  //	m_pTypeInfo[SKILL_BLOODY_STRIKE].SetSkillStep( SKILL_STEP_VAMPIRE_BLOOD
  //); 	m_pTypeInfo[SKILL_BLOODY_STRIKE].SetMP( 76 );
  //	m_pTypeInfo[SKILL_BLOODY_STRIKE].SetLearnLevel( 100 );
  //	m_pTypeInfo[SKILL_BLOODY_STRIKE].AddNextSkill( MAGIC_METEOR );

  //	m_pTypeInfo[MAGIC_RAISING_DEAD].Set(sid++, "Raising Dead", 0, 0,
  //173,"RaisingDead"); 	m_pTypeInfo[MAGIC_RAISING_DEAD].SetVampireSkill();
  //	m_pTypeInfo[MAGIC_RAISING_DEAD].SetSkillStep( SKILL_STEP_VAMPIRE_SUMMON
  //); 	m_pTypeInfo[MAGIC_RAISING_DEAD].SetMP( 55 );
  //	m_pTypeInfo[MAGIC_RAISING_DEAD].SetLearnLevel( 100 );
  //	m_pTypeInfo[MAGIC_RAISING_DEAD].AddNextSkill( SKILL_SUMMON_BAT );

  //	m_pTypeInfo[SKILL_SUMMON_BAT].Set(sid++, "Summon Bat", 0, 0,
  //87,"SummonBat");//324); 	m_pTypeInfo[SKILL_SUMMON_BAT].SetVampireSkill();
  //	m_pTypeInfo[SKILL_SUMMON_BAT].SetSkillStep( SKILL_STEP_VAMPIRE_SUMMON );
  //	m_pTypeInfo[SKILL_SUMMON_BAT].SetMP( 90 );
  //	m_pTypeInfo[SKILL_SUMMON_BAT].SetLearnLevel( 100 );
  //	m_pTypeInfo[SKILL_SUMMON_BAT].AddNextSkill( MAGIC_SUMMON_SERVANT );
  //
  //	m_pTypeInfo[MAGIC_SUMMON_SERVANT].Set(sid++, "Summon Servant", 0, 0,
  //170,"SummonServant"); 	m_pTypeInfo[MAGIC_SUMMON_SERVANT].SetVampireSkill();
  //	m_pTypeInfo[MAGIC_SUMMON_SERVANT].SetSkillStep(
  //SKILL_STEP_VAMPIRE_SUMMON ); 	m_pTypeInfo[MAGIC_SUMMON_SERVANT].SetMP( 93 );
  //	m_pTypeInfo[MAGIC_SUMMON_SERVANT].SetLearnLevel( 100 );
  //	m_pTypeInfo[MAGIC_SUMMON_SERVANT].AddNextSkill( MAGIC_METEOR );

  m_pTypeInfo[MAGIC_METEOR].Set(sid++, "Meteor Strike", 0, 0, 242,
                                "Meteor Strike");
  m_pTypeInfo[MAGIC_METEOR].SetVampireSkill();
  m_pTypeInfo[MAGIC_METEOR].SetSkillStep(SKILL_STEP_VAMPIRE_INNATE);
  m_pTypeInfo[MAGIC_METEOR].SetMP(70);
  m_pTypeInfo[MAGIC_METEOR].SetLearnLevel(100);
  m_pTypeInfo[MAGIC_METEOR].AddNextSkill(SKILL_VIOLENT_PHANTOM);

  //	m_pTypeInfo[SKILL_ARMAGEDDON].Set(sid++, "Armageddon", 0, 0,
  //172,"Armageddon"); 	m_pTypeInfo[SKILL_ARMAGEDDON].SetVampireSkill();
  //	m_pTypeInfo[SKILL_ARMAGEDDON].SetSkillStep( SKILL_STEP_VAMPIRE_BLOOD );
  //	m_pTypeInfo[SKILL_ARMAGEDDON].SetMP( 75 );
  //	m_pTypeInfo[SKILL_ARMAGEDDON].SetLearnLevel( 99 );
  //	m_pTypeInfo[SKILL_ARMAGEDDON].AddNextSkill( SKILL_BLOODY_STORM );

  //	m_pTypeInfo[SKILL_BLOODY_STORM].Set(sid++, "Bloody Storm", 0, 0,
  //93,"BloodyStorm"); 	m_pTypeInfo[SKILL_BLOODY_STORM].SetVampireSkill();
  //	m_pTypeInfo[SKILL_BLOODY_STORM].SetSkillStep( SKILL_STEP_VAMPIRE_BLOOD
  //); 	m_pTypeInfo[SKILL_BLOODY_STORM].SetMP( 75 );
  //	m_pTypeInfo[SKILL_BLOODY_STORM].SetLearnLevel( 100 );
  //	m_pTypeInfo[SKILL_BLOODY_STORM].AddNextSkill( SKILL_SOUL_CHAIN );

  // 2004, 9, 8, sobeit add start - �����̾� 130 ��ų
  m_pTypeInfo[SKILL_VIOLENT_PHANTOM].Set(sid++, "Violent phantom", 0, 0, 392,
                                         "Violent phantom");
  m_pTypeInfo[SKILL_VIOLENT_PHANTOM].SetVampireSkill();
  m_pTypeInfo[SKILL_VIOLENT_PHANTOM].SetSkillStep(SKILL_STEP_VAMPIRE_INNATE);
  m_pTypeInfo[SKILL_VIOLENT_PHANTOM].SetMP(20);
  m_pTypeInfo[SKILL_VIOLENT_PHANTOM].SetLearnLevel(130);
  m_pTypeInfo[SKILL_VIOLENT_PHANTOM].AddNextSkill(SKILL_SUMMON_GORE_GLAND);

  m_pTypeInfo[SKILL_SUMMON_GORE_GLAND].Set(sid++, "Summon Gore gland", 0, 0,
                                           393, "Summon Gore gland");
  m_pTypeInfo[SKILL_SUMMON_GORE_GLAND].SetVampireSkill();
  m_pTypeInfo[SKILL_SUMMON_GORE_GLAND].SetSkillStep(SKILL_STEP_VAMPIRE_SUMMON);
  m_pTypeInfo[SKILL_SUMMON_GORE_GLAND].SetMP(20);
  m_pTypeInfo[SKILL_SUMMON_GORE_GLAND].SetLearnLevel(130);
  m_pTypeInfo[SKILL_SUMMON_GORE_GLAND].AddNextSkill(SKILL_SET_AFIRE);
  // 2004, 9, 8, sobeit add end - �����̾� 130 ��ų

  // 2004, 11, 11, sobeit add start - �����̾� 140 ��ų
  m_pTypeInfo[SKILL_SET_AFIRE].Set(sid++, "Set Afire", 0, 0, 463,
                                   "Set Afire");
  m_pTypeInfo[SKILL_SET_AFIRE].SetVampireSkill();
  m_pTypeInfo[SKILL_SET_AFIRE].SetSkillStep(SKILL_STEP_VAMPIRE_INNATE);
  m_pTypeInfo[SKILL_SET_AFIRE].SetMP(20);
  m_pTypeInfo[SKILL_SET_AFIRE].SetLearnLevel(140);
  m_pTypeInfo[SKILL_SET_AFIRE].AddNextSkill(SKILL_NOOSE_OF_WRAITH);

  m_pTypeInfo[SKILL_NOOSE_OF_WRAITH].Set(sid++, "Noose of Wraith", 0, 0, 464,
                                         "Noose of Wraith");
  m_pTypeInfo[SKILL_NOOSE_OF_WRAITH].SetVampireSkill();
  m_pTypeInfo[SKILL_NOOSE_OF_WRAITH].SetSkillStep(SKILL_STEP_VAMPIRE_SUMMON);
  m_pTypeInfo[SKILL_NOOSE_OF_WRAITH].SetMP(20);
  m_pTypeInfo[SKILL_NOOSE_OF_WRAITH].SetLearnLevel(140);
  m_pTypeInfo[SKILL_NOOSE_OF_WRAITH].AddNextSkill(SKILL_ABERRATION);
  // 2004, 11, 11, sobeit add end - �����̾� 140 ��ų

  // 2005.1.5 by csm  2�� Blood Bible�̾� ��ų�߰�
  m_pTypeInfo[SKILL_ABERRATION].Set(sid++, "Aberration", 0, 0, 475,
                                    "Aberration");
  m_pTypeInfo[SKILL_ABERRATION].SetVampireSkill();
  m_pTypeInfo[SKILL_ABERRATION].SetSkillStep(
      SKILL_STEP_VAMPIRE_MAGICIAN_ADVANCEMENT);
  m_pTypeInfo[SKILL_ABERRATION].SetMP(20);
  m_pTypeInfo[SKILL_ABERRATION].SetLearnLevel(150);
  m_pTypeInfo[SKILL_ABERRATION].AddNextSkill(
      SKILL_WILD_WOLF); // SKILL_BLOODS_SYMPOSION) ; //SKILL_WILD_WOLF );

  m_pTypeInfo[SKILL_WILD_WOLF].Set(sid++, "Wild Wolf", 0, 0, 474,
                                   "Wild Wolf");
  m_pTypeInfo[SKILL_WILD_WOLF].SetVampireSkill();
  m_pTypeInfo[SKILL_WILD_WOLF].SetSkillStep(
      SKILL_STEP_VAMPIRE_WARRIOR_ADVANCEMENT);
  m_pTypeInfo[SKILL_WILD_WOLF].SetMP(20);
  m_pTypeInfo[SKILL_WILD_WOLF].SetLearnLevel(150);
  m_pTypeInfo[SKILL_WILD_WOLF].AddNextSkill(SKILL_BLOODS_SYMPOSION_ATTACK);

  // 2006.10.31 sjheon

  // ���� �⿬�� ���� ���ִ� ����� ��ų�̴�.
  // ����ڰ� ��ų�� ��� Blood Bible �⿬�� ���δ�.
  m_pTypeInfo[SKILL_BLOODS_SYMPOSION_ATTACK].Set(
      sid++, "Blood's Symposion Attack", 0, 0, 504, "Blood's Symposion Attack");
  m_pTypeInfo[SKILL_BLOODS_SYMPOSION_ATTACK].SetVampireSkill();
  m_pTypeInfo[SKILL_BLOODS_SYMPOSION_ATTACK].SetSkillStep(
      SKILL_STEP_VAMPIRE_WARRIOR_ADVANCEMENT);
  m_pTypeInfo[SKILL_BLOODS_SYMPOSION_ATTACK].SetMP(70);
  m_pTypeInfo[SKILL_BLOODS_SYMPOSION_ATTACK].SetLearnLevel(160);
  m_pTypeInfo[SKILL_BLOODS_SYMPOSION_ATTACK].AddNextSkill(
      SKILL_BLOOD_CURSE); // SKILL_WILD_WOLF );

  m_pTypeInfo[SKILL_BLOOD_CURSE].Set(0, "Blood Curse", 0, 0, 503, "Blood Curse");
  m_pTypeInfo[SKILL_BLOOD_CURSE].SetVampireSkill();
  m_pTypeInfo[SKILL_BLOOD_CURSE].SetSkillStep(
      SKILL_STEP_VAMPIRE_WARRIOR_ADVANCEMENT);
  m_pTypeInfo[SKILL_BLOOD_CURSE].SetLearnLevel(160);
  m_pTypeInfo[SKILL_BLOOD_CURSE].SetMP(60);
  m_pTypeInfo[SKILL_BLOOD_CURSE].AddNextSkill(SKILL_BLOODY_SCARIFY);

  m_pTypeInfo[SKILL_BLOODY_SCARIFY].Set(sid++, "Bloody Scarify", 0, 0, 506,
                                        "Bloody Scarify");
  m_pTypeInfo[SKILL_BLOODY_SCARIFY].SetVampireSkill();
  m_pTypeInfo[SKILL_BLOODY_SCARIFY].SetSkillStep(
      SKILL_STEP_VAMPIRE_MAGICIAN_ADVANCEMENT);
  m_pTypeInfo[SKILL_BLOODY_SCARIFY].SetMP(120);
  m_pTypeInfo[SKILL_BLOODY_SCARIFY].SetLearnLevel(160);
  m_pTypeInfo[SKILL_BLOODY_SCARIFY].AddNextSkill(SKILL_SOUL_CHAIN);

  // 2006.02.20 �����޺� ��ų �߰�
  //	m_pTypeInfo[SKILL_VAMPIRE_COMBO].Set(sid++, "Vampire Combo", 0, 0, 474,
  //"�����̾��޺�"); 	m_pTypeInfo[SKILL_VAMPIRE_COMBO].SetVampireSkill();
  //	m_pTypeInfo[SKILL_VAMPIRE_COMBO].SetSkillStep(
  //SKILL_STEP_VAMPIRE_ADVANCEMENT ); 	m_pTypeInfo[SKILL_VAMPIRE_COMBO].SetMP( 20
  //); 	m_pTypeInfo[SKILL_VAMPIRE_COMBO].SetLearnLevel( 150 );
  //	m_pTypeInfo[SKILL_VAMPIRE_COMBO].AddNextSkill( SKILL_SOUL_CHAIN );

  //----------------------------------------------------------------------
  // Ousters
  //----------------------------------------------------------------------
  sid = 0;
  int skillID;

  int NumSkillIcon = 259;
  skillID = SKILL_FLOURISH;
  m_pTypeInfo[skillID].Set(sid++, "FLOURISH", 7, 21, NumSkillIcon++,
                           "FLOURISH");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_COMBAT);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_EVADE);

  skillID = SKILL_EVADE;
  m_pTypeInfo[skillID].Set(sid++, "EVADE", 22, 84, NumSkillIcon++, "EVADE");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_COMBAT);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_SHARP_ROUND);

  skillID = SKILL_SHARP_ROUND;
  m_pTypeInfo[skillID].Set(sid++, "SHARP_ROUND", 151, 81, NumSkillIcon++,
                           "SHARP_ROUND");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_COMBAT);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_HIDE_SIGHT);

  skillID = SKILL_HIDE_SIGHT;
  m_pTypeInfo[skillID].Set(sid++, "HIDE_SIGHT", 168, 272, NumSkillIcon++,
                           "HIDE_SIGHT");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetPassive();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_COMBAT);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_BACK_STAB);

  skillID = SKILL_BACK_STAB;
  m_pTypeInfo[skillID].Set(sid++, "BACK_STAB", 17, 144, NumSkillIcon++,
                           "BACK_STAB");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_COMBAT);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_BLUNTING);

  skillID = SKILL_BLUNTING;
  m_pTypeInfo[skillID].Set(sid++, "BLUNTING", 213, 95, NumSkillIcon++,
                           "BLUNTING");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_COMBAT);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_GAMMA_CHOP);

  skillID = SKILL_GAMMA_CHOP;
  m_pTypeInfo[skillID].Set(sid++, "GAMMA_CHOP", 271, 219, NumSkillIcon++,
                           "GAMMA_CHOP");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_COMBAT);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_CROSS_GUARD);

  skillID = SKILL_CROSS_GUARD;
  m_pTypeInfo[skillID].Set(sid++, "CROSS_GUARD", 71, 199, NumSkillIcon++,
                           "CROSS_GUARD");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_COMBAT);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_KASAS_ARROW);

  //////////////////////////////////////////////////////////////////////////
  // elemental
  //////////////////////////////////////////////////////////////////////////

  skillID = SKILL_KASAS_ARROW;
  m_pTypeInfo[skillID].Set(sid++, "KASAS_ARROW", 298, 34, NumSkillIcon++,
                           "KASAS_ARROW");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_ELEMENTAL);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_HANDS_OF_FIRE);

  skillID = SKILL_HANDS_OF_FIRE;
  m_pTypeInfo[skillID].Set(sid++, "HANDS_OF_FIRE", 403, 153, NumSkillIcon++,
                           "HANDS_OF_FIRE");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_ELEMENTAL);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_PROMINENCE);

  skillID = SKILL_PROMINENCE;
  m_pTypeInfo[skillID].Set(sid++, "PROMINENCE", 166, 285, NumSkillIcon++,
                           "PROMINENCE");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_ELEMENTAL);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_RING_OF_FLARE);

  skillID = SKILL_RING_OF_FLARE;
  m_pTypeInfo[skillID].Set(sid++, "RING_OF_FLARE", 190, 345, NumSkillIcon++,
                           "RING_OF_FLARE");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_ELEMENTAL);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_BLAZE_BOLT);

  skillID = SKILL_BLAZE_BOLT;
  m_pTypeInfo[skillID].Set(sid++, "BLAZE_BOLT", 288, 424, NumSkillIcon++,
                           "BLAZE_BOLT");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_ELEMENTAL);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_ICE_FIELD);

  skillID = SKILL_ICE_FIELD;
  m_pTypeInfo[skillID].Set(sid++, "ICE_FIELD", 129, 157, NumSkillIcon++,
                           "ICE_FIELD");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_ELEMENTAL);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_WATER_BARRIER);

  skillID = SKILL_WATER_BARRIER;
  m_pTypeInfo[skillID].Set(sid++, "WATER_BARRIER", 112, 223, NumSkillIcon++,
                           "WATER_BARRIER");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_ELEMENTAL);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_HANDS_OF_NIZIE);

  skillID = SKILL_HANDS_OF_NIZIE;
  m_pTypeInfo[skillID].Set(sid++, "HANDS_OF_NIZIE", 357, 257, NumSkillIcon++,
                           "HANDS_OF_NIZIE");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_ELEMENTAL);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_NYMPH_RECOVERY);

  skillID = SKILL_NYMPH_RECOVERY;
  m_pTypeInfo[skillID].Set(sid++, "NYMPH_RECOVERY", 438, 289, NumSkillIcon++,
                           "NYMPH_RECOVERY");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_ELEMENTAL);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_LIBERTY);

  skillID = SKILL_LIBERTY;
  m_pTypeInfo[skillID].Set(sid++, "LIBERTY", 201, 407, NumSkillIcon++,
                           "LIBERTY");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_ELEMENTAL);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_TENDRIL);

  skillID = SKILL_TENDRIL;
  m_pTypeInfo[skillID].Set(sid++, "TENDRIL", 304, 104, NumSkillIcon++,
                           "TENDRIL");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_ELEMENTAL);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_GNOMES_WHISPER);

  skillID = SKILL_GNOMES_WHISPER;
  m_pTypeInfo[skillID].Set(sid++, "GNOMES_WHISPER", 222, 160, NumSkillIcon++,
                           "GNOMES_WHISPER");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_ELEMENTAL);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_STONE_AUGER);

  skillID = SKILL_STONE_AUGER;
  m_pTypeInfo[skillID].Set(sid++, "STONE_AUGER", 308, 161, NumSkillIcon++,
                           "STONE_AUGER");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_ELEMENTAL);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_REFUSAL_ETHER);

  skillID = SKILL_REFUSAL_ETHER;
  m_pTypeInfo[skillID].Set(sid++, "REFUSAL_ETHER", 245, 296, NumSkillIcon++,
                           "REFUSAL_ETHER");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_ELEMENTAL);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_EARTHS_TEETH);

  skillID = SKILL_EARTHS_TEETH;
  m_pTypeInfo[skillID].Set(sid++, "EARTHS_TEETH", 376, 425, NumSkillIcon++,
                           "EARTHS_TEETH");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_ELEMENTAL);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_ABSORB_SOUL);

  skillID = SKILL_ABSORB_SOUL;
  m_pTypeInfo[skillID].Set(sid++, "ABSORB_SOUL", MAKELONG(159, 13),
                           MAKELONG(17, 14), NumSkillIcon++, "ABSORB_SOUL");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_ETC);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_SUMMON_SYLPH);

  skillID = SKILL_SUMMON_SYLPH;
  m_pTypeInfo[skillID].Set(sid++, "SUMMON_SYLPH", MAKELONG(76, 148),
                           MAKELONG(17, 30), NumSkillIcon++, "SUMMON_SYLPH");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_ETC);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_DRIFTING_SOUL);

  skillID = SKILL_DRIFTING_SOUL;
  m_pTypeInfo[skillID].Set(sid++, "DRIFTING_SOUL", 266, 232, NumSkillIcon++,
                           "DRIFTING_SOUL");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetPassive();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_ELEMENTAL);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_CRITICAL_MAGIC);

  skillID = SKILL_CRITICAL_MAGIC;
  m_pTypeInfo[skillID].Set(sid++, "CRITICAL_MAGIC", 240, 492, NumSkillIcon++,
                           "CRITICAL_MAGIC");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetPassive();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_ELEMENTAL);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_EMISSION_WATER);

  skillID = SKILL_EMISSION_WATER;
  m_pTypeInfo[skillID].Set(sid++, "SKILL_EMISSION_WATER", 71, 94, 309,
                           "SKILL_EMISSION_WATER");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_ELEMENTAL);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_BEAT_HEAD);

  skillID = SKILL_BEAT_HEAD;
  m_pTypeInfo[skillID].Set(sid++, "SKILL_BEAT_HEAD", 229, 84, 310,
                           "SKILL_BEAT_HEAD");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_ELEMENTAL);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_FIRE_OF_SOUL_STONE);

  skillID = SKILL_FIRE_OF_SOUL_STONE;
  m_pTypeInfo[skillID].Set(sid++, "FIRE_OF_SOUL_STONE", 110, 138,
                           NumSkillIcon++, "FIRE_OF_SOUL_STONE");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetPassive();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_COMBAT);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_ICE_OF_SOUL_STONE);

  skillID = SKILL_ICE_OF_SOUL_STONE;
  m_pTypeInfo[skillID].Set(sid++, "ICE_OF_SOUL_STONE", 94, 258, NumSkillIcon++,
                           "ICE_OF_SOUL_STONE");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetPassive();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_COMBAT);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_SAND_OF_SOUL_STONE);

  skillID = SKILL_SAND_OF_SOUL_STONE;
  m_pTypeInfo[skillID].Set(sid++, "SAND_OF_SOUL_STONE", 204, 213,
                           NumSkillIcon++, "SAND_OF_SOUL_STONE");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetPassive();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_COMBAT);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_BLOCK_HEAD);

  skillID = SKILL_BLOCK_HEAD;
  m_pTypeInfo[skillID].Set(sid++, "BLOCK_HEAD", 226, 156, NumSkillIcon++,
                           "BLOCK_HEAD");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetPassive();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_COMBAT);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_DIVINE_SPIRITS);

  skillID = SKILL_DIVINE_SPIRITS;
  m_pTypeInfo[skillID].Set(sid++, "DIVINE_SPIRITS", MAKELONG(85, 154),
                           MAKELONG(78, 98), 311, "DIVINE_SPIRITS");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_ETC);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_TELEPORT);

  skillID = SKILL_TELEPORT;
  m_pTypeInfo[skillID].Set(sid++, "SKILL_TELEPORT", MAKELONG(141, 271),
                           MAKELONG(195, 357), 357, "TELEPORT");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_ETC);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_SOUL_REBIRTH);

  skillID = SKILL_SOUL_REBIRTH;
  m_pTypeInfo[skillID].Set(sid++, "SKILL_SOUL_REBIRTH", 368, 347, 358,
                           "REBIRTH");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_ELEMENTAL);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_ICE_LANCE);

  skillID = SKILL_ICE_LANCE;
  m_pTypeInfo[skillID].Set(sid++, "SKILL_ICE_LANCE", 337, 515, 359,
                           "ICE_LANCE");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_ELEMENTAL);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_ICE_LANCE_MASTERY);

  skillID = SKILL_ICE_LANCE_MASTERY;
  m_pTypeInfo[skillID].Set(sid++, "SKILL_ICE_LANCE_MASTERY", 337, 800, 359,
                           "ICE_LANCE_MASTERY");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetPassive();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_ELEMENTAL);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_EXPLOSION_WATER);

  skillID = SKILL_EXPLOSION_WATER;
  m_pTypeInfo[skillID].Set(sid++, "SKILL_EXPLOSION_WATER", 455, 621, 360,
                           "EXPLOSION_WATER");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_ELEMENTAL);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_FROZEN_ARMOR);

  skillID = SKILL_FROZEN_ARMOR;
  m_pTypeInfo[skillID].Set(sid++, "SKILL_FROZEN_ARMOR", 399, 561, 361,
                           "FROZEN_ARMOR");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_ELEMENTAL);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_FIRE_PIERCING);

  skillID = SKILL_FIRE_PIERCING;
  m_pTypeInfo[skillID].Set(sid++, "SKILL_FIRE_PIERCING", 233, 555, 362,
                           "FIRE_PIERCING");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_ELEMENTAL);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_SUMMON_OF_FIRE_ELEMENTAL);

  skillID = SKILL_SUMMON_OF_FIRE_ELEMENTAL;
  m_pTypeInfo[skillID].Set(sid++, "SKILL_SUMMON_OF_FIRE_ELEMENTAL", 199, 617,
                           363, "SUMMON_OF_FIRE_ELEMENTAL");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_ELEMENTAL);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_MAGNUM_SPEAR);

  skillID = SKILL_MAGNUM_SPEAR;
  m_pTypeInfo[skillID].Set(sid++, "SKILL_MAGNUM_SPEAR", 294, 591, 364,
                           "MAGNUM_SPEAR");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_ELEMENTAL);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_MAGNUM_SPEAR_MASTERY);

  skillID = SKILL_MAGNUM_SPEAR_MASTERY;
  m_pTypeInfo[skillID].Set(sid++, "SKILL_MAGNUM_SPEAR_MASTERY", 263, 800, 364,
                           "MAGNUM_SPEAR_MASTERY");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetPassive();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_ELEMENTAL);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_HELLFIRE);

  skillID = SKILL_HELLFIRE;
  m_pTypeInfo[skillID].Set(sid++, "SKILL_HELLFIRE", 204, 702, 365, "HELLFIRE");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_ELEMENTAL);
  m_pTypeInfo[skillID].AddDelayGroup(SKILL_FIERCE_FLAME);
  //	m_pTypeInfo[skillID].AddNextSkill( SKILL_SUMMON_OF_WATER_ELEMENTAL );
  m_pTypeInfo[skillID].AddNextSkill(SKILL_SUMMON_MIGA);

  skillID = SKILL_SUMMON_MIGA;
  m_pTypeInfo[skillID].Set(sid++, "SKILL_SUMMON_MIGA", 204, 860, 466,
                           "SUMMON_MIGA");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_ELEMENTAL);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_SUMMON_OF_WATER_ELEMENTAL);

  skillID = SKILL_SUMMON_OF_WATER_ELEMENTAL;
  m_pTypeInfo[skillID].Set(sid++, "SKILL_SUMMON_OF_WATER_ELEMENTAL", 378, 621,
                           366, "SUMMON_OF_WATER_ELEMENTAL");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_ELEMENTAL);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_SOUL_REBIRTH_MASTERY);

  skillID = SKILL_SOUL_REBIRTH_MASTERY;
  m_pTypeInfo[skillID].Set(sid++, "SKILL_SOUL_REBIRTH_MASTERY", 416, 688, 367,
                           "SOUL_REBIRTH_MASTERY");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetPassive();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_ELEMENTAL);
  //	m_pTypeInfo[skillID].AddNextSkill( SKILL_REACTIVE_ARMOR );
  m_pTypeInfo[skillID].AddNextSkill(SKILL_ICE_HORIZON);

  skillID = SKILL_ICE_HORIZON;
  m_pTypeInfo[skillID].Set(sid++, "SKILL_ICE_HORIZON", 416, 860, 467,
                           "ICE_HORIZON");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_ELEMENTAL);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_REACTIVE_ARMOR);

  skillID = SKILL_REACTIVE_ARMOR;
  m_pTypeInfo[skillID].Set(sid++, "SKILL_REACTIVE_ARMOR", 139, 521, 368,
                           "REACTIVE_ARMOR");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_ELEMENTAL);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_REACTIVE_ARMOR_MASTERY);

  skillID = SKILL_REACTIVE_ARMOR_MASTERY;
  m_pTypeInfo[skillID].Set(sid++, "SKILL_REACTIVE_ARMOR_MASTERY", 139, 800, 368,
                           "REACTIVE_ARMOR_MASTERY");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetPassive();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_ELEMENTAL);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_GROUND_BLESS);

  skillID = SKILL_GROUND_BLESS;
  m_pTypeInfo[skillID].Set(sid++, "SKILL_GROUND_BLESS", 85, 583, 369,
                           "GROUND_BLESS");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_ELEMENTAL);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_SUMMON_OF_GROUND_ELEMENTAL);

  skillID = SKILL_SUMMON_OF_GROUND_ELEMENTAL;
  m_pTypeInfo[skillID].Set(sid++, "SKILL_SUMMON_OF_GROUND_ELEMENTAL", 37, 650,
                           370, "SUMMON_OF_GROUND_ELEMENTAL");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_ELEMENTAL);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_METEOR_STORM);

  skillID = SKILL_METEOR_STORM;
  m_pTypeInfo[skillID].Set(sid++, "SKILL_METEOR_STORM", 100, 702, 371,
                           "METEOR_STORM");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_ELEMENTAL);
#if __CONTENTS(__SECOND_RARE_SKILL)
  m_pTypeInfo[skillID].AddDelayGroup(SKILL_ASTEROID_BOMBER);
#endif //__SECOND_RARE_SKILL
       //	m_pTypeInfo[skillID].AddNextSkill( SKILL_SHARP_CHAKRAM );
  m_pTypeInfo[skillID].AddNextSkill(SKILL_FURY_OF_GNOME);

  skillID = SKILL_FURY_OF_GNOME;
  m_pTypeInfo[skillID].Set(sid++, "SKILL_FURY_OF_GNOME", 100, 860, 468,
                           "FURY_OF_GNOME");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_ELEMENTAL);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_SHARP_CHAKRAM);

  skillID = SKILL_SHARP_CHAKRAM;
  m_pTypeInfo[skillID].Set(sid++, "SKILL_SHARP_CHAKRAM", 34, 291, 372,
                           "SHARP_CHAKRAM");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_COMBAT);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_SHIFT_BREAK);

  skillID = SKILL_SHIFT_BREAK;
  m_pTypeInfo[skillID].Set(sid++, "SKILL_SHIFT_BREAK", 154, 331, 373,
                           "SHIFT_BREAK");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_COMBAT);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_WATER_SHIELD);

  skillID = SKILL_WATER_SHIELD;
  m_pTypeInfo[skillID].Set(sid++, "SKILL_WATER_SHIELD", 263, 287, 374,
                           "WATER_SHIELD");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetPassive();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_COMBAT);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_DESTRUCTION_SPEAR);

  skillID = SKILL_DESTRUCTION_SPEAR;
  m_pTypeInfo[skillID].Set(sid++, "SKILL_DESTRUCTION_SPEAR", 84, 352, 375,
                           "DESTRUCTION_SPEAR");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_COMBAT);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_DESTRUCTION_SPEAR_MASTERY);

  skillID = SKILL_DESTRUCTION_SPEAR_MASTERY;
  m_pTypeInfo[skillID].Set(sid++, "SKILL_DESTRUCTION_SPEAR_MASTERY", 84, 510,
                           375, "DESTRUCTION_SPEAR_MASTERY");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetPassive();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_COMBAT);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_BLESS_FIRE);

  skillID = SKILL_BLESS_FIRE;
  m_pTypeInfo[skillID].Set(sid++, "SKILL_BLESS_FIRE", 210, 380, 376,
                           "BLESS_FIRE");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetPassive();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_COMBAT);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_FATAL_SNICK);

  skillID = SKILL_FATAL_SNICK;
  m_pTypeInfo[skillID].Set(sid++, "SKILL_FATAL_SNICK", 340, 343, 377,
                           "FATAL_SNICK");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_COMBAT);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_FATAL_SNICK_MASTERY);

  skillID = SKILL_FATAL_SNICK_MASTERY;
  m_pTypeInfo[skillID].Set(sid++, "SKILL_FATAL_SNICK_MASTERY", 340, 510, 377,
                           "FATAL_SNICK_MASTERY");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetPassive();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_COMBAT);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_SAND_CROSS);

  skillID = SKILL_SAND_CROSS;
  m_pTypeInfo[skillID].Set(sid++, "SKILL_SAND_CROSS", 47, 413, 378,
                           "SAND_CROSS");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetPassive();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_COMBAT);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_DUCKING_WALLOP);

  skillID = SKILL_DUCKING_WALLOP;
  m_pTypeInfo[skillID].Set(sid++, "SKILL_DUCKING_WALLOP", 152, 412, 379,
                           "DUCKING_WALLOP");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_COMBAT);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_CHARGING_ATTACK);

  skillID = SKILL_CHARGING_ATTACK;
  m_pTypeInfo[skillID].Set(sid++, "SKILL_CHARGING_ATTACK", 285, 409, 380,
                           "CHARGING_ATTACK");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_COMBAT);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_DISTANCE_BLITZ);

  skillID = SKILL_DISTANCE_BLITZ;
  m_pTypeInfo[skillID].Set(sid++, "SKILL_DISTANCE_BLITZ", 224, 450, 381,
                           "DISTANCE_BLITZ");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_COMBAT);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_SHARP_HAIL);

  skillID = SKILL_SHARP_HAIL;
  m_pTypeInfo[skillID].Set(sid++, "SKILL_SHARP_HAIL", 224, 565, 465,
                           "SHARP_HAIL");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_COMBAT);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_SOUL_CHAIN);

  // ��Ÿ �迭
  m_pTypeInfo[SKILL_SOUL_CHAIN].Set(sid++, "Soul Chain", MAKELONG(250, 400),
                                    MAKELONG(20, 40), 175, "Soul Chain");
  //	m_pTypeInfo[SKILL_SOUL_CHAIN].SetVampireSkill();
  m_pTypeInfo[SKILL_SOUL_CHAIN].SetSkillStep(SKILL_STEP_ETC);
  //	m_pTypeInfo[SKILL_SOUL_CHAIN].SetSkillStep( SKILL_STEP_VAMPIRE_INNATE );
  m_pTypeInfo[SKILL_SOUL_CHAIN].SetMP(75);
  m_pTypeInfo[SKILL_SOUL_CHAIN].SetLearnLevel(100);
  m_pTypeInfo[SKILL_SOUL_CHAIN].AddNextSkill(SKILL_MAGIC_ELUSION);

  m_pTypeInfo[SKILL_MAGIC_ELUSION].Set(sid++, "Magic Elusion", 0, 0, 226,
                                       "Magic Elusion");
  m_pTypeInfo[SKILL_MAGIC_ELUSION].SetSlayerSkill();
  m_pTypeInfo[SKILL_MAGIC_ELUSION].SetSkillStep(SKILL_STEP_ETC);
  m_pTypeInfo[SKILL_MAGIC_ELUSION].SetMP(50);
  m_pTypeInfo[SKILL_MAGIC_ELUSION].SetLearnLevel(0);
  m_pTypeInfo[SKILL_MAGIC_ELUSION].AddNextSkill(SKILL_ILLUSION_OF_AVENGE);

  m_pTypeInfo[SKILL_ILLUSION_OF_AVENGE].Set(sid++, "Illusion Of Avenge", 0, 0,
                                            227, "Illusion Of Avenge");
  m_pTypeInfo[SKILL_ILLUSION_OF_AVENGE].SetSlayerSkill();
  m_pTypeInfo[SKILL_ILLUSION_OF_AVENGE].SetSkillStep(SKILL_STEP_ETC);
  m_pTypeInfo[SKILL_ILLUSION_OF_AVENGE].SetMP(50);
  m_pTypeInfo[SKILL_ILLUSION_OF_AVENGE].SetLearnLevel(0);
  m_pTypeInfo[SKILL_ILLUSION_OF_AVENGE].AddNextSkill(SKILL_POISON_MESH);

  m_pTypeInfo[SKILL_POISON_MESH].Set(sid++, "Poison Mesh", 0, 0, 228,
                                     "Poison Mesh");
  m_pTypeInfo[SKILL_POISON_MESH].SetSlayerSkill();
  m_pTypeInfo[SKILL_POISON_MESH].SetSkillStep(SKILL_STEP_ETC);
  m_pTypeInfo[SKILL_POISON_MESH].SetMP(50);
  m_pTypeInfo[SKILL_POISON_MESH].SetLearnLevel(0);
  m_pTypeInfo[SKILL_POISON_MESH].AddNextSkill(SKILL_WILL_OF_LIFE);

  m_pTypeInfo[SKILL_WILL_OF_LIFE].Set(sid++, "Will Of Life", 0, 0, 229,
                                      "Will Of Life");
  m_pTypeInfo[SKILL_WILL_OF_LIFE].SetSlayerSkill();
  m_pTypeInfo[SKILL_WILL_OF_LIFE].SetSkillStep(SKILL_STEP_ETC);
  m_pTypeInfo[SKILL_WILL_OF_LIFE].SetMP(50);
  m_pTypeInfo[SKILL_WILL_OF_LIFE].SetLearnLevel(0);
  m_pTypeInfo[SKILL_WILL_OF_LIFE].AddNextSkill(SKILL_DELEO_EFFICIO);

  // 2004, 9, 4, sobeit add start - �ƿ콺Blood Bible�� ��ų
  m_pTypeInfo[SKILL_DELEO_EFFICIO].Set(sid++, "DELEO EFFICIO", 0, 0, 385,
                                       "DELEO EFFICIO"); // 385
  m_pTypeInfo[SKILL_DELEO_EFFICIO].SetOustersSkill();
  m_pTypeInfo[SKILL_DELEO_EFFICIO].SetSkillStep(SKILL_STEP_ETC);
  m_pTypeInfo[SKILL_DELEO_EFFICIO].SetMP(50);
  m_pTypeInfo[SKILL_DELEO_EFFICIO].SetLearnLevel(0);
  m_pTypeInfo[SKILL_DELEO_EFFICIO].AddNextSkill(SKILL_REPUTO_FACTUM);

  m_pTypeInfo[SKILL_REPUTO_FACTUM].Set(sid++, "REPUTO FACTUM", 0, 0, 386,
                                       "REPUTO FACTUM"); // 386
  m_pTypeInfo[SKILL_REPUTO_FACTUM].SetOustersSkill();
  m_pTypeInfo[SKILL_REPUTO_FACTUM].SetSkillStep(SKILL_STEP_ETC);
  m_pTypeInfo[SKILL_REPUTO_FACTUM].SetMP(50);
  m_pTypeInfo[SKILL_REPUTO_FACTUM].SetLearnLevel(0);
  m_pTypeInfo[SKILL_REPUTO_FACTUM].AddNextSkill(SKILL_TANNING);
  // 2004, 9, 4, sobeit add end - �ƿ콺Blood Bible�� ��ų

  // 2006.01.19 chyaya add start - �������� ���ѷα�
  m_pTypeInfo[SKILL_TANNING].Set(sid++, "Tanning", 0, 0, 511, "Tanning");
  m_pTypeInfo[SKILL_TANNING].SetSkillStep(SKILL_STEP_ETC);
  m_pTypeInfo[SKILL_TANNING].SetMP(0);
  m_pTypeInfo[SKILL_TANNING].SetLearnLevel(0);
  m_pTypeInfo[SKILL_TANNING].SetHideInSkillTree(true);
  m_pTypeInfo[SKILL_TANNING].AddNextSkill(SKILL_MAGIC_SHIELD1);
  // 2006.01.19 chyaya add end - �������� ���ѷα�

  //----------------------------------------------------------------------
  //
  //							Blood Bible�
  //
  //----------------------------------------------------------------------
  // ��ųBlood Bible� ��� �� �ִ� Blood Bible�
  //----------------------------------------------------------------------

  // �����̾�
  skillID = SKILL_MAGIC_SHIELD1;
  m_pTypeInfo[skillID].Set(sid++, "Magic Shield 1", 0, 0, 526, "Magic Shield 1");
  m_pTypeInfo[skillID].SetDescription(
      "���� �ð� Blood Bible ����Blood Bible Blood Bible.");
  m_pTypeInfo[skillID].SetSlayerSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_SLAYER_RARE);
  m_pTypeInfo[skillID].SetMP(30);
  m_pTypeInfo[skillID].SetLearnLevel(60);
  m_pTypeInfo[skillID].SetHideInSkillTree(true);
  m_pTypeInfo[skillID].AddDelayGroup(SKILL_MAGIC_SHIELD2);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_MAGIC_SHIELD2);

  skillID = SKILL_MAGIC_SHIELD2;
  m_pTypeInfo[skillID].Set(sid++, "Magic Shield 2", 0, 0, 527, "Magic Shield 2");
  m_pTypeInfo[skillID].SetDescription("Blood Bible1�� ���׷��̵� ��ų, ���� �ð� "
                                      "Blood Bible ����Blood Bible Blood Bible.");
  m_pTypeInfo[skillID].SetSlayerSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_SLAYER_RARE);
  m_pTypeInfo[skillID].SetMP(40);
  m_pTypeInfo[skillID].SetLearnLevel(120);
  m_pTypeInfo[skillID].SetHideInSkillTree(true);
  m_pTypeInfo[skillID].AddDelayGroup(SKILL_MAGIC_SHIELD1);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_EAGLES_EYE1);

  skillID = SKILL_EAGLES_EYE1;
  m_pTypeInfo[skillID].Set(sid++, "Eagle`s Eye 1", 0, 0, 521, "Eagle`s Eye 1");
  m_pTypeInfo[skillID].SetDescription("");
  m_pTypeInfo[skillID].SetSlayerSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_SLAYER_RARE);
  m_pTypeInfo[skillID].SetMP(30);
  m_pTypeInfo[skillID].SetLearnLevel(60);
  m_pTypeInfo[skillID].SetHideInSkillTree(true);
  m_pTypeInfo[skillID].AddDelayGroup(SKILL_EAGLES_EYE2);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_EAGLES_EYE2);

  skillID = SKILL_EAGLES_EYE2;
  m_pTypeInfo[skillID].Set(sid++, "Eagle`s Eye 2", 0, 0, 522, "Eagle`s Eye 2");
  m_pTypeInfo[skillID].SetDescription("");
  m_pTypeInfo[skillID].SetSlayerSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_SLAYER_RARE);
  m_pTypeInfo[skillID].SetMP(40);
  m_pTypeInfo[skillID].SetLearnLevel(120);
  m_pTypeInfo[skillID].SetHideInSkillTree(true);
  m_pTypeInfo[skillID].AddDelayGroup(SKILL_EAGLES_EYE1);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_FREEZE_RING1);

  skillID = SKILL_FREEZE_RING1;
  m_pTypeInfo[skillID].Set(sid++, "Freeze Ring 1", 0, 0, 523, "Freeze Ring 1");
  m_pTypeInfo[skillID].SetDescription("");
  m_pTypeInfo[skillID].SetSlayerSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_SLAYER_RARE);
  m_pTypeInfo[skillID].SetMP(20);
  m_pTypeInfo[skillID].SetLearnLevel(20);
  m_pTypeInfo[skillID].SetHideInSkillTree(true);
  m_pTypeInfo[skillID].AddDelayGroup(SKILL_FREEZE_RING2);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_FREEZE_RING2);

  skillID = SKILL_FREEZE_RING2;
  m_pTypeInfo[skillID].Set(sid++, "Freeze Ring 2", 0, 0, 524, "Freeze Ring 2");
  m_pTypeInfo[skillID].SetDescription("");
  m_pTypeInfo[skillID].SetSlayerSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_SLAYER_RARE);
  m_pTypeInfo[skillID].SetMP(30);
  m_pTypeInfo[skillID].SetLearnLevel(80);
  m_pTypeInfo[skillID].SetHideInSkillTree(true);
  m_pTypeInfo[skillID].AddDelayGroup(SKILL_FREEZE_RING1);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_BAT_STORM1);

  // �����̾�
  skillID = SKILL_BAT_STORM1;
  m_pTypeInfo[skillID].Set(sid++, "Bat Storm 1", 0, 0, 513, "Bat Storm 1");
  m_pTypeInfo[skillID].SetDescription("");
  m_pTypeInfo[skillID].SetVampireSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_VAMPIRE_RARE);
  m_pTypeInfo[skillID].SetMP(20);
  m_pTypeInfo[skillID].SetLearnLevel(30);
  m_pTypeInfo[skillID].SetHideInSkillTree(true);
  m_pTypeInfo[skillID].AddDelayGroup(SKILL_BAT_STORM2);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_BAT_STORM2);

  skillID = SKILL_BAT_STORM2;
  m_pTypeInfo[skillID].Set(sid++, "Bat Storm 2", 0, 0, 514, "Bat Storm 2");
  m_pTypeInfo[skillID].SetDescription("");
  m_pTypeInfo[skillID].SetVampireSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_VAMPIRE_RARE);
  m_pTypeInfo[skillID].SetMP(20);
  m_pTypeInfo[skillID].SetLearnLevel(100);
  m_pTypeInfo[skillID].SetHideInSkillTree(true);
  m_pTypeInfo[skillID].AddDelayGroup(SKILL_BAT_STORM1);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_CURSE_OF_BLOOD1);

  skillID = SKILL_CURSE_OF_BLOOD1;
  m_pTypeInfo[skillID].Set(sid++, "Curse Of Blood 1", 0, 0, 518,
                           "Curse Of Blood 1");
  m_pTypeInfo[skillID].SetDescription("");
  m_pTypeInfo[skillID].SetVampireSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_VAMPIRE_RARE);
  m_pTypeInfo[skillID].SetMP(20);
  m_pTypeInfo[skillID].SetLearnLevel(20);
  m_pTypeInfo[skillID].SetHideInSkillTree(true);
  m_pTypeInfo[skillID].AddDelayGroup(SKILL_CURSE_OF_BLOOD2);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_CURSE_OF_BLOOD2);

  skillID = SKILL_CURSE_OF_BLOOD2;
  m_pTypeInfo[skillID].Set(sid++, "Curse Of Blood 2", 0, 0, 519,
                           "Curse Of Blood 2");
  m_pTypeInfo[skillID].SetDescription("");
  m_pTypeInfo[skillID].SetVampireSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_VAMPIRE_RARE);
  m_pTypeInfo[skillID].SetMP(20);
  m_pTypeInfo[skillID].SetLearnLevel(80);
  m_pTypeInfo[skillID].SetHideInSkillTree(true);
  m_pTypeInfo[skillID].AddDelayGroup(SKILL_CURSE_OF_BLOOD1);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_BLOOD_DRAIN2);

  skillID = SKILL_BLOOD_DRAIN2;
  m_pTypeInfo[skillID].Set(sid++, "Blood Drain 2", 0, 0, 515,
                           "Blood Drain 2");
  m_pTypeInfo[skillID].SetDescription("");
  m_pTypeInfo[skillID].SetVampireSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_VAMPIRE_RARE);
  m_pTypeInfo[skillID].SetMP(0);
  m_pTypeInfo[skillID].SetLearnLevel(40);
  m_pTypeInfo[skillID].SetHideInSkillTree(true);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_BLOODY_SHOUT1);

  skillID = SKILL_BLOODY_SHOUT1;
  m_pTypeInfo[skillID].Set(sid++, "Bloody Shout 1", 0, 0, 516,
                           "Bloody Shout 1");
  m_pTypeInfo[skillID].SetDescription("");
  m_pTypeInfo[skillID].SetVampireSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_VAMPIRE_RARE);
  m_pTypeInfo[skillID].SetMP(10);
  m_pTypeInfo[skillID].SetLearnLevel(50);
  m_pTypeInfo[skillID].SetHideInSkillTree(true);
  m_pTypeInfo[skillID].AddDelayGroup(SKILL_BLOODY_SHOUT2);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_BLOODY_SHOUT2);

  skillID = SKILL_BLOODY_SHOUT2;
  m_pTypeInfo[skillID].Set(sid++, "Bloody Shout 2", 0, 0, 517,
                           "Bloody Shout 2");
  m_pTypeInfo[skillID].SetDescription("");
  m_pTypeInfo[skillID].SetVampireSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_VAMPIRE_RARE);
  m_pTypeInfo[skillID].SetMP(10);
  m_pTypeInfo[skillID].SetLearnLevel(100);
  m_pTypeInfo[skillID].SetHideInSkillTree(true);
  m_pTypeInfo[skillID].AddDelayGroup(SKILL_BLOODY_SHOUT1);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_BLOODY_WINGS);

  skillID = SKILL_BLOODY_WINGS;
  m_pTypeInfo[skillID].Set(sid++, "Bloody Wings", 0, 0, 520, "Bloody Wings");
  m_pTypeInfo[skillID].SetDescription("");
  m_pTypeInfo[skillID].SetVampireSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_VAMPIRE_RARE);
  m_pTypeInfo[skillID].SetMP(20);
  m_pTypeInfo[skillID].SetLearnLevel(80);
  m_pTypeInfo[skillID].SetHideInSkillTree(true);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_MIST_OF_SOUL1);

  // �ƿ콺����
  skillID = SKILL_MIST_OF_SOUL1;
  m_pTypeInfo[skillID].Set(sid++, "Mist Of Soul 2", 0, 0, 529,
                           "Mist Of Soul 2");
  m_pTypeInfo[skillID].SetDescription("");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_RARE);
  m_pTypeInfo[skillID].SetMP(20);
  m_pTypeInfo[skillID].SetLearnLevel(20);
  m_pTypeInfo[skillID].SetHideInSkillTree(true);
  m_pTypeInfo[skillID].AddDelayGroup(SKILL_MIST_OF_SOUL2);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_MIST_OF_SOUL2);

  skillID = SKILL_MIST_OF_SOUL2;
  m_pTypeInfo[skillID].Set(sid++, "Mist Of Soul 2", 0, 0, 530,
                           "Mist Of Soul 2");
  m_pTypeInfo[skillID].SetDescription("");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_RARE);
  m_pTypeInfo[skillID].SetMP(20);
  m_pTypeInfo[skillID].SetLearnLevel(80);
  m_pTypeInfo[skillID].SetHideInSkillTree(true);
  m_pTypeInfo[skillID].AddDelayGroup(SKILL_MIST_OF_SOUL1);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_ENERGY_BURST);

  skillID = SKILL_ENERGY_BURST;
  m_pTypeInfo[skillID].Set(sid++, "Energy Burst", 0, 0, 528, "Energy Burst");
  m_pTypeInfo[skillID].SetDescription("");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_RARE);
  m_pTypeInfo[skillID].SetPassive();
  m_pTypeInfo[skillID].SetLearnLevel(60);
  m_pTypeInfo[skillID].SetHideInSkillTree(true);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_MAGIC_AVOID1);

  skillID = SKILL_MAGIC_AVOID1;
  m_pTypeInfo[skillID].Set(sid++, "Magic Avoid 1", 0, 0, 525,
                           "Magic Avoid 1");
  m_pTypeInfo[skillID].SetDescription("");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_COMMON_RARE);
  m_pTypeInfo[skillID].SetPassive();
  m_pTypeInfo[skillID].SetLearnLevel(50);
  m_pTypeInfo[skillID].SetHideInSkillTree(true);
  m_pTypeInfo[skillID].AddDelayGroup(SKILL_MAGIC_AVOID2);
  m_pTypeInfo[skillID].AddDelayGroup(SKILL_MAGIC_AVOID3);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_MAGIC_AVOID2);

  skillID = SKILL_MAGIC_AVOID2;
  m_pTypeInfo[skillID].Set(sid++, "Magic Avoid 2", 0, 0, 531,
                           "Magic Avoid 2");
  m_pTypeInfo[skillID].SetDescription("");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_COMMON_RARE);
  m_pTypeInfo[skillID].SetPassive();
  m_pTypeInfo[skillID].SetLearnLevel(100);
  m_pTypeInfo[skillID].SetHideInSkillTree(true);
  m_pTypeInfo[skillID].AddDelayGroup(SKILL_MAGIC_AVOID1);
  m_pTypeInfo[skillID].AddDelayGroup(SKILL_MAGIC_AVOID3);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_MAGIC_AVOID3);

  skillID = SKILL_MAGIC_AVOID3;
  m_pTypeInfo[skillID].Set(sid++, "Magic Avoid 3", 0, 0, 532,
                           "Magic Avoid 3");
  m_pTypeInfo[skillID].SetDescription("");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_COMMON_RARE);
  m_pTypeInfo[skillID].SetPassive();
  m_pTypeInfo[skillID].SetLearnLevel(150);
  m_pTypeInfo[skillID].SetHideInSkillTree(true);
  m_pTypeInfo[skillID].AddDelayGroup(SKILL_MAGIC_AVOID2);
  m_pTypeInfo[skillID].AddDelayGroup(SKILL_MAGIC_AVOID3);
#if __CONTENTS(__SECOND_RARE_SKILL)
  m_pTypeInfo[skillID].AddNextSkill(SKILL_HEAVENLY_SPARK);
#else
  m_pTypeInfo[skillID].AddNextSkill(SKILL_HALO);
#endif //__SECOND_RARE_SKILL
#if __CONTENTS(__SECOND_RARE_SKILL)
  skillID = SKILL_HEAVENLY_SPARK;
  m_pTypeInfo[skillID].Set(sid++, "Heavenly Spark", 0, 0, 539, "Heavenly Spark");
  m_pTypeInfo[skillID].SetDescription("");
  m_pTypeInfo[skillID].SetSlayerSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_SLAYER_RARE);
  m_pTypeInfo[skillID].SetMP(10);
  m_pTypeInfo[skillID].SetLearnLevel(161);
  m_pTypeInfo[skillID].SetHideInSkillTree(true);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_BLADE_STORM);

  skillID = SKILL_BLADE_STORM;
  m_pTypeInfo[skillID].Set(sid++, "Blade Storm", 0, 0, 540, "Blade Storm");
  m_pTypeInfo[skillID].SetDescription("");
  m_pTypeInfo[skillID].SetSlayerSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_SLAYER_RARE);
  m_pTypeInfo[skillID].SetMP(8);
  m_pTypeInfo[skillID].SetLearnLevel(161);
  m_pTypeInfo[skillID].SetHideInSkillTree(true);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_XRL);

  skillID = SKILL_XRL;
  m_pTypeInfo[skillID].Set(sid++, "eXplosion Roket Launcher", 0, 0, 541, "XRL");
  m_pTypeInfo[skillID].SetDescription("");
  m_pTypeInfo[skillID].SetSlayerSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_SLAYER_RARE);
  m_pTypeInfo[skillID].SetMP(10);
  m_pTypeInfo[skillID].SetLearnLevel(161);
  m_pTypeInfo[skillID].SetHideInSkillTree(true);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_GENESIS);

  skillID = SKILL_GENESIS;
  m_pTypeInfo[skillID].Set(sid++, "Genesis", 0, 0, 542, "Genesis");
  m_pTypeInfo[skillID].SetDescription("");
  m_pTypeInfo[skillID].SetSlayerSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_SLAYER_RARE);
  m_pTypeInfo[skillID].SetMP(30);
  m_pTypeInfo[skillID].SetLearnLevel(161);
  m_pTypeInfo[skillID].SetHideInSkillTree(true);
  m_pTypeInfo[skillID].AddDelayGroup(SKILL_BOMBING_STAR);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_THORNS);

  skillID = SKILL_THORNS;
  m_pTypeInfo[skillID].Set(sid++, "Thorns", 0, 0, 543, "Thorns");
  m_pTypeInfo[skillID].SetDescription("");
  m_pTypeInfo[skillID].SetSlayerSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_SLAYER_RARE);
  m_pTypeInfo[skillID].SetMP(60);
  m_pTypeInfo[skillID].SetLearnLevel(161);
  m_pTypeInfo[skillID].SetHideInSkillTree(true);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_LETHAL_CLAW);

  skillID = SKILL_LETHAL_CLAW;
  m_pTypeInfo[skillID].Set(sid++, "Lethal Claw", 0, 0, 544, "Lethal Claw");
  m_pTypeInfo[skillID].SetDescription("");
  m_pTypeInfo[skillID].SetVampireSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_VAMPIRE_RARE);
  m_pTypeInfo[skillID].SetMP(15);
  m_pTypeInfo[skillID].SetLearnLevel(161);
  m_pTypeInfo[skillID].SetHideInSkillTree(true);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_VENOM_CIRCLE);

  skillID = SKILL_VENOM_CIRCLE;
  m_pTypeInfo[skillID].Set(sid++, "Venom Circle", 0, 0, 545, "Venom Circle");
  m_pTypeInfo[skillID].SetDescription("");
  m_pTypeInfo[skillID].SetVampireSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_VAMPIRE_RARE);
  m_pTypeInfo[skillID].SetMP(30);
  m_pTypeInfo[skillID].SetLearnLevel(161);
  m_pTypeInfo[skillID].SetHideInSkillTree(true);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_BRIONAC);

  skillID = SKILL_BRIONAC;
  m_pTypeInfo[skillID].Set(sid++, "Brionac", 0, 0, 546, "Brionac");
  m_pTypeInfo[skillID].SetDescription("");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_RARE);
  m_pTypeInfo[skillID].SetMP(13);
  m_pTypeInfo[skillID].SetLearnLevel(161);
  m_pTypeInfo[skillID].SetHideInSkillTree(true);
  m_pTypeInfo[skillID].AddDelayGroup(SKILL_DESTRUCTION_SPEAR);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_ASTEROID_BOMBER);

  skillID = SKILL_ASTEROID_BOMBER;
  m_pTypeInfo[skillID].Set(sid++, "Asteroid Bomber", 0, 0, 547,
                           "Asteroid Bomber");
  m_pTypeInfo[skillID].SetDescription("");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_RARE);
  m_pTypeInfo[skillID].SetMP(40);
  m_pTypeInfo[skillID].SetLearnLevel(161);
  m_pTypeInfo[skillID].SetHideInSkillTree(true);
  m_pTypeInfo[skillID].AddDelayGroup(SKILL_METEOR_STORM);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_FROZEN_GLACIER);

  skillID = SKILL_FROZEN_GLACIER;
  m_pTypeInfo[skillID].Set(sid++, "Frozen Glacier", 0, 0, 548, "Frozen Glacier");
  m_pTypeInfo[skillID].SetDescription("");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_RARE);
  m_pTypeInfo[skillID].SetMP(30);
  m_pTypeInfo[skillID].SetLearnLevel(161);
  m_pTypeInfo[skillID].SetHideInSkillTree(true);
  m_pTypeInfo[skillID].AddDelayGroup(SKILL_ICE_LANCE);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_INCINERATE);

  skillID = SKILL_INCINERATE;
  m_pTypeInfo[skillID].Set(sid++, "Incinerate", 0, 0, 549, "Incinerate");
  m_pTypeInfo[skillID].SetDescription("");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_RARE);
  m_pTypeInfo[skillID].SetMP(30);
  m_pTypeInfo[skillID].SetLearnLevel(161);
  m_pTypeInfo[skillID].SetHideInSkillTree(true);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_INCREASE_OF_HEALTH1);

  skillID = SKILL_INCREASE_OF_HEALTH1;
  m_pTypeInfo[skillID].Set(sid++, "Increase of health 1", 0, 0, 550,
                           "Increase of health 1");
  m_pTypeInfo[skillID].SetDescription("");
  m_pTypeInfo[skillID].SetPassive();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_COMMON_RARE);
  m_pTypeInfo[skillID].SetLearnLevel(151);
  m_pTypeInfo[skillID].SetHideInSkillTree(true);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_INCREASE_OF_HEALTH2);

  skillID = SKILL_INCREASE_OF_HEALTH2;
  m_pTypeInfo[skillID].Set(sid++, "Increase of health2", 0, 0, 551,
                           "Increase of health 2");
  m_pTypeInfo[skillID].SetDescription("");
  m_pTypeInfo[skillID].SetPassive();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_COMMON_RARE);
  m_pTypeInfo[skillID].SetLearnLevel(161);
  m_pTypeInfo[skillID].SetHideInSkillTree(true);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_INCREASE_OF_HEALTH3);

  skillID = SKILL_INCREASE_OF_HEALTH3;
  m_pTypeInfo[skillID].Set(sid++, "Increase of health3", 0, 0, 552,
                           "Increase of health 3");
  m_pTypeInfo[skillID].SetDescription("");
  m_pTypeInfo[skillID].SetPassive();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_COMMON_RARE);
  m_pTypeInfo[skillID].SetLearnLevel(171);
  m_pTypeInfo[skillID].SetHideInSkillTree(true);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_INCREASE_OF_HEALTH4);

  skillID = SKILL_INCREASE_OF_HEALTH4;
  m_pTypeInfo[skillID].Set(sid++, "Increase of health4", 0, 0, 553,
                           "Increase of health 4");
  m_pTypeInfo[skillID].SetDescription("");
  m_pTypeInfo[skillID].SetPassive();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_COMMON_RARE);
  m_pTypeInfo[skillID].SetLearnLevel(181);
  m_pTypeInfo[skillID].SetHideInSkillTree(true);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_INCREASE_OF_HEALTH5);

  skillID = SKILL_INCREASE_OF_HEALTH5;
  m_pTypeInfo[skillID].Set(sid++, "Increase of health5", 0, 0, 554,
                           "Increase of health 5");
  m_pTypeInfo[skillID].SetDescription("");
  m_pTypeInfo[skillID].SetPassive();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_COMMON_RARE);
  m_pTypeInfo[skillID].SetLearnLevel(191);
  m_pTypeInfo[skillID].SetHideInSkillTree(true);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_INCREASE_OF_HEALTH6);

  skillID = SKILL_INCREASE_OF_HEALTH6;
  m_pTypeInfo[skillID].Set(sid++, "Increase of health6", 0, 0, 555,
                           "Increase of health 6");
  m_pTypeInfo[skillID].SetDescription("");
  m_pTypeInfo[skillID].SetPassive();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_COMMON_RARE);
  m_pTypeInfo[skillID].SetLearnLevel(201);
  m_pTypeInfo[skillID].SetHideInSkillTree(true);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_BLADE_MAGMA_DETONATION_1);
#endif //__SECOND_RARE_SKILL

#if __CONTENTS(__20091116_ADD_RARE_SKILL)
  skillID = SKILL_BLADE_MAGMA_DETONATION_1;
  m_pTypeInfo[skillID].Set(sid++, "Magma detonation 1", 0, 0, 560,
                           "Magma detonation 1");
  m_pTypeInfo[skillID].SetDescription("");
  m_pTypeInfo[skillID].SetSlayerSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_SLAYER_RARE);
  m_pTypeInfo[skillID].SetMP(20);
  m_pTypeInfo[skillID].SetLearnLevel(50);
  m_pTypeInfo[skillID].SetHideInSkillTree(true);
  m_pTypeInfo[skillID].AddDelayGroup(SKILL_BLADE_MAGMA_DETONATION_2);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_BLADE_MAGMA_DETONATION_2);

  skillID = SKILL_BLADE_MAGMA_DETONATION_2;
  m_pTypeInfo[skillID].Set(sid++, "Magma detonation 2", 0, 0, 561,
                           "Magma detonation 2");
  m_pTypeInfo[skillID].SetDescription("");
  m_pTypeInfo[skillID].SetSlayerSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_SLAYER_RARE);
  m_pTypeInfo[skillID].SetMP(25);
  m_pTypeInfo[skillID].SetLearnLevel(100);
  m_pTypeInfo[skillID].SetHideInSkillTree(true);
  m_pTypeInfo[skillID].AddDelayGroup(SKILL_BLADE_MAGMA_DETONATION_1);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_OUSTERS_SQUALLY_BARRIER_1);

  skillID = SKILL_OUSTERS_SQUALLY_BARRIER_1;
  m_pTypeInfo[skillID].Set(sid++, "Squally barrier 1", 0, 0, 562,
                           "Squally barrier 1");
  m_pTypeInfo[skillID].SetDescription("");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_RARE);
  m_pTypeInfo[skillID].SetMP(25);
  m_pTypeInfo[skillID].SetLearnLevel(50);
  m_pTypeInfo[skillID].SetHideInSkillTree(true);
  m_pTypeInfo[skillID].AddDelayGroup(SKILL_OUSTERS_SQUALLY_BARRIER_2);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_OUSTERS_SQUALLY_BARRIER_2);

  skillID = SKILL_OUSTERS_SQUALLY_BARRIER_2;
  m_pTypeInfo[skillID].Set(sid++, "Squally barrier 2", 0, 0, 563,
                           "Squally barrier 2");
  m_pTypeInfo[skillID].SetDescription("");
  m_pTypeInfo[skillID].SetOustersSkill();
  m_pTypeInfo[skillID].SetSkillStep(SKILL_STEP_OUSTERS_RARE);
  m_pTypeInfo[skillID].SetMP(30);
  m_pTypeInfo[skillID].SetLearnLevel(100);
  m_pTypeInfo[skillID].SetHideInSkillTree(true);
  m_pTypeInfo[skillID].AddDelayGroup(SKILL_OUSTERS_SQUALLY_BARRIER_1);
  m_pTypeInfo[skillID].AddNextSkill(SKILL_HALO);
#endif //__20091116_ADD_RARE_SKILL

  // �ƿ콺Blood Bible ��ų
  // By csm �ƿ콺Blood Bible ���� ��ų

  // sjheon	2005.07.29 End

  m_pTypeInfo[SKILL_HALO].Set(sid++, "Halo", 0, 0, 476, "Halo");
  m_pTypeInfo[SKILL_HALO].SetOustersSkill();
  m_pTypeInfo[SKILL_HALO].SetSkillStep(SKILL_STEP_OUSTERS_COMBAT_ADVANCEMENT);
  m_pTypeInfo[SKILL_HALO].SetMP(50);
  m_pTypeInfo[SKILL_HALO].SetLearnLevel(150);
  m_pTypeInfo[SKILL_HALO].AddNextSkill(
      SKILL_HETER_CHAKRAM); // SKILL_DESTINIES );

  m_pTypeInfo[SKILL_DESTINIES].Set(sid++, "Destinies", 0, 0, 477, "Destinies");
  m_pTypeInfo[SKILL_DESTINIES].SetOustersSkill();
  m_pTypeInfo[SKILL_DESTINIES].SetSkillStep(
      SKILL_STEP_OUSTERS_EARTH_ADVANCEMENT);
  m_pTypeInfo[SKILL_DESTINIES].SetMP(50);
  m_pTypeInfo[SKILL_DESTINIES].SetLearnLevel(150);
  m_pTypeInfo[SKILL_DESTINIES].AddNextSkill(
      SKILL_SUMMON_CLAY); // SKILL_FIERCE_FLAME );

  m_pTypeInfo[SKILL_FIERCE_FLAME].Set(sid++, "Fierce Flame", 0, 0, 478,
                                      "Fierce Flame");
  m_pTypeInfo[SKILL_FIERCE_FLAME].SetOustersSkill();
  m_pTypeInfo[SKILL_FIERCE_FLAME].SetSkillStep(
      SKILL_STEP_OUSTERS_FIRE_ADVANCEMENT);
  m_pTypeInfo[SKILL_FIERCE_FLAME].SetMP(50);
  m_pTypeInfo[SKILL_FIERCE_FLAME].SetLearnLevel(150);
  m_pTypeInfo[SKILL_FIERCE_FLAME].AddDelayGroup(SKILL_HELLFIRE);
  m_pTypeInfo[SKILL_FIERCE_FLAME].AddNextSkill(
      SKILL_DUMMY_DRAKE); // SKILL_SHADOW_OF_STORM );

  m_pTypeInfo[SKILL_SHADOW_OF_STORM].Set(sid++, "Shadow Of Storm", 0, 0, 479,
                                         "Shadow Of Storm");
  m_pTypeInfo[SKILL_SHADOW_OF_STORM].SetOustersSkill();
  m_pTypeInfo[SKILL_SHADOW_OF_STORM].SetSkillStep(
      SKILL_STEP_OUSTERS_WATER_ADVANCEMENT);
  m_pTypeInfo[SKILL_SHADOW_OF_STORM].SetMP(50);
  m_pTypeInfo[SKILL_SHADOW_OF_STORM].SetLearnLevel(150);
  m_pTypeInfo[SKILL_SHADOW_OF_STORM].AddNextSkill(
      SKILL_HYDRO_CONVERGENCE); // SKILL_DRAGON_TORNADO) ;
                                // //SKILL_OUSTERS_COMBO) ; //
                                // SKILL_DRAGON_TORNADO) ;
                                // //SKILL_OUSTERS_COMBO) ;

  // 160 ��ų
  m_pTypeInfo[SKILL_HETER_CHAKRAM].Set(sid++, "Heter Chakram", 0, 0, 507,
                                       "Heter Chakram");
  m_pTypeInfo[SKILL_HETER_CHAKRAM].SetOustersSkill();
  m_pTypeInfo[SKILL_HETER_CHAKRAM].SetSkillStep(
      SKILL_STEP_OUSTERS_COMBAT_ADVANCEMENT);
  m_pTypeInfo[SKILL_HETER_CHAKRAM].SetMP(50);
  m_pTypeInfo[SKILL_HETER_CHAKRAM].SetLearnLevel(160);
  m_pTypeInfo[SKILL_HETER_CHAKRAM].AddNextSkill(
      SKILL_DESTINIES); // SKILL_DESTINIES );

  m_pTypeInfo[SKILL_DUMMY_DRAKE].Set(sid++, "Dummy Drake", 0, 0, 508,
                                     "Dummy Drake");
  m_pTypeInfo[SKILL_DUMMY_DRAKE].SetOustersSkill();
  m_pTypeInfo[SKILL_DUMMY_DRAKE].SetSkillStep(
      SKILL_STEP_OUSTERS_FIRE_ADVANCEMENT);
  m_pTypeInfo[SKILL_DUMMY_DRAKE].SetMP(50);
  m_pTypeInfo[SKILL_DUMMY_DRAKE].SetLearnLevel(160);
  m_pTypeInfo[SKILL_DUMMY_DRAKE].AddNextSkill(SKILL_SHADOW_OF_STORM);

  m_pTypeInfo[SKILL_HYDRO_CONVERGENCE].Set(sid++, "Hydro Convergence", 0, 0,
                                           509, "Hydro Convergence");
  m_pTypeInfo[SKILL_HYDRO_CONVERGENCE].SetOustersSkill();
  m_pTypeInfo[SKILL_HYDRO_CONVERGENCE].SetSkillStep(
      SKILL_STEP_OUSTERS_WATER_ADVANCEMENT);
  m_pTypeInfo[SKILL_HYDRO_CONVERGENCE].SetMP(50);
  m_pTypeInfo[SKILL_HYDRO_CONVERGENCE].SetLearnLevel(160);
  m_pTypeInfo[SKILL_HYDRO_CONVERGENCE].AddNextSkill(SKILL_DRAGON_TORNADO);

  m_pTypeInfo[SKILL_SUMMON_CLAY].Set(sid++, "Summon Clay", 0, 0, 510,
                                     "Summon Clay");
  m_pTypeInfo[SKILL_SUMMON_CLAY].SetOustersSkill();
  m_pTypeInfo[SKILL_SUMMON_CLAY].SetSkillStep(
      SKILL_STEP_OUSTERS_EARTH_ADVANCEMENT);
  m_pTypeInfo[SKILL_SUMMON_CLAY].SetMP(50);
  m_pTypeInfo[SKILL_SUMMON_CLAY].SetLearnLevel(160);
  m_pTypeInfo[SKILL_SUMMON_CLAY].AddNextSkill(SKILL_FIERCE_FLAME);

  // sjheon	2005.07.29 Add
  //	m_pTypeInfo[SKILL_OUSTERS_COMBO].Set(sid++, "Ousters Combo", 0, 0,
  //479,"�ƿ콺Ʈ �޺�"); 	m_pTypeInfo[SKILL_OUSTERS_COMBO].SetOustersSkill();
  //	m_pTypeInfo[SKILL_OUSTERS_COMBO].SetSkillStep(
  //SKILL_STEP_OUSTERS_COMBAT_ADVANCEMENT);
  //	m_pTypeInfo[SKILL_OUSTERS_COMBO].SetMP( 50 );
  //	m_pTypeInfo[SKILL_OUSTERS_COMBO].SetLearnLevel( 150 );
  //	m_pTypeInfo[SKILL_OUSTERS_COMBO].AddNextSkill(SKILL_DRAGON_TORNADO) ;
  ////SKILL_HALO) ; //SKILL_DESTINIES) ; // SKILL_HALO) ;
  ////SKILL_DRAGON_TORNADO) ; //SKILL_DESTINIES);
  // sjheon	2005.07.29 End

  //----------------------------------------------------------------------
  //
  //							��Ÿ �����
  //
  //----------------------------------------------------------------------
  // SkillTree���� �� ������, Icon�� �����ؼ� ����� �� �ִ� �����
  //----------------------------------------------------------------------

  //----------------------------------------------------------------------
  // Blood Bible��
  //----------------------------------------------------------------------
  m_pTypeInfo[MAGIC_THROW_HOLY_WATER].Set(0, "Throw Holy Water", 0, 0, 101,
                                          "Throw Holy Water");
  m_pTypeInfo[MAGIC_THROW_HOLY_WATER].SetMP(0);

  // �ͷ� ����
  m_pTypeInfo[SKILL_TURRET_FIRE].Set(0, "Turret Fire", 0, 0, 389,
                                     "Turret Fire");
  m_pTypeInfo[SKILL_TURRET_FIRE].SetMP(0);
  //----------------------------------------------------------------------
  // ��Ż
  //----------------------------------------------------------------------
  m_pTypeInfo[SUMMON_HELICOPTER].Set(0, "Call Helicopter", 0, 0, 115,
                                     "Call Helicopter");
  m_pTypeInfo[SUMMON_HELICOPTER].SetMP(0);

  //----------------------------------------------------------------------
  // ����
  //----------------------------------------------------------------------
  m_pTypeInfo[SKILL_BLOOD_DRAIN].Set(0, "Blood Drain", 0, 0, 102, "Blood Drain");
  m_pTypeInfo[SKILL_BLOOD_DRAIN].SetVampireSkill();
  m_pTypeInfo[SKILL_BLOOD_DRAIN].SetMP(0);

  //----------------------------------------------------------------------
  // ��ź
  //----------------------------------------------------------------------
  m_pTypeInfo[BOMB_SPLINTER].Set(0, "Bomb-Splinter", 0, 0, 137,
                                 "Bomb-Splinter");
  m_pTypeInfo[BOMB_SPLINTER].SetMP(0);

  m_pTypeInfo[BOMB_ACER].Set(0, "Bomb-Acer", 0, 0, 138, "Bomb-Acer");
  m_pTypeInfo[BOMB_ACER].SetMP(0);

  m_pTypeInfo[BOMB_BULLS].Set(0, "Bomb-Bulls", 0, 0, 139, "Bomb-Bulls");
  m_pTypeInfo[BOMB_BULLS].SetMP(0);

  m_pTypeInfo[BOMB_STUN].Set(0, "Bomb-Stun", 0, 0, 140, "Bomb-Stun");
  m_pTypeInfo[BOMB_STUN].SetMP(0);

  m_pTypeInfo[BOMB_CROSSBOW].Set(0, "Bomb-Crossbow", 0, 0, 141,
                                 "Bomb-Crossbow");
  m_pTypeInfo[BOMB_CROSSBOW].SetMP(0);

  m_pTypeInfo[BOMB_TWISTER].Set(0, "Bomb-Twister", 0, 0, 142, "Bomb-Twister");
  m_pTypeInfo[BOMB_TWISTER].SetMP(0);

  //----------------------------------------------------------------------
  // ����
  //----------------------------------------------------------------------
  m_pTypeInfo[MINE_ANKLE_KILLER].Set(0, "Mine-AnkleKiller", 0, 0, 143,
                                     "Mine-AnkleKiller");
  m_pTypeInfo[MINE_ANKLE_KILLER].SetMP(0);

  m_pTypeInfo[MINE_POMZ].Set(0, "Mine-Pomz", 0, 0, 144, "Mine-Pomz");
  m_pTypeInfo[MINE_POMZ].SetMP(0);

  m_pTypeInfo[MINE_AP_C1].Set(0, "Mine-AP-C1", 0, 0, 145, "Mine-AP-C1");
  m_pTypeInfo[MINE_AP_C1].SetMP(0);

  m_pTypeInfo[MINE_DIAMONDBACK].Set(0, "Mine-DiamondBack", 0, 0, 146,
                                    "Mine-DiamondBack");
  m_pTypeInfo[MINE_DIAMONDBACK].SetMP(0);

  m_pTypeInfo[MINE_SWIFT_EX].Set(0, "Mine-Swift-EX", 0, 0, 147,
                                 "Mine-Swift-EX");
  m_pTypeInfo[MINE_SWIFT_EX].SetMP(0);

  m_pTypeInfo[MINE_SIDEWINDER].Set(0, "Mine-SideWinder", 0, 0, 148,
                                   "Mine-SideWinder");
  m_pTypeInfo[MINE_SIDEWINDER].SetMP(0);

  m_pTypeInfo[MINE_COBRA].Set(0, "Mine-Cobra", 0, 0, 149, "Mine-Cobra");
  m_pTypeInfo[MINE_COBRA].SetMP(0);

  //----------------------------------------------------------------------
  // ���� ���
  //----------------------------------------------------------------------
  m_pTypeInfo[MAGIC_EAT_CORPSE].Set(0, "Eat Corpse", 0, 0, 123, "Eat Corpse");
  m_pTypeInfo[MAGIC_EAT_CORPSE].SetVampireSkill();
  m_pTypeInfo[MAGIC_EAT_CORPSE].SetMP(0);

  m_pTypeInfo[SKILL_BITE_OF_DEATH].Set(0, "Bite Of Death", 0, 0, 341,
                                       "Bite Of Death");
  m_pTypeInfo[SKILL_BITE_OF_DEATH].SetVampireSkill();
  m_pTypeInfo[SKILL_BITE_OF_DEATH].SetMP(0);

  //----------------------------------------------------------------------
  // Blood Bible
  //----------------------------------------------------------------------
  // Blood Bible ����
  // Blood Bible ����
  m_pTypeInfo[MAGIC_UN_TRANSFORM].Set(0, "Untransform", 0, 0, 103,
                                      "Untransform");
  m_pTypeInfo[MAGIC_UN_TRANSFORM].SetVampireSkill();
  m_pTypeInfo[MAGIC_UN_TRANSFORM].SetMP(0);

  // Invisible ����
  m_pTypeInfo[MAGIC_UN_INVISIBILITY].Set(0, "Uninvisiblity", 0, 0, 104,
                                         "Uninvisiblity");
  m_pTypeInfo[MAGIC_UN_INVISIBILITY].SetVampireSkill();
  m_pTypeInfo[MAGIC_UN_INVISIBILITY].SetMP(0);

  //----------------------------------------------------------------------
  // ��Blood Bible�� [�����3]
  //----------------------------------------------------------------------
  m_pTypeInfo[MAGIC_OPEN_CASKET].Set(0, "Open Casket", 0, 0, 103,
                                     "Open Casket");
  m_pTypeInfo[MAGIC_OPEN_CASKET].SetVampireSkill();
  m_pTypeInfo[MAGIC_OPEN_CASKET].SetMP(0);

  //----------------------------------------------------------------------
  // HolyLandBonus Skills
  //----------------------------------------------------------------------
  m_pTypeInfo[SKILL_HOLYLAND_BLOOD_BIBLE_ARMEGA].Set(
      0, "Blood Bible Armega", 0, 0, 230, "Blood Bible Armega");
  m_pTypeInfo[SKILL_HOLYLAND_BLOOD_BIBLE_ARMEGA].SetVampireSkill();
  m_pTypeInfo[SKILL_HOLYLAND_BLOOD_BIBLE_ARMEGA].SetPassive();
  m_pTypeInfo[SKILL_HOLYLAND_BLOOD_BIBLE_ARMEGA].SetMP(0);

  m_pTypeInfo[SKILL_HOLYLAND_BLOOD_BIBLE_MIHOLE].Set(
      0, "Blood Bible MIhole", 0, 0, 231, "Blood Bible MIhole");
  m_pTypeInfo[SKILL_HOLYLAND_BLOOD_BIBLE_MIHOLE].SetVampireSkill();
  m_pTypeInfo[SKILL_HOLYLAND_BLOOD_BIBLE_MIHOLE].SetPassive();
  m_pTypeInfo[SKILL_HOLYLAND_BLOOD_BIBLE_MIHOLE].SetMP(0);

  m_pTypeInfo[SKILL_HOLYLAND_BLOOD_BIBLE_KIRO].Set(0, "Blood Bible Kiro", 0, 0,
                                                   232, "Blood Bible Kiro");
  m_pTypeInfo[SKILL_HOLYLAND_BLOOD_BIBLE_KIRO].SetVampireSkill();
  m_pTypeInfo[SKILL_HOLYLAND_BLOOD_BIBLE_KIRO].SetPassive();
  m_pTypeInfo[SKILL_HOLYLAND_BLOOD_BIBLE_KIRO].SetMP(0);

  m_pTypeInfo[SKILL_HOLYLAND_BLOOD_BIBLE_INI].Set(0, "Blood Bible Ini", 0, 0,
                                                  233, "Blood Bible Ini");
  m_pTypeInfo[SKILL_HOLYLAND_BLOOD_BIBLE_INI].SetVampireSkill();
  m_pTypeInfo[SKILL_HOLYLAND_BLOOD_BIBLE_INI].SetPassive();
  m_pTypeInfo[SKILL_HOLYLAND_BLOOD_BIBLE_INI].SetMP(0);

  m_pTypeInfo[SKILL_HOLYLAND_BLOOD_BIBLE_GREGORI].Set(
      0, "Blood Bible Gregori", 0, 0, 234, "Blood Bible Gregori");
  m_pTypeInfo[SKILL_HOLYLAND_BLOOD_BIBLE_GREGORI].SetVampireSkill();
  m_pTypeInfo[SKILL_HOLYLAND_BLOOD_BIBLE_GREGORI].SetPassive();
  m_pTypeInfo[SKILL_HOLYLAND_BLOOD_BIBLE_GREGORI].SetMP(0);

  m_pTypeInfo[SKILL_HOLYLAND_BLOOD_BIBLE_CONCILIA].Set(
      0, "Blood Bible Concilia", 0, 0, 235, "Blood Bible Concilia");
  m_pTypeInfo[SKILL_HOLYLAND_BLOOD_BIBLE_CONCILIA].SetVampireSkill();
  m_pTypeInfo[SKILL_HOLYLAND_BLOOD_BIBLE_CONCILIA].SetPassive();
  m_pTypeInfo[SKILL_HOLYLAND_BLOOD_BIBLE_CONCILIA].SetMP(0);

  m_pTypeInfo[SKILL_HOLYLAND_BLOOD_BIBLE_LEGIOS].Set(
      0, "Blood Bible Legios", 0, 0, 236, "Blood Bible Legios");
  m_pTypeInfo[SKILL_HOLYLAND_BLOOD_BIBLE_LEGIOS].SetVampireSkill();
  m_pTypeInfo[SKILL_HOLYLAND_BLOOD_BIBLE_LEGIOS].SetPassive();
  m_pTypeInfo[SKILL_HOLYLAND_BLOOD_BIBLE_LEGIOS].SetMP(0);

  m_pTypeInfo[SKILL_HOLYLAND_BLOOD_BIBLE_HILLEL].Set(0, "Blood Bible Hillel", 0,
                                                     0, 237, "Blood Bible Hillel");
  m_pTypeInfo[SKILL_HOLYLAND_BLOOD_BIBLE_HILLEL].SetVampireSkill();
  m_pTypeInfo[SKILL_HOLYLAND_BLOOD_BIBLE_HILLEL].SetPassive();
  m_pTypeInfo[SKILL_HOLYLAND_BLOOD_BIBLE_HILLEL].SetMP(0);

  m_pTypeInfo[SKILL_HOLYLAND_BLOOD_BIBLE_JAVE].Set(0, "Blood Bible Jave", 0, 0,
                                                   238, "Blood Bible Jave");
  m_pTypeInfo[SKILL_HOLYLAND_BLOOD_BIBLE_JAVE].SetVampireSkill();
  m_pTypeInfo[SKILL_HOLYLAND_BLOOD_BIBLE_JAVE].SetPassive();
  m_pTypeInfo[SKILL_HOLYLAND_BLOOD_BIBLE_JAVE].SetMP(0);

  m_pTypeInfo[SKILL_HOLYLAND_BLOOD_BIBLE_NEMA].Set(0, "Blood Bible Nema", 0, 0,
                                                   239, "Blood Bible Nema");
  m_pTypeInfo[SKILL_HOLYLAND_BLOOD_BIBLE_NEMA].SetVampireSkill();
  m_pTypeInfo[SKILL_HOLYLAND_BLOOD_BIBLE_NEMA].SetPassive();
  m_pTypeInfo[SKILL_HOLYLAND_BLOOD_BIBLE_NEMA].SetMP(0);

  m_pTypeInfo[SKILL_HOLYLAND_BLOOD_BIBLE_AROSA].Set(0, "Blood Bible Arosa", 0,
                                                    0, 240, "Blood Bible Arosa");
  m_pTypeInfo[SKILL_HOLYLAND_BLOOD_BIBLE_AROSA].SetVampireSkill();
  m_pTypeInfo[SKILL_HOLYLAND_BLOOD_BIBLE_AROSA].SetPassive();
  m_pTypeInfo[SKILL_HOLYLAND_BLOOD_BIBLE_AROSA].SetMP(0);

  m_pTypeInfo[SKILL_HOLYLAND_BLOOD_BIBLE_CHASPA].Set(
      0, "Blood Bible Chaspa", 0, 0, 241, "Blood Bible Chaspa");
  m_pTypeInfo[SKILL_HOLYLAND_BLOOD_BIBLE_CHASPA].SetVampireSkill();
  m_pTypeInfo[SKILL_HOLYLAND_BLOOD_BIBLE_CHASPA].SetPassive();
  m_pTypeInfo[SKILL_HOLYLAND_BLOOD_BIBLE_CHASPA].SetMP(0);

  m_pTypeInfo[SKILL_SWEEPER_BONUS_1].Set(0, "M-03-01", 0, 0, 342,
                                         "M-03-01");
  m_pTypeInfo[SKILL_SWEEPER_BONUS_1].SetVampireSkill();
  m_pTypeInfo[SKILL_SWEEPER_BONUS_1].SetPassive();
  m_pTypeInfo[SKILL_SWEEPER_BONUS_1].SetMP(0);

  m_pTypeInfo[SKILL_SWEEPER_BONUS_2].Set(0, "M-03-02", 0, 0, 343,
                                         "M-03-02");
  m_pTypeInfo[SKILL_SWEEPER_BONUS_2].SetVampireSkill();
  m_pTypeInfo[SKILL_SWEEPER_BONUS_2].SetPassive();
  m_pTypeInfo[SKILL_SWEEPER_BONUS_2].SetMP(0);

  m_pTypeInfo[SKILL_SWEEPER_BONUS_3].Set(0, "M-03-03", 0, 0, 344,
                                         "M-03-03");
  m_pTypeInfo[SKILL_SWEEPER_BONUS_3].SetVampireSkill();
  m_pTypeInfo[SKILL_SWEEPER_BONUS_3].SetPassive();
  m_pTypeInfo[SKILL_SWEEPER_BONUS_3].SetMP(0);

  m_pTypeInfo[SKILL_SWEEPER_BONUS_4].Set(0, "M-03-04", 0, 0, 345,
                                         "M-03-04");
  m_pTypeInfo[SKILL_SWEEPER_BONUS_4].SetVampireSkill();
  m_pTypeInfo[SKILL_SWEEPER_BONUS_4].SetPassive();
  m_pTypeInfo[SKILL_SWEEPER_BONUS_4].SetMP(0);

  m_pTypeInfo[SKILL_SWEEPER_BONUS_5].Set(0, "M-03-05", 0, 0, 346,
                                         "M-03-05");
  m_pTypeInfo[SKILL_SWEEPER_BONUS_5].SetVampireSkill();
  m_pTypeInfo[SKILL_SWEEPER_BONUS_5].SetPassive();
  m_pTypeInfo[SKILL_SWEEPER_BONUS_5].SetMP(0);

  m_pTypeInfo[SKILL_SWEEPER_BONUS_6].Set(0, "M-03-06", 0, 0, 347,
                                         "M-03-06");
  m_pTypeInfo[SKILL_SWEEPER_BONUS_6].SetVampireSkill();
  m_pTypeInfo[SKILL_SWEEPER_BONUS_6].SetPassive();
  m_pTypeInfo[SKILL_SWEEPER_BONUS_6].SetMP(0);

  m_pTypeInfo[SKILL_SWEEPER_BONUS_7].Set(0, "M-03-07", 0, 0, 348,
                                         "M-03-07");
  m_pTypeInfo[SKILL_SWEEPER_BONUS_7].SetVampireSkill();
  m_pTypeInfo[SKILL_SWEEPER_BONUS_7].SetPassive();
  m_pTypeInfo[SKILL_SWEEPER_BONUS_7].SetMP(0);

  m_pTypeInfo[SKILL_SWEEPER_BONUS_8].Set(0, "M-03-08", 0, 0, 349,
                                         "M-03-08");
  m_pTypeInfo[SKILL_SWEEPER_BONUS_8].SetVampireSkill();
  m_pTypeInfo[SKILL_SWEEPER_BONUS_8].SetPassive();
  m_pTypeInfo[SKILL_SWEEPER_BONUS_8].SetMP(0);

  m_pTypeInfo[SKILL_SWEEPER_BONUS_9].Set(0, "M-03-09", 0, 0, 350,
                                         "M-03-09");
  m_pTypeInfo[SKILL_SWEEPER_BONUS_9].SetVampireSkill();
  m_pTypeInfo[SKILL_SWEEPER_BONUS_9].SetPassive();
  m_pTypeInfo[SKILL_SWEEPER_BONUS_9].SetMP(0);

  m_pTypeInfo[SKILL_SWEEPER_BONUS_10].Set(0, "M-03-10", 0, 0, 351,
                                          "M-03-10");
  m_pTypeInfo[SKILL_SWEEPER_BONUS_10].SetVampireSkill();
  m_pTypeInfo[SKILL_SWEEPER_BONUS_10].SetPassive();
  m_pTypeInfo[SKILL_SWEEPER_BONUS_10].SetMP(0);

  m_pTypeInfo[SKILL_SWEEPER_BONUS_11].Set(0, "M-03-11", 0, 0, 352,
                                          "M-03-11");
  m_pTypeInfo[SKILL_SWEEPER_BONUS_11].SetVampireSkill();
  m_pTypeInfo[SKILL_SWEEPER_BONUS_11].SetPassive();
  m_pTypeInfo[SKILL_SWEEPER_BONUS_11].SetMP(0);

  m_pTypeInfo[SKILL_SWEEPER_BONUS_12].Set(0, "M-03-12", 0, 0, 353,
                                          "M-03-12");
  m_pTypeInfo[SKILL_SWEEPER_BONUS_12].SetVampireSkill();
  m_pTypeInfo[SKILL_SWEEPER_BONUS_12].SetPassive();
  m_pTypeInfo[SKILL_SWEEPER_BONUS_12].SetMP(0);

  //------------------------------------------------------------------------------------------------
  // EffectStatus �� Skill ��
  //------------------------------------------------------------------------------------------------
  m_pTypeInfo[SKILL_LOVE_CHAIN].Set(0, "Love Chain", 0, 0, 245, "Love Chain");
  m_pTypeInfo[SKILL_TRANSLATION].Set(0, "Translation", 0, 0, 253, "Translation");
  m_pTypeInfo[SKILL_MEGAPHONE].Set(0, "Mega Phone", 0, 0, 312, "Mega Phone");
  m_pTypeInfo[SKILL_MUTE].Set(0, "Mute", 0, 0, 320, "Mute");
  m_pTypeInfo[FAMILY_BONUS].Set(0, "Family Bonus", 0, 0, 326, "Family Bonus");

  m_pTypeInfo[SKILL_CLIENT_CAN_ENTER_GDR_LAIR].Set(
      0, "Can Enter GDR Lair", 0, 0, 383, "Can Enter GDR Lair");
  m_pTypeInfo[SKILL_CLIENT_BLINDNESS].Set(0, "Blindness", 0, 0, 78,
                                          "Blindness");
  m_pTypeInfo[SKILL_PLEASURE_EXPLOSION].Set(0, "Pleasure Station", 0, 0, 384,
                                            "Pleasure Station");

  m_pTypeInfo[SKILL_CLIENT_EXP_BONUS_EVENT].Set(0, "Exp Bonus", 0, 0, 483,
                                                "Exp Bonus");

  // Sjheon 2005.06 Add

  m_pTypeInfo[SKILL_CLIENT_BEHEMOTH_FORCE_SCROLL].Set(
      0, "Behemoth Force Scroll", 0, 0, 484, "Behemoth Force Scroll");
  m_pTypeInfo[SKILL_CLIENT_SAFE_FORCE_SCROLL].Set(0, "Safe Force Scroll", 0, 0,
                                                  485, "Safe Force Scroll");
  m_pTypeInfo[SKILL_CLIENT_CARNELIAN_FORCE_SCROLL].Set(
      0, "Carnelian Force Scroll", 0, 0, 486, "Carnelian Force Scroll");
  // Sjheon 2005.06 End

  // Sjheon 2005.06 Blood Bible ���� ��ų Add

  m_pTypeInfo[SKILL_HOLY_SWORD].Set(0, "Holy sword", 0, 0, 487, "Holy sword");
  m_pTypeInfo[SKILL_HOLY_BLADE].Set(0, "Holy blade", 0, 0, 487, "Holy blade");

  m_pTypeInfo[SKILL_BURST_GUN].Set(0, "Burst Gun", 0, 0, 487, "Burst Gun");
  m_pTypeInfo[SKILL_HOLY_MACE].Set(0, "Holy mace", 0, 0, 487, "Holy mace");

  m_pTypeInfo[SKILL_HOLY_CROSS].Set(0, "Holy cross", 0, 0, 487, "Holy cross");
  m_pTypeInfo[SKILL_DARK_FORCE].Set(0, "Dark Force", 0, 0, 487, "Dark Force");

  m_pTypeInfo[SKILL_NATURAL_FORCE].Set(0, "Natural Force", 0, 0, 487,
                                       "Natural Force");
  m_pTypeInfo[SKILL_FIRE_FORCE].Set(0, "Fire Force", 0, 0, 487, "Fire Force");

  m_pTypeInfo[SKILL_WATER_FORCE].Set(0, "Water Force", 0, 0, 487, "Water Force");

  m_pTypeInfo[SKILL_EARTH_FORCE].Set(0, "Earth Force", 0, 0, 487, "Earth Force");

  m_pTypeInfo[SKILL_CLIENT_PARTY_AURA_HP].Set(0, "Party HP Aura", 0, 0, 488,
                                              "Party HP Aura");
  m_pTypeInfo[SKILL_CLIENT_PARTY_AURA_DEFENSE].Set(0, "Party DEF Aura", 0, 0, 488,
                                                   "Party DEF Aura");
  m_pTypeInfo[SKILL_CLIENT_PARTY_AURA_TOHIT].Set(0, "Party ToHit Aura", 0, 0, 488,
                                                 "Party ToHit Aura");
  m_pTypeInfo[SKILL_CLIENT_PARTY_AURA_LUCKY].Set(0, "Party LUCK Aura", 0, 0, 488,
                                                 "Party LUCK Aura");
  m_pTypeInfo[SKILL_CLIENT_PARTY_AURA_DAMAGE].Set(0, "Party DAM Aura", 0, 0, 488,
                                                  "Party DAM Aura");
  m_pTypeInfo[SKILL_CLIENT_PARTY_AURA_RESIST].Set(0, "Party RESIST Aura", 0, 0, 488,
                                                  "Party RESIST Aura");

  // Sjheon 2005.08 Market ������  Add
  m_pTypeInfo[SKILL_CLIENT_MAGICAL_PET_CHANGER].Set(0, "Magical Pet Active", 0,
                                                    0, 490, "Magical Pet Active");

  m_pTypeInfo[SKILL_CLIENT_HOODLUM_STIGMA].Set(0, "Hoodlum Stigma", 0, 0, 492,
                                               "Hoodlum Stigma");

#if __CONTENTS(__EXPERT_PET_CHANGER)
  m_pTypeInfo[SKILL_CLIENT_EXPERT_PET_CHANGER].Set(
      0, "Expert Pet Changer", 0, 0, 490, "Expert Pet Changer");
#endif //__EXPERT_PET_CHANGER

#if __CONTENTS(__JAPAN_REVERSE_DEVELOP)
  m_pTypeInfo[SKILL_CLIENT_ORIGIN_OF_FURY].Set(0, "Origin of Fury", 0, 0, 3,
                                               "Origin of Fury");
#endif //__JAPAN_REVERSE_DEVELOP

  m_pTypeInfo[SKILL_CLIENT_FURIOUS_INCENSE].Set(0, "Furious Incense", 0, 0, 492,
                                                "Furious Incense");
  m_pTypeInfo[SKILL_CLIENT_STRENGTHEN_INCENSE].Set(0, "Strengthen Incense", 0,
                                                   0, 492, "Strengthen Incense");
  m_pTypeInfo[SKILL_CLIENT_HYPERACTIVE_INCENSE].Set(0, "Hyperactive Incense", 0,
                                                    0, 492, "Hyperactive Incense");
  m_pTypeInfo[SKILL_CLIENT_IMPREGNABLE_INCENSE].Set(0, "Impregnable Incense", 0,
                                                    0, 492, "Impregnable Incense");

  m_pTypeInfo[SKILL_CLIENT_GREYFELL_FORCE_SCROLL].Set(
      0, "Greyfell Force Scroll", 0, 0, 484, "Greyfell Force Scroll");
  m_pTypeInfo[SKILL_CLIENT_SOVIUS_FORCE_SCROLL].Set(
      0, "Sovius Force Scroll", 0, 0, 485, "Sovius Force Scroll");
  m_pTypeInfo[SKILL_CLIENT_MIR_FORCE_SCROLL].Set(0, "Mir Force Scroll", 0, 0,
                                                 486, "Mir Force Scroll");

  m_pTypeInfo[SKILL_CLIENT_BLEEDING].Set(0, "Bleeding", 0, 0, 493, "Bleeding");
  m_pTypeInfo[SKILL_CLIENT_PARASITE].Set(0, "Parasite", 0, 0, 494,
                                         "Parasite");
  /// m_pTypeInfo[SKILL_CLIENT_SOUND_WAVE].Set(0, "Sound Wave", 0, 0,  484,"����
  /// ����");
  m_pTypeInfo[SKILL_CLIENT_SILENCE].Set(0, "Silence", 0, 0, 495, "Silence");

  m_pTypeInfo[SKILL_CLIENT_EDGE_ARCH].Set(0, "Edge Arch", 0, 0, 496,
                                          "Edge Arch");
  //
  m_pTypeInfo[SKILL_CLIENT_BOMB_CRASH_WALK_SPARK].Set(0, "Spark", 0, 0, 499,
                                                      "Spark");

  m_pTypeInfo[SKILL_CLIENT_BLOOD_CURSE].Set(0, "Blood Curse", 0, 0, 505,
                                            "Blood Curse");

  m_pTypeInfo[SKILL_CLIENT_DUMMY_DRAKE].Set(0, "Dummy Drake", 0, 0, 508,
                                            "Dummy Drake");

  m_pTypeInfo[SKILL_CLIENT_HYDRO_CONVERGENCE].Set(0, "Hydro Convergence", 0, 0,
                                                  509, "Hydro Convergence");

  m_pTypeInfo[SKILL_CLIENT_HARPOON_BOMB].Set(0, "Harpoon Bomb", 0, 0, 471,
                                             "Harpoon Bomb");

  m_pTypeInfo[SKILL_CLIENT_MYSTERY_POTION].Set(0, "Mystery Potion", 0, 0, 512,
                                               "Mystery Potion");

  m_pTypeInfo[SKILL_CLIENT_ODIN_FORCE_SCROLL].Set(0, "Odin Force Scroll", 0, 0,
                                                  484, "Odin Force Scroll");
  m_pTypeInfo[SKILL_CLIENT_TITUOS_FORCE_SCROLL].Set(
      0, "Tituos Force Scroll", 0, 0, 485, "Tituos Force Scroll");

  m_pTypeInfo[SKILL_CLIENT_PET_BONUS_EXP].Set(0, "Pet Exp Bonus", 0, 0, 483,
                                              "Pet Exp Bonus");

  m_pTypeInfo[SKILL_CLIENT_BLOODY_SHOUT].Set(0, "Bloody Shout", 0, 0, 516,
                                             "Bloody Shout");

#if __CONTENTS(__MENEGROTH)

  m_pTypeInfo[SKILL_CLIENT_MENEGROTH_TIME].Set(0, "Menegroth", 0, 0, 533,
                                               "Menegroth");

#endif //__MENEGROTH
#if __CONTENTS(__PREMIUM_CARD)
  m_pTypeInfo[SKILL_CLIENT_PREMIUM_CARD].Set(0, "Premium Card", 0, 0, 534,
                                             "Premium Card");
#endif //__PREMIUM_CARD
#if __CONTENTS(__DSG_EFFECT)
  m_pTypeInfo[SKILL_CLIENT_DSG].Set(0, "Phoenix Specialist Group", 0, 0, 536,
                                    "Phoenix Specialist Group");
#endif //__DSG_EFFECT

#if __CONTENTS(__080327_ORDINIS_VAPOR)
  m_pTypeInfo[SKILL_CLIENT_ORDINIS_VAPOR_RED].Set(0, "Ordinis Vapor Red", 0, 0,
                                                  537, "Ordinis Vapor Red");
  m_pTypeInfo[SKILL_CLIENT_ORDINIS_VAPOR_BLUE].Set(
      0, "Ordinis Vapor Blue", 0, 0, 538, "Ordinis Vapor Blue");
#endif //__080327_ORDINIS_VAPOR

#if __CONTENTS(__JAPAN_REVERSE_DEVELOP)
  m_pTypeInfo[SKILL_CLIENT_BONUS_ITEM_LOOTING_RATIO].Set(
      0, "Item Looting Ratio Bonus", 0, 0, 483, "Item Looting Ratio Bonus");
#endif //__JAPAN_REVERSE_DEVELOP

#if __CONTENTS(__EXTREME_FORCE_SCROLL)
  m_pTypeInfo[SKILL_CLIENT_EFFECT_ITEM_EXTREME_FORCE_SCROLL].Set(
      0, "Extreme Force Scroll", 0, 0, 556, "Extreme Force Scroll");
#endif //__EXTREME_FORCE_SCROLL

#if __CONTENTS(__MASTER_EFFECTS)
  m_pTypeInfo[SKILL_CLIENT_MASTER_EFFECT_ANGEL_FLUTTER].Set(
      0, "Angel's Flutter", 0, 0, 557, "Angel's Flutter");
  m_pTypeInfo[SKILL_CLIENT_MASTER_EFFECT_SHADOW_OF_BLOODY_DARKNESS].Set(
      0, "Shadow of Bloody Darkness", 0, 0, 558, "Shadow of Bloody Darkness");
  m_pTypeInfo[SKILL_CLIENT_MASTER_EFFECT_ARIEL_BREATHING].Set(
      0, "Ariel's Breathing", 0, 0, 559, "Ariel's Breathing");
#endif //__MASTER_EFFECTS

#if __CONTENTS(__NONE_ADVANCEMENT_SANDGLASS)
  m_pTypeInfo[SKILL_CLIENT_EFFECT_ITEM_BONUS_EXP_SECOND].Set(
      0, "Exp Bonus(100%)", 0, 0, 483, "Exp Bonus(100%)");
#endif //__NONE_ADVANCEMENT_SANDGLASS

#if __CONTENTS(__20091028_ADD_ITEM)
  m_pTypeInfo[SKILL_CLIENT_EFFECT_ITEM_BONUS_EXP_ADVANCE].Set(
      0, "Exp Bonus(50%)", 0, 0, 483, "Exp Bonus(50%)");
  m_pTypeInfo[SKILL_CLIENT_EFFECT_ITEM_ACCURA_FORCE_SCROLL].Set(
      0, "Accura Force Scroll", 0, 0, 556, "Accura Force Scroll");
  m_pTypeInfo[SKILL_CLIENT_EFFECT_ITEM_LIFE_FORCE_SCROLL].Set(
      0, "Life Force Scroll", 0, 0, 556, "Life Force Scroll");
  m_pTypeInfo[SKILL_CLIENT_EFFECT_ITEM_CHAOTIC_EXPERIENCE_STONE].Set(
      0, "Experience Stone", 0, 0, 483, "Experience Stone");
#endif //__20091028_ADD_ITEM

#if __CONTENTS(__20091116_ADD_RARE_SKILL)
  m_pTypeInfo[SKILL_CLIENT_EFFECT_OUSTERS_SQUALLY_BARRIER_1].Set(
      0, "Squally barrier(Defense up 60%)", 0, 0, 562,
      "Squally barrier(Defense up 60%)");
  m_pTypeInfo[SKILL_CLIENT_EFFECT_OUSTERS_SQUALLY_BARRIER_2].Set(
      0, "Squally barrier(Defense up 80%)", 0, 0, 563,
      "Squally barrier(Defense up 80%)");
#endif //__20091116_ADD_RARE_SKILL

  // m_pTypeInfo[SKILL_CLIENT_FIRE_BREATH].Set(0, "Fire Breath", 0, 0,
  // 253,"���̷� �극��");

  // m_pTypeInfo[SKILL_BLEEDING].Set(0, "Bleeding", 0, 0,  486,"����");

  // m_pTypeInfo[SKILL_PARTY_AURA].Set(0, "Party aura", 0, 0, 487,"��Ƽ������");

  //	//m_pTypeInfo[SKILL_LOSE_SIGHT].Set(0, "Lose sight", 0, 0,
  //486,"�������Ʈ");

  // m_pTypeInfo[SKILL_WIDE_BLOOD_DRAIN].Set(0, "Wide blood drain", 0, 0,
  // 486,"���̵� ����� �巹��");
  /// m_pTypeInfo[SKILL_NATURAL_PEACE].Set(0, "Natural peace", 0, 0, 486,"����
  /// �ǽ� ");

  // m_pTypeInfo[SKILL_PARTY_AURA].Set(0, "Party aura", 0, 0, 486,"��Ƽ������");
  // Sjheon 2005.06 Blood Bible ���� ��ų End

  // m_pTypeInfo[SKILL_CLIENT_DRAGON_EYES].Set(0, "Dragon Eyes", 0, 0,
  // 384,"�巹���� ��"); - ����Blood Bible�Ƿ� �ϴ� ����..��
  //----------------------------------------------------------------------
  //  Skill Step�� ����..
  //----------------------------------------------------------------------
  //  m_listSkillStep�� �������ָ� ó���Ҷ� ���ϴ�.
  //----------------------------------------------------------------------
#endif

  //----------------------------------------------------------------------
  // �ٲ�� ������ �ʱ�ȭ
  //----------------------------------------------------------------------
  Init();
}

MSkillInfoTable::~MSkillInfoTable() {}

//----------------------------------------------------------------------
//
// member functions
//
//----------------------------------------------------------------------
//----------------------------------------------------------------------
// Init
//----------------------------------------------------------------------
void MSkillInfoTable::Init() {
  // 2026-05-20 ROOT CAUSE FIX: Heap OOB write
  // Eski kod literal MIN_RESULT_ACTIONINFO (=1163) ile loop yapiyordu, ama
  // CTypeTable Skill.inf header'ina gore (1113 entry) tablo boyutunu Resize
  // ediyordu (CTypeTable.h:130-154). Mismatch: loop 0..1162 iterate, tablo
  // sadece 0..1112 slot. i=1113..1162 (50 entry) OOB heap write ->
  // ~250 OOB member assignment -> ~5KB heap region corrupt.
  // Sonra MSkillManager::Init MSkillDomain[8] allocate edince kirlenmis
  // bolgeye dusebiliyor -> map'lerin red-black tree pointer'lari cop ->
  // AddSkill recursion 10 derinlikte m_mapSkillID.find AV (crash).
  // Fix: GetSize() runtime size (Skill.inf'ten gelen) kullan -> in-bounds garanti.
  const int size = GetSize();
  for (int i = 0; i < size; i++) {
    m_pTypeInfo[i].SetExpLevel(0);
    m_pTypeInfo[i].SetSkillExp(0);
    m_pTypeInfo[i].SetDelayTime(0);
    m_pTypeInfo[i].SetAvailableTime();
    m_pTypeInfo[i].SetEnable();
  }

  // delay set — SUMMON_HELICOPTER enum value Skill.inf range disinda olabilir
  // (Phoenix enum drift), guard ekle (yine in-bounds garanti).
  if ((int)SUMMON_HELICOPTER < size) {
    m_pTypeInfo[SUMMON_HELICOPTER].SetDelayTime(3000);
  }
  //	m_pTypeInfo[MAGIC_BLOODY_TUNNEL].SetDelayTime( 3000 );
  //	m_pTypeInfo[MAGIC_BLOODY_MARK].SetDelayTime( 3000 );
}

//----------------------------------------------------------------------
// Load From File  ServerSkillInfo
//----------------------------------------------------------------------
void MSkillInfoTable::LoadFromFileServerSkillInfo(ivfstream &file) {
  int num, skillType;

  file.read((char *)&num, 4);

  if (num < 0 || num > 100000)
    return; // [Antigravity] Sanity check to prevent huge allocations loop

  for (int i = 0; i < num; i++) {
    file.read((char *)&skillType, 4);

    // [Antigravity] Bounds check to prevent out-of-bounds access
    if (skillType < 0 || skillType >= MIN_RESULT_ACTIONINFO)
      return;

    // ������ skillType�� loading�Ѵ�.
    m_pTypeInfo[skillType].LoadFromFileServerSkillInfo(file);
  }

  m_pTypeInfo[MAGIC_THROW_HOLY_WATER].minRange = 8; // �ּ� �����Ÿ�
  m_pTypeInfo[MAGIC_THROW_HOLY_WATER].maxRange = 8; // �ּ� �����Ÿ�

  m_pTypeInfo[BOMB_SPLINTER].minRange = 6; // �ּ� �����Ÿ�
  m_pTypeInfo[BOMB_SPLINTER].maxRange = 6; // �ּ� �����Ÿ�

  m_pTypeInfo[BOMB_ACER].minRange = 6; // �ּ� �����Ÿ�
  m_pTypeInfo[BOMB_ACER].maxRange = 6; // �ּ� �����Ÿ�

  m_pTypeInfo[BOMB_BULLS].minRange = 6; // �ּ� �����Ÿ�
  m_pTypeInfo[BOMB_BULLS].maxRange = 6; // �ּ� �����Ÿ�

  m_pTypeInfo[BOMB_STUN].minRange = 6; // �ּ� �����Ÿ�
  m_pTypeInfo[BOMB_STUN].maxRange = 6; // �ּ� �����Ÿ�

  m_pTypeInfo[BOMB_CROSSBOW].minRange = 6; // �ּ� �����Ÿ�
  m_pTypeInfo[BOMB_CROSSBOW].maxRange = 6; // �ּ� �����Ÿ�

  m_pTypeInfo[BOMB_TWISTER].minRange = 6; // �ּ� �����Ÿ�
  m_pTypeInfo[BOMB_TWISTER].maxRange = 6; // �ּ� �����Ÿ�

  m_pTypeInfo[MAGIC_RAPID_GLIDING].minRange = 2;
  m_pTypeInfo[MAGIC_RAPID_GLIDING].maxRange = 6;

  m_pTypeInfo[SKILL_ULTIMATE_BLOW].minRange = 1;
  m_pTypeInfo[SKILL_ULTIMATE_BLOW].maxRange = 3;
}

//----------------------------------------------------------------------
// Set AvailableTime
//----------------------------------------------------------------------
// ���� �ٷ� ��� �����ϰ� �����Ѵ�.
//----------------------------------------------------------------------
void MSkillInfoTable::SetAvailableTime(TYPE_ACTIONINFO id, int delay) {
#ifdef __GAME_CLIENT__
  SKILLINFO_NODE &skillNode = Get(id);

  skillNode.SetAvailableTime(delay);

  const SKILLINFO_NODE::SKILLID_LIST &delayGroupList =
      skillNode.GetDelayGroupList();

  SKILLINFO_NODE::SKILLID_LIST::const_iterator pos = delayGroupList.begin();
  for (; pos != delayGroupList.end(); ++pos) {
    SKILLINFO_NODE &delaySkillNode = Get(*pos);
    delaySkillNode.SetAvailableTime(delay);
  }

#endif
}

//----------------------------------------------------------------------
// Set Next AvailableTime
//----------------------------------------------------------------------
// ���� ��� ������ �ð��� �����Ѵ�.
//----------------------------------------------------------------------
void MSkillInfoTable::SetNextAvailableTime(TYPE_ACTIONINFO id) {
#ifdef __GAME_CLIENT__
  // ����ð����� delayTime�Ŀ��� ��� �����ϴ�.

  SKILLINFO_NODE &skillNode = Get(id);

  skillNode.SetNextAvailableTime();

  const SKILLINFO_NODE::SKILLID_LIST &delayGroupList =
      skillNode.GetDelayGroupList();

  SKILLINFO_NODE::SKILLID_LIST::const_iterator pos = delayGroupList.begin();
  for (; pos != delayGroupList.end(); ++pos) {
    SKILLINFO_NODE &delaySkillNode = Get(*pos);
    delaySkillNode.SetNextAvailableTime();
  }

#endif
}
