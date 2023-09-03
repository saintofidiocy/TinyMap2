#define SCENARIO_CHK "staredit\\scenario.chk"


// Save Modes
#define MODE_MELEE_SIMPLE   0x01
#define MODE_MELEE_EXTENDED 0x02
#define MODE_UMS_SIMPLESC   0x04
#define MODE_UMS_SIMPLEEX   0x08
#define MODE_UMS_STARCRAFT  0x10
#define MODE_UMS_BROODWAR   0x20
#define MODE_UMS_EXTENDED   0x40
#define MODE_REMASTERED     0x80

#define GAMETYPE_MELEE      0
#define GAMETYPE_UMS        1

#define COMPAT_STARCRAFT    0
#define COMPAT_BROODWAR     1
#define COMPAT_HYBRID       2

// MPQ File Types
#define MPQ_FILETYPE_LISTFILE   0
#define MPQ_FILETYPE_CHK        1
#define MPQ_FILETYPE_WAV        2
#define MPQ_FILETYPE_OGG        3
#define MPQ_FILETYPE_UNKNOWN    4
#define MPQ_FILETYPE_COUNT      5

// MPQ File Compressions
#define MPQ_COMP_DO_NOT_ADD     0
#define MPQ_COMP_NONE           1
#define MPQ_COMP_TYPE1          2
#define MPQ_COMP_TYPE2          3
#define MPQ_COMP_WAV_LOW        2
#define MPQ_COMP_WAV_MED        3
#define MPQ_COMP_WAV_HIGH       4

#define COMPWAV_OFF             0
#define COMPWAV_COMP            1
#define COMPWAV_CONV            2

// MPQ File Flags
#define MPQ_FLAGS_DO_NOT_ADD     0x80000000
#define MPQ_FLAGS_IS_CHK         ((MPQ_FILETYPE_CHK)<<24)
#define MPQ_FLAGS_IS_WAV         ((MPQ_FILETYPE_WAV)<<24)
#define MPQ_FLAGS_IS_OGG         ((MPQ_FILETYPE_OGG)<<24)
#define MPQ_FLAGS_IS_WAV_OR_OGG  MPQ_FLAGS_IS_WAV
#define MPQ_FLAGS_IS_FILE        ((MPQ_FILETYPE_LISTFILE)<<24)
#define MPQ_FLAGS_WAV_COMP       ((COMPWAV_COMP)<<16)
#define MPQ_FLAGS_WAV_CONV       ((COMPWAV_CONV)<<16)
#define MPQ_FLAGS_WAV_LOW        (MAWA_QUALITY_LOW + 1)
#define MPQ_FLAGS_WAV_MED        (MAWA_QUALITY_MEDIUM + 1)
#define MPQ_FLAGS_WAV_HIGH       (MAWA_QUALITY_HIGH + 1)
#define MPQ_FLAGS_FILE_TYPE1     (MAFA_COMPRESS)
#define MPQ_FLAGS_FILE_TYPE2     (MAFA_COMPRESS2)

#define MPQ_FLAGS_TYPE_MASK      0x7F000000
#define MPQ_FLAGS_COMP_MASK      0x0000FFFF

