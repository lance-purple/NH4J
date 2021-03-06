/* NetHack 3.6	extern.h	$NHDT-Date: 1449051498 2015/12/02 10:18:18 $  $NHDT-Branch: master $:$NHDT-Revision: 1.520 $ */
/* Copyright (c) Steve Creps, 1988.				  */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef EXTERN_H
#define EXTERN_H

#define E extern

#include <jni.h>
#include <limits.h>

E JNIEnv* jni_env;
E JavaVM* java_vm;

#define ACHIEVEMENT_TYPE_CLASS "rec/games/roguelike/nh4j/AchievementType"
#define ADVENTURER_ROLE_CLASS "rec/games/roguelike/nh4j/AdventurerRole"
#define ADVENTURER_SPECIES_CLASS "rec/games/roguelike/nh4j/AdventurerSpecies"
#define CLOCK_CLASS "rec/games/roguelike/nh4j/Clock"
#define CONTEXT_CLASS "rec/games/roguelike/nh4j/Context"
#define EXPERIENCE_CLASS "rec/games/roguelike/nh4j/Experience"
#define GENDER_CLASS "rec/games/roguelike/nh4j/Gender"
#define HALLUCINATORY_DEITY_CLASS "rec/games/roguelike/nh4j/HallucinatoryDeity"
#define MONSTER_DATA_CLASS "rec/games/roguelike/nh4j/MonsterData"
#define MONSTER_RANDOMIZER_CLASS "rec/games/roguelike/nh4j/MonsterRandomizer"
#define MONSTER_TYPE_CLASS "rec/games/roguelike/nh4j/MonsterType"
#define NOVEL_CLASS "rec/games/roguelike/nh4j/TributeNovel"
#define OCCUPIED_ROOMS_CLASS "rec/games/roguelike/nh4j/OccupiedRooms"
#define PLAYER_CHARACTER_CLASS "rec/games/roguelike/nh4j/PlayerCharacter"
#define ROLE_ALIGNMENT_CLASS "rec/games/roguelike/nh4j/RoleAlignment"
#define ROLE_FILTER_CLASS "rec/games/roguelike/nh4j/RoleFilter"
#define QUALITIES_CLASS "rec/games/roguelike/nh4j/Qualities"
#define SYS_OPT_CLASS "rec/games/roguelike/nh4j/SysOpt"
#define SPECIAL_ITEM_CLASS "rec/games/roguelike/nh4j/SpecialItem"
#define WEAPON_SKILL_CLASS "rec/games/roguelike/nh4j/WeaponSkill"

typedef struct {
    jstring j_str;
    const char* c_str;
} javaString;

#define NO_JAVA_STRING { NULL, NULL }

E int FDECL(pmid4mon, (const struct monst*));
E int FDECL(pmid4you, ());
E void FDECL(setPmid4you, (int));

E javaString FDECL(monsterTypeName, (int));
E javaString FDECL(javaStringFromC, (const char *));
E void FDECL(releaseJavaString, (javaString));


E int FDECL(monsterClass, (int));
E int FDECL(monsterLevel, (int));
E int FDECL(monsterDifficulty, (int));
E int FDECL(monsterMovementSpeed, (int));
E int FDECL(monsterBaseArmorClass, (int));
E int FDECL(monsterBaseMagicResistance, (int));
E int FDECL(monsterAlignment, (int));
E void FDECL(setMonsterAlignment, (int, int));
E int FDECL(monsterGenerationMask, (int));
E int FDECL(monsterCorpseWeight, (int));
E int FDECL(monsterCorpseNutrition, (int));
E int FDECL(monsterSound, (int));
E void FDECL(setMonsterSound, (int, int));
E int FDECL(monsterSize, (int));
E int FDECL(monsterAttacks, (int));
E const struct Attack FDECL(monsterAttack, (int, int));
E boolean FDECL(monsterHasPassiveAttack, (int));
E int FDECL(monsterPassiveAttackIndex, (int));
E long FDECL(monsterResistances, (int));
E boolean FDECL(monsterTypeResistsFire, (int));
E boolean FDECL(monsterTypeResistsStoning, (int));
E long FDECL(monsterConveysResistances, (int));

/* ### alloc.c ### */

#if 0
E long *FDECL(alloc, (unsigned int));
#endif
E char *FDECL(fmt_ptr, (const genericptr));

/* This next pre-processor directive covers almost the entire file,
 * interrupted only occasionally to pick up specific functions as needed. */
#if !defined(MAKEDEFS_C) && !defined(LEV_LEX_C)

/* ### allmain.c ### */

E void FDECL(moveloop, (BOOLEAN_P));
E void NDECL(stop_occupation);
E void NDECL(display_gamewindows);
E void NDECL(newgame);
E void FDECL(welcome, (BOOLEAN_P));
E time_t NDECL(get_realtime);

/* ### apply.c ### */

E int NDECL(doapply);
E int NDECL(dorub);
E int NDECL(dojump);
E int FDECL(jump, (int));
E int NDECL(number_leashed);
E void FDECL(o_unleash, (struct obj *));
E void FDECL(m_unleash, (struct monst *, BOOLEAN_P));
E void NDECL(unleash_all);
E boolean NDECL(next_to_u);
E struct obj *FDECL(get_mleash, (struct monst *));
E const char *NDECL(beautiful);
E void FDECL(check_leash, (XCHAR_P, XCHAR_P));
E boolean FDECL(um_dist, (XCHAR_P, XCHAR_P, XCHAR_P));
E boolean FDECL(snuff_candle, (struct obj *));
E boolean FDECL(snuff_lit, (struct obj *));
E boolean FDECL(catch_lit, (struct obj *));
E void FDECL(use_unicorn_horn, (struct obj *));
E boolean FDECL(tinnable, (struct obj *));
E void NDECL(reset_trapset);
E void FDECL(fig_transform, (ANY_P *, long));
E int FDECL(unfixable_trouble_count, (BOOLEAN_P));

/* ### artifact.c ### */

E void NDECL(init_artifacts);
E void FDECL(save_artifacts, (int));
E void FDECL(restore_artifacts, (int));
E const char *FDECL(artiname, (int));
E struct obj *FDECL(mk_artifact, (struct obj *, ALIGNTYP_P));
E const char *FDECL(artifact_name, (const char *, short *));
E boolean FDECL(exist_artifact, (int, const char *));
E void FDECL(artifact_exists, (struct obj *, const char *, BOOLEAN_P));
E int NDECL(nartifact_exist);
E boolean FDECL(arti_immune, (struct obj *, int));
E boolean FDECL(spec_ability, (struct obj *, unsigned long));
E boolean FDECL(confers_luck, (struct obj *));
E boolean FDECL(arti_reflects, (struct obj *));
E boolean FDECL(shade_glare, (struct obj *));
E boolean FDECL(restrict_name, (struct obj *, const char *));
E boolean FDECL(defends, (int, struct obj *));
E boolean FDECL(defends_when_carried, (int, struct obj *));
E boolean FDECL(protects, (struct obj *, BOOLEAN_P));
E void FDECL(set_artifact_intrinsic, (struct obj *, BOOLEAN_P, long));
E int FDECL(touch_artifact, (struct obj *, struct monst *));
E int FDECL(spec_abon, (struct obj *, struct monst *));
E int FDECL(spec_dbon, (struct obj *, struct monst *, int));
E void FDECL(discover_artifact, (XCHAR_P));
E boolean FDECL(undiscovered_artifact, (XCHAR_P));
E int FDECL(disp_artifact_discoveries, (winid));
E boolean FDECL(artifact_hit,
                (struct monst *, struct monst *, struct obj *, int *, int));
E int NDECL(doinvoke);
E boolean FDECL(finesse_ahriman, (struct obj *));
E void FDECL(arti_speak, (struct obj *));
E boolean FDECL(artifact_light, (struct obj *));
E long FDECL(spec_m2, (struct obj *));
E boolean FDECL(artifact_has_invprop, (struct obj *, UCHAR_P));
E long FDECL(arti_cost, (struct obj *));
E struct obj *FDECL(whatGivesExtrinsic, (int));
E const char *FDECL(glow_color, (int));
E void FDECL(Sting_effects, (int));
E int FDECL(retouch_object, (struct obj **, BOOLEAN_P));
E void FDECL(retouch_equipment, (int));

E boolean FDECL(isAffectedByWeaponFlag1, (int, long));
E boolean FDECL(isAffectedByWeaponFlag2, (int, long));

/* ### attrib.c ### */

E boolean FDECL(adjattrib, (int, int, int));
E void FDECL(gainstr, (struct obj *, int, BOOLEAN_P));
E void FDECL(losestr, (int));
E void FDECL(poisontell, (int, BOOLEAN_P));
E void FDECL(poisoned, (const char *, int, const char *, int, BOOLEAN_P));
E void FDECL(change_luck, (SCHAR_P));
E int FDECL(stone_luck, (BOOLEAN_P));
E void NDECL(set_moreluck);
E void NDECL(restore_attrib);
E void FDECL(exercise, (int, BOOLEAN_P));
E void NDECL(exerchk);
E void FDECL(init_attr, (int));
E void NDECL(redist_attr);
E void FDECL(adjabil, (int, int));
E int NDECL(newhp);
E schar FDECL(acurr, (int));
E schar NDECL(acurrstr);
E boolean FDECL(extremeattr, (int));
E void FDECL(adjalign, (int));
E int FDECL(is_innate, (int));
E char *FDECL(from_what, (int));
E void FDECL(uchangealign, (int, int));

E boolean FDECL(youHaveIntrinsic, (int));
E long FDECL(yourIntrinsic, (int));
E void FDECL(setYourIntrinsic, (int, long));
E boolean FDECL(yourIntrinsicHasMask, (int, long));
E void FDECL(setYourIntrinsicMask, (int, long));
E void FDECL(unsetYourIntrinsicMask, (int, long));

E boolean FDECL(youHaveExtrinsic, (int));
E long FDECL(yourExtrinsic, (int));
E void FDECL(setYourExtrinsic, (int, long));
E boolean FDECL(yourExtrinsicHasMask, (int, long));
E void FDECL(setYourExtrinsicMask, (int, long));
E void FDECL(unsetYourExtrinsicMask, (int, long));
E void FDECL(toggleYourExtrinsicMask, (int, long));

E boolean FDECL(youAreBlockedFrom, (int));
E long FDECL(yourBlocker, (int));
E void FDECL(setYourBlocker, (int, long));
E boolean FDECL(yourBlockerHasMask, (int, long));
E void FDECL(setYourBlockerMask, (int, long));
E void FDECL(unsetYourBlockerMask, (int, long));

E boolean FDECL(youResistFire, ());
E boolean FDECL(youResistCold, ());
E boolean FDECL(youResistSleep, ());
E boolean FDECL(youResistDisintegration, ());
E boolean FDECL(youResistShock, ());
E boolean FDECL(youResistPoison, ());
E boolean FDECL(youResistDraining, ());
E boolean FDECL(youResistMagic, ());
E boolean FDECL(youResistAcid, ());
E boolean FDECL(youResistStoning, ());
E boolean FDECL(youResistSickness, ());
E boolean FDECL(youResistHallucination, ());
E boolean FDECL(youAreInvulnerable, ());

E boolean FDECL(youAreBeingPunished, ());
E boolean FDECL(youAreStunned, ());
E boolean FDECL(youAreConfused, ());
E boolean FDECL(youAreHallucinating, ());
E boolean FDECL(youAreTemporarilyBlinded, ());
E boolean FDECL(youAreBlindfolded, ());
E boolean FDECL(youAreBlindDueToBlindfold, ());
E boolean FDECL(youCannotSee, ());
E boolean FDECL(youCanSee, ());
E boolean FDECL(youAreSick, ());
E boolean FDECL(youAreVomiting, ());
E boolean FDECL(youHaveSlipperyFingers, ());
E boolean FDECL(youAreTurningToStone, ());
E boolean FDECL(youAreTurningToSlime, ());
E boolean FDECL(youAreBeingStrangled, ());

E boolean FDECL(youAreDeaf, ());
E boolean FDECL(youKeepFumbling, ());
E boolean FDECL(youHaveWoundedLegs, ());
E boolean FDECL(youAreSleepy, ());
E boolean FDECL(youHunger, ());

E boolean FDECL(youCanSeeInvisible, ());
E boolean FDECL(youHaveTelepathyWhenBlind, ());
E boolean FDECL(youHaveTelepathyWhenNotBlind, ());
E boolean FDECL(youSenseWarnings, ());
E boolean FDECL(youAreWarnedOfMonsters, ());
E boolean FDECL(youAreWarnedOfUndead, ());
E boolean FDECL(youHaveAutomaticSearching, ());
E boolean FDECL(youAreClairvoyant, ());
E boolean FDECL(youHaveInfravision, ());
E boolean FDECL(youCanDetectMonsters, ());
E boolean FDECL(youAreAdorned, ());
E boolean FDECL(youAreInvisibleToOthers, ());
E boolean FDECL(youAreFullyInvisible, ());
E boolean FDECL(youAppearDisplaced, ());
E boolean FDECL(youAreStealthy, ());
E boolean FDECL(youAggravateMonsters, ());
E boolean FDECL(youCauseConflict, ());
E boolean FDECL(youCanJump, ());
E boolean FDECL(youCanTeleport, ());
E boolean FDECL(youHaveTeleportControl, ());
E boolean FDECL(youAreLevitating, ());
E boolean FDECL(youCanLevitateAtWill, ());
E boolean FDECL(youAreFlying, ());
E boolean FDECL(youCanSwim, ());
E boolean FDECL(youAreAmphibious, ());
E boolean FDECL(youNeedNotBreathe, ());
E boolean FDECL(youCanPassThroughWalls, ());
E boolean FDECL(youHaveSlowDigestion, ());
E boolean FDECL(youTakeHalfDamageFromSpells, ());
E boolean FDECL(youTakeHalfDamageFromPhysicalAttacks, ());
E boolean FDECL(youRegenerate, ());
E boolean FDECL(yourEnergyRegenerates, ());
E boolean FDECL(youAreProtected, ());
E boolean FDECL(youHaveProtectionFromShapeChangers, ());
E boolean FDECL(youPolymorph, ());
E boolean FDECL(youHavePolymorphControl, ());
E boolean FDECL(youAreUnchanging, ());
E boolean FDECL(youAreFast, ());
E boolean FDECL(youAreVeryFast, ());
E boolean FDECL(youCanReflectAttacks, ());
E boolean FDECL(youHaveFreeAction, ());
E boolean FDECL(youHaveFixedAbilities, ());
E boolean FDECL(yourLifeCanBeSaved, ());

/* ### ball.c ### */

E void FDECL(ballrelease, (BOOLEAN_P));
E void NDECL(ballfall);
E void NDECL(placebc);
E void NDECL(unplacebc);
E void FDECL(set_bc, (int));
E void FDECL(move_bc, (int, int, XCHAR_P, XCHAR_P, XCHAR_P, XCHAR_P));
E boolean FDECL(drag_ball, (XCHAR_P, XCHAR_P, int *, xchar *, xchar *,
                            xchar *, xchar *, boolean *, BOOLEAN_P));
E void FDECL(drop_ball, (XCHAR_P, XCHAR_P));
E void NDECL(drag_down);

/* ### bones.c ### */

E void FDECL(sanitize_name, (char *));
E void FDECL(drop_upon_death, (struct monst *, struct obj *, int, int));
E boolean NDECL(can_make_bones);
E void FDECL(savebones, (int, time_t, struct obj *));
E int NDECL(getbones);

/* ### botl.c ### */

E int FDECL(xlev_to_rank, (int));
E int FDECL(title_to_mon, (const char *, int *));
E void NDECL(max_rank_sz);
#ifdef SCORE_ON_BOTL
E long NDECL(botl_score);
#endif
E int FDECL(describe_level, (char *));
E javaString FDECL(rankOf, (int, SHORT_P, BOOLEAN_P));
E void NDECL(bot);
#ifdef STATUS_VIA_WINDOWPORT
E void FDECL(status_initialize, (BOOLEAN_P));
E void NDECL(status_finish);
E void FDECL(status_notify_windowport, (BOOLEAN_P));
#ifdef STATUS_HILITES
E boolean FDECL(set_status_hilites, (char *op, BOOLEAN_P));
E void FDECL(clear_status_hilites, (BOOLEAN_P));
E char *FDECL(get_status_hilites, (char *, int));
E boolean NDECL(status_hilite_menu);
#endif
#endif

/* ### cmd.c ### */

E boolean FDECL(redraw_cmd, (CHAR_P));
#ifdef USE_TRAMPOLI
E int NDECL(doextcmd);
E int NDECL(domonability);
E int NDECL(doprev_message);
E int NDECL(timed_occupation);
E int NDECL(doattributes);
E int NDECL(wiz_detect);
E int NDECL(wiz_genesis);
E int NDECL(wiz_identify);
E int NDECL(wiz_level_tele);
E int NDECL(wiz_map);
E int NDECL(wiz_where);
E int NDECL(wiz_wish);
#endif /* USE_TRAMPOLI */
E void NDECL(reset_occupations);
E void FDECL(set_occupation, (int (*)(void), const char *, int));
E char NDECL(pgetchar);
E void FDECL(pushch, (CHAR_P));
E void FDECL(savech, (CHAR_P));
E void NDECL(add_debug_extended_commands);
E void FDECL(reset_commands, (BOOLEAN_P));
E void FDECL(rhack, (char *));
E int NDECL(doextlist);
E int NDECL(extcmd_via_menu);
E int NDECL(enter_explore_mode);
E void FDECL(enlightenment, (int, int));
E void FDECL(youhiding, (BOOLEAN_P, int));
E void FDECL(show_conduct, (int));
E int FDECL(xytod, (SCHAR_P, SCHAR_P));
E void FDECL(dtoxy, (coord *, int));
E int FDECL(movecmd, (CHAR_P));
E int NDECL(dxdy_moveok);
E int FDECL(getdir, (const char *));
E void NDECL(confdir);
E const char *FDECL(directionname, (int));
E int FDECL(isok, (int, int));
E int FDECL(get_adjacent_loc,
            (const char *, const char *, XCHAR_P, XCHAR_P, coord *));
E const char *FDECL(click_to_cmd, (int, int, int));
#ifdef HANGUPHANDLING
E void FDECL(hangup, (int));
E void NDECL(end_of_input);
#endif
E char NDECL(readchar);
E void NDECL(sanity_check);
E char FDECL(yn_function, (const char *, const char *, CHAR_P));
E boolean FDECL(paranoid_query, (BOOLEAN_P, const char *));

/* ### context ### */

E void FDECL (initIdentifier, (int));
E int FDECL (currentIdentifier, ());
E int FDECL (nextIdentifier, ());

E int FDECL (startingPetID, ());
E void FDECL (setStartingPetID, (int));

E int FDECL (randomEscapeSequencePrefix, ());
E void FDECL (setRandomEscapeSequencePrefix, (int));

E int FDECL (numberOfWizards, ());
E void FDECL (setNumberOfWizards, (int));
E void FDECL (increaseNumberOfWizards, (int));
E void FDECL (decreaseNumberOfWizards, (int));

E int FDECL (monsterWarningLevel, ());
E void FDECL (setMonsterWarningLevel, (int));

E boolean FDECL (running, ());
E int FDECL (runningPace, ());
E void FDECL (setRunningPace, (int));

#define WALK_ONE_SQUARE 0
#define WALK_TIL_DIVERTED 1
#define RUN_TIL_DIVERTED 2
#define RUN_TIL_DIVERTED_BY_NONBRANCH 3
/* 4: ff+, 5: ff-, 6: FF+, 7: FF- */
#define TRAVEL_TO_POINT 8

E int FDECL (currentFruit, ());
E void FDECL (setCurrentFruit, (int));

/* ### dbridge.c ### */

E boolean FDECL(is_pool, (int, int));
E boolean FDECL(is_lava, (int, int));
E boolean FDECL(is_pool_or_lava, (int, int));
E boolean FDECL(is_ice, (int, int));
E boolean FDECL(is_moat, (int, int));
E schar FDECL(db_under_typ, (int));
E int FDECL(is_drawbridge_wall, (int, int));
E boolean FDECL(is_db_wall, (int, int));
E boolean FDECL(find_drawbridge, (int *, int *));
E boolean FDECL(create_drawbridge, (int, int, int, BOOLEAN_P));
E void FDECL(open_drawbridge, (int, int));
E void FDECL(close_drawbridge, (int, int));
E void FDECL(destroy_drawbridge, (int, int));

/* ### decl.c ### */

E void NDECL(decl_init);

/* ### detect.c ### */

E struct obj *FDECL(o_in, (struct obj *, CHAR_P));
E struct obj *FDECL(o_material, (struct obj *, unsigned));
E int FDECL(gold_detect, (struct obj *));
E int FDECL(food_detect, (struct obj *));
E int FDECL(object_detect, (struct obj *, int));
E int FDECL(monster_detect, (struct obj *, int));
E int FDECL(trap_detect, (struct obj *));
E const char *FDECL(level_distance, (d_level *));
E void FDECL(use_crystal_ball, (struct obj **));
E void NDECL(do_mapping);
E void NDECL(do_vicinity_map);
E void FDECL(cvt_sdoor_to_door, (struct rm *));
#ifdef USE_TRAMPOLI
E void FDECL(findone, (int, int, genericptr_t));
E void FDECL(openone, (int, int, genericptr_t));
#endif
E int NDECL(findit);
E int NDECL(openit);
E boolean FDECL(detecting, (void (*)(int, int, genericptr)));
E void FDECL(find_trap, (struct trap *));
E int FDECL(dosearch0, (int));
E int NDECL(dosearch);
E void NDECL(sokoban_detect);
E void FDECL(reveal_terrain, (int, int));

/* ### dig.c ### */

