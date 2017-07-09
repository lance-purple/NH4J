package rec.games.roguelike.nh4j;

public class MonsterData {

	public static boolean isVerySmallMonster(int pmid) {
		return MonsterType.monsterSize(pmid) < MZ.SMALL.id();
	}

	public static boolean isSmallMonster(int pmid) {
		return MonsterType.monsterSize(pmid) <= MZ.SMALL.id();
	}

	public static boolean isNotSmallMonster(int pmid) {
		return MonsterType.monsterSize(pmid) > MZ.SMALL.id();
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
    
    public static int numberOfHorns(int pmid) {
    	if (PM.matchesOneOf(pmid, PM.HORNED_DEVIL, PM.MINOTAUR, PM.ASMODEUS, PM.BALROG)) {
    		return 2;
    	} else if (PM.matchesOneOf(pmid, PM.WHITE_UNICORN, PM.GRAY_UNICORN, PM.BLACK_UNICORN, PM.KI_RIN)) {
    		return 1;
    	} else {
    		return 0;
    	}
    }

    public static boolean hasHorns(int pmid) {
    	return (numberOfHorns(pmid) > 0);
    }
    
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

    public static boolean isWooden(int pmid) {
    	return (PM.WOOD_GOLEM.id() == pmid);
    }
    
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

    public static boolean isTelepathic(int pmid) {
    	return PM.matchesOneOf(pmid, PM.FLOATING_EYE, PM.MIND_FLAYER, PM.MASTER_MIND_FLAYER);
    }

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

	public static boolean hasBreathWeapon(int pmid) {
    	return MonsterType.monsterHasBreathWeaponAttack(pmid);
	}
    
    public static boolean cannotWieldThings(int pmid) {
    	return hasNoHands(pmid) || isVerySmallMonster(pmid);
    }

    public static boolean cannotUseTwoWeapons(int pmid) {
    	return (MonsterType.getMonsterType(pmid).numberOfAttacksOfType(AT.WEAP) < 2);
    }

    /* creature will slide out of armor */
    public static boolean slipsOutOfArmor(int pmid) {
        return (boolean) (isWhirly(pmid) || isSmallMonster(pmid)
                          || isNoncorporeal(pmid));
    }

    /* creature will break out of armor */
    public static boolean breaksOutOfArmor(int pmid) {
    	if (slipsOutOfArmor(pmid)) {
    		return false;
    	}

        return isBigMonster(pmid)
                  || (isNotSmallMonster(pmid) && !isHumanoid(pmid))
                  /* winged humanoids cannot wear suits of armor */
                  || PM.matchesOneOf(pmid, PM.MARILITH, PM.WINGED_GARGOYLE);
    }

    public static boolean cannotWearArmor(int pmid) {
    	return breaksOutOfArmor(pmid) || slipsOutOfArmor(pmid);
    }
 
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

    public static boolean makesWebs(int pmid) {
    	return PM.matchesOneOf(pmid, PM.CAVE_SPIDER, PM.GIANT_SPIDER);
    }
    
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

    public static boolean isMonsterPlayer(int pmid) {
    	return (pmid >= PM.ARCHEOLOGIST.id()) && (pmid <= PM.WIZARD.id());
    }
    
    public static boolean isWatchman(int pmid) {
    	return PM.matchesOneOf(pmid, PM.WATCHMAN, PM.WATCH_CAPTAIN);
    }

    public static boolean isRiderOfTheApocalypse(int pmid) {
    	return PM.matchesOneOf(pmid, PM.DEATH, PM.FAMINE, PM.PESTILENCE);
    }

    public static boolean isPlaceholderMonster(int pmid) {
    	return PM.matchesOneOf(pmid, PM.ORC, PM.GIANT, PM.ELF, PM.HUMAN);
    }
    
    /* monsters that tend to revive */
    public static final boolean isReviver(int pmid) {
	    MC mc = MonsterType.getMonsterType(pmid).monsterClass();
    	return (isRiderOfTheApocalypse(pmid) || MC.TROLL.equals(mc));
    }
    
    /* monsters whose corpses and statues need special handling;
       note that high priests and the Wizard of Yendor are flagged
       as unique even though they really aren't; that's ok here */
    public static boolean corpseOrStatueIsUnique(int pmid) {
        int geno = MonsterType.getMonsterType(pmid).generationMask();
	return ((geno & MonsterType.ONLY_GENERATED_ONCE) != 0);
    }
    
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
    
    public static boolean likesLava(int pmid) {
    	return PM.matchesOneOf(pmid, PM.FIRE_ELEMENTAL, PM.SALAMANDER);
    }
    
    public static boolean isInvisibleMonsterType(int pmid) {
    	return PM.matchesOneOf(pmid, PM.STALKER, PM.BLACK_LIGHT);
    }
    
    public static boolean likesFire(int pmid) {
    	return PM.matchesOneOf(pmid, PM.FIRE_VORTEX, PM.FLAMING_SPHERE) || likesLava(pmid);
    }
 
    public static boolean touchPetrifies(int pmid) {
    	return PM.matchesOneOf(pmid, PM.COCKATRICE, PM.CHICKATRICE);
    }

    public static boolean fleshPetrifies(int pmid) {
    	return (touchPetrifies(pmid) || (PM.MEDUSA.id() == pmid));
    }

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

}
