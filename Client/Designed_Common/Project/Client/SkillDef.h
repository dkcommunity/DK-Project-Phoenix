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
	SKILL_STEP_NULL,			// °ü°è¾ø´Â °Íµé

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
	SKILL_STEP_OUSTERS_COMBAT,//Õ½Ä§¼¼ÄÜÀà
	SKILL_STEP_OUSTERS_ELEMENTAL,//·¨Ê¦Ê¯Í·Àà
	SKILL_STEP_OUSTERS_ETC,//Ä§Áé ÕÙ»½Àà
	//SKILL_STEP_OUSTERS_COMMON_ADVANCEMENT,   by rallser  È¡Ïû
	SKILL_STEP_OUSTERS_COMBAT_ADVANCEMENT,//Õ½¶·×ªÖ°Àà
	SKILL_STEP_OUSTERS_EARTH_ADVANCEMENT,//ÍÁÄ§×ªÖ°Àà
	SKILL_STEP_OUSTERS_FIRE_ADVANCEMENT,//»ðÄ§×ªÖ°Àà
	SKILL_STEP_OUSTERS_WATER_ADVANCEMENT,//Ë®Ä§×ªÖ°Àà

	SKILL_STEP_ETC,

	// by chyaya
	SKILL_STEP_SLAYER_RARE,
	SKILL_STEP_VAMPIRE_RARE,
	SKILL_STEP_OUSTERS_RARE,
	SKILL_STEP_COMMON_RARE,
	//by rallser
	SKILL_STEP_SLAYER_201_NEW,
	SKILL_STEP_VANPIRE_201_NEW,
	SKILL_STEP_OUSTERS_201_NEW,
};

//----------------------------------------------------------------------
// SkillID == ActionInfo Á¤ÀÇ
//----------------------------------------------------------------------
enum ACTIONINFO
{
		// °Ë°è¿­,
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

	// µµ °è¿­,
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

	// °Ç, ¶óÀÌÇÃ °è¿­
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
    
	// ¼ºÁ÷¸¶¹ý,
	// ÀÎÃ¾ °è¿­
	MAGIC_CREATE_HOLY_WATER, 			 		// 39
	MAGIC_LIGHT,				      			// 40
	MAGIC_DETECT_HIDDEN,	   	 				// 41
	MAGIC_AURA_BALL,	     		 		 	// 55
	MAGIC_BLESS,			       		 		// 45
	MAGIC_CONTINUAL_LIGHT,	 			 		// 46
	MAGIC_FLARE,								// ¾ó¸¶ÀÏ±î ´Ù½Ã Á¤¸® ÇØ¾ßÁö
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

	// Èú °è¿­
	MAGIC_CURE_LIGHT_WOUNDS,					// 52
	MAGIC_CURE_POISON,							// 54
	MAGIC_PROTECTION_FROM_POISON,				// 53
	MAGIC_CAUSE_LIGHT_WOUNDS,					// ¾ó¸¶ÀÏ±î ´Ù½Ã Á¤¸® ÇØ¾ßÁö
	MAGIC_CURE_SERIOUS_WOUNDS,					// 56
	MAGIC_REMOVE_CURSE,							// 89
	MAGIC_PROTECTION_FROM_CURSE,				// 59
	MAGIC_CAUSE_SERIOUS_WOUNDS,					// ¾ó¸¶ÀÏ±î ´Ù½Ã Á¤¸® ÇØ¾ßÁö
	MAGIC_CURE_CRITICAL_WOUNDS,					// 
	MAGIC_PROTECTION_FROM_ACID,					//
	MAGIC_SACRIFICE,							//
	MAGIC_CAUSE_CRITICAL_WOUNDS,				// ¾ó¸¶ÀÏ±î ´Ù½Ã Á¤¸® ÇØ¾ßÁö
	MAGIC_CURE_ALL,								// 62
	MAGIC_REGENERATION,							// 61
	MAGIC_MASS_CURE,							//
	MAGIC_MASS_HEAL,							//

	// ¹ìÇÁ±â¼ú,
	SKILL_BLOOD_DRAIN,							// 65

	// ¹ìÇÁ¸¶¹ý,
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

	// ½á¸ó
	MAGIC_SUMMON_WOLF,							// 87
	MAGIC_SUMMON_CASKET,						// 88
	MAGIC_RAISING_DEAD,							// 88
	MAGIC_SUMMON_SERVANT,						//

	// ETC
	MAGIC_UN_BURROW,							// 89
	MAGIC_UN_TRANSFORM,							// 89
	MAGIC_UN_INVISIBILITY,
	MAGIC_THROW_HOLY_WATER,						// 91

	// ´Á´ë ±â¼ú
	MAGIC_EAT_CORPSE,
	MAGIC_HOWL,

	// NPC ±â¼ú
	MAGIC_RESTORE,								// 90

	MAGIC_BLOODY_MARK,
	MAGIC_BLOODY_TUNNEL,

	MAGIC_SEDUCTION,
	SKILL_WIND_DIVIDER,
	SKILL_EARTHQUAKE,

	MAGIC_RESURRECT,
	MAGIC_PRAYER,
	MAGIC_MEDITATION,

	// 2001.12.4 Ãß°¡
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
	SKILL_CRITICAL_GROUND,		// criticalÀ» »ç¿ëÇÏ´Â °æ¿ì	
	
	SKILL_VIGOR_DROP,
	
	// [»õ±â¼ú]
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

 // Áúµå·¹ ½ºÅ³
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
    SKILL_CHOPPING_FIREWOOD,                    // 337 ÀåÀÛÆÐ±â	

	SKILL_CHAIN_THROWING_AXE,                   // 338
    SKILL_MULTI_THROWING_AXE,                   // 339
    SKILL_PLAYING_WITH_FIRE,                    // 340 ºÒÀå³­

	SKILL_INFINITY_THUNDERBOLT,                 // 341
    SKILL_SPIT_STREAM,                          // 342
    SKILL_PLASMA_ROCKET_LAUNCHER,               // 343
    SKILL_INTIMATE_GRAIL,                       // 344
    SKILL_BOMBING_STAR,                         // 345
    SKILL_SET_AFIRE,                            // 346
    SKILL_NOOSE_OF_WRAITH,                      // 347

    SKILL_SHARP_HAIL,                           // 348
    SKILL_SUMMON_MIGA,                          // 349  // ¾Æ¿ì½ºÅÍÁî°¡ ¾²´Â ½ºÅ³
    SKILL_SUMMON_MIGA_ATTACK,                   // 350  // ¼ÒÈ¯µÈ³ðÀÌ ¾²´Â ½ºÅ³
    SKILL_ICE_HORIZON,                          // 351
    SKILL_FURY_OF_GNOME,                        // 352
	
	SKILL_CANNONADE,                          // 353  // Æ÷°Ý
    SKILL_SELF_DESTRUCTION,                   // 354  // ÀÚÆø°ø°Ý

	SKILL_AR_ATTACK,                            // 355  // ¸ó½ºÅÍ¿ë
    SKILL_SMG_ATTACK,                           // 356  // ¸ó½ºÅÍ¿ë
    SKILL_GRENADE_ATTACK,                       // 357  // ¸ó½ºÅÍ¿ë

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
	
	SKILL_HEAL_PASS,                            // 369 // ¿Å°Ü°¡´Â Èú

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
	
	// ÀÏÀÚÄÚÈå ÇÁ·Ñ·Î±×
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
	//ÀÌ·¸°Ô ÇØ¾ß ¾È²¿ÀÎ´Ù.
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
	// ·¹¾î½ºÅ³ ÆÐ½Ãºê
	SKILL_INCREASE_OF_HEALTH1,					//459
	SKILL_INCREASE_OF_HEALTH2,					//460
	SKILL_INCREASE_OF_HEALTH3,					//461
	SKILL_INCREASE_OF_HEALTH4,					//462
	SKILL_INCREASE_OF_HEALTH5,					//463
	SKILL_INCREASE_OF_HEALTH6,					//464
#endif //__SECOND_RARE_SKILL

#if __CONTENTS(__TIPOJYU_CASTLE)	//Æ¼Æ÷ÁÖ ¸ó½ºÅÍ ½ºÅ³ (¼­¹ö¿Í ¹øÈ£°¡ ¸Â¾Æ¾ß ÇÑ´Ù.)
	SKILL_TIFFAUGES_BALBERITH_BLUE = 465,  // 465  // ¹®Áö±â µµ³¢
	SKILL_TIFFAUGES_BALBERITH_RED,			// 466  
 
	SKILL_TIFFAUGES_BERITH_BLUE,			// 467  // ±ÙÀ§´ë Ã¢
	SKILL_TIFFAUGES_BERITH_RED,				// 468  
 
	SKILL_TIFFAUGES_VALKYRJA_BLUE,			// 469  // ±ÙÀ§´ë ´ëÀå
	SKILL_TIFFAUGES_VALKYRJA_RED,			// 470  
 
	SKILL_TIFFAUGES_GOMORY_A_BLUE,			// 471  // ¿©ÀÚ ¹éÀÛ
	SKILL_TIFFAUGES_GOMORY_B_BLUE,			// 472
	SKILL_TIFFAUGES_GOMORY_A_RED,			// 473
	SKILL_TIFFAUGES_GOMORY_B_RED,			// 474
 
	SKILL_TIFFAUGES_KRSNIK_A_BLUE,			// 475  // ³²ÀÚ ¹éÀÛ
	SKILL_TIFFAUGES_KRSNIK_B_BLUE,			// 476
	SKILL_TIFFAUGES_KRSNIK_A_RED,			// 477
	SKILL_TIFFAUGES_KRSNIK_B_RED,			// 478
 
	SKILL_TIFFAUGES_GUSION_A_BLUE,			// 479  // ³²ÀÚ ¹éÀÛ ´ëÀå
	SKILL_TIFFAUGES_GUSION_B_BLUE,			// 480
	SKILL_TIFFAUGES_GUSION_C_BLUE,			// 481
	SKILL_TIFFAUGES_GUSION_A_RED,			// 482
	SKILL_TIFFAUGES_GUSION_B_RED,			// 483
	SKILL_TIFFAUGES_GUSION_C_RED,			// 484
 
	SKILL_TIFFAUGES_PAIMON_BLUE,			// 485  // °æºñº´
	SKILL_TIFFAUGES_PAIMON_RED,				// 486
 
	SKILL_TIFFAUGES_MORGOTH_A_BLUE,			// 487  // ¿þ¹öµ¥½º
	SKILL_TIFFAUGES_MORGOTH_B_BLUE,			// 488
	SKILL_TIFFAUGES_MORGOTH_C_BLUE,			// 489
	SKILL_TIFFAUGES_MORGOTH_A_RED,			// 490
	SKILL_TIFFAUGES_MORGOTH_B_RED,			// 491
	SKILL_TIFFAUGES_MORGOTH_C_RED,			// 492
#endif // __TIPOJYU_CASTLE
#if __CONTENTS(__ONIBLA)
	SKILL_ONIBLA_RAISE_BONE,				// 493 ¿À´Ïºí¶ó
	SKILL_ONIBLA_DROP_SKULL,				// 494

	SKILL_AGONY_CHILD_DROP_HEAD,			// 495 ¾Æ°í´Ï Â÷ÀÏµå
	SKILL_AGONY_CHILD_SHADOW_FOUR,			// 496

	SKILL_VILLAIN_LADY_GHOST_SHADOW,		// 497 // ºô¶óÀÎ ·¹ÀÌµð
	SKILL_VILLAIN_LADY_SELF_DESTRUCTION,	// 498 // ºô¶óÀÎ ·¹ÀÌµð

