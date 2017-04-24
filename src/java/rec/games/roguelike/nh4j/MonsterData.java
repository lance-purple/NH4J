package rec.games.roguelike.nh4j;

public class MonsterData {

	public static boolean isVerySmallMonster(int pmid) {
		return MonsterType.monsterSize(pmid) < MZ.SMALL.id();
	}

	public static boolean isBigMonster(int pmid) {
		return MonsterType.monsterSize(pmid) >= MZ.LARGE.id();
	}

	public static boolean monsterTypeResistsFire(int pmid) {
		return MonsterType.getMonsterType(pmid).hasResistance(MR.FIRE);
	}
	public static boolean monsterTypeResistsStoning(int pmid) {
		return MonsterType.getMonsterType(pmid).hasResistance(MR.STONE);
	}

    //
    //#define resists_fire(mon) (((mon)->mintrinsics & MR_FIRE) != 0)
    //#define resists_cold(mon) (((mon)->mintrinsics & MR_COLD) != 0)
    //#define resists_sleep(mon) (((mon)->mintrinsics & MR_SLEEP) != 0)
    //#define resists_disint(mon) (((mon)->mintrinsics & MR_DISINT) != 0)
    //#define resists_elec(mon) (((mon)->mintrinsics & MR_ELEC) != 0)
    //#define resists_poison(mon) (((mon)->mintrinsics & MR_POISON) != 0)
    //#define resists_acid(mon) (((mon)->mintrinsics & MR_ACID) != 0)
    //#define resists_ston(mon) (((mon)->mintrinsics & MR_STONE) != 0)
    //
    //#define is_lminion(mon) \
    //    (is_minion((mon)->data) && mon_aligntyp(mon) == A_LAWFUL)
    //#define is_flyer(ptr) (((ptr)->mflags1 & M1_FLY) != 0L)

    public static boolean isFloater(int pmid) {
	    MC mc = MonsterType.getMonsterType(pmid).monsterClass();
	    return MC.EYE.equals(mc) || MC.LIGHT.equals(mc);
    }

    //#define is_clinger(ptr) (((ptr)->mflags1 & M1_CLING) != 0L)
    //#define is_swimmer(ptr) (((ptr)->mflags1 & M1_SWIM) != 0L)
    //#define breathless(ptr) (((ptr)->mflags1 & M1_BREATHLESS) != 0L)
    //#define amphibious(ptr) \
    //    (((ptr)->mflags1 & (M1_AMPHIBIOUS | M1_BREATHLESS)) != 0L)
    //#define passes_walls(ptr) (((ptr)->mflags1 & M1_WALLWALK) != 0L)
    //#define amorphous(ptr) (((ptr)->mflags1 & M1_AMORPHOUS) != 0L)
    
    public static boolean isNoncorporeal(int pmid) {
	    MC mc = MonsterType.getMonsterType(pmid).monsterClass();
    	return MC.GHOST.equals(mc);
    }
    
    //#define tunnels(ptr) (((ptr)->mflags1 & M1_TUNNEL) != 0L)
    //#define needspick(ptr) (((ptr)->mflags1 & M1_NEEDPICK) != 0L)
    //#define hides_under(ptr) (((ptr)->mflags1 & M1_CONCEAL) != 0L)
    //#define is_hider(ptr) (((ptr)->mflags1 & M1_HIDE) != 0L)
    //#define haseyes(ptr) (((ptr)->mflags1 & M1_NOEYES) == 0L)
    //#define eyecount(ptr)                                         \
    //    (!haseyes(ptr) ? 0 : ((ptr) == &mons[PM_CYCLOPS]          \
    //                          || (ptr) == &mons[PM_FLOATING_EYE]) \
    //                             ? 1                              \
    //                             : 2)
    
    public static boolean hasNoHands(int pmid) {
    	return (MonsterType.getMonsterType(pmid).hasFlag1(M1.NOHANDS));
    }
    //#define nohands(ptr) (((ptr)->mflags1 & M1_NOHANDS) != 0L)
    
    //#define nolimbs(ptr) (((ptr)->mflags1 & M1_NOLIMBS) == M1_NOLIMBS)
    //#define notake(ptr) (((ptr)->mflags1 & M1_NOTAKE) != 0L)
    //#define has_head(ptr) (((ptr)->mflags1 & M1_NOHEAD) == 0L)
    //#define has_horns(ptr) (num_horns(ptr) > 0)
    
    public static boolean isWhirly(int pmid) {
	    MC mc = MonsterType.getMonsterType(pmid).monsterClass();
        return (MC.VORTEX.equals(mc) || (PM.AIR_ELEMENTAL.id() == pmid));
    }
    