E int FDECL(dig_typ, (struct obj *, XCHAR_P, XCHAR_P));
E boolean NDECL(is_digging);
#ifdef USE_TRAMPOLI
E int NDECL(dig);
#endif
E int NDECL(holetime);
E boolean FDECL(dig_check, (struct monst *, BOOLEAN_P, int, int));
E void FDECL(digactualhole, (int, int, struct monst *, int));
E boolean FDECL(dighole, (BOOLEAN_P, BOOLEAN_P, coord *));
E int FDECL(use_pick_axe, (struct obj *));
E int FDECL(use_pick_axe2, (struct obj *));
E boolean FDECL(mdig_tunnel, (struct monst *));
E void FDECL(draft_message, (BOOLEAN_P));
E void FDECL(watch_dig, (struct monst *, XCHAR_P, XCHAR_P, BOOLEAN_P));
E void NDECL(zap_dig);
E struct obj *FDECL(bury_an_obj, (struct obj *, boolean *));
E void FDECL(bury_objs, (int, int));
E void FDECL(unearth_objs, (int, int));
E void FDECL(rot_organic, (ANY_P *, long));
E void FDECL(rot_corpse, (ANY_P *, long));
E struct obj *FDECL(buried_ball, (coord *));
E void NDECL(buried_ball_to_punishment);
E void NDECL(buried_ball_to_freedom);
E schar FDECL(fillholetyp, (int, int, BOOLEAN_P));
E void FDECL(liquid_flow,
             (XCHAR_P, XCHAR_P, SCHAR_P, struct trap *, const char *));
E boolean FDECL(conjoined_pits, (struct trap *, struct trap *, BOOLEAN_P));
#if 0
E void FDECL(bury_monst, (struct monst *));
E void NDECL(bury_you);
E void NDECL(unearth_you);
E void NDECL(escape_tomb);
E void FDECL(bury_obj, (struct obj *));
#endif

/* ### display.c ### */

E void FDECL(magic_map_background, (XCHAR_P, XCHAR_P, int));
E void FDECL(map_background, (XCHAR_P, XCHAR_P, int));
E void FDECL(map_trap, (struct trap *, int));
E void FDECL(map_object, (struct obj *, int));
E void FDECL(map_invisible, (XCHAR_P, XCHAR_P));
E void FDECL(unmap_object, (int, int));
E void FDECL(map_location, (int, int, int));
E void FDECL(feel_newsym, (XCHAR_P, XCHAR_P));
E void FDECL(feel_location, (XCHAR_P, XCHAR_P));
E void FDECL(newsym, (int, int));
E void FDECL(shieldeff, (XCHAR_P, XCHAR_P));
E void FDECL(tmp_at, (int, int));
E void FDECL(showHeroBeingSwallowed, (int));
E void FDECL(showHeroBeingBuried, (int));
E void FDECL(showHeroUnderwater, (int));
E void NDECL(see_monsters);
E void NDECL(set_mimic_blocking);
E void NDECL(see_objects);
E void NDECL(see_traps);
E void NDECL(curs_on_u);
E int NDECL(doredraw);
E void NDECL(docrt);
E void FDECL(show_glyph, (int, int, int));
E void NDECL(clear_glyph_buffer);
E void FDECL(row_refresh, (int, int, int));
E void NDECL(cls);
E void FDECL(flush_screen, (int));
E int FDECL(back_to_glyph, (XCHAR_P, XCHAR_P));
E int FDECL(zapdir_to_glyph, (int, int, int));
E int FDECL(glyph_at, (XCHAR_P, XCHAR_P));
E void NDECL(set_wall_state);
E void FDECL(unset_seenv, (struct rm *, int, int, int, int));
E boolean FDECL(seeWithInfrared, (struct monst*));

/* ### do.c ### */

#ifdef USE_TRAMPOLI
E int FDECL(drop, (struct obj *));
E int NDECL(wipeoff);
#endif
E int NDECL(dodrop);
E boolean FDECL(boulder_hits_pool, (struct obj *, int, int, BOOLEAN_P));
E boolean FDECL(flooreffects, (struct obj *, int, int, const char *));
E void FDECL(doaltarobj, (struct obj *));
E boolean FDECL(canletgo, (struct obj *, const char *));
E void FDECL(dropx, (struct obj *));
E void FDECL(dropy, (struct obj *));
E void FDECL(dropz, (struct obj *, BOOLEAN_P));
E void FDECL(obj_no_longer_held, (struct obj *));
E int NDECL(doddrop);
E int NDECL(dodown);
E int NDECL(doup);
#ifdef INSURANCE
E void NDECL(save_currentstate);
#endif
E int FDECL(typeOfLevelYouWereSentTo, ());
E void FDECL(setTypeOfLevelYouWereSentTo, (int));
E void FDECL(goto_level, (d_level *, BOOLEAN_P, BOOLEAN_P, BOOLEAN_P));
E void FDECL(schedule_goto, (d_level *, BOOLEAN_P, BOOLEAN_P, int,
                             const char *, const char *));
E void NDECL(deferred_goto);
E boolean FDECL(revive_corpse, (struct obj *));
E void FDECL(revive_mon, (ANY_P *, long));
E int NDECL(donull);
E int NDECL(dowipe);
E void FDECL(set_wounded_legs, (long, int));
E void NDECL(heal_legs);

/* ### do_name.c ### */

E int FDECL(getpos, (coord *, BOOLEAN_P, const char *));
E void FDECL(getpos_sethilite, (void (*f)(int)));
E void FDECL(new_mname, (struct monst *, int));
E void FDECL(free_mname, (struct monst *));
E void FDECL(new_oname, (struct obj *, int));
E void FDECL(free_oname, (struct obj *));
E const char *FDECL(safe_oname, (struct obj *));
E struct monst *FDECL(christen_monst, (struct monst *, const char *));
E struct obj *FDECL(oname, (struct obj *, const char *));
E boolean FDECL(objtyp_is_callable, (int));
E int NDECL(docallcmd);
E void FDECL(docall, (struct obj *));
E const char *NDECL(rndghostname);
E char *FDECL(x_monnam, (struct monst *, int, const char *, int, BOOLEAN_P));
E char *FDECL(l_monnam, (struct monst *));
E char *FDECL(mon_nam, (struct monst *));
E char *FDECL(noit_mon_nam, (struct monst *));
E char *FDECL(Monnam, (struct monst *));
E char *FDECL(noit_Monnam, (struct monst *));
E char *FDECL(m_monnam, (struct monst *));
E char *FDECL(y_monnam, (struct monst *));
E char *FDECL(Adjmonnam, (struct monst *, const char *));
E char *FDECL(Amonnam, (struct monst *));
E char *FDECL(a_monnam, (struct monst *));
E char *FDECL(distant_monnam, (struct monst *, int, char *));
E char *FDECL(rndmonnam, (char *));
E const char *FDECL(hcolor, (const char *));
E const char *NDECL(rndcolor);
E const char *NDECL(roguename);
E struct obj *FDECL(realloc_obj,
                    (struct obj *, int, genericptr_t, int, const char *));
E char *FDECL(coyotename, (struct monst *, char *));
E const char *FDECL(noveltitle, (int *));
E const char *FDECL(lookup_novel, (const char *, int *));

/* ### do_wear.c ### */

#ifdef USE_TRAMPOLI
E int NDECL(Armor_on);
E int NDECL(Boots_on);
E int NDECL(Gloves_on);
E int NDECL(Helmet_on);
E int FDECL(select_off, (struct obj *));
E int NDECL(take_off);
#endif
E void FDECL(off_msg, (struct obj *));
E void FDECL(set_wear, (struct obj *));
E boolean FDECL(donning, (struct obj *));
E boolean FDECL(doffing, (struct obj *));
E void NDECL(cancel_don);
E int FDECL(stop_donning, (struct obj *));
E int NDECL(Armor_off);
E int NDECL(Armor_gone);
E int NDECL(Helmet_off);
E int NDECL(Gloves_off);
E int NDECL(Boots_off);
E int NDECL(Cloak_off);
E int NDECL(Shield_off);
E int NDECL(Shirt_off);
E void NDECL(Amulet_off);
E void FDECL(Ring_on, (struct obj *));
E void FDECL(Ring_off, (struct obj *));
E void FDECL(Ring_gone, (struct obj *));
E void FDECL(Blindf_on, (struct obj *));
E void FDECL(Blindf_off, (struct obj *));
E int NDECL(dotakeoff);
E int NDECL(doremring);
E int FDECL(cursed, (struct obj *));
E int FDECL(armoroff, (struct obj *));
E int FDECL(canwearobj, (struct obj *, long *, BOOLEAN_P));
E int NDECL(dowear);
E int NDECL(doputon);
E void NDECL(find_ac);
E void NDECL(glibr);
E struct obj *FDECL(some_armor, (struct monst *));
E struct obj *FDECL(stuck_ring, (struct obj *, int));
E struct obj *NDECL(unchanger);
E void NDECL(reset_remarm);
E int NDECL(doddoremarm);
E int FDECL(destroy_arm, (struct obj *));
E void FDECL(adj_abon, (struct obj *, SCHAR_P));
E boolean
FDECL(inaccessible_equipment, (struct obj *, const char *, BOOLEAN_P));

/* ### dog.c ### */

E boolean FDECL(yourRoleHasAPreferredPet, ());
E int FDECL(pmidOfPreferredPetForYourRole, ());

E void FDECL(newedog, (struct monst *));
E void FDECL(free_edog, (struct monst *));
E void FDECL(initedog, (struct monst *));
E struct monst *FDECL(make_familiar,
                      (struct obj *, XCHAR_P, XCHAR_P, BOOLEAN_P));
E struct monst *NDECL(makedog);
E void NDECL(update_mlstmv);
E void NDECL(losedogs);
E void FDECL(mon_arrive, (struct monst *, BOOLEAN_P));
E void FDECL(mon_catchup_elapsed_time, (struct monst *, long));
E void FDECL(keepdogs, (BOOLEAN_P));
E void FDECL(migrate_to_level, (struct monst *, XCHAR_P, XCHAR_P, coord *));
E int FDECL(dogfood, (struct monst *, struct obj *));
E boolean FDECL(tamedog, (struct monst *, struct obj *));
E void FDECL(abuse_dog, (struct monst *));
E void FDECL(wary_dog, (struct monst *, BOOLEAN_P));

/* ### dogmove.c ### */

E struct obj *FDECL(droppables, (struct monst *));
E int FDECL(dog_nutrition, (struct monst *, struct obj *));
E int FDECL(dog_eat, (struct monst *, struct obj *, int, int, BOOLEAN_P));
E int FDECL(dog_move, (struct monst *, int));
#ifdef USE_TRAMPOLI
E void FDECL(wantdoor, (int, int, genericptr_t));
#endif
E void FDECL(finish_meating, (struct monst *));

/* ### dokick.c ### */

E boolean FDECL(ghitm, (struct monst *, struct obj *));
E void FDECL(container_impact_dmg, (struct obj *, XCHAR_P, XCHAR_P));
E int NDECL(dokick);
E boolean FDECL(ship_object, (struct obj *, XCHAR_P, XCHAR_P, BOOLEAN_P));
E void FDECL(obj_delivery, (BOOLEAN_P));
E schar FDECL(down_gate, (XCHAR_P, XCHAR_P));
E void FDECL(impact_drop, (struct obj *, XCHAR_P, XCHAR_P, XCHAR_P));

/* ### dothrow.c ### */

E int NDECL(dothrow);
E int NDECL(dofire);
E void FDECL(endmultishot, (BOOLEAN_P));
E void FDECL(hitfloor, (struct obj *));
E void FDECL(hurtle, (int, int, int, BOOLEAN_P));
E void FDECL(mhurtle, (struct monst *, int, int, int));
E void FDECL(throwit, (struct obj *, long, BOOLEAN_P));
E int FDECL(omon_adj, (struct monst *, struct obj *, BOOLEAN_P));
E int FDECL(thitmonst, (struct monst *, struct obj *));
E int FDECL(hero_breaks, (struct obj *, XCHAR_P, XCHAR_P, BOOLEAN_P));
E int FDECL(breaks, (struct obj *, XCHAR_P, XCHAR_P));
E void FDECL(release_camera_demon, (struct obj *, XCHAR_P, XCHAR_P));
E void FDECL(breakobj,
             (struct obj *, XCHAR_P, XCHAR_P, BOOLEAN_P, BOOLEAN_P));
E boolean FDECL(breaktest, (struct obj *));
E boolean FDECL(walk_path, (coord *, coord *,
                            boolean (*)(genericptr, int, int), genericptr_t));
E boolean FDECL(hurtle_step, (genericptr_t, int, int));

/* ### drawing.c ### */
#endif /* !MAKEDEFS_C && !LEV_LEX_C */
E int FDECL(def_char_to_objclass, (CHAR_P));
E int FDECL(def_char_to_monclass, (CHAR_P));
#if !defined(MAKEDEFS_C) && !defined(LEV_LEX_C)
E void FDECL(switch_symbols, (int));
E void FDECL(assign_graphics, (int));
E void NDECL(init_r_symbols);
E void NDECL(init_symbols);
E void NDECL(update_bouldersym);
E void NDECL(init_showsyms);
E void NDECL(init_l_symbols);
E void FDECL(clear_symsetentry, (int, BOOLEAN_P));
E void FDECL(update_l_symset, (struct symparse *, int));
E void FDECL(update_r_symset, (struct symparse *, int));
E boolean FDECL(cursed_object_at, (int, int));

/* ### dungeon.c ### */

E void FDECL(save_dungeon, (int, BOOLEAN_P, BOOLEAN_P));
E void FDECL(restore_dungeon, (int));
E void FDECL(insert_branch, (branch *, BOOLEAN_P));
E void NDECL(init_dungeons);
E s_level *FDECL(find_level, (const char *));
E s_level *FDECL(Is_special, (d_level *));
E branch *FDECL(Is_branchlev, (d_level *));
E boolean FDECL(builds_up, (d_level *));
E boolean FDECL(currentDungeonBuildsUpward, ());
E xchar FDECL(ledger_no, (d_level *));
E xchar FDECL(currentLevelLedgerNum, ());
E xchar NDECL(maxledgerno);
E xchar FDECL(deepestLevelReachedInCurrentDungeon, ());
E void FDECL(setDeepestLevelReachedInCurrentDungeon, (xchar));
E schar FDECL(depth, (d_level *));
E schar FDECL(currentDepth, ());
E xchar FDECL(dunlev, (d_level *));
E xchar FDECL(dunlevs_in_dungeon, (d_level *));
E xchar FDECL(levelsInCurrentDungeon, ());
E xchar FDECL(ledger_to_dnum, (XCHAR_P));
E xchar FDECL(ledger_to_dlev, (XCHAR_P));
E xchar FDECL(deepest_lev_reached, (BOOLEAN_P));
E boolean FDECL(on_level, (d_level *, d_level *));
E boolean FDECL(areYouOnLevel, (d_level *));
E boolean FDECL(areYouBeingSentToSameLevel, ());
E boolean FDECL(onSameLevelAsLastTurn, ());
E boolean FDECL(notOnTheSameLevelAsLastTurn, ());
E void FDECL(next_level, (BOOLEAN_P));
E void FDECL(prev_level, (BOOLEAN_P));
E void FDECL(u_on_newpos, (int, int));
E void FDECL(u_on_rndspot, (int));
E void FDECL(u_on_sstairs, (int));
E void NDECL(u_on_upstairs);
E void NDECL(u_on_dnstairs);
E boolean FDECL(On_stairs, (XCHAR_P, XCHAR_P));
E void FDECL(get_level, (d_level *, int));
E boolean FDECL(Is_botlevel, (d_level *));
E boolean FDECL(canYouFallThroughCurrentLevel, ());
E boolean FDECL(canYouDigDown, ());
E boolean FDECL(canYouRiseUp, (int, int));
E boolean FDECL(canYouWalkOnWater, ());
E boolean FDECL(currentLevelHasCeiling, ());
E boolean FDECL(In_quest, (d_level *));
E boolean FDECL(areYouInTheQuestDungeon, ());
E boolean FDECL(wereYouInTheQuestDungeonLastTurn, ());
E boolean FDECL(areYouInTheMines, ());
E branch *FDECL(dungeon_branch, (const char *));
E boolean FDECL(at_dgn_entrance, (const char *));
E boolean FDECL(In_hell, (d_level *));
E boolean FDECL(areYouOnAVladsTowerLevel, ());
E boolean FDECL(On_W_tower_level, (d_level *));
E boolean FDECL(areYouOnAWizardTowerLevel, ());
E boolean FDECL(areYouInsideWizardTower, (int, int));
E boolean FDECL(areYouOnWizardTowerLevel1, ());
E boolean FDECL(areYouOnWizardTowerLevel2, ());
E boolean FDECL(areYouOnWizardTowerLevel3, ());
E void FDECL(find_hell, (d_level *));
E void FDECL(goto_hell, (BOOLEAN_P, BOOLEAN_P));
E void FDECL(assign_level, (d_level *, d_level *));
E void FDECL(assignFromCurrentLevel, (d_level *));
E void FDECL(setCurrentLevelTo, (d_level *));
E void FDECL(assignRandomLevelFromCurrentDungeon, (d_level *, int));
E int FDECL(induced_align, (int));
E boolean FDECL(Invocation_lev, (d_level *));
E boolean FDECL(areYouOnInvocationLevel, ());
E xchar NDECL(level_difficulty);
E schar FDECL(lev_by_name, (const char *));
E schar FDECL(print_dungeon, (BOOLEAN_P, schar *, xchar *));
E char *FDECL(currentLevelAnnotation, ());
E int NDECL(donamelevel);
E int NDECL(dooverview);
E void FDECL(show_overview, (int, int));
E void FDECL(forget_mapseen, (int));
E void FDECL(initCurrentLevelMapSeen, ());
E void NDECL(recalc_mapseen);
E void FDECL(mapseen_temple, (struct monst *));
E void FDECL(room_discovered, (int));
E void FDECL(setBranchSeenFromCurrentLevel, (d_level *));
E void FDECL(remdun_mapseen, (int));

/* ### eat.c ### */

#ifdef USE_TRAMPOLI
E int NDECL(eatmdone);
E int NDECL(eatfood);
E int NDECL(opentin);
E int NDECL(unfaint);
#endif
E void NDECL(eatmupdate);
E boolean FDECL(is_edible, (struct obj *));
E void NDECL(init_uhunger);
E int NDECL(Hear_again);
E void NDECL(reset_eat);
E int NDECL(doeat);
E void NDECL(gethungry);
E void FDECL(morehungry, (int));
E void FDECL(lesshungry, (int));
E boolean NDECL(is_fainted);
E void NDECL(reset_faint);
E void NDECL(violated_vegetarian);
E void FDECL(newuhs, (BOOLEAN_P));
E struct obj *FDECL(floorfood, (const char *, int));
E void NDECL(vomit);
E int FDECL(eaten_stat, (int, struct obj *));
E void FDECL(food_disappears, (struct obj *));
E void FDECL(food_substitution, (struct obj *, struct obj *));
E void FDECL(updateConductsAfterEating, (int));
E int FDECL(eat_brains, (struct monst *, struct monst *, BOOLEAN_P, int *));
E void NDECL(fix_petrification);
E void FDECL(consume_oeaten, (struct obj *, int));
E boolean FDECL(maybe_finished_meal, (BOOLEAN_P));
E void FDECL(set_tin_variety, (struct obj *, int));
E int FDECL(tin_variety_txt, (char *, int *));
E void FDECL(tin_details, (struct obj *, int, char *));
E boolean FDECL(Popeye, (int));

/* ### end.c ### */

E void FDECL(done1, (int));
E int NDECL(done2);
#ifdef USE_TRAMPOLI
E void FDECL(done_intr, (int));
#endif
E void FDECL(done_in_by, (struct monst *, int));
#endif /* !MAKEDEFS_C && !LEV_LEX_C */
E void VDECL(panic, (const char *, ...)) PRINTF_F(1, 2) NORETURN;
#if !defined(MAKEDEFS_C) && !defined(LEV_LEX_C)
E void FDECL(done, (int));
E void FDECL(container_contents, (struct obj *, BOOLEAN_P,
                                  BOOLEAN_P, BOOLEAN_P));
E void FDECL(terminate, (int)) NORETURN;
E int NDECL(dovanquished);
E int NDECL(num_genocides);
E void FDECL(delayed_killer, (int, int, const char *));
E struct kinfo *FDECL(find_delayed_killer, (int));
E void FDECL(dealloc_killer, (struct kinfo *));
E void FDECL(save_killers, (int, int));
E void FDECL(restore_killers, (int));
E char *FDECL(build_english_list, (char *));
#if defined(PANICTRACE) && !defined(NO_SIGNAL)
E void FDECL(panictrace_setsignals, (BOOLEAN_P));
#endif

/* ### engrave.c ### */

E char *FDECL(random_engraving, (char *));
E void FDECL(wipeout_text, (char *, int, unsigned));
E boolean FDECL(can_reach_floor, (BOOLEAN_P));
E void FDECL(cant_reach_floor, (int, int, BOOLEAN_P, BOOLEAN_P));
E const char *FDECL(surface, (int, int));
E const char *FDECL(ceiling, (int, int));
E struct engr *FDECL(engr_at, (XCHAR_P, XCHAR_P));
E int FDECL(sengr_at, (const char *, XCHAR_P, XCHAR_P, BOOLEAN_P));
E void FDECL(u_wipe_engr, (int));
E void FDECL(wipe_engr_at, (XCHAR_P, XCHAR_P, XCHAR_P, BOOLEAN_P));
E void FDECL(read_engr_at, (int, int));
E void FDECL(make_engr_at, (int, int, const char *, long, XCHAR_P));
E void FDECL(del_engr_at, (int, int));
E int NDECL(freehand);
E int NDECL(doengrave);
E void NDECL(sanitize_engravings);
E void FDECL(save_engravings, (int, int));
E void FDECL(rest_engravings, (int));
E void FDECL(del_engr, (struct engr *));
E void FDECL(rloc_engr, (struct engr *));
E void FDECL(make_grave, (int, int, const char *));

/* ### exper.c ### */

E int FDECL(energyAdvancementForRoleAndSpecies, (int));
E int FDECL(experience, (struct monst *, int));
E void FDECL(more_experienced, (int, int));
E void FDECL(losexp, (const char *));
E void NDECL(newexplevel);
E void FDECL(pluslvl, (BOOLEAN_P));
E long FDECL(rndexp, (BOOLEAN_P));

/* ### explode.c ### */