	SKILL_SPANIEL_THROWING_AXE,				// 499 // ¿À´Ï ½ºÆÐ´Ï¾ó
	SKILL_BLACKASTRAL_MAGIC_CRYSTAL,		// 500 // ºí·¢ ¾Æ½ºÆ®¶ö
	SKILL_WHITEASTRAL_SPELL_CRYSTAL,		// 501 // È­ÀÌÆ® Å©¸®½ºÅ»

#endif //__ONIBLA
#if __CONTENTS(__20091116_ADD_RARE_SKILL)
	SKILL_BLADE_MAGMA_DETONATION_1,		// 502	- µµ½½ ·¹¾î½ºÅ³ 1Â÷ ¸¶±×¸¶ µ¥Åä³×ÀÌ¼Ç
	SKILL_BLADE_MAGMA_DETONATION_2,		// 503	- µµ½½ ·¹¾î½ºÅ³ 2Â÷ ¸¶±×¸¶ µ¥Åä³×ÀÌ¼Ç
	SKILL_OUSTERS_SQUALLY_BARRIER_1,	// 504	- ¾Æ¿ì½ºÅÍÁî ·¹¾î½ºÅ³ 1Â÷ ÁúÇ³ÀÇ º£¸®¾î
	SKILL_OUSTERS_SQUALLY_BARRIER_2,	// 505	- ¾Æ¿ì½ºÅÍÁî ·¹¾î½ºÅ³ 2Â÷ ÁúÇ³ÀÇ º£¸®¾î
#endif //__20091116_ADD_RARE_SKILL
    SKILL_Sius_Crash,   //506
    SKILL_Lycan_Howl,   //507 BY RALLSER
    SKILL_Bifronze_Magic,  //508
    SKILL_Palus_Middle_Range_Magic, //509
    SKILL_Palus_Long_Range_Magic,//510
    SKILL_Garum_Magic, //511
    SKILL_Clavie_Magic,//512
    SKILL_Wide_Wild_Wolf,//513
    SKILL_BlessofLycan, //514
    SKILL_SmileofDeath, //515
    SKILL_FuryShock, //516
    SKILL_Rail_Gun_Attack, //517
	//SKILL_UNKNOW_1,//uu665
    SKILL_BLAZE_WALK_2,//518Í¨¹ý 13
    SKILL_GLADIATOR,//519Í¨¹ý14
    SKILL_CRUSHING_STORM,//520Í¨¹ý15
    SKILL_SATELLITE_BOMB_2,//521Í¨¹ý16
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
	SKILL_BREATH_OF_DRYAD,//267 32   ºôÎü
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
	SKILL_DRAGON_HURRICANE,//43 ¾ÞÐÍÁú¾í·ç
	SKILL_LAR_STROKE,//44   Ç¿Á¦´ò»÷
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
	SKILL_RALLSER_44,
	SKILL_RALLSER_5,
	SKILL_RALLSER_6,
	SKILL_RALLSER_7,
	SKILL_RALLSER_8,
	SKILL_MAGMATIC_ERUPTION,//UU 201   µ¶ µØÑ×Ö®»ð
	SKILL_ADV2_LIGHTING_SHOCK,//UU201  ½£  À×öªÉÁµç
	SKILL_ADV2_HALCYON_MK1,//UU201     Ç¹  À×öªMK1
	SKILL_ADV2_SOLAR_FLARE,//           ×£¸£    Ì«ÑôÉÁÒ«
	SKILL_ADV2_VIA_CRUCIS,          //Ò½Éú       ¹â»ÔÊ®×Ö¼Ü
	SKILL_ADV2_CHAOS_SPLITTER,//       Ãô¹í   ¼±ËÙ¿ñ»÷
	SKILL_ADV2_DEADLY_GROUND,//        ÖÇ¹í  ÖÂÃüÖ®Óò  Ñª¹Ü2
	SKILL_ADV2_SUDDEN_BLAST,//       Ä§Õ½µãÈ¼ â§È»±¬ÁÑ
	SKILL_ADV2_METEOR_BLAZE,//    »ð   ÖËÈÈÁ÷ÐÇ  
	SKILL_ADV2_METEOR_BLAZE2,//   Ë®   ¾çÁÒÇÀ¶á
	SKILL_ADV2_CYCLING_WARROCK,//       ´óµØÑªÊ¯
	SKILL_CLINT_ADV2_HALCYON_MK1,//
	SKILL_ADV2_UNKNOW1,
	SKILL_ADV2_UNKNOW2,
	SKILL_ADV2_UNKNOW3,
	SKILL_ADV2_UNKNOW4,
	SKILL_ADV2_UNKNOW5,
	SKILL_ADV2_UNKNOW6,
	SKILL_ADV2_UNKNOW7,
	SKILL_ADV2_UNKNOW8,
	SKILL_ADV2_UNKNOW9,
	SKILL_ADV2_UNKNOW10,
	SKILL_ADV2_UNKNOW11,
	SKILL_ADV2_UNKNOW12,
	SKILL_ADV2_THE_ROAR_OF_AWAKEN_WARRIOR, //¹â»ÔÅ­ºð  µ¶201±»¶¯
	SKILL_ADV2_TRUE_CLOLOR_OF_VAMP_LOAD,// ¹âÃ¢¶À½£Ê¿  ½£201±»¶¯
	SKILL_ADV2_THE_RAGE_OF_GUARDIAN_SPRIT,//Ê¥ÁéµÄ·ßÅ­ 
	SKILL_CRUEL_RAT_VIRUS_INFECTION,
	SKILL_CENTIPEDE_GIANT_DISGUSTING_ACID_GAS,
	SKILL_OUTRAGE_SPECIMEN_BRUTAL_MACE,
	SKILL_CEREBRAL_TOUCH_GREATER_SUMMON_TENTACLE,
	SKILL_CARMILRA_CIRCLE_OF_BLOOD,
	SKILL_CARMILRA_DARK_BAT_STORM,
	SKILL_CARMILRA_BAT_STORM,
	SKILL_CARMILRA_DESIRE_TO_SEDUCE,
	RESULT_SKILL_CARMILRA_MELEE_ATTACK,
	SKILL_CENTIPEDE_GIANT_DISGUSTING_ACID_GAS_II,
	SKILL_TRANSFORM_ICE_WAVE, //ÈÕ·þ211
	SKILL_TRANSFORM_WIDE_ICE_HAIL, //ÈÕ·þ211
	SKILL_TRANSFORM_CIRCLE_OF_BLOOD, //ÈÕ·þ211
	SKILL_TRANSFORM_DESIRE_TO_SEDUCE, //ÈÕ·þ211
	SKILL_TRANSFORM_SMILE_OF_DEATH, //ÈÕ·þ211
	SKILL_TRANSFORM_FURY_SHOCK, //ÈÕ·þ211
	SKILL_MONSTER_TRANSFORM_HOLYSPRIT_SWORD, //ÈÕ·þ211
	SKILL_MONSTER_TRANSFORM_HOLYSPRIT_FURY, //ÈÕ·þ211
	SKILL_MONSTER_TRANSFORM_HOLYSPRIT_BLESS, //ÈÕ·þ211
	SKILL_TRANSFORM_DARK_BAT_STORM, //ÈÕ·þ211
	SKILL_ULRAYIRY_MELLE_ATTACK_A, //ÈÕ·þ211
	SKILL_ULRAYIRY_MELLE_ATTACK_B, //ÈÕ·þ211
	SKILL_GREEN_ULRAYIRY_ANGER, //ÈÕ·þ211
	SKILL_BLUE_ULRAYIRY_ANGER, //ÈÕ·þ211
	SKILL_BLUE_ULRAYIRY_ANGER_2, //ÈÕ·þ211
	SKILL_ULRAYIRY_ANGER_D, //ÈÕ·þ211
	SKILL_GREEN_ULRAYIRY_PAIN, //ÈÕ·þ211*/
	SKILL_Not_Defined_hfnew1,
	SKILL_Not_Defined_hfnew2,
	SKILL_Not_Defined_hfnew3,
	SKILL_Not_Defined_hfnew4,
	SKILL_Not_Defined_hfnew5,
	SKILL_Not_Defined_hfnew6,
	SKILL_Not_Defined_hfnew7,
	SKILL_Not_Defined_hfnew8,
	SKILL_Not_Defined_hfnew9,
    SKILL_ENRAGE_CONQUROR,
	SKILL_ATTRACT_CIRCLE,
	SKILL_PULSEMAGNETICFIELD,
	SKILL_TEMENOS,
	SKILL_DEPEND_SOUL,
	SKILL_IGNIS_INFERNUM,
	SKILL_DEMONIC_RESURRECTIO,
	SKILL_WOLF_ROAR,
	SKILL_EVGENEIA_BELLUM,
    SKILL_OUTRAGE_BOOSTER,
	SKILL_ICE_PRESSURE,
	SKILL_CRYSTAL_THORN,
	SKILL_FAVOR_OF_LUNA,
	SKILL_ULRAYIRY_CURSE,
	SKILL_Not_Defined_hf221new1,//57
	SKILL_ION_CANNON_ATTACK,//58  
	SLILL_BOMBING_VULCAN,
	SKILL_AXE_OF_RUIN,
	SKILL_EYES_OF_DESTRUCTION,
	SKILL_THORN_STRIKE,
	SKILL_THORNBUSH_WAVE,
	SKILL_CREATURE_GOLEM_QUAKE_ATTACK,
	SKILL_CREATURE_GOLEM_HANDWAVE_ATTACK,
	SKILL_CREATURE_GOLEM_DEFENSIVE_ENERGEFILED,
	SKILL_ANCIENT_VANDALIZER_THROW_DOUBLE_AXE,
	SKILL_LAY_WASTE,
	SKILL_Not_Defined_hf221new2,
	SKILL_IMMUNE_DRAIN,
	SKILL_ABSORB_SHIELD,
	SKILL_APOCALYPSE,
	SKILL_DRAFTED_COMMAND,
	SKILL_VOMIT,
	SKILL_WHIP,
	SKILL_MODERAS_ATTACK1,
	SKILL_TRANSFORM_GILLES,
	SKILL_UNTRANSFORM_GILLES,
	SKILL_Not_Defined_hf221new3,
	SKILL_Not_Defined_hf221new4,
	SKILL_Not_Defined_hf221new5,
	SKILL_ADV2_CONTRADICTO,
	SKILL_ADV2_CHAIN_LIGHTNING_SWORD,
	SKILL_ADV2_HALCYON_MK1_NAPALN_BULLET,
	SKILL_ADV2_CONTRADICTO2,
	SKILL_ADV2_DIVINE_TRACES,
	SKILL_ADV2_WILD_WOLF_STRIKE,
	SKILL_ADV2_DEADLY_STORM,
	SKILL_ADV2_SHINING_CHAKRAM_SPIN,
	SKILL_ADV2_INFINITY_BLAZE,
	SKILL_ADV2_GRACE_OF_ELQUINES,
	SKILL_ADV2_SUMMON_DEFENSIVE_SPINEL,
	SKILL_TRANSPORTATION_TELEPORT,
	SKILL_HALF_MOON_KICK,
	SKILL_HALF_MOON_KICK2,
	SKILL_BLOOD_PUNISH,
	SKILL_BLOOD_PRAY,
	SKILL_DEVIL_WAVE,
	SKILL_PARASITE_ATTACK,
	SKILL_DEVIL_SCREAM,
	SKILL_POISON_MIST,
	SKILL_BONE_STRIKE,
	SKILL_CLIENT_LUNATIC_SHOCK,
    SKILL_CLIENT_BLOOD_SPLASHER,
	SKILL_CLIENT_FATAL_SCREAM,
	SKILL_LIGHTNING_SLASH,
	SKILL_GENESIS_ATTACK,
	SKILL_BLOODY_SKULL_2,//201¼ÓÇ¿¼¼ÄÜ
	SKILL_ADV2_CHAOS_SPLITTER_2,
	SKILL_CRYSTAL_THORN_2,
	SKILL_BLIZZARD_2,
	SKILL_ADV2_METEOR_BLAZE_2,
	SKILL_CHAKRAM_HAIL_2,
	SKILL_ADV2_SOLAR_FLARE_2,
	SKILL_ADV2_VIA_CRUCIS_2,
	SKILL_CLIENT_ADV2_HALCYON_MK1,
	SKILL_ADV2_LIGHTING_SHOCK_2,
	SKILL_DRAGON_HURRICANE_2,
	SKILL_BLADE_ERUPTION_1,//241¼¼ÄÜ  µ¶
	SKILL_ATTRACT_SWORD_CRASH_1,		//241½£
	SKILL_STRATEGIC_BOMBER_1,			 //241 Ç¹
	SKILL_AURA_EXPLOSION_1,			   //×£¸£
	SKILL_HOLY_ACIDALIUM_1,			   //Ò½Éú
	SKILL_BAT_SPEAR_1,				   //Õ½¹í
	SKILL_EXTENSIVE_DELUSION_1,		   //ÖÇ¹í
	SKILL_ACCELERATE_CHARGE_1,		   //Õ½Ä§
	SKILL_RAGE_BURST_1,				   //»ðÄ§
	SKILL_FROZEN_GROUND_1,			   //Ë®Ä§
	SKILL_METEORIC_FALL_1,			   //ÍÁÄ§
	SKILL_BLADE_ERUPTION_2,
	SKILL_ATTRACT_SWORD_CRASH_2,
	SKILL_STRATEGIC_BOMBER_2,
	SKILL_AURA_EXPLOSION_2,
	SKILL_HOLY_ACIDALIUM_2,
	SKILL_BAT_SPEAR_2,
	SKILL_EXTENSIVE_DELUSION_2,
	SKILL_ACCELERATE_CHARGE_2,
	SKILL_RAGE_BURST_2,
	SKILL_FROZEN_GROUND_2,
	SKILL_METEORIC_FALL_2,
	SKILL_WARP_PORTAL,
	SKILL_ADV3_UNKNOW1,
	SKILL_ADV3_UNKNOW2,
	SKILL_FRAGRANT_BREATH,
	SKILL_TREE_TRUNK_STRIKE,
	SKILL_DEATHFUL_BREATH,
	SKILL_SPIRIT_OF_DEAD_SICKLE,
	SKILL_COME_ON_OVER,
	SKILL_EREBUS_IS_CRY,
	SKILL_ROLLING_CONTINUE,
	SKILL_BITE,
	SKILL_THORN_LAUNCH,
	SKILL_ONE_TWO_CUT,
	SKILL_OVEREAT,
	SKILL_THORN_LAUNCH_ADD_DEBUFF,
	SKILL_TREE_TRUNK_STRIKE_ADD_DEBUFF,
	SKILL_SPIRIT_OF_DEAD_SICKLE_ADD_DEBUFF,
	SKILL_POWERFUL_CLAW,
	SKILL_ADELANTE_IS_CRY,
	SKILL_POTENTIAL_EXPLOSION_2,
	SKILL_SHARP_SHIELD_2,
	SKILL_SNIPING_2,
	SKILL_STUN_SHOT,
	SKILL_THORNS_2,
	SKILL_CURE_SERIOUS_WOUNDS_2,
	SKILL_SET_AFIRE_2,
	SKILL_SHADY_DOUPLE_2,
	SKILL_HALO_2,
	SKILL_RING_OF_FLARE_2,
	SKILL_WATER_BARRIER_2,
	SKILL_REACTIVE_ARMOR_2,