    public static boolean isFlaming(int pmid) {
    	return PM.matchesOneOf(pmid, PM.FIRE_VORTEX, PM.FLAMING_SPHERE, PM.FIRE_ELEMENTAL, PM.SALAMANDER);
    }
    
    public static boolean isSilent(int pmid) {
    	return (MS.SILENT.id() == MonsterType.monsterSound(pmid));
    }

    //#define unsolid(ptr) (((ptr)->mflags1 & M1_UNSOLID) != 0L)
    //#define mindless(ptr) (((ptr)->mflags1 & M1_MINDLESS) != 0L)
    //#define humanoid(ptr) (((ptr)->mflags1 & M1_HUMANOID) != 0L)
    //#define is_animal(ptr) (((ptr)->mflags1 & M1_ANIMAL) != 0L)
    //#define slithy(ptr) (((ptr)->mflags1 & M1_SLITHY) != 0L)
    //#define is_wooden(ptr) ((ptr) == &mons[PM_WOOD_GOLEM])
    //#define thick_skinned(ptr) (((ptr)->mflags1 & M1_THICK_HIDE) != 0L)
    
    public boolean isSlimeproof(int pmid) {
    	return (PM.GREEN_SLIME.id() == pmid) || isFlaming(pmid) || isNoncorporeal(pmid);
    }
    
    //#define lays_eggs(ptr) (((ptr)->mflags1 & M1_OVIPAROUS) != 0L)
    //#define regenerates(ptr) (((ptr)->mflags1 & M1_REGEN) != 0L)
    //#define perceives(ptr) (((ptr)->mflags1 & M1_SEE_INVIS) != 0L)
    //#define can_teleport(ptr) (((ptr)->mflags1 & M1_TPORT) != 0L)
    //#define control_teleport(ptr) (((ptr)->mflags1 & M1_TPORT_CNTRL) != 0L)
    //#define telepathic(ptr)                                                \
    //    ((ptr) == &mons[PM_FLOATING_EYE] || (ptr) == &mons[PM_MIND_FLAYER] \
    //     || (ptr) == &mons[PM_MASTER_MIND_FLAYER])
    //#define is_armed(ptr) attacktype(ptr, AT_WEAP)
    //#define acidic(ptr) (((ptr)->mflags1 & M1_ACID) != 0L)
    //#define poisonous(ptr) (((ptr)->mflags1 & M1_POIS) != 0L)
    //#define carnivorous(ptr) (((ptr)->mflags1 & M1_CARNIVORE) != 0L)
    //#define herbivorous(ptr) (((ptr)->mflags1 & M1_HERBIVORE) != 0L)
    //#define metallivorous(ptr) (((ptr)->mflags1 & M1_METALLIVORE) != 0L)
    //#define polyok(ptr) (((ptr)->mflags2 & M2_NOPOLY) == 0L)
    //#define is_shapeshifter(ptr) (((ptr)->mflags2 & M2_SHAPESHIFTER) != 0L)
    
    public static boolean isUndead(int pmid) {
    	return MonsterType.getMonsterType(pmid).hasFlag2(M2.UNDEAD);
    }
    
    //#define is_were(ptr) (((ptr)->mflags2 & M2_WERE) != 0L)
    //#define is_elf(ptr) (((ptr)->mflags2 & M2_ELF) != 0L)
    //#define is_dwarf(ptr) (((ptr)->mflags2 & M2_DWARF) != 0L)
    //#define is_gnome(ptr) (((ptr)->mflags2 & M2_GNOME) != 0L)
    //#define is_orc(ptr) (((ptr)->mflags2 & M2_ORC) != 0L)
    //#define is_human(ptr) (((ptr)->mflags2 & M2_HUMAN) != 0L)
    //#define your_race(ptr) (((ptr)->mflags2 & urace.selfmask) != 0L)
    
	public static boolean isBat(int pmid) {
		return PM.matchesOneOf(pmid, PM.BAT, PM.GIANT_BAT, PM.VAMPIRE_BAT);
	}
    
    public static boolean isBird(int pmid) {
	    MC mc = MonsterType.getMonsterType(pmid).monsterClass();
	    return (MC.BAT.equals(mc) && (! isBat(pmid)));
    }
    //#define is_giant(ptr) (((ptr)->mflags2 & M2_GIANT) != 0L)
    