E void FDECL(explode, (int, int, int, int, CHAR_P, int));
E long FDECL(scatter, (int, int, int, unsigned int, struct obj *));
E void FDECL(splatter_burning_oil, (int, int));
E void FDECL(explode_oil, (struct obj *, int, int));

/* ### extralev.c ### */

E void NDECL(makeroguerooms);
E void FDECL(corr, (int, int));
E void NDECL(makerogueghost);

/* ### files.c ### */

E char *FDECL(fname_encode, (const char *, CHAR_P, char *, char *, int));
E char *FDECL(fname_decode, (CHAR_P, char *, char *, int));
E const char *FDECL(fqname, (const char *, int, int));
E FILE *FDECL(fopen_datafile, (const char *, const char *, int));
#ifdef MFLOPPY
E void NDECL(set_lock_and_bones);
#endif
E void FDECL(set_levelfile_name, (char *, int));
E int FDECL(create_levelfile, (int, char *));
E int FDECL(open_levelfile, (int, char *));
E void FDECL(delete_levelfile, (int));
E void NDECL(clearlocks);
E int FDECL(createBonesFileForCurrentLevel, (char **, char *));
#ifdef MFLOPPY
E void NDECL(cancel_bonesfile);
#endif
E void FDECL(commitBonesFileForCurrentLevel, ());
E int FDECL(openBonesFileForCurrentLevel, (char **));
E int FDECL(deleteBonesFileForCurrentLevel, ());
E void NDECL(compress_bonesfile);
E void FDECL(set_savefile_name, (BOOLEAN_P));
#ifdef INSURANCE
E void FDECL(save_savefile_name, (int));
#endif
#ifndef MICRO
E void NDECL(set_error_savefile);
#endif
E int NDECL(create_savefile);
E int NDECL(open_savefile);
E int NDECL(delete_savefile);
E int NDECL(restore_saved_game);
E void FDECL(nh_compress, (const char *));
E void FDECL(nh_uncompress, (const char *));
E boolean FDECL(lock_file, (const char *, int, int));
E void FDECL(unlock_file, (const char *));
#ifdef USER_SOUNDS
E boolean FDECL(can_read_file, (const char *));
#endif
E boolean FDECL(read_config_file, (const char *, int));
E void FDECL(check_recordfile, (const char *));
E void NDECL(read_wizkit);
E int FDECL(read_sym_file, (int));
E int FDECL(parse_sym_line, (char *, int));
E void FDECL(paniclog, (const char *, const char *));
E int FDECL(validate_prefix_locations, (char *));
#ifdef SELECTSAVED
E char *FDECL(plname_from_file, (const char *));
#endif
E char **NDECL(get_saved_games);
E void FDECL(free_saved_games, (char **));
#ifdef SELF_RECOVER
E boolean NDECL(recover_savefile);
#endif
#ifdef SYSCF_FILE
E void NDECL(assure_syscf_file);
#endif
E int FDECL(nhclose, (int));
#ifdef HOLD_LOCKFILE_OPEN
E void NDECL(really_close);
#endif
#ifdef DEBUG
E boolean FDECL(debugcore, (const char *, BOOLEAN_P));
#endif
E boolean FDECL(tributeNovelsEnabled, ());
E void FDECL(setTributeNovelsEnabled, (boolean));
E boolean FDECL(tributeNovelStocked, ());
E void FDECL(setTributeNovelStocked, (boolean));
E boolean FDECL(tributeNovelNoticedByDeath, ());
E void FDECL(setTributeNovelNoticedByDeath, (boolean));

E int FDECL(maximumNovelPassages, ());
E int FDECL(currentNovelId, ());
E void FDECL(setCurrentNovelId, (int));
E int FDECL(currentNovelPassage, (int));
E void FDECL(setCurrentNovelPassage, (int, int));

E int FDECL(currentNovelPassageCount, ());
E void FDECL(setCurrentNovelPassageCount, (int));
E void FDECL(increaseCurrentNovelPassageCount, (int));
E void FDECL(decreaseCurrentNovelPassageCount, (int));

E boolean FDECL(read_tribute, (const char *, const char *, int,
                               char *, int, unsigned));
E boolean FDECL(Death_quote, (char *, int));

/* ### fountain.c ### */

E void FDECL(floating_above, (const char *));
E void FDECL(dogushforth, (int));
#ifdef USE_TRAMPOLI
E void FDECL(gush, (int, int, genericptr_t));
#endif
E void FDECL(dryup, (XCHAR_P, XCHAR_P, BOOLEAN_P));
E void NDECL(drinkfountain);
E void FDECL(dipfountain, (struct obj *));
E void FDECL(breaksink, (int, int));
E void NDECL(drinksink);

/* ### hack.c ### */

E anything *FDECL(uint_to_any, (unsigned));
E anything *FDECL(long_to_any, (long));
E anything *FDECL(monst_to_any, (struct monst *));
E anything *FDECL(obj_to_any, (struct obj *));
E boolean FDECL(revive_nasty, (int, int, const char *));
E void FDECL(movobj, (struct obj *, XCHAR_P, XCHAR_P));
E boolean FDECL(may_dig, (XCHAR_P, XCHAR_P));
E boolean FDECL(may_passwall, (XCHAR_P, XCHAR_P));
E boolean FDECL(badRock, (int, XCHAR_P, XCHAR_P));
E int FDECL(cant_squeeze_thru, (struct monst *));
E boolean FDECL(invocation_pos, (XCHAR_P, XCHAR_P));
E boolean FDECL(test_move, (int, int, int, int, int));
#ifdef DEBUG
E int NDECL(wiz_debug_cmd_traveldisplay);
#endif
E boolean NDECL(u_rooted);
E void NDECL(domove);
E boolean NDECL(overexertion);
E void NDECL(invocation_message);
E boolean FDECL(pooleffects, (BOOLEAN_P));
E void FDECL(spoteffects, (BOOLEAN_P));

E char* FDECL(allPlainRoomsLocatedAt, (XCHAR_P, XCHAR_P));
E char FDECL(plainRoomLocatedAt, (XCHAR_P, XCHAR_P));
E boolean FDECL(locationIsInAPlainRoom, (XCHAR_P, XCHAR_P));

E char* FDECL(allShopsLocatedAt, (XCHAR_P, XCHAR_P));
E char FDECL(shopLocatedAt, (XCHAR_P, XCHAR_P));
E boolean FDECL(locationIsInAShop, (XCHAR_P, XCHAR_P));

E char FDECL(templeLocatedAt, (XCHAR_P, XCHAR_P));
E boolean FDECL(locationIsInATemple, (XCHAR_P, XCHAR_P));

E char FDECL(vaultLocatedAt, (XCHAR_P, XCHAR_P));
E boolean FDECL(locationIsInAVault, (XCHAR_P, XCHAR_P));

E boolean FDECL(in_town, (int, int));
E void FDECL(check_special_room, (BOOLEAN_P));
E int NDECL(dopickup);
E void NDECL(lookaround);
E boolean FDECL(crawl_destination, (int, int));
E int NDECL(monster_nearby);
E void FDECL(nomul, (int));
E void FDECL(unmul, (const char *));
E void FDECL(losehp, (int, const char *, BOOLEAN_P));
E int NDECL(weight_cap);
E int NDECL(inv_weight);
E int NDECL(near_capacity);
E int FDECL(calc_capacity, (int));
E int NDECL(max_capacity);
E boolean FDECL(check_capacity, (const char *));
E int FDECL(inv_cnt, (BOOLEAN_P));
E long FDECL(money_cnt, (struct obj *));
E void FDECL(exit_java, ());

/* ### hacklib.c ### */

extern int javaGetInt(const char* classname, const char* methodname);
extern int javaGetIntFromInt(const char* classname, const char* methodname, int i);
extern int javaGetIntFromIntAndInt(const char* classname, const char* methodname, int i, int j);
extern long javaGetLong(const char* classname, const char* methodname);
extern long javaGetLongFromInt(const char* classname, const char* methodname, int i);
extern boolean javaGetBoolean(const char* classname, const char* methodname);
extern boolean javaGetBooleanFromInt(const char* classname, const char* methodname, int i);
extern boolean javaGetBooleanFromIntAndInt(const char* classname, const char* methodname, int i, int j);
extern boolean javaGetBooleanFromIntAndLong(const char* classname, const char* methodname, int i, long j);
extern jstring javaGetString(const char* classname, const char* methodname);
extern jstring javaGetStringFromInt(const char* classname, const char* methodname, int i);
extern jstring javaGetStringFromIntAndInt(const char* classname, const char* methodname, int i, int j);
extern void javaSetVoid(const char* classname, const char* methodname);
extern void javaSetInt(const char* classname, const char* methodname, int v);
extern void javaSetIntFromInt(const char* classname, const char* methodname, int i, int v);
extern void javaSetLong(const char* classname, const char* methodname, long v);
extern void javaSetLongFromInt(const char* classname, const char* methodname, int i, long v);
extern void javaSetBoolean(const char* classname, const char* methodname, boolean v);
extern void javaSetBooleanFromInt(const char* classname, const char* methodname, int i, boolean v);

E boolean FDECL(digit, (CHAR_P));
E boolean FDECL(letter, (CHAR_P));
E char FDECL(highc, (CHAR_P));
E char FDECL(lowc, (CHAR_P));
E char *FDECL(lcase, (char *));
E char *FDECL(ucase, (char *));
E char *FDECL(upstart, (char *));
E char *FDECL(mungspaces, (char *));
E char *FDECL(eos, (char *));
E boolean FDECL(str_end_is, (const char *, const char *));
E char *FDECL(strkitten, (char *, CHAR_P));
E void FDECL(copynchars, (char *, const char *, int));
E char FDECL(chrcasecpy, (int, int));
E char *FDECL(strcasecpy, (char *, const char *));
E char *FDECL(s_suffix, (const char *));
E char *FDECL(ing_suffix, (const char *));
E char *FDECL(xcrypt, (const char *, char *));
E boolean FDECL(onlyspace, (const char *));
E char *FDECL(tabexpand, (char *));
E char *FDECL(visctrl, (CHAR_P));
E char *FDECL(strsubst, (char *, const char *, const char *));
E const char *FDECL(ordin, (int));
E char *FDECL(sitoa, (int));
E int FDECL(sgn, (int));
E int FDECL(rounddiv, (long, int));
E int FDECL(dist2, (int, int, int, int));
E int FDECL(currentDungeonLevel, ());
E int FDECL(currentDungeonNumber, ());
E void FDECL(setCurrentDungeonLevel, (int));
E void FDECL(setCurrentDungeonNumber, (int));
E int FDECL(previousDungeonLevel, ());
E int FDECL(previousDungeonNumber, ());
E void FDECL(setPreviousDungeonLevel, (int));
E void FDECL(setPreviousDungeonNumber, (int));
E int FDECL(sentToDungeonLevel, ());
E int FDECL(sentToDungeonNumber, ());
E void FDECL(setSentToDungeonLevel, (int));
E void FDECL(setSentToDungeonNumber, (int));
E int FDECL(currentX, ());
E int FDECL(currentY, ());
E void FDECL(setCurrentX, (int));
E void FDECL(setCurrentY, (int));
E int FDECL(directionX, ());
E int FDECL(directionY, ());
E int FDECL(directionZ, ());
E void FDECL(setDirectionX, (int));
E void FDECL(setDirectionY, (int));
E void FDECL(setDirectionZ, (int));
E int FDECL(originalX, ());
E int FDECL(originalY, ());
E void FDECL(setOriginalX, (int));
E void FDECL(setOriginalY, (int));
E int FDECL(destinationX, ());
E int FDECL(destinationY, ());
E void FDECL(setDestinationX, (int));
E void FDECL(setDestinationY, (int));
E int FDECL(distanceSquaredToYou, (int, int));
E int FDECL(isqrt, (int));
E int FDECL(distmin, (int, int, int, int));
E boolean FDECL(linedUpWithYou, (int, int));
E boolean FDECL(pmatch, (const char *, const char *));
E boolean FDECL(pmatchi, (const char *, const char *));
E boolean FDECL(pmatchz, (const char *, const char *));
E boolean FDECL(youMoved, ());
E void FDECL(setYouMoved, (boolean));
E int FDECL(lastTurningDirection, ());
E void FDECL(setLastTurningDirection, (int));

E boolean FDECL(currentlyTrapped, ());
E int FDECL(currentTrapType, ());
E void FDECL(setCurrentTrapType, (int));
E int FDECL(currentTrapTimeout, ());
E void FDECL(setCurrentTrapTimeout, (int));

E long FDECL(sleepingSinceMove, ());
E void FDECL(setSleepingSinceMove, (long));
E boolean FDECL(youAreAware, ());
E boolean FDECL(youAreUnaware, ());

E boolean FDECL(underwater, ());
E void FDECL(setUnderwater, (boolean));
E boolean FDECL(inWater, ());
E void FDECL(setInWater, (boolean));

E boolean FDECL(swallowed, ());
E void FDECL(setSwallowed, (boolean));

E boolean FDECL(buried, ());
E void FDECL(setBuried, (boolean));

E int FDECL(timeInVault, ());
E void FDECL(setTimeInVault, (int));
E void FDECL(increaseTimeInVault, (int));
E void FDECL(decreaseTimeInVault, (int));

E int FDECL(timeSinceBeingSwallowed, ());
E void FDECL(setTimeSinceBeingSwallowed, (int));
E void FDECL(increaseTimeSinceBeingSwallowed, (int));
E void FDECL(decreaseTimeSinceBeingSwallowed, (int));

E int FDECL(timeSinceBecomingADemigod, ());
E void FDECL(setTimeSinceBecomingADemigod, (int));
E void FDECL(increaseTimeSinceBecomingADemigod, (int));
E void FDECL(decreaseTimeSinceBecomingADemigod, (int));

E int FDECL(currentExperienceLevel, ());
E void FDECL(setCurrentExperienceLevel, (int));
E int FDECL(highestExperienceLevelSoFar, ());
E void FDECL(setHighestExperienceLevelSoFar, (int));

E long FDECL(currentXP, ());
E void FDECL(setCurrentXP, (long));

E long FDECL(currentScore, ());
E void FDECL(setCurrentScore, (long));
E void FDECL(increaseCurrentScore, (long));
E void FDECL(decreaseCurrentScore, (long));

E int FDECL(originalAlignmentBase, ());
E void FDECL(setOriginalAlignmentBase, (int));
E int FDECL(currentAlignmentBase, ());
E void FDECL(setCurrentAlignmentBase, (int));

E int FDECL(currentAlignmentType, ());
E void FDECL(setCurrentAlignmentType, (int));
E int FDECL(currentAlignmentRecord, ());
E void FDECL(setCurrentAlignmentRecord, (int));

E int FDECL(currentLuck, ());
E void FDECL(setCurrentLuck, (int));
E void FDECL(increaseCurrentLuck, (int));
E void FDECL(decreaseCurrentLuck, (int));
E int FDECL(luckBonus, ());
E void FDECL(setLuckBonus, (int));
E int FDECL(currentLuckWithBonus, ());

E int FDECL(minimumPossibleLuck, ());
E int FDECL(maximumPossibleLuck, ());

E int FDECL(blessings, ());
E void FDECL(setBlessings, (int));
E void FDECL(increaseBlessings, (int));
E void FDECL(decreaseBlessings, (int));

E int FDECL(timeToNextBlessing, ());
E void FDECL(setTimeToNextBlessing, (int));
E void FDECL(increaseTimeToNextBlessing, (int));
E void FDECL(decreaseTimeToNextBlessing, (int));

E int FDECL(divineWrath, ());
E void FDECL(setDivineWrath, (int));
E void FDECL(increaseDivineWrath, (int));
E void FDECL(decreaseDivineWrath, (int));

E long FDECL(cleansedSinceMove, ());
E void FDECL(setCleansedSinceMove, (long));

E boolean FDECL(invulnerableWhilePraying, ());
E void FDECL(setInvulnerableWhilePraying, (boolean));

E int FDECL(toHitModifier, ());
E void FDECL(setToHitModifier, (int));
E void FDECL(increaseToHitModifier, (int));
E void FDECL(decreaseToHitModifier, (int));

E int FDECL(damageBonus, ());
E void FDECL(setDamageBonus, (int));
E void FDECL(increaseDamageBonus, (int));
E void FDECL(decreaseDamageBonus, (int));

E boolean FDECL(usingTwoWeapons, ());
E void FDECL(setUsingTwoWeapons, (boolean));

E int FDECL(armorClass, ());
E void FDECL(setArmorClass, (int));

E int FDECL(armorBonusFromProtectionSpell, ());
E void FDECL(setArmorBonusFromProtectionSpell, (int));
E void FDECL(increaseArmorBonusFromProtectionSpell, (int));
E void FDECL(decreaseArmorBonusFromProtectionSpell, (int));
E int FDECL(protectionSpellPointDuration, ());
E void FDECL(setProtectionSpellPointDuration, (int));
E int FDECL(protectionSpellPointCountdown, ());
E void FDECL(setProtectionSpellPointCountdown, (int));
E void FDECL(increaseProtectionSpellPointCountdown, (int));
E void FDECL(decreaseProtectionSpellPointCountdown, (int));

E boolean FDECL(areYouPolymorphed, ());
E boolean FDECL(areYouOrcish, ());
E boolean FDECL(areYouElvish, ());

E int FDECL(originalMonsterNumber, ());
E void FDECL(setOriginalMonsterNumber, (int));

E int FDECL(currentMonsterNumber, ());
E void FDECL(setCurrentMonsterNumber, (int));

E int FDECL(timeRemainingAsMonster, ());
E void FDECL(setTimeRemainingAsMonster, (int));
E void FDECL(increaseTimeRemainingAsMonster, (int));
E void FDECL(decreaseTimeRemainingAsMonster, (int));
E void FDECL(multiplyTimeRemainingAsMonster, (int));

E boolean FDECL(lurking, ());
E void FDECL(setLurking, (boolean));

E boolean FDECL(inherentlyFemale, ());
E void FDECL(setInherentlyFemale, (boolean));

E int FDECL(lycanthropeType, ());
E void FDECL(setLycanthropeType, (int));
E boolean FDECL(youHateSilver, ());

E int FDECL(deathCount, ());
E void FDECL(setDeathCount, (int));
E void FDECL(increaseDeathCount, (int));
E void FDECL(decreaseDeathCount, (int));

E int FDECL(ariseFromGraveAsMonster, ());
E void FDECL(setAriseFromGraveAsMonster, (int));

E int FDECL(currentHitPoints, ());
E void FDECL(setCurrentHitPoints, (int));
E void FDECL(increaseCurrentHitPoints, (int));
E void FDECL(decreaseCurrentHitPoints, (int));

E int FDECL(maximumHitPoints, ());
E void FDECL(setMaximumHitPoints, (int));
E void FDECL(increaseMaximumHitPoints, (int));
E void FDECL(decreaseMaximumHitPoints, (int));

E int FDECL(hitPointIncreasePerLevel, (int));
E void FDECL(setHitPointIncreasePerLevel, (int, int));

E int FDECL(currentHitPointsAsMonster, ());
E void FDECL(setCurrentHitPointsAsMonster, (int));
E void FDECL(increaseCurrentHitPointsAsMonster, (int));
E void FDECL(decreaseCurrentHitPointsAsMonster, (int));

E int FDECL(maximumHitPointsAsMonster, ());
E void FDECL(setMaximumHitPointsAsMonster, (int));
E void FDECL(increaseMaximumHitPointsAsMonster, (int));
E void FDECL(decreaseMaximumHitPointsAsMonster, (int));
E void FDECL(multiplyMaximumHitPointsAsMonster, (int));

E int FDECL(currentMagicalEnergy, ());
E void FDECL(setCurrentMagicalEnergy, (int));
E void FDECL(increaseCurrentMagicalEnergy, (int));
E void FDECL(decreaseCurrentMagicalEnergy, (int));

E int FDECL(maximumMagicalEnergy, ());
E void FDECL(setMaximumMagicalEnergy, (int));
E void FDECL(increaseMaximumMagicalEnergy, (int));
E void FDECL(decreaseMaximumMagicalEnergy, (int));

E int FDECL(magicalEnergyIncreasePerLevel, (int));
E void FDECL(setMagicalEnergyIncreasePerLevel, (int, int));

E long FDECL(remainingGallopTime, ());
E void FDECL(setRemainingGallopTime, (long));
E void FDECL(increaseRemainingGallopTime, (long));
E void FDECL(decreaseRemainingGallopTime, (long));

E int FDECL(turnsRiddenSinceLastSkillCheck, ());
E void FDECL(setTurnsRiddenSinceLastSkillCheck, (int));
E void FDECL(increaseTurnsRiddenSinceLastSkillCheck, (int));
E void FDECL(decreaseTurnsRiddenSinceLastSkillCheck, (int));

E int FDECL(currentNutrition, ());
E void FDECL(setCurrentNutrition, (int));
E void FDECL(increaseCurrentNutrition, (int));
E void FDECL(decreaseCurrentNutrition, (int));

E int FDECL(currentHungerState, ());
E void FDECL(setCurrentHungerState, (int));

E boolean FDECL(canSenseUnsafeFood, ());
E void FDECL(setCanSenseUnsafeFood, (boolean));

E boolean FDECL(sickWithFoodPoisoning, ());
E void FDECL(setSickWithFoodPoisoning, (boolean));

E boolean FDECL(sickWithIllness, ());
E void FDECL(setSickWithIllness, (boolean));

E int FDECL(creamed, ());
E void FDECL(setCreamed, (int));
E void FDECL(increaseCreamed, (int));
E void FDECL(decreaseCreamed, (int));

E int FDECL(abilityToConfuseMonsters, ());
E void FDECL(setAbilityToConfuseMonsters, (int));
E void FDECL(increaseAbilityToConfuseMonsters, (int));
E void FDECL(decreaseAbilityToConfuseMonsters, (int));

E int FDECL(nightVisionRange, ());
E int FDECL(xRayVisionRange, ());
E void FDECL(setNightVisionRange, (int));
E void FDECL(setXRayVisionRange, (int));

E boolean FDECL(feltBall, ());
E boolean FDECL(feltChain, ());
E void FDECL(setFeltBall, (boolean));
E void FDECL(setFeltChain, (boolean));
E int FDECL(glyphUnderBall, ());
E int FDECL(glyphUnderChain, ());
E void FDECL(setGlyphUnderBall, (int));
E void FDECL(setGlyphUnderChain, (int));
E int FDECL(ballAndChainOrder, ());
E void FDECL(setBallAndChainOrder, (int));

