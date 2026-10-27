
//----------------------------------------------------------------------
// SkillDef.h
//----------------------------------------------------------------------

#ifndef	__SKILLDEF_H__
#define	__SKILLDEF_H__

#include "ContentsFilter.h"

// defines
#define SELF_SKILL_MP_POINT			5
#define TILE_SKILL_MP_POINT			5
#define OBJECT_SKILL_MP_POINT		5

#define MAX_VAMPIRE_SKILL_SLOT		20	// Test
#define MAX_SLAYER_SKILL_SLOT		20


//----------------------------------------------------------------------
// Skill Domain
//----------------------------------------------------------------------
enum SKILLDOMAIN {
	SKILLDOMAIN_BLADE,
	SKILLDOMAIN_SWORD,
	SKILLDOMAIN_GUN,
	SKILLDOMAIN_HEAL,
	SKILLDOMAIN_ENCHANT,	
	SKILLDOMAIN_ETC,
	SKILLDOMAIN_VAMPIRE,	
	SKILLDOMAIN_OUSTERS,

	MAX_SKILLDOMAIN
};

// Name String
extern int SKILLDOMAIN_NAME[MAX_SKILLDOMAIN];

//----------------------------------------------------------------------
// Skill Step
//----------------------------------------------------------------------
enum SKILL_STEP {
	SKILL_STEP_NULL,			// 관계없는 것들

	// Slayer
	SKILL_STEP_APPRENTICE,
	SKILL_STEP_ADEPT,
	SKILL_STEP_EXPERT,
	SKILL_STEP_MASTER, 
	SKILL_STEP_GRAND_MASTER,
	SKILL_STEP_SLAYER_BLADE_ADVANCEMENT,
	SKILL_STEP_SLAYER_SWORD_ADVANCEMENT,
	SKILL_STEP_SLAYER_GUN_BOMB_ADVANCEMENT,
	SKILL_STEP_SLAYER_HEAL_ADVANCEMENT,
	SKILL_STEP_SLAYER_ENCHANT_ADVANCEMENT,

	// Vampire
	SKILL_STEP_VAMPIRE_POISON,
	SKILL_STEP_VAMPIRE_ACID,
	SKILL_STEP_VAMPIRE_CURSE,
	SKILL_STEP_VAMPIRE_SUMMON,
	SKILL_STEP_VAMPIRE_BLOOD,
	SKILL_STEP_VAMPIRE_INNATE,
	SKILL_STEP_VAMPIRE_WARRIOR_ADVANCEMENT,
	SKILL_STEP_VAMPIRE_MAGICIAN_ADVANCEMENT,
	
	// Ousters
	SKILL_STEP_OUSTERS_COMBAT,
	SKILL_STEP_OUSTERS_ELEMENTAL,
	SKILL_STEP_OUSTERS_ETC,
	SKILL_STEP_OUSTERS_COMMON_ADVANCEMENT,
	SKILL_STEP_OUSTERS_COMBAT_ADVANCEMENT,
	SKILL_STEP_OUSTERS_EARTH_ADVANCEMENT,
	SKILL_STEP_OUSTERS_FIRE_ADVANCEMENT,
	SKILL_STEP_OUSTERS_WATER_ADVANCEMENT,

	SKILL_STEP_ETC,

	// by chyaya
	SKILL_STEP_SLAYER_RARE,
	SKILL_STEP_VAMPIRE_RARE,
	SKILL_STEP_OUSTERS_RARE,
	SKILL_STEP_COMMON_RARE,
};

//----------------------------------------------------------------------
// SkillID == ActionInfo 정의
//----------------------------------------------------------------------
enum ACTIONINFO
{
		// 검계열,
	SKILL_ATTACK_MELEE = 0,						// 0
	SKILL_ATTACK_ARMS,							// 1
	SKILL_SELF,									// 2
	SKILL_TILE,									// 3
	SKILL_OBJECT,	    						// 4

	SKILL_DOUBLE_IMPACT,  					  	// 5
	SKILL_TRIPLE_SLASHER,				  	  	// 6
	SKILL_RAINBOW_SLASHER,					  	// 7
	SKILL_THUNDER_SPARK,	   				 	// 8
	SKILL_DANCING_SWORD,						// 92
	SKILL_CROSS_COUNTER,		   			 	// 10
	SKILL_FLASH_SLIDING,		  			  	// 11
	SKILL_LIGHTNING_HAND,	  					// 12
	SKILL_SNAKE_COMBO,	  	 				 	// 9
	SKILL_SWORD_WAVE,		  	 				// 13
	SKILL_DRAGON_RISING,		 			    // 14
	SKILL_FIVE_STORM_CRASH,	 				 	// 15
	SKILL_HEAVENS_SWORD,		  			  	// 16

	// 도 계열,
	SKILL_SINGLE_BLOW,						    // 17
	SKILL_SPIRAL_SLAY,						    // 18
	SKILL_TRIPLE_BREAK,						    // 19
	SKILL_WILD_SMASH,						    // 20
	SKILL_GHOST_BLADE,							// 93
	SKILL_POTENTIAL_EXPLOSION,			  		// 22
	SKILL_SHADOW_WALK,			  			  	// 23
	SKILL_CHARGING_POWER,		  			  	// 24
	SKILL_HURRICANE_COMBO,	 				 	// 21
	SKILL_TORNADO_SEVER,		  			  	// 25
	SKILL_ARMAGEDDON_SLASHER,	  				// 26
	SKILL_SOUL_SHOCK,			   				// 27
	SKILL_SAINT_BLADE,			   			  	// 28

	// 건, 라이플 계열
	SKILL_FAST_RELOAD,						    // 30
	SKILL_QUICK_FIRE,							//
	SKILL_SMG_MASTERY,							// 
	SKILL_MULTI_SHOT,						    // 94
	SKILL_HEAD_SHOT,						    // 94
	SKILL_AR_MASTERY,							// 
	SKILL_PIERCING,						    	// 97
	SKILL_SNIPPING,						    	// 95
	SKILL_SG_MASTERY,							//
	SKILL_DETECT_MINE,						    // 35
	SKILL_MAKE_BOMB,		 				    // 36
	SKILL_SR_MASTERY,
	SKILL_DISARM_MINE,						    // 37
	SKILL_INSTALL_MINE,						    // 37
	SKILL_MAKE_MINE,			  			    // 38
    
	// 성직마법,
	// 인첸 계열
	MAGIC_CREATE_HOLY_WATER, 			 		// 39
	MAGIC_LIGHT,				      			// 40
	MAGIC_DETECT_HIDDEN,	   	 				// 41
	MAGIC_AURA_BALL,	     		 		 	// 55
	MAGIC_BLESS,			       		 		// 45
	MAGIC_CONTINUAL_LIGHT,	 			 		// 46
	MAGIC_FLARE,								// 얼마일까 다시 정리 해야지
	MAGIC_PURIFY,    						// 43
	MAGIC_AURA_RING,		  	  		  		// 42
	MAGIC_STRIKING,		      					// 44
	MAGIC_DETECT_INVISIBILITY,					// 48
	MAGIC_IDENTIFY,								//
	MAGIC_AURA_SHIELD,	    					// 60
	MAGIC_ENCHANT,	    					// 60
	MAGIC_VISIBLE,			    			  	// 49
	MAGIC_CHAIN_AURA,							// 50
	MAGIC_SAINT_AURA,							// 51

	// 힐 계열
	MAGIC_CURE_LIGHT_WOUNDS,					// 52
	MAGIC_CURE_POISON,							// 54
	MAGIC_PROTECTION_FROM_POISON,				// 53
	MAGIC_CAUSE_LIGHT_WOUNDS,					// 얼마일까 다시 정리 해야지
	MAGIC_CURE_SERIOUS_WOUNDS,					// 56
	MAGIC_REMOVE_CURSE,							// 89
	MAGIC_PROTECTION_FROM_CURSE,				// 59
	MAGIC_CAUSE_SERIOUS_WOUNDS,					// 얼마일까 다시 정리 해야지
	MAGIC_CURE_CRITICAL_WOUNDS,					// 
	MAGIC_PROTECTION_FROM_ACID,					//
	MAGIC_SACRIFICE,							//
	MAGIC_CAUSE_CRITICAL_WOUNDS,				// 얼마일까 다시 정리 해야지
	MAGIC_CURE_ALL,								// 62
	MAGIC_REGENERATION,							// 61
	MAGIC_MASS_CURE,							//
	MAGIC_MASS_HEAL,							//

	// 뱀프기술,
	SKILL_BLOOD_DRAIN,							// 65

	// 뱀프마법,
	// Poison
	MAGIC_POISONOUS_HANDS,						// 67
	MAGIC_GREEN_POISON,							// 69
	MAGIC_YELLOW_POISON,						// 70
	MAGIC_DARKBLUE_POISON,						// 79
	MAGIC_GREEN_STALKER,						// 75

	// Acid
	MAGIC_ACID_TOUCH,							// 68
	MAGIC_ACID_BOLT,							// 73
	MAGIC_ACID_BALL,							// 78
	MAGIC_ACID_SWAMP,							// 81

	// Curse
	MAGIC_PARALYZE,								// 74
	MAGIC_DOOM,									// 74
	MAGIC_HALLUCINATION,							//
	MAGIC_DEATH,								//

	// Bloody
	MAGIC_BLOODY_NAIL,							// 82
	MAGIC_BLOODY_KNIFE,							// 83
	MAGIC_BLOODY_BALL,							// 84
	MAGIC_BLOODY_WALL,							// 85
	MAGIC_BLOODY_SPEAR,							// 86

	// Innate
	MAGIC_HIDE,									// 66
	MAGIC_DARKNESS,								// 71
//	MAGIC_PURPLE_WALL,							// 72
	MAGIC_INVISIBILITY,							// 76
	MAGIC_TRANSFORM_TO_WOLF,					// 77
	MAGIC_TRANSFORM_TO_BAT,						// 80

	// 써몬
	MAGIC_SUMMON_WOLF,							// 87
	MAGIC_SUMMON_CASKET,						// 88
	MAGIC_RAISING_DEAD,							// 88
	MAGIC_SUMMON_SERVANT,						//

	// ETC
	MAGIC_UN_BURROW,							// 89
	MAGIC_UN_TRANSFORM,							// 89
	MAGIC_UN_INVISIBILITY,
	MAGIC_THROW_HOLY_WATER,						// 91

	// 늑대 기술
	MAGIC_EAT_CORPSE,
	MAGIC_HOWL,

	// NPC 기술
	MAGIC_RESTORE,								// 90

	MAGIC_BLOODY_MARK,
	MAGIC_BLOODY_TUNNEL,

	MAGIC_SEDUCTION,
	SKILL_WIND_DIVIDER,
	SKILL_EARTHQUAKE,

	MAGIC_RESURRECT,
	MAGIC_PRAYER,
	MAGIC_MEDITATION,

	// 2001.12.4 추가
	SKILL_THUNDER_BOLT,							// 122
	SKILL_EXPANSION, 							// 123
	SKILL_MIRACLE_SHIELD, 						// 124
	SKILL_THUNDER_FLASH,						// 125
	SKILL_MASSACRE,								// 126
	SKILL_INVINCIBLE,							// 127

	SKILL_BERSERKER,							// 128
	SKILL_MOONLIGHT_SEVER,						// 129
	SKILL_SHADOW_DANCING,						// 130
	SKILL_TYPHOON,								// 131
	SKILL_PSYCHOKINESIS,						// 132
	SKILL_EXTERMINATION,						// 133

	SKILL_MIND_CONTROL,							// 134
	SKILL_REVOLVING,							// 135
	SKILL_FATALITY,								// 136
	SKILL_BLITZ,								// 137

	SKILL_ACTIVATION,							// 138
	SKILL_PEACE,								// 139
	SKILL_ENERGY_DROP,							// 140
	SKILL_EXORCISM,								// 141

	SKILL_SANCTUARY,							// 142
	SKILL_REFLECTION,							// 143
	SKILL_ARMAGEDDON,							// 144

	
	SKILL_POISON_STRIKE,						// 145
	SKILL_POISON_STORM,							// 146
	SKILL_ACID_STRIKE,							// 147
	SKILL_ACID_STORM,							// 148
	SKILL_BLOODY_STRIKE,						// 149
	SKILL_BLOODY_STORM,							// 150
	
	// 2002.3.23
	SKILL_SUMMON_BAT,
	SKILL_CHARM,
	SKILL_POLYMORPH,
	SKILL_MEPHISTO,
	SKILL_HYPNOSIS,
	SKILL_TRANSFUSION,
	SKILL_EXTREME,
	SKILL_BLOODY_WAVE,	

	SKILL_THROW_BOMB,

	SKILL_DOUBLE_SHOT,
	SKILL_TRIPLE_SHOT,
	
	SKILL_CURE_EFFECT,			// cure
	SKILL_CRITICAL_EFFECT,
	SKILL_CRITICAL_GROUND,		// critical을 사용하는 경우	
	
	SKILL_VIGOR_DROP,
	
	// [새기술]
	SKILL_SWORD_MASTERY,                        // 166
    SKILL_SHIELD_MASTERY,                       // 167
    SKILL_THUNDER_STORM,                        // 168
    SKILL_CONCENTRATION,                        // 169
    SKILL_EVASION,                              // 170
    SKILL_HOLY_BLAST,                           // 171
    SKILL_HYMN,                                 // 172
	SKILL_MENTAL_SWORD,							// 173
	SKILL_OBSERVING_EYE,						// 174
	SKILL_REFLECTION_EFFECT,					// 175

	// 2002.6.21
	SKILL_TEMP,									// 176
	MAGIC_OPEN_CASKET,							// 177

	// 2002.9.2
	MAGIC_SUMMON_MONSTERS,						// 178
	MAGIC_GROUND_ATTACK,						// 179
	MAGIC_METEOR,								// 180
	MAGIC_DUPLICATE_SELF,						// 181
	MAGIC_BLOODY_MASTER_WAVE,							// 182

	// 2002.10.1
    MAGIC_BLOODY_WARP,                          // 183
    MAGIC_BLOODY_SNAKE,                         // 184

	// 2002.10.23
	SKILL_SOUL_CHAIN,							// 185

	// 2002.11.18
	SKILL_LIVENESS,								// 186

	// 2002.11.20
	MAGIC_DARKNESS_WIDE,							// 187
	SKILL_POISON_STORM_WIDE,						// 188
	SKILL_ACID_STORM_WIDE,							// 189

    // 2002.12.26
    SKILL_SHARP_SHIELD,                         // 190
    SKILL_WIDE_LIGHTNING,                       // 191
    SKILL_AIR_SHIELD,                           // 192
    SKILL_POWER_OF_LAND,                        // 193
    SKILL_BULLET_OF_LIGHT,                      // 194
    SKILL_GUN_SHOT_GUIDANCE,                    // 195
    SKILL_REBUKE,                               // 196
    SKILL_SPIRIT_GUARD,                         // 197
    SKILL_TURN_UNDEAD,                          // 198

	SKILL_HANDS_OF_WISDOM,						// 199
	SKILL_LIGHT_BALL,							// 200
	SKILL_HOLY_ARROW,							// 201

	// 2003.2.26 

	MAGIC_BLOODY_BREAKER,						// 202
	MAGIC_RAPID_GLIDING,						// 203

    SKILL_MAGIC_ELUSION,                        // 204
    SKILL_POISON_MESH,                          // 205
    SKILL_ILLUSION_OF_AVENGE,                   // 206
    SKILL_WILL_OF_LIFE,                         // 207
	