// Units.dat flags
#define UNITSDAT_ABIL_BUILDING          0x00000001
#define UNITSDAT_ABIL_ADDON             0x00000002
#define UNITSDAT_ABIL_FLYER             0x00000004
#define UNITSDAT_ABIL_RESOURCEMINER     0x00000008
#define UNITSDAT_ABIL_SUBUNIT           0x00000010
#define UNITSDAT_ABIL_FLYINGBUILDING    0x00000020
#define UNITSDAT_ABIL_HERO              0x00000040
#define UNITSDAT_ABIL_REGENERATE        0x00000080
#define UNITSDAT_ABIL_ANIMIDLE          0x00000100
#define UNITSDAT_ABIL_CLOAKABLE         0x00000200
#define UNITSDAT_ABIL_2UNIT1EGG         0x00000400
#define UNITSDAT_ABIL_SINGLEENTITY      0x00000800
#define UNITSDAT_ABIL_RESOURCEDEPOT     0x00001000
#define UNITSDAT_ABIL_RESOURCECONTAINER 0x00002000
#define UNITSDAT_ABIL_ROBOTIC           0x00004000
#define UNITSDAT_ABIL_DETECTOR          0x00008000
#define UNITSDAT_ABIL_ORGANIC           0x00010000
#define UNITSDAT_ABIL_REQUIRESCREEP     0x00020000
#define UNITSDAT_ABIL_UNUSED            0x00040000
#define UNITSDAT_ABIL_REQUIRESPSI       0x00080000
#define UNITSDAT_ABIL_BURROWABLE        0x00100000
#define UNITSDAT_ABIL_SPELLCASTER       0x00200000
#define UNITSDAT_ABIL_CLOAKED           0x00400000
#define UNITSDAT_ABIL_PICKUPITEM        0x00800000
#define UNITSDAT_ABIL_IGNORESUPPLYCHECK 0x01000000
#define UNITSDAT_ABIL_MEDIUMOVERLAY     0x02000000
#define UNITSDAT_ABIL_LARGEOVERLAY      0x04000000
#define UNITSDAT_ABIL_BATTLEREACTIONS   0x08000000
#define UNITSDAT_ABIL_AUTOATTACK        0x10000000
#define UNITSDAT_ABIL_INVINCIBLE        0x20000000
#define UNITSDAT_ABIL_MECHANICAL        0x40000000
#define UNITSDAT_ABIL_PRODUCESUNITS     0x80000000

#define UNITSDAT_AVAIL_NONNEUTRAL           0x0001
#define UNITSDAT_AVAIL_UNITLISTING          0x0002
#define UNITSDAT_AVAIL_MISSIONBRIEFING      0x0004
#define UNITSDAT_AVAIL_PLAYERSETTINGS       0x0008
#define UNITSDAT_AVAIL_ALLRACES             0x0010
#define UNITSDAT_AVAIL_SETDOODADSTATE       0x0020
#define UNITSDAT_AVAIL_NONLOCATIONTRIGGERS  0x0040
#define UNITSDAT_AVAIL_UNITANDHEROSETTINGS  0x0080
#define UNITSDAT_AVAIL_LOCATIONTRIGGERS     0x0100
#define UNITSDAT_AVAIL_BROODWAR             0x0200

#define UNITSDAT_GROUP_ZERG                 0x01
#define UNITSDAT_GROUP_TERRAN               0x02
#define UNITSDAT_GROUP_PROTOSS              0x04
#define UNITSDAT_GROUP_MEN                  0x08
#define UNITSDAT_GROUP_BUILDING             0x10
#define UNITSDAT_GROUP_FACTORY              0x20
#define UNITSDAT_GROUP_INDEPENDANT          0x40
#define UNITSDAT_GROUP_NEUTRAL              0x80


// CHK Flags
#define UNIT_PROP_CLOAK                    0x00000001
#define UNIT_PROP_BURROW                   0x00000002
#define UNIT_PROP_TRANSIT                  0x00000004
#define UNIT_PROP_HALLUCINATED             0x00000008
#define UNIT_PROP_INVINCIBLE               0x00000010
#define UNIT_PROP_PLAYER                   0x00010000
#define UNIT_PROP_HP                       0x00020000
#define UNIT_PROP_SHIELDS                  0x00040000
#define UNIT_PROP_ENERGY                   0x00080000
#define UNIT_PROP_RESOURCES                0x00100000
#define UNIT_PROP_HANGAR                   0x00200000
#define UNIT_REL_ADDON                     0x0200
#define UNIT_REL_NYDUS                     0x0400

#define THG2_FLAG_SPRITE                   0x1000
#define THG2_FLAG_DISABLED                 0x8000

#define SPRITE_SCOURGE                     130
#define SPRITE_EGG_SPAWN                   319
#define SPRITE_LURKER_EGG                  481

#define MRGN_FLAG_MASK                     0x003F

#define UNIT_TYPE_COUNT                    228


#define FOURCC(a,b,c,d) (u32)((a)|((b)<<8)|((c)<<16)|((d)<<24))