E int FDECL(giftsBestowed, ());
E void FDECL(setGiftsBestowed, (int));
E void FDECL(increaseGiftsBestowed, (int));
E void FDECL(decreaseGiftsBestowed, (int));

E int FDECL(startingMoney, ());
E void FDECL(setStartingMoney, (int));
E void FDECL(increaseStartingMoney, (int));
E void FDECL(decreaseStartingMoney, (int));

#ifndef STRNCMPI
E int FDECL(strncmpi, (const char *, const char *, int));
#endif
#ifndef STRSTRI
E char *FDECL(strstri, (const char *, const char *));
#endif
E boolean
FDECL(fuzzymatch, (const char *, const char *, const char *, BOOLEAN_P));
E void NDECL(setrandom);
E time_t NDECL(getnow);
E int NDECL(getyear);
#if 0
E char *FDECL(yymmdd, (time_t));
#endif
E long FDECL(yyyymmdd, (time_t));
E long FDECL(hhmmss, (time_t));
E char *FDECL(yyyymmddhhmmss, (time_t));
E time_t FDECL(time_from_yyyymmddhhmmss, (char *));
E int NDECL(phase_of_the_moon);
E boolean NDECL(friday_13th);
E int NDECL(night);
E int NDECL(midnight);

E boolean FDECL(achieved, (int));
E void FDECL(setAchieved, (int, boolean));
E int FDECL(knownAchievementTypes, ());

E boolean FDECL(haveSpecialItem, (int));
E void FDECL(setHaveSpecialItem, (int, boolean));
E int FDECL(knownSpecialItems, ());

E int FDECL(numBonesFilesLoaded, ());
E void FDECL(setNumBonesFilesLoaded, (int));
E void FDECL(increaseNumBonesFilesLoaded, (int));
E void FDECL(decreaseNumBonesFilesLoaded, (int));

E boolean FDECL(permanentlyBlind, ());
E void FDECL(setPermanentlyBlind, (boolean));
E boolean FDECL(nudist, ());
E void FDECL(setNudist, (boolean));

E boolean FDECL(veganConduct, ());
E void FDECL(setVeganConduct, (boolean));
E boolean FDECL(vegetarianConduct, ());
E void FDECL(setVegetarianConduct, (boolean));
E boolean FDECL(foodlessConduct, ());
E void FDECL(setFoodlessConduct, (boolean));
E boolean FDECL(atheistConduct, ());
E void FDECL(setAtheistConduct, (boolean));

E boolean FDECL(illiterateConduct, ());
E long FDECL(literacyCount, ());
E void FDECL(setLiteracyCount, (long));
E void FDECL(incrementLiteracyCount, (long));

E boolean FDECL(unpolypilingConduct, ());
E long FDECL(polypileCount, ());
E void FDECL(setPolypileCount, (long));
E void FDECL(incrementPolypileCount, (long));

E boolean FDECL(unpolyselfingConduct, ());
E long FDECL(polyselfCount, ());
E void FDECL(setPolyselfCount, (long));
E void FDECL(incrementPolyselfCount, (long));

E boolean FDECL(wishlessConduct, ());
E long FDECL(wishCount, ());
E void FDECL(setWishCount, (long));
E void FDECL(incrementWishCount, (long));

E boolean FDECL(artifactWishlessConduct, ());
E long FDECL(artifactWishCount, ());
E void FDECL(setArtifactWishCount, (long));
E void FDECL(incrementArtifactWishCount, (long));

E boolean FDECL(hitlessConduct, ());
E long FDECL(weaponHitCount, ());
E void FDECL(setWeaponHitCount, (long));
E void FDECL(incrementWeaponHitCount, (long));

E boolean FDECL(pacifistConduct, ());
E void FDECL(setPacifistConduct, (boolean));

E boolean FDECL(gotMinorOracle, ());
E void FDECL(setGotMinorOracle, (boolean));
E boolean FDECL(gotMajorOracle, ());
E void FDECL(setGotMajorOracle, (boolean));
E boolean FDECL(hasReadTribute, ());
E void FDECL(setHasReadTribute, (boolean));
E boolean FDECL(calledByQuestLeader, ());
E void FDECL(setCalledByQuestLeader, (boolean));
E boolean FDECL(expelledFromQuestDungeon, ());
E void FDECL(setExpelledFromQuestDungeon, (boolean));
E boolean FDECL(completedQuest, ());
E void FDECL(setCompletedQuest, (boolean));

E boolean FDECL(haveOpenedDrawbridge, ());
E void FDECL(setHaveOpenedDrawbridge, (boolean));
E boolean FDECL(haveInvokedGateToSanctum, ());
E void FDECL(setHaveInvokedGateToSanctum, (boolean));
E boolean FDECL(enteredGehennomViaValley, ());
E void FDECL(setEnteredGehennomViaValley, (boolean));

E int FDECL(handOfElberethLevel, ());
E void FDECL(setHandOfElberethLevel, (int));

E boolean FDECL(becameDemigod, ());
E void FDECL(setBecameDemigod, (boolean));
E boolean FDECL(steppedOnVibratingSquare, ());
E void FDECL(setSteppedOnVibratingSquare, (boolean));
E boolean FDECL(haveAscended, ());
E void FDECL(setHaveAscended, (boolean));

E int FDECL(knowledgeOfPasstune, ());
E void FDECL(setKnowledgeOfPasstune, (int));
E void FDECL(incrementKnowledgeOfPasstune, (int));

E void FDECL(copyRoomIDs, (char*, const char*));
E boolean FDECL(oneOfRoomsHasID, (const char*, char));
E void FDECL(removeIDFromRooms, (char*, char));

E int FDECL(maximumOccupiedRoomCount, ());

E char FDECL(currentlyOccupiedRoomIDs, (int));
E void FDECL(setCurrentlyOccupiedRoomIDs, (int, char));
E void FDECL(clearCurrentlyOccupiedRoomIDs, ());
E void FDECL(copyRoomIDsToCurrentlyOccupied, (const char*));
E boolean FDECL(oneOfCurrentlyOccupiedRoomsHasID, (char));
E boolean FDECL(noneOfCurrentlyOccupiedRoomsHasID, (char));

E char FDECL(previouslyOccupiedRoomIDs, (int));
E void FDECL(setPreviouslyOccupiedRoomIDs, (int, char));
E void FDECL(copyCurrentlyOccupiedRoomIDsToPrevious, ());
E boolean FDECL(wasNotPreviouslyInRoom, (char));
E boolean FDECL(wasPreviouslyOccupyingTemple, ());

E char FDECL(freshlyEnteredRoomIDs, (int));
E void FDECL(setFreshlyEnteredRoomIDs, (int, char));
E void FDECL(clearFreshlyEnteredRoomIDs, ());
E boolean FDECL(youHaveNotFreshlyEnteredARoom, ());

E char FDECL(currentlyOccupiedShopIDs, (int));
E void FDECL(setCurrentlyOccupiedShopIDs, (int, char));
E char FDECL(mostRecentCurrentlyOccupiedShopID, ());
E void FDECL(clearCurrentlyOccupiedShopIDs, ());
E void FDECL(removeIDFromCurrentlyOccupiedShops, (char));
E boolean FDECL(youAreCurrentlyOccupyingAShop, ());
E boolean FDECL(youAreNotCurrentlyOccupyingAShop, ());
E boolean FDECL(noneOfCurrentlyOccupiedShopsHasID, (char));
E boolean FDECL(oneOfCurrentlyOccupiedShopsHasID, (char));

E char FDECL(emptyShopIDs, (int));
E void FDECL(setEmptyShopIDs, (int, char));
E boolean FDECL(noneOfEmptyShopsHasID, (char));
E void FDECL(copyCurrentlyOccupiedShopIDsToEmpty, ());

E char FDECL(previouslyOccupiedShopIDs, (int));
E void FDECL(setPreviouslyOccupiedShopIDs, (int, char));
E void FDECL(clearPreviouslyOccupiedShopIDs, ());
E void FDECL(copyCurrentlyOccupiedShopIDsToPrevious, ());
E void FDECL(copyPreviouslyOccupiedShopIDsToExited, ());
E char FDECL(mostRecentPreviouslyOccupiedShopID, ());
E boolean FDECL(youWerePreviouslyOccupyingAShop, ());
E boolean FDECL(noneOfPreviouslyOccupiedShopsHasID, (char));

E char FDECL(freshlyEnteredShopIDs, (int));
E void FDECL(setFreshlyEnteredShopIDs, (int, char));
E void FDECL(clearFreshlyEnteredShopIDs, ());
E char FDECL(mostRecentFreshlyEnteredShopID, ());
E boolean FDECL(youHaveFreshlyEnteredAShop, ());
E boolean FDECL(youHaveNotFreshlyEnteredAShop, ());

E char FDECL(freshlyExitedShopIDs, (int));
E void FDECL(setFreshlyExitedShopIDs, (int, char));
E char FDECL(mostRecentFreshlyExitedShopID, ());

E boolean FDECL(monsterHasFlag2, (int, long));
E void FDECL(setMonsterFlag2, (int, long));
E void FDECL(unsetMonsterFlag2, (int, long));

E boolean FDECL(monsterHasFlag3, (int, long));
E void FDECL(setMonsterFlag3, (int, long));
E void FDECL(unsetMonsterFlag3, (int, long));

E int FDECL(monsterColor, (int));

E int FDECL(monsterRandomizerChoiceCount, ());
E void FDECL(setMonsterRandomizerChoiceCount, (int));
E void FDECL(increaseMonsterRandomizerChoiceCount, (int));
E void FDECL(decreaseMonsterRandomizerChoiceCount, (int));
E int FDECL(monsterRandomizerChoices, (int));
E void FDECL(setMonsterRandomizerChoices, (int, int));

E javaString FDECL(yourDeityName, ());
E javaString FDECL(nameOfAlignedDeityFromYourPantheon, (int));
E javaString FDECL(titleOfAlignedDeityFromYourPantheon, (int));

E int FDECL(adventurerGenders, ());

E javaString FDECL(genderAdjective, (int));
E javaString FDECL(subjectivePronoun, (int));
E javaString FDECL(objectivePronoun, (int));
E javaString FDECL(possessivePronoun, (int));
E javaString FDECL(reflexivePronoun, (int));
E javaString FDECL(genderAbbreviation, (int));
E int FDECL(genderMask, (int));

extern int roleFilterMask();
extern void addRoleFilterMask(int mask);
extern void resetRoleFilterMask();
extern boolean roleFilter(int roleID);
extern void setRoleFilter(int roleID, boolean set);
extern boolean anyRoleFilters();
extern void resetRoleFilters();

/* ### invent.c ### */

E struct obj **FDECL(objarr_init, (int));
E void FDECL(objarr_set, (struct obj *, int, struct obj **, BOOLEAN_P));
E void FDECL(assigninvlet, (struct obj *));
E struct obj *FDECL(merge_choice, (struct obj *, struct obj *));
E int FDECL(merged, (struct obj **, struct obj **));
#ifdef USE_TRAMPOLI
E int FDECL(ckunpaid, (struct obj *));
#endif
E void FDECL(addinv_core1, (struct obj *));
E void FDECL(addinv_core2, (struct obj *));
E struct obj *FDECL(addinv, (struct obj *));
E struct obj *FDECL(hold_another_object,
                    (struct obj *, const char *, const char *, const char *));
E void FDECL(useupall, (struct obj *));
E void FDECL(useup, (struct obj *));
E void FDECL(consume_obj_charge, (struct obj *, BOOLEAN_P));
E void FDECL(freeinv_core, (struct obj *));
E void FDECL(freeinv, (struct obj *));
E void FDECL(delallobj, (int, int));
E void FDECL(delobj, (struct obj *));
E struct obj *FDECL(sobj_at, (int, int, int));
E struct obj *FDECL(nxtobj, (struct obj *, int, BOOLEAN_P));
E struct obj *FDECL(carrying, (int));
E boolean NDECL(have_lizard);
E struct obj *NDECL(u_have_novel);
E struct obj *FDECL(o_on, (unsigned int, struct obj *));
E boolean FDECL(obj_here, (struct obj *, int, int));
E boolean NDECL(wearing_armor);
E boolean FDECL(is_worn, (struct obj *));
E struct obj *FDECL(g_at, (int, int));
E struct obj *FDECL(getobj, (const char *, const char *));
E int FDECL(ggetobj, (const char *, int (*)(OBJ_P), int,
                      BOOLEAN_P, unsigned *));
E int FDECL(askchain, (struct obj **, const char *, int, int (*)(OBJ_P),
                       int (*)(OBJ_P), int, const char *));
E void FDECL(fully_identify_obj, (struct obj *));
E int FDECL(identify, (struct obj *));
E void FDECL(identify_pack, (int, BOOLEAN_P));
E void NDECL(learn_unseen_invent);
E void FDECL(prinv, (const char *, struct obj *, long));
E char *FDECL(xprname,
              (struct obj *, const char *, CHAR_P, BOOLEAN_P, long, long));
E int NDECL(ddoinv);
E char FDECL(display_inventory, (const char *, BOOLEAN_P));
E int FDECL(display_binventory, (int, int, BOOLEAN_P));
E struct obj *FDECL(display_cinventory, (struct obj *));
E struct obj *FDECL(display_minventory, (struct monst *, int, char *));
E int NDECL(dotypeinv);
E const char *FDECL(dfeature_at, (int, int, char *));
E int FDECL(look_here, (int, BOOLEAN_P));
E int NDECL(dolook);
E boolean FDECL(will_feel_cockatrice, (struct obj *, BOOLEAN_P));
E void FDECL(feel_cockatrice, (struct obj *, BOOLEAN_P));
E void FDECL(stackobj, (struct obj *));
E int NDECL(doprgold);
E int NDECL(doprwep);
E int NDECL(doprarm);
E int NDECL(doprring);
E int NDECL(dopramulet);
E int NDECL(doprtool);
E int NDECL(doprinuse);
E void FDECL(useupf, (struct obj *, long));
E char *FDECL(let_to_name, (CHAR_P, BOOLEAN_P, BOOLEAN_P));
E void NDECL(free_invbuf);
E void NDECL(reassign);
E int NDECL(doorganize);
E void NDECL(free_pickinv_cache);
E int FDECL(count_unpaid, (struct obj *));
E int FDECL(count_buc, (struct obj *, int));
E long FDECL(count_contents, (struct obj *, BOOLEAN_P, BOOLEAN_P, BOOLEAN_P));
E void FDECL(carry_obj_effects, (struct obj *));
E const char *FDECL(currency, (long));
E void FDECL(silly_thing, (const char *, struct obj *));

/* ### ioctl.c ### */

#if defined(UNIX) || defined(__BEOS__)
E void NDECL(getwindowsz);
E void NDECL(getioctls);
E void NDECL(setioctls);
#ifdef SUSPEND
E int NDECL(dosuspend);
#endif /* SUSPEND */
#endif /* UNIX || __BEOS__ */

/* ### light.c ### */

E void FDECL(new_light_source, (XCHAR_P, XCHAR_P, int, int, ANY_P *));
E void FDECL(del_light_source, (int, ANY_P *));
E void FDECL(do_light_sources, (char **));
E struct monst *FDECL(find_mid, (unsigned, unsigned));
E void FDECL(save_light_sources, (int, int, int));
E void FDECL(restore_light_sources, (int));
E void FDECL(relink_light_sources, (BOOLEAN_P));
E void NDECL(light_sources_sanity_check);
E void FDECL(obj_move_light_source, (struct obj *, struct obj *));
E boolean NDECL(any_light_source);
E void FDECL(snuff_light_source, (int, int));
E boolean FDECL(obj_sheds_light, (struct obj *));
E boolean FDECL(obj_is_burning, (struct obj *));
E void FDECL(obj_split_light_source, (struct obj *, struct obj *));
E void FDECL(obj_merge_light_sources, (struct obj *, struct obj *));
E void FDECL(obj_adjust_light_radius, (struct obj *, int));
E int FDECL(candle_light_range, (struct obj *));
E int FDECL(arti_light_radius, (struct obj *));
E const char *FDECL(arti_light_description, (struct obj *));
E int NDECL(wiz_light_sources);

/* ### lock.c ### */

#ifdef USE_TRAMPOLI
E int NDECL(forcelock);
E int NDECL(picklock);
#endif
E boolean FDECL(picking_lock, (int *, int *));
E boolean FDECL(picking_at, (int, int));
E void FDECL(breakchestlock, (struct obj *, BOOLEAN_P));
E void NDECL(reset_pick);
E int FDECL(pick_lock, (struct obj *));
E int NDECL(doforce);
E boolean FDECL(boxlock, (struct obj *, struct obj *));
E boolean FDECL(doorlock, (struct obj *, int, int));
E int NDECL(doopen);
E boolean FDECL(stumble_on_door_mimic, (int, int));
E int FDECL(doopen_indir, (int, int));
E int NDECL(doclose);

#ifdef MAC
/* These declarations are here because the main code calls them. */

/* ### macfile.c ### */

E int FDECL(maccreat, (const char *, long));
E int FDECL(macopen, (const char *, int, long));
E int FDECL(macclose, (int));
E int FDECL(macread, (int, void *, unsigned));
E int FDECL(macwrite, (int, void *, unsigned));
E long FDECL(macseek, (int, long, short));
E int FDECL(macunlink, (const char *));

/* ### macmain.c ### */

E boolean NDECL(authorize_wizard_mode);

/* ### macsnd.c ### */

E void FDECL(mac_speaker, (struct obj *, char *));

/* ### macunix.c ### */

E void FDECL(regularize, (char *));
E void NDECL(getlock);

/* ### macwin.c ### */

E void FDECL(lock_mouse_cursor, (Boolean));
E int NDECL(SanePositions);

/* ### mttymain.c ### */

E void FDECL(getreturn, (char *));
E void VDECL(msmsg, (const char *, ...));
E void NDECL(gettty);
E void NDECL(setftty);
E void FDECL(settty, (const char *));
E int NDECL(tgetch);
E void FDECL(cmov, (int x, int y));
E void FDECL(nocmov, (int x, int y));

#endif /* MAC */

/* ### mail.c ### */

#ifdef MAIL
#ifdef UNIX
E void NDECL(getmailstatus);
#endif
E void NDECL(ckmailstatus);
E void FDECL(readmail, (struct obj *));
#endif /* MAIL */

/* ### makemon.c ### */

E void FDECL(dealloc_monst, (struct monst *));
E boolean FDECL(isHomeElemental, (int));
E struct monst *FDECL(clone_mon, (struct monst *, XCHAR_P, XCHAR_P));
E int FDECL(monhp_per_lvl, (struct monst *));
E void FDECL(newmonhp, (struct monst *, int));
E struct mextra *NDECL(newmextra);
E void FDECL(copy_mextra, (struct monst *, struct monst *));
E void FDECL(dealloc_mextra, (struct monst *));
E struct monst *FDECL(makeMonsterOfType, (int, int, int, int));
E boolean FDECL(createCrittersOfType, (int, int, BOOLEAN_P));
E int NDECL(randomMonster);
E void FDECL(resetMonsterRandomizer, (int));
E int FDECL(pickMonsterTypeOfClass, (CHAR_P, int));
E int FDECL(mkclass_poly, (int));
E int FDECL(adjustMonsterLevel, (int));
E int FDECL(growUpIntoMonsterType, (struct monst *, struct monst *));
E int FDECL(mongets, (struct monst *, int));
E int FDECL(golemhp, (int));
E boolean FDECL(peacefullyMinded, (int));
E void FDECL(set_malign, (struct monst *));
E void FDECL(newmcorpsenm, (struct monst *));
E void FDECL(freemcorpsenm, (struct monst *));
E void FDECL(set_mimic_sym, (struct monst *));
E int FDECL(mbirth_limit, (int));
E void FDECL(mimic_hit_msg, (struct monst *, SHORT_P));
E void FDECL(mkmonmoney, (struct monst *, long));
E int FDECL(bagotricks, (struct obj *, BOOLEAN_P, int *));
E boolean FDECL(propagate, (int, BOOLEAN_P, BOOLEAN_P));
E boolean FDECL(youCanSmellMonster, (int));

/* ### mapglyph.c ### */

E int FDECL(mapglyph, (int, int *, int *, unsigned *, int, int));
E char *FDECL(encglyph, (int));
E void FDECL(genl_putmixed, (winid, int, const char *));

/* ### mcastu.c ### */

E int FDECL(castmu, (struct monst *, const struct Attack, BOOLEAN_P, BOOLEAN_P));
E int FDECL(buzzmu, (struct monst *, const struct Attack));

/* ### mhitm.c ### */

E int FDECL(fightm, (struct monst *));
E int FDECL(mattackm, (struct monst *, struct monst *));
E boolean FDECL(engulf_target, (struct monst *, struct monst *));
E int FDECL(mdisplacem, (struct monst *, struct monst *, BOOLEAN_P));
E void FDECL(paralyze_monst, (struct monst *, int));
E int FDECL(sleep_monst, (struct monst *, int, int));
E void FDECL(slept_monst, (struct monst *));
E void FDECL(xdrainenergym, (struct monst *, BOOLEAN_P));
E long FDECL(attk_protection, (int));
E void FDECL(rustm, (struct monst *, struct obj *));

/* ### mhitu.c ### */

E const char *FDECL(mpoisons_subj, (struct monst *, const struct Attack));
E void NDECL(u_slow_down);
E struct monst *NDECL(cloneu);
E void FDECL(expels, (struct monst *, int, BOOLEAN_P));
E const struct Attack FDECL(getMonsterAttack, (int, int, int *));
E int FDECL(mattacku, (struct monst *));
E int FDECL(magic_negation, (struct monst *));
E boolean NDECL(gulp_blnd_check);
E int FDECL(gazemu, (struct monst *, const struct Attack));
E void FDECL(mdamageu, (struct monst *, int));
E int FDECL(could_seduce, (struct monst *, struct monst *, const struct Attack));
E int FDECL(doseduce, (struct monst *));

/* ### minion.c ### */