	SKILL_DENIAL_MAGIC,							// 208
	SKILL_REQUITAL,								// 209
	SKILL_CONCEALMENT,							// 210
	SKILL_SWORD_RAY,							// 211
	SKILL_MULTI_AMPUTATE,						// 212
	SKILL_NAIL_MASTERY,							// 213	
	
	SKILL_HIT_CONVERT,							// 214
	SKILL_WILD_TYPHOON,							// 215
	SKILL_ULTIMATE_BLOW,						// 216
	SKILL_ILLENDUE,								// 217
	SKILL_LIGHTNESS,							// 218

	SKILL_FLOURISH,                             // 219
    SKILL_EVADE,                                // 220
    SKILL_SHARP_ROUND,                          // 221
    SKILL_HIDE_SIGHT,                           // 222
    SKILL_BACK_STAB,                            // 223
    SKILL_BLUNTING,                             // 224
    SKILL_GAMMA_CHOP,                           // 225

    SKILL_CROSS_GUARD,                          // 226
    SKILL_FIRE_OF_SOUL_STONE,                   // 227
    SKILL_ICE_OF_SOUL_STONE,                    // 228
    SKILL_SAND_OF_SOUL_STONE,                   // 229
    SKILL_BLOCK_HEAD,                           // 230
    SKILL_KASAS_ARROW,                          // 231
    SKILL_HANDS_OF_FIRE,                        // 232

    SKILL_PROMINENCE,                           // 233
    SKILL_RING_OF_FLARE,                        // 234
    SKILL_BLAZE_BOLT,                           // 235
    SKILL_ICE_FIELD,                            // 236
    SKILL_WATER_BARRIER,                        // 237
    SKILL_HANDS_OF_NIZIE,                       // 238
    SKILL_NYMPH_RECOVERY,                       // 239

    SKILL_LIBERTY,                              // 240
    SKILL_TENDRIL,                              // 241
    SKILL_GNOMES_WHISPER,                       // 242
    SKILL_STONE_AUGER,                          // 243
    SKILL_REFUSAL_ETHER,                        // 244
    SKILL_EARTHS_TEETH,                         // 245
    SKILL_ABSORB_SOUL,                          // 246

    SKILL_SUMMON_SYLPH,                         // 247
    SKILL_DRIFTING_SOUL,                        // 248
    SKILL_CRITICAL_MAGIC,                       // 249
	
	SKILL_EMISSION_WATER,						// 250
	SKILL_BEAT_HEAD,							// 251
	SKILL_DIVINE_SPIRITS,						// 252
	
	SKILL_BLITZ_SLIDING,                        // 253
    SKILL_BLAZE_WALK,                           // 254
    SKILL_JABBING_VEIN,                         // 255
    SKILL_GREAT_HEAL,                           // 256
    SKILL_DIVINE_GUIDANCE,                      // 257

	MAGIC_BLOODY_ZENITH,						// 258	
	
	SKILL_REDIANCE,								// 259
	SKILL_LARSLASH,								// 260
	SKILL_HEART_CATALYST,						// 261
	SKILL_ARMS_MASTERY1,						// 262
	SKILL_VIVID_MAGAZINE,						// 263
	SKILL_TRIDENT,								// 264
	SKILL_ARMS_MASTERY2,						// 265
	SKILL_MOLE_SHOT,							// 266

	SKILL_ETERNITY,								// 267
	SKILL_PROTECTION_FROM_BLOOD,				// 268

	SKILL_INSTALL_TRAP,                         // 269
	SKILL_CREATE_HOLY_POTION,                   // 270
	SKILL_MERCY_GROUND,                         // 271
	SKILL_HOLY_ARMOR,                           // 272
	
	SKILL_TRANSFORM_TO_WERWOLF,					// 273
	SKILL_STONE_SKIN,							// 274

	SKILL_ACID_ERUPTION,						// 275
	SKILL_TALON_OF_CROW,						// 276
	SKILL_GRAY_DARKNESS,						// 277	

    SKILL_BITE_OF_DEATH,                        // 278

	SKILL_WIDE_GRAY_DARKNESS,					// 279
    SKILL_TELEPORT,                             // 280

    SKILL_FIRE_PIERCING,                        // 281
    SKILL_SUMMON_OF_FIRE_ELEMENTAL,             // 282
    SKILL_MAGNUM_SPEAR,                         // 283
    SKILL_HELLFIRE,                             // 284

    SKILL_ICE_LANCE,                            // 285
    SKILL_FROZEN_ARMOR,                         // 286
    SKILL_SUMMON_OF_WATER_ELEMENTAL,            // 287
    SKILL_EXPLOSION_WATER,                      // 288
    SKILL_SOUL_REBIRTH,                         // 289
    SKILL_SOUL_REBIRTH_MASTERY,                 // 290

    SKILL_REACTIVE_ARMOR,                       // 291
    SKILL_GROUND_BLESS,                         // 292
    SKILL_SUMMON_OF_GROUND_ELEMENTAL,           // 293
    SKILL_METEOR_STORM,                         // 294

    SKILL_SHARP_CHAKRAM,                        // 295
    SKILL_SHIFT_BREAK,                          // 296
    SKILL_WATER_SHIELD,                         // 297
    SKILL_DESTRUCTION_SPEAR,                    // 298
    SKILL_BLESS_FIRE,                           // 299
    SKILL_FATAL_SNICK,                          // 300
    SKILL_SAND_CROSS,                           // 301
    SKILL_DUCKING_WALLOP,                       // 302
    SKILL_CHARGING_ATTACK,                      // 303
    SKILL_DISTANCE_BLITZ,                       // 304

	SKILL_FABULOUS_SOUL,						//305
	SKILL_WILL_OF_IRON,							//306

 // 질드레 스킬
    SKILL_WIDE_ICE_FIELD,                       // 307
    SKILL_GLACIER_1,                            // 308
    SKILL_GLACIER_2,                            // 309
    SKILL_ICE_AUGER,                            // 310
    SKILL_ICE_HAIL,                             // 311
    SKILL_WIDE_ICE_HAIL,                        // 312
    SKILL_ICE_WAVE,                             // 313

	SKILL_LAND_MINE_EXPLOSION,                  // 314
    SKILL_CLAYMORE_EXPLOSION,                   // 315
    SKILL_PLEASURE_EXPLOSION,                   // 316

	SKILL_DELEO_EFFICIO,                        // 317  // DELETE EFFECT
    SKILL_REPUTO_FACTUM,                        // 318

	SKILL_SWORD_OF_THOR,                        // 319
    SKILL_BURNING_SOUL_CHARGING,                // 320
    SKILL_BURNING_SOUL_LAUNCH,                  // 321
    SKILL_INSTALL_TURRET,                       // 322
    SKILL_TURRET_FIRE,                          // 323
    SKILL_SWEEP_VICE_1,                         // 324
    SKILL_SWEEP_VICE_3,                         // 325
	SKILL_SWEEP_VICE_5,                         // 326
    SKILL_WHITSUNTIDE,                          // 327
    SKILL_VIOLENT_PHANTOM,                      // 328
    SKILL_SUMMON_GORE_GLAND,                    // 329
    SKILL_GORE_GLAND_FIRE,                      // 330
    SKILL_DESTRUCTION_SPEAR_MASTERY,            // 331
    SKILL_FATAL_SNICK_MASTERY,                  // 332
	
	SKILL_MAGNUM_SPEAR_MASTERY,                // 333
    SKILL_ICE_LANCE_MASTERY,                  // 334
    SKILL_REACTIVE_ARMOR_MASTERY,             // 335

	SKILL_THROWING_AXE,                         // 336
    SKILL_CHOPPING_FIREWOOD,                    // 337 장작패기	

	SKILL_CHAIN_THROWING_AXE,                   // 338
    SKILL_MULTI_THROWING_AXE,                   // 339
    SKILL_PLAYING_WITH_FIRE,                    // 340 불장난

	SKILL_INFINITY_THUNDERBOLT,                 // 341
    SKILL_SPIT_STREAM,                          // 342
    SKILL_PLASMA_ROCKET_LAUNCHER,               // 343
    SKILL_INTIMATE_GRAIL,                       // 344
    SKILL_BOMBING_STAR,                         // 345
    SKILL_SET_AFIRE,                            // 346
    SKILL_NOOSE_OF_WRAITH,                      // 347

    SKILL_SHARP_HAIL,                           // 348
    SKILL_SUMMON_MIGA,                          // 349  // 아우스터즈가 쓰는 스킬
    SKILL_SUMMON_MIGA_ATTACK,                   // 350  // 소환된놈이 쓰는 스킬
    SKILL_ICE_HORIZON,                          // 351
    SKILL_FURY_OF_GNOME,                        // 352
	
	SKILL_CANNONADE,                          // 353  // 포격
    SKILL_SELF_DESTRUCTION,                   // 354  // 자폭공격

	SKILL_AR_ATTACK,                            // 355  // 몬스터용
    SKILL_SMG_ATTACK,                           // 356  // 몬스터용
    SKILL_GRENADE_ATTACK,                       // 357  // 몬스터용

	SKILL_DRAGON_TORNADO,                       // 358
    SKILL_BIKE_CRASH,                           // 359
    SKILL_HARPOON_BOMB,                         // 360
    SKILL_PASSING_HEAL,                         // 361
    SKILL_ROTTEN_APPLE,                         // 362
    SKILL_WILD_WOLF,                            // 363
    SKILL_ABERRATION,                           // 364
	SKILL_HALO,                                 // 365
    SKILL_DESTINIES,                            // 366
    SKILL_FIERCE_FLAME,                         // 367
    SKILL_SHADOW_OF_STORM,                      // 368
	
	SKILL_HEAL_PASS,                            // 369 // 옮겨가는 힐

	SKILL_TRASLA_ATTACK,                      // 370
    SKILL_PUSCA_ATTACK,                       // 371
	SKILL_NOD_COPILA_ATTACK,                  // 372
    SKILL_NOD_COPILA_ATTACK_2,                    // 373
    SKILL_UNTERFELDWEBEL_FIRE,                  // 374
    SKILL_FELDWEBEL_FIRE,                       // 375
	SKILL_FEEBLE_VIRUS ,						// 376

	SKILL_SWORD_COMBO,                          // 377
    SKILL_BLADE_COMBO,                          // 378
    SKILL_VAMPIRE_COMBO,                        // 379
    SKILL_OUSTERS_COMBO,                        // 380

    SKILL_HOLY_SWORD,                           // 381
    SKILL_HOLY_BLADE,                           // 382
    SKILL_BURST_GUN,                            // 383
    SKILL_HOLY_MACE,                            // 384
    SKILL_HOLY_CROSS,                           // 385
    SKILL_DARK_FORCE,                           // 386
    SKILL_NATURAL_FORCE,                        // 387
    SKILL_FIRE_FORCE,                           // 388
    SKILL_WATER_FORCE,                          // 389
    SKILL_EARTH_FORCE,                          // 390
    SKILL_LOSE_SIGHT,                           // 391
    SKILL_WIDE_BLOOD_DRAIN,                     // 392
    SKILL_NATURAL_PEACE,                        // 393
    SKILL_PARTY_AURA,                           // 394

    SKILL_MULTI_DOOM,                           // 395
    SKILL_MULTI_PARALYZE,                       // 396

    SKILL_BRANDISH,                             // 397
    SKILL_PARASITE,                             // 398
    SKILL_SOUND_WAVE,                           // 399
	SKILL_FIRE_WAVE,                            // 400
	SKILL_TARGET_SUMMON_MONSTERS,				// 401
	SKILL_BLOODY_BURST,							// 402
	SKILL_TRACE_CREATURE,						// 403

	// AC Slayer 2nd Skill
	SKILL_SHINE_SWORD,							// 404
	SKILL_BOMB_CRASH_WALK,						// 405
	SKILL_SATELLITE_BOMB,						// 406
	SKILL_ILLUSION_INVERSION,					// 407
	SKILL_HEAVEN_GROUND,						// 408
	
	// AC Vampire 2nd Skill
	SKILL_BLOODY_SCARIFY,						// 409

	// AC Ousters 2nd Skill
    SKILL_HETER_CHAKRAM,                        // 410
    SKILL_DUMMY_DRAKE,                          // 411
    SKILL_HYDRO_CONVERGENCE,                    // 412
    SKILL_SUMMON_CLAY,                          // 413

	// AC Vampire 2nd Skill
    SKILL_BLOOD_CURSE,                          // 414
    SKILL_BLOODS_SYMPOSION_ATTACK,              // 415

	SKILL_HETER_CHAKRAM_CHAIN,					// 416
	
	// 일자코흐 프롤로그
	SKILL_TANNING,								// 417

	SKILL_REQUEST_RESURRECT,					// 418
	
	// Skill Book - by chyaya
	SKILL_MAGIC_SHIELD1,                        // 419
    SKILL_MAGIC_SHIELD2,                        // 420

    SKILL_EAGLES_EYE1,                          // 421
    SKILL_EAGLES_EYE2,                          // 422

    SKILL_FREEZE_RING1,                         // 423
    SKILL_FREEZE_RING2,                         // 424

    SKILL_BAT_STORM1,                           // 425
    SKILL_BAT_STORM2,                           // 426

    SKILL_CURSE_OF_BLOOD1,                      // 427
    SKILL_CURSE_OF_BLOOD2,                      // 428

    SKILL_BLOOD_DRAIN2,                         // 429

	SKILL_BLOODY_SHOUT1,						// 430
	SKILL_BLOODY_SHOUT2,						// 431

	SKILL_BLOODY_WINGS,                         // 432

    SKILL_MIST_OF_SOUL1,                        // 433
    SKILL_MIST_OF_SOUL2,                        // 434

	SKILL_ENERGY_BURST,							// 435
	
	SKILL_MAGIC_AVOID1,							// 436
	SKILL_MAGIC_AVOID2,							// 437
	SKILL_MAGIC_AVOID3,							// 438

#if __CONTENTS(__MENEGROTH)
	
	SKILL_CERBERUS,								//439
	SKILL_MANTICORET,							//440
	SKILL_BOGLE_H,								//441
	SKILL_BOGLE_B,								//442
	SKILL_MASSACRE_MENEGROTH,					//443
	SKILL_PLUMYBOAR,							//444
	SKILL_CABRACAM,								//445
#endif //__MENEGROTH
#if __CONTENTS(__SECOND_RARE_SKILL)
	//이렇게 해야 안꼬인다.
	SKILL_MENEGROTH_TRAP1,						//446
	SKILL_MENEGROTH_TRAP2,						//447
	SKILL_HEAVENLY_SPARK,						//448
	SKILL_BLADE_STORM,							//449
	SKILL_XRL,									//450
	SKILL_GENESIS,								//451
	SKILL_THORNS,								//452
	SKILL_LETHAL_CLAW,							//453
	SKILL_VENOM_CIRCLE,							//454
	SKILL_BRIONAC,								//455
	SKILL_ASTEROID_BOMBER,						//456
	SKILL_FROZEN_GLACIER,						//457
	SKILL_INCINERATE,							//458
	// 레어스킬 패시브
	SKILL_INCREASE_OF_HEALTH1,					//459
	SKILL_INCREASE_OF_HEALTH2,					//460
	SKILL_INCREASE_OF_HEALTH3,					//461
	SKILL_INCREASE_OF_HEALTH4,					//462
	SKILL_INCREASE_OF_HEALTH5,					//463
	SKILL_INCREASE_OF_HEALTH6,					//464
#endif //__SECOND_RARE_SKILL

#if __CONTENTS(__TIPOJYU_CASTLE)	//티포주 몬스터 스킬 (서버와 번호가 맞아야 한다.)
	SKILL_TIFFAUGES_BALBERITH_BLUE = 465,  // 465  // 문지기 도끼
	SKILL_TIFFAUGES_BALBERITH_RED,			// 466  
 