	SKILL_FIRE_WAVE_2,
	SKILL_PLAYING_WITH_FIRE_2,
	SKILL_GENISYS_ANGER,
	SKILL_DIE_EXPLOSION,

	SKILL_CLIENT_SHADY_DOUPLE,
	SKILL_CLIENT_BAT_BREAKER,
	SKILL_CLIENT_AD2_INFINITY_BLAZE,
	SKILL_CLIENT_ADV2_NAYPAM_BULLET,
	//------------------------------------------------------------
	// ¿©±âºÎÅÍ´Â client Àü¿ë SKILL ID
	//------------------------------------------------------------
	// ÀÓ½Ã·Î Ãß°¡	
	
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
	SKILL_ATTACK_GUN_SR,			// ÃÑ1
	SKILL_ATTACK_GUN_SG,			// ÃÑ2
	SKILL_ATTACK_GUN_AR,			// ÃÑ3
	SKILL_ATTACK_GUN_SMG,			// ÃÑ4

	// sword
	SKILL_ATTACK_SWORD,		// Ä®1
	SKILL_ATTACK_BLADE,		// Ä®2	

	// ÃÑ¿¡ µû¸¥ holy shooting
	SKILL_HOLY_SHOOTING_TR,					//	Holy shooting
	SKILL_HOLY_SHOOTING_SG,					//	Holy shooting
	SKILL_IRON_MAIDEN,					//	Holy shooting
	SKILL_CHAOS_SPLITTER_2_M,				//	Holy shooting

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

	// Dummy Skill - ÀÌÆÑÆ®¸¦ º¸¿©ÁÖÁö ¾Ê°í ½ÃÀü µ¿ÀÛ¸¸ ÀÖ´Â Client SkillÀÌ´Ù.
	SKILL_DUMMY_ACTION_MAGIC,

	//------------------------------------------------------------------------------
	// ½ºÅ³ ´Ü°è¿¡ µû¸¥ ½ºÅ³
	//------------------------------------------------------------------------------
	STEP_SKILL_FLOURISH_2,							// 2´Ü°è FLOURISH
	STEP_SKILL_FLOURISH_3,							// 3´Ü°è FLOURISH
	STEP_SKILL_SHARP_ROUND_2,						// 2´Ü°è Sharp Round
	STEP_SKILL_SHARP_ROUND_3,						// 3´Ü°è Sharp Round
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
	MAGIC_ENCHANT_OPTION_NULL,				// ¿É¼ÇÀÌ ¾ø¾îÁø´Ù
	MAGIC_ENCHANT_OPTION_PLUS,				// ¿É¼ÇÀÌ ÇÏ³ª ¿Ã¶ó°£´Ù
	MAGIC_ENCHANT_REMOVE_ITEM,				// ¾ÆÀÌÅÛÀÌ ¾ø¾îÁø´Ù

	// HolyLandBonus Skills 2003.2.10
	SKILL_HOLYLAND_BLOOD_BIBLE_ARMEGA,				// ±â¼ú »ç¿ë½Ã MP(HP)¼Ò¸ð·® 50% °¨¼Ò  
	SKILL_HOLYLAND_BLOOD_BIBLE_MIHOLE,				// Å¸Á¾Á·ÀÇ ¾ð¾î Åë¿ª  
	SKILL_HOLYLAND_BLOOD_BIBLE_KIRO,				// INT+5, DEX +5  
	SKILL_HOLYLAND_BLOOD_BIBLE_INI,					// ¹°¸®°ø°Ý µ¥¹ÌÁö +10  
	SKILL_HOLYLAND_BLOOD_BIBLE_GREGORI,				// ¹°¾à°ª 50% ÇÒÀÎ  
	SKILL_HOLYLAND_BLOOD_BIBLE_CONCILIA,			// HP +50  
	SKILL_HOLYLAND_BLOOD_BIBLE_LEGIOS,				// ¸¶¹ý°ø°Ýµ¥¹ÌÁö +10  
	SKILL_HOLYLAND_BLOOD_BIBLE_HILLEL,				// Çà¿î +10  
	SKILL_HOLYLAND_BLOOD_BIBLE_JAVE,				// °×ºí°¡°Ý 50% ÇÒÀÎ  
	SKILL_HOLYLAND_BLOOD_BIBLE_NEMA,				// ½Ã¾ß +5, ¸ðµç ´É·ÂÄ¡ +3  
	SKILL_HOLYLAND_BLOOD_BIBLE_AROSA,				// ¸ðµç ÀúÇ×·Â +9  
	SKILL_HOLYLAND_BLOOD_BIBLE_CHASPA,				// STR +5, INT +5 

	SKILL_LOVE_CHAIN,								// ·¯ºê Ã¼ÀÎ ½ºÅ³	
	
	MAGIC_TRANS_ITEM_OK,	
	SKILL_TRANSLATION,
	
	SKILL_SWEEPER_BONUS_1,							// ½ºÀ§ÆÛ º¸³Ê½º
	SKILL_SWEEPER_BONUS_2,							// ½ºÀ§ÆÛ º¸³Ê½º
	SKILL_SWEEPER_BONUS_3,							// ½ºÀ§ÆÛ º¸³Ê½º
	SKILL_SWEEPER_BONUS_4,							// ½ºÀ§ÆÛ º¸³Ê½º
	SKILL_SWEEPER_BONUS_5,							// ½ºÀ§ÆÛ º¸³Ê½º
	SKILL_SWEEPER_BONUS_6,							// ½ºÀ§ÆÛ º¸³Ê½º
	SKILL_SWEEPER_BONUS_7,							// ½ºÀ§ÆÛ º¸³Ê½º
	SKILL_SWEEPER_BONUS_8,							// ½ºÀ§ÆÛ º¸³Ê½º
	SKILL_SWEEPER_BONUS_9,							// ½ºÀ§ÆÛ º¸³Ê½º
	SKILL_SWEEPER_BONUS_10,							// ½ºÀ§ÆÛ º¸³Ê½º
	SKILL_SWEEPER_BONUS_11,							// ½ºÀ§ÆÛ º¸³Ê½º
	SKILL_SWEEPER_BONUS_12,							// ½ºÀ§ÆÛ º¸³Ê½º
	
	// 2004, 6, 15 sobeit add start 
	SKILL_CLIENT_CAN_ENTER_GDR_LAIR, //- Áúµå·¹ µé¾î°¥¼ö ÀÖÀ½
	SKILL_CLIENT_BLINDNESS,			// ´ÙÅ©´Ï½º¿Í °°Àº È¿°ú
	// 2004, 6, 15 sobeit add end - 
	SKILL_CLIENT_DRAGON_EYES,	// µå·¹°ï ¾ÆÀÌÁî

	SKILL_CLIENT_EXP_BONUS_EVENT, // °æÇèÄ¡ º¸³Ê½º
		

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
	SKILL_FIRE_WAVE_M,					// 485
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
// by rallser
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
	SKILL_CLIENT_EFFECT_NOT_1,
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
	SKILL_CLIENT_RALLSER_2,//20 ÑªÇò
    SKILL_CLIENT_BLAZE_WALK_2,//21
    SKILL_CLIENT_CASTLE_MIHNEA2,//22
	SKILL_CLIENT_201_UNKNOW1,
	SKILL_CLIENT_201_UNKNOW2,
	SKILL_CLIENT_201_UNKNOW3,
	SKILL_CLIENT_201_UNKNOW4,
	SKILL_CLIENT_201_UNKNOW5,
	SKILL_CLIENT_201_UNKNOW6,
	SKILL_CLIENT_201_UNKNOW7,
	SKILL_CLIENT_201_UNKNOW8,
	SKILL_CLIENT_201_UNKNOW9,
	SKILL_CLIENT_201_UNKNOW10,
	SKILL_CLIENT_201_UNKNOW11,
	SKILL_CLIENT_201_UNKNOW12,
	SKILL_CLIENT_201_UNKNOW13,
	SKILL_CLIENT_201_UNKNOW14,
	SKILL_CLIENT_201_UNKNOW15,
	SKILL_CLIENT_MONSTER_TRANSFORM_HOLYSPRIT_BLESS_TARGET,  //ÈÕ·þ
	SKILL_CLIENT_201_UNKNOW16,
	SKILL_CLIENT_201_UNKNOW17,
	SKILL_CLIENT_201_UNKNOW18,
	SKILL_CLIENT_201_UNKNOW19,
	SKILL_CLIENT_201_UNKNOW20,
	SKILL_CLIENT_201_UNKNOW21,
	SKILL_CLIENT_201_UNKNOW22,
	SKILL_CLIENT_201_UNKNOW23,
	SKILL_CLIENT_201_UNKNOW24,
	SKILL_CLIENT_201_UNKNOW25,
	SKILL_EXTENSIVE_DELUSION_2_M,
	SKILL_CLIENT_201_UNKNOW27,
	SKILL_CLIENT_201_UNKNOW28,
  	SKILL_CLIENT_201_UNKNOW29,