E void FDECL(newemin, (struct monst *));
E void FDECL(free_emin, (struct monst *));
E int FDECL(monster_census, (BOOLEAN_P));
E int FDECL(msummon, (struct monst *));
E void FDECL(summon_minion, (ALIGNTYP_P, BOOLEAN_P));
E int FDECL(demon_talk, (struct monst *));
E long FDECL(bribe, (struct monst *));
E int FDECL(dprince, (ALIGNTYP_P));
E int FDECL(dlord, (ALIGNTYP_P));
E int NDECL(llord);
E int FDECL(ndemon, (ALIGNTYP_P));
E int NDECL(lminion);
E void FDECL(lose_guardian_angel, (struct monst *));
E void NDECL(gain_guardian_angel);

/* ### mklev.c ### */

#ifdef USE_TRAMPOLI
E int FDECL(do_comp, (genericptr_t, genericptr_t));
#endif
E void NDECL(sort_rooms);
E void FDECL(add_room, (int, int, int, int, BOOLEAN_P, SCHAR_P, BOOLEAN_P));
E void FDECL(add_subroom, (struct mkroom *, int, int, int, int, BOOLEAN_P,
                           SCHAR_P, BOOLEAN_P));
E void NDECL(makecorridors);
E void FDECL(add_door, (int, int, struct mkroom *));
E void NDECL(mklev);
#ifdef SPECIALIZATION
E void FDECL(topologize, (struct mkroom *, BOOLEAN_P));
#else
E void FDECL(topologize, (struct mkroom *));
#endif
E void FDECL(place_branch, (branch *, XCHAR_P, XCHAR_P));
E boolean FDECL(occupied, (XCHAR_P, XCHAR_P));
E int FDECL(okdoor, (XCHAR_P, XCHAR_P));
E void FDECL(dodoor, (int, int, struct mkroom *));
E void FDECL(mktrap, (int, int, struct mkroom *, coord *));
E void FDECL(mkstairs, (XCHAR_P, XCHAR_P, CHAR_P, struct mkroom *));
E void NDECL(mkinvokearea);
E void FDECL(mineralize, (int, int, int, int, BOOLEAN_P));

/* ### mkmap.c ### */

E void FDECL(flood_fill_rm, (int, int, int, BOOLEAN_P, BOOLEAN_P));
E void FDECL(remove_rooms, (int, int, int, int));
/* E void FDECL(mkmap, (lev_init *)); -- need sp_lev.h for lev_init */

/* ### mkmaze.c ### */

E void FDECL(wallification, (int, int, int, int));
E void FDECL(walkfrom, (int, int, SCHAR_P));
E void FDECL(makemaz, (const char *));
E void FDECL(mazexy, (coord *));
E void NDECL(bound_digging);
E void FDECL(mkportal, (XCHAR_P, XCHAR_P, XCHAR_P, XCHAR_P));
E boolean FDECL(bad_location, (XCHAR_P, XCHAR_P, XCHAR_P, XCHAR_P,
                               XCHAR_P, XCHAR_P));
E void FDECL(place_lregion, (XCHAR_P, XCHAR_P, XCHAR_P, XCHAR_P, XCHAR_P,
                             XCHAR_P, XCHAR_P, XCHAR_P, XCHAR_P, d_level *));
E void NDECL(fumaroles);
E void NDECL(movebubbles);
E void NDECL(water_friction);
E void FDECL(save_waterlevel, (int, int));
E void FDECL(restore_waterlevel, (int));
E const char *FDECL(waterbody_name, (XCHAR_P, XCHAR_P));

/* ### mkobj.c ### */

E struct oextra *NDECL(newoextra);
E void FDECL(copy_oextra, (struct obj *, struct obj *));
E void FDECL(dealloc_oextra, (struct obj *));
E void FDECL(newomonst, (struct obj *));
E void FDECL(free_omonst, (struct obj *));
E void FDECL(newomid, (struct obj *));
E void FDECL(free_omid, (struct obj *));
E void FDECL(newolong, (struct obj *));
E void FDECL(free_olong, (struct obj *));
E void FDECL(new_omailcmd, (struct obj *, const char *));
E void FDECL(free_omailcmd, (struct obj *));
E struct obj *FDECL(mkobj_at, (CHAR_P, int, int, BOOLEAN_P));
E struct obj *FDECL(mksobj_at, (int, int, int, BOOLEAN_P, BOOLEAN_P));
E struct obj *FDECL(mkobj, (CHAR_P, BOOLEAN_P));
E int NDECL(rndmonnum);
E boolean FDECL(bogon_is_pname, (CHAR_P));
E struct obj *FDECL(splitobj, (struct obj *, long));
E struct obj *FDECL(unsplitobj, (struct obj *));
E void NDECL(clear_splitobjs);
E void FDECL(replace_object, (struct obj *, struct obj *));
E void FDECL(bill_dummy_object, (struct obj *));
E void FDECL(costly_alteration, (struct obj *, int));
E struct obj *FDECL(mksobj, (int, BOOLEAN_P, BOOLEAN_P));
E int FDECL(bcsign, (struct obj *));
E int FDECL(weight, (struct obj *));
E struct obj *FDECL(mkgold, (long, int, int));
E struct obj *FDECL(makeCorpseOrStatue, (int, struct monst *, int, int, int, unsigned));
E int FDECL(corpse_revive_type, (struct obj *));
E struct obj *FDECL(obj_attach_mid, (struct obj *, unsigned));
E struct monst *FDECL(get_mtraits, (struct obj *, BOOLEAN_P));
E struct obj *FDECL(mk_tt_object, (int, int, int));
E struct obj *FDECL(makeNamedObject, (int, int, int, int, const char *));
E struct obj *FDECL(rnd_treefruit_at, (int, int));
E void FDECL(set_corpsenm, (struct obj *, int));
E void FDECL(start_corpse_timeout, (struct obj *));
E void FDECL(bless, (struct obj *));
E void FDECL(unbless, (struct obj *));
E void FDECL(curse, (struct obj *));
E void FDECL(uncurse, (struct obj *));
E void FDECL(blessorcurse, (struct obj *, int));
E boolean FDECL(is_flammable, (struct obj *));
E boolean FDECL(is_rottable, (struct obj *));
E void FDECL(place_object, (struct obj *, int, int));
E void FDECL(remove_object, (struct obj *));
E void FDECL(discard_minvent, (struct monst *));
E void FDECL(obj_extract_self, (struct obj *));
E void FDECL(extract_nobj, (struct obj *, struct obj **));
E void FDECL(extract_nexthere, (struct obj *, struct obj **));
E int FDECL(add_to_minv, (struct monst *, struct obj *));
E struct obj *FDECL(add_to_container, (struct obj *, struct obj *));
E void FDECL(add_to_migration, (struct obj *));
E void FDECL(add_to_buried, (struct obj *));
E void FDECL(dealloc_obj, (struct obj *));
E void FDECL(obj_ice_effects, (int, int, BOOLEAN_P));
E long FDECL(peek_at_iced_corpse_age, (struct obj *));
E int FDECL(hornoplenty, (struct obj *, BOOLEAN_P));
E void NDECL(obj_sanity_check);
E struct obj *FDECL(obj_nexto, (struct obj *));
E struct obj *FDECL(obj_nexto_xy, (int, int, int, unsigned));
E struct obj *FDECL(obj_absorb, (struct obj **, struct obj **));
E struct obj *FDECL(obj_meld, (struct obj **, struct obj **));

/* ### mkroom.c ### */

E void FDECL(mkroom, (int));
E void FDECL(fill_zoo, (struct mkroom *));
E int NDECL(antHoleMonsterType);
E boolean FDECL(nexttodoor, (int, int));
E boolean FDECL(has_dnstairs, (struct mkroom *));
E boolean FDECL(has_upstairs, (struct mkroom *));
E int FDECL(somex, (struct mkroom *));
E int FDECL(somey, (struct mkroom *));
E boolean FDECL(inside_room, (struct mkroom *, XCHAR_P, XCHAR_P));
E boolean FDECL(somexy, (struct mkroom *, coord *));
E void FDECL(mkundead, (coord *, BOOLEAN_P, int));
E int NDECL(throneRoomMonsterType);
E void FDECL(save_rooms, (int));
E void FDECL(rest_rooms, (int));
E struct mkroom *FDECL(search_special, (SCHAR_P));
E int FDECL(cmap_to_type, (int));

/* ### mon.c ### */

E void NDECL(mon_sanity_check);
E int FDECL(undead_to_corpse, (int));
E int FDECL(genus, (int, int));
E int FDECL(pm_to_cham, (int));
E int FDECL(minliquid, (struct monst *));
E int NDECL(movemon);
E int FDECL(meatmetal, (struct monst *));
E int FDECL(meatobj, (struct monst *));
E void FDECL(mpickgold, (struct monst *));
E boolean FDECL(mpickstuff, (struct monst *, const char *));
E int FDECL(curr_mon_load, (struct monst *));
E int FDECL(max_mon_load, (struct monst *));
E int FDECL(can_carry, (struct monst *, struct obj *));
E int FDECL(mfndpos, (struct monst *, coord *, long *, long));
E boolean FDECL(monnear, (struct monst *, int, int));
E void NDECL(dmonsfree);
E int FDECL(mcalcmove, (struct monst *));
E void NDECL(mcalcdistress);
E void FDECL(replmon, (struct monst *, struct monst *));
E void FDECL(relmon, (struct monst *, struct monst **));
E struct obj *FDECL(mlifesaver, (struct monst *));
E boolean FDECL(corpse_chance, (struct monst *, struct monst *, BOOLEAN_P));
E void FDECL(mondead, (struct monst *));
E void FDECL(mondied, (struct monst *));
E void FDECL(mongone, (struct monst *));
E void FDECL(monstone, (struct monst *));
E void FDECL(monkilled, (struct monst *, const char *, int));
E void FDECL(unstuck, (struct monst *));
E void FDECL(killed, (struct monst *));
E void FDECL(xkilled, (struct monst *, int));
E void FDECL(mon_to_stone, (struct monst *));
E void FDECL(mnexto, (struct monst *));
E void FDECL(maybe_mnexto, (struct monst *));
E boolean FDECL(mnearto, (struct monst *, XCHAR_P, XCHAR_P, BOOLEAN_P));
E void FDECL(m_respond, (struct monst *));
E void FDECL(setmangry, (struct monst *));
E void FDECL(wakeup, (struct monst *));
E void NDECL(wake_nearby);
E void FDECL(wake_nearto, (int, int, int));
E void FDECL(seemimic, (struct monst *));
E void NDECL(rescham);
E void NDECL(restartcham);
E void FDECL(restore_cham, (struct monst *));
E boolean FDECL(hideunder, (struct monst *));
E void FDECL(hide_monst, (struct monst *));
E void FDECL(mon_animal_list, (BOOLEAN_P));
E int FDECL(select_newcham_form, (struct monst *));
E void FDECL(monsterGenderFromType, (struct monst *, int));
E boolean FDECL(changeChameleonToType, (struct monst *, int, BOOLEAN_P, BOOLEAN_P));
E int FDECL(can_be_hatched, (int));
E int FDECL(egg_type_from_parent, (int, BOOLEAN_P));
E boolean FDECL(dead_species, (int, BOOLEAN_P));
E void NDECL(kill_genocided_monsters);
E void FDECL(golemeffects, (struct monst *, int, int));
E boolean FDECL(angry_guards, (BOOLEAN_P));
E void NDECL(pacify_guards);
E void FDECL(decide_to_shapeshift, (struct monst *, int));

/* ### mondata.c ### */

E void FDECL(setMonsterType, (struct monst *, int, int));
E boolean FDECL(monsterHasAttackWithDamageType, (int, const int, const int));
E const struct Attack FDECL(monsterAttackWithDamageType, (int, const int, const int));
E boolean FDECL(monsterHasAttackType, (int, int));
E boolean FDECL(monsterDoesNotAttack, (int));
E boolean FDECL(monsterPolymorphsWhenStoned, (int));
E boolean FDECL(resists_drli, (struct monst *));
E boolean FDECL(resists_magm, (struct monst *));
E boolean FDECL(resists_blnd, (struct monst *));
E boolean
FDECL(can_blnd, (struct monst *, struct monst *, UCHAR_P, struct obj *));
E boolean FDECL(monsterHasRangedAttack, (int));
E boolean FDECL(monsterTypeHatesSilver, (int));
E boolean FDECL(monsterHatesSilver, (struct monst *));
E boolean FDECL(monsterPassesThroughBars, (int));
E boolean FDECL(can_blow, (struct monst *));
E boolean FDECL(can_be_strangled, (struct monst *));
E boolean FDECL(monsterCanTrack, (int));
E boolean FDECL(breaksOutOfArmor, (int));
E boolean FDECL(slipsOutOfArmor, (int));
E boolean FDECL(monsterSticksInCombat, (int));
E boolean FDECL(monsterCannotVomit, (int));
E int FDECL(numberOfHorns, (int));
/* E boolean FDECL(canseemon, (struct monst *)); */
E boolean FDECL(monsterTypeCanCauseDamageType, (int, int));
E int FDECL(max_passive_dmg, (struct monst *, struct monst *));
E boolean FDECL(sameMonsterType, (int, int));
E int FDECL(name_to_mon, (const char *));
E int FDECL(name_to_monclass, (const char *, int *));
E int FDECL(gender, (struct monst *));
E int FDECL(pronoun_gender, (struct monst *));
E boolean FDECL(levl_follower, (struct monst *));
E int FDECL(little_to_big, (int));
E int FDECL(big_to_little, (int));
E const char *FDECL(locomotionVerb, (int, const char *));
E const char *FDECL(staggerVerb, (int, const char *));
E const char *FDECL(fireDescription, (int, const struct Attack));
E int FDECL(raceType, (struct monst *));
E boolean FDECL(monsterHasSenseOfSmell, (int));

E int FDECL(emitsLightWithRange, (int));
E boolean FDECL(isFloater, (int));
E boolean FDECL(isGolem, (int));
E boolean FDECL(isNonliving, (int));
E boolean FDECL(isReviver, (int));
E boolean FDECL(isUnicorn, (int));
E boolean FDECL(isVampire, (int));
E boolean FDECL(isNoncorporeal, (int));
E boolean FDECL(isSlimeproof, (int));
E boolean FDECL(isVeganOption, (int));
E boolean FDECL(isVegetarianOption, (int));
E boolean FDECL(isWhirly, (int));
E boolean FDECL(corpseOrStatueIsUnique, (int));
E boolean FDECL(isSilent, (int));
E boolean FDECL(isBigMonster, (int));
E boolean FDECL(isVerySmallMonster, (int));
E boolean FDECL(cannotWieldThings, (int));
E boolean FDECL(cannotUseTwoWeapons, (int));
E boolean FDECL(isFlyer, (int));

E boolean FDECL(isClinger, (int));
E boolean FDECL(isSwimmer, (int));
E boolean FDECL(doesNotBreathe, (int));
E boolean FDECL(isAmphibious, (int));
E boolean FDECL(passesThroughWalls, (int));
E boolean FDECL(isAmorphous, (int));
E boolean FDECL(isTunneler, (int));
E boolean FDECL(needsPickaxe, (int));
E boolean FDECL(hidesUnderStuff, (int));
E boolean FDECL(isHider, (int));
E boolean FDECL(hasEyes, (int));
E int FDECL(eyeCount, (int));
E boolean FDECL(hasNoHands, (int));
E boolean FDECL(hasNoLimbs, (int));
E boolean FDECL(doesNotTakeStuff, (int));
E boolean FDECL(hasAHead, (int));
E boolean FDECL(isUnsolid, (int));
E boolean FDECL(isMindless, (int));
E boolean FDECL(isHumanoid, (int));
E boolean FDECL(isAnimal, (int));
E boolean FDECL(isSlithy, (int));
E boolean FDECL(isThickSkinned, (int));
E boolean FDECL(laysEggs, (int));
E boolean FDECL(regenerates, (int));
E boolean FDECL(perceivesTheInvisible, (int));
E boolean FDECL(canTeleport, (int));
E boolean FDECL(canControlTeleport, (int));
E boolean FDECL(isAcidic, (int));
E boolean FDECL(isPoisonous, (int));
E boolean FDECL(isCarnivorous, (int));
E boolean FDECL(isHerbivorous, (int));
E boolean FDECL(isMetallivorous, (int));

E boolean FDECL(okToPolymorphInto, (int));
E boolean FDECL(isShapeshifter, (int));
E boolean FDECL(isUndead, (int));
E boolean FDECL(isWere, (int));
E boolean FDECL(isElf, (int));
E boolean FDECL(isDwarf, (int));
E boolean FDECL(isGnome, (int));
E boolean FDECL(isOrc, (int));
E boolean FDECL(isHuman, (int));
E boolean FDECL(isOfYourRace, (int));
E boolean FDECL(isBat, (int));

E boolean FDECL(isGiant, (int));
E boolean FDECL(isDomestic, (int));
E boolean FDECL(isDemon, (int));
E boolean FDECL(isMercenary, (int));
E boolean FDECL(isMale, (int));
E boolean FDECL(isFemale, (int));
E boolean FDECL(isNeuter, (int));
E boolean FDECL(isWanderer, (int));
E boolean FDECL(isAlwaysHostile, (int));
E boolean FDECL(isAlwaysPeaceful, (int));
E boolean FDECL(racialHostility, (int, long));
E boolean FDECL(racialFriendship, (int, long));
E boolean FDECL(isExtraNasty, (int));
E boolean FDECL(isStrongMonster, (int));

E boolean FDECL(throwsRocks, (int));
E boolean FDECL(typeIsProperName, (int));
E boolean FDECL(isLord, (int));
E boolean FDECL(isPrince, (int));
E boolean FDECL(isNamelessMajorDemon, (int));
E boolean FDECL(isDemonLord, (int));
E boolean FDECL(isDemonPrince, (int));
E boolean FDECL(isMinion, (int));
E boolean FDECL(isLawfulMinion, (int));
E boolean FDECL(likesGold, (int));
E boolean FDECL(likesGems, (int));
E boolean FDECL(likesObjects, (int));
E boolean FDECL(likesMagicItems, (int));

E boolean FDECL(isCovetous, (int));
E boolean FDECL(hasInfravision, (int));
E boolean FDECL(isInfravisible, (int));
E boolean FDECL(isDisplacer, (int));

E boolean FDECL(allowsCloseApproach, (int));
E boolean FDECL(touchPetrifies, (int));
E boolean FDECL(fleshPetrifies, (int));
E boolean FDECL(isMindFlayer, (int));
E boolean FDECL(likesFire, (int));
E boolean FDECL(likesLava, (int));
E boolean FDECL(befriendWithObject, (int, struct obj*));

E boolean FDECL(hasHorns, (int));
E boolean FDECL(isFlaming, (int));
E boolean FDECL(isWooden, (int));
E boolean FDECL(isTelepathic, (int));
E boolean FDECL(isArmed, (int));
E boolean FDECL(hasBreathWeapon, (int));
E boolean FDECL(cannotWearArmor, (int));
E boolean FDECL(makesWebs, (int));
E boolean FDECL(isLongWorm, (int));
E boolean FDECL(isMonsterPlayer, (int));
E boolean FDECL(isWatchman, (int));
E boolean FDECL(isRiderOfTheApocalypse, (int));
E boolean FDECL(isPlaceholderMonster, (int));
E boolean FDECL(isInvisibleMonsterType, (int));

/* ### monmove.c ### */

E boolean FDECL(itsstuck, (struct monst *));
E boolean FDECL(mb_trapped, (struct monst *));
E boolean FDECL(monhaskey, (struct monst *, BOOLEAN_P));
E void FDECL(mon_regen, (struct monst *, BOOLEAN_P));
E int FDECL(dochugw, (struct monst *));
E boolean FDECL(onscary, (int, int, struct monst *));
E void FDECL(monflee, (struct monst *, int, BOOLEAN_P, BOOLEAN_P));
E void FDECL(mon_yells, (struct monst *, const char *));
E int FDECL(dochug, (struct monst *));
E int FDECL(m_move, (struct monst *, int));
E void FDECL(dissolve_bars, (int, int));
E boolean FDECL(closed_door, (int, int));
E boolean FDECL(accessible, (int, int));
E void FDECL(set_apparxy, (struct monst *));
E boolean FDECL(can_ooze, (struct monst *));
E boolean FDECL(can_fog, (struct monst *));
E boolean FDECL(should_displace,
                (struct monst *, coord *, long *, int, XCHAR_P, XCHAR_P));
E boolean FDECL(undesirable_disp, (struct monst *, XCHAR_P, XCHAR_P));

/* ### monst.c ### */

E void NDECL(monst_init);

/* ### mplayer.c ### */

E struct monst *FDECL(makeMonsterPlayerOfType, (int, XCHAR_P, XCHAR_P, BOOLEAN_P));
E void FDECL(create_mplayers, (int, BOOLEAN_P));
E void FDECL(mplayer_talk, (struct monst *));

#if defined(MICRO) || defined(WIN32)

/* ### msdos.c,os2.c,tos.c,winnt.c ### */

#ifndef WIN32
E int NDECL(tgetch);
#endif
#ifndef TOS
E char NDECL(switchar);
#endif
#ifndef __GO32__
E long FDECL(freediskspace, (char *));
#ifdef MSDOS
E int FDECL(findfirst_file, (char *));
E int NDECL(findnext_file);
E long FDECL(filesize_nh, (char *));
#else
E int FDECL(findfirst, (char *));
E int NDECL(findnext);
E long FDECL(filesize, (char *));
#endif /* MSDOS */
E char *NDECL(foundfile_buffer);
#endif /* __GO32__ */
E void FDECL(chdrive, (char *));
#ifndef TOS
E void NDECL(disable_ctrlP);
E void NDECL(enable_ctrlP);
#endif
#if defined(MICRO) && !defined(WINNT)
E void NDECL(get_scr_size);
#ifndef TOS
E void FDECL(gotoxy, (int, int));
#endif
#endif
#ifdef TOS
E int FDECL(_copyfile, (char *, char *));
E int NDECL(kbhit);
E void NDECL(set_colors);
E void NDECL(restore_colors);
#ifdef SUSPEND
E int NDECL(dosuspend);
#endif
#endif /* TOS */
#ifdef WIN32
E char *FDECL(get_username, (int *));
E void FDECL(nt_regularize, (char *));
E int NDECL((*nt_kbhit));
E void FDECL(Delay, (int));
#endif /* WIN32 */
#endif /* MICRO || WIN32 */