	SKILL_TIFFAUGES_BERITH_BLUE,			// 467  // 근위대 창
	SKILL_TIFFAUGES_BERITH_RED,				// 468  
 
	SKILL_TIFFAUGES_VALKYRJA_BLUE,			// 469  // 근위대 대장
	SKILL_TIFFAUGES_VALKYRJA_RED,			// 470  
 
	SKILL_TIFFAUGES_GOMORY_A_BLUE,			// 471  // 여자 백작
	SKILL_TIFFAUGES_GOMORY_B_BLUE,			// 472
	SKILL_TIFFAUGES_GOMORY_A_RED,			// 473
	SKILL_TIFFAUGES_GOMORY_B_RED,			// 474
 
	SKILL_TIFFAUGES_KRSNIK_A_BLUE,			// 475  // 남자 백작
	SKILL_TIFFAUGES_KRSNIK_B_BLUE,			// 476
	SKILL_TIFFAUGES_KRSNIK_A_RED,			// 477
	SKILL_TIFFAUGES_KRSNIK_B_RED,			// 478
 
	SKILL_TIFFAUGES_GUSION_A_BLUE,			// 479  // 남자 백작 대장
	SKILL_TIFFAUGES_GUSION_B_BLUE,			// 480
	SKILL_TIFFAUGES_GUSION_C_BLUE,			// 481
	SKILL_TIFFAUGES_GUSION_A_RED,			// 482
	SKILL_TIFFAUGES_GUSION_B_RED,			// 483
	SKILL_TIFFAUGES_GUSION_C_RED,			// 484
 
	SKILL_TIFFAUGES_PAIMON_BLUE,			// 485  // 경비병
	SKILL_TIFFAUGES_PAIMON_RED,				// 486
 
	SKILL_TIFFAUGES_MORGOTH_A_BLUE,			// 487  // 웨버데스
	SKILL_TIFFAUGES_MORGOTH_B_BLUE,			// 488
	SKILL_TIFFAUGES_MORGOTH_C_BLUE,			// 489
	SKILL_TIFFAUGES_MORGOTH_A_RED,			// 490
	SKILL_TIFFAUGES_MORGOTH_B_RED,			// 491
	SKILL_TIFFAUGES_MORGOTH_C_RED,			// 492
#endif // __TIPOJYU_CASTLE
#if __CONTENTS(__ONIBLA)
	SKILL_ONIBLA_RAISE_BONE,				// 493 오니블라
	SKILL_ONIBLA_DROP_SKULL,				// 494

	SKILL_AGONY_CHILD_DROP_HEAD,			// 495 아고니 차일드
	SKILL_AGONY_CHILD_SHADOW_FOUR,			// 496

	SKILL_VILLAIN_LADY_GHOST_SHADOW,		// 497 // 빌라인 레이디
	SKILL_VILLAIN_LADY_SELF_DESTRUCTION,	// 498 // 빌라인 레이디

	SKILL_SPANIEL_THROWING_AXE,				// 499 // 오니 스패니얼
	SKILL_BLACKASTRAL_MAGIC_CRYSTAL,		// 500 // 블랙 아스트랄
	SKILL_WHITEASTRAL_SPELL_CRYSTAL,		// 501 // 화이트 크리스탈

#endif //__ONIBLA
#if __CONTENTS(__20091116_ADD_RARE_SKILL)
	SKILL_BLADE_MAGMA_DETONATION_1,		// 502	- 도슬 레어스킬 1차 마그마 데토네이션
	SKILL_BLADE_MAGMA_DETONATION_2,		// 503	- 도슬 레어스킬 2차 마그마 데토네이션
	SKILL_OUSTERS_SQUALLY_BARRIER_1,	// 504	- 아우스터즈 레어스킬 1차 질풍의 베리어
	SKILL_OUSTERS_SQUALLY_BARRIER_2,	// 505	- 아우스터즈 레어스킬 2차 질풍의 베리어
#endif //__20091116_ADD_RARE_SKILL
    //rallser 劤속세콘
	SKILL_SIUS_CRASH,//繫법 506   1
    SKILL_LYCAN_HOWL,//繫법 507   2
    SKILL_BIRFRONZE_MAGIC,//繫법 508 3
    SKILL_PALUS_MIDDLE_RANGE_MAGIC,//4繫법509
    SKILL_PALUS_LONG_RANGE_MAGIC,//5繫법510
    SKILL_GARUM_MAGIC,//繫법511 6
    SKILL_CLAVIE_MAGIC,//繫법512 7
    SKILL_Wide_Wild_Wolf,//繫법513 8
    SKILL_RALLSER_1,//繫법514 9
    SKILL_SMILE_OF_DEATH,//10繫법515
    SKILL_FURY_SHOCK,//繫법516 11
    SKILL_RAIL_GUN_ATTACK,//繫법517 12
	//SKILL_NOT_1,
    SKILL_BLAZE_WALK_2,//518繫법 13
    SKILL_GLADIATOR,//519繫법14
    SKILL_CRUSHING_STORM,//520繫법15
    SKILL_SATELLITE_BOMB_2,//521繫법16
	SKILL_INTIMATE_GRAIL_2,//522 17
	SKILL_HOLY_ARMOR_2,//523 18
	SKILL_STRIKING_2,//524 19
	SKILL_BLESS_2,//525 20
	SKILL_GLORY_GROUND,//526 21
	SKILL_PASSING_HEAL_K2,//257 22
	SKILL_GREAT_HEAL_2,//258 23
	SKILL_CHAIN_OF_DEMON,//259 24
	SKILL_BLOODY_SKULL,//260 25
	SKILL_RAGE_OF_BLOOD,//261 26
	SKILL_ALLYSTER_WIND,//262 27
	SKILL_FLAME_SIGHT,//263 28
	SKILL_SPIRAL_MEGALITH,//264 29
	SKILL_RADCHIA,//265 30
	SKILL_HEAL_PASS_2,//266 31
	SKILL_BREATH_OF_DRYAD,//267 32
	SKILL_CURE_EFFECT_2,//268 33
	SKILL_LIVINGDEAD_B_ECHO_ESPER,//269 34
	SKILL_MIHAI_BAT_STREAM,//270 35
	SKILL_TSAR_BLUE_ROSE_DEVIL,//271 36
	SKILL_FOLLET_POISON_FRAME,//37
	SKILL_FUE_FOLLET_RED_ROSE_DEVIL,//38
	SKILL_DACULE_TALON_OF_DDRACULE,//39
	SKILL_DRACULE_BLOOKDY_SCARIFY_2,//40
	SKILL_DRACULE_BLOODY_CARPE,//41
	SKILL_DRACULE_DRACULE_CASKET,//42
	SKILL_DRAGON_HURRICANE,//43
	SKILL_LAR_STROKE,//44
	SKILL_GAE_BULGA,//45
	SKILL_VICIOUS_GUIDANCE,//46
	SKILL_SPECTOR_INVERSE, //47
	SKILL_BAT_BREAKER,//48
    SKILL_SHADY_DOUPLE,//49
	SKILL_CHAKRAM_HAIL,//50
	SKILL_FLAME_SPIKE,//51
	SKILL_BLIZZARD,//52
	SKILL_DEMOLISHER,//53
	SKILL_RALLSER_2,//54 sheng ming shu 7
	SKILL_RALLSER_3,//55 8
	SKILL_RALLSER_4,//56  9
	//SKILL_RALLSER_44,
	//SKILL_RALLSER_5,
	//SKILL_RALLSER_6,
	//SKILL_RALLSER_7,
	//SKILL_RALLSER_8,
	SKILL_CLIENT_SHADY_DOUPLE,//57
	SKILL_CLIENT_BAT_BREAKER,//58  



	//------------------------------------------------------------
	// 여기부터는 client 전용 SKILL ID
	//------------------------------------------------------------
	// 임시로 추가	
	
	MAGIC_BLOODY_TUNNEL_INTO,
//	MAGIC_OPEN_CASKET,
	SUMMON_HELICOPTER,	

	
	// Bomb
	BOMB_SPLINTER,
	BOMB_ACER,
	BOMB_BULLS,	
	BOMB_STUN,
	BOMB_CROSSBOW,
	BOMB_TWISTER,

	// Mine
	MINE_ANKLE_KILLER,
	MINE_POMZ,
	MINE_AP_C1,
	MINE_DIAMONDBACK,
	MINE_SWIFT_EX,
	MINE_SIDEWINDER,
	MINE_COBRA,

	// gun
	SKILL_ATTACK_GUN_SR,			// 총1
	SKILL_ATTACK_GUN_SG,			// 총2
	SKILL_ATTACK_GUN_AR,			// 총3
	SKILL_ATTACK_GUN_SMG,			// 총4

	// sword
	SKILL_ATTACK_SWORD,		// 칼1
	SKILL_ATTACK_BLADE,		// 칼2	

	// 총에 따른 holy shooting
	SKILL_HOLY_SHOOTING_TR,					//	Holy shooting
	SKILL_HOLY_SHOOTING_SG,					//	Holy shooting
	SKILL_HOLY_SHOOTING_AR,					//	Holy shooting
	SKILL_HOLY_SHOOTING_SMG,				//	Holy shooting

	// etc
	GUIDANCE_FIRE,
	FALLING_LIGHTNING,
	ATTACH_FIRE,
	ATTACH_AURA,  

	SKILL_MEGAPHONE,
	SKILL_BLITZ_SLIDING_ATTACK,
	SKILL_BLAZE_WALK_ATTACK,
	SKILL_MUTE,	
	SKILL_ATTACK_MELEE_REDIANCE,
	FAMILY_BONUS,	

	//Combo Skill Result
	SKILL_SWORD_COMBO_EFFECT,
	SKILL_BLADE_COMBO_EFFECT,
	SKILL_VAMPIRE_COMBO_EFFECT,
	SKILL_OUSTERS_COMBO_EFFECT,

	// Dummy Skill - 이팩트를 보여주지 않고 시전 동작만 있는 Client Skill이다.
	SKILL_DUMMY_ACTION_MAGIC,

	//------------------------------------------------------------------------------
	// 스킬 단계에 따른 스킬
	//------------------------------------------------------------------------------
	STEP_SKILL_FLOURISH_2,							// 2단계 FLOURISH
	STEP_SKILL_FLOURISH_3,							// 3단계 FLOURISH
	STEP_SKILL_SHARP_ROUND_2,						// 2단계 Sharp Round
	STEP_SKILL_SHARP_ROUND_3,						// 3단계 Sharp Round
	STEP_SKILL_GAMMA_CHOP_2,
	STEP_SKILL_GAMMA_CHOP_3,
	STEP_SKILL_CROSS_GUARD_2,
	STEP_SKILL_CROSS_GUARD_3,
	STEP_SKILL_PROMINENCE_2,
	STEP_SKILL_PROMINENCE_3,
	STEP_SKILL_HANDS_OF_NIZE_2,
	STEP_SKILL_HANDS_OF_NIZE_3,
	STEP_SKILL_NYMPH_RECOVERY_2,
	STEP_SKILL_NYMPH_RECOVERY_3,
	STEP_SKILL_EARTHS_TEETH_2,
	STEP_SKILL_EARTHS_TEETH_3,
	STEP_SKILL_REFUSAL_ETHER_2,
	STEP_SKILL_REFUSAL_ETHER_3,
	STEP_SKILL_KASAS_ARROW_2,
	STEP_SKILL_KASAS_ARROW_3,
	STEP_SKILL_BLAZE_BOLT_2,
	STEP_SKILL_BLAZE_BOLT_3,
	STEP_SKILL_STONE_AUGER_2,
	STEP_SKILL_STONE_AUGER_3,

	STEP_SKILL_EMISSION_WATER_2,
	STEP_SKILL_EMISSION_WATER_3,
	STEP_SKILL_BEAT_HEAD_2,
	STEP_SKILL_BEAT_HEAD_3,
	STEP_SKILL_MAGNUM_SPEAR_2,
	STEP_SKILL_MAGNUM_SPEAR_3,
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------

	// enchant
	MAGIC_ENCHANT_OPTION_NULL,				// 옵션이 없어진다
	MAGIC_ENCHANT_OPTION_PLUS,				// 옵션이 하나 올라간다
	MAGIC_ENCHANT_REMOVE_ITEM,				// 아이템이 없어진다

	// HolyLandBonus Skills 2003.2.10
	SKILL_HOLYLAND_BLOOD_BIBLE_ARMEGA,				// 기술 사용시 MP(HP)소모량 50% 감소  
	SKILL_HOLYLAND_BLOOD_BIBLE_MIHOLE,				// 타종족의 언어 통역  
	SKILL_HOLYLAND_BLOOD_BIBLE_KIRO,				// INT+5, DEX +5  
	SKILL_HOLYLAND_BLOOD_BIBLE_INI,					// 물리공격 데미지 +10  
	SKILL_HOLYLAND_BLOOD_BIBLE_GREGORI,				// 물약값 50% 할인  
	SKILL_HOLYLAND_BLOOD_BIBLE_CONCILIA,			// HP +50  
	SKILL_HOLYLAND_BLOOD_BIBLE_LEGIOS,				// 마법공격데미지 +10  
	SKILL_HOLYLAND_BLOOD_BIBLE_HILLEL,				// 행운 +10  
	SKILL_HOLYLAND_BLOOD_BIBLE_JAVE,				// 겜블가격 50% 할인  
	SKILL_HOLYLAND_BLOOD_BIBLE_NEMA,				// 시야 +5, 모든 능력치 +3  
	SKILL_HOLYLAND_BLOOD_BIBLE_AROSA,				// 모든 저항력 +9  
	SKILL_HOLYLAND_BLOOD_BIBLE_CHASPA,				// STR +5, INT +5 

	SKILL_LOVE_CHAIN,								// 러브 체인 스킬	
	
	MAGIC_TRANS_ITEM_OK,	
	SKILL_TRANSLATION,
	
	SKILL_SWEEPER_BONUS_1,							// 스위퍼 보너스
	SKILL_SWEEPER_BONUS_2,							// 스위퍼 보너스
	SKILL_SWEEPER_BONUS_3,							// 스위퍼 보너스
	SKILL_SWEEPER_BONUS_4,							// 스위퍼 보너스
	SKILL_SWEEPER_BONUS_5,							// 스위퍼 보너스
	SKILL_SWEEPER_BONUS_6,							// 스위퍼 보너스
	SKILL_SWEEPER_BONUS_7,							// 스위퍼 보너스
	SKILL_SWEEPER_BONUS_8,							// 스위퍼 보너스
	SKILL_SWEEPER_BONUS_9,							// 스위퍼 보너스
	SKILL_SWEEPER_BONUS_10,							// 스위퍼 보너스
	SKILL_SWEEPER_BONUS_11,							// 스위퍼 보너스
	SKILL_SWEEPER_BONUS_12,							// 스위퍼 보너스
	
	// 2004, 6, 15 sobeit add start 
	SKILL_CLIENT_CAN_ENTER_GDR_LAIR, //- 질드레 들어갈수 있음
	SKILL_CLIENT_BLINDNESS,			// 다크니스와 같은 효과
	// 2004, 6, 15 sobeit add end - 
	SKILL_CLIENT_DRAGON_EYES,	// 드레곤 아이즈

	SKILL_CLIENT_EXP_BONUS_EVENT, // 경험치 보너스
		