// Section names
#define SECT_TYPE FOURCC('T','Y','P','E')
#define SECT_VER  FOURCC('V','E','R',' ')
#define SECT_VCOD FOURCC('V','C','O','D')
#define SECT_OWNR FOURCC('O','W','N','R')
#define SECT_ERA  FOURCC('E','R','A',' ')
#define SECT_DIM  FOURCC('D','I','M',' ')
#define SECT_SIDE FOURCC('S','I','D','E')
#define SECT_MTXM FOURCC('M','T','X','M')
#define SECT_PUNI FOURCC('P','U','N','I')
#define SECT_UPGR FOURCC('U','P','G','R')
#define SECT_PTEC FOURCC('P','T','E','C')
#define SECT_UNIT FOURCC('U','N','I','T')
#define SECT_THG2 FOURCC('T','H','G','2')
#define SECT_MASK FOURCC('M','A','S','K')
#define SECT_STR  FOURCC('S','T','R',' ')
#define SECT_UPRP FOURCC('U','P','R','P')
#define SECT_MRGN FOURCC('M','R','G','N')
#define SECT_TRIG FOURCC('T','R','I','G')
#define SECT_MBRF FOURCC('M','B','R','F')
#define SECT_SPRP FOURCC('S','P','R','P')
#define SECT_FORC FOURCC('F','O','R','C')
#define SECT_UNIS FOURCC('U','N','I','S')
#define SECT_UPGS FOURCC('U','P','G','S')
#define SECT_TECS FOURCC('T','E','C','S')
#define SECT_COLR FOURCC('C','O','L','R')
#define SECT_PUPx FOURCC('P','U','P','x')
#define SECT_PTEx FOURCC('P','T','E','x')
#define SECT_UNIx FOURCC('U','N','I','x')
#define SECT_UPGx FOURCC('U','P','G','x')
#define SECT_TECx FOURCC('T','E','C','x')

// SC:R Sections
#define SECT_STRx FOURCC('S','T','R','x')
#define SECT_CRGB FOURCC('C','R','G','B')

#define SECT_JUMP 0 //FOURCC('J','U','M','P')

// Section IDs (used in arrays)
enum {
  SECT_TYPE_ID,
  SECT_VER_ID,
  SECT_VCOD_ID,
  SECT_OWNR_ID,
  SECT_ERA_ID,
  SECT_DIM_ID,
  SECT_SIDE_ID,
  SECT_MTXM_ID,
  SECT_PUNI_ID,
  SECT_UPGR_ID,
  SECT_PTEC_ID,
  SECT_UNIT_ID,
  SECT_THG2_ID,
  SECT_MASK_ID,
  SECT_STR_ID,
  SECT_UPRP_ID,
  SECT_MRGN_ID,
  SECT_TRIG_ID,
  SECT_MBRF_ID,
  SECT_SPRP_ID,
  SECT_FORC_ID,
  SECT_UNIS_ID,
  SECT_UPGS_ID,
  SECT_TECS_ID,
  SECT_COLR_ID,
  SECT_PUPx_ID,
  SECT_PTEx_ID,
  SECT_UNIx_ID,
  SECT_UPGx_ID,
  SECT_TECx_ID,
  SECT_STRx_ID,
  SECT_CRGB_ID,
  SECTION_COUNT
};

// Map Types
#define TYPE_RAWS FOURCC('R','A','W','S')
#define TYPE_RAWB FOURCC('R','A','W','B')

// Map Versions
#define VER_STARCRAFT                        0x3B
#define VER_HYBRID                           0x3F
#define VER_HYBRID_RM                        0x40
#define VER_BROODWAR                         0xCD
#define VER_BROODWAR_RM                      0xCE

// SIDE Races
#define SIDE_ZERG                            0
#define SIDE_TERRAN                          1
#define SIDE_PROTOSS                         2
#define SIDE_USER_SELECTABLE                 5
#define SIDE_RANDOM                          6
#define SIDE_INACTIVE                        7

// OWNR Controller
#define OWNR_INACTIVE                        0
#define OWNR_RESCUABLE                       3
#define OWNR_COMPUTER                        5
#define OWNR_HUMAN                           6
#define OWNR_NEUTRAL                         7