/* ### mthrowu.c ### */

E int FDECL(thitu, (int, int, struct obj *, const char *));
E int FDECL(ohitmon, (struct monst *, struct obj *, int, BOOLEAN_P));
E void FDECL(thrwmu, (struct monst *));
E int FDECL(spitmu, (struct monst *, const struct Attack));
E int FDECL(breamu, (struct monst *, const struct Attack));
E boolean FDECL(linedup, (XCHAR_P, XCHAR_P, XCHAR_P, XCHAR_P, int));
E boolean FDECL(lined_up, (struct monst *));
E struct obj *FDECL(m_carrying, (struct monst *, int));
E void FDECL(m_useupall, (struct monst *, struct obj *));
E void FDECL(m_useup, (struct monst *, struct obj *));
E void FDECL(m_throw,
             (struct monst *, int, int, int, int, int, struct obj *));
E boolean FDECL(hits_bars, (struct obj **, int, int, int, int));

/* ### muse.c ### */

E boolean FDECL(find_defensive, (struct monst *));
E int FDECL(use_defensive, (struct monst *));
E int FDECL(rnd_defensive_item, (struct monst *));
E boolean FDECL(find_offensive, (struct monst *));
#ifdef USE_TRAMPOLI
E int FDECL(mbhitm, (struct monst *, struct obj *));
#endif
E int FDECL(use_offensive, (struct monst *));
E int FDECL(rnd_offensive_item, (struct monst *));
E boolean FDECL(find_misc, (struct monst *));
E int FDECL(use_misc, (struct monst *));
E int FDECL(rnd_misc_item, (struct monst *));
E boolean FDECL(searches_for_item, (struct monst *, struct obj *));
E boolean FDECL(mon_reflects, (struct monst *, const char *));
E boolean FDECL(ureflects, (const char *, const char *));
E boolean FDECL(munstone, (struct monst *, BOOLEAN_P));
E boolean FDECL(munslime, (struct monst *, BOOLEAN_P));

/* ### music.c ### */

E void FDECL(awaken_soldiers, (struct monst *));
E int FDECL(do_play_instrument, (struct obj *));

/* ### nhlan.c ### */
#ifdef LAN_FEATURES
E void NDECL(init_lan_features);
E char *NDECL(lan_username);
#endif

/* ### nhregex.c ### */
E struct nhregex *NDECL(regex_init);
E boolean FDECL(regex_compile, (const char *, struct nhregex *));
E const char *FDECL(regex_error_desc, (struct nhregex *));
E boolean FDECL(regex_match, (const char *, struct nhregex *));
E void FDECL(regex_free, (struct nhregex *));

/* ### nttty.c ### */

#ifdef WIN32
E void NDECL(get_scr_size);
E int NDECL(nttty_kbhit);
E void FDECL(nttty_open, (int));
E void NDECL(nttty_rubout);
E int NDECL(tgetch);
E int FDECL(ntposkey, (int *, int *, int *));
E void FDECL(set_output_mode, (int));
E void NDECL(synch_cursor);
#endif

/* ### o_init.c ### */

E void NDECL(init_objects);
E void FDECL(obj_shuffle_range, (int, int *, int *));
E int NDECL(find_skates);
E void NDECL(oinit);
E void FDECL(savenames, (int, int));
E void FDECL(restnames, (int));
E void FDECL(discover_object, (int, BOOLEAN_P, BOOLEAN_P));
E void FDECL(undiscover_object, (int));
E int NDECL(dodiscovered);
E int NDECL(doclassdisco);
E void NDECL(rename_disco);

/* ### objects.c ### */

E void NDECL(objects_init);

E unsigned FDECL(parentObjectIDForSplitStack, ());
E void FDECL(setParentObjectIDForSplitStack, (unsigned));
E unsigned FDECL(childObjectIDForSplitStack, ());
E void FDECL(setChildObjectIDForSplitStack, (unsigned));

/* ### objnam.c ### */

E char *FDECL(obj_typename, (int));
E char *FDECL(simple_typename, (int));
E boolean FDECL(obj_is_pname, (struct obj *));
E char *FDECL(distant_name, (struct obj *, char *(*)(OBJ_P)));
E char *FDECL(fruitname, (BOOLEAN_P));
E char *FDECL(xname, (struct obj *));
E char *FDECL(mshot_xname, (struct obj *));
E boolean FDECL(the_unique_obj, (struct obj *));
E boolean FDECL(isUniqueMonsterType, (int));
E char *FDECL(doname, (struct obj *));
E char *FDECL(doname_with_price, (struct obj *));
E boolean FDECL(not_fully_identified, (struct obj *));
E char *FDECL(corpse_xname, (struct obj *, const char *, unsigned));
E char *FDECL(cxname, (struct obj *));
E char *FDECL(cxname_singular, (struct obj *));
E char *FDECL(killer_xname, (struct obj *));
E char *FDECL(short_oname,
              (struct obj *, char *(*)(OBJ_P), char *(*)(OBJ_P), unsigned));
E const char *FDECL(singular, (struct obj *, char *(*)(OBJ_P)));
E char *FDECL(an, (const char *));
E char *FDECL(An, (const char *));
E char *FDECL(The, (const char *));
E char *FDECL(the, (const char *));
E char *FDECL(aobjnam, (struct obj *, const char *));
E char *FDECL(yobjnam, (struct obj *, const char *));
E char *FDECL(Yobjnam2, (struct obj *, const char *));
E char *FDECL(Tobjnam, (struct obj *, const char *));
E char *FDECL(otense, (struct obj *, const char *));
E char *FDECL(vtense, (const char *, const char *));
E char *FDECL(Doname2, (struct obj *));
E char *FDECL(yname, (struct obj *));
E char *FDECL(Yname2, (struct obj *));
E char *FDECL(ysimple_name, (struct obj *));
E char *FDECL(Ysimple_name2, (struct obj *));
E char *FDECL(simpleonames, (struct obj *));
E char *FDECL(ansimpleoname, (struct obj *));
E char *FDECL(thesimpleoname, (struct obj *));
E char *FDECL(bare_artifactname, (struct obj *));
E char *FDECL(makeplural, (const char *));
E char *FDECL(makesingular, (const char *));
E struct obj *FDECL(readobjnam, (char *, struct obj *));
E int FDECL(rnd_class, (int, int));
E const char *FDECL(suit_simple_name, (struct obj *));
E const char *FDECL(cloak_simple_name, (struct obj *));
E const char *FDECL(helm_simple_name, (struct obj *));
E const char *FDECL(mimic_obj_name, (struct monst *));
E char *FDECL(safe_qbuf, (char *, const char *, const char *, struct obj *,
                          char *(*)(OBJ_P), char *(*)(OBJ_P), const char *));

/* ### options.c ### */

E void NDECL(reglyph_darkroom);
E boolean FDECL(match_optname, (const char *, const char *, int, BOOLEAN_P));
E void NDECL(initoptions);
E void NDECL(initoptions_init);
E void NDECL(initoptions_finish);
E void FDECL(parseoptions, (char *, BOOLEAN_P, BOOLEAN_P));
E int NDECL(doset);
E int NDECL(dotogglepickup);
E void NDECL(option_help);
E void FDECL(next_opt, (winid, const char *));
E int FDECL(fruitadd, (char *, struct fruit *));
E int FDECL(choose_classes_menu,
            (const char *, int, BOOLEAN_P, char *, char *));
E void FDECL(add_menu_cmd_alias, (CHAR_P, CHAR_P));
E char FDECL(map_menu_cmd, (CHAR_P));
E void FDECL(assign_warnings, (uchar *));
E char *FDECL(nh_getenv, (const char *));
E void FDECL(set_duplicate_opt_detection, (int));
E void FDECL(set_wc_option_mod_status, (unsigned long, int));
E void FDECL(set_wc2_option_mod_status, (unsigned long, int));
E void FDECL(set_option_mod_status, (const char *, int));
E int FDECL(add_autopickup_exception, (const char *));
E void NDECL(free_autopickup_exceptions);
E int FDECL(load_symset, (const char *, int));
E void NDECL(free_symsets);
E void FDECL(parsesymbols, (char *));
E struct symparse *FDECL(match_sym, (char *));
E void NDECL(set_playmode);
E int FDECL(sym_val, (char *));
E const char *FDECL(clr2colorname, (int));
E boolean FDECL(add_menu_coloring, (char *));
E boolean FDECL(get_menu_coloring, (char *, int *, int *));
E void NDECL(free_menu_coloring);
E boolean FDECL(msgtype_parse_add, (char *));
E int FDECL(msgtype_type, (const char *));
E void NDECL(msgtype_free);
E javaString FDECL(alignmentString, (int));

/* ### pager.c ### */

E char *FDECL(self_lookat, (char *));
E boolean FDECL(object_from_map, (int,int,int,struct obj **));
E int NDECL(dowhatis);
E int NDECL(doquickwhatis);
E int NDECL(doidtrap);
E int NDECL(dowhatdoes);
E char *FDECL(dowhatdoes_core, (CHAR_P, char *));
E int NDECL(dohelp);
E int NDECL(dohistory);
E int FDECL(do_screen_description,
            (coord, BOOLEAN_P, int, char *, const char **));
E int FDECL(do_look, (int, coord *));

/* ### pcmain.c ### */

#if defined(MICRO) || defined(WIN32)
#ifdef CHDIR
E void FDECL(chdirx, (char *, BOOLEAN_P));
#endif /* CHDIR */
E boolean NDECL(authorize_wizard_mode);
#endif /* MICRO || WIN32 */

/* ### pcsys.c ### */

#if defined(MICRO) || defined(WIN32)
E void NDECL(flushout);
E int NDECL(dosh);
#ifdef MFLOPPY
E void FDECL(eraseall, (const char *, const char *));
E void FDECL(copybones, (int));
E void NDECL(playwoRAMdisk);
E int FDECL(saveDiskPrompt, (int));
E void NDECL(gameDiskPrompt);
#endif
E void FDECL(append_slash, (char *));
E void FDECL(getreturn, (const char *));
#ifndef AMIGA
E void VDECL(msmsg, (const char *, ...));
#endif
E FILE *FDECL(fopenp, (const char *, const char *));
#endif /* MICRO || WIN32 */

/* ### pctty.c ### */

#if defined(MICRO) || defined(WIN32)
E void NDECL(gettty);
E void FDECL(settty, (const char *));
E void NDECL(setftty);
E void VDECL(error, (const char *, ...));
#if defined(TIMED_DELAY) && defined(_MSC_VER)
E void FDECL(msleep, (unsigned));
#endif
#endif /* MICRO || WIN32 */

/* ### pcunix.c ### */

#if defined(MICRO)
E void FDECL(regularize, (char *));
#endif /* MICRO */
#if defined(PC_LOCKING)
E void NDECL(getlock);
#endif

/* ### pickup.c ### */

E int FDECL(collect_obj_classes, (char *, struct obj *, BOOLEAN_P,
                                  boolean FDECL((*), (OBJ_P)), int *));
E boolean FDECL(rider_corpse_revival, (struct obj *, BOOLEAN_P));
E void FDECL(add_valid_menu_class, (int));
E boolean FDECL(allow_all, (struct obj *));
E boolean FDECL(allow_category, (struct obj *));
E boolean FDECL(is_worn_by_type, (struct obj *));
E int FDECL(ck_bag, (struct obj *));
#ifdef USE_TRAMPOLI
E int FDECL(in_container, (struct obj *));
E int FDECL(out_container, (struct obj *));
#endif
E int FDECL(pickup, (int));
E int FDECL(pickup_object, (struct obj *, long, BOOLEAN_P));
E int FDECL(query_category,
            (const char *, struct obj *, int, menu_item **, int));
E int FDECL(query_objlist, (const char *, struct obj *, int, menu_item **,
                            int, boolean (*)(OBJ_P)));
E struct obj *FDECL(pick_obj, (struct obj *));
E int NDECL(encumber_msg);
E int NDECL(doloot);
E boolean FDECL(container_gone, (int (*)(OBJ_P)));
E boolean NDECL(u_handsy);
E int FDECL(use_container, (struct obj **, int));
E int FDECL(loot_mon, (struct monst *, int *, boolean *));
E int NDECL(dotip);
E boolean FDECL(is_autopickup_exception, (struct obj *, BOOLEAN_P));

/* ### pline.c ### */

E void VDECL(pline, (const char *, ...)) PRINTF_F(1, 2);
E void VDECL(Norep, (const char *, ...)) PRINTF_F(1, 2);
E void NDECL(free_youbuf);
E void VDECL(You, (const char *, ...)) PRINTF_F(1, 2);
E void VDECL(Your, (const char *, ...)) PRINTF_F(1, 2);
E void VDECL(You_feel, (const char *, ...)) PRINTF_F(1, 2);
E void VDECL(You_cant, (const char *, ...)) PRINTF_F(1, 2);
E void VDECL(You_hear, (const char *, ...)) PRINTF_F(1, 2);
E void VDECL(You_see, (const char *, ...)) PRINTF_F(1, 2);
E void VDECL(pline_The, (const char *, ...)) PRINTF_F(1, 2);
E void VDECL(There, (const char *, ...)) PRINTF_F(1, 2);
E void VDECL(verbalize, (const char *, ...)) PRINTF_F(1, 2);
E void VDECL(raw_printf, (const char *, ...)) PRINTF_F(1, 2);
E void VDECL(impossible, (const char *, ...)) PRINTF_F(1, 2);
E const char *FDECL(align_str, (ALIGNTYP_P));
E void FDECL(mstatusline, (struct monst *));
E void NDECL(ustatusline);
E void NDECL(self_invis_message);
E void FDECL(pudding_merge_message, (struct obj *, struct obj *));

/* ### polyself.c ### */

E void NDECL(set_uasmon);
E void NDECL(float_vs_flight);
E void NDECL(change_sex);
E void FDECL(polyself, (int));
E int FDECL(polymon, (int));
E void NDECL(rehumanize);
E int NDECL(dobreathe);
E int NDECL(dospit);
E int NDECL(doremove);
E int NDECL(dospinweb);
E int NDECL(dosummon);
E int NDECL(dogaze);
E int NDECL(dohide);
E int NDECL(dopoly);
E int NDECL(domindblast);
E void FDECL(skinback, (BOOLEAN_P));
E const char *FDECL(mbodypart, (struct monst *, int));
E const char *FDECL(body_part, (int));
E int NDECL(poly_gender);
E void FDECL(ugolemeffects, (int, int));

/* ### potion.c ### */

E void FDECL(make_confused, (long, BOOLEAN_P));
E void FDECL(make_stunned, (long, BOOLEAN_P));
E void FDECL(make_blinded, (long, BOOLEAN_P));
E void FDECL(make_sick, (long, const char *, BOOLEAN_P, int));

E int FDECL(sickNonVomitable, ());
E int FDECL(sickVomitable, ());
E int FDECL(sickAll, ());

E void FDECL(make_slimed, (long, const char *));
E void FDECL(make_stoned, (long, const char *, int, const char *));
E void FDECL(make_vomiting, (long, BOOLEAN_P));
E boolean FDECL(make_hallucinated, (long, BOOLEAN_P, long));
E void FDECL(make_deaf, (long, BOOLEAN_P));
E int NDECL(dodrink);
E int FDECL(dopotion, (struct obj *));
E int FDECL(peffects, (struct obj *));
E void FDECL(healup, (int, int, BOOLEAN_P, BOOLEAN_P));
E void FDECL(strange_feeling, (struct obj *, const char *));
E void FDECL(potionhit, (struct monst *, struct obj *, BOOLEAN_P));
E void FDECL(potionbreathe, (struct obj *));
E int NDECL(dodip);
E void FDECL(mongrantswish, (struct monst **));
E void FDECL(djinni_from_bottle, (struct obj *));
E struct monst *FDECL(split_mon, (struct monst *, struct monst *));
E const char *NDECL(bottlename);

/* ### pray.c ### */

E boolean FDECL(critically_low_hp, (BOOLEAN_P));
#ifdef USE_TRAMPOLI
E int NDECL(prayer_done);
#endif
E int NDECL(dosacrifice);
E boolean FDECL(can_pray, (BOOLEAN_P));
E int NDECL(dopray);
E int NDECL(doturn);
E int FDECL(a_align, (int, int));
E javaString NDECL(altarDeityName);
E void FDECL(altar_wrath, (int, int));
E javaString FDECL(randomHallucinatoryDeityName, ());

/* ### priest.c ### */

E int FDECL(move_special, (struct monst *, BOOLEAN_P, SCHAR_P, BOOLEAN_P,
                           BOOLEAN_P, XCHAR_P, XCHAR_P, XCHAR_P, XCHAR_P));
E char FDECL(mostRecentCurrentlyOccupiedTempleID, ());
E boolean FDECL(inhistemple, (struct monst *));
E int FDECL(pri_move, (struct monst *));
E void FDECL(initPriestOnCurrentLevel, (struct mkroom *, int, int, BOOLEAN_P));
E aligntyp FDECL(mon_aligntyp, (struct monst *));
E char *FDECL(priestname, (struct monst *, char *));
E boolean FDECL(p_coaligned, (struct monst *));
E struct monst *FDECL(findpriest, (CHAR_P));
E void FDECL(intemple, (int));
E void FDECL(forget_temple_entry, (struct monst *));
E void FDECL(priest_talk, (struct monst *));
E struct monst *FDECL(makeRoamingMonsterOfType, (int, ALIGNTYP_P, XCHAR_P, XCHAR_P, BOOLEAN_P));
E void FDECL(reset_hostility, (struct monst *));
E boolean FDECL(in_your_sanctuary, (struct monst *, XCHAR_P, XCHAR_P));
E void FDECL(ghod_hitsu, (struct monst *));
E void NDECL(angry_priest);
E void NDECL(clearpriests);
E void FDECL(restpriest, (struct monst *, BOOLEAN_P));
E void FDECL(newepri, (struct monst *));
E void FDECL(free_epri, (struct monst *));

/* ### quest.c ### */

E boolean FDECL (yourRoleHasQuestLeader, ());
E int FDECL(pmidOfQuestLeaderForYourRole, ());

E boolean FDECL (yourRoleHasQuestGuardians, ());
E int FDECL(pmidOfQuestGuardiansForYourRole, ());

E boolean FDECL (yourRoleHasQuestNemesis, ());
E int FDECL(pmidOfQuestNemesisForYourRole, ());

E int FDECL(pmidOfQuestEnemiesForYourRole, ());
E int FDECL(pmidOfOtherQuestEnemiesForYourRole, ());
E int FDECL(classOfQuestEnemiesForYourRole, ());
E int FDECL(classOfOtherQuestEnemiesForYourRole, ());

E boolean FDECL(yourRoleHasQuestArtifact, ());
E int FDECL(idOfQuestArtifactForYourRole, ());

E void NDECL(onquest);
E void NDECL(nemdead);
E void FDECL(artitouch, (struct obj *));
E boolean NDECL(ok_to_quest);
E void FDECL(leader_speaks, (struct monst *));
E void NDECL(nemesis_speaks);
E void FDECL(quest_chat, (struct monst *));
E void FDECL(quest_talk, (struct monst *));
E void FDECL(quest_stat_check, (struct monst *));
E void FDECL(finish_quest, (struct obj *));

/* ### questpgr.c ### */

E void NDECL(load_qtlist);
E void NDECL(unload_qtlist);
E short FDECL(quest_info, (int));
E const char *NDECL(ldrname);
E boolean FDECL(is_quest_artifact, (struct obj *));
E void FDECL(com_pager, (int));
E void FDECL(qt_pager, (int));
E int NDECL(questMonsterType);
E void NDECL(deliver_splev_message);

/* ### random.c ### */

#if defined(RANDOM) && !defined(__GO32__) /* djgpp has its own random */
E void FDECL(srandom, (unsigned));
E char *FDECL(initstate, (unsigned, char *, int));
E char *FDECL(setstate, (char *));
E long NDECL(random);
#endif /* RANDOM */

/* ### read.c ### */

E void FDECL(learnscroll, (struct obj *));
E char *FDECL(tshirt_text, (struct obj *, char *));
E int NDECL(doread);
E boolean FDECL(is_chargeable, (struct obj *));
E void FDECL(recharge, (struct obj *, int));
E void FDECL(forget_objects, (int));
E void FDECL(forget_levels, (int));
E void NDECL(forget_traps);
E void FDECL(forget_map, (int));
E int FDECL(seffects, (struct obj *));
E void FDECL(drop_boulder_on_player,
             (BOOLEAN_P, BOOLEAN_P, BOOLEAN_P, BOOLEAN_P));
E boolean FDECL(drop_boulder_on_monster, (int, int, BOOLEAN_P, BOOLEAN_P));
E void FDECL(wand_explode, (struct obj *, int));
#ifdef USE_TRAMPOLI
E void FDECL(set_lit, (int, int, genericptr_t));
#endif
E void FDECL(litroom, (BOOLEAN_P, struct obj *));
E void FDECL(do_genocide, (int));
E void FDECL(punish, (struct obj *));
E void NDECL(unpunish);
E boolean FDECL(cant_revive, (int *, BOOLEAN_P, struct obj *));
E boolean NDECL(create_particular);

/* ### rect.c ### */

E void NDECL(init_rect);
E NhRect *FDECL(get_rect, (NhRect *));
E NhRect *NDECL(rnd_rect);
E void FDECL(remove_rect, (NhRect *));
E void FDECL(add_rect, (NhRect *));
E void FDECL(split_rects, (NhRect *, NhRect *));

/* ## region.c ### */
E void NDECL(clear_regions);
E void NDECL(run_regions);
E boolean FDECL(in_out_region, (XCHAR_P, XCHAR_P));
E boolean FDECL(m_in_out_region, (struct monst *, XCHAR_P, XCHAR_P));
E void NDECL(update_player_regions);
E void FDECL(update_monster_region, (struct monst *));
E NhRegion *FDECL(visible_region_at, (XCHAR_P, XCHAR_P));
E void FDECL(show_region, (NhRegion *, XCHAR_P, XCHAR_P));
E void FDECL(save_regions, (int, int));
E void FDECL(rest_regions, (int, BOOLEAN_P));
E NhRegion *FDECL(create_gas_cloud, (XCHAR_P, XCHAR_P, int, int));
E boolean NDECL(region_danger);
E void NDECL(region_safety);