	SKILL_CLIENT_201_UNKNOW30,
	SKILL_CLIENT_201_UNKNOW31,
	SKILL_CLIENT_201_UNKNOW32,
	SKILL_CLIENT_201_UNKNOW33,
	SKILL_CLIENT_201_UNKNOW34,
	SKILL_CLIENT_201_UNKNOW35,
	SKILL_CLIENT_201_UNKNOW36,
	SKILL_CLIENT_201_UNKNOW37,
	SKILL_CLIENT_201_UNKNOW38,
  	SKILL_CLIENT_201_UNKNOW39,

	SKILL_CLIENT_201_UNKNOW40,
	SKILL_CLIENT_201_UNKNOW41,
	SKILL_CLIENT_201_UNKNOW42,
	SKILL_CLIENT_201_UNKNOW43,
	SKILL_CLIENT_201_UNKNOW44,
	SKILL_CLIENT_201_UNKNOW45,
	SKILL_CLIENT_201_UNKNOW46,
	SKILL_CLIENT_201_UNKNOW47,
	SKILL_CLIENT_201_UNKNOW48,
  	SKILL_CLIENT_201_UNKNOW49,

	SKILL_CLIENT_201_UNKNOW50,
	SKILL_CLIENT_201_UNKNOW51,
	SKILL_CLIENT_201_UNKNOW52,
	SKILL_CLIENT_201_UNKNOW53,
	SKILL_CLIENT_201_UNKNOW54,
    SKILL_CLIENT_ACT_OPTION_TARGET_ADD_DAMAGE,
	SKILL_CLIENT_201_UNKNOW55,
  	SKILL_CLIENT_201_UNKNOW56,

	SKILL_CLIENT_201_UNKNOW57,
	SKILL_CLIENT_201_UNKNOW58,
	SKILL_CLIENT_201_UNKNOW59,
	SKILL_CLIENT_201_UNKNOW60,
	SKILL_CLIENT_201_UNKNOW61,
	SKILL_CLIENT_DRAFTED_COMMAND_TOPC,
	SKILL_CLIENT_IMMUNE_DRAIN_CASTING,
	SKILL_CLIENT_201_UNKNOW62,
	SKILL_CLIENT_201_UNKNOW63,
	SKILL_CLIENT_ADV2_CONTRADICTO,
	SKILL_CLIENT_AD2_CHAIN_LIGHTNING_SWORD_DMG,
	SKILL_CLIENT_AD2_ADV2_DEADLY_STORM_DMG,
	SKILL_CLIENT_GRACE_OF_ELQUINES_CASTING,
	SKILL_CLIENT_HALF_MOON_KICK_DMG,
	SKILL_CLIENT_DEVIL_SCREAM_TARGET,
	SKILL_CLIENT_POISON_MIST,
	SKILL_CLIENT_RUENHAIM_TILE,
	SKILL_CLIENT_221_UNKNOW01,
	SKILL_CLIENT_221_UNKNOW02,
	SKILL_BLOODY_SKULL_2_M,
	SKILL_TEMENOS_M,
	SKILL_WILD_WOLF_STRIKE_M,
	SKILL_VENOM_CIRCLE_M,
	SKILL_BAT_SPEAR_2_M,
	SKILL_CLIENT_221_UNKNOW08,
	SKILL_CLIENT_221_UNKNOW09,
	SKILL_CLIENT_221_UNKNOW10,
	SKILL_CLIENT_GARGOYLE_YELLOW,
	SKILL_CLIENT_GARGOYLE_ORANGE,
	SKILL_CLIENT_GARGOYLE_RED,
	SKILL_CLIENT_221_UNKNOW11,
	SKILL_CLIENT_221_UNKNOW12,
	SKILL_CLIENT_221_UNKNOW13,
	SKILL_CLIENT_221_UNKNOW14,
	SKILL_CLIENT_221_UNKNOW15,
	SKILL_CLIENT_KAMALA_PET_RED_EFFECT,
	SKILL_CLIENT_221_UNKNOW16,
	SKILL_CLIENT_221_UNKNOW17,
	SKILL_CLIENT_221_UNKNOW18,
	SKILL_CLIENT_221_UNKNOW19,
	SKILL_CLIENT_221_UNKNOW20,
	SKILL_CLIENT_221_UNKNOW21,
	SKILL_CLIENT_221_UNKNOW22,
	SKILL_CLIENT_221_UNKNOW23,
	SKILL_CLIENT_221_UNKNOW24,
	SKILL_CLIENT_221_UNKNOW25,
	SKILL_CLIENT_221_UNKNOW26,
	SKILL_CLIENT_221_UNKNOW27,
	SKILL_CLIENT_221_UNKNOW28,
	SKILL_CLIENT_221_UNKNOW29,
	SKILL_CLIENT_221_UNKNOW30,
	SKILL_CLIENT_221_UNKNOW31,
	SKILL_CLIENT_221_UNKNOW32,

	SKILL_CLIENT_221_UNKNOW33,
	SKILL_CLIENT_221_UNKNOW34,
	SKILL_CLIENT_221_UNKNOW35,
	SKILL_CLIENT_221_UNKNOW36,
	SKILL_CLIENT_221_UNKNOW37,
	SKILL_CLIENT_221_UNKNOW38,
	SKILL_CLIENT_221_UNKNOW39,
	SKILL_CLIENT_221_UNKNOW40,
	SKILL_CLIENT_221_UNKNOW41,
	SKILL_CLIENT_221_UNKNOW42,
	SKILL_CLIENT_221_UNKNOW43,
	SKILL_CLIENT_221_UNKNOW44,
	SKILL_CLIENT_221_UNKNOW45,
	SKILL_CLIENT_221_UNKNOW46,
	SKILL_CLIENT_221_UNKNOW47,
	SKILL_CLIENT_221_UNKNOW48,
	SKILL_CLIENT_221_UNKNOW49,
	SKILL_CLIENT_221_UNKNOW50,
	SKILL_CLIENT_221_UNKNOW51,
	SKILL_CLIENT_221_UNKNOW52,
	SKILL_CLIENT_221_UNKNOW53,
	SKILL_CLIENT_221_UNKNOW54,
	SKILL_CLIENT_221_UNKNOW55,
	SKILL_CLIENT_221_UNKNOW56,
	SKILL_CLIENT_221_UNKNOW57,
	SKILL_CLIENT_221_UNKNOW58,

	SKILL_CLIENT_221_UNKNOW59,//241¼¼ÄÜ
	SKILL_CLIENT_221_UNKNOW60,
	SKILL_CLIENT_ATTRACT_SWORD_CRASH_PC_HOLD,
	SKILL_CLIENT_ATTRACT_SWORD_CRASH_2_PC_HOLD,
	SKILL_CLIENT_EXTENSIVE_DELUSION_CURSE,
	SKILL_CLIENT_EXTENSIVE_DELUSION_CURSE_2,
	SKILL_CLIENT_RAGE_BURST_HIT,
	SKILL_CLIENT_RAGE_BURST_2_HIT,
	SKILL_CLIENT_FROZEN_GROUND_HIT,
	SKILL_CLIENT_FROZEN_GROUND_2_HIT,
	SKILL_ADV3_UNKNOW3,
	SKILL_ADV3_UNKNOW4,
	SKILL_ADV4_UNKNOW1,
	SKILL_ADV4_UNKNOW2,
	SKILL_ADV4_UNKNOW3,
	SKILL_ADV4_UNKNOW4,
	SKILL_ADV4_UNKNOW5,
	SKILL_ADV4_UNKNOW6,
	SKILL_ADV4_UNKNOW7,
	SKILL_ADV4_UNKNOW8,
	SKILL_ADV4_UNKNOW9,
	SKILL_ADV4_UNKNOW10,
	SKILL_CLIENT_FOREST_DOT_DAM,
	SKILL_CLIENT_FOREST_STATIC_DAM,
	SKILL_CLIENT_FOREST_INVINCIBILITY,
	SKILL_CLIENT_FOREST_INCREASE_DAM,
	SKILL_CLIENT_FOREST_HIDDEN_BOSS_BODY,
	SKILL_CLIENT_FOREST_ALL_MAGIC_HIT,
	SKILL_ADV4_UNKNOW11,
	SKILL_CLIENT_SHADY_DOUPLE_2,
	SKILL_CLIENT_RUNE_ENCHANT_RESULT_EFFECT,

	SKILL_ADV5_UNKNOW10,
	SKILL_ADV5_UNKNOW11,
	SKILL_ADV5_UNKNOW12,
	SKILL_ADV5_UNKNOW13,
	SKILL_ADV5_UNKNOW14,
	SKILL_ADV5_UNKNOW15,
	SKILL_ADV5_UNKNOW16,
	SKILL_ADV5_UNKNOW17,
	SKILL_ADV5_UNKNOW18,
	SKILL_ADV5_UNKNOW19,
	SKILL_ADV5_UNKNOW20,
	SKILL_ADV5_UNKNOW21,
	SKILL_ADV5_UNKNOW22,
	SKILL_ADV5_UNKNOW23,
	SKILL_ADV5_UNKNOW24,
	SKILL_CLIENT_BASE_WAR_STATUE_TILEAREA_SLAYER,
	SKILL_CLIENT_BASE_WAR_STATUE_TILEAREA_VAMPIRE,
	SKILL_CLIENT_BASE_WAR_STATUE_TILEAREA_OUSTERS,

	//-----------------------------------------------------------------
	// RESULT_ACTIONINFO
	//-----------------------------------------------------------------		
	// ±âº» ±â¼ú
	RESULT_SKILL_ATTACK_MELEE,						// 0
	RESULT_SKILL_ATTACK_ARMS,							// 1
	RESULT_SKILL_SELF,									// 2
	RESULT_SKILL_TILE,									// 3
	RESULT_SKILL_OBJECT,	    						// 4

	// °Ë°è¿­,	
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

	// µµ °è¿­,
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

	// °Ç, ¶óÀÌÇÃ °è¿­
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
    
	// ¼ºÁ÷¸¶¹ý,
	// ÀÎÃ¾ °è¿­
	RESULT_MAGIC_CREATE_HOLY_WATER, 			 		// 39
	RESULT_MAGIC_LIGHT,				      			// 40
	RESULT_MAGIC_DETECT_HIDDEN,	   	 				// 41
	RESULT_MAGIC_AURA_BALL,	     		 		 	// 55
	RESULT_MAGIC_BLESS,			       		 		// 45
	RESULT_MAGIC_CONTINUAL_LIGHT,	 			 		// 46
	RESULT_MAGIC_FLARE,								// ¾ó¸¶ÀÏ±î ´Ù½Ã Á¤¸® ÇØ¾ßÁö
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

	// Èú °è¿­
	RESULT_MAGIC_CURE_LIGHT_WOUNDS,					// 52
	RESULT_MAGIC_CURE_POISON,							// 54
	RESULT_MAGIC_PROTECTION_FROM_POISON,				// 53
	RESULT_MAGIC_CAUSE_LIGHT_WOUNDS,					// ¾ó¸¶ÀÏ±î ´Ù½Ã Á¤¸® ÇØ¾ßÁö
	RESULT_MAGIC_CURE_SERIOUS_WOUNDS,					// 56
	RESULT_MAGIC_REMOVE_CURSE,							// 89
	RESULT_MAGIC_PROTECTION_FROM_CURSE,				// 59
	RESULT_MAGIC_CAUSE_SERIOUS_WOUNDS,					// ¾ó¸¶ÀÏ±î ´Ù½Ã Á¤¸® ÇØ¾ßÁö
	RESULT_MAGIC_CURE_CRITICAL_WOUNDS,					// 
	RESULT_MAGIC_PROTECTION_FROM_ACID,					//
	RESULT_MAGIC_SACRIFICE,							//
	RESULT_MAGIC_CAUSE_CRITICAL_WOUNDS,				// ¾ó¸¶ÀÏ±î ´Ù½Ã Á¤¸® ÇØ¾ßÁö
	RESULT_MAGIC_CURE_ALL,								// 62
	RESULT_MAGIC_REGENERATION,							// 61
	RESULT_MAGIC_MASS_CURE,							//
	RESULT_MAGIC_MASS_HEAL,							//

	// ¹ìÇÁ±â¼ú,
	RESULT_SKILL_BLOOD_DRAIN,							// 65