    public static boolean isGolem(int pmid) {
	    MC mc = MonsterType.getMonsterType(pmid).monsterClass();
        return (MC.GOLEM.equals(mc));
    }
    //#define is_domestic(ptr) (((ptr)->mflags2 & M2_DOMESTIC) != 0L)
    //#define is_demon(ptr) (((ptr)->mflags2 & M2_DEMON) != 0L)
    //#define is_mercenary(ptr) (((ptr)->mflags2 & M2_MERC) != 0L)
    //#define is_male(ptr) (((ptr)->mflags2 & M2_MALE) != 0L)
    //#define is_female(ptr) (((ptr)->mflags2 & M2_FEMALE) != 0L)
    //#define is_neuter(ptr) (((ptr)->mflags2 & M2_NEUTER) != 0L)
    //#define is_wanderer(ptr) (((ptr)->mflags2 & M2_WANDER) != 0L)
    //#define always_hostile(ptr) (((ptr)->mflags2 & M2_HOSTILE) != 0L)
    //#define always_peaceful(ptr) (((ptr)->mflags2 & M2_PEACEFUL) != 0L)
    //#define race_hostile(ptr) (((ptr)->mflags2 & urace.hatemask) != 0L)
    //#define race_peaceful(ptr) (((ptr)->mflags2 & urace.lovemask) != 0L)
    //#define extra_nasty(ptr) (((ptr)->mflags2 & M2_NASTY) != 0L)
    //#define strongmonst(ptr) (((ptr)->mflags2 & M2_STRONG) != 0L)
    //#define can_breathe(ptr) attacktype(ptr, AT_BREA)
    
    public static boolean cannotWieldThings(int pmid) {
    	return hasNoHands(pmid) || isVerySmallMonster(pmid);
    }
    
    //#define could_twoweap(ptr) ((ptr)->mattk[1].aatyp == AT_WEAP)
    //#define cantweararm(ptr) (breakarm(ptr) || sliparm(ptr))
    //#define throws_rocks(ptr) (((ptr)->mflags2 & M2_ROCKTHROW) != 0L)
    //#define type_is_pname(ptr) (((ptr)->mflags2 & M2_PNAME) != 0L)
    //#define is_lord(ptr) (((ptr)->mflags2 & M2_LORD) != 0L)
    //#define is_prince(ptr) (((ptr)->mflags2 & M2_PRINCE) != 0L)
    //#define is_ndemon(ptr) \
    //    (is_demon(ptr) && (((ptr)->mflags2 & (M2_LORD | M2_PRINCE)) == 0L))
    //#define is_dlord(ptr) (is_demon(ptr) && is_lord(ptr))
    //#define is_dprince(ptr) (is_demon(ptr) && is_prince(ptr))
    //#define is_minion(ptr) (((ptr)->mflags2 & M2_MINION) != 0L)
    //#define likes_gold(ptr) (((ptr)->mflags2 & M2_GREEDY) != 0L)
    
    public static boolean likesGems(int pmid) {
    	return MonsterType.getMonsterType(pmid).hasFlag2(M2.JEWELS);
    }
    //#define likes_gems(ptr) (((ptr)->mflags2 & M2_JEWELS) != 0L)
    //#define likes_objs(ptr) (((ptr)->mflags2 & M2_COLLECT) != 0L || is_armed(ptr))
    //#define likes_magic(ptr) (((ptr)->mflags2 & M2_MAGIC) != 0L)
    //#define webmaker(ptr) \
    //    ((ptr) == &mons[PM_CAVE_SPIDER] || (ptr) == &mons[PM_GIANT_SPIDER])
    
    public static boolean isUnicorn(int pmid) {
	    MC mc = MonsterType.getMonsterType(pmid).monsterClass();
	    return (MC.UNICORN.equals(mc) && likesGems(pmid));
    }

    //#define is_longworm(ptr)                                                   \
    //    (((ptr) == &mons[PM_BABY_LONG_WORM]) || ((ptr) == &mons[PM_LONG_WORM]) \
    //     || ((ptr) == &mons[PM_LONG_WORM_TAIL]))
    //#define is_covetous(ptr) ((ptr->mflags3 & M3_COVETOUS))
    //#define infravision(ptr) ((ptr->mflags3 & M3_INFRAVISION))
    //#define infravisible(ptr) ((ptr->mflags3 & M3_INFRAVISIBLE))
    //#define is_displacer(ptr) (((ptr)->mflags3 & M3_DISPLACES) != 0L)
    //#define is_mplayer(ptr) \
    //    (((ptr) >= &mons[PM_ARCHEOLOGIST]) && ((ptr) <= &mons[PM_WIZARD]))
    //#define is_watch(ptr) \
    //    ((ptr) == &mons[PM_WATCHMAN] || (ptr) == &mons[PM_WATCH_CAPTAIN])