	SKILL_CLIENT_BEHEMOTH_FORCE_SCROLL,         // 465
    SKILL_CLIENT_SAFE_FORCE_SCROLL,             // 466
    SKILL_CLIENT_CARNELIAN_FORCE_SCROLL,		// 467	

	SKILL_CLIENT_PARTY_AURA_HP,                 // 468
    SKILL_CLIENT_PARTY_AURA_DEFENSE,            // 469
    SKILL_CLIENT_PARTY_AURA_TOHIT,              // 470
    SKILL_CLIENT_PARTY_AURA_LUCKY,              // 471

	SKILL_CLIENT_PARTY_AURA_DAMAGE,             // 472
    SKILL_CLIENT_PARTY_AURA_RESIST,             // 473
	SKILL_CLIENT_MAGICAL_PET_CHANGER,			// 474
	SKILL_CLIENT_HOODLUM_STIGMA,				// 475

	SKILL_CLIENT_FURIOUS_INCENSE,               // 476
    SKILL_CLIENT_STRENGTHEN_INCENSE,            // 477
    SKILL_CLIENT_HYPERACTIVE_INCENSE,           // 478
    SKILL_CLIENT_IMPREGNABLE_INCENSE,           // 479

	SKILL_CLIENT_GREYFELL_FORCE_SCROLL,         // 480
    SKILL_CLIENT_SOVIUS_FORCE_SCROLL,           // 481
    SKILL_CLIENT_MIR_FORCE_SCROLL,				// 482
	
	SKILL_CLIENT_BLEEDING,						// 483
	SKILL_CLIENT_PARASITE,						// 484
	SKILL_CLIENT_SOUND_WAVE,					// 485
	SKILL_CLIENT_SILENCE,						// 486
	SKILL_CLIENT_FIRE_BREATH,					// 487
	SKILL_CLIENT_EDGE_ARCH,						// 488
	
	SKILL_CLIENT_BOMB_CRASH_WALK_SPARK,			// 489
	SKILL_CLIENT_BLOOD_CURSE,					// 490
	SKILL_CLIENT_DUMMY_DRAKE,					// 491

	SKILL_CLIENT_HYDRO_CONVERGENCE,				// 492

	SKILL_CLIENT_HARPOON_BOMB,					// 493

	SKILL_CLIENT_MYSTERY_POTION,				// 494

	SKILL_CLIENT_ODIN_FORCE_SCROLL,				// 495
    SKILL_CLIENT_TITUOS_FORCE_SCROLL,			// 496


	SKILL_CLIENT_PET_BONUS_EXP,					// 497


	SKILL_CLIENT_BLOODY_SHOUT,					// 498

	MAGIC_ENCHANT_THIRD_OPTION_NULL,			// 499
	MAGIC_ENCHANT_THIRD_OPTION_PLUS,			// 500

#if __CONTENTS(__HERO_SYSTEM)
	SKILL_CLIENT_SLAYER_HERO,					// 501
	SKILL_CLIENT_VAMPIRE_HERO,					// 502
	SKILL_CLIENT_OUSTERS_HERO,					// 503
#endif //__HERO_SYSTEM

#if __CONTENTS(__MENEGROTH)
	SKILL_CLIENT_MENEGROTH_TIME,				//504
#endif //__MENEGROTH
	
#if __CONTENTS(__PREMIUM_CARD)
	SKILL_CLIENT_PREMIUM_CARD,					//505
#endif //__PREMIUM_CARD

#if __CONTENTS(__DSG_EFFECT)
	SKILL_CLIENT_DSG,							//506
#endif //__DSG_EFFECT

#if __CONTENTS(__080327_ORDINIS_VAPOR)
	SKILL_CLIENT_ORDINIS_VAPOR_RED,				//507
	SKILL_CLIENT_ORDINIS_VAPOR_BLUE,			//508
#endif //__080327_ORDINIS_VAPOR

#if __CONTENTS(__EXPERT_PET_CHANGER)
	SKILL_CLIENT_EXPERT_PET_CHANGER,			//509
#endif //__EXPERT_PET_CHANGER

//#if __CONTENTS(__JAPAN_REVERSE_DEVELOP)
//	SKILL_CLIENT_BONUS_ITEM_LOOTING_RATIO,		//510
//	SKILL_CLIENT_ORIGIN_OF_FURY,				//511
//#endif //__JAPAN_REVERSE_DEVELOP

#if __CONTENTS(__EXTREME_FORCE_SCROLL)
	SKILL_CLIENT_EFFECT_ITEM_EXTREME_FORCE_SCROLL,			//512
#endif //__EXTREME_FORCE_SCROLL

#if __CONTENTS(__MASTER_EFFECTS)
	SKILL_CLIENT_MASTER_EFFECT_ANGEL_FLUTTER,				//513
	SKILL_CLIENT_MASTER_EFFECT_SHADOW_OF_BLOODY_DARKNESS,	//514
	SKILL_CLIENT_MASTER_EFFECT_ARIEL_BREATHING,				//515
#endif //__MASTER_EFFECTS

#if __CONTENTS(__NONE_ADVANCEMENT_SANDGLASS)
	SKILL_CLIENT_EFFECT_ITEM_BONUS_EXP_SECOND,				//516
#endif //__NONE_ADVANCEMENT_SANDGLASS

#if __CONTENTS(__20091028_ADD_ITEM)
	SKILL_CLIENT_EFFECT_ITEM_BONUS_EXP_ADVANCE,				//517
	SKILL_CLIENT_EFFECT_ITEM_ACCURA_FORCE_SCROLL,			//518
	SKILL_CLIENT_EFFECT_ITEM_LIFE_FORCE_SCROLL,				//519
	SKILL_CLIENT_EFFECT_ITEM_CHAOTIC_EXPERIENCE_STONE,		//520
#endif //__20091028_ADD_ITEM

#if __CONTENTS(__20091116_ADD_RARE_SKILL)
	SKILL_CLIENT_EFFECT_OUSTERS_SQUALLY_BARRIER_1,			// 521
	SKILL_CLIENT_EFFECT_OUSTERS_SQUALLY_BARRIER_2,			// 522
#endif //__20091116_ADD_RARE_SKILL
//	SKILL_GIFTBOX_FOR_PLAYER,
	SKILL_CLIENT_SIUS_DEATH,//1
    SKILL_CLIENT_RUPER_ZONE_BUFF,//2
    SKILL_CLIENT_RALLSER_1,//3
    SKILL_CLIENT_CREATURE_TELEPORT_START,//4
    SKILL_CLIENT_CREATURE_TELEPORT_END,//5
    SKILL_CLIENT_MASS_HYPNOTISM,//6
    SKILL_CLIENT_DEATH_OF_TOPF,//7
    SKILL_CLIENT_CRUSHING_STORM,//8
    SKILL_CLIENT_RAD_CHIA,//9
    SKILL_CLIENT_TSAR_DEAD,//10
    SKILL_CLIENT_FUE_FOLLET_DEAD,//11
    SKILL_CLIENT_CASTLE_GATE,//12
    SKILL_CLIENT_CASTLE_GATE_BRAKE,//13
    SKILL_CLIENT_CASTLE_PLATFORM_BRAKE,//14
    SKILL_CLIENT_CASTLE_PLATFORM,//15
    SKILL_CLIENT_CASTLE_MIHNEA,//16
    SKILL_CLIENT_CASTLE_EMPTY_PLATFORM_BRAKE,//17
    SKILL_CLIENT_CASTLE_EMPTY_PLATFORM,//18
    SKILL_CLIENT_FIERCE_FLAME2,//19
    SKILL_CLIENT_RALLSER_2,//20
    SKILL_CLIENT_RALLSER_3,//21
    SKILL_CLIENT_CASTLE_MIHNEA2,//22
	//-----------------------------------------------------------------
	// RESULT_ACTIONINFO
	//-----------------------------------------------------------------		
	// 기본 기술
	RESULT_SKILL_ATTACK_MELEE,						// 0
	RESULT_SKILL_ATTACK_ARMS,							// 1
	RESULT_SKILL_SELF,									// 2
	RESULT_SKILL_TILE,									// 3
	RESULT_SKILL_OBJECT,	    						// 4

	// 검계열,	
	RESULT_SKILL_DOUBLE_IMPACT,  					  	// 5
	RESULT_SKILL_TRIPLE_SLASHER,				  	  	// 6
	RESULT_SKILL_RAINBOW_SLASHER,					  	// 7
	RESULT_SKILL_THUNDER_SPARK,	   				 	// 8
	RESULT_SKILL_DANCING_SWORD,						// 92
	RESULT_SKILL_CROSS_COUNTER,		   			 	// 10
	RESULT_SKILL_FLASH_SLIDING,		  			  	// 11
	RESULT_SKILL_LIGHTNING_HAND,	  					// 12
	RESULT_SKILL_SNAKE_COMBO,	  	 				 	// 9
	RESULT_SKILL_SWORD_WAVE,		  	 				// 13
	RESULT_SKILL_DRAGON_RISING,		 			    // 14
	RESULT_SKILL_FIVE_STORM_CRASH,	 				 	// 15
	RESULT_SKILL_HEAVENS_SWORD,		  			  	// 16

	// 도 계열,
	RESULT_SKILL_SINGLE_BLOW,						    // 17
	RESULT_SKILL_SPIRAL_SLAY,						    // 18
	RESULT_SKILL_TRIPLE_BREAK,						    // 19
	RESULT_SKILL_WILD_SMASH,						    // 20
	RESULT_SKILL_GHOST_BLADE,							// 93
	RESULT_SKILL_POTENTIAL_EXPLOSION,			  		// 22
	RESULT_SKILL_SHADOW_WALK,			  			  	// 23
	RESULT_SKILL_CHARGING_POWER,		  			  	// 24
	RESULT_SKILL_HURRICANE_COMBO,	 				 	// 21
	RESULT_SKILL_TORNADO_SEVER,		  			  	// 25
	RESULT_SKILL_ARMAGEDDON_SLASHER,	  				// 26
	RESULT_SKILL_SOUL_SHOCK,			   				// 27
	RESULT_SKILL_SAINT_BLADE,			   			  	// 28

	// 건, 라이플 계열
	RESULT_SKILL_FAST_RELOAD,						    // 30
	RESULT_SKILL_QUICK_FIRE,							//
	RESULT_SKILL_SMG_MASTERY,							// 
	RESULT_SKILL_MULTI_SHOT,						    // 94
	RESULT_SKILL_HEAD_SHOT,						    // 94
	RESULT_SKILL_AR_MASTERY,							// 
	RESULT_SKILL_PIERCING,						    	// 97
	RESULT_SKILL_SNIPPING,						    	// 95
	RESULT_SKILL_SG_MASTERY,							//
	RESULT_SKILL_DETECT_MINE,						    // 35
	RESULT_SKILL_MAKE_BOMB,		 				    // 36
	RESULT_SKILL_SR_MASTERY,
	RESULT_SKILL_DISARM_MINE,						    // 37
	RESULT_SKILL_INSTALL_MINE,						    // 37
	RESULT_SKILL_MAKE_MINE,			  			    // 38
    
	// 성직마법,
	// 인첸 계열
	RESULT_MAGIC_CREATE_HOLY_WATER, 			 		// 39
	RESULT_MAGIC_LIGHT,				      			// 40
	RESULT_MAGIC_DETECT_HIDDEN,	   	 				// 41
	RESULT_MAGIC_AURA_BALL,	     		 		 	// 55
	RESULT_MAGIC_BLESS,			       		 		// 45
	RESULT_MAGIC_CONTINUAL_LIGHT,	 			 		// 46
	RESULT_MAGIC_FLARE,								// 얼마일까 다시 정리 해야지
	RESULT_MAGIC_PURIFY,    						// 43
	RESULT_MAGIC_AURA_RING,		  	  		  		// 42
	RESULT_MAGIC_STRIKING,		      					// 44
	RESULT_MAGIC_DETECT_INVISIBILITY,					// 48
	RESULT_MAGIC_IDENTIFY,								//
	RESULT_MAGIC_AURA_SHIELD,	    					// 60
	RESULT_MAGIC_ENCHANT,	    					// 60
	RESULT_MAGIC_VISIBLE,			    			  	// 49
	RESULT_MAGIC_CHAIN_AURA,							// 50
	RESULT_MAGIC_SAINT_AURA,							// 51

	// 힐 계열
	RESULT_MAGIC_CURE_LIGHT_WOUNDS,					// 52
	RESULT_MAGIC_CURE_POISON,							// 54
	RESULT_MAGIC_PROTECTION_FROM_POISON,				// 53
	RESULT_MAGIC_CAUSE_LIGHT_WOUNDS,					// 얼마일까 다시 정리 해야지
	RESULT_MAGIC_CURE_SERIOUS_WOUNDS,					// 56
	RESULT_MAGIC_REMOVE_CURSE,							// 89
	RESULT_MAGIC_PROTECTION_FROM_CURSE,				// 59
	RESULT_MAGIC_CAUSE_SERIOUS_WOUNDS,					// 얼마일까 다시 정리 해야지
	RESULT_MAGIC_CURE_CRITICAL_WOUNDS,					// 
	RESULT_MAGIC_PROTECTION_FROM_ACID,					//
	RESULT_MAGIC_SACRIFICE,							//
	RESULT_MAGIC_CAUSE_CRITICAL_WOUNDS,				// 얼마일까 다시 정리 해야지
	RESULT_MAGIC_CURE_ALL,								// 62
	RESULT_MAGIC_REGENERATION,							// 61
	RESULT_MAGIC_MASS_CURE,							//
	RESULT_MAGIC_MASS_HEAL,							//

	// 뱀프기술,
	RESULT_SKILL_BLOOD_DRAIN,							// 65

	// 뱀프마법,
	// Poison
	RESULT_MAGIC_POISONOUS_HANDS,						// 67
	RESULT_MAGIC_GREEN_POISON,							// 69
	RESULT_MAGIC_YELLOW_POISON,						// 70
	RESULT_MAGIC_DARKBLUE_POISON,						// 79
	RESULT_MAGIC_GREEN_STALKER,						// 75

	// Acid
	RESULT_MAGIC_ACID_TOUCH,							// 68
	RESULT_MAGIC_ACID_BOLT,							// 73
	RESULT_MAGIC_ACID_BALL,							// 78
	RESULT_MAGIC_ACID_SWAMP,							// 81

	// Curse
	RESULT_MAGIC_PARALYZE,						// 74
	RESULT_MAGIC_DOOM,							// 74
	RESULT_MAGIC_HALLUCINATION,							//
	RESULT_MAGIC_DEATH,							//

	// Bloody
	RESULT_MAGIC_BLOODY_NAIL,							// 82
	RESULT_MAGIC_BLOODY_KNIFE,							// 83
	RESULT_MAGIC_BLOODY_BALL,							// 84
	RESULT_MAGIC_BLOODY_WALL,							// 85
	RESULT_MAGIC_BLOODY_SPEAR,							// 86

	// Innate
	RESULT_MAGIC_HIDE,									// 66
	RESULT_MAGIC_DARKNESS,								// 71
//	RESULT_MAGIC_PURPLE_WALL,							// 72
	RESULT_MAGIC_INVISIBILITY,							// 76
	RESULT_MAGIC_TRANSFORM_TO_WOLF,					// 77
	RESULT_MAGIC_TRANSFORM_TO_BAT,						// 80

	// 써몬
	RESULT_MAGIC_SUMMON_WOLF,							// 87
	RESULT_MAGIC_SUMMON_CASKET,						// 88
	RESULT_MAGIC_RAISING_DEAD,							// 88
	RESULT_MAGIC_SUMMON_SERVANT,						//