	// ¹ìÇÁ¸¶¹ý,
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

	// ½á¸ó
	RESULT_MAGIC_SUMMON_WOLF,							// 87
	RESULT_MAGIC_SUMMON_CASKET,						// 88
	RESULT_MAGIC_RAISING_DEAD,							// 88
	RESULT_MAGIC_SUMMON_SERVANT,						//

	// ETC
	RESULT_MAGIC_UN_BURROW,							// 89
	RESULT_MAGIC_UN_TRANSFORM,							// 89
	RESULT_MAGIC_UN_INVISIBILITY,
	RESULT_MAGIC_THROW_HOLY_WATER,						// 91

	// ´Á´ë ±â¼ú
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

	// 2001.12.4 Ãß°¡
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

	// Ãß°¡ 2002.3.23
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
	RESULT_SKILL_CRITICAL_GROUND,		// criticalÀ» »ç¿ëÇÏ´Â °æ¿ì	

	RESULT_SKILL_VIGOR_DROP,

	// [»õ±â¼ú]
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
	// Áúµå·¹ ½ºÅ³
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
    RESULT_SKILL_CHOPPING_FIREWOOD,                    // 337 ÀåÀÛÆÐ±â	

	RESULT_SKILL_CHAIN_THROWING_AXE,                   // 338
    RESULT_SKILL_MULTI_THROWING_AXE,                   // 339
    RESULT_SKILL_PLAYING_WITH_FIRE,                    // 340 ºÒÀå³­

	RESULT_SKILL_INFINITY_THUNDERBOLT,                 // 341
    RESULT_SKILL_SPIT_STREAM,                          // 342
    RESULT_SKILL_PLASMA_ROCKET_LAUNCHER,               // 343
    RESULT_SKILL_INTIMATE_GRAIL,                       // 344
    RESULT_SKILL_BOMBING_STAR,                         // 345
    RESULT_SKILL_SET_AFIRE,                            // 346
    RESULT_SKILL_NOOSE_OF_WRAITH,                      // 347

	RESULT_SKILL_SHARP_HAIL,                           // 348
    RESULT_SKILL_SUMMON_MIGA,                          // 349  // ¾Æ¿ì½ºÅÍÁî°¡ ¾²´Â ½ºÅ³
    RESULT_SKILL_SUMMON_MIGA_ATTACK,                   // 350  // ¼ÒÈ¯µÈ³ðÀÌ ¾²´Â ½ºÅ³
    RESULT_SKILL_ICE_HORIZON,                          // 351
    RESULT_SKILL_FURY_OF_GNOME,                        // 352
	
	RESULT_SKILL_CANNONADE,								// 353  // Æ÷°Ý
    RESULT_SKILL_SELF_DESTRUCTION,						// 354  // ÀÚÆø°ø°Ý

	RESULT_SKILL_AR_ATTACK,                            // 355  // ¸ó½ºÅÍ¿ë
    RESULT_SKILL_SMG_ATTACK,                           // 356  // ¸ó½ºÅÍ¿ë
    RESULT_SKILL_GRENADE_ATTACK,                       // 357  // ¸ó½ºÅÍ¿ë

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

	RESULT_SKILL_HEAL_PASS,                            // 369 // ¿Å°Ü°¡´Â Èú

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
	// ·¹¾î½ºÅ³ ÆÐ½Ãºê
	RESULT_SKILL_INCREASE_OF_HEALTH1,					//459
	RESULT_SKILL_INCREASE_OF_HEALTH2,					//460
	RESULT_SKILL_INCREASE_OF_HEALTH3,					//461
	RESULT_SKILL_INCREASE_OF_HEALTH4,					//462
	RESULT_SKILL_INCREASE_OF_HEALTH5,					//463
	RESULT_SKILL_INCREASE_OF_HEALTH6,					//464
#endif //__SECOND_RARE_SKILL

#if __CONTENTS(__TIPOJYU_CASTLE)	//Æ¼Æ÷ÁÖ ¸ó½ºÅÍ ½ºÅ³ (¼­¹ö¿Í ¹øÈ£°¡ ¸Â¾Æ¾ß ÇÑ´Ù.)
	RESULT_SKILL_TIFFAUGES_BALBERITH_BLUE,				// 465  // ¹®Áö±â µµ³¢
	RESULT_SKILL_TIFFAUGES_BALBERITH_RED,				// 466  
 
	RESULT_SKILL_TIFFAUGES_BERITH_BLUE,					// 467  // ±ÙÀ§´ë Ã¢
	RESULT_SKILL_TIFFAUGES_BERITH_RED,					// 468  
 
	RESULT_SKILL_TIFFAUGES_VALKYRJA_BLUE,				// 469  // ±ÙÀ§´ë ´ëÀå
	RESULT_SKILL_TIFFAUGES_VALKYRJA_RED,				// 470  
 
	RESULT_SKILL_TIFFAUGES_GOMORY_A_BLUE,				// 471  // ¿©ÀÚ ¹éÀÛ
	RESULT_SKILL_TIFFAUGES_GOMORY_B_BLUE,				// 472
	RESULT_SKILL_TIFFAUGES_GOMORY_A_RED,				// 473
	RESULT_SKILL_TIFFAUGES_GOMORY_B_RED,				// 474
 
	RESULT_SKILL_TIFFAUGES_KRSNIK_A_BLUE,				// 475  // ³²ÀÚ ¹éÀÛ
	RESULT_SKILL_TIFFAUGES_KRSNIK_B_BLUE,				// 476
	RESULT_SKILL_TIFFAUGES_KRSNIK_A_RED,				// 477
	RESULT_SKILL_TIFFAUGES_KRSNIK_B_RED,				// 478
 
	RESULT_SKILL_TIFFAUGES_GUSION_A_BLUE,				// 479  // ³²ÀÚ ¹éÀÛ ´ëÀå
	RESULT_SKILL_TIFFAUGES_GUSION_B_BLUE,				// 480
	RESULT_SKILL_TIFFAUGES_GUSION_C_BLUE,				// 481
	RESULT_SKILL_TIFFAUGES_GUSION_A_RED,				// 482
	RESULT_SKILL_TIFFAUGES_GUSION_B_RED,				// 483
	RESULT_SKILL_TIFFAUGES_GUSION_C_RED,				// 484
 
	RESULT_SKILL_TIFFAUGES_PAIMON_BLUE,					// 485  // °æºñº´
	RESULT_SKILL_TIFFAUGES_PAIMON_RED,					// 486
 
	RESULT_SKILL_TIFFAUGES_MORGOTH_A_BLUE,				// 487  // ¿þ¹öµ¥½º
	RESULT_SKILL_TIFFAUGES_MORGOTH_B_BLUE,				// 488
	RESULT_SKILL_TIFFAUGES_MORGOTH_C_BLUE,				// 489
	RESULT_SKILL_TIFFAUGES_MORGOTH_A_RED,				// 490
	RESULT_SKILL_TIFFAUGES_MORGOTH_B_RED,				// 491
	RESULT_SKILL_TIFFAUGES_MORGOTH_C_RED,				// 492
#endif // __TIPOJYU_CASTLE

#if __CONTENTS(__ONIBLA)

	RESULT_SKILL_ONIBLA_RAISE_BONE,						// 493 ¿À´Ïºí¶ó 
	RESULT_SKILL_ONIBLA_DROP_SKULL,						// 494 

	RESULT_SKILL_AGONY_CHILD_DROP_HEAD,					// 495 ¾Æ°í´Ï Â÷ÀÏµå
	RESULT_SKILL_AGONY_CHILD_SHADOW_FOUR,				// 496

	RESULT_SKILL_VILLAIN_LADY_GHOST_SHADOW,				// 497 // ºô¶óÀÎ ·¹ÀÌµð
	RESULT_SKILL_VILLAIN_LADY_SELF_DESTRUCTION,			// 498 // ºô¶óÀÎ ·¹ÀÌµð

