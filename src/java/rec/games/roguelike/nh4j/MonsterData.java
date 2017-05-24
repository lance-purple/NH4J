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
	
	public static boolean isFlyer(int pmid) {
		return MonsterType.getMonsterType(pmid).hasFlag1(M1.FLY);
	}

    public static boolean isFloater(int pmid) {
	    MC mc = MonsterType.getMonsterType(pmid).monsterClass();
	    return MC.EYE.equals(mc) || MC.LIGHT.equals(mc);
    }

    public static boolean isClinger(int pmid) {
	    return MonsterType.getMonsterType(pmid).hasFlag1(M1.CLING);
    }

    public static boolean isSwimmer(int pmid) {
	    return MonsterType.getMonsterType(pmid).hasFlag1(M1.SWIM);
    }

    public static boolean doesNotBreathe(int pmid) {
	    return MonsterType.getMonsterType(pmid).hasFlag1(M1.BREATHLESS);
    }

    public static boolean isAmphibious(int pmid) {
	    return MonsterType.getMonsterType(pmid).hasFlag1(M1.BREATHLESS | M1.AMPHIBIOUS);
    }

    public static boolean passesThroughWalls(int pmid) {
	    return MonsterType.getMonsterType(pmid).hasFlag1(M1.WALLWALK);
    }

    public static boolean isAmorphous(int pmid) {
	    return MonsterType.getMonsterType(pmid).hasFlag1(M1.AMORPHOUS);
    }

    public static boolean isNoncorporeal(int pmid) {
	    MC mc = MonsterType.getMonsterType(pmid).monsterClass();
    	return MC.GHOST.equals(mc);
    }

    public static boolean isTunneler(int pmid) {
	    return MonsterType.getMonsterType(pmid).hasFlag1(M1.TUNNEL);
    }

    public static boolean needsPickaxe(int pmid) {
	    return MonsterType.getMonsterType(pmid).hasFlag1(M1.NEEDPICK);
    }

    public static boolean hidesUnderStuff(int pmid) {
	    return MonsterType.getMonsterType(pmid).hasFlag1(M1.CONCEAL);
    }

    public static boolean isHider(int pmid) {
	    return MonsterType.getMonsterType(pmid).hasFlag1(M1.HIDE);
    }

    public static boolean hasEyes(int pmid) {
	    return (! MonsterType.getMonsterType(pmid).hasFlag1(M1.NOEYES));
    }

    public static int eyeCount(int pmid) {
	   if (MonsterType.getMonsterType(pmid).hasFlag1(M1.NOEYES)) {
		   return 0;
	   }
	   else if ((PM.CYCLOPS.id() == pmid) || (PM.FLOATING_EYE.id() == pmid)) {
		   return 1;
	   } else {
		   return 2;
	   }
    }
    
    public static boolean hasNoHands(int pmid) {
    	return (MonsterType.getMonsterType(pmid).hasFlag1(M1.NOHANDS));
    }
    
    public static boolean hasNoLimbs(int pmid) {
    	return (MonsterType.getMonsterType(pmid).hasFlag1(M1.NOLIMBS));
    }

    public static boolean doesNotTakeStuff(int pmid) {
    	return (MonsterType.getMonsterType(pmid).hasFlag1(M1.NOTAKE));
    }

    public static boolean hasAHead(int pmid) {
    	return (! MonsterType.getMonsterType(pmid).hasFlag1(M1.NOHEAD));
    }

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

    public static boolean isUnsolid(int pmid) {
    	return (MonsterType.getMonsterType(pmid).hasFlag1(M1.UNSOLID));
    }

    public static boolean isMindless(int pmid) {
    	return (MonsterType.getMonsterType(pmid).hasFlag1(M1.MINDLESS));
    }

    public static boolean isHumanoid(int pmid) {
    	return (MonsterType.getMonsterType(pmid).hasFlag1(M1.HUMANOID));
    }

    public static boolean isAnimal(int pmid) {
    	return (MonsterType.getMonsterType(pmid).hasFlag1(M1.ANIMAL));
    }

    public static boolean isSlithy(int pmid) {
    	return (MonsterType.getMonsterType(pmid).hasFlag1(M1.SLITHY));
    }

    public static boolean isThickSkinned(int pmid) {
    	return (MonsterType.getMonsterType(pmid).hasFlag1(M1.THICK_HIDE));
    }

    //#define is_wooden(ptr) ((ptr) == &mons[PM_WOOD_GOLEM])
    
    public static boolean isSlimeproof(int pmid) {
    	return (PM.GREEN_SLIME.id() == pmid) || isFlaming(pmid) || isNoncorporeal(pmid);
    }
    
    public static boolean laysEggs(int pmid) {
    	return (MonsterType.getMonsterType(pmid).hasFlag1(M1.OVIPAROUS));
    }

    public static boolean regenerates(int pmid) {
    	return (MonsterType.getMonsterType(pmid).hasFlag1(M1.REGEN));
    }

    public static boolean perceivesTheInvisible(int pmid) {
    	return (MonsterType.getMonsterType(pmid).hasFlag1(M1.SEE_INVIS));
    }

    public static boolean canTeleport(int pmid) {
    	return (MonsterType.getMonsterType(pmid).hasFlag1(M1.TPORT));
    }

    public static boolean canControlTeleport(int pmid) {
    	return (MonsterType.getMonsterType(pmid).hasFlag1(M1.TPORT_CNTRL));
    }

    //#define telepathic(ptr)                                                \
    //    ((ptr) == &mons[PM_FLOATING_EYE] || (ptr) == &mons[PM_MIND_FLAYER] \
    //     || (ptr) == &mons[PM_MASTER_MIND_FLAYER])
    //#define is_armed(ptr) attacktype(ptr, AT_WEAP)

    public static boolean isAcidic(int pmid) {
    	return (MonsterType.getMonsterType(pmid).hasFlag1(M1.ACID));
    }

    public static boolean isPoisonous(int pmid) {
    	return (MonsterType.getMonsterType(pmid).hasFlag1(M1.POIS));
    }

    public static boolean isCarnivorous(int pmid) {
    	return (MonsterType.getMonsterType(pmid).hasFlag1(M1.CARNIVORE));
    }

    public static boolean isHerbivorous(int pmid) {
    	return (MonsterType.getMonsterType(pmid).hasFlag1(M1.HERBIVORE));
    }

    public static boolean isMetallivorous(int pmid) {
    	return (MonsterType.getMonsterType(pmid).hasFlag1(M1.METALLIVORE));
    }

    public static boolean okToPolymorphInto(int pmid) {
    	return (! MonsterType.getMonsterType(pmid).hasFlag2(M2.NOPOLY));    	
    }

    public static boolean isShapeshifter(int pmid) {
    	return MonsterType.getMonsterType(pmid).hasFlag2(M2.SHAPESHIFTER);
    }

    public static boolean isUndead(int pmid) {
    	return MonsterType.getMonsterType(pmid).hasFlag2(M2.UNDEAD);
    }

    public static boolean isWere(int pmid) {
    	return MonsterType.getMonsterType(pmid).hasFlag2(M2.WERE);
    }

    public static boolean isElf(int pmid) {
    	return MonsterType.getMonsterType(pmid).hasFlag2(M2.ELF);
    }

    public static boolean isDwarf(int pmid) {
    	return MonsterType.getMonsterType(pmid).hasFlag2(M2.DWARF);
    }

    public static boolean isGnome(int pmid) {
    	return MonsterType.getMonsterType(pmid).hasFlag2(M2.GNOME);
    }

    public static boolean isOrc(int pmid) {
    	return MonsterType.getMonsterType(pmid).hasFlag2(M2.ORC);
    }

    public static boolean isHuman(int pmid) {
    	return MonsterType.getMonsterType(pmid).hasFlag2(M2.HUMAN);
    }

    public static boolean isOfYourRace(int pmid, long yourRace) {
    	return MonsterType.getMonsterType(pmid).hasFlag2(yourRace);
    }
    
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
	
	public static final boolean isAffectedByWeaponFlag1(int pmid, long weaponFlag1) {
		return MonsterType.getMonsterType(pmid).hasFlag1(weaponFlag1);
	}

	public static final boolean isAffectedByWeaponFlag2(int pmid, long weaponFlag2) {
		return MonsterType.getMonsterType(pmid).hasFlag2(weaponFlag2);
	}

    //#define befriend_with_obj(ptr, obj) \
    //    ((obj)->oclass == FOOD_CLASS && is_domestic(ptr))

}