// FORC flags
#define FORC_RANDOM_START_LOCATIONS          0x01
#define FORC_ALLIES                          0x02
#define FORC_ALLIED_VICTORY                  0x04
#define FORC_SHARED_VISION                   0x08
#define FORC_FLAGS_MASK                      0x0F

// CRGB properties
#define CRGB_RANDOM                          0
#define CRGB_USER_SELECTABLE                 1
#define CRGB_RGB                             2
#define CRGB_COLOR_ID                        3

// Conditions
#define COND_NO_CONDITION                    0
#define COND_COUNTDOWN_TIMER                 1
#define COND_COMMAND                         2
#define COND_BRING                           3
#define COND_ACCUMULATE                      4
#define COND_KILL                            5
#define COND_COMMAND_THE_MOST                6
#define COND_COMMAND_THE_MOST_AT             7
#define COND_MOST_KILLS                      8
#define COND_HIGHEST_SCORE                   9
#define COND_MOST_RESOURCES                  10
#define COND_SWITCH                          11
#define COND_ELAPSED_TIME                    12
#define COND_MISSION_BRIEFING                13
#define COND_OPPONENTS                       14
#define COND_DEATHS                          15
#define COND_COMMAND_THE_LEAST               16
#define COND_COMMAND_THE_LEAST_AT            17
#define COND_LEAST_KILLS                     18
#define COND_LOWEST_SCORE                    19
#define COND_LEAST_RESOURCES                 20
#define COND_SCORE                           21
#define COND_ALWAYS                          22
#define COND_NEVER                           23


// Actions
#define ACT_NO_ACTION                        0
#define ACT_VICTORY                          1
#define ACT_DEFEAT                           2
#define ACT_PRESERVE_TRIGGER                 3
#define ACT_WAIT                             4
#define ACT_PAUSE_GAME                       5
#define ACT_UNPAUSE_GAME                     6
#define ACT_TRANSMISSION                     7
#define ACT_PLAY_WAV                         8
#define ACT_DISPLAY_TEXT_MESSAGE             9
#define ACT_CENTER_VIEW                      10
#define ACT_CREATE_UNITS_WITH_PROPERTIES     11
#define ACT_SET_MISSION_OBJECTIVES           12
#define ACT_SET_SWITCH                       13
#define ACT_SET_COUNTDOWN_TIMER              14
#define ACT_RUN_AI_SCRIPT                    15
#define ACT_RUN_AI_SCRIPT_AT                 16
#define ACT_LEADERBOARD_CONTROL              17
#define ACT_LEADERBOARD_CONTROL_AT           18
#define ACT_LEADERBOARD_RESOURCES            19
#define ACT_LEADERBOARD_KILLS                20
#define ACT_LEADERBOARD_POINTS               21
#define ACT_KILL_UNIT                        22
#define ACT_KILL_UNITS_AT                    23
#define ACT_REMOVE_UNIT                      24
#define ACT_REMOVE_UNITS_AT                  25
#define ACT_SET_RESOURCES                    26
#define ACT_SET_SCORE                        27
#define ACT_MINIMAP_PING                     28
#define ACT_TALKING_PORTRAIT                 29
#define ACT_MUTE_UNIT_SPEECH                 30
#define ACT_UNMUTE_UNIT_SPEECH               31
#define ACT_LEADERBOARD_COMPUTER_PLAYERS     32
#define ACT_LEADERBOARD_GOAL_CONTROL         33
#define ACT_LEADERBOARD_GOAL_CONTROL_AT      34
#define ACT_LEADERBOARD_GOAL_RESOURCES       35
#define ACT_LEADERBOARD_GOAL_KILLS           36
#define ACT_LEADERBOARD_GOAL_POINTS          37
#define ACT_MOVE_LOCATION                    38
#define ACT_MOVE_UNITS                       39
#define ACT_LEADERBOARD_GREED                40
#define ACT_SET_NEXT_SCENARIO                41
#define ACT_SET_DOODAD_STATE                 42
#define ACT_SET_INVINCIBILITY                43
#define ACT_CREATE_UNITS                     44
#define ACT_SET_DEATHS                       45
#define ACT_ORDER                            46
#define ACT_COMMENT                          47
#define ACT_GIVE_UNITS_TO_PLAYER             48
#define ACT_MODIFY_UNIT_HIT_POINTS           49
#define ACT_MODIFY_UNIT_ENERGY               50
#define ACT_MODIFY_UNIT_SHIELD_POINTS        51
#define ACT_MODIFY_UNIT_RESOURCE_AMOUNT      52
#define ACT_MODIFY_UNIT_HANGAR_COUNT         53
#define ACT_PAUSE_TIMER                      54
#define ACT_UNPAUSE_TIMER                    55
#define ACT_DRAW                             56
#define ACT_SET_ALLIANCE_STATUS              57
#define ACT_DISABLE_DEBUG_MODE               58
#define ACT_ENABLE_DEBUG_MODE                59