	// ETC
	RESULT_MAGIC_UN_BURROW,							// 89
	RESULT_MAGIC_UN_TRANSFORM,							// 89
	RESULT_MAGIC_UN_INVISIBILITY,
	RESULT_MAGIC_THROW_HOLY_WATER,						// 91

	// 늑대 기술
	RESULT_MAGIC_EAT_CORPSE,
	RESULT_MAGIC_HOWL,

	RESULT_MAGIC_RESTORE,								// 90

	RESULT_MAGIC_BLOODY_MARK,
	RESULT_MAGIC_BLOODY_TUNNEL,
	
	RESULT_MAGIC_SEDUCTION,
	RESULT_SKILL_WIND_DIVIDER,
	RESULT_SKILL_EARTHQUAKE,

	RESULT_MAGIC_RESURRECT,
	RESULT_MAGIC_PRAYER,
	RESULT_MAGIC_MEDITATION,

	// 2001.12.4 추가
	RESULT_SKILL_THUNDER_BOLT,							// 122
	RESULT_SKILL_EXPANSION, 							// 123
	RESULT_SKILL_MIRACLE_SHIELD, 						// 124
	RESULT_SKILL_THUNDER_FLASH,						// 125
	RESULT_SKILL_MASSACRE,								// 126
	RESULT_SKILL_INVINCIBLE,							// 127

	RESULT_SKILL_BERSERKER,							// 128
	RESULT_SKILL_MOONLIGHT_SEVER,						// 129
	RESULT_SKILL_SHADOW_DANCING,						// 130
	RESULT_SKILL_TYPHOON,								// 131
	RESULT_SKILL_PSYCHOKINESIS,						// 132
	RESULT_SKILL_EXTERMINATION,						// 133

	RESULT_SKILL_MIND_CONTROL,							// 134
	RESULT_SKILL_REVOLVING,							// 135
	RESULT_SKILL_FATALITY,								// 136
	RESULT_SKILL_BLITZ,								// 137

	RESULT_SKILL_ACTIVATION,							// 138
	RESULT_SKILL_PEACE,								// 139
	RESULT_SKILL_ENERGY_DROP,							// 140
	RESULT_SKILL_EXORCISM,								// 141

	RESULT_SKILL_SANCTUARY,							// 142
	RESULT_SKILL_REFLECTION,							// 143
	RESULT_SKILL_ARMAGEDDON,							// 144

	RESULT_SKILL_POISON_STRIKE,						// 145
	RESULT_SKILL_POISON_STORM,							// 146
	RESULT_SKILL_ACID_STRIKE,							// 147
	RESULT_SKILL_ACID_STORM,							// 148
	RESULT_SKILL_BLOODY_STRIKE,						// 149
	RESULT_SKILL_BLOODY_STORM,							// 150

	// 추가 2002.3.23
	RESULT_SKILL_SUMMON_BAT,
	RESULT_SKILL_CHARM,
	RESULT_SKILL_POLYMORPH,
	RESULT_SKILL_MEPHISTO,
	RESULT_SKILL_HYPNOSIS,
	RESULT_SKILL_TRANSFUSION,
	RESULT_SKILL_EXTREME,
	RESULT_SKILL_BLOODY_WAVE,	
	RESULT_SKILL_THROW_BOMB,

	RESULT_SKILL_DOUBLE_SHOT,
	RESULT_SKILL_TRIPLE_SHOT,

	RESULT_SKILL_CURE_EFFECT,			// cure
	RESULT_SKILL_CRITICAL_EFFECT,
	RESULT_SKILL_CRITICAL_GROUND,		// critical을 사용하는 경우	

	RESULT_SKILL_VIGOR_DROP,

	// [새기술]
	RESULT_SKILL_SWORD_MASTERY,                        // 166
    RESULT_SKILL_SHIELD_MASTERY,                       // 167
    RESULT_SKILL_THUNDER_STORM,                        // 168
    RESULT_SKILL_CONCENTRATION,                        // 169
    RESULT_SKILL_EVASION,                              // 170
    RESULT_SKILL_HOLY_BLAST,                           // 171
    RESULT_SKILL_HYMN,                                 // 172
	RESULT_SKILL_MENTAL_SWORD,							// 173
	RESULT_SKILL_OBSERVING_EYE,						// 174
	RESULT_SKILL_REFLECTION_EFFECT,					// 175

	// 2002.6.21
	RESULT_SKILL_TEMP,									// 176
	RESULT_MAGIC_OPEN_CASKET,							// 177

	// 2002.9.2
	RESULT_MAGIC_SUMMON_MONSTERS,						// 178

	RESULT_MAGIC_GROUND_ATTACK,						// 179
	RESULT_MAGIC_METEOR,								// 180

	RESULT_MAGIC_DUPLICATE_SELF,						// 181

	RESULT_MAGIC_BLOODY_MASTER_WAVE,						// 182

	// 2002.10.1
    RESULT_MAGIC_BLOODY_WARP,                          // 183
    RESULT_MAGIC_BLOODY_SNAKE,                         // 184

	RESULT_SKILL_SOUL_CHAIN,							//185

	// 2002.11.18
	RESULT_SKILL_LIVENESS,								// 186

	// 2002.11.20
	RESULT_MAGIC_DARKNESS_WIDE,								// 187
	RESULT_SKILL_POISON_STORM_WIDE,							// 188
	RESULT_SKILL_ACID_STORM_WIDE,							// 189

    // 2002.12.26
    RESULT_SKILL_SHARP_SHIELD,                         // 190
    RESULT_SKILL_WIDE_LIGHTNING,                       // 191
    RESULT_SKILL_AIR_SHIELD,                           // 192
    RESULT_SKILL_POWER_OF_LAND,                        // 193
    RESULT_SKILL_BULLET_OF_LIGHT,                      // 194
    RESULT_SKILL_GUN_SHOT_GUIDANCE,                    // 195
    RESULT_SKILL_REBUKE,                               // 196
    RESULT_SKILL_SPIRIT_GUARD,                         // 197
    RESULT_SKILL_TURN_UNDEAD,                          // 198

	RESULT_SKILL_HANDS_OF_WISDOM,						// 199
	RESULT_SKILL_LIGHT_BALL,							// 200
	RESULT_SKILL_HOLY_ARROW,							// 201

	RESULT_MAGIC_BLOODY_BREAKER,						// 202
	RESULT_MAGIC_RAPID_GLIDING,							// 203

	RESULT_SKILL_MAGIC_ELUSION,							// 204
	RESULT_SKILL_POISON_MESH,							// 205
	RESULT_SKILL_ILLUSION_OF_AVENGE,					// 206
	RESULT_SKILL_WILL_OF_LIFE,							// 207

	RESULT_SKILL_DENIAL_MAGIC,							// 208
	RESULT_SKILL_REQUITAL,								// 209
	RESULT_SKILL_CONCEALMENT,							// 210
	RESULT_SKILL_SWORD_RAY,								// 211
	RESULT_SKILL_MULTI_AMPUTATE,						// 212
	RESULT_SKILL_NAIL_MASTERY,							// 213	
	
	RESULT_SKILL_HIT_CONVERT,							// 214
	RESULT_SKILL_WILD_TYPHOON,							// 215
	RESULT_SKILL_ULTIMATE_BLOW,							// 216
	RESULT_SKILL_ILLENDUE,								// 217
	RESULT_SKILL_LIGHTNESS,								// 218

	RESULT_SKILL_FLOURISH,                             // 219
    RESULT_SKILL_EVADE,                                // 220
    RESULT_SKILL_SHARP_ROUND,                          // 221
    RESULT_SKILL_HIDE_SIGHT,                           // 222
    RESULT_SKILL_BACK_STAB,                            // 223
    RESULT_SKILL_BLUNTING,                             // 224
    RESULT_SKILL_GAMMA_CHOP,                           // 225

    RESULT_SKILL_CROSS_GUARD,                          // 226
    RESULT_SKILL_FIRE_OF_SOUL_STONE,                   // 227
    RESULT_SKILL_ICE_OF_SOUL_STONE,                    // 228
    RESULT_SKILL_SAND_OF_SOUL_STONE,                   // 229
    RESULT_SKILL_BLOCK_HEAD,                           // 230
    RESULT_SKILL_KASAS_ARROW,                          // 231
    RESULT_SKILL_HANDS_OF_FIRE,                        // 232

    RESULT_SKILL_PROMINENCE,                           // 233
    RESULT_SKILL_RING_OF_FLARE,                        // 234
    RESULT_SKILL_BLAZE_BOLT,                           // 235
    RESULT_SKILL_ICE_FIELD,                            // 236
    RESULT_SKILL_WATER_BARRIER,                        // 237
    RESULT_SKILL_HANDS_OF_NIZIE,                       // 238
    RESULT_SKILL_NYMPH_RECOVERY,                       // 239

    RESULT_SKILL_LIBERTY,                              // 240
    RESULT_SKILL_TENDRIL,                              // 241
    RESULT_SKILL_GNOMES_WHISPER,                       // 242
    RESULT_SKILL_STONE_AUGER,                          // 243
    RESULT_SKILL_REFUSAL_ETHER,                        // 244
    RESULT_SKILL_EARTHS_TEETH,                         // 245
    RESULT_SKILL_ABSORB_SOUL,                          // 246

    RESULT_SKILL_SUMMON_SYLPH,                         // 247
    RESULT_SKILL_DRIFTING_SOUL,                        // 248
    RESULT_SKILL_CRITICAL_MAGIC,                       // 249

	RESULT_SKILL_EMISSION_WATER,						// 250
	RESULT_SKILL_BEAT_HEAD,								// 251
	RESULT_SKILL_DIVINE_SPIRITS,						// 252

	RESULT_SKILL_BLITZ_SLIDING,							// 253
    RESULT_SKILL_BLAZE_WALK,							// 254
    RESULT_SKILL_JABBING_VEIN,							// 255
    RESULT_SKILL_GREAT_HEAL,							// 256
    RESULT_SKILL_DIVINE_GUIDANCE,						// 257

	RESULT_MAGIC_BLOODY_ZENITH,							// 258
	
	RESULT_SKILL_REDIANCE,								// 259
	RESULT_SKILL_LARSLASH,								// 260
	RESULT_SKILL_HEART_CATALYST,						// 261
	RESULT_SKILL_ARMS_MASTERY1,							// 262
	RESULT_SKILL_VIVID_MAGAZINE,						// 263
	RESULT_SKILL_TRIDENT,								// 264
	RESULT_SKILL_ARMS_MASTERY2,							// 265
	RESULT_SKILL_MOLE_SHOT,								// 266

	RESULT_SKILL_ETERNITY,								// 267
	RESULT_SKILL_PROTECTION_FROM_BLOOD,					// 268

	RESULT_SKILL_INSTALL_TRAP,							// 269
	RESULT_SKILL_CREATE_HOLY_POTION,					// 270
	RESULT_SKILL_MERCY_GROUND,							// 271
	RESULT_SKILL_HOLY_ARMOR,							// 272
	RESULT_SKILL_TRANSFORM_TO_WERWOLF,					// 273
	RESULT_SKILL_STONE_SKIN,							// 274

	RESULT_SKILL_ACID_ERUPTION,							// 275
	RESULT_SKILL_TALON_OF_CROW,							// 276
	RESULT_SKILL_GRAY_DARKNESS,							// 277	

	RESULT_SKILL_BITE_OF_DEATH,							// 278
	RESULT_SKILL_WIDE_GRAY_DARKNESS,					// 279
    RESULT_SKILL_TELEPORT,                             // 280

    RESULT_SKILL_FIRE_PIERCING,                        // 281
    RESULT_SKILL_SUMMON_OF_FIRE_ELEMENTAL,             // 282
    RESULT_SKILL_MAGNUM_SPEAR,                         // 283
    RESULT_SKILL_HELLFIRE,                             // 284

    RESULT_SKILL_ICE_LANCE,                            // 285
    RESULT_SKILL_FROZEN_ARMOR,                         // 286
    RESULT_SKILL_SUMMON_OF_WATER_ELEMENTAL,            // 287
    RESULT_SKILL_EXPLOSION_WATER,                      // 288
    RESULT_SKILL_SOUL_REBIRTH,                         // 289
    RESULT_SKILL_SOUL_REBIRTH_MASTERY,                 // 290

    RESULT_SKILL_REACTIVE_ARMOR,                       // 291
    RESULT_SKILL_GROUND_BLESS,                         // 292
    RESULT_SKILL_SUMMON_OF_GROUND_ELEMENTAL,           // 293
    RESULT_SKILL_METEOR_STORM,                         // 294

    RESULT_SKILL_SHARP_CHAKRAM,                        // 295
    RESULT_SKILL_SHIFT_BREAK,                          // 296
    RESULT_SKILL_WATER_SHIELD,                         // 297
    RESULT_SKILL_DESTRUCTION_SPEAR,                    // 298
    RESULT_SKILL_BLESS_FIRE,                           // 299
    RESULT_SKILL_FATAL_SNICK,                          // 300
    RESULT_SKILL_SAND_CROSS,                           // 301
    RESULT_SKILL_DUCKING_WALLOP,                       // 302
    RESULT_SKILL_CHARGING_ATTACK,                      // 303
    RESULT_SKILL_DISTANCE_BLITZ,                       // 304
	
	RESULT_SKILL_FABULOUS_SOUL,						//305
	RESULT_SKILL_WILL_OF_IRON,							//306
	// 질드레 스킬
    RESULT_SKILL_WIDE_ICE_FIELD,                       // 307
    RESULT_SKILL_GLACIER_1,                            // 308
    RESULT_SKILL_GLACIER_2,                            // 309
    RESULT_SKILL_ICE_AUGER,                            // 310
    RESULT_SKILL_ICE_HAIL,                             // 311
    RESULT_SKILL_WIDE_ICE_HAIL,                        // 312
    RESULT_SKILL_ICE_WAVE,                             // 313

	RESULT_SKILL_LAND_MINE_EXPLOSION,                  // 314
    RESULT_SKILL_CLAYMORE_EXPLOSION,                   // 315
    RESULT_SKILL_PLEASURE_EXPLOSION,                   // 316

	RESULT_SKILL_DELEO_EFFICIO,                        // 317  // DELETE EFFECT
    RESULT_SKILL_REPUTO_FACTUM,                        // 318

	RESULT_SKILL_SWORD_OF_THOR,                        // 319
    RESULT_SKILL_BURNING_SOUL_CHARGING,                // 320
    RESULT_SKILL_BURNING_SOUL_LAUNCH,                  // 321
    RESULT_SKILL_INSTALL_TURRET,                       // 322
    RESULT_SKILL_TURRET_FIRE,                          // 323
    RESULT_SKILL_SWEEP_VICE_1,                         // 324
    RESULT_SKILL_SWEEP_VICE_3,                         // 325
	RESULT_SKILL_SWEEP_VICE_5,                         // 326
    RESULT_SKILL_WHITSUNTIDE,                          // 327
    RESULT_SKILL_VIOLENT_PHANTOM,                      // 328
    RESULT_SKILL_SUMMON_GORE_GLAND,                    // 329
    RESULT_SKILL_GORE_GLAND_FIRE,                      // 330
    RESULT_SKILL_DESTRUCTION_SPEAR_MASTERY,            // 331
    RESULT_SKILL_FATAL_SNICK_MASTERY,                  // 332
	
	RESULT_SKILL_MAGNUM_SPEAR_MASTERY,                // 333
    RESULT_SKILL_ICE_LANCE_MASTERY,                  // 334
    RESULT_SKILL_REACTIVE_ARMOR_MASTERY,             // 335