	RESULT_SKILL_SPANIEL_THROWING_AXE,					// 499 // ¿À´Ï ½ºÆÐ´Ï¾ó
	RESULT_SKILL_BLACKASTRAL_MAGIC_CRYSTAL,				// 500 // ºí·¢ ¾Æ½ºÆ®¶ö
	RESULT_SKILL_WHITEASTRAL_SPELL_CRYSTAL,				// 501 // È­ÀÌÆ® Å©¸®½ºÅ»
#endif //__ONIBLA

#if __CONTENTS(__20091116_ADD_RARE_SKILL)
	RESULT_SKILL_BLADE_MAGMA_DETONATION_1,				// 502	- µµ½½ ·¹¾î½ºÅ³ 1Â÷ ¸¶±×¸¶ µ¥Åä³×ÀÌ¼Ç
	RESULT_SKILL_BLADE_MAGMA_DETONATION_2,				// 503	- µµ½½ ·¹¾î½ºÅ³ 2Â÷ ¸¶±×¸¶ µ¥Åä³×ÀÌ¼Ç
	RESULT_SKILL_OUSTERS_SQUALLY_BARRIER_1,				// 504	- ¾Æ¿ì½ºÅÍÁî ·¹¾î½ºÅ³ 1Â÷ ÁúÇ³ÀÇ º£¸®¾î
	RESULT_SKILL_OUSTERS_SQUALLY_BARRIER_2,				// 505	- ¾Æ¿ì½ºÅÍÁî ·¹¾î½ºÅ³ 2Â÷ ÁúÇ³ÀÇ º£¸®¾î
#endif //__20091116_ADD_RARE_SKILL
    RESULT_SKILL_SIUS_CRASH,//Í¨¹ý 506 1
    RESULT_SKILL_LYCAN_HOWL,//Í¨¹ý 507 2
    RESULT_SKILL_BIRFRONZE_MAGIC,//Í¨¹ý3 508
    RESULT_SKILL_PALUS_MIDDLE_RANGE_MAGIC,//4Í¨¹ý509
    RESULT_SKILL_PALUS_LONG_RANGE_MAGIC,//5Í¨¹ý510
    RESULT_SKILL_GARUM_MAGIC,//Í¨¹ý511 6
    RESULT_SKILL_CLAVIE_MAGIC,//Í¨¹ý512 7
    RESULT_SKILL_Wide_Wild_Wolf,//Í¨¹ý513 8
    RESULT_SKILL_BlessofLycan,//Í¨¹ý514 9
    RESULT_SKILL_SMILE_OF_DEATH,//10Í¨¹ý515
    RESULT_SKILL_FURY_SHOCK,//Í¨11¹ý516
    RESULT_SKILL_RAIL_GUN_ATTACK,//12Í¨¹ý517
//	RESULT_NOT_1, //uu665
    RESULT_SKILL_BLAZE_WALK_2,//518Í¨¹ý13
    RESULT_SKILL_GLADIATOR,//519Í¨¹ý 14
    RESULT_SKILL_CRUSHING_STORM,//520Í¨¹ý15
    RESULT_SKILL_SATELLITE_BOMB_2,//521Í¨¹ý16
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
	RESULT_SKILL_RALLSER_44,//56
	RESULT_SKILL_RALLSER_5,//56
	RESULT_SKILL_RALLSER_6,//56
	RESULT_SKILL_RALLSER_7,//56
	RESULT_SKILL_RALLSER_8,//56
	RESULT_SKILL_MAGMATIC_ERUPTION,
	RESULT_SKILL_201_unknow1,
	RESULT_SKILL__ADV2_HALCYON_MK1,
	RESULT_SKILL_201_unknow2,
	RESULT_SKILL_201_unknow3,
	RESULT_SKILL_ADV2_CHAOS_SPLITTER,
	RESULT_SKILL_201_unknow4,
	RESULT_SKILL_201_unknow5,
	RESULT_SKILL_201_unknow6,
	RESULT_SKILL_201_unknow7,
	RESULT_SKILL_201_unknow8,
	RESULT_SKILL_201_unknow9,
	RESULT_SKILL_201_unknow10,
	RESULT_SKILL_201_unknow11,
	RESULT_SKILL_201_unknow12,
	RESULT_SKILL_201_unknow13,
	RESULT_SKILL_201_unknow14,
	RESULT_SKILL_201_unknow15,
	RESULT_SKILL_201_unknow16,
	RESULT_SKILL_201_unknow17,
	RESULT_SKILL_201_unknow18,
	RESULT_SKILL_201_unknow19,
	RESULT_SKILL_201_unknow20,
	RESULT_SKILL_201_unknow21,
	RESULT_SKILL_201_unknow22,
	RESULT_SKILL_201_unknow23,
	RESULT_SKILL_ADV2_THE_RAGE_OF_GUARDIAN_SPRIT,
	RESULT_SKILL_201_unknow24,
	RESULT_SKILL_201_unknow25,
	RESULT_SKILL_201_unknow26,
	RESULT_SKILL_201_unknow27,
	RESULT_SKILL_201_unknow28,
	RESULT_SKILL_201_unknow29,
	RESULT_SKILL_CARMILRA_BAT_STORM,
	RESULT_SKILL_201_unknow30,
	RESULT_SKILL_201_unknow31,
	RESULT_SKILL_201_unknow32,
    RESULT_SKILL_TRANSFORM_ICE_WAVE,//ÈÕ·þ
	RESULT_SKILL_TRANSFORM_WIDE_ICE_HAIL,//ÈÕ·þ
	RESULT_SKILL_201_unknow33,
	RESULT_SKILL_201_unknow34,
	RESULT_SKILL_TRANSFORM_SMILE_OF_DEATH,//ÈÕ·þ
	RESULT_SKILL_TRANSFORM_FURY_SHOCK,//ÈÕ·þ
	RESULT_SKILL_201_unknow35,
	RESULT_SKILL_MONSTER_TRANSFORM_HOLYSPRIT_FURY,//ÈÕ·þ
	RESULT_SKILL_201_unknow36,
	RESULT_SKILL_201_unknow37,
	RESULT_SKILL_201_unknow38,
	RESULT_SKILL_201_unknow39,
	RESULT_SKILL_GREEN_ULRAYIRY_ANGER,//ÈÕ·þ
	RESULT_SKILL_ULRAYIRY_ANGER_B,//ÈÕ·þ
	RESULT_SKILL_BLUE_ULRAYIRY_ANGER,//ÈÕ·þ
	RESULT_SKILL_ULRAYIRY_ANGER_D,//ÈÕ·þ
	RESULT_SKILL_GREEN_ULRAYIRY_PAIN,//ÈÕ·þ
	RESULT_SKILL_201_unknow40,
	RESULT_SKILL_201_unknow41,
	RESULT_SKILL_201_unknow42,
	RESULT_SKILL_201_unknow43,
	RESULT_SKILL_201_unknow44,
	RESULT_SKILL_201_unknow45,
	RESULT_SKILL_201_unknow46,
	RESULT_SKILL_201_unknow47,
	RESULT_SKILL_201_unknow48,
	RESULT_SKILL_201_unknow49,
	RESULT_SKILL_ATTRACT_CIRCLE,
	RESULT_SKILL_PULSEMAGNETICFIELD,
	RESULT_SKILL_TEMENOS,
	RESULT_SKILL_DEPEND_SOUL,
	RESULT_SKILL_IGNIS_INFERNUM,
	RESULT_SKILL_DEMONIC_RESURRECTIO,
	RESULT_SKILL_WOLF_ROAR,
	RESULT_SKILL_EVGENEIA_BELLUM,
	RESULT_SKILL_201_unknow50,
	RESULT_SKILL_201_unknow51,
    RESULT_SKILL_CRYSTAL_THORN,
	RESULT_SKILL_FAVOR_OF_LUNA,
	RESULT_SKILL_GREEN_ULRAYIRY_PAIN_2,
	RESULT_SKILL_211_221_newhf1,
	RESULT_SKILL_ION_CANNON_ATTACK,
	RESULT_SLILL_BOMBING_VULCAN,
	RESULT_SKILL_AXE_OF_RUIN,
	RESULT_SKILL_EYES_OF_DESTRUCTION,
	RESULT_SKILL_THORN_STRIKE,
	RESULT_SKILL_THORNBUSH_WAVE,
	RESULT_SKILL_CREATURE_GOLEM_QUAKE_ATTACK,
	RESULT_SKILL_CREATURE_GOLEM_HANDWAVE_ATTACK,
	RESULT_SKILL_CREATURE_GOLEM_DEFENSIVE_ENERGEFILED,
	RESULT_SKILL_CREATURE_GOLEM_HANDWAVE_ATTACK2,
    RESULT_SKILL_LAY_WASTE,
	RESULT_SKILL_211_221_newhf2,
	RESULT_SKILL_IMMUNE_DRAIN,
    RESULT_SKILL_211_221_newhf3,
	RESULT_SKILL_LAY_WASTE2,
	RESULT_SKILL_DRAFTED_COMMAND,
	RESULT_SKILL_VOMIT,
	RESULT_SKILL_211_221_newhf4,
	RESULT_SKILL_211_221_newhf5,
	RESULT_SKILL_211_221_newhf6,
	RESULT_SKILL_211_221_newhf7,
	RESULT_SKILL_211_221_newhf8,
	RESULT_SKILL_211_221_newhf9,
	RESULT_SKILL_211_221_newhf10,
	RESULT_SKILL_ADV2_CONTRADICTO,
	RESULT_SKILL_ADV2_CHAIN_LIGHTNING_SWORD,
	RESULT_SKILL_211_221_newhf11,
	RESULT_SKILL_ADV2_CONTRADICTO2,
	RESULT_SKILL_ADV2_DIVINE_TRACES,
	RESULT_SKILL_ADV2_WILD_WOLF_STRIKE,
	RESULT_SKILL_ADV2_DEADLY_STORM,
	RESULT_SKILL_ADV2_SHINING_CHAKRAM_SPIN,
	RESULT_SKILL_ADV2_INFINITY_BLAZE,
	RESULT_SKILL_ADV2_GRACE_OF_ELQUINES,
	RESULT_SKILL_211_221_newhf12,
	RESULT_SKILL_TRANSPORTATION_TELEPORT,
	RESULT_SKILL_211_221_newhf211,
	RESULT_SKILL_HILDEGARD_NORMAL_ATTACK,
	RESULT_SKILL_211_221_newhf2111,
	RESULT_SKILL_211_221_newhf2112,
	RESULT_SKILL_211_221_newhf2113,
	RESULT_SKILL_211_221_newhf2114,
	RESULT_SKILL_211_221_newhf2115,
	RESULT_SKILL_211_221_newhf2116,
	RESULT_SKILL_211_221_newhf2117,
	RESULT_SKILL_211_221_newhf2118,
	RESULT_SKILL_211_221_newhf2119,
	RESULT_SKILL_211_221_newhf2120,
	RESULT_SKILL_211_221_newhf2121,
	RESULT_SKILL_211_221_newhf2122,
	RESULT_SKILL_BLOODY_SKULL_2,
	RESULT_SKILL_ADV2_CHAOS_SPLITTER_2,
	RESULT_SKILL_CRYSTAL_THORN_2,
	RESULT_SKILL_BLIZZARD_2,
	RESULT_SKILL_211_221_newhf21221,
	RESULT_SKILL_CHAKRAM_HAIL_2,
	RESULT_SKILL_211_221_newhf21222,
	RESULT_SKILL_211_221_newhf21223,
	RESULT_SKILL_211_221_newhf21224,
	RESULT_SKILL_211_221_newhf21225,
	RESULT_SKILL_DRAGON_HURRICANE_2,

	RESULT_SKILL_BLADE_ERUPTION,
	RESULT_SKILL_ATTRACT_SWORD_CRASH,
	RESULT_SKILL_STRATEGIC_BOMBER,
	RESULT_SKILL_AURA_EXPLOSION,
	RESULT_SKILL_HOLY_ACIDALIUM,
	RESULT_SKILL_BAT_SPEAR,
	RESULT_SKILL_EXTENSIVE_DELUSION,
	RESULT_SKILL_ACCELERATE_CHARGE,
	RESULT_SKILL_241_1,
	RESULT_SKILL_FROZEN_GROUND,
	RESULT_SKILL_METEORIC_FALL,
	RESULT_SKILL_BLADE_ERUPTION_2,
	RESULT_SKILL_ATTRACT_SWORD_CRASH_2,
	RESULT_SKILL_STRATEGIC_BOMBER_2,
	RESULT_SKILL_AURA_EXPLOSION_2,
	RESULT_SKILL_HOLY_ACIDALIUM_2,
	RESULT_SKILL_BAT_SPEAR_2,
	RESULT_SKILL_EXTENSIVE_DELUSION_2,
	RESULT_SKILL_ACCELERATE_CHARGE_2,
	RESULT_SKILL_241_2,
	RESULT_SKILL_FROZEN_GROUND_2,
	RESULT_SKILL_METEORIC_FALL_2,
	RESULT_SKILL_ADV3_UNKNOW1,
	RESULT_SKILL_ADV3_UNKNOW2,
	RESULT_SKILL_ADV3_UNKNOW3,

	RESULT_SKILL_FRAGRANT_BREATH,
	RESULT_SKILL_TREE_TRUNK_STRIKE,
	RESULT_SKILL_DEATHFUL_BREATH,
	RESULT_SKILL_SPIRIT_OF_DEAD_SICKLE,
	RESULT_SKILL_COME_ON_OVER,
	RESULT_SKILL_EREBUS_IS_CRY,
	RESULT_SKILL_ROLLING_CONTINUE,
	RESULT_SKILL_BITE,
	RESULT_SKILL_THORN_LAUNCH,
	RESULT_SKILL_ONE_TWO_CUT,
	RESULT_SKILL_OVEREAT,
	RESULT_SKILL_THORN_LAUNCH_ADD_DEBUFF,
	RESULT_SKILL_TREE_TRUNK_STRIKE_ADD_DEBUFF,
	RESULT_SKILL_SPIRIT_OF_DEAD_SICKLE_ADD_DEBUFF,
	RESULT_SKILL_POWERFUL_CLAW,
	RESULT_SKILL_ADELANTE_IS_CRY,
	RESULT_SKILL_POTENTIAL_EXPLOSION_2,
	RESULT_SKILL_SHARP_SHIELD_22,
	RESULT_SKILL_SNIPING_2,
	RESULT_SKILL_STUN_SHOT,
	RESULT_SKILL_THORNS_2,
	RESULT_SKILL_CURE_SERIOUS_WOUNDS_2,
	RESULT_SKILL_SET_AFIRE_2,
	RESULT_SKILL_SHADY_DOUPLE_2,

	RESULT_SKILL_HALO_2,
	RESULT_SKILL_RING_OF_FLARE_2,
	RESULT_SKILL_WATER_BARRIER_2,
	RESULT_SKILL_REACTIVE_ARMOR_2,
	RESULT_SKILL_241_newhf11,

	RESULT_SKILL_PLAYING_WITH_FIRE_2,
	RESULT_SKILL_GENISYS_ANGER,
	RESULT_SKILL_DIE_EXPLOSION,
	RESULT_SKILL_CLIENT_SHADY_DOUPLE,//57
	RESULT_SKILL_RALLSER_55,
	RESULT_SKILL_CLIENT_AD2_INFINITY_BLAZE,
	RESULT_SKILL_211_221_newhf13,



	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	
	// ÀÓ½Ã·Î Ãß°¡
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
	RESULT_SKILL_ATTACK_GUN_SR,			// ÃÑ1
	RESULT_SKILL_ATTACK_GUN_SG,			// ÃÑ2
	RESULT_SKILL_ATTACK_GUN_AR,			// ÃÑ3
	RESULT_SKILL_ATTACK_GUN_SMG,		// ÃÑ4
	
	// Sword
	RESULT_SKILL_ATTACK_SWORD,			// Ä®
	RESULT_SKILL_ATTACK_BLADE,			// µµ  ·Ö¸î 1226

	// ÃÑ¿¡ µû¸¥ holy shooting
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