    public static boolean isRider(int pmid) {
    	return PM.matchesOneOf(pmid, PM.DEATH, PM.FAMINE, PM.PESTILENCE);
    }

    //#define is_placeholder(ptr)                             \
    //    ((ptr) == &mons[PM_ORC] || (ptr) == &mons[PM_GIANT] \
    //     || (ptr) == &mons[PM_ELF] || (ptr) == &mons[PM_HUMAN])
    ///* return TRUE if the monster tends to revive */
    
    public static final boolean isReviver(int pmid) {
	    MC mc = MonsterType.getMonsterType(pmid).monsterClass();
    	return (isRider(pmid) || MC.TROLL.equals(mc));
    }
    
    /* monsters whose corpses and statues need special handling;
       note that high priests and the Wizard of Yendor are flagged
       as unique even though they really aren't; that's ok here */
    public static boolean corpseOrStatueIsUnique(int pmid) {
        int geno = MonsterType.getMonsterType(pmid).generationMask();
	return ((geno & MonsterType.ONLY_GENERATED_ONCE) != 0);
    }

    //
    
    
    public static int emitsLightWithRange(int pmid) {
        MC mc = MonsterType.getMonsterType(pmid).monsterClass();
        if (MC.LIGHT.equals(mc)) {
            return 1;
        } else if (PM.matchesOneOf(pmid, PM.FLAMING_SPHERE, PM.SHOCKING_SPHERE,
	                           PM.FIRE_VORTEX, PM.FIRE_ELEMENTAL)) {
            return 1;
        } else {
            return 0;
        }
    }
    
    //#define likes_lava(ptr) \
    //    (ptr == &mons[PM_FIRE_ELEMENTAL] || ptr == &mons[PM_SALAMANDER])
    //#define pm_invisible(ptr) \
    //    ((ptr) == &mons[PM_STALKER] || (ptr) == &mons[PM_BLACK_LIGHT])
    //
    ///* could probably add more */
    //#define likes_fire(ptr)                                                  \
    //    ((ptr) == &mons[PM_FIRE_VORTEX] || (ptr) == &mons[PM_FLAMING_SPHERE] \
    //     || likes_lava(ptr))
    //
    //#define touch_petrifies(ptr) \
    //    ((ptr) == &mons[PM_COCKATRICE] || (ptr) == &mons[PM_CHICKATRICE])
    //
    //#define is_mind_flayer(ptr) \
    //    ((ptr) == &mons[PM_MIND_FLAYER] || (ptr) == &mons[PM_MASTER_MIND_FLAYER])
    
	public static boolean isVampire(int pmid) {
		MC mc = MonsterType.getMonsterType(pmid).monsterClass();
		return MC.VAMPIRE.equals(mc);
	}
	
	public static boolean isNonliving(int pmid) {
		MC mc = MonsterType.getMonsterType(pmid).monsterClass();
		return (isGolem(pmid) || isUndead(pmid) ||
				MC.VORTEX.equals(mc) || (PM.MANES.id() == pmid));
	}

    ///* Used for conduct with corpses, tins, and digestion attacks */
    ///* G_NOCORPSE monsters might still be swallowed as a purple worm */
    ///* Maybe someday this could be in mflags... */
	public static boolean isVeganOption(int pmid) {
		MC mc = MonsterType.getMonsterType(pmid).monsterClass();

		if (MC.BLOB.equals(mc) || MC.JELLY.equals(mc) || MC.FUNGUS.equals(mc) || MC.VORTEX.equals(mc)
				|| MC.LIGHT.equals(mc)) {
			return true;
		} else if (MC.ELEMENTAL.equals(mc) && (PM.STALKER.id() != pmid)) {
			return true;
		} else if (MC.GOLEM.equals(mc) && (PM.FLESH_GOLEM.id() != pmid) && (PM.LEATHER_GOLEM.id() == pmid)) {
			return true;
		} else if (isNoncorporeal(pmid)) {
			return true;
		} else {
			return false;
		}
	}
	
	public static final boolean isVegetarianOption(int pmid) {
		if (isVeganOption(pmid)) {
			return true;
		}

		MC mc = MonsterType.getMonsterType(pmid).monsterClass();
		if (MC.PUDDING.equals(mc) && (PM.BLACK_PUDDING.id() != pmid)) {
			return true;
		}
		
		return false;
	}
	
    //#define befriend_with_obj(ptr, obj) \
    //    ((obj)->oclass == FOOD_CLASS && is_domestic(ptr))

}