// Briefing Actions
#define BACT_NO_ACTION                       0
#define BACT_WAIT                            1
#define BACT_PLAY_WAV                        2
#define BACT_TEXT_MESSAGE                    3
#define BACT_MISSION_OBJECTIVES              4
#define BACT_SHOW_PORTRAIT                   5
#define BACT_HIDE_PORTRAIT                   6
#define BACT_DISPLAY_SPEAKING_PORTRAIT       7
#define BACT_TRANSMISSION                    8
#define BACT_SKIP_TUTORIAL                   9

// TRIG flags
#define TRIG_DISABLED                      0x02
#define TRIG_ALWAYSDISPLAY                 0x04
#define TRIG_FLAG_PRESERVE                 0x00000004
// I don't remember what each of these are:
#define TRIG_USED_FLAGS_MASK               0x0000003F

// player IDs
enum {
  PLAYER_1 = 0,
  PLAYER_2,
  PLAYER_3,
  PLAYER_4,
  PLAYER_5,
  PLAYER_6,
  PLAYER_7,
  PLAYER_8,
  PLAYER_TRIG_COUNT = 8,
  PLAYER_9 = 8,
  PLAYER_10,
  PLAYER_11,
  PLAYER_12,
  PLAYER_COUNT = 12,
  PLAYER_CURRENT = 13,
  PLAYER_FOES = 14,
  PLAYER_ALLIES = 15,
  PLAYER_NEUTRAL_PLAYERS = 16,
  PLAYER_ALL_PLAYERS = 17,
  PLAYER_FORCE_1 = 18,
  PLAYER_FORCE_2,
  PLAYER_FORCE_3,
  PLAYER_FORCE_4,
  PLAYER_FORCE_COUNT,
  PLAYER_NAVP = 26,
  PLAYER_ID_COUNT = 27
};