	RESULT_SKILL_THROWING_AXE,                         // 336
    RESULT_SKILL_CHOPPING_FIREWOOD,                    // 337 장작패기	

	RESULT_SKILL_CHAIN_THROWING_AXE,                   // 338
    RESULT_SKILL_MULTI_THROWING_AXE,                   // 339
    RESULT_SKILL_PLAYING_WITH_FIRE,                    // 340 불장난

	RESULT_SKILL_INFINITY_THUNDERBOLT,                 // 341
    RESULT_SKILL_SPIT_STREAM,                          // 342
    RESULT_SKILL_PLASMA_ROCKET_LAUNCHER,               // 343
    RESULT_SKILL_INTIMATE_GRAIL,                       // 344
    RESULT_SKILL_BOMBING_STAR,                         // 345
    RESULT_SKILL_SET_AFIRE,                            // 346
    RESULT_SKILL_NOOSE_OF_WRAITH,                      // 347

	RESULT_SKILL_SHARP_HAIL,                           // 348
    RESULT_SKILL_SUMMON_MIGA,                          // 349  // 아우스터즈가 쓰는 스킬
    RESULT_SKILL_SUMMON_MIGA_ATTACK,                   // 350  // 소환된놈이 쓰는 스킬
    RESULT_SKILL_ICE_HORIZON,                          // 351
    RESULT_SKILL_FURY_OF_GNOME,                        // 352
	
	RESULT_SKILL_CANNONADE,								// 353  // 포격
    RESULT_SKILL_SELF_DESTRUCTION,						// 354  // 자폭공격

	RESULT_SKILL_AR_ATTACK,                            // 355  // 몬스터용
    RESULT_SKILL_SMG_ATTACK,                           // 356  // 몬스터용
    RESULT_SKILL_GRENADE_ATTACK,                       // 357  // 몬스터용

	RESULT_SKILL_DRAGON_TORNADO,                       // 358
    RESULT_SKILL_BIKE_CRASH,                           // 359
    RESULT_SKILL_HARPOON_BOMB,                         // 360
    RESULT_SKILL_PASSING_HEAL,                         // 361
    RESULT_SKILL_ROTTEN_APPLE,                         // 362
    RESULT_SKILL_WILD_WOLF,                            // 363
    RESULT_SKILL_ABERRATION,                           // 364
	RESULT_SKILL_HALO,                                 // 365
    RESULT_SKILL_DESTINIES,                            // 366
    RESULT_SKILL_FIERCE_FLAME,                         // 367
    RESULT_SKILL_SHADOW_OF_STORM,                      // 368

	RESULT_SKILL_HEAL_PASS,                            // 369 // 옮겨가는 힐

	RESULT_SKILL_TRASLA_ATTACK,                      // 370
    RESULT_SKILL_PUSCA_ATTACK,                       // 371
    RESULT_SKILL_NOD_COPILA_ATTACK,                  // 372
    RESULT_SKILL_NOD_COPILA_ATTACK_2,                    // 373
    RESULT_SKILL_UNTERFELDWEBEL_FIRE,                  // 374
    RESULT_SKILL_FELDWEBEL_FIRE,                       // 375
	RESULT_SKILL_FEEBLE_VIRUS,							//376

	RESULT_SKILL_SWORD_COMBO,                          // 377
    RESULT_SKILL_BLADE_COMBO,                          // 378
    RESULT_SKILL_VAMPIRE_COMBO,                        // 379
    RESULT_SKILL_OUSTERS_COMBO,                        // 380

    RESULT_SKILL_HOLY_SWORD,                           // 381
    RESULT_SKILL_HOLY_BLADE,                           // 382
    RESULT_SKILL_BURST_GUN,                            // 383
    RESULT_SKILL_HOLY_MACE,                            // 384
    RESULT_SKILL_HOLY_CROSS,                           // 385
    RESULT_SKILL_DARK_FORCE,                           // 386
    RESULT_SKILL_NATURAL_FORCE,                        // 387
    RESULT_SKILL_FIRE_FORCE,                           // 388
    RESULT_SKILL_WATER_FORCE,                          // 389
    RESULT_SKILL_EARTH_FORCE,                          // 390
    RESULT_SKILL_LOSE_SIGHT,                           // 391
    RESULT_SKILL_WIDE_BLOOD_DRAIN,                     // 392
    RESULT_SKILL_NATURAL_PEACE,                        // 393
    RESULT_SKILL_PARTY_AURA,                           // 394
	RESULT_SKILL_MULTI_DOOM,                           // 395
    RESULT_SKILL_MULTI_PARALYZE,                       // 396
    RESULT_SKILL_BRANDISH,                             // 397
    RESULT_SKILL_PARASITE,                             // 398
    RESULT_SKILL_SOUND_WAVE,                           // 399
	RESULT_SKILL_FIRE_WAVE,                            // 400
	RESULT_SKILL_TARGET_SUMMON_MONSTERS,			   // 401
	RESULT_SKILL_BLOODY_BURST,						   // 402
	RESULT_SKILL_TRACE_CREATURE,					   // 403
	RESULT_SKILL_SHINE_SWORD,						   // 404	
	RESULT_SKILL_BOMB_CRASH_WALK,					   // 405
	RESULT_SKILL_SATELLITE_BOMB,					   // 406
	RESULT_SKILL_ILLUSION_INVERSION,				   // 407
	RESULT_SKILL_HEAVEN_GROUND,						   // 408
	RESULT_SKILL_BLOODY_SCARIFY,                       // 409
    RESULT_SKILL_HETER_CHAKRAM,                        // 410
    RESULT_SKILL_DUMMY_DRAKE,                          // 411
    RESULT_SKILL_HYDRO_CONVERGENCE,                    // 412
    RESULT_SKILL_SUMMON_CLAY,                          // 413
    RESULT_SKILL_BLOOD_CURSE,                          // 414
    RESULT_SKILL_BLOODS_SYMPOSION_ATTACK,              // 415
	RESULT_SKILL_HETER_CHAKRAM_CHAIN,				   // 416
	RESULT_SKILL_TANNING,							   // 417
	RESULT_SKILL_REQUEST_RESURRECT,					   // 418

	RESULT_SKILL_MAGIC_SHIELD1,                        // 419
    RESULT_SKILL_MAGIC_SHIELD2,                        // 420

    RESULT_SKILL_EAGLES_EYE1,                          // 421
    RESULT_SKILL_EAGLES_EYE2,                          // 422

    RESULT_SKILL_FREEZE_RING1,                         // 423
    RESULT_SKILL_FREEZE_RING2,                         // 424

    RESULT_SKILL_BAT_STORM1,                           // 425
    RESULT_SKILL_BAT_STORM2,                           // 426

    RESULT_SKILL_CURSE_OF_BLOOD1,                      // 427
    RESULT_SKILL_CURSE_OF_BLOOD2,                      // 428

    RESULT_SKILL_BLOOD_DRAIN2,                         // 429

	RESULT_SKILL_BLOODY_SHOUT1,						   // 430
	RESULT_SKILL_BLOODY_SHOUT2,						   // 431

	RESULT_SKILL_BLOODY_WINGS,                         // 432

    RESULT_SKILL_MIST_OF_SOUL1,                        // 433
    RESULT_SKILL_MIST_OF_SOUL2,                        // 434

	RESULT_SKILL_ENERGY_BURST,							// 435
	
	RESULT_SKILL_MAGIC_AVOID1,							// 436
	RESULT_SKILL_MAGIC_AVOID2,							// 437
	RESULT_SKILL_MAGIC_AVOID3,							// 438

#if __CONTENTS(__MENEGROTH)
	
	RESULT_SKILL_CERBERUS,								//439
	RESULT_SKILL_MANTICORET,							//440
	RESULT_SKILL_BOGLE_H,								//441
	RESULT_SKILL_BOGLE_B,								//442
	RESULT_SKILL_MASSACRE_MENEGROTH,					//443
	RESULT_SKILL_PLUMYBOAR,								//444
	RESULT_SKILL_CABRACAM,								//445

#endif //__MENEGROTH
#if __CONTENTS(__SECOND_RARE_SKILL)
	RESULT_SKILL_MENEGROTH_TRAP1,						//446
	RESULT_SKILL_MENEGROTH_TRAP2,						//447
	RESULT_SKILL_HEAVENLY_SPARK,						//448
	RESULT_SKILL_BLADE_STORM,							//449
	RESULT_SKILL_XRL,									//450
	RESULT_SKILL_GENESIS,								//451
	RESULT_SKILL_THORNS,								//452
	RESULT_SKILL_LETHAL_CLAW,							//453
	RESULT_SKILL_VENOM_CIRCLE,							//454
	RESULT_SKILL_BRIONAC,								//455
	RESULT_SKILL_ASTEROID_BOMBER,						//456
	RESULT_SKILL_FROZEN_GLACIER,						//457
	RESULT_SKILL_INCINERATE,							//458
	// 레어스킬 패시브
	RESULT_SKILL_INCREASE_OF_HEALTH1,					//459
	RESULT_SKILL_INCREASE_OF_HEALTH2,					//460
	RESULT_SKILL_INCREASE_OF_HEALTH3,					//461
	RESULT_SKILL_INCREASE_OF_HEALTH4,					//462
	RESULT_SKILL_INCREASE_OF_HEALTH5,					//463
	RESULT_SKILL_INCREASE_OF_HEALTH6,					//464
#endif //__SECOND_RARE_SKILL

#if __CONTENTS(__TIPOJYU_CASTLE)	//티포주 몬스터 스킬 (서버와 번호가 맞아야 한다.)
	RESULT_SKILL_TIFFAUGES_BALBERITH_BLUE,				// 465  // 문지기 도끼
	RESULT_SKILL_TIFFAUGES_BALBERITH_RED,				// 466  
 
	RESULT_SKILL_TIFFAUGES_BERITH_BLUE,					// 467  // 근위대 창
	RESULT_SKILL_TIFFAUGES_BERITH_RED,					// 468  
 
	RESULT_SKILL_TIFFAUGES_VALKYRJA_BLUE,				// 469  // 근위대 대장
	RESULT_SKILL_TIFFAUGES_VALKYRJA_RED,				// 470  
 
	RESULT_SKILL_TIFFAUGES_GOMORY_A_BLUE,				// 471  // 여자 백작
	RESULT_SKILL_TIFFAUGES_GOMORY_B_BLUE,				// 472
	RESULT_SKILL_TIFFAUGES_GOMORY_A_RED,				// 473
	RESULT_SKILL_TIFFAUGES_GOMORY_B_RED,				// 474
 
	RESULT_SKILL_TIFFAUGES_KRSNIK_A_BLUE,				// 475  // 남자 백작
	RESULT_SKILL_TIFFAUGES_KRSNIK_B_BLUE,				// 476
	RESULT_SKILL_TIFFAUGES_KRSNIK_A_RED,				// 477
	RESULT_SKILL_TIFFAUGES_KRSNIK_B_RED,				// 478
 
	RESULT_SKILL_TIFFAUGES_GUSION_A_BLUE,				// 479  // 남자 백작 대장
	RESULT_SKILL_TIFFAUGES_GUSION_B_BLUE,				// 480
	RESULT_SKILL_TIFFAUGES_GUSION_C_BLUE,				// 481
	RESULT_SKILL_TIFFAUGES_GUSION_A_RED,				// 482
	RESULT_SKILL_TIFFAUGES_GUSION_B_RED,				// 483
	RESULT_SKILL_TIFFAUGES_GUSION_C_RED,				// 484
 
	RESULT_SKILL_TIFFAUGES_PAIMON_BLUE,					// 485  // 경비병
	RESULT_SKILL_TIFFAUGES_PAIMON_RED,					// 486
 
	RESULT_SKILL_TIFFAUGES_MORGOTH_A_BLUE,				// 487  // 웨버데스
	RESULT_SKILL_TIFFAUGES_MORGOTH_B_BLUE,				// 488
	RESULT_SKILL_TIFFAUGES_MORGOTH_C_BLUE,				// 489
	RESULT_SKILL_TIFFAUGES_MORGOTH_A_RED,				// 490
	RESULT_SKILL_TIFFAUGES_MORGOTH_B_RED,				// 491
	RESULT_SKILL_TIFFAUGES_MORGOTH_C_RED,				// 492
#endif // __TIPOJYU_CASTLE

#if __CONTENTS(__ONIBLA)

	RESULT_SKILL_ONIBLA_RAISE_BONE,						// 493 오니블라 
	RESULT_SKILL_ONIBLA_DROP_SKULL,						// 494 

	RESULT_SKILL_AGONY_CHILD_DROP_HEAD,					// 495 아고니 차일드
	RESULT_SKILL_AGONY_CHILD_SHADOW_FOUR,				// 496

	RESULT_SKILL_VILLAIN_LADY_GHOST_SHADOW,				// 497 // 빌라인 레이디
	RESULT_SKILL_VILLAIN_LADY_SELF_DESTRUCTION,			// 498 // 빌라인 레이디