	// Dummy Skill Result - ÀÌÆÑÆ®¸¦ º¸¿©ÁÖÁö ¾Ê°í ½ÃÀü µ¿ÀÛ¸¸ ÀÖ´Â Client SkillÀÌ´Ù.
	RESULT_SKILL_DUMMY_ACTION_MAGIC,
	
	//-----------------------------------------------------------------------------
	// ½ºÅ³ ´Ü°è
	//-----------------------------------------------------------------------------
	RESULT_STEP_SKILL_FLOURISH_2,							// 2´Ü°è FLOURISH
	RESULT_STEP_SKILL_FLOURISH_3,							// 3´Ü°è FLOURISH
	RESULT_STEP_SKILL_SHARP_ROUND_2,						// 2´Ü°è Sharp Round
	RESULT_STEP_SKILL_SHARP_ROUND_3,						// 3´Ü°è Sharp Round
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

	RESULT_STEP_SKILL_COMBO_ATTACK_1,	// ÄÞº¸±â¼ú 1´Ü°è 
	RESULT_STEP_SKILL_COMBO_ATTACK_2,	// ÄÞº¸±â¼ú 2´Ü°è
	RESULT_STEP_SKILL_COMBO_ATTACK_3,	// ÄÞº¸±â¼ú 3´Ü°è
	RESULT_STEP_SKILL_COMBO_ATTACK_4,	// ÄÞº¸±â¼ú 4´Ü°è
	//-----------------------------------------------------------------------------

	// client Àü¿ë
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

	// ¹æ¾î¸· È¿°ú¸¦ ³»±â À§ÇØ¼­..
	REFLECT_AURA_PRISM,				//	
	REFLECT_AURA_SHIELD,				//	
	PROTECTION_FROM_ACID,
	REFLECT_REFLECTION,				//	SKILL_REFLECTION¿¡ ÀÇÇÑ°Í

	// ¹Ù´Ú¿¡ Èê¸®´Â ÇÇ
	BLOOD_DROP_GROUND,
	BLOOD_CRITICAL_HIT,	
	BLOOD_RESURRECT,
	
	// ¾Æ¸¶°Ôµ· 2002.3.4
	ARMAGEDDON_CRASH_1,			// Ã¹¹øÂ° ºÎ¼­Áö´Â°Å
	ARMAGEDDON_CRASH_2,			// µÎ¹øÂ°
	ARMAGEDDON_CRASH_3,			// ¼¼¹øÂ°
	ARMAGEDDON_CRASH_4,			// ¸¶Áö¸· ³²±â
	
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

	ARMEGA_TILE,					// ¾Æ¸£¸Þ°¡
	MIHOLE_TILE,					// ¹ÌÈ£·¹
	KIRO_TILE,						// Å°·Î
	INI_TILE,						// ¾ÆÀÌ´Ï
	GREGORI_TILE,					// ±×¸®°í¸® ÀÌÆåÆ®(Å¸ÀÏ¿¡ ºÙ´Â°Í)
	CONCILIA_TILE,					// ÄÜÄ¥¸®¾Æ
	LEGIOS_TILE,					// ·¹Áö¿À½º
	HILLEL_TILE,					// Èú¸±
	JAVE_TILE,						// Àðºê
	NEMA_TILE,						// ³×¸¶
	AROSA_TILE,						// ¾Æ·Î»ç
	CHASPA_TILE,					// Â÷½ºÆÄ

	HOLY_LAND_WARP_VAMPIRE,
	HOLY_LAND_WARP_SLAYER,

	WARP_BLOOD_BIBLE_TILE,

	AMBER_OF_GUARD_TILE,
	AMBER_OF_AVENGER_TILE,
	AMBER_OF_IMMORTAL_TILE,
	AMBER_OF_CURSE_TILE,

	SKILL_DENIAL_MAGIC_DAMAGED,
	SKILL_DRAGON_FIRE_CRACKER,

	SKILL_FIRE_CRACKER_4,			// Ãß°¡
	
	SKILL_FIRE_CRACKER_VOLLEY_1,		// 3¿¬¹ß ÆøÁ×-_-
	SKILL_FIRE_CRACKER_VOLLEY_2,		// 3¿¬¹ß ÆøÁ×-_-
	SKILL_FIRE_CRACKER_VOLLEY_3,		// 3¿¬¹ß ÆøÁ×-_-
	SKILL_FIRE_CRACKER_VOLLEY_4,		// 3¿¬¹ß ÆøÁ×-_-
	
	SKILL_FIRE_CRACKER_WIDE_VOLLEY_1,	// 3¿¬¹ß ±¤¿ª ÆøÁ×
	SKILL_FIRE_CRACKER_WIDE_VOLLEY_2,	// 3¿¬¹ß ±¤¿ª ÆøÁ×
	SKILL_FIRE_CRACKER_WIDE_VOLLEY_3,	// 3¿¬¹ß ±¤¿ª ÆøÁ×
	SKILL_FIRE_CRACKER_WIDE_VOLLEY_4,	// 3¿¬¹ß ±¤¿ª ÆøÁ×

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
	
	REGEN_TOWER_TRING_POSITION,			// ¸®Á¨Á¸ Å¸¿ö ÀïÅ» Å¸ÀÏ
	REGEN_TOWER_THROW_LIGHT,			// ¸®Á¨Á¸ ºû ºñÄ¡´Â°Å 
	REGEN_TOWER_TRING_POSITION_LIGHT,	// ¸®Á¨Á¸ Å¸¿ö ÀïÅ» Å¸ÀÏ¿¡ ¿Ã¶úÀ»¶§
	REGEN_TOWER_FLASH_BEAD,				// ¸®Á¨Á¸ Å¸¿ö ±¸½½ ¹ÝÂ¦°Å¸®´Â°Å.

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


	// 2004,3,6 sobeit add start - Æê ½ºÅ³
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
	// 2004,3,6 sobeit add end - Æê ½ºÅ³

	// 2004,3,22 sobeit add start - °ËÀº ±â¿î
	MAP_BLACK_LARGE_SMOKE,
	MAP_BLACK_HALL,
	MAP_BLACK_SMALL_SMOKE_1,
	MAP_BLACK_SMALL_SMOKE_2,
	MAP_BLACK_SMALL_SMOKE_3,

	MAP_ELECTRIC_POST,
	MAP_GHOST_1,
	MAP_GHOST_2,
	// 2004,3,22 sobeit add end - °ËÀº ±â¿î
	
	// 2004, 3, 30 sobeit add start - ¿ï¹ö¸° ³ª¿Ã¶§ ÀÌÆåÆ®
	RESULT_SUMMON_ULBERINE,
	// 2004, 3, 30 sobeit add end - ¿ï¹ö¸° ³ª¿Ã¶§ ÀÌÆåÆ®

	// 2004, 4,2  sobeit add start - Çà¿îÀÇ ¿À·Î¶ó(³×ÀÙ Å¬·Î¹Ù)
	RESULT_LUCKY_AURA,
	RESULT_MISFORTUNE_AURA,
	// 2004, 4,2 sobeit add end - Çà¿îÀÇ ¿À·Î¶ó(³×ÀÙ Å¬·Î¹Ù)

	// 2004, 5, 6, sobeit add start
	AMBER_OF_LIFE_TILE,
	AMBER_OF_LIGHT_TILE,
	// 2004, 5, 6, sobeit add end

	// 2004, 5, 18 sobeit add start
	SKILL_CLIENT_TRAP_ICICLE_DROP,					// ¶³¾îÁö´Â ¾óÀ½ Æ®·¦
	SKILL_CLIENT_TRAP_ICICLE_DROP_SIGN,

	SKILL_CLIENT_TRAP_ICICLE_AUGER,
	SKILL_CLIENT_TRAP_ICICLE_AUGER_SIGN,

	SKILL_CLIENT_TRAP_ICE_STICK_SW,		// ÁÂÇÏ
	SKILL_CLIENT_TRAP_ICE_STICK_SW_SIGN,// ÁÂÇÏ
	SKILL_CLIENT_TRAP_ICE_STICK_SE,		// ¿ìÇÏ
	SKILL_CLIENT_TRAP_ICE_STICK_SE_SIGN,// ¿ìÇÏ
	SKILL_CLIENT_TRAP_ICE_STICK_NE,		// ¿ì»ó
	SKILL_CLIENT_TRAP_ICE_STICK_NE_SIGN,// ¿ì»ó
	SKILL_CLIENT_TRAP_ICE_STICK_NW,		// ÁÂ»ó
	SKILL_CLIENT_TRAP_ICE_STICK_NW_SIGN,// ÁÂ»ó

	SKILL_CLIENT_TRAP_ICICLE_LARGE_AUGER,
	SKILL_CLIENT_TRAP_ICICLE_LARGE_AUGER_SIGN,
	// 2004, 5, 18 sobeit add start
	

	// 2004, 6, 8 sobeit add start - Áúµå·¹ ÁÖº¯ ÀÌÆåÆ®
//	SKILL_CLIENT_SUMMON_GDR, 
//    SKILL_CLIENT_GDR_SATELLITE,
//    SKILL_CLIENT_GDR_FLOATING, 
//
//	SKILL_CLIENT_SLAYER_MASTER_130,
//	SKILL_CLIENT_SLAYER_MASTER_150,
//	SKILL_CLIENT_VAMPIRE_MASTER_130,
//	SKILL_CLIENT_VAMPIRE_MASTER_150, // 150 ·¹º§ ¸¶½ºÅÍ ÀÌÆåÆ®
//	SKILL_CLIENT_OUSTERS_MASTER_130,
//	SKILL_CLIENT_OUSTERS_MASTER_150,
	// 2004, 6, 8 sobeit add end

	// 2004, 6, 15 sobeit add start - gdr skill - glacier
	SKILL_CLIENT_GDR_GLACIER_SLAYER,
	SKILL_CLIENT_GDR_GLACIER_VAMPIRE,
	SKILL_CLIENT_GDR_GLACIER_OUSTERS,
	// 2004, 6, 15 sobeit add end - gdr skill - glacier

	// 2004, 6, 26 sobeit add start
	MAP_GDR_LAIR_POTAL,	// Áúµå·¹ Æ÷Å» ÀÌÆåÆ®
	// 2004, 6, 26 sobeit add end

	// 2004, 8, 5 sobeit add start
	SKILL_CLIENT_CASTLE_GATE_DEAD, // ¼º¹® ºÎ¼­Áú¶§(Á×À»¶§) Å« ¸ÕÁöÀÌÆåÆ®
	SKILL_CLIENT_CASTLE_GATE_DAMAGED,// ¼º¹® Å¸°Ý ¹ÞÀ» ¶§
	SKILL_CLIENT_GUILD_TOWER_DEAD, // ±æµåÅ¸¿ö ºÎ¼­Áú¶§(Á×À»¶§) Å« ¸ÕÁöÀÌÆåÆ®
	SKILL_CLIENT_GUILD_TOWER_PROTECT, // ±æµåÅ¸¿ö º¸È£ ÀÌÆåÆ®
	// 2004, 8, 5 sobeit add end

	// 2004, 8, 31, sobeit add start
	SKILL_CLIENT_LAND_MINE_LIGHT,		// ·£µå¸¶ÀÎ¿¡ °è¼Ó ºÙ¾î ÀÖ´Â ÀÌÆåÆ®
	SKILL_CLIENT_PLEASUER_STATION_LIGHT,// ÇÁ·¹Á® ½ºÅ×ÀÌ¼Ç¿¡ °è¼Ó ºÙ¾î ÀÖ´Â ÀÌÆåÆ®
	// 2004, 8, 31, sobeit add end

	// 2004, 9, 2, sobeit add start
	SKILL_CLIENT_TURRET_LASER,					// ÅÍ·¿ ·¹ÀÌÀú 1Å¸ÀÏ
	SKILL_CLIENT_TURRET_LASER_ATTACK,           // ÅÍ·¿ ·¹ÀÌÀú °ø°Ý 1Å¸ÀÏ (°ø°ÝÃø¿¡µµ º¸ÀÓ) 
	// 2004, 9, 2, sobeit add end