// Unit IDs
#define UNIT_MARINE                          0
#define UNIT_GHOST                           1
#define UNIT_VULTURE                         2
#define UNIT_GOLIATH                         3
#define UNIT_GOLIATH_TURR                    4
#define UNIT_TANK                            5
#define UNIT_TANK_TURR                       6
#define UNIT_SCV                             7
#define UNIT_WRAITH                          8
#define UNIT_SCIENCE_VESSEL                  9
#define UNIT_MONTAG                          10
#define UNIT_DROPSHIP                        11
#define UNIT_BATTLECRUISER                   12
#define UNIT_SPIDER_MINE                     13
#define UNIT_NUCLEAR_MISSILE                 14
#define UNIT_CIVILIAN                        15
#define UNIT_KERRIGAN                        16
#define UNIT_SCHEZAR                         17
#define UNIT_SCHEZAR_TURR                    18
#define UNIT_RAYNOR_VULTURE                  19
#define UNIT_RAYNOR_MARINE                   20
#define UNIT_KAZANSKY                        21
#define UNIT_MAGELLEN                        22
#define UNIT_DUKE_TANK                       23
#define UNIT_DUKE_TANK_TURR                  24
#define UNIT_DUKE_SIEGE                      25
#define UNIT_DUKE_SIEGE_TURR                 26
#define UNIT_MENGSK                          27
#define UNIT_HYPERION                        28
#define UNIT_NORAD_II                        29
#define UNIT_TANK_SIEGE                      30
#define UNIT_TANK_SIEGE_T                    31
#define UNIT_FIREBAT                         32
#define UNIT_SCANNER_SWEEP                   33
#define UNIT_MEDIC                           34
#define UNIT_LARVA                           35
#define UNIT_EGG                             36
#define UNIT_ZERGLING                        37
#define UNIT_HYDRALISK                       38
#define UNIT_ULTRALISK                       39
#define UNIT_BROODLING                       40
#define UNIT_DRONE                           41
#define UNIT_OVERLORD                        42
#define UNIT_MUTALISK                        43
#define UNIT_GUARDIAN                        44
#define UNIT_QUEEN                           45
#define UNIT_DEFILER                         46
#define UNIT_SCOURGE                         47
#define UNIT_TORRASQUE                       48
#define UNIT_MATRIARCH                       49
#define UNIT_INFESTED_TERRAN                 50
#define UNIT_INFESTED_KERRIGAN               51
#define UNIT_UNCLEAN_ONE                     52
#define UNIT_HUNTER_KILLER                   53
#define UNIT_DEVOURING_ONE                   54
#define UNIT_KUKULZA_MUTALISK                55
#define UNIT_KUKULZA_GUARDIAN                56
#define UNIT_YGGDRASIL                       57
#define UNIT_VALKYRIE                        58
#define UNIT_COCOON                          59
#define UNIT_CORSAIR                         60
#define UNIT_DARK_TEMPLAR                    61
#define UNIT_DEVOURER                        62
#define UNIT_DARK_ARCHON                     63
#define UNIT_PROBE                           64
#define UNIT_ZEALOT                          65
#define UNIT_DRAGOON                         66
#define UNIT_HIGH_TEMPLAR                    67
#define UNIT_ARCHON                          68
#define UNIT_SHUTTLE                         69
#define UNIT_SCOUT                           70
#define UNIT_ARBITER                         71
#define UNIT_CARRIER                         72
#define UNIT_INTERCEPTOR                     73
#define UNIT_DARK_TEMPLAR_HERO               74
#define UNIT_ZERATUL                         75
#define UNIT_TASS_ZER                        76
#define UNIT_FENIX_ZEALOT                    77
#define UNIT_FENIX_DRAGOON                   78
#define UNIT_TASSADAR                        79
#define UNIT_MOJO                            80
#define UNIT_WARBRINGER                      81
#define UNIT_GANTRITHOR                      82
#define UNIT_REAVER                          83
#define UNIT_OBSERVER                        84
#define UNIT_SCARAB                          85
#define UNIT_DANIMOTH                        86
#define UNIT_ALDARIS                         87
#define UNIT_ARTANIS                         88
#define UNIT_RHYNADON                        89
#define UNIT_BENGALAAS                       90
#define UNIT_CARGO_SHIP                      91
#define UNIT_MERCENARY_GUNSHIP               92
#define UNIT_SCANTID                         93
#define UNIT_KAKARU                          94
#define UNIT_RAGNASAUR                       95
#define UNIT_URSADON                         96
#define UNIT_LURKER_EGG                      97
#define UNIT_RASZAGAL                        98
#define UNIT_DURAN                           99
#define UNIT_STUKOV                          100
#define UNIT_MAP_REVEALER                    101
#define UNIT_DUGALLE                         102
#define UNIT_LURKER                          103
#define UNIT_INFESTED_DURAN                  104
#define UNIT_DISRUPTION_WEB                  105
#define UNIT_COMMAND_CENTER                  106
#define UNIT_COMSAT_STATION                  107
#define UNIT_NUCLEAR_SILO                    108
#define UNIT_SUPPLY_DEPOT                    109
#define UNIT_REFINERY                        110
#define UNIT_BARRACKS                        111
#define UNIT_ACADEMY                         112
#define UNIT_FACTORY                         113
#define UNIT_STARPORT                        114
#define UNIT_CONTROL_TOWER                   115
#define UNIT_SCIENCE_FACILITY                116
#define UNIT_COVERT_OPS                      117
#define UNIT_PHYSICS_LAB                     118
#define UNIT_STARBASE                        119
#define UNIT_MACHINE_SHOP                    120
#define UNIT_REPAIR_BAY                      121
#define UNIT_ENGINEERING_BAY                 122
#define UNIT_ARMORY                          123
#define UNIT_MISSILE_TURRET                  124
#define UNIT_BUNKER                          125
#define UNIT_NORAD_II_CRASHED                126
#define UNIT_ION_CANNON                      127
#define UNIT_URAJ_CRYSTAL                    128
#define UNIT_KHALIS_CRYSTAL                  129
#define UNIT_INFESTED_COMMAND_CENTER         130
#define UNIT_HATCHERY                        131
#define UNIT_LAIR                            132
#define UNIT_HIVE                            133
#define UNIT_NYDUS_CANAL                     134
#define UNIT_HYDRALISK_DEN                   135
#define UNIT_DEFILER_MOUND                   136
#define UNIT_GREATER_SPIRE                   137
#define UNIT_QUEENS_NEST                     138
#define UNIT_EVOLUTION_CHAMBER               139
#define UNIT_ULTRALISK_CAVERN                140
#define UNIT_SPIRE                           141
#define UNIT_SPAWNING_POOL                   142
#define UNIT_CREEP_COLONY                    143
#define UNIT_SPORE_COLONY                    144
#define UNIT_UNUSED_ZERG_1                   145
#define UNIT_SUNKEN_COLONY                   146
#define UNIT_OVERMIND_SHELL                  147
#define UNIT_OVERMIND                        148
#define UNIT_EXTRACTOR                       149
#define UNIT_MATURE_CHRYSALIS                150
#define UNIT_CEREBRATE                       151
#define UNIT_CEREBRATE_DAGGOTH               152
#define UNIT_UNUSED_ZERG_2                   153
#define UNIT_NEXUS                           154
#define UNIT_ROBOTICS_FACILITY               155
#define UNIT_PYLON                           156
#define UNIT_ASSIMILATOR                     157
#define UNIT_UNUSED_PROTOSS_1                158
#define UNIT_OBSERVATORY                     159
#define UNIT_GATEWAY                         160
#define UNIT_UNUSED_PROTOSS_2                161
#define UNIT_PHOTON_CANNON                   162
#define UNIT_CITADEL_OF_ADUN                 163
#define UNIT_CYBERNETICS_CORE                164
#define UNIT_TEMPLAR_ARCHIVES                165
#define UNIT_FORGE                           166
#define UNIT_STARGATE                        167
#define UNIT_STASIS_CELL                     168
#define UNIT_FLEET_BEACON                    169
#define UNIT_ARBITER_TRIBUNAL                170
#define UNIT_ROBOTICS_SUPPORT_BAY            171
#define UNIT_SHIELD_BATTERY                  172
#define UNIT_KHAYDARIN_FORMATION             173
#define UNIT_PROTOSS_TEMPLE                  174
#define UNIT_XEL_NAGA_TEMPLE                 175
#define UNIT_MINERAL_FIELD_1                 176
#define UNIT_MINERAL_FIELD_2                 177
#define UNIT_MINERAL_FIELD_3                 178
#define UNIT_CAVE                            179
#define UNIT_CAVE_IN                         180
#define UNIT_CANTINA                         181
#define UNIT_MINING_PLATFORM                 182
#define UNIT_INDEPENDENT_COMMAND_CENTER      183
#define UNIT_INDEPENDENT_STARPORT            184
#define UNIT_INDEPENDENT_JUMP_GATE           185
#define UNIT_RUINS                           186
#define UNIT_UNUSED_CRYSTAL_FORMATION        187
#define UNIT_VESPENE_GEYSER                  188
#define UNIT_WARP_GATE                       189
#define UNIT_PSI_DISRUPTOR                   190
#define UNIT_ZERG_MARKER                     191
#define UNIT_TERRAN_MARKER                   192
#define UNIT_PROTOSS_MARKER                  193
#define UNIT_ZERG_BEACON                     194
#define UNIT_TERRAN_BEACON                   195
#define UNIT_PROTOSS_BEACON                  196
#define UNIT_ZERG_FLAG_BEACON                197
#define UNIT_TERRAN_FLAG_BEACON              198
#define UNIT_PROTOSS_FLAG_BEACON             199
#define UNIT_POWER_GENERATOR                 200
#define UNIT_OVERMIND_COCOON                 201
#define UNIT_DARK_SWARM                      202
#define UNIT_FLOOR_MISSILE_TRAP              203
#define UNIT_FLOOR_HATCH                     204
#define UNIT_LEFT_UPPER_DOOR                 205
#define UNIT_RIGHT_UPPER_DOOR                206
#define UNIT_LEFT_PIT_DOOR                   207
#define UNIT_RIGHT_PUT_DOOR                  208
#define UNIT_FLOOR_GUN_TRAP                  209
#define UNIT_LEFT_WALL_MISSILE_TRAP          210
#define UNIT_LEFT_WALL_FLAME_TRAP            211
#define UNIT_RIGHT_WALL_MISSILE_TRAP         212
#define UNIT_RIGHT_WALL_FLAME_TRAP           213
#define UNIT_START_LOCATION                  214
#define UNIT_FLAG                            215
#define UNIT_YOUNG_CHRYSALIS                 216
#define UNIT_PSI_EMITTER                     217
#define UNIT_DATA_DISC                       218
#define UNIT_KHAYDARIN_CRYSTAL               219
#define UNIT_MINERAL_CLUSTER_1               220
#define UNIT_MINERAL_CLUSTER_2               221
#define UNIT_PROTOSS_VESPENE_ORB_1           222
#define UNIT_PROTOSS_VESPENE_ORB_2           223
#define UNIT_ZERG_VESPENE_SAC_1              224
#define UNIT_ZERG_VESPENE_SAC_2              225
#define UNIT_TERRAN_VESPENE_TANK_1           226
#define UNIT_TERRAN_VESPENE_TANK_2           227