/* ### restore.c ### */

E void FDECL(inven_inuse, (BOOLEAN_P));
E int FDECL(dorecover, (int));
E void FDECL(restcemetery, (int, struct cemetery **));
E void FDECL(trickery, (char *));
E void FDECL(getlev, (int, int, XCHAR_P, BOOLEAN_P));
E void FDECL(get_plname_from_file, (int, char *));
#ifdef SELECTSAVED
E int FDECL(restore_menu, (winid));
#endif
E void NDECL(minit);
E boolean FDECL(lookup_id_mapping, (unsigned, unsigned *));
E void FDECL(mread, (int, genericptr_t, unsigned int));
E int FDECL(validate, (int, const char *));
E void NDECL(reset_restpref);
E void FDECL(set_restpref, (const char *));
E void FDECL(set_savepref, (const char *));

/* ### rip.c ### */

E void FDECL(genl_outrip, (winid, int, time_t));

/* ### rnd.c ### */

E int FDECL(rn2, (int));
E int FDECL(rnl, (int));
E int FDECL(rnd, (int));
E int FDECL(d, (int, int));
E int FDECL(rne, (int));
E int FDECL(rnz, (int));

/* ### role.c ### */

E int FDECL(numberOfRoleAlignments, ());
E javaString FDECL(alignmentFileCode, (int));
E int FDECL(alignmentMask, (int));
E int FDECL(alignmentType, (int));

E int FDECL(roleNone, ());
E int FDECL(roleRandom, ());
E int FDECL(numberOfKnownRoles, ());

E boolean FDECL(roleHasDefaultPantheon, (int));
E int FDECL(defaultPantheonForRole, (int));

E boolean FDECL(roleNameHasMaleVersion, (int));
E javaString FDECL(roleNameAsMale, (int));
E boolean FDECL(roleNameHasFemaleVersion, (int));
E javaString FDECL(roleNameAsFemale, (int));

E boolean FDECL(rankNameHasMaleVersion, (int, int));
E javaString FDECL(rankNameAsMale, (int, int));
E boolean FDECL(rankNameHasFemaleVersion, (int, int));
E javaString FDECL(rankNameAsFemale, (int, int));

E int FDECL(yourCurrentPantheon, ());
E void FDECL(setYourCurrentPantheon, (int));

E int FDECL(yourCurrentRoleID, ());
E void FDECL(setYourCurrentRoleID, (int));

E javaString FDECL(fileCodeForRole, (int));
E javaString FDECL(yourRoleFileCode, ());

E javaString FDECL(yourQuestHomeBase, ());
E javaString FDECL(yourIntermediateQuestLevel, ());

E int FDECL(yourRolePMID, ());
E void FDECL(setYourRolePMID, (int));
E boolean FDECL(yourRoleHasPMID, (int));

E boolean FDECL(yourRoleHasFemalePMID, ());
E int FDECL(femalePMIDForYourRole, ());

E boolean FDECL(yourRoleHasMalePMID, ());
E int FDECL(malePMIDForYourRole, ());

E boolean FDECL(yourRoleCanStartAsMaleOrFemale, ());
E boolean FDECL(roleCanStartAsMaleOrFemale, (int));

E long FDECL(startingMaskForRole, (int));
E long FDECL(startingAlignmentMaskForRole, (int));
E long FDECL(startingGenderMaskForRole, (int));
E long FDECL(startingSpeciesMaskForRole, (int));

E boolean FDECL(roleHasFemalePMID, (int));
E int FDECL(femalePMIDForRole, (int));
E boolean FDECL(roleHasMalePMID, (int));
E int FDECL(malePMIDForRole, (int));

E int FDECL(attributeBaseForYourRole, (int));
E int FDECL(attributeDistributionForYourRole, (int));

E int FDECL(cutoffLevelForYourRole, ());
E int FDECL(hitPointAdvancementForYourRole, ());
E int FDECL(initialAlignmentRecordForYourRole, ());
E int FDECL(baseSpellcastingPenaltyForYourRole, ());
E int FDECL(healingSpellPenaltyForYourRole, ());
E int FDECL(spellPenaltyWithShieldForYourRole, ());
E int FDECL(spellPenaltyWithArmorForYourRole, ());
E int FDECL(spellcastingAttributeIDForYourRole, ());
E int FDECL(specialtySpellIDForYourRole, ());
E int FDECL(specialtySpellPenaltyForYourRole, ());

E int FDECL(hitPointAdvancementForYourSpecies, ());

E boolean FDECL(validSpeciesID, (int));
E void FDECL(setYourSpeciesID, (int));
E int FDECL(yourSpeciesID, ());
E int FDECL(numberOfPlayableSpecies, ());
E javaString FDECL(nounForSpecies, (int));
E javaString FDECL(adjectiveForSpecies, (int));
E javaString FDECL(fileCodeForSpecies, (int));

E javaString FDECL(yourSpeciesNoun, ());
E javaString FDECL(yourSpeciesAdjective, ());
E javaString FDECL(yourSpeciesFileCode, ());
E boolean FDECL(yourSpeciesIndividualNameHasMaleVersion, ());
E boolean FDECL(yourSpeciesIndividualNameHasFemaleVersion, ());
E javaString FDECL(yourSpeciesIndividualNameAsMale, ());
E javaString FDECL(yourSpeciesIndividualNameAsFemale, ());
E boolean FDECL(yourSpeciesIs, (int));

E boolean FDECL(yourSpeciesPMIDHasFemaleVersion, ());
E int FDECL(yourSpeciesPMIDAsFemale, ());
E boolean FDECL(yourSpeciesPMIDHasMaleVersion, ());
E int FDECL(yourSpeciesPMIDAsMale, ());
E boolean FDECL(yourSpeciesPMIDHasMummyVersion, ());
E int FDECL(yourSpeciesPMIDAsMummy, ());
E boolean FDECL(yourSpeciesPMIDHasZombieVersion, ());
E int FDECL(yourSpeciesPMIDAsZombie, ());

E long FDECL(startingMaskForSpecies, (int));
E long FDECL(startingAlignmentMaskForSpecies, (int));
E boolean FDECL(speciesCanStartAsChaotic, (int));
E boolean FDECL(speciesCanStartAsNeutral, (int));
E boolean FDECL(speciesCanStartAsLawful, (int));
E long FDECL(startingGenderMaskForSpecies, (int));

E long FDECL(selfMaskForSpecies, (int));
E long FDECL(yourSpeciesSelfMask, ());
E long FDECL(yourSpeciesFriendshipMask, ());
E long FDECL(yourSpeciesHostilityMask, ());
E int FDECL(yourSpeciesAttributeMaximum, (int));
E int FDECL(yourSpeciesAttributeMinimum, (int));

E boolean FDECL(validrole, (int));
E boolean FDECL(validrace, (int, int));
E boolean FDECL(validgend, (int, int, int));
E boolean FDECL(validalign, (int, int, int));
E int NDECL(randrole);
E int FDECL(randrace, (int));
E int FDECL(randgend, (int, int));
E int FDECL(randalign, (int, int));
E int FDECL(str2role, (const char *));
E int FDECL(str2race, (const char *));
E int FDECL(str2gend, (const char *));
E int FDECL(str2align, (const char *));
E boolean FDECL(ok_role, (int, int, int, int));
E int FDECL(pick_role, (int, int, int, int));
E boolean FDECL(ok_race, (int, int, int, int));
E int FDECL(pick_race, (int, int, int, int));
E boolean FDECL(ok_gend, (int, int, int, int));
E int FDECL(pick_gend, (int, int, int, int));
E boolean FDECL(ok_align, (int, int, int, int));
E int FDECL(pick_align, (int, int, int, int));
E void NDECL(rigid_role_checks);
E boolean FDECL(setrolefilter, (const char *));
E boolean NDECL(gotrolefilter);
E void NDECL(clearrolefilter);
E char *FDECL(build_plselection_prompt, (char *, int, int, int, int, int));
E char *FDECL(root_plselection_prompt, (char *, int, int, int, int, int));
E void NDECL(plnamesuffix);
E void FDECL(role_selection_prolog, (int, winid));
E void FDECL(role_menu_extra, (int, winid));
E void NDECL(role_init);
E const char *FDECL(Hello, (struct monst *));
E const char *NDECL(Goodbye);
E int FDECL(roleAlignmentMask, ());
E int FDECL(roleGenderMask, ());
E int FDECL(roleSpeciesMask, ());

E int FDECL(maleMask, ());
E int FDECL(femaleMask, ());
E int FDECL(neuterMask, ());

E int FDECL(lawfulMask, ());
E int FDECL(neutralMask, ());
E int FDECL(chaoticMask, ());

/* ### rumors.c ### */

E char *FDECL(getrumor, (int, char *, BOOLEAN_P));
E char *FDECL(get_rnd_text, (const char *, char *));
E void FDECL(outrumor, (int, int));
E void FDECL(outoracle, (BOOLEAN_P, BOOLEAN_P));
E void FDECL(save_oracles, (int, int));
E void FDECL(restore_oracles, (int));
E int FDECL(doconsult, (struct monst *));
E void NDECL(rumor_check);

/* ### save.c ### */

E int NDECL(dosave);
E int NDECL(dosave0);
E boolean FDECL(tricked_fileremoved, (int, char *));
#ifdef INSURANCE
E void NDECL(savestateinlock);
#endif
#ifdef MFLOPPY
E boolean FDECL(savelev, (int, XCHAR_P, int));
E boolean FDECL(swapin_file, (int));
E void NDECL(co_false);
#else
E void FDECL(savelev, (int, XCHAR_P, int));
#endif
E genericptr_t FDECL(mon_to_buffer, (struct monst *, int *));
E void FDECL(bufon, (int));
E void FDECL(bufoff, (int));
E void FDECL(bflush, (int));
E void FDECL(bwrite, (int, genericptr_t, unsigned int));
E void FDECL(bclose, (int));
E void FDECL(def_bclose, (int));
#if defined(ZEROCOMP)
E void FDECL(zerocomp_bclose, (int));
#endif
E void FDECL(savecemetery, (int, int, struct cemetery **));
E void FDECL(savefruitchn, (int, int));
E void FDECL(store_plname_in_file, (int));
E void NDECL(free_dungeons);
E void NDECL(freedynamicdata);
E void FDECL(store_savefileinfo, (int));

/* ### shk.c ### */

E long FDECL(money2mon, (struct monst *, long));
E void FDECL(money2u, (struct monst *, long));
E void FDECL(shkgone, (struct monst *));
E void FDECL(set_residency, (struct monst *, BOOLEAN_P));
E void FDECL(replshk, (struct monst *, struct monst *));
E void FDECL(restshk, (struct monst *, BOOLEAN_P));
E char FDECL(inside_shop, (XCHAR_P, XCHAR_P));
E void FDECL(youLeftAShop, (char, boolean));
E void FDECL(remote_burglary, (XCHAR_P, XCHAR_P));
E void FDECL(youEnteredAShop, (char));
E void FDECL(pick_pick, (struct obj *));
E boolean FDECL(same_price, (struct obj *, struct obj *));
E void NDECL(shopper_financial_report);
E int FDECL(inhishop, (struct monst *));
E struct monst *FDECL(shop_keeper, (CHAR_P));
E boolean FDECL(tended_shop, (struct mkroom *));
E boolean FDECL(is_unpaid, (struct obj *));
E void FDECL(delete_contents, (struct obj *));
E void FDECL(obfree, (struct obj *, struct obj *));
E void FDECL(home_shk, (struct monst *, BOOLEAN_P));
E void FDECL(make_happy_shk, (struct monst *, BOOLEAN_P));
E void FDECL(make_happy_shoppers, (BOOLEAN_P));
E void FDECL(hot_pursuit, (struct monst *));
E void FDECL(make_angry_shk, (struct monst *, XCHAR_P, XCHAR_P));
E int NDECL(dopay);
E boolean FDECL(paybill, (int));
E void NDECL(finish_paybill);
E struct obj *FDECL(find_oid, (unsigned));
E long FDECL(contained_cost,
             (struct obj *, struct monst *, long, BOOLEAN_P, BOOLEAN_P));
E long FDECL(contained_gold, (struct obj *));
E void FDECL(picked_container, (struct obj *));
E void FDECL(alter_cost, (struct obj *, long));
E long FDECL(unpaid_cost, (struct obj *, BOOLEAN_P));
E boolean FDECL(billable, (struct monst **, struct obj *, CHAR_P, BOOLEAN_P));
E void FDECL(addtobill, (struct obj *, BOOLEAN_P, BOOLEAN_P, BOOLEAN_P));
E void FDECL(splitbill, (struct obj *, struct obj *));
E void FDECL(subfrombill, (struct obj *, struct monst *));
E long FDECL(stolen_value,
             (struct obj *, XCHAR_P, XCHAR_P, BOOLEAN_P, BOOLEAN_P));
E void FDECL(sellobj_state, (int));
E void FDECL(sellobj, (struct obj *, XCHAR_P, XCHAR_P));
E int FDECL(doinvbill, (int));
E struct monst *FDECL(shkcatch, (struct obj *, XCHAR_P, XCHAR_P));
E void FDECL(add_damage, (XCHAR_P, XCHAR_P, long));
E int FDECL(repair_damage, (struct monst *, struct damage *, BOOLEAN_P));
E int FDECL(shk_move, (struct monst *));
E void FDECL(after_shk_move, (struct monst *));
E boolean FDECL(is_fshk, (struct monst *));
E void FDECL(shopdig, (int));
E void FDECL(pay_for_damage, (const char *, BOOLEAN_P));
E boolean FDECL(costly_spot, (XCHAR_P, XCHAR_P));
E struct obj *FDECL(shop_object, (XCHAR_P, XCHAR_P));
E void FDECL(price_quote, (struct obj *));
E void FDECL(shk_chat, (struct monst *));
E void FDECL(check_unpaid_usage, (struct obj *, BOOLEAN_P));
E void FDECL(check_unpaid, (struct obj *));
E void FDECL(costly_gold, (XCHAR_P, XCHAR_P, long));
E long FDECL(get_cost_of_shop_item, (struct obj *));
E boolean FDECL(block_door, (XCHAR_P, XCHAR_P));
E boolean FDECL(block_entry, (XCHAR_P, XCHAR_P));
E char *FDECL(shk_your, (char *, struct obj *));
E char *FDECL(Shk_Your, (char *, struct obj *));

/* ### shknam.c ### */

E void FDECL(neweshk, (struct monst *));
E void FDECL(free_eshk, (struct monst *));
E void FDECL(stock_room, (int, struct mkroom *));
E boolean FDECL(saleable, (struct monst *, struct obj *));
E int FDECL(get_shop_item, (int));
E const char *FDECL(shkname, (struct monst *));
E boolean FDECL(shkname_is_pname, (struct monst *));
E boolean FDECL(is_izchak, (struct monst *, BOOLEAN_P));

/* ### sit.c ### */

E void NDECL(take_gold);
E int NDECL(dosit);
E void NDECL(rndcurse);
E void NDECL(attrcurse);

/* ### sounds.c ### */

E void NDECL(dosounds);
E const char *FDECL(growl_sound, (struct monst *));
E void FDECL(growl, (struct monst *));
E void FDECL(yelp, (struct monst *));
E void FDECL(whimper, (struct monst *));
E void FDECL(beg, (struct monst *));
E int NDECL(dotalk);
#ifdef USER_SOUNDS
E int FDECL(add_sound_mapping, (const char *));
E void FDECL(play_sound_for_message, (const char *));
#endif

/* ### sys.c ### */

E void NDECL(sys_early_init);
E void NDECL(sysopt_release);
E void FDECL(enableSeductionAttacks, (boolean));

/* ### sys/msdos/sound.c ### */

#ifdef MSDOS
E int FDECL(assign_soundcard, (char *));
#endif

/* ### sp_lev.c ### */

E boolean FDECL(check_room, (xchar *, xchar *, xchar *, xchar *, BOOLEAN_P));
E boolean FDECL(create_room, (XCHAR_P, XCHAR_P, XCHAR_P, XCHAR_P, XCHAR_P,
                              XCHAR_P, XCHAR_P, XCHAR_P));
E void FDECL(create_secret_door, (struct mkroom *, XCHAR_P));
E boolean
FDECL(dig_corridor, (coord *, coord *, BOOLEAN_P, SCHAR_P, SCHAR_P));
E void FDECL(fill_room, (struct mkroom *, BOOLEAN_P));
E boolean FDECL(load_special, (const char *));

/* ### spell.c ### */

#ifdef USE_TRAMPOLI
E int NDECL(learn);
#endif
E int FDECL(study_book, (struct obj *));
E void FDECL(book_disappears, (struct obj *));
E void FDECL(book_substitution, (struct obj *, struct obj *));
E void NDECL(age_spells);
E int NDECL(docast);
E int FDECL(spell_skilltype, (int));
E int FDECL(spelleffects, (int, BOOLEAN_P));
E void NDECL(losespells);
E int NDECL(dovspell);
E void FDECL(initialspell, (struct obj *));

/* ### steal.c ### */

#ifdef USE_TRAMPOLI
E int NDECL(stealarm);
#endif
E long FDECL(somegold, (long));
E void FDECL(stealgold, (struct monst *));
E void FDECL(remove_worn_item, (struct obj *, BOOLEAN_P));
E int FDECL(steal, (struct monst *, char *));
E int FDECL(mpickobj, (struct monst *, struct obj *));
E void FDECL(stealamulet, (struct monst *));
E void FDECL(maybe_absorb_item, (struct monst *, struct obj *, int, int));
E void FDECL(mdrop_obj, (struct monst *, struct obj *, BOOLEAN_P));
E void FDECL(mdrop_special_objs, (struct monst *));
E void FDECL(relobj, (struct monst *, int, BOOLEAN_P));
E struct obj *FDECL(findgold, (struct obj *));

/* ### steed.c ### */

E void NDECL(rider_cant_reach);
E boolean FDECL(can_saddle, (struct monst *));
E int FDECL(use_saddle, (struct obj *));
E boolean FDECL(can_ride, (struct monst *));
E int NDECL(doride);
E boolean FDECL(mount_steed, (struct monst *, BOOLEAN_P));
E void NDECL(exercise_steed);
E void NDECL(kick_steed);
E void FDECL(dismount_steed, (int));
E void FDECL(place_monster, (struct monst *, int, int));
E boolean FDECL(stucksteed, (BOOLEAN_P));

/* ### teleport.c ### */

E boolean FDECL(goodPositionForMonsterOfType, (int, int, int, unsigned));
E boolean FDECL(goodpos, (int, int, struct monst *, unsigned));
E boolean FDECL(canPlaceMonsterNear, (coord *, XCHAR_P, XCHAR_P, int, unsigned));
E void FDECL(teleds, (int, int, BOOLEAN_P));
E boolean FDECL(safe_teleds, (BOOLEAN_P));
E boolean FDECL(teleport_pet, (struct monst *, BOOLEAN_P));
E void NDECL(tele);
E boolean FDECL(scrolltele, (struct obj *));
E int NDECL(dotele);
E void NDECL(level_tele);
E void FDECL(domagicportal, (struct trap *));
E void FDECL(tele_trap, (struct trap *));
E void FDECL(level_tele_trap, (struct trap *));
E void FDECL(rloc_to, (struct monst *, int, int));
E boolean FDECL(rloc, (struct monst *, BOOLEAN_P));
E boolean FDECL(tele_restrict, (struct monst *));
E void FDECL(mtele_trap, (struct monst *, struct trap *, int));
E int FDECL(mlevel_tele_trap,
            (struct monst *, struct trap *, BOOLEAN_P, int));
E boolean FDECL(rloco, (struct obj *));
E int NDECL(random_teleport_level);
E boolean FDECL(u_teleport_mon, (struct monst *, BOOLEAN_P));

/* ### tile.c ### */
#ifdef USE_TILES
E void FDECL(substitute_tiles, (d_level *));
#endif

/* ### timeout.c ### */

E void NDECL(burn_away_slime);
E void NDECL(nh_timeout);
E void FDECL(fall_asleep, (int, BOOLEAN_P));
E void FDECL(attach_egg_hatch_timeout, (struct obj *, long));
E void FDECL(attach_fig_transform_timeout, (struct obj *));
E void FDECL(kill_egg, (struct obj *));
E void FDECL(hatch_egg, (ANY_P *, long));
E void FDECL(learn_egg_type, (int));
E void FDECL(burn_object, (ANY_P *, long));
E void FDECL(begin_burn, (struct obj *, BOOLEAN_P));
E void FDECL(end_burn, (struct obj *, BOOLEAN_P));
E void NDECL(do_storms);
E boolean FDECL(start_timer, (long, SHORT_P, SHORT_P, ANY_P *));
E long FDECL(stop_timer, (SHORT_P, ANY_P *));
E long FDECL(peek_timer, (SHORT_P, ANY_P *));
E void NDECL(run_timers);
E void FDECL(obj_move_timers, (struct obj *, struct obj *));
E void FDECL(obj_split_timers, (struct obj *, struct obj *));
E void FDECL(obj_stop_timers, (struct obj *));
E boolean FDECL(obj_has_timer, (struct obj *, SHORT_P));
E void FDECL(spot_stop_timers, (XCHAR_P, XCHAR_P, SHORT_P));
E long FDECL(spot_time_expires, (XCHAR_P, XCHAR_P, SHORT_P));
E long FDECL(spot_time_left, (XCHAR_P, XCHAR_P, SHORT_P));
E boolean FDECL(obj_is_local, (struct obj *));
E void FDECL(save_timers, (int, int, int));
E void FDECL(restore_timers, (int, int, BOOLEAN_P, long));
E void FDECL(relink_timers, (BOOLEAN_P));
E int NDECL(wiz_timeout_queue);
E void NDECL(timer_sanity_check);

E long FDECL(yourIntrinsicTimeout, (int));
E void FDECL(setYourIntrinsicTimeout, (int, long));
E void FDECL(incrementYourIntrinsicTimeout, (int, long));

