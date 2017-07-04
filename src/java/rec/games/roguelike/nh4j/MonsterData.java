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
	
	public static boolean isLawfulMinion(int pmid) {
		MonsterType mt = MonsterType.getMonsterType(pmid);
		return (mt.hasFlag2(M2.MINION) && mt.isLawful());
	}
	
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

    public static boolean isArmed(int pmid) {
    	return MonsterType.monsterHasWeaponAttack(pmid);
    }

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
    
	public static boolean isGiant(int pmid) {
    	return MonsterType.getMonsterType(pmid).hasFlag2(M2.GIANT);
	}
    
    public static boolean isGolem(int pmid) {
	    MC mc = MonsterType.getMonsterType(pmid).monsterClass();
        return (MC.GOLEM.equals(mc));
    }

	public static boolean isDomestic(int pmid) {
    	return MonsterType.getMonsterType(pmid).hasFlag2(M2.DOMESTIC);
	}

	public static boolean isDemon(int pmid) {
    	return MonsterType.getMonsterType(pmid).hasFlag2(M2.DEMON);
	}

	public static boolean isMercenary(int pmid) {
    	return MonsterType.getMonsterType(pmid).hasFlag2(M2.MERC);
	}

	public static boolean isMale(int pmid) {
    	return MonsterType.getMonsterType(pmid).hasFlag2(M2.MALE);
	}

	public static boolean isFemale(int pmid) {
    	return MonsterType.getMonsterType(pmid).hasFlag2(M2.FEMALE);
	}

	public static boolean isNeuter(int pmid) {
    	return MonsterType.getMonsterType(pmid).hasFlag2(M2.NEUTER);
	}

	public static boolean isWanderer(int pmid) {
    	return MonsterType.getMonsterType(pmid).hasFlag2(M2.WANDER);
	}

	public static boolean isAlwaysHostile(int pmid) {
    	return MonsterType.getMonsterType(pmid).hasFlag2(M2.HOSTILE);
	}

	public static boolean isAlwaysPeaceful(int pmid) {
    	return MonsterType.getMonsterType(pmid).hasFlag2(M2.PEACEFUL);
	}

	public static boolean racialHostility(int pmid, long hateMask) {
    	return MonsterType.getMonsterType(pmid).hasFlag2(hateMask);
	}

	public static boolean racialFriendship(int pmid, long likeMask) {
    	return MonsterType.getMonsterType(pmid).hasFlag2(likeMask);
	}

	public static boolean isExtraNasty(int pmid) {
    	return MonsterType.getMonsterType(pmid).hasFlag2(M2.NASTY);
	}

	public static boolean isStrongMonster(int pmid) {
    	return MonsterType.getMonsterType(pmid).hasFlag2(M2.STRONG);
	}

    //#define can_breathe(ptr) attacktype(ptr, AT_BREA)
    
    public static boolean cannotWieldThings(int pmid) {
    	return hasNoHands(pmid) || isVerySmallMonster(pmid);
    }
    
    //#define could_twoweap(ptr) ((ptr)->mattk[1].aatyp == AT_WEAP)
    //#define cantweararm(ptr) (breakarm(ptr) || sliparm(ptr))

    public static boolean throwsRocks(int pmid) {
    	return MonsterType.getMonsterType(pmid).hasFlag2(M2.ROCKTHROW);
	}

    public static boolean typeIsProperName(int pmid) {
    	return MonsterType.getMonsterType(pmid).hasFlag2(M2.PNAME);
	}

    public static boolean isLord(int pmid) {
    	return MonsterType.getMonsterType(pmid).hasFlag2(M2.LORD);
	}

    public static boolean isPrince(int pmid) {
    	return MonsterType.getMonsterType(pmid).hasFlag2(M2.PRINCE);
	}
    
    public static boolean isNamelessMajorDemon(int pmid) {
    	MonsterType m = MonsterType.getMonsterType(pmid);
    	return m.hasFlag2(M2.DEMON) && (!m.hasFlag2(M2.LORD)) && (!m.hasFlag2(M2.PRINCE));
    }

    public static boolean isDemonLord(int pmid) {
    	MonsterType m = MonsterType.getMonsterType(pmid);
    	return m.hasFlag2(M2.DEMON) && m.hasFlag2(M2.LORD);
	}

    public static boolean isDemonPrince(int pmid) {
    	MonsterType m = MonsterType.getMonsterType(pmid);
    	return m.hasFlag2(M2.DEMON) && m.hasFlag2(M2.PRINCE);
	}

    public static boolean isMinion(int pmid) {
    	return MonsterType.getMonsterType(pmid).hasFlag2(M2.MINION);
	}

    public static boolean likesGold(int pmid) {
    	return MonsterType.getMonsterType(pmid).hasFlag2(M2.GREEDY);
	}
    
    public static boolean likesGems(int pmid) {
    	return MonsterType.getMonsterType(pmid).hasFlag2(M2.JEWELS);
    }

    public static boolean likesObjects(int pmid) {
    	return MonsterType.getMonsterType(pmid).hasFlag2(M2.COLLECT)
    			|| isArmed(pmid);
    }

    public static boolean likesMagicItems(int pmid) {
    	return MonsterType.getMonsterType(pmid).hasFlag2(M2.MAGIC);
    }

    //#define webmaker(ptr) \
    //    ((ptr) == &mons[PM_CAVE_SPIDER] || (ptr) == &mons[PM_GIANT_SPIDER])
    
    public static boolean isUnicorn(int pmid) {
	    MC mc = MonsterType.getMonsterType(pmid).monsterClass();
	    return (MC.UNICORN.equals(mc) && likesGems(pmid));
    }

    public static boolean isLongWorm(int pmid) {
    	return PM.matchesOneOf(pmid, PM.BABY_LONG_WORM, PM.LONG_WORM, PM.LONG_WORM_TAIL);
    }
    
    public static boolean isCovetous(int pmid) {
    	return MonsterType.getMonsterType(pmid).hasFlag3(M3.COVETOUS);
    }

    public static boolean hasInfravision(int pmid) {
    	return MonsterType.getMonsterType(pmid).hasFlag3(M3.INFRAVISION);
    }

    public static boolean isInfravisible(int pmid) {
    	return MonsterType.getMonsterType(pmid).hasFlag3(M3.INFRAVISIBLE);
    }

    public static boolean isDisplacer(int pmid) {
    	return MonsterType.getMonsterType(pmid).hasFlag3(M3.DISPLACES);
    }

    //#define is_mplayer(ptr) \
    //    (((ptr) >= &mons[PM_ARCHEOLOGIST]) && ((ptr) <= &mons[PM_WIZARD]))
    //#define is_watch(ptr) \
    //    ((ptr) == &mons[PM_WATCHMAN] || (ptr) == &mons[PM_WATCH_CAPTAIN])

    public static boolean isRiderOfApocalypse(int pmid) {
    	return PM.matchesOneOf(pmid, PM.DEATH, PM.FAMINE, PM.PESTILENCE);
    }

    //#define is_placeholder(ptr)                             \
    //    ((ptr) == &mons[PM_ORC] || (ptr) == &mons[PM_GIANT] \
    //     || (ptr) == &mons[PM_ELF] || (ptr) == &mons[PM_HUMAN])
    ///* return TRUE if the monster tends to revive */
    
    public static final boolean isReviver(int pmid) {
	    MC mc = MonsterType.getMonsterType(pmid).monsterClass();
    	return (isRiderOfApocalypse(pmid) || MC.TROLL.equals(mc));
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
    
    public static boolean isMindFlayer(int pmid) {
    	return PM.matchesOneOf(pmid, PM.MIND_FLAYER, PM.MASTER_MIND_FLAYER);
    }
    
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
	
    public static final boolean allowsCloseApproach(int pmid) {
    	return MonsterType.getMonsterType(pmid).hasFlag3(M3.CLOSE);
    }
    
	public static boolean areSameRace(int pmid1, int pmid2) {

	    if (pmid1 == pmid2) {
	        return true; /* exact match */
	    }

	    /* player races have their own predicates */

	    if (isHuman(pmid1)) {
	        return isHuman(pmid2);
	    }

	    if (isElf(pmid1)) {
	        return isElf(pmid2);
	    }

	    if (isDwarf(pmid1)) {
	        return isDwarf(pmid2);
	    }

	    if (isGnome(pmid1)) {
	        return isGnome(pmid2);
	    }

	    if (isOrc(pmid1)) {
	        return isOrc(pmid2);
	    }

	    /* other creatures are less precise */

	    if (isGiant(pmid1)) {
	        return isGiant(pmid2); /* open to quibbling here */
	    }

	    if (isGolem(pmid1)) {
	        return isGolem(pmid2); /* even moreso... */
	    }

	    if (isMindFlayer(pmid1)) {
	        return isMindFlayer(pmid2);
	    }
	    
	    int mc1 = MonsterType.monsterClass(pmid1);
	    int mc2 = MonsterType.monsterClass(pmid2);

	    if ((mc1 == MC.KOBOLD.id()) || PM.matchesOneOf(pmid1, PM.KOBOLD_ZOMBIE, PM.KOBOLD_MUMMY)) {
	    	return ((mc2 == MC.KOBOLD.id()) || PM.matchesOneOf(pmid2, PM.KOBOLD_ZOMBIE, PM.KOBOLD_MUMMY));
	    }

	    if (mc1 == MC.OGRE.id()) {
	    	return (mc2 == MC.OGRE.id());
	    }

	    if (mc1 == MC.NYMPH.id()) {
	    	return (mc2 == MC.NYMPH.id());
	    }

	    if (mc1 == MC.CENTAUR.id()) {
	    	return (mc2 == MC.CENTAUR.id());
	    }

	    if (isUnicorn(pmid1)) {
	    	return isUnicorn(pmid2);
	    }

	    if (mc1 == MC.DRAGON.id()) {
	    	return (mc2 == MC.DRAGON.id());
	    }

	    if (mc1 == MC.NAGA.id()) {
	    	return (mc2 == MC.NAGA.id());
	    }

	    /* other critters get steadily messier */
	    if (isRiderOfApocalypse(pmid1)) {
	    	return isRiderOfApocalypse(pmid2); /* debatable */
	    }

	    if (isMinion(pmid1)) {
	    	return isMinion(pmid2); /* [needs work?] */
	    }

	    /* tengu don't match imps (first test handled case of both being tengu) */
	    if ((pmid1 == PM.TENGU.id()) || (pmid2 == PM.TENGU.id())) {
	    	return false;
	    }

	    if (mc1 == MC.IMP.id()) {
	    	return (mc2 == MC.IMP.id());
	    }
	    /* and minor demons (imps) don't match major demons */
	    else if (mc2 == MC.IMP.id()) {
	    	return false;
	    }

	    if (isDemon(pmid1)) {
	    	return isDemon(pmid2);
	    }

	    if (isUndead(pmid1)) {
	    	if (mc1 == MC.ZOMBIE.id()) {
	    		return (mc2 == MC.ZOMBIE.id());
	    	}
	    	if (mc1 == MC.MUMMY.id()) {
	    		return (mc2 == MC.MUMMY.id());
	    	}
	    	if (mc1 == MC.VAMPIRE.id()) {
	    		return (mc2 == MC.VAMPIRE.id());
	    	}
	    	if (mc1 == MC.LICH.id()) {
	    		return (mc2 == MC.LICH.id());
	    	}
	    	if (mc1 == MC.WRAITH.id()) {
	    		return (mc2 == MC.WRAITH.id());
	    	}
	    	if (mc1 == MC.GHOST.id()) {
	    		return (mc2 == MC.GHOST.id());
	    	}
	    } else if (isUndead(pmid2)) {
	    	return false;
	    }

	    /* check for monsters which grow into more mature forms */

	    if (mc1 == mc2) {

	    	if (MonsterType.growsInto(pmid1, pmid2)) {
	    		return true;
	    	}
	    	if (MonsterType.growsInto(pmid2, pmid1)) {
	    		return true;
	    	}
	    }

	    /* not caught by little/big handling */

	    if (PM.matchesOneOf(pmid1, PM.GARGOYLE, PM.WINGED_GARGOYLE)) {
	    	return (PM.matchesOneOf(pmid2, PM.GARGOYLE, PM.WINGED_GARGOYLE));
	    }

	    if (PM.matchesOneOf(pmid1, PM.KILLER_BEE, PM.QUEEN_BEE)) {
	    	return (PM.matchesOneOf(pmid2, PM.KILLER_BEE, PM.QUEEN_BEE));
	    }

	    if (isLongWorm(pmid1)) {
	    	return isLongWorm(pmid2); /* handles tail */
	    }

	    /* [currently there's no reason to bother matching up
		        assorted bugs and blobs with their closest variants] */

	    /* the types didn't match */

	    return false;
	}
}