	// 2004, 9, 6, sobeit add start - Áúµå·¹ Á×±â ÀÌÆåÆ® , ¸®ÇªÅä ÆåÅÒ Å¸°Ý
	SKILL_CLIENT_GDR_DEAD,
	SKILL_CLIENT_HIT_REPUTO_FACTUM,
	// 2004, 9, 6, sobeit add end - Áúµå·¹ Á×±â ÀÌÆåÆ®

	SKILL_CLIENT_SWEEP_VICE,// ÀÎÃ¦ ½ºÀ¬ ¹ÙÀÌ½º 1´Ü°è
	SKILL_CLIENT_SWORD_OF_THOR, // ¼Òµå ¿Àºê Åä¸£ Áö¼Ó µÇ´Â°Å

	SKILL_CLIENT_BURNING_SOL_1, // µµ½½ ¹ö´×¼Ö 1´Ü°è
	SKILL_CLIENT_BURNING_SOL_2, // µµ½½ ¹ö´×¼Ö 2´Ü°è
	SKILL_CLIENT_BURNING_SOL_3, // µµ½½ ¹ö´×¼Ö 3´Ü°è
	SKILL_CLIENT_BURNING_SOL_4, // µµ½½ ¹ö´×¼Ö 4´Ü°è
	RESULT_CLIENT_BURNING_SOL_1, // µµ½½ ¹ö´×¼Ö 1´Ü°è
	RESULT_CLIENT_BURNING_SOL_2, // µµ½½ ¹ö´×¼Ö 2´Ü°è
	RESULT_CLIENT_BURNING_SOL_3, // µµ½½ ¹ö´×¼Ö 3´Ü°è
	RESULT_CLIENT_BURNING_SOL_4, // µµ½½ ¹ö´×¼Ö 4´Ü°è
	
	// 2004, 10, 21, sobeit add start - ±×·¹ÀÌÆ® ·¯ÇÇ¾ð °ü·Ã
	SKILL_CLIENT_GREAT_RUFFIAN_ALIVE, // 2Â÷ »ý¼ºµÉ¶§ ÀÌÆåÆ®
	SKILL_CLIENT_GREAT_RUFFIAN_BODY, // ¸ö¿¡ ºÙ´Â ÀÌÆåÆ®
	SKILL_CLIENT_GREAT_RUFFIAN_DIE, // Á×À»¶§ ³ª¿À´Â ÀÌÆåÆ®
	// 2004, 10, 21, sobeit add end

	// 2004, 11, 11, sobeit add start - ÃÑ 140 ÅÍÁö´Â°Å
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

	SKILL_CLIENT_CANDLE, // ÃÐºÒ ÀÌÆåÆ®
	
	SKILL_CLIENT_ADVANCEMENT_SUMMON_SYLPH_START,
	SKILL_CLIENT_ADVANCEMENT_SUMMON_SYLPH_END,

	SKILL_CLIENT_QUEST_MAGIC_ZONE,
	SKILL_CLIENT_QUEST_MAGIC_ZONE_POTAL,

	// 2004, 10, 15, sobeit add start - Å×½ºÆ®¿ë
	TEMP_SKILL_INFINITY_THUNDERBLOT, // °Ë½½ 140
	TEMP_SKILL_SPIT_STREAM, // µµ½½ 140
	TEMP_SKILL_PLASMA_ROCKET_LAUNCHER, //ÃÑ 140
	TEMP_SKILL_INTIMATE_GRAIL, // ÀÎÃ¾ 140
	TEMP_SKILL_BOMBING_STAR, // Èú 140,
	TEMP_SKILL_SET_AFIRE, // ÀüÅõ¹ì 140
	TEMP_SKILL_NOOSE_OF_WRAITH, // ¸¶¹ý¹ì 140

	RESULT_TEMP_SKILL_INFINITY_THUNDERBLOT, // °Ë½½ 140
	RESULT_TEMP_SKILL_SPIT_STREAM, // µµ½½ 140
	RESULT_TEMP_SKILL_PLASMA_ROCKET_LAUNCHER, //ÃÑ 140
	RESULT_TEMP_SKILL_INTIMATE_GRAIL, // ÀÎÃ¾ 140
	RESULT_TEMP_SKILL_BOMBING_STAR, // Èú 140,
	RESULT_TEMP_SKILL_SET_AFIRE, // ÀüÅõ¹ì 140
	RESULT_TEMP_SKILL_NOOSE_OF_WRAITH, // ¸¶¹ý¹ì 140
	// 2004, 10, 15, sobeit add end - Å×½ºÆ®¿ë
	
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
	//¾Æ¿ì½ºÅÍÁî ½ÂÁ÷ ÀÌµ¿ ¼ö´Ü ÀÌÆåÆ® ½ºÅ×ÀÌÅÍ½º¿¡´Â wispÀ¸·Î °Ë»ö sprite´Â ousters_fastmove¿Í SUMMON_WISPÀÓ
	//by diesirace 20070312
	SKILL_CLIENT_ADVANCEMENT_SUMMON_SYLPH_RED_START, 
	SKILL_CLIENT_ADVANCEMENT_SUMMON_SYLPH_RED_END,
	SKILL_CLIENT_ADVANCEMENT_SUMMON_SYLPH_YELLOW_START, 
	SKILL_CLIENT_ADVANCEMENT_SUMMON_SYLPH_YELLOW_END,
	SKILL_CLIENT_ADVANCEMENT_SUMMON_SYLPH_GREEN_START, 
	SKILL_CLIENT_ADVANCEMENT_SUMMON_SYLPH_GREEN_END,
	// end
	
	//Add Begin - 2007.07.30 by chyaya
	RESULT_SUMMON_LUPENG,	// ±«µµ ·çÆë ¼ÒÈ¯
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
	SKILL_CLIENT_OUSTERS_UNICORN,		// ¹ÌÁ¤
	SKILL_CLIENT_SLAYER_HOVER_VEHICLE,	// ¹ÌÁ¤
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
//	SKILL_CLIENT_CATS_SUMMON,
//#endif //__PET_CATS_UPGRADED
    SKILL_CLIENT_RUFFIAN_SUMMON,
    RESULT_SKILL_CHARGING_POWER2,
    SKILL_CLIENT_BOOST,
	// max
   SKILL_CLIENT_ADV2_MAGMATIC_ERUPTION,
   SKILL_CLIENT_DUCKING_WALLOP2,
   SKILL_CLIENT_201_BOOT_UNKNOW1,
   SKILL_CLIENT_201_BOOT_UNKNOW2,
   SKILL_SKILL_GORE_GLAND_FIRE,
   SKILL_SKILL_GORE_GLAND_FIRE2,
   SKILL_CLIENT_201_BOOT_UNKNOW3,
   SKILL_CLIENT_201_BOOT_UNKNOW4,
   SKILL_CLIENT_201_BOOT_UNKNOW5,
   SKILL_CLIENT_201_BOOT_UNKNOW6,
   SKILL_CLIENT_201_BOOT_UNKNOW7,
   SKILL_CLIENT_201_BOOT_UNKNOW8,
   SKILL_CLIENT_201_BOOT_UNKNOW9,
   SKILL_CLIENT_201_BOOT_UNKNOW10,
   SKILL_CLIENT_201_BOOT_UNKNOW11,
   SKILL_CLIENT_201_BOOT_UNKNOW12,
   SKILL_CLIENT_201_BOOT_UNKNOW13,
   SKILL_CLIENT_201_BOOT_UNKNOW14,
   SKILL_CLIENT_201_BOOT_UNKNOW15,
   SKILL_CLIENT_201_BOOT_UNKNOW16,//ÈÕ·þ
   SKILL_CLIENT_201_BOOT_UNKNOW17,//ÈÕ·þ
   SKILL_CLIENT_201_BOOT_UNKNOW18,//ÈÕ·þ
   SKILL_CLIENT_201_BOOT_UNKNOW19,//ÈÕ·þ
   SKILL_CLIENT_201_BOOT_UNKNOW20,//ÈÕ·þ
   SKILL_CLIENT_201_BOOT_UNKNOW21,//ÈÕ·þ
   SKILL_CLIENT_MONSTER_TRANSFORM_HOLYSPRIT_SWORD,//ÈÕ·þ
	SKILL_CLIENT_201_BOOT_UNKNOW22,
	SKILL_CLIENT_201_BOOT_UNKNOW23,
	SKILL_CLIENT_201_BOOT_UNKNOW24,
	SKILL_CLIENT_201_BOOT_UNKNOW25,
	SKILL_CLIENT_201_BOOT_UNKNOW26,
	SKILL_CLIENT_201_BOOT_UNKNOW27,
	SKILL_CLIENT_201_BOOT_UNKNOW28,
	SKILL_CLIENT_201_BOOT_UNKNOW29,
	SKILL_CLIENT_201_BOOT_UNKNOW30,
	SKILL_CLIENT_201_BOOT_UNKNOW31,
	SKILL_CLIENT_201_BOOT_UNKNOW32,
	SKILL_CLIENT_201_BOOT_UNKNOW33,
	SKILL_CLIENT_201_BOOT_UNKNOW34,
	SKILL_CLIENT_201_BOOT_UNKNOW35,
	SKILL_CLIENT_201_BOOT_UNKNOW36,
	SKILL_CLIENT_201_BOOT_UNKNOW37,
	SKILL_CLIENT_201_BOOT_UNKNOW38,
	SKILL_CLIENT_201_BOOT_UNKNOW39,
	SKILL_CLIENT_201_BOOT_UNKNOW40,
	SKILL_CLIENT_201_BOOT_UNKNOW41,
	SKILL_CLIENT_ATTRACT_CIRCLE,
	SKILL_CLIENT_201_BOOT_UNKNOW42,
	SKILL_CLIENT_201_BOOT_UNKNOW43,
	SKILL_CILENT_DEPEND_SOUL,
    SKILL_CLIENT_201_BOOT_UNKNOW44,
	SKILL_CLIENT_DEMONIC_RESURRECTIO,
	SKILL_CLIENT_201_BOOT_UNKNOW45,
	SKILL_CLIENT_201_BOOT_UNKNOW46,
	SKILL_CLIENT_OUTRAGE_BOOSTER,
	SKILL_CLIENT_201_BOOT_UNKNOW47,
	SKILL_CLIENT_201_BOOT_UNKNOW48,
	SKILL_CLIENT_201_BOOT_UNKNOW49,
	RUNE_ADD_SUCCESS,
    RUNE_REMOVE_SUCCESS,
	RUNE_MIX_SUCCESS,
	SKILL_CLIENT_SIEGE_TOWER_DIE,
	SKILL_CLIENT_NOSPERATUS_DIE,
	MAGIC_CLIENT_NEWREINFORCE_OPTION_NULL,
	MAGIC_CLIENT_NEWREINFORCE_OPTION_PLUS,
	SKILL_CLIENT_ATTRACT_SWORD_CRASH,
	SKILL_CLIENT_ATTRACT_SWORD_CRASH_2,
	SKILL_CLIENT_STRATEGIC_BOMBER_EXPLOSION,
	SKILL_CLIENT_STRATEGIC_BOMBER_2_EXPLOSION,
	SKILL_CLIENT_ACCELERATE_CHARGE,
	SKILL_CLIENT_ACCELERATE_CHARGE_2,
	SKILL_CLIENT_WARP_PORTAL_SLAYER_MALE,
	SKILL_CLIENT_WARP_PORTAL_SLAYER_FEMALE,
	SKILL_CLIENT_HALO_2_STOP,
	SKILL_CLIENT_HALO_2_ATTACK,
	RESULT_SKILL_CLIENT_SHADY_DOUPLE_2,
	SKILL_CLIENT_GENESIS_PLAYING_WITH_FIRE_2,
	MAX_ACTIONINFO
};


// MAX_ACTIONINFO
#define		MIN_RESULT_ACTIONINFO		RESULT_SKILL_ATTACK_MELEE
#define		MAX_RESULT_ACTIONINFO		RESULT_SKILL_ATTACK_BLADE
#define		MIN_RESULT_STEP_ACTIONINFO	STEP_SKILL_FLOURISH_2

#endif