/* ### topten.c ### */

E void FDECL(formatkiller, (char *, unsigned, int));
E void FDECL(topten, (int, time_t));
E void FDECL(prscore, (int, char **));
E struct obj *FDECL(tt_oname, (struct obj *));

/* ### track.c ### */

E void NDECL(initrack);
E void NDECL(settrack);
E coord *FDECL(gettrack, (int, int));

/* ### trap.c ### */

E boolean FDECL(burnarmor, (struct monst *));
E int FDECL(erode_obj, (struct obj *, const char *, int, int));
E boolean FDECL(grease_protect, (struct obj *, const char *, struct monst *));
E struct trap *FDECL(maketrap, (int, int, int));
E void FDECL(fall_through, (BOOLEAN_P));
E struct monst *FDECL(animate_statue,
                      (struct obj *, XCHAR_P, XCHAR_P, int, int *));
E struct monst *FDECL(activate_statue_trap,
                      (struct trap *, XCHAR_P, XCHAR_P, BOOLEAN_P));
E void FDECL(dotrap, (struct trap *, unsigned));
E void FDECL(seetrap, (struct trap *));
E void FDECL(feeltrap, (struct trap *));
E int FDECL(mintrap, (struct monst *));
E void FDECL(instapetrify, (const char *));
E void FDECL(minstapetrify, (struct monst *, BOOLEAN_P));
E void FDECL(selftouch, (const char *));
E void FDECL(mselftouch, (struct monst *, const char *, BOOLEAN_P));
E void NDECL(float_up);
E void FDECL(fill_pit, (int, int));
E int FDECL(float_down, (long, long));
E void NDECL(climb_pit);
E boolean FDECL(fire_damage, (struct obj *, BOOLEAN_P, XCHAR_P, XCHAR_P));
E int FDECL(fire_damage_chain,
            (struct obj *, BOOLEAN_P, BOOLEAN_P, XCHAR_P, XCHAR_P));
E void acid_damage(struct obj *);
E int FDECL(water_damage, (struct obj *, const char *, BOOLEAN_P));
E void FDECL(water_damage_chain, (struct obj *, BOOLEAN_P));
E boolean NDECL(drown);
E void FDECL(drain_en, (int));
E int NDECL(dountrap);
E void FDECL(cnv_trap_obj, (int, int, struct trap *, BOOLEAN_P));
E int FDECL(untrap, (BOOLEAN_P));
E boolean FDECL(openholdingtrap, (struct monst *, boolean *));
E boolean FDECL(closeholdingtrap, (struct monst *, boolean *));
E boolean FDECL(openfallingtrap, (struct monst *, BOOLEAN_P, boolean *));
E boolean FDECL(chest_trap, (struct obj *, int, BOOLEAN_P));
E void FDECL(deltrap, (struct trap *));
E boolean FDECL(delfloortrap, (struct trap *));
E struct trap *FDECL(t_at, (int, int));
E void FDECL(b_trapped, (const char *, int));
E boolean NDECL(unconscious);
E void FDECL(blow_up_landmine, (struct trap *));
E int FDECL(launch_obj, (SHORT_P, int, int, int, int, int));
E boolean NDECL(launch_in_progress);
E void NDECL(force_launch_placement);
E boolean FDECL(uteetering_at_seen_pit, (struct trap *));
E boolean NDECL(lava_effects);
E void NDECL(sink_into_lava);
E void NDECL(sokoban_guilt);

/* ### u_init.c ### */

E void NDECL(u_init);

/* ### uhitm.c ### */

E void FDECL(erode_armor, (struct monst *, int));
E boolean FDECL(attack_checks, (struct monst *, struct obj *));
E void FDECL(check_caitiff, (struct monst *));
E int FDECL(find_roll_to_hit,
            (struct monst *, UCHAR_P, struct obj *, int *, int *));
E boolean FDECL(attack, (struct monst *));
E boolean FDECL(hmon, (struct monst *, struct obj *, int));
E int FDECL(damageum, (struct monst *, const struct Attack));
E void FDECL(missum, (struct monst *, const struct Attack, BOOLEAN_P));
E int FDECL(passive, (struct monst *, BOOLEAN_P, int, UCHAR_P, BOOLEAN_P));
E void FDECL(monsterPassivelyAttacksObjectWith, (struct monst *, struct obj *, int));
E void FDECL(monsterPassivelyAttacksObject, (struct monst *, struct obj *));
E void FDECL(stumble_onto_mimic, (struct monst *));
E int FDECL(flash_hits_mon, (struct monst *, struct obj *));
E void FDECL(light_hits_gremlin, (struct monst *, int));

/* ### unixmain.c ### */

#ifdef UNIX
#ifdef PORT_HELP
E void NDECL(port_help);
#endif
E void FDECL(sethanguphandler, (void (*)(int)));
E boolean NDECL(authorize_wizard_mode);
E boolean FDECL(check_user_string, (char *));
#endif /* UNIX */

/* ### unixtty.c ### */

#if defined(UNIX) || defined(__BEOS__)
E void NDECL(gettty);
E void FDECL(settty, (const char *));
E void NDECL(setftty);
E void NDECL(intron);
E void NDECL(introff);
E void VDECL(error, (const char *, ...)) PRINTF_F(1, 2);
#endif /* UNIX || __BEOS__ */

/* ### unixunix.c ### */

#ifdef UNIX
E void NDECL(getlock);
E void FDECL(regularize, (char *));
#if defined(TIMED_DELAY) && !defined(msleep) && defined(SYSV)
E void FDECL(msleep, (unsigned));
#endif
#ifdef SHELL
E int NDECL(dosh);
#endif /* SHELL */
#if defined(SHELL) || defined(DEF_PAGER) || defined(DEF_MAILREADER)
E int FDECL(child, (int));
#endif
#ifdef PANICTRACE
E boolean FDECL(file_exists, (const char *));
#endif
#endif /* UNIX */

/* ### unixres.c ### */

#ifdef UNIX
#ifdef GNOME_GRAPHICS
E int FDECL(hide_privileges, (BOOLEAN_P));
#endif
#endif /* UNIX */

/* ### vault.c ### */

E void FDECL(newegd, (struct monst *));
E void FDECL(free_egd, (struct monst *));
E boolean FDECL(grddead, (struct monst *));
E char FDECL(mostRecentCurrentlyOccupiedVaultID, ());
E char FDECL(vault_currently_occupied, ());
E void NDECL(invault);
E int FDECL(gd_move, (struct monst *));
E void NDECL(paygd);
E long NDECL(hidden_gold);
E boolean NDECL(gd_sound);
E void FDECL(vault_gd_watching, (unsigned int));

/* ### version.c ### */

E char *FDECL(version_string, (char *));
E char *FDECL(getversionstring, (char *));
E int NDECL(doversion);
E int NDECL(doextversion);
#ifdef MICRO
E boolean FDECL(comp_times, (long));
#endif
E boolean
FDECL(check_version, (struct version_info *, const char *, BOOLEAN_P));
E boolean FDECL(uptodate, (int, const char *));
E void FDECL(store_version, (int));
E unsigned long FDECL(get_feature_notice_ver, (char *));
E unsigned long NDECL(get_current_feature_ver);
E const char *FDECL(copyright_banner_line, (int));

#ifdef RUNTIME_PORT_ID
E void FDECL(append_port_id, (char *));
#endif

/* ### video.c ### */

#ifdef MSDOS
E int FDECL(assign_video, (char *));
#ifdef NO_TERMS
E void NDECL(gr_init);
E void NDECL(gr_finish);
#endif
E void FDECL(tileview, (BOOLEAN_P));
#endif
#ifdef VIDEOSHADES
E int FDECL(assign_videoshades, (char *));
E int FDECL(assign_videocolors, (char *));
#endif

/* ### vis_tab.c ### */

#ifdef VISION_TABLES
E void NDECL(vis_tab_init);
#endif

/* ### vision.c ### */

E void NDECL(vision_init);
E int FDECL(does_block, (int, int, struct rm *));
E void NDECL(vision_reset);
E void FDECL(vision_recalc, (int));
E void FDECL(block_point, (int, int));
E void FDECL(unblock_point, (int, int));
E boolean FDECL(clear_path, (int, int, int, int));
E void FDECL(do_clear_area, (int, int, int,
                             void (*)(int, int, genericptr), genericptr_t));
E unsigned FDECL(howmonseen, (struct monst *));

#ifdef VMS

/* ### vmsfiles.c ### */

E int FDECL(vms_link, (const char *, const char *));
E int FDECL(vms_unlink, (const char *));
E int FDECL(vms_creat, (const char *, unsigned int));
E int FDECL(vms_open, (const char *, int, unsigned int));
E boolean FDECL(same_dir, (const char *, const char *));
E int FDECL(c__translate, (int));
E char *FDECL(vms_basename, (const char *));

/* ### vmsmail.c ### */

E unsigned long NDECL(init_broadcast_trapping);
E unsigned long NDECL(enable_broadcast_trapping);
E unsigned long NDECL(disable_broadcast_trapping);
#if 0
E struct mail_info *NDECL(parse_next_broadcast);
#endif /*0*/

/* ### vmsmain.c ### */

E int FDECL(main, (int, char **));
#ifdef CHDIR
E void FDECL(chdirx, (const char *, BOOLEAN_P));
#endif /* CHDIR */
E void FDECL(sethanguphandler, (void (*)(int)));
E boolean NDECL(authorize_wizard_mode);

/* ### vmsmisc.c ### */

E void NDECL(vms_abort) NORETURN;
E void FDECL(vms_exit, (int)) NORETURN;
#ifdef PANICTRACE
E void FDECL(vms_traceback, (int));
#endif

/* ### vmstty.c ### */

E int NDECL(vms_getchar);
E void NDECL(gettty);
E void FDECL(settty, (const char *));
E void FDECL(shuttty, (const char *));
E void NDECL(setftty);
E void NDECL(intron);
E void NDECL(introff);
E void VDECL(error, (const char *, ...)) PRINTF_F(1, 2);
#ifdef TIMED_DELAY
E void FDECL(msleep, (unsigned));
#endif

/* ### vmsunix.c ### */

E void NDECL(getlock);
E void FDECL(regularize, (char *));
E int NDECL(vms_getuid);
E boolean FDECL(file_is_stmlf, (int));
E int FDECL(vms_define, (const char *, const char *, int));
E int FDECL(vms_putenv, (const char *));
E char *NDECL(verify_termcap);
#if defined(CHDIR) || defined(SHELL) || defined(SECURE)
E void NDECL(privoff);
E void NDECL(privon);
#endif
#ifdef SHELL
E int NDECL(dosh);
#endif
#if defined(SHELL) || defined(MAIL)
E int FDECL(vms_doshell, (const char *, BOOLEAN_P));
#endif
#ifdef SUSPEND
E int NDECL(dosuspend);
#endif
#ifdef SELECTSAVED
E int FDECL(vms_get_saved_games, (const char *, char ***));
#endif

#endif /* VMS */

/* ### weapon.c ### */

E const char *FDECL(weapon_descr, (struct obj *));
E int FDECL(hitval, (struct obj *, struct monst *));
E int FDECL(dmgval, (struct obj *, struct monst *));
E struct obj *FDECL(select_rwep, (struct monst *));
E struct obj *FDECL(select_hwep, (struct monst *));
E void FDECL(possibly_unwield, (struct monst *, BOOLEAN_P));
E void FDECL(mwepgone, (struct monst *));
E int FDECL(mon_wield_item, (struct monst *));
E int NDECL(abon);
E int NDECL(dbon);
E void FDECL(wet_a_towel, (struct obj *, int, BOOLEAN_P));
E void FDECL(dry_a_towel, (struct obj *, int, BOOLEAN_P));
E int NDECL(enhance_weapon_skill);
E void FDECL(unrestrict_weapon_skill, (int));
E void FDECL(use_skill, (int, int));
E void FDECL(add_weapon_skill, (int));
E void FDECL(lose_weapon_skill, (int));
E int FDECL(weapon_type, (struct obj *));
E int NDECL(uwep_skill_type);
E int FDECL(weapon_hit_bonus, (struct obj *));
E int FDECL(weapon_dam_bonus, (struct obj *));
E void FDECL(skill_init, (const struct def_skill *));

/* ### were.c ### */

E void FDECL(were_change, (struct monst *));
E int FDECL(counter_were, (int));
E int FDECL(were_beastie, (int));
E void FDECL(new_were, (struct monst *));
E int FDECL(wereCreatureSummonsOtherMonsters, (int, BOOLEAN_P, int *, char *));
E void NDECL(you_were);
E void FDECL(you_unwere, (BOOLEAN_P));

/* ### wield.c ### */

E void FDECL(setuwep, (struct obj *));
E void FDECL(setuqwep, (struct obj *));
E void FDECL(setuswapwep, (struct obj *));
E int NDECL(dowield);
E int NDECL(doswapweapon);
E int NDECL(dowieldquiver);
E boolean FDECL(wield_tool, (struct obj *, const char *));
E int NDECL(can_twoweapon);
E void NDECL(drop_uswapwep);
E int NDECL(dotwoweapon);
E void NDECL(uwepgone);
E void NDECL(uswapwepgone);
E void NDECL(uqwepgone);
E void NDECL(untwoweapon);
E int FDECL(chwepon, (struct obj *, int));
E int FDECL(welded, (struct obj *));
E void FDECL(weldmsg, (struct obj *));
E void FDECL(setmnotwielded, (struct monst *, struct obj *));
E boolean FDECL(mwelded, (struct obj *));

/* ### windows.c ### */

E void FDECL(choose_windows, (const char *));
#ifdef WINCHAIN
void FDECL(addto_windowchain, (const char *s));
void NDECL(commit_windowchain);
#endif
E boolean NDECL(genl_can_suspend_no);
E boolean NDECL(genl_can_suspend_yes);
E char FDECL(genl_message_menu, (CHAR_P, int, const char *));
E void FDECL(genl_preference_update, (const char *));
E char *FDECL(genl_getmsghistory, (BOOLEAN_P));
E void FDECL(genl_putmsghistory, (const char *, BOOLEAN_P));
#ifdef HANGUPHANDLING
E void NDECL(nhwindows_hangup);
#endif
#ifdef STATUS_VIA_WINDOWPORT
E void NDECL(genl_status_init);
E void NDECL(genl_status_finish);
E void FDECL(genl_status_enablefield,
             (int, const char *, const char *, BOOLEAN_P));
E void FDECL(genl_status_update, (int, genericptr_t, int, int));
#ifdef STATUS_HILITES
E void FDECL(genl_status_threshold, (int, int, anything, int, int, int));
#endif
#endif

/* ### wizard.c ### */

E void NDECL(amulet);
E int FDECL(mon_has_amulet, (struct monst *));
E int FDECL(mon_has_special, (struct monst *));
E int FDECL(tactics, (struct monst *));
E void NDECL(aggravate);
E void NDECL(clonewiz);
E int NDECL(pick_nasty);
E int FDECL(nasty, (struct monst *));
E void NDECL(resurrect);
E void NDECL(intervene);
E void NDECL(wizdead);
E void FDECL(cuss, (struct monst *));

/* ### worm.c ### */

E int NDECL(get_wormno);
E void FDECL(initworm, (struct monst *, int));
E void FDECL(worm_move, (struct monst *));
E void FDECL(worm_nomove, (struct monst *));
E void FDECL(wormgone, (struct monst *));
E void FDECL(wormhitu, (struct monst *));
E void FDECL(cutworm, (struct monst *, XCHAR_P, XCHAR_P, struct obj *));
E void FDECL(see_wsegs, (struct monst *));
E void FDECL(detect_wsegs, (struct monst *, BOOLEAN_P));
E void FDECL(save_worm, (int, int));
E void FDECL(rest_worm, (int));
E void FDECL(place_wsegs, (struct monst *));
E void FDECL(remove_worm, (struct monst *));
E void FDECL(place_worm_tail_randomly, (struct monst *, XCHAR_P, XCHAR_P));
E int FDECL(count_wsegs, (struct monst *));
E boolean FDECL(worm_known, (struct monst *));
E boolean FDECL(worm_cross, (int, int, int, int));

/* ### worn.c ### */

E void FDECL(setworn, (struct obj *, long));
E void FDECL(setnotworn, (struct obj *));
E long FDECL(wearslot, (struct obj *));
E void FDECL(mon_set_minvis, (struct monst *));
E void FDECL(mon_adjust_speed, (struct monst *, int, struct obj *));
E void FDECL(update_mon_intrinsics,
             (struct monst *, struct obj *, BOOLEAN_P, BOOLEAN_P));
E int FDECL(find_mac, (struct monst *));
E void FDECL(m_dowear, (struct monst *, BOOLEAN_P));
E struct obj *FDECL(which_armor, (struct monst *, long));
E void FDECL(mon_break_armor, (struct monst *, BOOLEAN_P));
E void FDECL(bypass_obj, (struct obj *));
E void NDECL(clear_bypasses);
E void FDECL(bypass_objlist, (struct obj *, BOOLEAN_P));
E struct obj *FDECL(nxt_unbypassed_obj, (struct obj *));
E int FDECL(racial_exception, (struct monst *, struct obj *));

/* ### write.c ### */

E int FDECL(dowrite, (struct obj *));

/* ### zap.c ### */

E void FDECL(learnwand, (struct obj *));
E int FDECL(bhitm, (struct monst *, struct obj *));
E void FDECL(probe_monster, (struct monst *));
E boolean FDECL(get_obj_location, (struct obj *, xchar *, xchar *, int));
E boolean FDECL(get_mon_location, (struct monst *, xchar *, xchar *, int));
E struct monst *FDECL(get_container_location,
                      (struct obj * obj, int *, int *));
E struct monst *FDECL(montraits, (struct obj *, coord *));
E struct monst *FDECL(revive, (struct obj *, BOOLEAN_P));
E int FDECL(unturn_dead, (struct monst *));
E void FDECL(cancel_item, (struct obj *));
E boolean FDECL(drain_item, (struct obj *));
E struct obj *FDECL(poly_obj, (struct obj *, int));
E boolean FDECL(obj_resists, (struct obj *, int, int));
E boolean FDECL(obj_shudders, (struct obj *));
E void FDECL(do_osshock, (struct obj *));
E int FDECL(bhito, (struct obj *, struct obj *));
E int FDECL(bhitpile,
            (struct obj *, int (*)(OBJ_P, OBJ_P), int, int, SCHAR_P));
E int FDECL(zappable, (struct obj *));
E void FDECL(zapnodir, (struct obj *));
E int NDECL(dozap);
E int FDECL(zapyourself, (struct obj *, BOOLEAN_P));
E void FDECL(ubreatheu, (const struct Attack));
E int FDECL(lightdamage, (struct obj *, BOOLEAN_P, int));
E boolean FDECL(flashburn, (long));
E boolean FDECL(cancel_monst, (struct monst *, struct obj *, BOOLEAN_P,
                               BOOLEAN_P, BOOLEAN_P));
E void NDECL(zapsetup);
E void NDECL(zapwrapup);
E void FDECL(weffects, (struct obj *));
E int FDECL(spell_damage_bonus, (int));
E const char *FDECL(exclam, (int force));
E void FDECL(hit, (const char *, struct monst *, const char *));
E void FDECL(miss, (const char *, struct monst *));
E struct monst *FDECL(bhit, (int, int, int, int, int (*)(MONST_P, OBJ_P),
                             int (*)(OBJ_P, OBJ_P), struct obj **));
E struct monst *FDECL(boomhit, (struct obj *, int, int));
E int FDECL(zhitm, (struct monst *, int, int, struct obj **));
E int FDECL(burn_floor_objects, (int, int, BOOLEAN_P, BOOLEAN_P));
E void FDECL(buzz, (int, int, XCHAR_P, XCHAR_P, int, int));
E void FDECL(melt_ice, (XCHAR_P, XCHAR_P, const char *));
E void FDECL(start_melt_ice_timeout, (XCHAR_P, XCHAR_P, long));
E void FDECL(melt_ice_away, (ANY_P *, long));
E int FDECL(zap_over_floor, (XCHAR_P, XCHAR_P, int, boolean *, SHORT_P));
E void FDECL(fracture_rock, (struct obj *));
E boolean FDECL(break_statue, (struct obj *));
E void FDECL(destroy_item, (int, int));
E int FDECL(destroy_mitem, (struct monst *, int, int));
E int FDECL(resist, (struct monst *, CHAR_P, int, int));
E void NDECL(makewish);

E int NDECL(ACHIEVEMENT_GOT_AMULET);
E int NDECL(ACHIEVEMENT_GOT_BELL);
E int NDECL(ACHIEVEMENT_GOT_BOOK);
E int NDECL(ACHIEVEMENT_GOT_CANDELABRUM);
E int NDECL(ACHIEVEMENT_ENTERED_GEHENNOM);
E int NDECL(ACHIEVEMENT_ASCENDED);
E int NDECL(ACHIEVEMENT_FOUND_LUCKSTONE_IN_MINES);
E int NDECL(ACHIEVEMENT_FINISHED_SOKOBAN_LEVEL);
E int NDECL(ACHIEVEMENT_KILLED_MEDUSA);

E int NDECL(SPECIAL_ITEM_AMULET);
E int NDECL(SPECIAL_ITEM_BELL);
E int NDECL(SPECIAL_ITEM_BOOK);
E int NDECL(SPECIAL_ITEM_CANDELABRUM);
E int NDECL(SPECIAL_ITEM_QUEST_ARTIFACT);

E long FDECL(elapsedPlayingTimeSeconds, ());
E void FDECL(setElapsedPlayingTimeSeconds, (long));

E long FDECL(epochSeconds, ());
E long FDECL(epochSecondsOfMostRecentRestore, ());
E void FDECL(setEpochSecondsOfMostRecentRestore, (long));

E long FDECL(epochSecondsOfGameEnd, ());
E void FDECL(setEpochSecondsOfGameEnd, (long));

/*hacklib.c*/

#endif /* !MAKEDEFS_C && !LEV_LEX_C */

#undef E

extern jclass getJavaClass(const char*);
extern jmethodID getStaticMethod(jclass, const char*, const char*);

#endif /* EXTERN_H */