// Upgrades -- the only ones that matter
#define UPGR_BURST_LASER                     18
#define UPGR_UNUSED_45                       45
#define UPGR_UNUSED_46                       46
#define UPGR_UNUSED_48                       48
#define UPGR_UNUSED_50                       50
#define UPGR_UNUSED_55                       55
#define UPGR_UNUSED_56                       56
#define UPGR_UNUSED_57                       57
#define UPGR_UNUSED_58                       58
#define UPGR_UNUSED_59                       59
#define UPGR_UPGRADE_60                      60

// Technologies -- the only ones that matter
#define TECH_SPIDER_STIM_PACKS               0
#define TECH_SPIDER_MINES                    3
#define TECH_TANK_SIEGE_MODE                 5
#define TECH_SPIDER_BURROWING                11
#define TECH_INFESTATION                     12
#define TECH_SPAWN_BROODLINGS                13
#define TECH_ARCHON_WARP                     23
#define TECH_UNUSED_26                       26
#define TECH_DARK_ARCHON_MELD                28
#define TECH_LURKER_ASPECT                   32
#define TECH_UNUSED_33                       33
#define TECH_UNUSED_35                       35

// UsedPlayers flags
#define USEDPLAYER_UNUSED                    -1
#define USEDPLAYER_NEUTRAL                   0
#define USEDPLAYER_HUMAN                     1
#define USEDPLAYER_COMPUTER                  2
#define FORCUSED_OTHER                       1
#define FORCUSED_HUMAN                       2
#define FORCUSED_COMPUTER                    4

// UsedUnit flags
#define USEDUNIT_PREPLACED   1
#define USEDUNIT_ENABLED     2
#define USEDUNIT_BUILDABLE   4


// buildops.hpcodes
#define UU_AND          1
#define UU_OR           2
#define UU_SELF         255
#define UU_PREPLACED    1
#define UU_ENABLED      2
#define UU_BUILDABLE    4
#define UU_STANDARD     (UU_BUILDABLE | UU_PREPLACED)
#define UU_END          0
#define UU_EOF          255