	RESULT_SKILL_SPANIEL_THROWING_AXE,					// 499 // 오니 스패니얼
	RESULT_SKILL_BLACKASTRAL_MAGIC_CRYSTAL,				// 500 // 블랙 아스트랄
	RESULT_SKILL_WHITEASTRAL_SPELL_CRYSTAL,				// 501 // 화이트 크리스탈
#endif //__ONIBLA

#if __CONTENTS(__20091116_ADD_RARE_SKILL)
	RESULT_SKILL_BLADE_MAGMA_DETONATION_1,				// 502	- 도슬 레어스킬 1차 마그마 데토네이션
	RESULT_SKILL_BLADE_MAGMA_DETONATION_2,				// 503	- 도슬 레어스킬 2차 마그마 데토네이션
	RESULT_SKILL_OUSTERS_SQUALLY_BARRIER_1,				// 504	- 아우스터즈 레어스킬 1차 질풍의 베리어
	RESULT_SKILL_OUSTERS_SQUALLY_BARRIER_2,				// 505	- 아우스터즈 레어스킬 2차 질풍의 베리어
#endif //__20091116_ADD_RARE_SKILL
//rallser
	RESULT_SKILL_SIUS_CRASH,//繫법 506 1
    RESULT_SKILL_LYCAN_HOWL,//繫법 507 2
    RESULT_SKILL_BIRFRONZE_MAGIC,//繫법3 508
    RESULT_SKILL_PALUS_MIDDLE_RANGE_MAGIC,//4繫법509
    RESULT_SKILL_PALUS_LONG_RANGE_MAGIC,//5繫법510
    RESULT_SKILL_GARUM_MAGIC,//繫법511 6
    RESULT_SKILL_CLAVIE_MAGIC,//繫법512 7
    RESULT_SKILL_Wide_Wild_Wolf,//繫법513 8
    RESULT_SKILL_RALLSER_1,//繫법514 9
    RESULT_SKILL_SMILE_OF_DEATH,//10繫법515
    RESULT_SKILL_FURY_SHOCK,//繫11법516
    RESULT_SKILL_RAIL_GUN_ATTACK,//12繫법517
	//RESULT_NOT_1,
    RESULT_SKILL_BLAZE_WALK_2,//518繫법13
    RESULT_SKILL_GLADIATOR,//519繫법 14
    RESULT_SKILL_CRUSHING_STORM,//520繫법15
    RESULT_SKILL_SATELLITE_BOMB_2,//521繫법16
	RESULT_SKILL_INTIMATE_GRAIL_2,//522 17
	RESULT_SKILL_HOLY_ARMOR_2,//523 18
	RESULT_SKILL_STRIKING_2,//524 19
	RESULT_SKILL_BLESS_2,//525 20
	RESULT_SKILL_GLORY_GROUND,//526 21
	RESULT_SKILL_PASSING_HEAL_K2,//257 22
	RESULT_SKILL_GREAT_HEAL_2,//258 23
	RESULT_SKILL_CHAIN_OF_DEMON,//259 24
	RESULT_SKILL_BLOODY_SKULL,//260 25
	RESULT_SKILL_RAGE_OF_BLOOD,//261 26
	RESULT_SKILL_ALLYSTER_WIND,//262 27
	RESULT_SKILL_FLAME_SIGHT,//263 28
	RESULT_SKILL_SPIRAL_MEGALITH,//264 29
	RESULT_SKILL_RADCHIA,//265 30
	RESULT_SKILL_HEAL_PASS_2,//266 31
	RESULT_SKILL_BREATH_OF_DRYAD,//267 32
	RESULT_SKILL_CURE_EFFECT_2,//268 33
	RESULT_SKILL_LIVINGDEAD_B_ECHO_ESPER,//269 34
	RESULT_SKILL_MIHAI_BAT_STREAM,//270 35
	RESULT_SKILL_TSAR_BLUE_ROSE_DEVIL,//271 36
	RESULT_SKILL_FOLLET_POISON_FRAME,// 37
	RESULT_SKILL_FUE_FOLLET_RED_ROSE_DEVIL,//38
	RESULT_SKILL_DACULE_TALON_OF_DDRACULE,//39
	RESULT_SKILL_DRACULE_BLOOKDY_SCARIFY_2,//40
	RESULT_SKILL_DRACULE_BLOODY_CARPE,//41
	RESULT_SKILL_DRACULE_DRACULE_CASKET,//42
	RESULT_SKILL_DRAGON_HURRICANE,//43
	RESULT_SKILL_LAR_STROKE,//44
	RESULT_SKILL_GAE_BULGA,//45
	RESULT_SKILL_VICIOUS_GUIDANCE,//46
	RESULT_SKILL_SPECTOR_INVERSE,//47
	RESULT_SKILL_BAT_BREAKER,//48
    RESULT_SKILL_SHADY_DOUPLE,//49
	RESULT_SKILL_CHAKRAM_HAIL,//50
	RESULT_SKILL_FLAME_SPIKE,//51
	RESULT_SKILL_BLIZZARD,//52
	RESULT_SKILL_DEMOLISHER,//53
	RESULT_SKILL_RALLSER_2,//54
	RESULT_SKILL_RALLSER_3,//55rallser
	RESULT_SKILL_RALLSER_4,//56
	//RESULT_SKILL_RALLSER_44,//56
	//RESULT_SKILL_RALLSER_5,//56
	//RESULT_SKILL_RALLSER_6,//56
	//RESULT_SKILL_RALLSER_7,//56
	//RESULT_SKILL_RALLSER_8,//56
	RESULT_SKILL_CLIENT_SHADY_DOUPLE,//57
	RESULT_SKILL_RALLSER_55, //




	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	
	// 임시로 추가
	RESULT_MAGIC_BLOODY_TUNNEL_INTO,
//	RESULT_MAGIC_OPEN_CASKET,
	RESULT_SUMMON_HELICOPTER,		
	
	// Bomb
	RESULT_BOMB_SPLINTER,
	RESULT_BOMB_ACER,
	RESULT_BOMB_BULLS,	
	RESULT_BOMB_STUN,
	RESULT_BOMB_CROSSBOW,
	RESULT_BOMB_TWISTER,

	// Mine
	RESULT_MINE_ANKLE_KILLER,
	RESULT_MINE_POMZ,
	RESULT_MINE_AP_C1,
	RESULT_MINE_DIAMONDBACK,
	RESULT_MINE_SWIFT_EX,
	RESULT_MINE_SIDEWINDER,
	RESULT_MINE_COBRA,

	// Gun
	RESULT_SKILL_ATTACK_GUN_SR,			// 총1
	RESULT_SKILL_ATTACK_GUN_SG,			// 총2
	RESULT_SKILL_ATTACK_GUN_AR,			// 총3
	RESULT_SKILL_ATTACK_GUN_SMG,		// 총4
	
	// Sword
	RESULT_SKILL_ATTACK_SWORD,			// 칼
	RESULT_SKILL_ATTACK_BLADE,			// 도

	// 총에 따른 holy shooting
	RESULT_SKILL_HOLY_SHOOTING_TR,					//	Holy shooting
	RESULT_SKILL_HOLY_SHOOTING_SG,					//	Holy shooting
	RESULT_SKILL_HOLY_SHOOTING_AR,					//	Holy shooting
	RESULT_SKILL_HOLY_SHOOTING_SMG,				//	Holy shooting

	// etc
	RESULT_GUIDANCE_FIRE,
	RESULT_FALLING_LIGHTNING,
	RESULT_ATTACH_FIRE,
	RESULT_ATTACH_AURA,	
	RESULT_SKILL_MEGAPHONE,
	RESULT_SKILL_BLITZ_SLIDING_ATTACK,
	RESULT_SKILL_BLAZE_WALK_ATTACK,
	RESULT_SKILL_MUTE,
	RESULT_SKILL_ATTACK_MELEE_REDIANCE,
	RESULT_FAMILY_BONUS,
	
	//Combo Skill Result
	RESULT_SKILL_SWORD_COMBO_EFFECT,
	RESULT_SKILL_BLADE_COMBO_EFFECT,
	RESULT_SKILL_VAMPIRE_COMBO_EFFECT,
	RESULT_SKILL_OUSTERS_COMBO_EFFECT,

	// Dummy Skill Result - 이팩트를 보여주지 않고 시전 동작만 있는 Client Skill이다.
	RESULT_SKILL_DUMMY_ACTION_MAGIC,
	
	//-----------------------------------------------------------------------------
	// 스킬 단계
	//-----------------------------------------------------------------------------
	RESULT_STEP_SKILL_FLOURISH_2,							// 2단계 FLOURISH
	RESULT_STEP_SKILL_FLOURISH_3,							// 3단계 FLOURISH
	RESULT_STEP_SKILL_SHARP_ROUND_2,						// 2단계 Sharp Round
	RESULT_STEP_SKILL_SHARP_ROUND_3,						// 3단계 Sharp Round
	RESULT_STEP_SKILL_GAMMA_CHOP_2,
	RESULT_STEP_SKILL_GAMMA_CHOP_3,
	RESULT_STEP_SKILL_CROSS_GUARD_2,
	RESULT_STEP_SKILL_CROSS_GUARD_3,
	RESULT_STEP_SKILL_PROMINENCE_2,
	RESULT_STEP_SKILL_PROMINENCE_3,
	RESULT_STEP_SKILL_HANDS_OF_NIZE_2,
	RESULT_STEP_SKILL_HANDS_OF_NIZE_3,
	RESULT_STEP_SKILL_NYMPH_RECOVERY_2,
	RESULT_STEP_SKILL_NYMPH_RECOVERY_3,
	RESULT_STEP_SKILL_EARTHS_TEETH_2,
	RESULT_STEP_SKILL_EARTHS_TEETH_3,
	RESULT_STEP_SKILL_REFUSAL_ETHER_2,
	RESULT_STEP_SKILL_REFUSAL_ETHER_3,
	RESULT_STEP_SKILL_KASAS_ARROW_2,
	RESULT_STEP_SKILL_KASAS_ARROW_3,
	RESULT_STEP_SKILL_BLAZE_BOLT_2,
	RESULT_STEP_SKILL_BLAZE_BOLT_3,
	RESULT_STEP_SKILL_STONE_AUGER_2,
	RESULT_STEP_SKILL_STONE_AUGER_3,
	RESULT_STEP_SKILL_EMISSION_WATER_2,
	RESULT_STEP_SKILL_EMISSION_WATER_3,
	RESULT_STEP_SKILL_BEAT_HEAD_2,
	RESULT_STEP_SKILL_BEAT_HEAD_3,
	RESULT_STEP_SKILL_MAGNUM_SPEAR_2,
	RESULT_STEP_SKILL_MAGNUM_SPEAR_3,

	RESULT_STEP_SKILL_COMBO_ATTACK_1,	// 콤보기술 1단계 
	RESULT_STEP_SKILL_COMBO_ATTACK_2,	// 콤보기술 2단계
	RESULT_STEP_SKILL_COMBO_ATTACK_3,	// 콤보기술 3단계
	RESULT_STEP_SKILL_COMBO_ATTACK_4,	// 콤보기술 4단계
	//-----------------------------------------------------------------------------

	// client 전용
	RESULT_SLAYER_DIE,
	RESULT_VAMPIRE_DIE,
	RESULT_OUSTERS_DIE,
	RESULT_MASTER_DIE,
	RESULT_VAMPIRE_DIE_NO_EFFECT,
	RESULT_FAKE_DIE,
	RESULT_GROUND_ELEMENTAL_DIE,

	// casting ActionInfo
	CASTING_ENCHANT,					// enchant domain casting
	CASTING_HEALING,					// healing domain casting
	CASTING_FIRE,
	CASTING_GROUND,
	CASTING_WATER,
	CASTING_FIRE2,
	CASTING_GROUND2,
	CASTING_WATER2,

	// 방어막 효과를 내기 위해서..
	REFLECT_AURA_PRISM,				//	
	REFLECT_AURA_SHIELD,				//	
	PROTECTION_FROM_ACID,
	REFLECT_REFLECTION,				//	SKILL_REFLECTION에 의한것

	// 바닥에 흘리는 피
	BLOOD_DROP_GROUND,
	BLOOD_CRITICAL_HIT,	
	BLOOD_RESURRECT,
	
	// 아마게돈 2002.3.4
	ARMAGEDDON_CRASH_1,			// 첫번째 부서지는거
	ARMAGEDDON_CRASH_2,			// 두번째
	ARMAGEDDON_CRASH_3,			// 세번째
	ARMAGEDDON_CRASH_4,			// 마지막 남기
	
	// bloody_wall_single
	RESULT_MAGIC_BLOODY_WALL_SINGLE,

	// 2002.12.09
    SKILL_FIRE_CRACKER_1,
    SKILL_FIRE_CRACKER_2,
    SKILL_FIRE_CRACKER_3,

    RESULT_SKILL_SHARP_SHIELD_1,
    RESULT_SKILL_SHARP_SHIELD_2,
    RESULT_SKILL_SHARP_SHIELD_3,
    RESULT_SKILL_SHARP_SHIELD_4,

	RESULT_SKILL_AIR_SHIELD_1,
	RESULT_SKILL_AIR_SHIELD_2,
	RESULT_SKILL_AIR_SHIELD_3,
	RESULT_SKILL_AIR_SHIELD_4,

	RESULT_SKILL_SPIRIT_GUARD_1,
	RESULT_SKILL_SPIRIT_GUARD_2,
	RESULT_SKILL_SPIRIT_GUARD_3,
	RESULT_SKILL_SPIRIT_GUARD_4,
	
    RESULT_SKILL_GUN_SHOT_GUIDANCE_BOMB,
    RESULT_SKILL_GUN_SHOT_GUIDANCE_POINT,

	SHRINE_SHIELD,
	DROP_BLOOD_BIBLE,

	ARMEGA_TILE,					// 아르메가
	MIHOLE_TILE,					// 미호레
	KIRO_TILE,						// 키로
	INI_TILE,						// 아이니
	GREGORI_TILE,					// 그리고리 이펙트(타일에 붙는것)
	CONCILIA_TILE,					// 콘칠리아
	LEGIOS_TILE,					// 레지오스
	HILLEL_TILE,					// 힐릴
	JAVE_TILE,						// 쟈브
	NEMA_TILE,						// 네마
	AROSA_TILE,						// 아로사
	CHASPA_TILE,					// 차스파

	HOLY_LAND_WARP_VAMPIRE,
	HOLY_LAND_WARP_SLAYER,

	WARP_BLOOD_BIBLE_TILE,

	AMBER_OF_GUARD_TILE,
	AMBER_OF_AVENGER_TILE,
	AMBER_OF_IMMORTAL_TILE,
	AMBER_OF_CURSE_TILE,

	SKILL_DENIAL_MAGIC_DAMAGED,
	SKILL_DRAGON_FIRE_CRACKER,

	SKILL_FIRE_CRACKER_4,			// 추가
	
	SKILL_FIRE_CRACKER_VOLLEY_1,		// 3연발 폭죽-_-
	SKILL_FIRE_CRACKER_VOLLEY_2,		// 3연발 폭죽-_-
	SKILL_FIRE_CRACKER_VOLLEY_3,		// 3연발 폭죽-_-
	SKILL_FIRE_CRACKER_VOLLEY_4,		// 3연발 폭죽-_-
	
	SKILL_FIRE_CRACKER_WIDE_VOLLEY_1,	// 3연발 광역 폭죽
	SKILL_FIRE_CRACKER_WIDE_VOLLEY_2,	// 3연발 광역 폭죽
	SKILL_FIRE_CRACKER_WIDE_VOLLEY_3,	// 3연발 광역 폭죽
	SKILL_FIRE_CRACKER_WIDE_VOLLEY_4,	// 3연발 광역 폭죽

	SKILL_FIRE_CRACKER_STORM,
	SAND_OF_SOUL_STONE,
	ICE_OF_SOUL_STONE,
	BACK_STAB_GRADE_3,

	BLESS_FIRE,
	WATER_SHIELD,
	SAND_CROSS,
	
	HOLY_LAND_WARP_OUSTERS,

	ETERNITY,
	TRAP_INSTALLED,
	TRAP_TRIGGERED,
	TRAPPED,
	
	REGEN_TOWER_TRING_POSITION,			// 리젠존 타워 쟁탈 타일
	REGEN_TOWER_THROW_LIGHT,			// 리젠존 빛 비치는거 
	REGEN_TOWER_TRING_POSITION_LIGHT,	// 리젠존 타워 쟁탈 타일에 올랐을때
	REGEN_TOWER_FLASH_BEAD,				// 리젠존 타워 구슬 반짝거리는거.

	SKILL_CLIENT_TELEPORT,
	SKILL_CLIENT_TELEPORT2,
	SKILL_CLIENT_DUCKING_WALLOP,
	SKILL_CLIENT_CHARGING_ATTACK,

	SKILL_CLIENT_SUMMON_GROUND_ELEMENTAL,
	SKILL_CLIENT_FIRE_ELEMENTAL_ATTACK,
	SKILL_CLIENT_WATER_ELEMENTAL_HEAL,
	
	SKILL_CLIENT_TANK_SMOKE,
	SKILL_CLIENT_TANK_ATTACK_3,
	SKILL_CLIENT_TANK_ATTACK_4,
	SKILL_CLIENT_TANK_ATTACK_5,
	SKILL_CLIENT_TANK_ATTACKED,


	// 2004,3,6 sobeit add start - 펫 스킬
	SKILL_CLIENT_STIRGE_BLOOD_DRAIN_1,
	SKILL_CLIENT_STIRGE_BLOOD_DRAIN_2,
	SKILL_CLIENT_STIRGE_BLOOD_DRAIN_3,
	SKILL_CLIENT_STIRGE_BLOOD_DRAIN_4,
	SKILL_CLIENT_STIRGE_BLOOD_DRAIN_5,

	SKILL_CLIENT_PIXIE_ABSORB_SOUL_1,
	SKILL_CLIENT_PIXIE_ABSORB_SOUL_2,
	SKILL_CLIENT_PIXIE_ABSORB_SOUL_3,
	SKILL_CLIENT_PIXIE_ABSORB_SOUL_4,
	SKILL_CLIENT_PIXIE_ABSORB_SOUL_5,
	// 2004,3,6 sobeit add end - 펫 스킬

	// 2004,3,22 sobeit add start - 검은 기운
	MAP_BLACK_LARGE_SMOKE,
	MAP_BLACK_HALL,
	MAP_BLACK_SMALL_SMOKE_1,
	MAP_BLACK_SMALL_SMOKE_2,
	MAP_BLACK_SMALL_SMOKE_3,

	MAP_ELECTRIC_POST,
	MAP_GHOST_1,
	MAP_GHOST_2,
	// 2004,3,22 sobeit add end - 검은 기운
	
	// 2004, 3, 30 sobeit add start - 울버린 나올때 이펙트
	RESULT_SUMMON_ULBERINE,
	// 2004, 3, 30 sobeit add end - 울버린 나올때 이펙트

	// 2004, 4,2  sobeit add start - 행운의 오로라(네잎 클로바)
	RESULT_LUCKY_AURA,
	RESULT_MISFORTUNE_AURA,
	// 2004, 4,2 sobeit add end - 행운의 오로라(네잎 클로바)

	// 2004, 5, 6, sobeit add start
	AMBER_OF_LIFE_TILE,
	AMBER_OF_LIGHT_TILE,
	// 2004, 5, 6, sobeit add end

	// 2004, 5, 18 sobeit add start
	SKILL_CLIENT_TRAP_ICICLE_DROP,					// 떨어지는 얼음 트랩
	SKILL_CLIENT_TRAP_ICICLE_DROP_SIGN,

	SKILL_CLIENT_TRAP_ICICLE_AUGER,
	SKILL_CLIENT_TRAP_ICICLE_AUGER_SIGN,

	SKILL_CLIENT_TRAP_ICE_STICK_SW,		// 좌하
	SKILL_CLIENT_TRAP_ICE_STICK_SW_SIGN,// 좌하
	SKILL_CLIENT_TRAP_ICE_STICK_SE,		// 우하
	SKILL_CLIENT_TRAP_ICE_STICK_SE_SIGN,// 우하
	SKILL_CLIENT_TRAP_ICE_STICK_NE,		// 우상
	SKILL_CLIENT_TRAP_ICE_STICK_NE_SIGN,// 우상
	SKILL_CLIENT_TRAP_ICE_STICK_NW,		// 좌상
	SKILL_CLIENT_TRAP_ICE_STICK_NW_SIGN,// 좌상

	SKILL_CLIENT_TRAP_ICICLE_LARGE_AUGER,
	SKILL_CLIENT_TRAP_ICICLE_LARGE_AUGER_SIGN,
	// 2004, 5, 18 sobeit add start
	

	// 2004, 6, 8 sobeit add start - 질드레 주변 이펙트
//	SKILL_CLIENT_SUMMON_GDR, 
//    SKILL_CLIENT_GDR_SATELLITE,
//    SKILL_CLIENT_GDR_FLOATING, 
//
//	SKILL_CLIENT_SLAYER_MASTER_130,
//	SKILL_CLIENT_SLAYER_MASTER_150,
//	SKILL_CLIENT_VAMPIRE_MASTER_130,
//	SKILL_CLIENT_VAMPIRE_MASTER_150, // 150 레벨 마스터 이펙트
//	SKILL_CLIENT_OUSTERS_MASTER_130,
//	SKILL_CLIENT_OUSTERS_MASTER_150,
	// 2004, 6, 8 sobeit add end

	// 2004, 6, 15 sobeit add start - gdr skill - glacier
	SKILL_CLIENT_GDR_GLACIER_SLAYER,
	SKILL_CLIENT_GDR_GLACIER_VAMPIRE,
	SKILL_CLIENT_GDR_GLACIER_OUSTERS,
	// 2004, 6, 15 sobeit add end - gdr skill - glacier

	// 2004, 6, 26 sobeit add start
	MAP_GDR_LAIR_POTAL,	// 질드레 포탈 이펙트
	// 2004, 6, 26 sobeit add end

	// 2004, 8, 5 sobeit add start
	SKILL_CLIENT_CASTLE_GATE_DEAD, // 성문 부서질때(죽을때) 큰 먼지이펙트
	SKILL_CLIENT_CASTLE_GATE_DAMAGED,// 성문 타격 받을 때
	SKILL_CLIENT_GUILD_TOWER_DEAD, // 길드타워 부서질때(죽을때) 큰 먼지이펙트
	SKILL_CLIENT_GUILD_TOWER_PROTECT, // 길드타워 보호 이펙트
	// 2004, 8, 5 sobeit add end

	// 2004, 8, 31, sobeit add start
	SKILL_CLIENT_LAND_MINE_LIGHT,		// 랜드마인에 계속 붙어 있는 이펙트
	SKILL_CLIENT_PLEASUER_STATION_LIGHT,// 프레져 스테이션에 계속 붙어 있는 이펙트
	// 2004, 8, 31, sobeit add end

	// 2004, 9, 2, sobeit add start
	SKILL_CLIENT_TURRET_LASER,					// 터렛 레이저 1타일
	SKILL_CLIENT_TURRET_LASER_ATTACK,           // 터렛 레이저 공격 1타일 (공격측에도 보임) 
	// 2004, 9, 2, sobeit add end

	// 2004, 9, 6, sobeit add start - 질드레 죽기 이펙트 , 리푸토 펙텀 타격
	SKILL_CLIENT_GDR_DEAD,
	SKILL_CLIENT_HIT_REPUTO_FACTUM,
	// 2004, 9, 6, sobeit add end - 질드레 죽기 이펙트

	SKILL_CLIENT_SWEEP_VICE,// 인챈 스윕 바이스 1단계
	SKILL_CLIENT_SWORD_OF_THOR, // 소드 오브 토르 지속 되는거

	SKILL_CLIENT_BURNING_SOL_1, // 도슬 버닝솔 1단계
	SKILL_CLIENT_BURNING_SOL_2, // 도슬 버닝솔 2단계
	SKILL_CLIENT_BURNING_SOL_3, // 도슬 버닝솔 3단계
	SKILL_CLIENT_BURNING_SOL_4, // 도슬 버닝솔 4단계
	RESULT_CLIENT_BURNING_SOL_1, // 도슬 버닝솔 1단계
	RESULT_CLIENT_BURNING_SOL_2, // 도슬 버닝솔 2단계
	RESULT_CLIENT_BURNING_SOL_3, // 도슬 버닝솔 3단계
	RESULT_CLIENT_BURNING_SOL_4, // 도슬 버닝솔 4단계
	
	// 2004, 10, 21, sobeit add start - 그레이트 러피언 관련
	SKILL_CLIENT_GREAT_RUFFIAN_ALIVE, // 2차 생성될때 이펙트
	SKILL_CLIENT_GREAT_RUFFIAN_BODY, // 몸에 붙는 이펙트
	SKILL_CLIENT_GREAT_RUFFIAN_DIE, // 죽을때 나오는 이펙트
	// 2004, 10, 21, sobeit add end

	// 2004, 11, 11, sobeit add start - 총 140 터지는거
	SKILL_CLIENT_PLASMA_ROCKET_LAUNCHER_BLOW,
	// 2004, 11, 11, sobeit add end - 

	// 2004, 11, 18, sobeit add start
	SKILL_CLIENT_FURY_OF_GNOME,
	// 2004, 11, 18, sobeit add end

	// 2004, 11, 22, sobeit add start
	SKILL_CLIENT_BLOOD_BIBLE_IN_GEAR_WINDOW,
	// 2004, 11, 22, sobeit add end
	
	// 2004, 12, 30, sobeit add start
	SKILL_CLIENT_HALO_ATTACK,
	SKILL_CLIENT_HALO_STOP,
	SKILL_CLIENT_FIERCE_FLAME,
	SKILL_CLIENT_BIKE_CRASH,
	SKILL_CLIENT_ROTTEN_APPLE,
	// 2004, 12, 30, sobeit add end

	SKILL_CLIENT_CANDLE, // 촛불 이펙트
	
	SKILL_CLIENT_ADVANCEMENT_SUMMON_SYLPH_START,
	SKILL_CLIENT_ADVANCEMENT_SUMMON_SYLPH_END,

	SKILL_CLIENT_QUEST_MAGIC_ZONE,
	SKILL_CLIENT_QUEST_MAGIC_ZONE_POTAL,

	// 2004, 10, 15, sobeit add start - 테스트용
	TEMP_SKILL_INFINITY_THUNDERBLOT, // 검슬 140
	TEMP_SKILL_SPIT_STREAM, // 도슬 140
	TEMP_SKILL_PLASMA_ROCKET_LAUNCHER, //총 140
	TEMP_SKILL_INTIMATE_GRAIL, // 인첸 140
	TEMP_SKILL_BOMBING_STAR, // 힐 140,
	TEMP_SKILL_SET_AFIRE, // 전투뱀 140
	TEMP_SKILL_NOOSE_OF_WRAITH, // 마법뱀 140

	RESULT_TEMP_SKILL_INFINITY_THUNDERBLOT, // 검슬 140
	RESULT_TEMP_SKILL_SPIT_STREAM, // 도슬 140
	RESULT_TEMP_SKILL_PLASMA_ROCKET_LAUNCHER, //총 140
	RESULT_TEMP_SKILL_INTIMATE_GRAIL, // 인첸 140
	RESULT_TEMP_SKILL_BOMBING_STAR, // 힐 140,
	RESULT_TEMP_SKILL_SET_AFIRE, // 전투뱀 140
	RESULT_TEMP_SKILL_NOOSE_OF_WRAITH, // 마법뱀 140
	// 2004, 10, 15, sobeit add end - 테스트용
	
	//Add Begin - 2006.10.24 by chyaya
	SKILL_CLIENT_SHINE_SWORD, 
	SKILL_CLIENT_BOMB_CRASH_WALK,
	SKILL_CLIENT_BLOODY_SCARIFY_BOMB,
	SKILL_CLIENT_DUMMY_DRAKE_BOMB,
	SKILL_CLIENT_HYDRO_CONVERGENCE_BOMB,
	SKILL_CLIENT_SUMMON_CLAY,
	SKILL_CLIENT_SATELLITE_BOMB_POINT,
	SKILL_CLIENT_SATELLITE_BOMB_FIRE,
	SKILL_CLIENT_HETER_CHAKRAM_BOMB,
	//Add End

	SKILL_CLIENT_REQUEST_RESURRECT_WAITING,
	//아우스터즈 승직 이동 수단 이펙트 스테이터스에는 wisp으로 검색 sprite는 ousters_fastmove와 SUMMON_WISP임
	//by diesirace 20070312
	SKILL_CLIENT_ADVANCEMENT_SUMMON_SYLPH_RED_START, 
	SKILL_CLIENT_ADVANCEMENT_SUMMON_SYLPH_RED_END,
	SKILL_CLIENT_ADVANCEMENT_SUMMON_SYLPH_YELLOW_START, 
	SKILL_CLIENT_ADVANCEMENT_SUMMON_SYLPH_YELLOW_END,
	SKILL_CLIENT_ADVANCEMENT_SUMMON_SYLPH_GREEN_START, 
	SKILL_CLIENT_ADVANCEMENT_SUMMON_SYLPH_GREEN_END,
	// end
	
	//Add Begin - 2007.07.30 by chyaya
	RESULT_SUMMON_LUPENG,	// 괴도 루펭 소환
	//Add End
	
	//Add Begin - 2007.10.02 by chyaya
	SKILL_CLIENT_CURSE_OF_BLOOD,
	SKILL_CLIENT_FREEZE,
	//Add End
#if __CONTENTS(__MENEGROTH)
	SKILL_CLIENT_CABRACAM_SUMMON,
	SKILL_CLIENT_MENEGROTH_TRAP,
#endif //__MENEGROTH
#if __CONTENTS(__SECOND_RARE_SKILL)
	SKILL_CLIENT_XRL_BLOW,
	SKILL_CLIENT_VENOM_CIRCLE,
#endif //__SECOND_RARE_SKILL
#if __CONTENTS(__FAST_TRANSFORTER)
	SKILL_CLIENT_FLITTERMOUSE,
	SKILL_CLIENT_OUSTERS_WING,
	SKILL_CLIENT_SLAYER_WING,
#endif //__FAST_TRANSFORTER

#if __CONTENTS(__TIPOJYU_CASTLE)
	SKILL_CLIENT_TIPOJYU_LIFE_GATEWAY_NE,
	SKILL_CLIENT_TIPOJYU_LIFE_GATEWAY_NW,
	SKILL_CLIENT_TIPOJYU_HADES_GATEWAY_NE,
	SKILL_CLIENT_TIPOJYU_HADES_GATEWAY_NW,

	SKILL_CLIENT_TIPOJYU_LIFE_TRAP,
	SKILL_CLIENT_TIPOJYU_HADES_TRAP,
#endif // __TIPOJYU_CASTLE

#if __CONTENTS(__SECOND_TRANSFORTER)
	SKILL_CLIENT_SHAPE_OF_DEMON,
	SKILL_CLIENT_OUSTERS_UNICORN,		// 미정
	SKILL_CLIENT_SLAYER_HOVER_VEHICLE,	// 미정
#endif //__SECOND_TRANSFORTER
#if __CONTENTS(__2008_OCTOBER_MAKETITEM)
	SKILL_CLIENT_BEARBOX_SUMMON,
	SKILL_CLIENT_SUCCUBUS_SUMMON,
	SKILL_CLIENT_FIREELEMENTAL_SUMMON,
#endif //__2008_OCTOBER_MAKETITEM

#if __CONTENTS(__NEW_PET_INCUBUS)
	SKILL_CLIENT_INCUBUS_ATTACK,
#endif //__NEW_PET_INCUBUS

#if __CONTENTS(__ONIBLA)
	SKILL_CLIENT_ONIBLA_BARRIER_RIGHT_UP,
	SKILL_CLIENT_ONIBLA_BARRIER_LEFT_UP,
	SKILL_CLIENT_ONIBLA_RAISE_BONE,
	
	SKILL_CLIENT_ONIBLA_BLOCK_DOOR_LEFT_UP,
	SKILL_CLIENT_ONIBLA_BLOCK_DOOR_LEFT_UP_OPEN,
	SKILL_CLIENT_ONIBLA_BLOCK_DOOR_RIGHT_UP,
	SKILL_CLIENT_ONIBLA_BLOCK_DOOR_RIGHT_UP_OPEN,

	SKILL_CLIENT_ONIBLA_EXIT_RIGHT_UP,
	SKILL_CLIENT_ONIBLA_EXIT_LEFT_UP,
#endif //__ONIBLA
#if __CONTENTS(__PET_MASHIMARO)
	SKILL_CLIENT_MASHIMARO_SUMMON,
#endif //__PET_MASHIMARO
#if __CONTENTS(__PET_CATS) || __CONTENTS(__PET_CATS_UPGRADED)
	SKILL_CLIENT_CATS_SUMMON,
#endif //__PET_CATS || __PET_CATS_UPGRADED
//#if __CONTENTS(__PET_CATS_UPGRADED)
    SKILL_CLIENT_RUFFIAN_SUMMON, //rallser
//#endif //__PET_CATS_UPGRADED
    RESULT_SKILL_CHARGING_POWER2,
    SKILL_CLIENT_BOOST,
	// max
	MAX_ACTIONINFO
};


// MAX_ACTIONINFO
#define		MIN_RESULT_ACTIONINFO		RESULT_SKILL_ATTACK_MELEE
#define		MAX_RESULT_ACTIONINFO		RESULT_SKILL_ATTACK_BLADE
#define		MIN_RESULT_STEP_ACTIONINFO	STEP_SKILL_FLOURISH_2

#endif
