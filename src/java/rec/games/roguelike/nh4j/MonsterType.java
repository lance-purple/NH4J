package rec.games.roguelike.nh4j;

import java.util.ArrayList;
import java.util.HashMap;

public class MonsterType {

	private static HashMap<Integer, MonsterType> monsterTypesByPMID = new HashMap<>();;

	private static final int FREQUENCY_MASK = 0x7;
	private static final int LEAVES_NO_CORPSE = 0x10;
	private static final int GENOCIDABLE = 0x20;
	private static final int APPEARS_IN_LARGE_GROUPS = 0x40;
	private static final int APPEARS_IN_SMALL_GROUPS = 0x80;
	private static final int ONLY_GENERATED_SPECIALLY = 0x200;
	private static final int ONLY_GENERATED_IN_HELL = 0x400;
	private static final int NOT_GENERATED_IN_HELL = 0x800;
	private static final int ONLY_GENERATED_ONCE = 0x1000;

	private static final MonsterType UNKNOWN_MONSTER_TYPE = new MonsterType(MC.UNKNOWN, PM.UNKNOWN, "UNKNOWN");

	private PM pm;
	private MC monsterClass;
	private String name;

	private int monsterLevel;
	private int movementSpeed;
	private int baseArmorClass;
	private int baseMagicResistance;

	private int alignment;

	private int generationMask;

	private ArrayList<Attack> attacks = new ArrayList<>();

	private int corpseWeight;
	private int corpseNutritionalValue;

	private MS makesSound;
	private MZ size;
	private long hasResistances;
	private long conveysResistances;

	private long flags1;
	private long flags2;
	private long flags3;

	private CLR color;

	public MonsterType(MC mc, PM pm, String name) {
		this.pm = pm;
		this.name = name;
		this.monsterClass = mc;
	}
	
	public MC monsterClass() {
		return monsterClass;
	}

	private static MonsterType of(MC mc, PM pm, String name) {
		return new MonsterType(mc, pm, name);
	}

	public MonsterType level(int level) {
		this.monsterLevel = level;
		return this;
	}

	public MonsterType move(int move) {
		this.movementSpeed = move;
		return this;
	}

	public MonsterType ac(int ac) {
		this.baseArmorClass = ac;
		return this;
	}

	public MonsterType mr(int mr) {
		this.baseMagicResistance = mr;
		return this;
	}

	public MonsterType neutral() {
		this.alignment = 0;
		return this;
	}

	public MonsterType lawful(int amount) {
		this.alignment = amount;
		return this;
	}

	public MonsterType chaotic(int amount) {
		this.alignment = -amount;
		return this;
	}

	public MonsterType genocidable() {
		this.generationMask |= GENOCIDABLE;
		return this;
	}

	public MonsterType smallGroups() {
		this.generationMask |= APPEARS_IN_SMALL_GROUPS;
		return this;
	}

	public MonsterType largeGroups() {
		this.generationMask |= APPEARS_IN_LARGE_GROUPS;
		return this;
	}

	public MonsterType notInHell() {
		this.generationMask |= NOT_GENERATED_IN_HELL;
		return this;
	}

	public MonsterType onlyInHell() {
		this.generationMask |= ONLY_GENERATED_IN_HELL;
		return this;
	}

	public MonsterType noCorpse() {
		this.generationMask |= LEAVES_NO_CORPSE;
		return this;
	}

	public MonsterType unique() {
		this.generationMask |= ONLY_GENERATED_ONCE;
		return this;
	}

	public MonsterType special() {
		this.generationMask |= ONLY_GENERATED_SPECIALLY;
		return this;
	}

	public MonsterType noGenerate() {
		this.generationMask |= ONLY_GENERATED_SPECIALLY;
		return this;
	}

	public MonsterType freq(int frequency) {
		this.generationMask |= (frequency & FREQUENCY_MASK);
		return this;
	}

	public MonsterType attack(AT type, AD damage, int numberOfDice, Dice dice) {
		attacks.add(new Attack(type, damage, numberOfDice, dice));
		return this;
	}

	public MonsterType weight(int weight) {
		this.corpseWeight = weight;
		return this;
	}

	public MonsterType nutrition(int nutrition) {
		this.corpseNutritionalValue = nutrition;
		return this;
	}

	public MonsterType sound(MS sound) {
		this.makesSound = sound;
		return this;
	}

	public MonsterType size(MZ size) {
		this.size = size;
		return this;
	}

	public MonsterType resists(Integer... resists) {
		for (int resist : resists) {
			this.hasResistances |= resist;
		}
		return this;
	}

	public MonsterType conveys(Integer... conveyances) {
		for (int resist : conveyances) {
			this.conveysResistances |= resist;
		}
		return this;
	}

	public MonsterType flags1(Long... flags) {
		for (long flag : flags) {
			this.flags1 |= flag;
		}
		return this;
	}

	public MonsterType flags2(Long... flags) {
		for (long flag : flags) {
			this.flags2 |= flag;
		}
		return this;
	}
	
	
	public long flags2() {
		return this.flags2;
	}

	
	public MonsterType flags3(Long... flags) {
		for (long flag : flags) {
			this.flags3 |= flag;
		}
		return this;
	}

	public MonsterType color(CLR color) {
		this.color = color;
		return this;
	}

	private void add() {
		monsterTypesByPMID.put(pm.id(), this);
	}

	private static void initializeAnts() {

		MonsterType.of(MC.ANT, PM.GIANT_ANT, "giant ant").level(2).move(18).ac(3).mr(0).neutral().freq(3).smallGroups()
				.genocidable().attack(AT.BITE, AD.PHYS, 1, Dice.D4).weight(10).nutrition(10).sound(MS.SILENT)
				.size(MZ.TINY).flags1(M1.ANIMAL, M1.CARNIVORE, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE, M1.OVIPAROUS)
				.flags2(M2.HOSTILE).color(CLR.BROWN).add();

		MonsterType.of(MC.ANT, PM.KILLER_BEE, "killer bee").level(1).move(18).ac(-1).mr(0).neutral().freq(2)
				.largeGroups().genocidable().attack(AT.STNG, AD.DRST, 1, Dice.D3).weight(1).nutrition(5).sound(MS.BUZZ)
				.size(MZ.TINY).resists(MR.POISON).conveys(MR.POISON)
				.flags1(M1.ANIMAL, M1.FLY, M1.NOHANDS, M1.NOLIMBS, M1.POIS).flags2(M2.FEMALE, M2.HOSTILE)
				.color(CLR.YELLOW).add();

		MonsterType.of(MC.ANT, PM.SOLDIER_ANT, "soldier ant").level(3).move(18).ac(3).mr(0).neutral().freq(2)
				.smallGroups().genocidable().attack(AT.BITE, AD.PHYS, 2, Dice.D4).attack(AT.STNG, AD.DRST, 3, Dice.D4)
				.weight(20).nutrition(5).sound(MS.SILENT).size(MZ.TINY).resists(MR.POISON).conveys(MR.POISON)
				.flags1(M1.ANIMAL, M1.CARNIVORE, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE, M1.OVIPAROUS, M1.POIS)
				.flags2(M2.HOSTILE).color(CLR.BLUE).add();

		MonsterType.of(MC.ANT, PM.FIRE_ANT, "fire ant").level(3).move(18).ac(3).mr(10).neutral().freq(1).smallGroups()
				.genocidable().attack(AT.BITE, AD.PHYS, 2, Dice.D4).attack(AT.BITE, AD.FIRE, 2, Dice.D4).weight(30)
				.nutrition(10).sound(MS.SILENT).size(MZ.TINY).resists(MR.FIRE).conveys(MR.FIRE)
				.flags1(M1.ANIMAL, M1.CARNIVORE, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE, M1.OVIPAROUS).flags2(M2.HOSTILE)
				.flags3(M3.INFRAVISIBLE).color(CLR.RED).add();

		MonsterType.of(MC.ANT, PM.GIANT_BEETLE, "giant beetle").level(5).move(6).ac(4).mr(0).neutral().freq(3)
				.genocidable().attack(AT.BITE, AD.PHYS, 3, Dice.D6).weight(10).nutrition(10).sound(MS.SILENT)
				.size(MZ.LARGE).resists(MR.POISON).conveys(MR.POISON)
				.flags1(M1.ANIMAL, M1.CARNIVORE, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE, M1.POIS).flags2(M2.HOSTILE)
				.color(CLR.BLACK).add();

		MonsterType.of(MC.ANT, PM.QUEEN_BEE, "queen bee").level(9).move(24).ac(-4).mr(0).neutral().freq(0).special()
				.genocidable().attack(AT.STNG, AD.DRST, 1, Dice.D8).weight(1).nutrition(5).sound(MS.BUZZ).size(MZ.TINY)
				.resists(MR.POISON).conveys(MR.POISON)
				.flags1(M1.ANIMAL, M1.FLY, M1.NOHANDS, M1.NOLIMBS, M1.OVIPAROUS, M1.POIS)
				.flags2(M2.FEMALE, M2.HOSTILE, M2.PRINCE).color(CLR.MAGENTA).add();
	}

	private static void initializeBlobs() {

		MonsterType.of(MC.BLOB, PM.ACID_BLOB, "acid blob").level(1).move(3).ac(8).mr(0).neutral().freq(2).genocidable()
				.weight(30).nutrition(10).sound(MS.SILENT).size(MZ.TINY).resists(MR.SLEEP, MR.POISON, MR.ACID, MR.STONE)
				.conveys(MR.STONE)
				.flags1(M1.ACID, M1.AMORPHOUS, M1.BREATHLESS, M1.MINDLESS, M1.NOEYES, M1.NOHANDS, M1.NOHEAD, M1.NOLIMBS)
				.flags2(M2.NEUTER, M2.WANDER).color(CLR.GREEN).add();

		MonsterType.of(MC.BLOB, PM.QUIVERING_BLOB, "quivering blob").level(5).move(1).ac(8).mr(0).neutral().freq(2)
				.genocidable().attack(AT.TUCH, AD.PHYS, 1, Dice.D8).weight(200).nutrition(100).sound(MS.SILENT)
				.size(MZ.SMALL).resists(MR.SLEEP, MR.POISON).conveys(MR.POISON)
				.flags1(M1.MINDLESS, M1.NOEYES, M1.NOHANDS, M1.NOHEAD, M1.NOLIMBS)
				.flags2(M2.HOSTILE, M2.NEUTER, M2.WANDER).color(CLR.WHITE).add();

		MonsterType.of(MC.BLOB, PM.GELATINOUS_CUBE, "gelatinous cube").level(6).move(6).ac(8).mr(0).neutral().freq(2)
				.genocidable().attack(AT.TUCH, AD.PLYS, 2, Dice.D4).weight(600).nutrition(150).sound(MS.SILENT)
				.size(MZ.LARGE).resists(MR.FIRE, MR.COLD, MR.SLEEP, MR.ELEC, MR.POISON, MR.ACID, MR.STONE)
				.conveys(MR.FIRE, MR.COLD, MR.SLEEP, MR.ELEC).flags1(M1.ACID, M1.CARNIVORE, M1.HERBIVORE, M1.MINDLESS,
						M1.NOEYES, M1.NOHANDS, M1.NOHEAD, M1.NOLIMBS, M1.OMNIVORE)
				.flags2(M2.HOSTILE, M2.NEUTER, M2.WANDER).color(CLR.CYAN).add();
	}

	private static void initializeCockatrices() {
		
		MonsterType.of(MC.COCKATRICE, PM.CHICKATRICE, "chickatrice").level(4).move(4).ac(8).mr(30).neutral().freq(1)
				.smallGroups().genocidable().attack(AT.BITE, AD.PHYS, 1, Dice.D2).attack(AT.TUCH, AD.STON, 0, Dice.D0)
				.weight(10).nutrition(10).sound(MS.HISS).size(MZ.TINY).resists(MR.POISON, MR.STONE)
				.conveys(MR.POISON, MR.STONE)
				.flags1(M1.ANIMAL, M1.CARNIVORE, M1.HERBIVORE, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE).flags2(M2.HOSTILE)
				.flags3(M3.INFRAVISIBLE).color(CLR.BROWN).add();

		MonsterType.of(MC.COCKATRICE, PM.COCKATRICE, "cockatrice").level(5).move(6).ac(6).mr(30).neutral().freq(5)
				.genocidable().attack(AT.BITE, AD.PHYS, 1, Dice.D3).attack(AT.TUCH, AD.STON, 0, Dice.D0).weight(30)
				.nutrition(30).sound(MS.HISS).size(MZ.SMALL).resists(MR.POISON, MR.STONE).conveys(MR.POISON, MR.STONE)
				.flags1(M1.ANIMAL, M1.CARNIVORE, M1.HERBIVORE, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE, M1.OVIPAROUS)
				.flags2(M2.HOSTILE).flags3(M3.INFRAVISIBLE).color(CLR.YELLOW).add();

		MonsterType.of(MC.COCKATRICE, PM.PYROLISK, "pyrolisk").level(6).move(6).ac(6).mr(30).neutral().freq(1)
				.genocidable().attack(AT.GAZE, AD.FIRE, 2, Dice.D6).weight(30).nutrition(30).sound(MS.HISS)
				.size(MZ.SMALL).resists(MR.FIRE, MR.POISON).conveys(MR.FIRE, MR.POISON)
				.flags1(M1.ANIMAL, M1.CARNIVORE, M1.HERBIVORE, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE, M1.OVIPAROUS)
				.flags2(M2.HOSTILE).flags3(M3.INFRAVISIBLE).color(CLR.RED).add();
	}

	private static void initializeDogs() {
		
		MonsterType.of(MC.DOG, PM.JACKAL, "jackal").level(0).move(12).ac(7).mr(0).neutral().freq(3).smallGroups()
				.genocidable().attack(AT.BITE, AD.PHYS, 1, Dice.D2).weight(300).nutrition(250).sound(MS.BARK)
				.size(MZ.SMALL).flags1(M1.ANIMAL, M1.CARNIVORE, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE).flags2(M2.HOSTILE)
				.flags3(M3.INFRAVISIBLE).color(CLR.BROWN).add();

		MonsterType.of(MC.DOG, PM.FOX, "fox").level(0).move(15).ac(7).mr(0).neutral().freq(1).genocidable()
				.attack(AT.BITE, AD.PHYS, 1, Dice.D3).weight(300).nutrition(250).sound(MS.BARK).size(MZ.SMALL)
				.flags1(M1.ANIMAL, M1.CARNIVORE, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE).flags2(M2.HOSTILE)
				.flags3(M3.INFRAVISIBLE).color(CLR.RED).add();

		MonsterType.of(MC.DOG, PM.COYOTE, "coyote").level(1).move(12).ac(7).mr(0).neutral().freq(1).smallGroups()
				.genocidable().attack(AT.BITE, AD.PHYS, 1, Dice.D4).weight(300).nutrition(250).sound(MS.BARK)
				.size(MZ.SMALL).flags1(M1.ANIMAL, M1.CARNIVORE, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE).flags2(M2.HOSTILE)
				.flags3(M3.INFRAVISIBLE).color(CLR.BROWN).add();

		MonsterType.of(MC.DOG, PM.WEREJACKAL, "werejackal").level(2).move(12).ac(7).mr(10).chaotic(7).freq(0).special()
				.noCorpse().attack(AT.BITE, AD.WERE, 1, Dice.D4).weight(300).nutrition(250).sound(MS.BARK)
				.size(MZ.SMALL).resists(MR.POISON)
				.flags1(M1.CARNIVORE, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE, M1.POIS, M1.REGEN)
				.flags2(M2.HOSTILE, M2.NOPOLY, M2.WERE).flags3(M3.INFRAVISIBLE).color(CLR.BROWN).add();

		MonsterType.of(MC.DOG, PM.LITTLE_DOG, "little dog").level(2).move(18).ac(6).mr(0).neutral().freq(1)
				.genocidable().attack(AT.BITE, AD.PHYS, 1, Dice.D6).weight(150).nutrition(150).sound(MS.BARK)
				.size(MZ.SMALL).flags1(M1.ANIMAL, M1.CARNIVORE, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE).flags2(M2.DOMESTIC)
				.flags3(M3.INFRAVISIBLE).color(CLR.WHITE).add();

		MonsterType.of(MC.DOG, PM.DINGO, "dingo").level(4).move(16).ac(5).mr(0).neutral().freq(1).genocidable()
				.attack(AT.BITE, AD.PHYS, 1, Dice.D6).weight(400).nutrition(200).sound(MS.BARK).size(MZ.MEDIUM)
				.flags1(M1.ANIMAL, M1.CARNIVORE, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE).flags2(M2.HOSTILE)
				.flags3(M3.INFRAVISIBLE).color(CLR.YELLOW).add();

		MonsterType.of(MC.DOG, PM.DOG, "dog").level(4).move(16).ac(5).mr(0).neutral().freq(1).genocidable()
				.attack(AT.BITE, AD.PHYS, 1, Dice.D6).weight(400).nutrition(200).sound(MS.BARK).size(MZ.MEDIUM)
				.flags1(M1.ANIMAL, M1.CARNIVORE, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE).flags2(M2.DOMESTIC)
				.flags3(M3.INFRAVISIBLE).color(CLR.WHITE).add();

		MonsterType.of(MC.DOG, PM.LARGE_DOG, "large dog").level(6).move(15).ac(4).mr(0).neutral().freq(1).genocidable()
				.attack(AT.BITE, AD.PHYS, 2, Dice.D4).weight(800).nutrition(250).sound(MS.BARK).size(MZ.MEDIUM)
				.flags1(M1.ANIMAL, M1.CARNIVORE, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE).flags2(M2.DOMESTIC, M2.STRONG)
				.flags3(M3.INFRAVISIBLE).color(CLR.WHITE).add();

		MonsterType.of(MC.DOG, PM.WOLF, "wolf").level(5).move(12).ac(4).mr(0).neutral().freq(2).smallGroups()
				.genocidable().attack(AT.BITE, AD.PHYS, 2, Dice.D4).weight(500).nutrition(250).sound(MS.BARK)
				.size(MZ.MEDIUM).flags1(M1.ANIMAL, M1.CARNIVORE, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE).flags2(M2.HOSTILE)
				.flags3(M3.INFRAVISIBLE).color(CLR.BROWN).add();

		MonsterType.of(MC.DOG, PM.WEREWOLF, "werewolf").level(5).move(12).ac(4).mr(20).chaotic(7).freq(0).special()
				.noCorpse().attack(AT.BITE, AD.WERE, 2, Dice.D6).weight(500).nutrition(250).sound(MS.BARK)
				.size(MZ.MEDIUM).resists(MR.POISON)
				.flags1(M1.CARNIVORE, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE, M1.POIS, M1.REGEN)
				.flags2(M2.HOSTILE, M2.NOPOLY, M2.WERE).flags3(M3.INFRAVISIBLE).color(CLR.BROWN).add();

		MonsterType.of(MC.DOG, PM.WINTER_WOLF_CUB, "winter wolf cub").level(5).move(12).ac(4).mr(0).chaotic(5).freq(2)
				.notInHell().smallGroups().genocidable().attack(AT.BITE, AD.PHYS, 1, Dice.D8)
				.attack(AT.BREA, AD.COLD, 1, Dice.D8).weight(250).nutrition(200).sound(MS.BARK).size(MZ.SMALL)
				.resists(MR.COLD).conveys(MR.COLD).flags1(M1.ANIMAL, M1.CARNIVORE, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE)
				.flags2(M2.HOSTILE).color(CLR.CYAN).add();

		MonsterType.of(MC.DOG, PM.WARG, "warg").level(7).move(12).ac(4).mr(0).chaotic(5).freq(2).smallGroups()
				.genocidable().attack(AT.BITE, AD.PHYS, 2, Dice.D6).weight(850).nutrition(350).sound(MS.BARK)
				.size(MZ.MEDIUM).flags1(M1.ANIMAL, M1.CARNIVORE, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE).flags2(M2.HOSTILE)
				.flags3(M3.INFRAVISIBLE).color(CLR.BROWN).add();

		MonsterType.of(MC.DOG, PM.WINTER_WOLF, "winter wolf").level(7).move(12).ac(4).mr(20).neutral().freq(1)
				.notInHell().genocidable().attack(AT.BITE, AD.PHYS, 2, Dice.D6).attack(AT.BREA, AD.COLD, 2, Dice.D6)
				.weight(700).nutrition(300).sound(MS.BARK).size(MZ.LARGE).resists(MR.COLD).conveys(MR.COLD)
				.flags1(M1.ANIMAL, M1.CARNIVORE, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE).flags2(M2.HOSTILE, M2.STRONG)
				.color(CLR.CYAN).add();

		MonsterType.of(MC.DOG, PM.HELL_HOUND_PUP, "hell hound pup").level(7).move(12).ac(4).mr(20).chaotic(5).freq(1)
				.onlyInHell().smallGroups().genocidable().attack(AT.BITE, AD.PHYS, 2, Dice.D6)
				.attack(AT.BREA, AD.FIRE, 2, Dice.D6).weight(200).nutrition(200).sound(MS.BARK).size(MZ.SMALL)
				.resists(MR.FIRE).conveys(MR.FIRE).flags1(M1.ANIMAL, M1.CARNIVORE, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE)
				.flags2(M2.HOSTILE).flags3(M3.INFRAVISIBLE).color(CLR.RED).add();

		MonsterType.of(MC.DOG, PM.HELL_HOUND, "hell hound").level(12).move(14).ac(2).mr(20).neutral().freq(1)
				.onlyInHell().genocidable().attack(AT.BITE, AD.PHYS, 3, Dice.D6).attack(AT.BREA, AD.FIRE, 3, Dice.D6)
				.weight(600).nutrition(300).sound(MS.BARK).size(MZ.MEDIUM).resists(MR.FIRE).conveys(MR.FIRE)
				.flags1(M1.ANIMAL, M1.CARNIVORE, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE).flags2(M2.HOSTILE, M2.STRONG)
				.flags3(M3.INFRAVISIBLE).color(CLR.RED).add();
	}

	private static void initializeEyes() {

		MonsterType.of(MC.EYE, PM.GAS_SPORE, "gas spore").level(1).move(3).ac(10).mr(0).neutral().freq(1).genocidable()
				.noCorpse().attack(AT.BOOM, AD.PHYS, 4, Dice.D6).weight(10).nutrition(10).sound(MS.SILENT)
				.size(MZ.SMALL).flags1(M1.BREATHLESS, M1.FLY, M1.MINDLESS, M1.NOHANDS, M1.NOHEAD, M1.NOLIMBS)
				.flags2(M2.HOSTILE, M2.NEUTER).color(CLR.GRAY).add();

		MonsterType.of(MC.EYE, PM.FLOATING_EYE, "floating eye").level(2).move(1).ac(9).mr(10).neutral().freq(5)
				.genocidable().weight(10).nutrition(10).sound(MS.SILENT).size(MZ.SMALL)
				.flags1(M1.AMPHIBIOUS, M1.FLY, M1.NOHANDS, M1.NOHEAD, M1.NOLIMBS, M1.NOTAKE)
				.flags2(M2.HOSTILE, M2.NEUTER).flags3(M3.INFRAVISIBLE).color(CLR.BLUE).add();

		MonsterType.of(MC.EYE, PM.FREEZING_SPHERE, "freezing sphere").level(6).move(13).ac(4).mr(0).neutral().freq(2)
				.notInHell().genocidable().noCorpse().attack(AT.EXPL, AD.COLD, 4, Dice.D6).weight(10).nutrition(10)
				.sound(MS.SILENT).size(MZ.SMALL).resists(MR.COLD).conveys(MR.COLD)
				.flags1(M1.BREATHLESS, M1.FLY, M1.MINDLESS, M1.NOHANDS, M1.NOHEAD, M1.NOLIMBS, M1.NOTAKE)
				.flags2(M2.HOSTILE, M2.NEUTER).flags3(M3.INFRAVISIBLE).color(CLR.WHITE).add();

		MonsterType.of(MC.EYE, PM.FLAMING_SPHERE, "flaming sphere").level(6).move(13).ac(4).mr(0).neutral().freq(2)
				.genocidable().noCorpse().attack(AT.EXPL, AD.FIRE, 4, Dice.D6).weight(10).nutrition(10).sound(MS.SILENT)
				.size(MZ.SMALL).resists(MR.FIRE).conveys(MR.FIRE)
				.flags1(M1.BREATHLESS, M1.FLY, M1.MINDLESS, M1.NOHANDS, M1.NOHEAD, M1.NOLIMBS, M1.NOTAKE)
				.flags2(M2.HOSTILE, M2.NEUTER).flags3(M3.INFRAVISIBLE).color(CLR.RED).add();

		MonsterType.of(MC.EYE, PM.SHOCKING_SPHERE, "shocking sphere").level(6).move(13).ac(4).mr(0).neutral().freq(2)
				.genocidable().noCorpse().attack(AT.EXPL, AD.ELEC, 4, Dice.D6).weight(10).nutrition(10).sound(MS.SILENT)
				.size(MZ.SMALL).resists(MR.ELEC).conveys(MR.ELEC)
				.flags1(M1.BREATHLESS, M1.FLY, M1.MINDLESS, M1.NOHANDS, M1.NOHEAD, M1.NOLIMBS, M1.NOTAKE)
				.flags2(M2.HOSTILE, M2.NEUTER).flags3(M3.INFRAVISIBLE).color(CLR.BRIGHT_BLUE).add();
	}

	private static void initializeFelines() {

		MonsterType.of(MC.FELINE, PM.KITTEN, "kitten").level(2).move(18).ac(6).mr(0).neutral().freq(1).genocidable()
				.attack(AT.BITE, AD.PHYS, 1, Dice.D6).weight(150).nutrition(150).sound(MS.MEW).size(MZ.SMALL)
				.flags1(M1.ANIMAL, M1.CARNIVORE, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE).flags2(M2.DOMESTIC, M2.WANDER)
				.flags3(M3.INFRAVISIBLE).color(CLR.WHITE).add();

		MonsterType.of(MC.FELINE, PM.HOUSECAT, "housecat").level(4).move(16).ac(5).mr(0).neutral().freq(1).genocidable()
				.attack(AT.BITE, AD.PHYS, 1, Dice.D6).weight(200).nutrition(200).sound(MS.MEW).size(MZ.SMALL)
				.flags1(M1.ANIMAL, M1.CARNIVORE, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE).flags2(M2.DOMESTIC)
				.flags3(M3.INFRAVISIBLE).color(CLR.WHITE).add();

		MonsterType.of(MC.FELINE, PM.JAGUAR, "jaguar").level(4).move(15).ac(6).mr(0).neutral().freq(2).genocidable()
				.attack(AT.CLAW, AD.PHYS, 1, Dice.D4).attack(AT.CLAW, AD.PHYS, 1, Dice.D4)
				.attack(AT.BITE, AD.PHYS, 1, Dice.D8).weight(600).nutrition(300).sound(MS.GROWL).size(MZ.LARGE)
				.flags1(M1.ANIMAL, M1.CARNIVORE, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE).flags2(M2.HOSTILE)
				.flags3(M3.INFRAVISIBLE).color(CLR.BROWN).add();

		MonsterType.of(MC.FELINE, PM.LYNX, "lynx").level(5).move(15).ac(6).mr(0).neutral().freq(1).genocidable()
				.attack(AT.CLAW, AD.PHYS, 1, Dice.D4).attack(AT.CLAW, AD.PHYS, 1, Dice.D4)
				.attack(AT.BITE, AD.PHYS, 1, Dice.D10).weight(600).nutrition(300).sound(MS.GROWL).size(MZ.SMALL)
				.flags1(M1.ANIMAL, M1.CARNIVORE, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE).flags2(M2.HOSTILE)
				.flags3(M3.INFRAVISIBLE).color(CLR.CYAN).add();

		MonsterType.of(MC.FELINE, PM.PANTHER, "panther").level(5).move(15).ac(6).mr(0).neutral().freq(1).genocidable()
				.attack(AT.CLAW, AD.PHYS, 1, Dice.D6).attack(AT.CLAW, AD.PHYS, 1, Dice.D6)
				.attack(AT.BITE, AD.PHYS, 1, Dice.D10).weight(600).nutrition(300).sound(MS.GROWL).size(MZ.LARGE)
				.flags1(M1.ANIMAL, M1.CARNIVORE, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE).flags2(M2.HOSTILE)
				.flags3(M3.INFRAVISIBLE).color(CLR.BLACK).add();

		MonsterType.of(MC.FELINE, PM.LARGE_CAT, "large cat").level(6).move(15).ac(4).mr(0).neutral().freq(1)
				.genocidable().attack(AT.BITE, AD.PHYS, 2, Dice.D4).weight(250).nutrition(250).sound(MS.MEW)
				.size(MZ.SMALL).flags1(M1.ANIMAL, M1.CARNIVORE, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE)
				.flags2(M2.DOMESTIC, M2.STRONG).flags3(M3.INFRAVISIBLE).color(CLR.WHITE).add();

		MonsterType.of(MC.FELINE, PM.TIGER, "tiger").level(6).move(12).ac(6).mr(0).neutral().freq(2).genocidable()
				.attack(AT.CLAW, AD.PHYS, 2, Dice.D4).attack(AT.CLAW, AD.PHYS, 2, Dice.D4)
				.attack(AT.BITE, AD.PHYS, 1, Dice.D10).weight(600).nutrition(300).sound(MS.GROWL).size(MZ.LARGE)
				.flags1(M1.ANIMAL, M1.CARNIVORE, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE).flags2(M2.HOSTILE)
				.flags3(M3.INFRAVISIBLE).color(CLR.YELLOW).add();
	}

	private static void initializeGremlins() {
		
		MonsterType.of(MC.GREMLIN, PM.GREMLIN, "gremlin").level(5).move(12).ac(2).mr(25).chaotic(9).freq(2)
				.genocidable().attack(AT.CLAW, AD.PHYS, 1, Dice.D6).attack(AT.CLAW, AD.PHYS, 1, Dice.D6)
				.attack(AT.BITE, AD.PHYS, 1, Dice.D4).attack(AT.CLAW, AD.CURS, 0, Dice.D0).weight(100).nutrition(20)
				.sound(MS.LAUGH).size(MZ.SMALL).resists(MR.POISON).conveys(MR.POISON)
				.flags1(M1.HUMANOID, M1.POIS, M1.SWIM).flags2(M2.STALK).flags3(M3.INFRAVISIBLE).color(CLR.GREEN).add();

		MonsterType.of(MC.GREMLIN, PM.GARGOYLE, "gargoyle").level(6).move(10).ac(-4).mr(0).chaotic(9).freq(2)
				.genocidable().attack(AT.CLAW, AD.PHYS, 2, Dice.D6).attack(AT.CLAW, AD.PHYS, 2, Dice.D6)
				.attack(AT.BITE, AD.PHYS, 2, Dice.D4).weight(1000).nutrition(200).sound(MS.GRUNT).size(MZ.HUMAN)
				.resists(MR.STONE).conveys(MR.STONE).flags1(M1.BREATHLESS, M1.HUMANOID, M1.THICK_HIDE)
				.flags2(M2.HOSTILE, M2.STRONG).color(CLR.BROWN).add();

		MonsterType.of(MC.GREMLIN, PM.WINGED_GARGOYLE, "winged gargoyle").level(9).move(15).ac(-2).mr(0).chaotic(12)
				.freq(1).genocidable().attack(AT.CLAW, AD.PHYS, 3, Dice.D6).attack(AT.CLAW, AD.PHYS, 3, Dice.D6)
				.attack(AT.BITE, AD.PHYS, 3, Dice.D4).weight(1200).nutrition(300).sound(MS.GRUNT).size(MZ.HUMAN)
				.resists(MR.STONE).conveys(MR.STONE)
				.flags1(M1.BREATHLESS, M1.FLY, M1.HUMANOID, M1.OVIPAROUS, M1.THICK_HIDE)
				.flags2(M2.HOSTILE, M2.LORD, M2.MAGIC, M2.STRONG).color(CLR.MAGENTA).add();
	}

	private static void initializeHumanoids() {
		
		MonsterType.of(MC.HUMANOID, PM.HOBBIT, "hobbit").level(1).move(9).ac(10).mr(0).lawful(6).freq(2).genocidable()
				.attack(AT.WEAP, AD.PHYS, 1, Dice.D6).weight(500).nutrition(200).sound(MS.HUMANOID).size(MZ.SMALL)
				.flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE).flags2(M2.COLLECT)
				.flags3(M3.INFRAVISION, M3.INFRAVISIBLE).color(CLR.GREEN).add();

		MonsterType.of(MC.HUMANOID, PM.DWARF, "dwarf").level(2).move(6).ac(10).mr(10).lawful(4).freq(3).genocidable()
				.attack(AT.WEAP, AD.PHYS, 1, Dice.D8).weight(900).nutrition(300).sound(MS.HUMANOID).size(MZ.HUMAN)
				.flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.NEEDPICK, M1.OMNIVORE, M1.TUNNEL)
				.flags2(M2.COLLECT, M2.DWARF, M2.GREEDY, M2.JEWELS, M2.NOPOLY, M2.STRONG)
				.flags3(M3.INFRAVISION, M3.INFRAVISIBLE).color(CLR.RED).add();

		MonsterType.of(MC.HUMANOID, PM.BUGBEAR, "bugbear").level(3).move(9).ac(5).mr(0).chaotic(6).freq(1).genocidable()
				.attack(AT.WEAP, AD.PHYS, 2, Dice.D4).weight(1250).nutrition(250).sound(MS.GROWL).size(MZ.LARGE)
				.flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE).flags2(M2.COLLECT, M2.STRONG)
				.flags3(M3.INFRAVISION, M3.INFRAVISIBLE).color(CLR.BROWN).add();

		MonsterType.of(MC.HUMANOID, PM.DWARF_LORD, "dwarf lord").level(4).move(6).ac(10).mr(10).lawful(5).freq(2)
				.genocidable().attack(AT.WEAP, AD.PHYS, 2, Dice.D4).attack(AT.WEAP, AD.PHYS, 2, Dice.D4).weight(900)
				.nutrition(300).sound(MS.HUMANOID).size(MZ.HUMAN)
				.flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.NEEDPICK, M1.OMNIVORE, M1.TUNNEL)
				.flags2(M2.COLLECT, M2.DWARF, M2.GREEDY, M2.JEWELS, M2.LORD, M2.MALE, M2.STRONG)
				.flags3(M3.INFRAVISION, M3.INFRAVISIBLE).color(CLR.BLUE).add();

		MonsterType.of(MC.HUMANOID, PM.DWARF_KING, "dwarf king").level(6).move(6).ac(10).mr(20).lawful(6).freq(1)
				.genocidable().attack(AT.WEAP, AD.PHYS, 2, Dice.D6).attack(AT.WEAP, AD.PHYS, 2, Dice.D6).weight(900)
				.nutrition(300).sound(MS.HUMANOID).size(MZ.HUMAN)
				.flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.NEEDPICK, M1.OMNIVORE, M1.TUNNEL)
				.flags2(M2.COLLECT, M2.DWARF, M2.GREEDY, M2.JEWELS, M2.MALE, M2.PRINCE, M2.STRONG)
				.flags3(M3.INFRAVISION, M3.INFRAVISIBLE).color(CLR.MAGENTA).add();

		MonsterType.of(MC.HUMANOID, PM.MIND_FLAYER, "mind flayer").level(9).move(12).ac(5).mr(90).chaotic(8).freq(1)
				.genocidable().attack(AT.WEAP, AD.PHYS, 1, Dice.D4).attack(AT.TENT, AD.DRIN, 2, Dice.D1)
				.attack(AT.TENT, AD.DRIN, 2, Dice.D1).attack(AT.TENT, AD.DRIN, 2, Dice.D1).weight(1450).nutrition(400)
				.sound(MS.HISS).size(MZ.HUMAN)
				.flags1(M1.CARNIVORE, M1.FLY, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE, M1.SEE_INVIS)
				.flags2(M2.COLLECT, M2.GREEDY, M2.HOSTILE, M2.JEWELS, M2.NASTY).flags3(M3.INFRAVISION, M3.INFRAVISIBLE)
				.color(CLR.MAGENTA).add();

		MonsterType.of(MC.HUMANOID, PM.MASTER_MIND_FLAYER, "master mind flayer").level(13).move(12).ac(0).mr(90)
				.chaotic(8).freq(1).genocidable().attack(AT.WEAP, AD.PHYS, 1, Dice.D8)
				.attack(AT.TENT, AD.DRIN, 2, Dice.D1).attack(AT.TENT, AD.DRIN, 2, Dice.D1)
				.attack(AT.TENT, AD.DRIN, 2, Dice.D1).attack(AT.TENT, AD.DRIN, 2, Dice.D1)
				.attack(AT.TENT, AD.DRIN, 2, Dice.D1).weight(1450).nutrition(400).sound(MS.HISS).size(MZ.HUMAN)
				.flags1(M1.CARNIVORE, M1.FLY, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE, M1.SEE_INVIS)
				.flags2(M2.COLLECT, M2.GREEDY, M2.HOSTILE, M2.JEWELS, M2.NASTY).flags3(M3.INFRAVISION, M3.INFRAVISIBLE)
				.color(CLR.MAGENTA).add();
	}

	private static void initializeImps() {

		MonsterType.of(MC.IMP, PM.MANES, "manes").level(1).move(3).ac(7).mr(0).chaotic(7).freq(1).largeGroups()
				.genocidable().noCorpse().attack(AT.CLAW, AD.PHYS, 1, Dice.D3).attack(AT.CLAW, AD.PHYS, 1, Dice.D3)
				.attack(AT.BITE, AD.PHYS, 1, Dice.D4).weight(100).nutrition(100).sound(MS.SILENT).size(MZ.SMALL)
				.resists(MR.SLEEP, MR.POISON).flags1(M1.POIS).flags2(M2.HOSTILE, M2.STALK)
				.flags3(M3.INFRAVISION, M3.INFRAVISIBLE).color(CLR.RED).add();

		MonsterType.of(MC.IMP, PM.HOMUNCULUS, "homunculus").level(2).move(12).ac(6).mr(10).chaotic(7).freq(2)
				.genocidable().attack(AT.BITE, AD.SLEE, 1, Dice.D3).weight(60).nutrition(100).sound(MS.SILENT)
				.size(MZ.TINY).resists(MR.SLEEP, MR.POISON).conveys(MR.SLEEP, MR.POISON).flags1(M1.FLY, M1.POIS)
				.flags2(M2.STALK).flags3(M3.INFRAVISION, M3.INFRAVISIBLE).color(CLR.GREEN).add();

		MonsterType.of(MC.IMP, PM.IMP, "imp").level(3).move(12).ac(2).mr(20).chaotic(7).freq(1).genocidable()
				.attack(AT.CLAW, AD.PHYS, 1, Dice.D4).weight(20).nutrition(10).sound(MS.CUSS).size(MZ.TINY)
				.flags1(M1.REGEN).flags2(M2.STALK, M2.WANDER).flags3(M3.INFRAVISION, M3.INFRAVISIBLE).color(CLR.RED)
				.add();

		MonsterType.of(MC.IMP, PM.LEMURE, "lemure").level(3).move(3).ac(7).mr(0).chaotic(7).freq(1).onlyInHell()
				.largeGroups().genocidable().noCorpse().attack(AT.CLAW, AD.PHYS, 1, Dice.D3).weight(150).nutrition(100)
				.sound(MS.SILENT).size(MZ.MEDIUM).resists(MR.SLEEP, MR.POISON).conveys(MR.SLEEP)
				.flags1(M1.POIS, M1.REGEN).flags2(M2.HOSTILE, M2.NEUTER, M2.STALK, M2.WANDER)
				.flags3(M3.INFRAVISION, M3.INFRAVISIBLE).color(CLR.BROWN).add();

		MonsterType.of(MC.IMP, PM.QUASIT, "quasit").level(3).move(15).ac(2).mr(20).chaotic(7).freq(2).genocidable()
				.attack(AT.CLAW, AD.DRDX, 1, Dice.D2).attack(AT.CLAW, AD.DRDX, 1, Dice.D2)
				.attack(AT.BITE, AD.PHYS, 1, Dice.D4).weight(200).nutrition(200).sound(MS.SILENT).size(MZ.SMALL)
				.resists(MR.POISON).conveys(MR.POISON).flags1(M1.REGEN).flags2(M2.STALK)
				.flags3(M3.INFRAVISION, M3.INFRAVISIBLE).color(CLR.BLUE).add();

		MonsterType.of(MC.IMP, PM.TENGU, "tengu").level(6).move(13).ac(5).mr(30).lawful(7).freq(3).genocidable()
				.attack(AT.BITE, AD.PHYS, 1, Dice.D7).weight(300).nutrition(200).sound(MS.SQAWK).size(MZ.SMALL)
				.resists(MR.POISON).conveys(MR.POISON).flags1(M1.TPORT, M1.TPORT_CNTRL).flags2(M2.STALK)
				.flags3(M3.INFRAVISION, M3.INFRAVISIBLE).color(CLR.CYAN).add();
	}

	private static void initializeJellies() {
		
		MonsterType.of(MC.JELLY, PM.BLUE_JELLY, "blue jelly").level(4).move(0).ac(8).mr(10).neutral().freq(2)
				.genocidable().weight(50).nutrition(20).sound(MS.SILENT).size(MZ.MEDIUM)
				.resists(MR.COLD, MR.POISON).conveys(MR.COLD, MR.POISON).flags1(M1.AMORPHOUS, M1.BREATHLESS,
						M1.MINDLESS, M1.NOEYES, M1.NOHANDS, M1.NOHEAD, M1.NOLIMBS, M1.NOTAKE)
				.flags2(M2.HOSTILE, M2.NEUTER).color(CLR.BLUE).add();

		MonsterType.of(MC.JELLY, PM.SPOTTED_JELLY, "spotted jelly").level(5).move(0).ac(8).mr(10).neutral().freq(1)
				.genocidable().weight(50).nutrition(20).sound(MS.SILENT).size(MZ.MEDIUM)
				.resists(MR.ACID, MR.STONE).flags1(M1.ACID, M1.AMORPHOUS, M1.BREATHLESS, M1.MINDLESS, M1.NOEYES,
						M1.NOHANDS, M1.NOHEAD, M1.NOLIMBS, M1.NOTAKE)
				.flags2(M2.HOSTILE, M2.NEUTER).color(CLR.GREEN).add();

		MonsterType.of(MC.JELLY, PM.OCHRE_JELLY, "ochre jelly").level(6).move(3).ac(8).mr(20).neutral().freq(2)
				.genocidable().attack(AT.ENGL, AD.ACID, 3, Dice.D6).weight(50).nutrition(20).sound(MS.SILENT)
				.size(MZ.MEDIUM)
				.resists(MR.ACID, MR.STONE).flags1(M1.ACID, M1.AMORPHOUS, M1.BREATHLESS, M1.MINDLESS, M1.NOEYES,
						M1.NOHANDS, M1.NOHEAD, M1.NOLIMBS, M1.NOTAKE)
				.flags2(M2.HOSTILE, M2.NEUTER).color(CLR.BROWN).add();
	}

	private static void initializeKobolds() {

		MonsterType.of(MC.KOBOLD, PM.KOBOLD, "kobold").level(0).move(6).ac(10).mr(0).chaotic(2).freq(1).genocidable()
				.attack(AT.WEAP, AD.PHYS, 1, Dice.D4).weight(400).nutrition(100).sound(MS.GRUNT).size(MZ.SMALL)
				.resists(MR.POISON).flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE, M1.POIS)
				.flags2(M2.COLLECT, M2.HOSTILE).flags3(M3.INFRAVISION, M3.INFRAVISIBLE).color(CLR.BROWN).add();

		MonsterType.of(MC.KOBOLD, PM.LARGE_KOBOLD, "large kobold").level(1).move(6).ac(10).mr(0).chaotic(3).freq(1)
				.genocidable().attack(AT.WEAP, AD.PHYS, 1, Dice.D6).weight(450).nutrition(150).sound(MS.GRUNT)
				.size(MZ.SMALL).resists(MR.POISON).flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE, M1.POIS)
				.flags2(M2.COLLECT, M2.HOSTILE).flags3(M3.INFRAVISION, M3.INFRAVISIBLE).color(CLR.RED).add();

		MonsterType.of(MC.KOBOLD, PM.KOBOLD_LORD, "kobold lord").level(2).move(6).ac(10).mr(0).chaotic(4).freq(1)
				.genocidable().attack(AT.WEAP, AD.PHYS, 2, Dice.D4).weight(500).nutrition(200).sound(MS.GRUNT)
				.size(MZ.SMALL).resists(MR.POISON).flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE, M1.POIS)
				.flags2(M2.COLLECT, M2.HOSTILE, M2.LORD, M2.MALE).flags3(M3.INFRAVISION, M3.INFRAVISIBLE)
				.color(CLR.MAGENTA).add();

		MonsterType.of(MC.KOBOLD, PM.KOBOLD_SHAMAN, "kobold shaman").level(2).move(6).ac(6).mr(10).chaotic(4).freq(1)
				.genocidable().attack(AT.MAGC, AD.SPEL, 0, Dice.D0).weight(450).nutrition(150).sound(MS.GRUNT)
				.size(MZ.SMALL).resists(MR.POISON).flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE, M1.POIS)
				.flags2(M2.HOSTILE, M2.MAGIC).flags3(M3.INFRAVISION, M3.INFRAVISIBLE).color(CLR.BRIGHT_BLUE).add();
	}

	private static void initializeLeprechauns() {

		MonsterType.of(MC.LEPRECHAUN, PM.LEPRECHAUN, "leprechaun").level(5).move(15).ac(8).mr(20).neutral().freq(4)
				.genocidable().attack(AT.CLAW, AD.SGLD, 1, Dice.D2).weight(60).nutrition(30).sound(MS.LAUGH)
				.size(MZ.TINY).flags1(M1.HUMANOID, M1.TPORT).flags2(M2.GREEDY, M2.HOSTILE).flags3(M3.INFRAVISIBLE)
				.color(CLR.GREEN).add();
	}

	private static void initializeMimics() {

		MonsterType.of(MC.MIMIC, PM.SMALL_MIMIC, "small mimic").level(7).move(3).ac(7).mr(0).neutral().freq(2)
				.genocidable().attack(AT.CLAW, AD.PHYS, 3, Dice.D4).weight(300).nutrition(200).sound(MS.SILENT)
				.size(MZ.HUMAN)
				.resists(MR.ACID).flags1(M1.AMORPHOUS, M1.ANIMAL, M1.BREATHLESS, M1.CARNIVORE, M1.HIDE, M1.NOEYES,
						M1.NOHANDS, M1.NOHEAD, M1.NOLIMBS, M1.OMNIVORE, M1.THICK_HIDE)
				.flags2(M2.HOSTILE).color(CLR.BROWN).add();

		MonsterType.of(MC.MIMIC, PM.LARGE_MIMIC, "large mimic").level(8).move(3).ac(7).mr(10).neutral().freq(1)
				.genocidable().attack(AT.CLAW, AD.STCK, 3, Dice.D4).weight(600).nutrition(400).sound(MS.SILENT)
				.size(MZ.LARGE).resists(MR.ACID)
				.flags1(M1.AMORPHOUS, M1.ANIMAL, M1.BREATHLESS, M1.CARNIVORE, M1.CLING, M1.HIDE, M1.NOEYES, M1.NOHANDS,
						M1.NOHEAD, M1.NOLIMBS, M1.OMNIVORE, M1.THICK_HIDE)
				.flags2(M2.HOSTILE, M2.STRONG).color(CLR.RED).add();

		MonsterType.of(MC.MIMIC, PM.GIANT_MIMIC, "giant mimic").level(9).move(3).ac(7).mr(20).neutral().freq(1)
				.genocidable().attack(AT.CLAW, AD.STCK, 3, Dice.D6).attack(AT.CLAW, AD.STCK, 3, Dice.D6).weight(800)
				.nutrition(500).sound(MS.SILENT).size(MZ.LARGE).resists(MR.ACID)
				.flags1(M1.AMORPHOUS, M1.ANIMAL, M1.BREATHLESS, M1.CARNIVORE, M1.CLING, M1.HIDE, M1.NOEYES, M1.NOHANDS,
						M1.NOHEAD, M1.NOLIMBS, M1.OMNIVORE, M1.THICK_HIDE)
				.flags2(M2.HOSTILE, M2.STRONG).color(CLR.MAGENTA).add();
	}

	private static void initializeNymphs() {

		MonsterType.of(MC.NYMPH, PM.WOOD_NYMPH, "wood nymph").level(3).move(12).ac(9).mr(20).neutral().freq(2)
				.genocidable().attack(AT.CLAW, AD.SITM, 0, Dice.D0).attack(AT.CLAW, AD.SEDU, 0, Dice.D0).weight(600)
				.nutrition(300).sound(MS.SEDUCE).size(MZ.HUMAN).flags1(M1.HUMANOID, M1.TPORT)
				.flags2(M2.COLLECT, M2.FEMALE, M2.HOSTILE).flags3(M3.INFRAVISIBLE).color(CLR.GREEN).add();

		MonsterType.of(MC.NYMPH, PM.WATER_NYMPH, "water nymph").level(3).move(12).ac(9).mr(20).neutral().freq(2)
				.genocidable().attack(AT.CLAW, AD.SITM, 0, Dice.D0).attack(AT.CLAW, AD.SEDU, 0, Dice.D0).weight(600)
				.nutrition(300).sound(MS.SEDUCE).size(MZ.HUMAN).flags1(M1.HUMANOID, M1.SWIM, M1.TPORT)
				.flags2(M2.COLLECT, M2.FEMALE, M2.HOSTILE).flags3(M3.INFRAVISIBLE).color(CLR.BLUE).add();

		MonsterType.of(MC.NYMPH, PM.MOUNTAIN_NYMPH, "mountain nymph").level(3).move(12).ac(9).mr(20).neutral().freq(2)
				.genocidable().attack(AT.CLAW, AD.SITM, 0, Dice.D0).attack(AT.CLAW, AD.SEDU, 0, Dice.D0).weight(600)
				.nutrition(300).sound(MS.SEDUCE).size(MZ.HUMAN).flags1(M1.HUMANOID, M1.TPORT)
				.flags2(M2.COLLECT, M2.FEMALE, M2.HOSTILE).flags3(M3.INFRAVISIBLE).color(CLR.BROWN).add();
	}

	private static void initializeOrcs() {

		MonsterType.of(MC.ORC, PM.GOBLIN, "goblin").level(0).move(6).ac(10).mr(0).chaotic(3).freq(2).genocidable()
				.attack(AT.WEAP, AD.PHYS, 1, Dice.D4).weight(400).nutrition(100).sound(MS.GRUNT).size(MZ.SMALL)
				.flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE).flags2(M2.COLLECT, M2.ORC)
				.flags3(M3.INFRAVISION, M3.INFRAVISIBLE).color(CLR.GRAY).add();

		MonsterType.of(MC.ORC, PM.HOBGOBLIN, "hobgoblin").level(1).move(9).ac(10).mr(0).chaotic(4).freq(2).genocidable()
				.attack(AT.WEAP, AD.PHYS, 1, Dice.D6).weight(1000).nutrition(200).sound(MS.GRUNT).size(MZ.HUMAN)
				.flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE).flags2(M2.COLLECT, M2.ORC, M2.STRONG)
				.flags3(M3.INFRAVISION, M3.INFRAVISIBLE).color(CLR.BROWN).add();

		MonsterType.of(MC.ORC, PM.ORC, "orc").level(1).move(9).ac(10).mr(0).chaotic(3).freq(0).special().largeGroups()
				.genocidable().attack(AT.WEAP, AD.PHYS, 1, Dice.D8).weight(850).nutrition(150).sound(MS.GRUNT)
				.size(MZ.HUMAN).flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.GREEDY, M2.JEWELS, M2.NOPOLY, M2.ORC, M2.STRONG)
				.flags3(M3.INFRAVISION, M3.INFRAVISIBLE).color(CLR.RED).add();

		MonsterType.of(MC.ORC, PM.HILL_ORC, "hill orc").level(2).move(9).ac(10).mr(0).chaotic(4).freq(2).largeGroups()
				.genocidable().attack(AT.WEAP, AD.PHYS, 1, Dice.D6).weight(1000).nutrition(200).sound(MS.GRUNT)
				.size(MZ.HUMAN).flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.GREEDY, M2.JEWELS, M2.ORC, M2.STRONG).flags3(M3.INFRAVISION, M3.INFRAVISIBLE)
				.color(CLR.YELLOW).add();

		MonsterType.of(MC.ORC, PM.MORDOR_ORC, "Mordor orc").level(3).move(5).ac(10).mr(0).chaotic(5).freq(1)
				.largeGroups().genocidable().attack(AT.WEAP, AD.PHYS, 1, Dice.D6).weight(1200).nutrition(200)
				.sound(MS.GRUNT).size(MZ.HUMAN).flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.GREEDY, M2.JEWELS, M2.ORC, M2.STRONG).flags3(M3.INFRAVISION, M3.INFRAVISIBLE)
				.color(CLR.BLUE).add();

		MonsterType.of(MC.ORC, PM.URUK_HAI, "Uruk-hai").level(3).move(7).ac(10).mr(0).chaotic(4).freq(1).largeGroups()
				.genocidable().attack(AT.WEAP, AD.PHYS, 1, Dice.D8).weight(1300).nutrition(300).sound(MS.GRUNT)
				.size(MZ.HUMAN).flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.GREEDY, M2.JEWELS, M2.ORC, M2.STRONG).flags3(M3.INFRAVISION, M3.INFRAVISIBLE)
				.color(CLR.BLACK).add();

		MonsterType.of(MC.ORC, PM.ORC_SHAMAN, "orc shaman").level(3).move(9).ac(5).mr(10).chaotic(5).freq(1)
				.genocidable().attack(AT.MAGC, AD.SPEL, 0, Dice.D0).weight(1000).nutrition(300).sound(MS.GRUNT)
				.size(MZ.HUMAN).flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.GREEDY, M2.JEWELS, M2.MAGIC, M2.ORC, M2.STRONG).flags3(M3.INFRAVISION, M3.INFRAVISIBLE)
				.color(CLR.BRIGHT_BLUE).add();

		MonsterType.of(MC.ORC, PM.ORC_CAPTAIN, "orc-captain").level(5).move(5).ac(10).mr(0).chaotic(5).freq(1)
				.genocidable().attack(AT.WEAP, AD.PHYS, 2, Dice.D4).attack(AT.WEAP, AD.PHYS, 2, Dice.D4).weight(1350)
				.nutrition(350).sound(MS.GRUNT).size(MZ.HUMAN)
				.flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.GREEDY, M2.JEWELS, M2.ORC, M2.STRONG).flags3(M3.INFRAVISION, M3.INFRAVISIBLE)
				.color(CLR.MAGENTA).add();
	}

	private static void initializePiercers() {

		MonsterType.of(MC.PIERCER, PM.ROCK_PIERCER, "rock piercer").level(3).move(1).ac(3).mr(0).neutral().freq(4)
				.genocidable().attack(AT.BITE, AD.PHYS, 2, Dice.D6).weight(200)
				.nutrition(200).sound(MS.SILENT).size(MZ.SMALL).flags1(M1.ANIMAL, M1.CARNIVORE, M1.CLING, M1.HIDE,
						M1.NOEYES, M1.NOHANDS, M1.NOLIMBS, M1.NOTAKE, M1.OMNIVORE)
				.flags2(M2.HOSTILE).color(CLR.GRAY).add();

		MonsterType.of(MC.PIERCER, PM.IRON_PIERCER, "iron piercer").level(5).move(1).ac(0).mr(0).neutral().freq(2)
				.genocidable().attack(AT.BITE, AD.PHYS, 3, Dice.D6).weight(400)
				.nutrition(300).sound(MS.SILENT).size(MZ.MEDIUM).flags1(M1.ANIMAL, M1.CARNIVORE, M1.CLING, M1.HIDE,
						M1.NOEYES, M1.NOHANDS, M1.NOLIMBS, M1.NOTAKE, M1.OMNIVORE)
				.flags2(M2.HOSTILE).color(CLR.CYAN).add();

		MonsterType.of(MC.PIERCER, PM.GLASS_PIERCER, "glass piercer").level(7).move(1).ac(0).mr(0).neutral().freq(1)
				.genocidable().attack(AT.BITE, AD.PHYS, 4, Dice.D6).weight(400).nutrition(300)
				.sound(MS.SILENT).size(MZ.MEDIUM).resists(MR.ACID).flags1(M1.ANIMAL, M1.CARNIVORE, M1.CLING, M1.HIDE,
						M1.NOEYES, M1.NOHANDS, M1.NOLIMBS, M1.NOTAKE, M1.OMNIVORE)
				.flags2(M2.HOSTILE).color(CLR.WHITE).add();
	}

	private static void initializeQuadrupeds() {

		MonsterType.of(MC.QUADRUPED, PM.ROTHE, "rothe").level(2).move(9).ac(7).mr(0).neutral().freq(4).smallGroups()
				.genocidable().attack(AT.CLAW, AD.PHYS, 1, Dice.D3).attack(AT.BITE, AD.PHYS, 1, Dice.D3)
				.attack(AT.BITE, AD.PHYS, 1, Dice.D8).weight(400).nutrition(100).sound(MS.SILENT).size(MZ.LARGE)
				.flags1(M1.ANIMAL, M1.CARNIVORE, M1.HERBIVORE, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE).flags2(M2.HOSTILE)
				.flags3(M3.INFRAVISIBLE).color(CLR.BROWN).add();

		MonsterType.of(MC.QUADRUPED, PM.MUMAK, "mumak").level(5).move(9).ac(0).mr(0).chaotic(2).freq(1).genocidable()
				.attack(AT.BUTT, AD.PHYS, 4, Dice.D12).attack(AT.BITE, AD.PHYS, 2, Dice.D6).weight(2500).nutrition(500)
				.sound(MS.ROAR).size(MZ.LARGE)
				.flags1(M1.ANIMAL, M1.HERBIVORE, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE, M1.THICK_HIDE)
				.flags2(M2.HOSTILE, M2.STRONG).flags3(M3.INFRAVISIBLE).color(CLR.GRAY).add();

		MonsterType.of(MC.QUADRUPED, PM.LEOCROTTA, "leocrotta").level(6).move(18).ac(4).mr(10).neutral().freq(2)
				.genocidable().attack(AT.CLAW, AD.PHYS, 2, Dice.D6).attack(AT.BITE, AD.PHYS, 2, Dice.D6)
				.attack(AT.CLAW, AD.PHYS, 2, Dice.D6).weight(1200).nutrition(500).sound(MS.IMITATE).size(MZ.LARGE)
				.flags1(M1.ANIMAL, M1.CARNIVORE, M1.HERBIVORE, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE)
				.flags2(M2.HOSTILE, M2.STRONG).flags3(M3.INFRAVISIBLE).color(CLR.RED).add();

		MonsterType.of(MC.QUADRUPED, PM.WUMPUS, "wumpus").level(8).move(3).ac(2).mr(10).neutral().freq(1).genocidable()
				.attack(AT.BITE, AD.PHYS, 3, Dice.D6).weight(2500).nutrition(500).sound(MS.BURBLE).size(MZ.LARGE)
				.flags1(M1.ANIMAL, M1.CARNIVORE, M1.CLING, M1.HERBIVORE, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE)
				.flags2(M2.HOSTILE, M2.STRONG).flags3(M3.INFRAVISIBLE).color(CLR.CYAN).add();

		MonsterType.of(MC.QUADRUPED, PM.TITANOTHERE, "titanothere").level(12).move(12).ac(6).mr(0).neutral().freq(2)
				.genocidable().attack(AT.CLAW, AD.PHYS, 2, Dice.D8).weight(2650).nutrition(650).sound(MS.SILENT)
				.size(MZ.LARGE).flags1(M1.ANIMAL, M1.HERBIVORE, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE, M1.THICK_HIDE)
				.flags2(M2.HOSTILE, M2.STRONG).flags3(M3.INFRAVISIBLE).color(CLR.GRAY).add();

		MonsterType.of(MC.QUADRUPED, PM.BALUCHITHERIUM, "baluchitherium").level(14).move(12).ac(5).mr(0).neutral()
				.freq(2).genocidable().attack(AT.CLAW, AD.PHYS, 5, Dice.D4).attack(AT.CLAW, AD.PHYS, 5, Dice.D4)
				.weight(3800).nutrition(800).sound(MS.SILENT).size(MZ.LARGE)
				.flags1(M1.ANIMAL, M1.HERBIVORE, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE, M1.THICK_HIDE)
				.flags2(M2.HOSTILE, M2.STRONG).flags3(M3.INFRAVISIBLE).color(CLR.GRAY).add();

		MonsterType.of(MC.QUADRUPED, PM.MASTODON, "mastodon").level(20).move(12).ac(5).mr(0).neutral().freq(1)
				.genocidable().attack(AT.BUTT, AD.PHYS, 4, Dice.D8).attack(AT.BUTT, AD.PHYS, 4, Dice.D8).weight(3800)
				.nutrition(800).sound(MS.SILENT).size(MZ.LARGE)
				.flags1(M1.ANIMAL, M1.HERBIVORE, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE, M1.THICK_HIDE)
				.flags2(M2.HOSTILE, M2.STRONG).flags3(M3.INFRAVISIBLE).color(CLR.BLACK).add();
	}

	private static void initializeRodents() {

		MonsterType.of(MC.RODENT, PM.SEWER_RAT, "sewer rat").level(0).move(12).ac(7).mr(0).neutral().freq(1)
				.smallGroups().genocidable().attack(AT.BITE, AD.PHYS, 1, Dice.D3).weight(20).nutrition(12)
				.sound(MS.SQEEK).size(MZ.TINY).flags1(M1.ANIMAL, M1.CARNIVORE, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE)
				.flags2(M2.HOSTILE).flags3(M3.INFRAVISIBLE).color(CLR.BROWN).add();

		MonsterType.of(MC.RODENT, PM.GIANT_RAT, "giant rat").level(1).move(10).ac(7).mr(0).neutral().freq(2)
				.smallGroups().genocidable().attack(AT.BITE, AD.PHYS, 1, Dice.D3).weight(30).nutrition(30)
				.sound(MS.SQEEK).size(MZ.TINY).flags1(M1.ANIMAL, M1.CARNIVORE, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE)
				.flags2(M2.HOSTILE).flags3(M3.INFRAVISIBLE).color(CLR.BROWN).add();

		MonsterType.of(MC.RODENT, PM.RABID_RAT, "rabid rat").level(2).move(12).ac(6).mr(0).neutral().freq(1)
				.genocidable().attack(AT.BITE, AD.DRCO, 2, Dice.D4).weight(30).nutrition(5).sound(MS.SQEEK)
				.size(MZ.TINY).resists(MR.POISON)
				.flags1(M1.ANIMAL, M1.CARNIVORE, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE, M1.POIS).flags2(M2.HOSTILE)
				.flags3(M3.INFRAVISIBLE).color(CLR.BROWN).add();

		MonsterType.of(MC.RODENT, PM.WERERAT, "wererat").level(2).move(12).ac(6).mr(10).chaotic(7).freq(0).special()
				.noCorpse().attack(AT.BITE, AD.WERE, 1, Dice.D4).weight(40).nutrition(30).sound(MS.SQEEK).size(MZ.TINY)
				.resists(MR.POISON).flags1(M1.CARNIVORE, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE, M1.POIS, M1.REGEN)
				.flags2(M2.HOSTILE, M2.NOPOLY, M2.WERE).flags3(M3.INFRAVISIBLE).color(CLR.BROWN).add();

		MonsterType.of(MC.RODENT, PM.ROCK_MOLE, "rock mole").level(3).move(3).ac(0).mr(20).neutral().freq(2)
				.genocidable().attack(AT.BITE, AD.PHYS, 1, Dice.D6).weight(30).nutrition(30).sound(MS.SILENT)
				.size(MZ.SMALL).flags1(M1.ANIMAL, M1.METALLIVORE, M1.NOHANDS, M1.NOLIMBS, M1.TUNNEL)
				.flags2(M2.COLLECT, M2.GREEDY, M2.HOSTILE, M2.JEWELS).flags3(M3.INFRAVISIBLE).color(CLR.GRAY).add();

		MonsterType.of(MC.RODENT, PM.WOODCHUCK, "woodchuck").level(3).move(3).ac(0).mr(20).neutral().freq(0).special()
				.genocidable().attack(AT.BITE, AD.PHYS, 1, Dice.D6).weight(30).nutrition(30).sound(MS.SILENT)
				.size(MZ.SMALL).flags1(M1.ANIMAL, M1.HERBIVORE, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE, M1.SWIM, M1.TUNNEL)
				.flags2(M2.HOSTILE, M2.WANDER).flags3(M3.INFRAVISIBLE).color(CLR.BROWN).add();
	}

	private static void initializeSpiders() {

		MonsterType.of(MC.SPIDER, PM.CAVE_SPIDER, "cave spider").level(1).move(12).ac(3).mr(0).neutral().freq(2)
				.smallGroups().genocidable().attack(AT.BITE, AD.PHYS, 1, Dice.D2).weight(50).nutrition(50)
				.sound(MS.SILENT).size(MZ.TINY).resists(MR.POISON).conveys(MR.POISON)
				.flags1(M1.ANIMAL, M1.CARNIVORE, M1.CONCEAL, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE, M1.OVIPAROUS)
				.flags2(M2.HOSTILE).color(CLR.GRAY).add();

		MonsterType.of(MC.SPIDER, PM.CENTIPEDE, "centipede").level(2).move(4).ac(3).mr(0).neutral().freq(1)
				.genocidable().attack(AT.BITE, AD.DRST, 1, Dice.D3).weight(50).nutrition(50).sound(MS.SILENT)
				.size(MZ.TINY).resists(MR.POISON).conveys(MR.POISON)
				.flags1(M1.ANIMAL, M1.CARNIVORE, M1.CONCEAL, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE, M1.OVIPAROUS)
				.flags2(M2.HOSTILE).color(CLR.YELLOW).add();

		MonsterType.of(MC.SPIDER, PM.GIANT_SPIDER, "giant spider").level(5).move(15).ac(4).mr(0).neutral().freq(1)
				.genocidable().attack(AT.BITE, AD.DRST, 2, Dice.D4).weight(100).nutrition(100).sound(MS.SILENT)
				.size(MZ.LARGE).resists(MR.POISON).conveys(MR.POISON)
				.flags1(M1.ANIMAL, M1.CARNIVORE, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE, M1.OVIPAROUS, M1.POIS)
				.flags2(M2.HOSTILE, M2.STRONG).color(CLR.MAGENTA).add();

		MonsterType.of(MC.SPIDER, PM.SCORPION, "scorpion").level(5).move(15).ac(3).mr(0).neutral().freq(2).genocidable()
				.attack(AT.CLAW, AD.PHYS, 1, Dice.D2).attack(AT.CLAW, AD.PHYS, 1, Dice.D2)
				.attack(AT.STNG, AD.DRST, 1, Dice.D4).weight(50).nutrition(100).sound(MS.SILENT).size(MZ.SMALL)
				.resists(MR.POISON).conveys(MR.POISON)
				.flags1(M1.ANIMAL, M1.CARNIVORE, M1.CONCEAL, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE, M1.OVIPAROUS, M1.POIS)
				.flags2(M2.HOSTILE).color(CLR.RED).add();
	}

	private static void initializeTrappers() {

		MonsterType.of(MC.TRAPPER, PM.LURKER_ABOVE, "lurker above").level(10).move(3).ac(3).mr(0).neutral().freq(2)
				.genocidable().attack(AT.ENGL, AD.DGST, 1, Dice.D8).weight(800).nutrition(350)
				.sound(MS.SILENT).size(MZ.HUGE).flags1(M1.ANIMAL, M1.CARNIVORE, M1.FLY, M1.HIDE, M1.NOEYES, M1.NOHANDS,
						M1.NOHEAD, M1.NOLIMBS, M1.OMNIVORE)
				.flags2(M2.HOSTILE, M2.STALK, M2.STRONG).color(CLR.GRAY).add();

		MonsterType.of(MC.TRAPPER, PM.TRAPPER, "trapper").level(12).move(3).ac(3).mr(0).neutral().freq(2).genocidable()
				.attack(AT.ENGL, AD.DGST, 1, Dice.D10).weight(800).nutrition(350).sound(MS.SILENT).size(MZ.HUGE)
				.flags1(M1.ANIMAL, M1.CARNIVORE, M1.HIDE, M1.NOEYES, M1.NOHANDS, M1.NOHEAD, M1.NOLIMBS, M1.OMNIVORE)
				.flags2(M2.HOSTILE, M2.STALK, M2.STRONG).color(CLR.GREEN).add();
	}

	private static void initializeUnicorns() {

		MonsterType.of(MC.UNICORN, PM.PONY, "pony").level(3).move(16).ac(6).mr(0).neutral().freq(2).genocidable()
				.attack(AT.KICK, AD.PHYS, 1, Dice.D6).attack(AT.BITE, AD.PHYS, 1, Dice.D2).weight(1300).nutrition(250)
				.sound(MS.NEIGH).size(MZ.MEDIUM).flags1(M1.ANIMAL, M1.HERBIVORE, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE)
				.flags2(M2.DOMESTIC, M2.STRONG, M2.WANDER).flags3(M3.INFRAVISIBLE).color(CLR.BROWN).add();

		MonsterType.of(MC.UNICORN, PM.WHITE_UNICORN, "white unicorn").level(4).move(24).ac(2).mr(70).lawful(7).freq(2)
				.genocidable().attack(AT.BUTT, AD.PHYS, 1, Dice.D12).attack(AT.KICK, AD.PHYS, 1, Dice.D6).weight(1300)
				.nutrition(300).sound(MS.NEIGH).size(MZ.LARGE).resists(MR.POISON).conveys(MR.POISON)
				.flags1(M1.HERBIVORE, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE).flags2(M2.JEWELS, M2.STRONG, M2.WANDER)
				.flags3(M3.INFRAVISIBLE).color(CLR.WHITE).add();

		MonsterType.of(MC.UNICORN, PM.GRAY_UNICORN, "gray unicorn").level(4).move(24).ac(2).mr(70).neutral().freq(1)
				.genocidable().attack(AT.BUTT, AD.PHYS, 1, Dice.D12).attack(AT.KICK, AD.PHYS, 1, Dice.D6).weight(1300)
				.nutrition(300).sound(MS.NEIGH).size(MZ.LARGE).resists(MR.POISON).conveys(MR.POISON)
				.flags1(M1.HERBIVORE, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE).flags2(M2.JEWELS, M2.STRONG, M2.WANDER)
				.flags3(M3.INFRAVISIBLE).color(CLR.GRAY).add();

		MonsterType.of(MC.UNICORN, PM.BLACK_UNICORN, "black unicorn").level(4).move(24).ac(2).mr(70).chaotic(7).freq(1)
				.genocidable().attack(AT.BUTT, AD.PHYS, 1, Dice.D12).attack(AT.KICK, AD.PHYS, 1, Dice.D6).weight(1300)
				.nutrition(300).sound(MS.NEIGH).size(MZ.LARGE).resists(MR.POISON).conveys(MR.POISON)
				.flags1(M1.HERBIVORE, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE).flags2(M2.JEWELS, M2.STRONG, M2.WANDER)
				.flags3(M3.INFRAVISIBLE).color(CLR.BLACK).add();

		MonsterType.of(MC.UNICORN, PM.HORSE, "horse").level(5).move(20).ac(5).mr(0).neutral().freq(2).genocidable()
				.attack(AT.KICK, AD.PHYS, 1, Dice.D8).attack(AT.BITE, AD.PHYS, 1, Dice.D3).weight(1500).nutrition(300)
				.sound(MS.NEIGH).size(MZ.LARGE).flags1(M1.ANIMAL, M1.HERBIVORE, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE)
				.flags2(M2.DOMESTIC, M2.STRONG, M2.WANDER).flags3(M3.INFRAVISIBLE).color(CLR.BROWN).add();

		MonsterType.of(MC.UNICORN, PM.WARHORSE, "warhorse").level(7).move(24).ac(4).mr(0).neutral().freq(2)
				.genocidable().attack(AT.KICK, AD.PHYS, 1, Dice.D10).attack(AT.BITE, AD.PHYS, 1, Dice.D4).weight(1800)
				.nutrition(350).sound(MS.NEIGH).size(MZ.LARGE)
				.flags1(M1.ANIMAL, M1.HERBIVORE, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE)
				.flags2(M2.DOMESTIC, M2.STRONG, M2.WANDER).flags3(M3.INFRAVISIBLE).color(CLR.BROWN).add();
	}

	private static void initializeVortices() {

		MonsterType.of(MC.VORTEX, PM.FOG_CLOUD, "fog cloud").level(3).move(1).ac(0).mr(0).neutral().freq(2)
				.genocidable().noCorpse().attack(AT.ENGL, AD.PHYS, 1, Dice.D6).weight(0).nutrition(0).sound(MS.SILENT)
				.size(MZ.HUGE)
				.resists(MR.SLEEP, MR.POISON, MR.STONE).flags1(M1.AMORPHOUS, M1.BREATHLESS, M1.FLY, M1.MINDLESS,
						M1.NOEYES, M1.NOHANDS, M1.NOHEAD, M1.NOLIMBS, M1.UNSOLID)
				.flags2(M2.HOSTILE, M2.NEUTER).color(CLR.GRAY).add();

		MonsterType.of(MC.VORTEX, PM.DUST_VORTEX, "dust vortex").level(4).move(20).ac(2).mr(30).neutral().freq(2)
				.genocidable().noCorpse().attack(AT.ENGL, AD.BLND, 2, Dice.D8).weight(0).nutrition(0).sound(MS.SILENT)
				.size(MZ.HUGE).resists(MR.SLEEP, MR.POISON, MR.STONE)
				.flags1(M1.BREATHLESS, M1.FLY, M1.MINDLESS, M1.NOEYES, M1.NOHANDS, M1.NOHEAD, M1.NOLIMBS)
				.flags2(M2.HOSTILE, M2.NEUTER).color(CLR.BROWN).add();

		MonsterType.of(MC.VORTEX, PM.ICE_VORTEX, "ice vortex").level(5).move(20).ac(2).mr(30).neutral().freq(1)
				.notInHell().genocidable().noCorpse().attack(AT.ENGL, AD.COLD, 1, Dice.D6).weight(0).nutrition(0)
				.sound(MS.SILENT).size(MZ.HUGE).resists(MR.COLD, MR.SLEEP, MR.POISON, MR.STONE)
				.flags1(M1.BREATHLESS, M1.FLY, M1.MINDLESS, M1.NOEYES, M1.NOHANDS, M1.NOHEAD, M1.NOLIMBS)
				.flags2(M2.HOSTILE, M2.NEUTER).flags3(M3.INFRAVISIBLE).color(CLR.CYAN).add();

		MonsterType.of(MC.VORTEX, PM.ENERGY_VORTEX, "energy vortex").level(6).move(20).ac(2).mr(30).neutral().freq(1)
				.genocidable().noCorpse().attack(AT.ENGL, AD.ELEC, 1, Dice.D6).attack(AT.ENGL, AD.DREN, 4, Dice.D6)
				.weight(0).nutrition(0).sound(MS.SILENT).size(MZ.HUGE)
				.resists(MR.SLEEP, MR.DISINT, MR.ELEC, MR.POISON, MR.STONE)
				.flags1(M1.BREATHLESS, M1.FLY, M1.MINDLESS, M1.NOEYES, M1.NOHANDS, M1.NOHEAD, M1.NOLIMBS, M1.UNSOLID)
				.flags2(M2.HOSTILE, M2.NEUTER).color(CLR.BRIGHT_BLUE).add();

		MonsterType.of(MC.VORTEX, PM.STEAM_VORTEX, "steam vortex").level(7).move(22).ac(2).mr(30).neutral().freq(2)
				.onlyInHell().genocidable().noCorpse().attack(AT.ENGL, AD.FIRE, 1, Dice.D8).weight(0).nutrition(0)
				.sound(MS.SILENT).size(MZ.HUGE).resists(MR.FIRE, MR.SLEEP, MR.POISON, MR.STONE)
				.flags1(M1.BREATHLESS, M1.FLY, M1.MINDLESS, M1.NOEYES, M1.NOHANDS, M1.NOHEAD, M1.NOLIMBS, M1.UNSOLID)
				.flags2(M2.HOSTILE, M2.NEUTER).flags3(M3.INFRAVISIBLE).color(CLR.BLUE).add();

		MonsterType.of(MC.VORTEX, PM.FIRE_VORTEX, "fire vortex").level(8).move(22).ac(2).mr(30).neutral().freq(1)
				.onlyInHell().genocidable().noCorpse().attack(AT.ENGL, AD.FIRE, 1, Dice.D10).weight(0).nutrition(0)
				.sound(MS.SILENT).size(MZ.HUGE).resists(MR.FIRE, MR.SLEEP, MR.POISON, MR.STONE)
				.flags1(M1.BREATHLESS, M1.FLY, M1.MINDLESS, M1.NOEYES, M1.NOHANDS, M1.NOHEAD, M1.NOLIMBS, M1.UNSOLID)
				.flags2(M2.HOSTILE, M2.NEUTER).flags3(M3.INFRAVISIBLE).color(CLR.YELLOW).add();
	}

	private static void initializeWorms() {

		MonsterType.of(MC.WORM, PM.BABY_LONG_WORM, "baby long worm").level(5).move(3).ac(5).mr(0).neutral().freq(0)
				.genocidable().attack(AT.BITE, AD.PHYS, 1, Dice.D4).weight(600).nutrition(250).sound(MS.SILENT)
				.size(MZ.LARGE)
				.flags1(M1.ANIMAL, M1.CARNIVORE, M1.NOHANDS, M1.NOLIMBS, M1.NOTAKE, M1.OMNIVORE, M1.SLITHY)
				.flags2(M2.HOSTILE).color(CLR.BROWN).add();

		MonsterType.of(MC.WORM, PM.BABY_PURPLE_WORM, "baby purple worm").level(8).move(3).ac(5).mr(0).neutral().freq(0)
				.genocidable().attack(AT.BITE, AD.PHYS, 1, Dice.D6).weight(600).nutrition(250).sound(MS.SILENT)
				.size(MZ.LARGE).flags1(M1.ANIMAL, M1.CARNIVORE, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE, M1.SLITHY)
				.flags2(M2.HOSTILE).color(CLR.MAGENTA).add();

		MonsterType.of(MC.WORM, PM.LONG_WORM, "long worm").level(9).move(3).ac(5).mr(10).neutral().freq(2).genocidable()
				.attack(AT.BITE, AD.PHYS, 2, Dice.D4).weight(1500).nutrition(500)
				.sound(MS.SILENT).size(MZ.GIGANTIC).flags1(M1.ANIMAL, M1.CARNIVORE, M1.NOHANDS, M1.NOLIMBS, M1.NOTAKE,
						M1.OMNIVORE, M1.OVIPAROUS, M1.SLITHY)
				.flags2(M2.HOSTILE, M2.NASTY, M2.STRONG).color(CLR.BROWN).add();

		MonsterType.of(MC.WORM, PM.PURPLE_WORM, "purple worm").level(15).move(9).ac(6).mr(20).neutral().freq(2)
				.genocidable().attack(AT.BITE, AD.PHYS, 2, Dice.D8).attack(AT.ENGL, AD.DGST, 1, Dice.D10).weight(2700)
				.nutrition(700).sound(MS.SILENT).size(MZ.GIGANTIC)
				.flags1(M1.ANIMAL, M1.CARNIVORE, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE, M1.OVIPAROUS, M1.SLITHY)
				.flags2(M2.HOSTILE, M2.NASTY, M2.STRONG).color(CLR.MAGENTA).add();
	}

	private static void initializeXan() {

		MonsterType.of(MC.XAN, PM.GRID_BUG, "grid bug").level(0).move(12).ac(9).mr(0).neutral().freq(3).smallGroups()
				.genocidable().noCorpse().attack(AT.BITE, AD.ELEC, 1, Dice.D1).weight(15).nutrition(10).sound(MS.BUZZ)
				.size(MZ.TINY).resists(MR.ELEC, MR.POISON).flags1(M1.ANIMAL).flags2(M2.HOSTILE).flags3(M3.INFRAVISIBLE)
				.color(CLR.MAGENTA).add();

		MonsterType.of(MC.XAN, PM.XAN, "xan").level(7).move(18).ac(-4).mr(0).neutral().freq(3).genocidable()
				.attack(AT.STNG, AD.LEGS, 1, Dice.D4).weight(300).nutrition(300).sound(MS.BUZZ).size(MZ.TINY)
				.resists(MR.POISON).conveys(MR.POISON).flags1(M1.ANIMAL, M1.FLY, M1.NOHANDS, M1.NOLIMBS, M1.POIS)
				.flags2(M2.HOSTILE).flags3(M3.INFRAVISIBLE).color(CLR.RED).add();
	}

	private static void initializeLights() {

		MonsterType.of(MC.LIGHT, PM.YELLOW_LIGHT, "yellow light").level(3).move(15).ac(0).mr(0).neutral().freq(4)
				.genocidable().noCorpse().attack(AT.EXPL, AD.BLND, 10, Dice.D20).weight(0).nutrition(0).sound(MS.SILENT)
				.size(MZ.SMALL).resists(MR.FIRE, MR.COLD, MR.SLEEP, MR.DISINT, MR.ELEC, MR.POISON, MR.ACID, MR.STONE)
				.flags1(M1.AMORPHOUS, M1.BREATHLESS, M1.FLY, M1.MINDLESS, M1.NOEYES, M1.NOHANDS, M1.NOHEAD, M1.NOLIMBS,
						M1.NOTAKE, M1.UNSOLID)
				.flags2(M2.HOSTILE, M2.NEUTER).flags3(M3.INFRAVISIBLE).color(CLR.YELLOW).add();

		MonsterType.of(MC.LIGHT, PM.BLACK_LIGHT, "black light").level(5).move(15).ac(0).mr(0).neutral().freq(2)
				.genocidable().noCorpse().attack(AT.EXPL, AD.HALU, 10, Dice.D12).weight(0).nutrition(0).sound(MS.SILENT)
				.size(MZ.SMALL).resists(MR.FIRE, MR.COLD, MR.SLEEP, MR.DISINT, MR.ELEC, MR.POISON, MR.ACID, MR.STONE)
				.flags1(M1.AMORPHOUS, M1.BREATHLESS, M1.FLY, M1.MINDLESS, M1.NOEYES, M1.NOHANDS, M1.NOHEAD, M1.NOLIMBS,
						M1.NOTAKE, M1.SEE_INVIS, M1.UNSOLID)
				.flags2(M2.HOSTILE, M2.NEUTER).color(CLR.BLACK).add();
	}

	private static void initializeZruties() {

		MonsterType.of(MC.ZRUTY, PM.ZRUTY, "zruty").level(9).move(8).ac(3).mr(0).neutral().freq(2).genocidable()
				.attack(AT.CLAW, AD.PHYS, 3, Dice.D4).attack(AT.CLAW, AD.PHYS, 3, Dice.D4)
				.attack(AT.BITE, AD.PHYS, 3, Dice.D6).weight(1200).nutrition(600).sound(MS.SILENT).size(MZ.LARGE)
				.flags1(M1.ANIMAL, M1.CARNIVORE, M1.HUMANOID, M1.OMNIVORE).flags2(M2.HOSTILE, M2.STRONG)
				.flags3(M3.INFRAVISIBLE).color(CLR.BROWN).add();
	}

	private static void initializeAngel() {

		MonsterType.of(MC.ANGEL, PM.COUATL, "couatl").level(8).move(10).ac(5).mr(30).lawful(7).freq(1).notInHell()
				.smallGroups().noCorpse().attack(AT.BITE, AD.DRST, 2, Dice.D4).attack(AT.BITE, AD.PHYS, 1, Dice.D3)
				.attack(AT.HUGS, AD.WRAP, 2, Dice.D4).weight(900).nutrition(400).sound(MS.HISS).size(MZ.LARGE)
				.resists(MR.POISON).flags1(M1.FLY, M1.NOHANDS, M1.NOLIMBS, M1.POIS, M1.SLITHY)
				.flags2(M2.MINION, M2.NASTY, M2.STALK, M2.STRONG).flags3(M3.INFRAVISION, M3.INFRAVISIBLE)
				.color(CLR.GREEN).add();

		MonsterType.of(MC.ANGEL, PM.ALEAX, "Aleax").level(10).move(8).ac(0).mr(30).lawful(7).freq(1).notInHell()
				.noCorpse().attack(AT.WEAP, AD.PHYS, 1, Dice.D6).attack(AT.WEAP, AD.PHYS, 1, Dice.D6)
				.attack(AT.KICK, AD.PHYS, 1, Dice.D4).weight(1450).nutrition(400).sound(MS.IMITATE).size(MZ.HUMAN)
				.resists(MR.COLD, MR.SLEEP, MR.ELEC, MR.POISON).flags1(M1.HUMANOID, M1.SEE_INVIS)
				.flags2(M2.COLLECT, M2.MINION, M2.NASTY, M2.STALK).flags3(M3.INFRAVISION, M3.INFRAVISIBLE)
				.color(CLR.YELLOW).add();

		MonsterType.of(MC.ANGEL, PM.ANGEL, "Angel").level(14).move(10).ac(-4).mr(55).lawful(12).freq(1).notInHell()
				.noCorpse().attack(AT.WEAP, AD.PHYS, 1, Dice.D6).attack(AT.WEAP, AD.PHYS, 1, Dice.D6)
				.attack(AT.CLAW, AD.PHYS, 1, Dice.D4).attack(AT.MAGC, AD.MAGM, 2, Dice.D6).weight(1450).nutrition(400)
				.sound(MS.CUSS).size(MZ.HUMAN).resists(MR.COLD, MR.SLEEP, MR.ELEC, MR.POISON)
				.flags1(M1.FLY, M1.HUMANOID, M1.SEE_INVIS)
				.flags2(M2.COLLECT, M2.MINION, M2.NASTY, M2.NOPOLY, M2.STALK, M2.STRONG)
				.flags3(M3.INFRAVISION, M3.INFRAVISIBLE).color(CLR.WHITE).add();

		MonsterType.of(MC.ANGEL, PM.KI_RIN, "ki-rin").level(16).move(18).ac(-5).mr(90).lawful(15).freq(1).notInHell()
				.noCorpse().attack(AT.KICK, AD.PHYS, 2, Dice.D4).attack(AT.KICK, AD.PHYS, 2, Dice.D4)
				.attack(AT.BUTT, AD.PHYS, 3, Dice.D6).attack(AT.MAGC, AD.SPEL, 2, Dice.D6).weight(1450).nutrition(400)
				.sound(MS.NEIGH).size(MZ.LARGE).flags1(M1.ANIMAL, M1.FLY, M1.NOHANDS, M1.NOLIMBS, M1.SEE_INVIS)
				.flags2(M2.LORD, M2.MINION, M2.NASTY, M2.NOPOLY, M2.STALK, M2.STRONG)
				.flags3(M3.INFRAVISION, M3.INFRAVISIBLE).color(CLR.GOLD).add();

		MonsterType.of(MC.ANGEL, PM.ARCHON, "Archon").level(19).move(16).ac(-6).mr(80).lawful(15).freq(1).notInHell()
				.noCorpse().attack(AT.WEAP, AD.PHYS, 2, Dice.D4).attack(AT.WEAP, AD.PHYS, 2, Dice.D4)
				.attack(AT.GAZE, AD.BLND, 2, Dice.D6).attack(AT.CLAW, AD.PHYS, 1, Dice.D8)
				.attack(AT.MAGC, AD.SPEL, 4, Dice.D6).weight(1450).nutrition(400).sound(MS.CUSS).size(MZ.LARGE)
				.resists(MR.FIRE, MR.COLD, MR.SLEEP, MR.ELEC, MR.POISON)
				.flags1(M1.FLY, M1.HUMANOID, M1.REGEN, M1.SEE_INVIS)
				.flags2(M2.COLLECT, M2.LORD, M2.MAGIC, M2.MINION, M2.NASTY, M2.NOPOLY, M2.STALK, M2.STRONG)
				.flags3(M3.INFRAVISION, M3.INFRAVISIBLE).color(CLR.MAGENTA).add();
	}

	private static void initializeBats() {

		MonsterType.of(MC.BAT, PM.BAT, "bat").level(0).move(22).ac(8).mr(0).neutral().freq(1).smallGroups()
				.genocidable().attack(AT.BITE, AD.PHYS, 1, Dice.D4).weight(20).nutrition(20).sound(MS.SQEEK)
				.size(MZ.TINY).flags1(M1.ANIMAL, M1.CARNIVORE, M1.FLY, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE)
				.flags2(M2.WANDER).flags3(M3.INFRAVISIBLE).color(CLR.BROWN).add();

		MonsterType.of(MC.BAT, PM.GIANT_BAT, "giant bat").level(2).move(22).ac(7).mr(0).neutral().freq(2).genocidable()
				.attack(AT.BITE, AD.PHYS, 1, Dice.D6).weight(30).nutrition(30).sound(MS.SQEEK).size(MZ.SMALL)
				.flags1(M1.ANIMAL, M1.CARNIVORE, M1.FLY, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE)
				.flags2(M2.HOSTILE, M2.WANDER).flags3(M3.INFRAVISIBLE).color(CLR.RED).add();

		MonsterType.of(MC.BAT, PM.RAVEN, "raven").level(4).move(20).ac(6).mr(0).neutral().freq(2).genocidable()
				.attack(AT.BITE, AD.PHYS, 1, Dice.D6).attack(AT.CLAW, AD.BLND, 1, Dice.D6).weight(40).nutrition(20)
				.sound(MS.SQAWK).size(MZ.SMALL)
				.flags1(M1.ANIMAL, M1.CARNIVORE, M1.FLY, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE)
				.flags2(M2.HOSTILE, M2.WANDER).flags3(M3.INFRAVISIBLE).color(CLR.BLACK).add();

		MonsterType.of(MC.BAT, PM.VAMPIRE_BAT, "vampire bat").level(5).move(20).ac(6).mr(0).neutral().freq(2)
				.genocidable().attack(AT.BITE, AD.PHYS, 1, Dice.D6).attack(AT.BITE, AD.DRST, 0, Dice.D0).weight(30)
				.nutrition(20).sound(MS.SQEEK).size(MZ.SMALL)
				.resists(MR.SLEEP, MR.POISON).flags1(M1.ANIMAL, M1.CARNIVORE, M1.FLY, M1.HERBIVORE, M1.NOHANDS,
						M1.NOLIMBS, M1.OMNIVORE, M1.POIS, M1.REGEN)
				.flags2(M2.HOSTILE).flags3(M3.INFRAVISIBLE).color(CLR.BLACK).add();
	}

	private static void initializeCentaurs() {

		MonsterType.of(MC.CENTAUR, PM.PLAINS_CENTAUR, "plains centaur").level(4).move(18).ac(4).mr(0).neutral().freq(1)
				.genocidable().attack(AT.WEAP, AD.PHYS, 1, Dice.D6).attack(AT.KICK, AD.PHYS, 1, Dice.D6).weight(2500)
				.nutrition(500).sound(MS.HUMANOID).size(MZ.LARGE)
				.flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE).flags2(M2.COLLECT, M2.GREEDY, M2.STRONG)
				.flags3(M3.INFRAVISIBLE).color(CLR.BROWN).add();

		MonsterType.of(MC.CENTAUR, PM.FOREST_CENTAUR, "forest centaur").level(5).move(18).ac(3).mr(10).chaotic(1)
				.freq(1).genocidable().attack(AT.WEAP, AD.PHYS, 1, Dice.D8).attack(AT.KICK, AD.PHYS, 1, Dice.D6)
				.weight(2550).nutrition(600).sound(MS.HUMANOID).size(MZ.LARGE)
				.flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE).flags2(M2.COLLECT, M2.GREEDY, M2.STRONG)
				.flags3(M3.INFRAVISIBLE).color(CLR.GREEN).add();

		MonsterType.of(MC.CENTAUR, PM.MOUNTAIN_CENTAUR, "mountain centaur").level(6).move(20).ac(2).mr(10).chaotic(3)
				.freq(1).genocidable().attack(AT.WEAP, AD.PHYS, 1, Dice.D10).attack(AT.KICK, AD.PHYS, 1, Dice.D6)
				.attack(AT.KICK, AD.PHYS, 1, Dice.D6).weight(2550).nutrition(500).sound(MS.HUMANOID).size(MZ.LARGE)
				.flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE).flags2(M2.COLLECT, M2.GREEDY, M2.STRONG)
				.flags3(M3.INFRAVISIBLE).color(CLR.CYAN).add();
	}

	private static void initializeDragons() {

		MonsterType.of(MC.DRAGON, PM.BABY_GRAY_DRAGON, "baby gray dragon").level(12).move(9).ac(2).mr(10).neutral()
				.freq(0).genocidable().attack(AT.BITE, AD.PHYS, 2, Dice.D6).weight(1500).nutrition(500).sound(MS.ROAR)
				.size(MZ.HUGE).flags1(M1.CARNIVORE, M1.FLY, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE, M1.THICK_HIDE)
				.flags2(M2.GREEDY, M2.HOSTILE, M2.JEWELS, M2.STRONG).color(CLR.GRAY).add();

		MonsterType.of(MC.DRAGON, PM.BABY_SILVER_DRAGON, "baby silver dragon").level(12).move(9).ac(2).mr(10).neutral()
				.freq(0).genocidable().attack(AT.BITE, AD.PHYS, 2, Dice.D6).weight(1500).nutrition(500).sound(MS.ROAR)
				.size(MZ.HUGE).flags1(M1.CARNIVORE, M1.FLY, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE, M1.THICK_HIDE)
				.flags2(M2.GREEDY, M2.HOSTILE, M2.JEWELS, M2.STRONG).color(CLR.BRIGHT_CYAN).add();

		MonsterType.of(MC.DRAGON, PM.BABY_RED_DRAGON, "baby red dragon").level(12).move(9).ac(2).mr(10).neutral()
				.freq(0).genocidable().attack(AT.BITE, AD.PHYS, 2, Dice.D6).weight(1500).nutrition(500).sound(MS.ROAR)
				.size(MZ.HUGE).resists(MR.FIRE)
				.flags1(M1.CARNIVORE, M1.FLY, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE, M1.THICK_HIDE)
				.flags2(M2.GREEDY, M2.HOSTILE, M2.JEWELS, M2.STRONG).flags3(M3.INFRAVISIBLE).color(CLR.RED).add();

		MonsterType.of(MC.DRAGON, PM.BABY_WHITE_DRAGON, "baby white dragon").level(12).move(9).ac(2).mr(10).neutral()
				.freq(0).genocidable().attack(AT.BITE, AD.PHYS, 2, Dice.D6).weight(1500).nutrition(500).sound(MS.ROAR)
				.size(MZ.HUGE).resists(MR.COLD)
				.flags1(M1.CARNIVORE, M1.FLY, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE, M1.THICK_HIDE)
				.flags2(M2.GREEDY, M2.HOSTILE, M2.JEWELS, M2.STRONG).color(CLR.WHITE).add();

		MonsterType.of(MC.DRAGON, PM.BABY_ORANGE_DRAGON, "baby orange dragon").level(12).move(9).ac(2).mr(10).neutral()
				.freq(0).genocidable().attack(AT.BITE, AD.PHYS, 2, Dice.D6).weight(1500).nutrition(500).sound(MS.ROAR)
				.size(MZ.HUGE).resists(MR.SLEEP)
				.flags1(M1.CARNIVORE, M1.FLY, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE, M1.THICK_HIDE)
				.flags2(M2.GREEDY, M2.HOSTILE, M2.JEWELS, M2.STRONG).color(CLR.ORANGE).add();

		MonsterType.of(MC.DRAGON, PM.BABY_BLACK_DRAGON, "baby black dragon").level(12).move(9).ac(2).mr(10).neutral()
				.freq(0).genocidable().attack(AT.BITE, AD.PHYS, 2, Dice.D6).weight(1500).nutrition(500).sound(MS.ROAR)
				.size(MZ.HUGE).resists(MR.DISINT)
				.flags1(M1.CARNIVORE, M1.FLY, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE, M1.THICK_HIDE)
				.flags2(M2.GREEDY, M2.HOSTILE, M2.JEWELS, M2.STRONG).color(CLR.BLACK).add();

		MonsterType.of(MC.DRAGON, PM.BABY_BLUE_DRAGON, "baby blue dragon").level(12).move(9).ac(2).mr(10).neutral()
				.freq(0).genocidable().attack(AT.BITE, AD.PHYS, 2, Dice.D6).weight(1500).nutrition(500).sound(MS.ROAR)
				.size(MZ.HUGE).resists(MR.ELEC)
				.flags1(M1.CARNIVORE, M1.FLY, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE, M1.THICK_HIDE)
				.flags2(M2.GREEDY, M2.HOSTILE, M2.JEWELS, M2.STRONG).color(CLR.BLUE).add();

		MonsterType.of(MC.DRAGON, PM.BABY_GREEN_DRAGON, "baby green dragon").level(12).move(9).ac(2).mr(10).neutral()
				.freq(0).genocidable().attack(AT.BITE, AD.PHYS, 2, Dice.D6).weight(1500).nutrition(500).sound(MS.ROAR)
				.size(MZ.HUGE).resists(MR.POISON)
				.flags1(M1.CARNIVORE, M1.FLY, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE, M1.POIS, M1.THICK_HIDE)
				.flags2(M2.GREEDY, M2.HOSTILE, M2.JEWELS, M2.STRONG).color(CLR.GREEN).add();

		MonsterType.of(MC.DRAGON, PM.BABY_YELLOW_DRAGON, "baby yellow dragon").level(12).move(9).ac(2).mr(10).neutral()
				.freq(0).genocidable().attack(AT.BITE, AD.PHYS, 2, Dice.D6).weight(1500).nutrition(500).sound(MS.ROAR)
				.size(MZ.HUGE).resists(MR.ACID, MR.STONE)
				.flags1(M1.ACID, M1.CARNIVORE, M1.FLY, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE, M1.THICK_HIDE)
				.flags2(M2.GREEDY, M2.HOSTILE, M2.JEWELS, M2.STRONG).color(CLR.YELLOW).add();

		MonsterType.of(MC.DRAGON, PM.GRAY_DRAGON, "gray dragon").level(15).move(9).ac(-1).mr(20).lawful(4).freq(1)
				.genocidable().attack(AT.BREA, AD.MAGM, 4, Dice.D6).attack(AT.BITE, AD.PHYS, 3, Dice.D8)
				.attack(AT.CLAW, AD.PHYS, 1, Dice.D4).attack(AT.CLAW, AD.PHYS, 1, Dice.D4).weight(4500).nutrition(1500)
				.sound(MS.ROAR).size(MZ.GIGANTIC)
				.flags1(M1.CARNIVORE, M1.FLY, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE, M1.OVIPAROUS, M1.SEE_INVIS,
						M1.THICK_HIDE)
				.flags2(M2.GREEDY, M2.HOSTILE, M2.JEWELS, M2.MAGIC, M2.NASTY, M2.STRONG).color(CLR.GRAY).add();

		MonsterType.of(MC.DRAGON, PM.SILVER_DRAGON, "silver dragon").level(15).move(9).ac(-1).mr(20).lawful(4).freq(1)
				.genocidable().attack(AT.BREA, AD.COLD, 4, Dice.D6).attack(AT.BITE, AD.PHYS, 3, Dice.D8)
				.attack(AT.CLAW, AD.PHYS, 1, Dice.D4).attack(AT.CLAW, AD.PHYS, 1, Dice.D4).weight(4500).nutrition(1500)
				.sound(MS.ROAR).size(MZ.GIGANTIC).resists(MR.COLD)
				.flags1(M1.CARNIVORE, M1.FLY, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE, M1.OVIPAROUS, M1.SEE_INVIS,
						M1.THICK_HIDE)
				.flags2(M2.GREEDY, M2.HOSTILE, M2.JEWELS, M2.MAGIC, M2.NASTY, M2.STRONG).color(CLR.BRIGHT_CYAN).add();

		MonsterType.of(MC.DRAGON, PM.RED_DRAGON, "red dragon").level(15).move(9).ac(-1).mr(20).chaotic(4).freq(1)
				.genocidable().attack(AT.BREA, AD.FIRE, 6, Dice.D6).attack(AT.BITE, AD.PHYS, 3, Dice.D8)
				.attack(AT.CLAW, AD.PHYS, 1, Dice.D4).attack(AT.CLAW, AD.PHYS, 1, Dice.D4).weight(4500).nutrition(1500)
				.sound(MS.ROAR).size(MZ.GIGANTIC).resists(MR.FIRE).conveys(MR.FIRE)
				.flags1(M1.CARNIVORE, M1.FLY, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE, M1.OVIPAROUS, M1.SEE_INVIS,
						M1.THICK_HIDE)
				.flags2(M2.GREEDY, M2.HOSTILE, M2.JEWELS, M2.MAGIC, M2.NASTY, M2.STRONG).flags3(M3.INFRAVISIBLE)
				.color(CLR.RED).add();

		MonsterType.of(MC.DRAGON, PM.WHITE_DRAGON, "white dragon").level(15).move(9).ac(-1).mr(20).chaotic(5).freq(1)
				.genocidable().attack(AT.BREA, AD.COLD, 4, Dice.D6).attack(AT.BITE, AD.PHYS, 3, Dice.D8)
				.attack(AT.CLAW, AD.PHYS, 1, Dice.D4).attack(AT.CLAW, AD.PHYS, 1, Dice.D4).weight(4500).nutrition(1500)
				.sound(MS.ROAR).size(MZ.GIGANTIC).resists(MR.COLD).conveys(MR.COLD)
				.flags1(M1.CARNIVORE, M1.FLY, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE, M1.OVIPAROUS, M1.SEE_INVIS,
						M1.THICK_HIDE)
				.flags2(M2.GREEDY, M2.HOSTILE, M2.JEWELS, M2.MAGIC, M2.NASTY, M2.STRONG).color(CLR.WHITE).add();

		MonsterType.of(MC.DRAGON, PM.ORANGE_DRAGON, "orange dragon").level(15).move(9).ac(-1).mr(20).lawful(5).freq(1)
				.genocidable().attack(AT.BREA, AD.SLEE, 4, Dice.D25).attack(AT.BITE, AD.PHYS, 3, Dice.D8)
				.attack(AT.CLAW, AD.PHYS, 1, Dice.D4).attack(AT.CLAW, AD.PHYS, 1, Dice.D4).weight(4500).nutrition(1500)
				.sound(MS.ROAR).size(MZ.GIGANTIC).resists(MR.SLEEP).conveys(MR.SLEEP)
				.flags1(M1.CARNIVORE, M1.FLY, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE, M1.OVIPAROUS, M1.SEE_INVIS,
						M1.THICK_HIDE)
				.flags2(M2.GREEDY, M2.HOSTILE, M2.JEWELS, M2.MAGIC, M2.NASTY, M2.STRONG).color(CLR.ORANGE).add();

		MonsterType.of(MC.DRAGON, PM.BLACK_DRAGON, "black dragon").level(15).move(9).ac(-1).mr(20).chaotic(6).freq(1)
				.genocidable().attack(AT.BREA, AD.DISN, 4, Dice.D10).attack(AT.BITE, AD.PHYS, 3, Dice.D8)
				.attack(AT.CLAW, AD.PHYS, 1, Dice.D4).attack(AT.CLAW, AD.PHYS, 1, Dice.D4).weight(4500).nutrition(1500)
				.sound(MS.ROAR).size(MZ.GIGANTIC).resists(MR.DISINT).conveys(MR.DISINT)
				.flags1(M1.CARNIVORE, M1.FLY, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE, M1.OVIPAROUS, M1.SEE_INVIS,
						M1.THICK_HIDE)
				.flags2(M2.GREEDY, M2.HOSTILE, M2.JEWELS, M2.MAGIC, M2.NASTY, M2.STRONG).color(CLR.BLACK).add();

		MonsterType.of(MC.DRAGON, PM.BLUE_DRAGON, "blue dragon").level(15).move(9).ac(-1).mr(20).chaotic(7).freq(1)
				.genocidable().attack(AT.BREA, AD.ELEC, 4, Dice.D6).attack(AT.BITE, AD.PHYS, 3, Dice.D8)
				.attack(AT.CLAW, AD.PHYS, 1, Dice.D4).attack(AT.CLAW, AD.PHYS, 1, Dice.D4).weight(4500).nutrition(1500)
				.sound(MS.ROAR).size(MZ.GIGANTIC).resists(MR.ELEC).conveys(MR.ELEC)
				.flags1(M1.CARNIVORE, M1.FLY, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE, M1.OVIPAROUS, M1.SEE_INVIS,
						M1.THICK_HIDE)
				.flags2(M2.GREEDY, M2.HOSTILE, M2.JEWELS, M2.MAGIC, M2.NASTY, M2.STRONG).color(CLR.BLUE).add();

		MonsterType.of(MC.DRAGON, PM.GREEN_DRAGON, "green dragon").level(15).move(9).ac(-1).mr(20).lawful(6).freq(1)
				.genocidable().attack(AT.BREA, AD.DRST, 4, Dice.D6).attack(AT.BITE, AD.PHYS, 3, Dice.D8)
				.attack(AT.CLAW, AD.PHYS, 1, Dice.D4).attack(AT.CLAW, AD.PHYS, 1, Dice.D4).weight(4500).nutrition(1500)
				.sound(MS.ROAR).size(MZ.GIGANTIC).resists(MR.POISON).conveys(MR.POISON)
				.flags1(M1.CARNIVORE, M1.FLY, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE, M1.OVIPAROUS, M1.POIS, M1.SEE_INVIS,
						M1.THICK_HIDE)
				.flags2(M2.GREEDY, M2.HOSTILE, M2.JEWELS, M2.MAGIC, M2.NASTY, M2.STRONG).color(CLR.GREEN).add();

		MonsterType.of(MC.DRAGON, PM.YELLOW_DRAGON, "yellow dragon").level(15).move(9).ac(-1).mr(20).lawful(7).freq(1)
				.genocidable().attack(AT.BREA, AD.ACID, 4, Dice.D6).attack(AT.BITE, AD.PHYS, 3, Dice.D8)
				.attack(AT.CLAW, AD.PHYS, 1, Dice.D4).attack(AT.CLAW, AD.PHYS, 1, Dice.D4).weight(4500).nutrition(1500)
				.sound(MS.ROAR).size(MZ.GIGANTIC).resists(MR.ACID, MR.STONE).conveys(MR.STONE)
				.flags1(M1.ACID, M1.CARNIVORE, M1.FLY, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE, M1.OVIPAROUS, M1.SEE_INVIS,
						M1.THICK_HIDE)
				.flags2(M2.GREEDY, M2.HOSTILE, M2.JEWELS, M2.MAGIC, M2.NASTY, M2.STRONG).color(CLR.YELLOW).add();
	}

	private static void initializeElementals() {

		MonsterType.of(MC.ELEMENTAL, PM.STALKER, "stalker").level(8).move(12).ac(3).mr(0).neutral().freq(3)
				.genocidable().attack(AT.CLAW, AD.PHYS, 4, Dice.D4).weight(900).nutrition(400).sound(MS.SILENT)
				.size(MZ.LARGE).flags1(M1.ANIMAL, M1.FLY, M1.SEE_INVIS)
				.flags2(M2.HOSTILE, M2.STALK, M2.STRONG, M2.WANDER).flags3(M3.INFRAVISION).color(CLR.WHITE).add();

		MonsterType.of(MC.ELEMENTAL, PM.AIR_ELEMENTAL, "air elemental").level(8).move(36).ac(2).mr(30).neutral().freq(1)
				.noCorpse().attack(AT.ENGL, AD.PHYS, 1, Dice.D10).weight(0).nutrition(0).sound(MS.SILENT).size(MZ.HUGE)
				.resists(MR.POISON, MR.STONE)
				.flags1(M1.BREATHLESS, M1.FLY, M1.MINDLESS, M1.NOEYES, M1.NOHANDS, M1.NOHEAD, M1.NOLIMBS, M1.UNSOLID)
				.flags2(M2.NEUTER, M2.STRONG).color(CLR.CYAN).add();

		MonsterType.of(MC.ELEMENTAL, PM.FIRE_ELEMENTAL, "fire elemental").level(8).move(12).ac(2).mr(30).neutral()
				.freq(1).noCorpse().attack(AT.CLAW, AD.FIRE, 3, Dice.D6).weight(0).nutrition(0).sound(MS.SILENT)
				.size(MZ.HUGE)
				.resists(MR.FIRE, MR.POISON, MR.STONE).flags1(M1.BREATHLESS, M1.FLY, M1.MINDLESS, M1.NOEYES, M1.NOHANDS,
						M1.NOHEAD, M1.NOLIMBS, M1.NOTAKE, M1.UNSOLID)
				.flags2(M2.NEUTER, M2.STRONG).flags3(M3.INFRAVISIBLE).color(CLR.YELLOW).add();

		MonsterType.of(MC.ELEMENTAL, PM.EARTH_ELEMENTAL, "earth elemental").level(8).move(6).ac(2).mr(30).neutral()
				.freq(1).noCorpse().attack(AT.CLAW, AD.PHYS, 4, Dice.D6).weight(2500).nutrition(0).sound(MS.SILENT)
				.size(MZ.HUGE)
				.resists(MR.FIRE, MR.COLD, MR.POISON, MR.STONE).flags1(M1.BREATHLESS, M1.MINDLESS, M1.NOEYES,
						M1.NOHANDS, M1.NOHEAD, M1.NOLIMBS, M1.THICK_HIDE, M1.WALLWALK)
				.flags2(M2.NEUTER, M2.STRONG).color(CLR.BROWN).add();

		MonsterType.of(MC.ELEMENTAL, PM.WATER_ELEMENTAL, "water elemental").level(8).move(6).ac(2).mr(30).neutral()
				.freq(1).noCorpse().attack(AT.CLAW, AD.PHYS, 5, Dice.D6).weight(2500).nutrition(0).sound(MS.SILENT)
				.size(MZ.HUGE).resists(MR.POISON, MR.STONE).flags1(M1.AMPHIBIOUS, M1.BREATHLESS, M1.MINDLESS, M1.NOEYES,
						M1.NOHANDS, M1.NOHEAD, M1.NOLIMBS, M1.SWIM, M1.UNSOLID)
				.flags2(M2.NEUTER, M2.STRONG).color(CLR.BLUE).add();
	}

	private static void initializeFungi() {

		MonsterType.of(MC.FUNGUS, PM.LICHEN, "lichen").level(0).move(1).ac(9).mr(0).neutral().freq(4).genocidable()
				.attack(AT.TUCH, AD.STCK, 0, Dice.D0).weight(20).nutrition(200).sound(MS.SILENT).size(MZ.SMALL)
				.flags1(M1.BREATHLESS, M1.MINDLESS, M1.NOEYES, M1.NOHANDS, M1.NOHEAD, M1.NOLIMBS, M1.NOTAKE)
				.flags2(M2.HOSTILE, M2.NEUTER).color(CLR.BRIGHT_GREEN).add();

		MonsterType.of(MC.FUNGUS, PM.BROWN_MOLD, "brown mold").level(1).move(0).ac(9).mr(0).neutral().freq(1)
				.genocidable().weight(50).nutrition(30).sound(MS.SILENT).size(MZ.SMALL).resists(MR.COLD, MR.POISON)
				.conveys(MR.COLD, MR.POISON)
				.flags1(M1.BREATHLESS, M1.MINDLESS, M1.NOEYES, M1.NOHANDS, M1.NOHEAD, M1.NOLIMBS, M1.NOTAKE)
				.flags2(M2.HOSTILE, M2.NEUTER).color(CLR.BROWN).add();

		MonsterType.of(MC.FUNGUS, PM.YELLOW_MOLD, "yellow mold").level(1).move(0).ac(9).mr(0).neutral().freq(2)
				.genocidable().weight(50).nutrition(30).sound(MS.SILENT).size(MZ.SMALL).resists(MR.POISON)
				.conveys(MR.POISON)
				.flags1(M1.BREATHLESS, M1.MINDLESS, M1.NOEYES, M1.NOHANDS, M1.NOHEAD, M1.NOLIMBS, M1.NOTAKE, M1.POIS)
				.flags2(M2.HOSTILE, M2.NEUTER).color(CLR.YELLOW).add();

		MonsterType.of(MC.FUNGUS, PM.GREEN_MOLD, "green mold").level(1).move(0).ac(9).mr(0).neutral().freq(1)
				.genocidable().weight(50).nutrition(30).sound(MS.SILENT).size(MZ.SMALL).resists(MR.ACID, MR.STONE)
				.conveys(MR.STONE)
				.flags1(M1.ACID, M1.BREATHLESS, M1.MINDLESS, M1.NOEYES, M1.NOHANDS, M1.NOHEAD, M1.NOLIMBS, M1.NOTAKE)
				.flags2(M2.HOSTILE, M2.NEUTER).color(CLR.GREEN).add();

		MonsterType.of(MC.FUNGUS, PM.RED_MOLD, "red mold").level(1).move(0).ac(9).mr(0).neutral().freq(1).genocidable()
				.weight(50).nutrition(30).sound(MS.SILENT).size(MZ.SMALL).resists(MR.FIRE, MR.POISON)
				.conveys(MR.FIRE, MR.POISON)
				.flags1(M1.BREATHLESS, M1.MINDLESS, M1.NOEYES, M1.NOHANDS, M1.NOHEAD, M1.NOLIMBS, M1.NOTAKE)
				.flags2(M2.HOSTILE, M2.NEUTER).flags3(M3.INFRAVISIBLE).color(CLR.RED).add();

		MonsterType.of(MC.FUNGUS, PM.SHRIEKER, "shrieker").level(3).move(1).ac(7).mr(0).neutral().freq(1).genocidable()
				.weight(100).nutrition(100).sound(MS.SHRIEK).size(MZ.SMALL).resists(MR.POISON).conveys(MR.POISON)
				.flags1(M1.BREATHLESS, M1.MINDLESS, M1.NOEYES, M1.NOHANDS, M1.NOHEAD, M1.NOLIMBS, M1.NOTAKE)
				.flags2(M2.HOSTILE, M2.NEUTER).color(CLR.MAGENTA).add();

		MonsterType.of(MC.FUNGUS, PM.VIOLET_FUNGUS, "violet fungus").level(3).move(1).ac(7).mr(0).neutral().freq(2)
				.genocidable().attack(AT.TUCH, AD.PHYS, 1, Dice.D4).attack(AT.TUCH, AD.STCK, 0, Dice.D0).weight(100)
				.nutrition(100).sound(MS.SILENT).size(MZ.SMALL).resists(MR.POISON).conveys(MR.POISON)
				.flags1(M1.BREATHLESS, M1.MINDLESS, M1.NOEYES, M1.NOHANDS, M1.NOHEAD, M1.NOLIMBS, M1.NOTAKE)
				.flags2(M2.HOSTILE, M2.NEUTER).color(CLR.MAGENTA).add();
	}

	private static void initializeGnomes() {

		MonsterType.of(MC.GNOME, PM.GNOME, "gnome").level(1).move(6).ac(10).mr(4).neutral().freq(1).smallGroups()
				.genocidable().attack(AT.WEAP, AD.PHYS, 1, Dice.D6).weight(650).nutrition(100).sound(MS.GRUNT)
				.size(MZ.SMALL).flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.GNOME, M2.NOPOLY).flags3(M3.INFRAVISION, M3.INFRAVISIBLE).color(CLR.BROWN).add();

		MonsterType.of(MC.GNOME, PM.GNOME_LORD, "gnome lord").level(3).move(8).ac(10).mr(4).neutral().freq(2)
				.genocidable().attack(AT.WEAP, AD.PHYS, 1, Dice.D8).weight(700).nutrition(120).sound(MS.GRUNT)
				.size(MZ.SMALL).flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.GNOME, M2.LORD, M2.MALE).flags3(M3.INFRAVISION, M3.INFRAVISIBLE).color(CLR.BLUE)
				.add();

		MonsterType.of(MC.GNOME, PM.GNOMISH_WIZARD, "gnomish wizard").level(3).move(10).ac(4).mr(10).neutral().freq(1)
				.genocidable().attack(AT.MAGC, AD.SPEL, 0, Dice.D0).weight(700).nutrition(120).sound(MS.GRUNT)
				.size(MZ.SMALL).flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE).flags2(M2.GNOME, M2.MAGIC)
				.flags3(M3.INFRAVISION, M3.INFRAVISIBLE).color(CLR.BRIGHT_BLUE).add();

		MonsterType.of(MC.GNOME, PM.GNOME_KING, "gnome king").level(5).move(10).ac(10).mr(20).neutral().freq(1)
				.genocidable().attack(AT.WEAP, AD.PHYS, 2, Dice.D6).weight(750).nutrition(150).sound(MS.GRUNT)
				.size(MZ.SMALL).flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.GNOME, M2.MALE, M2.PRINCE).flags3(M3.INFRAVISION, M3.INFRAVISIBLE)
				.color(CLR.MAGENTA).add();
	}

	private static void initializeGiants() {

		MonsterType.of(MC.GIANT, PM.GIANT, "giant").level(6).move(6).ac(0).mr(0).lawful(2).freq(1).special()
				.genocidable().attack(AT.WEAP, AD.PHYS, 2, Dice.D10).weight(2250).nutrition(750).sound(MS.BOAST)
				.size(MZ.HUGE).flags1(M1.CARNIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.GIANT, M2.JEWELS, M2.NASTY, M2.ROCKTHROW, M2.STRONG)
				.flags3(M3.INFRAVISION, M3.INFRAVISIBLE).color(CLR.RED).add();

		MonsterType.of(MC.GIANT, PM.STONE_GIANT, "stone giant").level(6).move(6).ac(0).mr(0).lawful(2).freq(1)
				.smallGroups().genocidable().attack(AT.WEAP, AD.PHYS, 2, Dice.D10).weight(2250).nutrition(750)
				.sound(MS.BOAST).size(MZ.HUGE).flags1(M1.CARNIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.GIANT, M2.JEWELS, M2.NASTY, M2.ROCKTHROW, M2.STRONG)
				.flags3(M3.INFRAVISION, M3.INFRAVISIBLE).color(CLR.GRAY).add();

		MonsterType.of(MC.GIANT, PM.HILL_GIANT, "hill giant").level(8).move(10).ac(6).mr(0).chaotic(2).freq(1)
				.smallGroups().genocidable().attack(AT.WEAP, AD.PHYS, 2, Dice.D8).weight(2200).nutrition(700)
				.sound(MS.BOAST).size(MZ.HUGE).flags1(M1.CARNIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.GIANT, M2.JEWELS, M2.NASTY, M2.ROCKTHROW, M2.STRONG)
				.flags3(M3.INFRAVISION, M3.INFRAVISIBLE).color(CLR.CYAN).add();

		MonsterType.of(MC.GIANT, PM.FIRE_GIANT, "fire giant").level(9).move(12).ac(4).mr(5).lawful(2).freq(1)
				.smallGroups().genocidable().attack(AT.WEAP, AD.PHYS, 2, Dice.D10).weight(2250).nutrition(750)
				.sound(MS.BOAST).size(MZ.HUGE).resists(MR.FIRE).conveys(MR.FIRE)
				.flags1(M1.CARNIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.GIANT, M2.JEWELS, M2.NASTY, M2.ROCKTHROW, M2.STRONG)
				.flags3(M3.INFRAVISION, M3.INFRAVISIBLE).color(CLR.YELLOW).add();

		MonsterType.of(MC.GIANT, PM.FROST_GIANT, "frost giant").level(10).move(12).ac(3).mr(10).chaotic(3).freq(1)
				.notInHell().smallGroups().genocidable().attack(AT.WEAP, AD.PHYS, 2, Dice.D12).weight(2250)
				.nutrition(750).sound(MS.BOAST).size(MZ.HUGE).resists(MR.COLD).conveys(MR.COLD)
				.flags1(M1.CARNIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.GIANT, M2.JEWELS, M2.NASTY, M2.ROCKTHROW, M2.STRONG)
				.flags3(M3.INFRAVISION, M3.INFRAVISIBLE).color(CLR.WHITE).add();

		MonsterType.of(MC.GIANT, PM.ETTIN, "ettin").level(10).move(12).ac(3).mr(0).neutral().freq(1).genocidable()
				.attack(AT.WEAP, AD.PHYS, 2, Dice.D8).attack(AT.WEAP, AD.PHYS, 3, Dice.D6).weight(1700).nutrition(500)
				.sound(MS.GRUNT).size(MZ.HUGE).flags1(M1.ANIMAL, M1.CARNIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.HOSTILE, M2.NASTY, M2.STRONG).flags3(M3.INFRAVISION, M3.INFRAVISIBLE)
				.color(CLR.BROWN).add();

		MonsterType.of(MC.GIANT, PM.STORM_GIANT, "storm giant").level(16).move(12).ac(3).mr(10).chaotic(3).freq(1)
				.smallGroups().genocidable().attack(AT.WEAP, AD.PHYS, 2, Dice.D12).weight(2250).nutrition(750)
				.sound(MS.BOAST).size(MZ.HUGE).resists(MR.ELEC).conveys(MR.ELEC)
				.flags1(M1.CARNIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.GIANT, M2.JEWELS, M2.NASTY, M2.ROCKTHROW, M2.STRONG)
				.flags3(M3.INFRAVISION, M3.INFRAVISIBLE).color(CLR.BLUE).add();

		MonsterType.of(MC.GIANT, PM.TITAN, "titan").level(16).move(18).ac(-3).mr(70).lawful(9).freq(1)
				.attack(AT.WEAP, AD.PHYS, 2, Dice.D8).attack(AT.MAGC, AD.SPEL, 0, Dice.D0).weight(2300).nutrition(900)
				.sound(MS.SPELL).size(MZ.HUGE).flags1(M1.CARNIVORE, M1.FLY, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.MAGIC, M2.NASTY, M2.ROCKTHROW, M2.STRONG).flags3(M3.INFRAVISION, M3.INFRAVISIBLE)
				.color(CLR.MAGENTA).add();

		MonsterType.of(MC.GIANT, PM.MINOTAUR, "minotaur").level(15).move(15).ac(6).mr(0).neutral().freq(0).special()
				.genocidable().attack(AT.CLAW, AD.PHYS, 3, Dice.D10).attack(AT.CLAW, AD.PHYS, 3, Dice.D10)
				.attack(AT.BUTT, AD.PHYS, 2, Dice.D8).weight(1500).nutrition(700).sound(MS.SILENT).size(MZ.LARGE)
				.flags1(M1.ANIMAL, M1.CARNIVORE, M1.HUMANOID, M1.OMNIVORE).flags2(M2.HOSTILE, M2.NASTY, M2.STRONG)
				.flags3(M3.INFRAVISION, M3.INFRAVISIBLE).color(CLR.BROWN).add();
	}

	private static void initializeJabberwocks() {

		MonsterType.of(MC.JABBERWOCK, PM.JABBERWOCK, "jabberwock").level(15).move(12).ac(-2).mr(50).neutral().freq(1)
				.genocidable().attack(AT.BITE, AD.PHYS, 2, Dice.D10).attack(AT.BITE, AD.PHYS, 2, Dice.D10)
				.attack(AT.CLAW, AD.PHYS, 2, Dice.D10).attack(AT.CLAW, AD.PHYS, 2, Dice.D10).weight(1300).nutrition(600)
				.sound(MS.BURBLE).size(MZ.LARGE).flags1(M1.ANIMAL, M1.CARNIVORE, M1.FLY, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.HOSTILE, M2.NASTY, M2.STRONG).flags3(M3.INFRAVISIBLE).color(CLR.ORANGE).add();
	}

	private static void initializeKops() {

		MonsterType.of(MC.KOP, PM.KEYSTONE_KOP, "Keystone Kop").level(1).move(6).ac(10).mr(10).lawful(9).freq(0)
				.special().largeGroups().genocidable().attack(AT.WEAP, AD.PHYS, 1, Dice.D4).weight(1450).nutrition(200)
				.sound(MS.ARREST).size(MZ.HUMAN).flags1(M1.HUMANOID)
				.flags2(M2.COLLECT, M2.HOSTILE, M2.HUMAN, M2.MALE, M2.WANDER).flags3(M3.INFRAVISIBLE).color(CLR.BLUE)
				.add();

		MonsterType.of(MC.KOP, PM.KOP_SERGEANT, "Kop Sergeant").level(2).move(8).ac(10).mr(10).lawful(10).freq(0)
				.special().smallGroups().genocidable().attack(AT.WEAP, AD.PHYS, 1, Dice.D6).weight(1450).nutrition(200)
				.sound(MS.ARREST).size(MZ.HUMAN).flags1(M1.HUMANOID)
				.flags2(M2.COLLECT, M2.HOSTILE, M2.HUMAN, M2.MALE, M2.STRONG, M2.WANDER).flags3(M3.INFRAVISIBLE)
				.color(CLR.BLUE).add();

		MonsterType.of(MC.KOP, PM.KOP_LIEUTENANT, "Kop Lieutenant").level(3).move(10).ac(10).mr(20).lawful(11).freq(0)
				.special().genocidable().attack(AT.WEAP, AD.PHYS, 1, Dice.D8).weight(1450).nutrition(200)
				.sound(MS.ARREST).size(MZ.HUMAN).flags1(M1.HUMANOID)
				.flags2(M2.COLLECT, M2.HOSTILE, M2.HUMAN, M2.MALE, M2.STRONG, M2.WANDER).flags3(M3.INFRAVISIBLE)
				.color(CLR.CYAN).add();

		MonsterType.of(MC.KOP, PM.KOP_KAPTAIN, "Kop Kaptain").level(4).move(12).ac(10).mr(20).lawful(12).freq(0)
				.special().genocidable().attack(AT.WEAP, AD.PHYS, 2, Dice.D6).weight(1450).nutrition(200)
				.sound(MS.ARREST).size(MZ.HUMAN).flags1(M1.HUMANOID)
				.flags2(M2.COLLECT, M2.HOSTILE, M2.HUMAN, M2.MALE, M2.STRONG, M2.WANDER).flags3(M3.INFRAVISIBLE)
				.color(CLR.MAGENTA).add();
	}

	private static void initializeLiches() {

		MonsterType.of(MC.LICH, PM.LICH, "lich").level(11).move(6).ac(0).mr(30).chaotic(9).freq(1).genocidable()
				.noCorpse().attack(AT.TUCH, AD.COLD, 1, Dice.D10).attack(AT.MAGC, AD.SPEL, 0, Dice.D0).weight(1200)
				.nutrition(100).sound(MS.MUMBLE).size(MZ.HUMAN).resists(MR.COLD, MR.SLEEP, MR.POISON).conveys(MR.COLD)
				.flags1(M1.BREATHLESS, M1.HUMANOID, M1.POIS, M1.REGEN).flags2(M2.HOSTILE, M2.MAGIC, M2.UNDEAD)
				.flags3(M3.INFRAVISION).color(CLR.BROWN).add();

		MonsterType.of(MC.LICH, PM.DEMILICH, "demilich").level(14).move(9).ac(-2).mr(60).chaotic(12).freq(1)
				.genocidable().noCorpse().attack(AT.TUCH, AD.COLD, 3, Dice.D4).attack(AT.MAGC, AD.SPEL, 0, Dice.D0)
				.weight(1200).nutrition(100).sound(MS.MUMBLE).size(MZ.HUMAN).resists(MR.COLD, MR.SLEEP, MR.POISON)
				.conveys(MR.COLD).flags1(M1.BREATHLESS, M1.HUMANOID, M1.POIS, M1.REGEN)
				.flags2(M2.HOSTILE, M2.MAGIC, M2.UNDEAD).flags3(M3.INFRAVISION).color(CLR.RED).add();

		MonsterType.of(MC.LICH, PM.MASTER_LICH, "master lich").level(17).move(9).ac(-4).mr(90).chaotic(15).freq(1)
				.onlyInHell().genocidable().noCorpse().attack(AT.TUCH, AD.COLD, 3, Dice.D6)
				.attack(AT.MAGC, AD.SPEL, 0, Dice.D0).weight(1200).nutrition(100).sound(MS.MUMBLE).size(MZ.HUMAN)
				.resists(MR.FIRE, MR.COLD, MR.SLEEP, MR.POISON).conveys(MR.FIRE, MR.COLD)
				.flags1(M1.BREATHLESS, M1.HUMANOID, M1.POIS, M1.REGEN).flags2(M2.HOSTILE, M2.MAGIC, M2.UNDEAD)
				.flags3(M3.WANTSBOOK, M3.WANTSALL, M3.COVETOUS, M3.INFRAVISION).color(CLR.MAGENTA).add();

		MonsterType.of(MC.LICH, PM.ARCH_LICH, "arch-lich").level(25).move(9).ac(-6).mr(90).chaotic(15).freq(1)
				.onlyInHell().genocidable().noCorpse().attack(AT.TUCH, AD.COLD, 5, Dice.D6)
				.attack(AT.MAGC, AD.SPEL, 0, Dice.D0).weight(1200).nutrition(100).sound(MS.MUMBLE).size(MZ.HUMAN)
				.resists(MR.FIRE, MR.COLD, MR.SLEEP, MR.ELEC, MR.POISON).conveys(MR.FIRE, MR.COLD)
				.flags1(M1.BREATHLESS, M1.HUMANOID, M1.POIS, M1.REGEN).flags2(M2.HOSTILE, M2.MAGIC, M2.UNDEAD)
				.flags3(M3.WANTSBOOK, M3.WANTSALL, M3.COVETOUS, M3.INFRAVISION).color(CLR.MAGENTA).add();
	}

	private static void initializeMummies() {

		MonsterType.of(MC.MUMMY, PM.KOBOLD_MUMMY, "kobold mummy").level(3).move(8).ac(6).mr(20).chaotic(2).freq(1)
				.genocidable().noCorpse().attack(AT.CLAW, AD.PHYS, 1, Dice.D4).weight(400).nutrition(50)
				.sound(MS.SILENT).size(MZ.SMALL).resists(MR.COLD, MR.SLEEP, MR.POISON)
				.flags1(M1.BREATHLESS, M1.HUMANOID, M1.MINDLESS, M1.POIS).flags2(M2.HOSTILE, M2.UNDEAD)
				.flags3(M3.INFRAVISION).color(CLR.BROWN).add();

		MonsterType.of(MC.MUMMY, PM.GNOME_MUMMY, "gnome mummy").level(4).move(10).ac(6).mr(20).chaotic(3).freq(1)
				.genocidable().noCorpse().attack(AT.CLAW, AD.PHYS, 1, Dice.D6).weight(650).nutrition(50)
				.sound(MS.SILENT).size(MZ.SMALL).resists(MR.COLD, MR.SLEEP, MR.POISON)
				.flags1(M1.BREATHLESS, M1.HUMANOID, M1.MINDLESS, M1.POIS).flags2(M2.GNOME, M2.HOSTILE, M2.UNDEAD)
				.flags3(M3.INFRAVISION).color(CLR.RED).add();

		MonsterType.of(MC.MUMMY, PM.ORC_MUMMY, "orc mummy").level(5).move(10).ac(5).mr(20).chaotic(4).freq(1)
				.genocidable().noCorpse().attack(AT.CLAW, AD.PHYS, 1, Dice.D6).weight(850).nutrition(75)
				.sound(MS.SILENT).size(MZ.HUMAN).resists(MR.COLD, MR.SLEEP, MR.POISON)
				.flags1(M1.BREATHLESS, M1.HUMANOID, M1.MINDLESS, M1.POIS)
				.flags2(M2.GREEDY, M2.HOSTILE, M2.JEWELS, M2.ORC, M2.UNDEAD).flags3(M3.INFRAVISION).color(CLR.GRAY)
				.add();

		MonsterType.of(MC.MUMMY, PM.DWARF_MUMMY, "dwarf mummy").level(5).move(10).ac(5).mr(20).chaotic(4).freq(1)
				.genocidable().noCorpse().attack(AT.CLAW, AD.PHYS, 1, Dice.D6).weight(900).nutrition(150)
				.sound(MS.SILENT).size(MZ.HUMAN).resists(MR.COLD, MR.SLEEP, MR.POISON)
				.flags1(M1.BREATHLESS, M1.HUMANOID, M1.MINDLESS, M1.POIS)
				.flags2(M2.DWARF, M2.GREEDY, M2.HOSTILE, M2.JEWELS, M2.UNDEAD).flags3(M3.INFRAVISION).color(CLR.RED)
				.add();

		MonsterType.of(MC.MUMMY, PM.ELF_MUMMY, "elf mummy").level(6).move(12).ac(4).mr(30).chaotic(5).freq(1)
				.genocidable().noCorpse().attack(AT.CLAW, AD.PHYS, 2, Dice.D4).weight(800).nutrition(175)
				.sound(MS.SILENT).size(MZ.HUMAN).resists(MR.COLD, MR.SLEEP, MR.POISON)
				.flags1(M1.BREATHLESS, M1.HUMANOID, M1.MINDLESS, M1.POIS).flags2(M2.ELF, M2.HOSTILE, M2.UNDEAD)
				.flags3(M3.INFRAVISION).color(CLR.GREEN).add();

		MonsterType.of(MC.MUMMY, PM.HUMAN_MUMMY, "human mummy").level(6).move(12).ac(4).mr(30).chaotic(5).freq(1)
				.genocidable().noCorpse().attack(AT.CLAW, AD.PHYS, 2, Dice.D4).attack(AT.CLAW, AD.PHYS, 2, Dice.D4)
				.weight(1450).nutrition(200).sound(MS.SILENT).size(MZ.HUMAN).resists(MR.COLD, MR.SLEEP, MR.POISON)
				.flags1(M1.BREATHLESS, M1.HUMANOID, M1.MINDLESS, M1.POIS).flags2(M2.HOSTILE, M2.UNDEAD)
				.flags3(M3.INFRAVISION).color(CLR.GRAY).add();

		MonsterType.of(MC.MUMMY, PM.ETTIN_MUMMY, "ettin mummy").level(7).move(12).ac(4).mr(30).chaotic(6).freq(1)
				.genocidable().noCorpse().attack(AT.CLAW, AD.PHYS, 2, Dice.D6).attack(AT.CLAW, AD.PHYS, 2, Dice.D6)
				.weight(1700).nutrition(250).sound(MS.SILENT).size(MZ.HUGE).resists(MR.COLD, MR.SLEEP, MR.POISON)
				.flags1(M1.BREATHLESS, M1.HUMANOID, M1.MINDLESS, M1.POIS).flags2(M2.HOSTILE, M2.STRONG, M2.UNDEAD)
				.flags3(M3.INFRAVISION).color(CLR.BLUE).add();

		MonsterType.of(MC.MUMMY, PM.GIANT_MUMMY, "giant mummy").level(8).move(14).ac(3).mr(30).chaotic(7).freq(1)
				.genocidable().noCorpse().attack(AT.CLAW, AD.PHYS, 3, Dice.D4).attack(AT.CLAW, AD.PHYS, 3, Dice.D4)
				.weight(2050).nutrition(375).sound(MS.SILENT).size(MZ.HUGE).resists(MR.COLD, MR.SLEEP, MR.POISON)
				.flags1(M1.BREATHLESS, M1.HUMANOID, M1.MINDLESS, M1.POIS)
				.flags2(M2.GIANT, M2.HOSTILE, M2.JEWELS, M2.STRONG, M2.UNDEAD).flags3(M3.INFRAVISION).color(CLR.CYAN)
				.add();
	}

	private static void initializeNagas() {

		MonsterType.of(MC.NAGA, PM.RED_NAGA_HATCHLING, "red naga hatchling").level(3).move(10).ac(6).mr(0).neutral()
				.freq(0).genocidable().attack(AT.BITE, AD.PHYS, 1, Dice.D4).weight(500).nutrition(100).sound(MS.MUMBLE)
				.size(MZ.LARGE).resists(MR.FIRE, MR.POISON)
				.conveys(MR.FIRE, MR.POISON).flags1(M1.CARNIVORE, M1.HERBIVORE, M1.NOHANDS, M1.NOLIMBS, M1.NOTAKE,
						M1.OMNIVORE, M1.SLITHY, M1.THICK_HIDE)
				.flags2(M2.STRONG).flags3(M3.INFRAVISIBLE).color(CLR.RED).add();

		MonsterType.of(MC.NAGA, PM.BLACK_NAGA_HATCHLING, "black naga hatchling").level(3).move(10).ac(6).mr(0).neutral()
				.freq(0).genocidable().attack(AT.BITE, AD.PHYS, 1, Dice.D4).weight(500).nutrition(100).sound(MS.MUMBLE)
				.size(MZ.LARGE).resists(MR.POISON, MR.ACID, MR.STONE).conveys(MR.POISON, MR.STONE)
				.flags1(M1.ACID, M1.CARNIVORE, M1.NOHANDS, M1.NOLIMBS, M1.NOTAKE, M1.OMNIVORE, M1.SLITHY, M1.THICK_HIDE)
				.flags2(M2.STRONG).color(CLR.BLACK).add();

		MonsterType.of(MC.NAGA, PM.GOLDEN_NAGA_HATCHLING, "golden naga hatchling").level(3).move(10).ac(6).mr(0)
				.neutral().freq(0).genocidable().attack(AT.BITE, AD.PHYS, 1, Dice.D4).weight(500).nutrition(100)
				.sound(MS.MUMBLE)
				.size(MZ.LARGE).resists(MR.POISON).conveys(MR.POISON).flags1(M1.CARNIVORE, M1.HERBIVORE, M1.NOHANDS,
						M1.NOLIMBS, M1.NOTAKE, M1.OMNIVORE, M1.SLITHY, M1.THICK_HIDE)
				.flags2(M2.STRONG).color(CLR.GOLD).add();

		MonsterType.of(MC.NAGA, PM.GUARDIAN_NAGA_HATCHLING, "guardian naga hatchling").level(3).move(10).ac(6).mr(0)
				.neutral().freq(0).genocidable().attack(AT.BITE, AD.PHYS, 1, Dice.D4).weight(500).nutrition(100)
				.sound(MS.MUMBLE)
				.size(MZ.LARGE).resists(MR.POISON).conveys(MR.POISON).flags1(M1.CARNIVORE, M1.HERBIVORE, M1.NOHANDS,
						M1.NOLIMBS, M1.NOTAKE, M1.OMNIVORE, M1.SLITHY, M1.THICK_HIDE)
				.flags2(M2.STRONG).color(CLR.GREEN).add();

		MonsterType.of(MC.NAGA, PM.RED_NAGA, "red naga").level(6).move(12).ac(4).mr(0).chaotic(4).freq(1).genocidable()
				.attack(AT.BITE, AD.PHYS, 2, Dice.D4).attack(AT.BREA, AD.FIRE, 2, Dice.D6).weight(2600).nutrition(400)
				.sound(MS.MUMBLE).size(MZ.HUGE).resists(MR.FIRE, MR.POISON)
				.conveys(MR.FIRE, MR.POISON).flags1(M1.CARNIVORE, M1.HERBIVORE, M1.NOHANDS, M1.NOLIMBS, M1.NOTAKE,
						M1.OMNIVORE, M1.OVIPAROUS, M1.SLITHY, M1.THICK_HIDE)
				.flags2(M2.STRONG).flags3(M3.INFRAVISIBLE).color(CLR.RED).add();

		MonsterType.of(MC.NAGA, PM.BLACK_NAGA, "black naga").level(8).move(14).ac(2).mr(10).lawful(4).freq(1)
				.genocidable().attack(AT.BITE, AD.PHYS, 2, Dice.D6).attack(AT.SPIT, AD.ACID, 0, Dice.D0).weight(2600)
				.nutrition(400).sound(MS.MUMBLE).size(MZ.HUGE)
				.resists(MR.POISON, MR.ACID, MR.STONE).conveys(MR.POISON, MR.STONE).flags1(M1.ACID, M1.CARNIVORE,
						M1.NOHANDS, M1.NOLIMBS, M1.NOTAKE, M1.OMNIVORE, M1.OVIPAROUS, M1.SLITHY, M1.THICK_HIDE)
				.flags2(M2.STRONG).color(CLR.BLACK).add();

		MonsterType.of(MC.NAGA, PM.GOLDEN_NAGA, "golden naga").level(10).move(14).ac(2).mr(70).lawful(5).freq(1)
				.genocidable().attack(AT.BITE, AD.PHYS, 2, Dice.D6).attack(AT.MAGC, AD.SPEL, 4, Dice.D6).weight(2600)
				.nutrition(400).sound(MS.MUMBLE).size(MZ.HUGE)
				.resists(MR.POISON).conveys(MR.POISON).flags1(M1.CARNIVORE, M1.HERBIVORE, M1.NOHANDS, M1.NOLIMBS,
						M1.NOTAKE, M1.OMNIVORE, M1.OVIPAROUS, M1.SLITHY, M1.THICK_HIDE)
				.flags2(M2.STRONG).color(CLR.GOLD).add();

		MonsterType.of(MC.NAGA, PM.GUARDIAN_NAGA, "guardian naga").level(12).move(16).ac(0).mr(50).lawful(7).freq(1)
				.genocidable().attack(AT.BITE, AD.PLYS, 1, Dice.D6).attack(AT.SPIT, AD.DRST, 1, Dice.D6)
				.attack(AT.HUGS, AD.PHYS, 2, Dice.D4).weight(2600).nutrition(400).sound(MS.MUMBLE).size(MZ.HUGE)
				.resists(MR.POISON).conveys(MR.POISON).flags1(M1.CARNIVORE, M1.HERBIVORE, M1.NOHANDS, M1.NOLIMBS,
						M1.NOTAKE, M1.OMNIVORE, M1.OVIPAROUS, M1.POIS, M1.SLITHY, M1.THICK_HIDE)
				.flags2(M2.STRONG).color(CLR.GREEN).add();
	}

	private static void initializeOgres() {

		MonsterType.of(MC.OGRE, PM.OGRE, "ogre").level(5).move(10).ac(5).mr(0).chaotic(3).freq(1).smallGroups()
				.genocidable().attack(AT.WEAP, AD.PHYS, 2, Dice.D5).weight(1600).nutrition(500).sound(MS.GRUNT)
				.size(MZ.LARGE).flags1(M1.CARNIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.GREEDY, M2.JEWELS, M2.STRONG).flags3(M3.INFRAVISION, M3.INFRAVISIBLE)
				.color(CLR.BROWN).add();

		MonsterType.of(MC.OGRE, PM.OGRE_LORD, "ogre lord").level(7).move(12).ac(3).mr(30).chaotic(5).freq(2)
				.genocidable().attack(AT.WEAP, AD.PHYS, 2, Dice.D6).weight(1700).nutrition(700).sound(MS.GRUNT)
				.size(MZ.LARGE).flags1(M1.CARNIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.GREEDY, M2.JEWELS, M2.LORD, M2.MALE, M2.STRONG)
				.flags3(M3.INFRAVISION, M3.INFRAVISIBLE).color(CLR.RED).add();

		MonsterType.of(MC.OGRE, PM.OGRE_KING, "ogre king").level(9).move(14).ac(4).mr(60).chaotic(7).freq(2)
				.genocidable().attack(AT.WEAP, AD.PHYS, 3, Dice.D5).weight(1700).nutrition(750).sound(MS.GRUNT)
				.size(MZ.LARGE).flags1(M1.CARNIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.GREEDY, M2.JEWELS, M2.MALE, M2.PRINCE, M2.STRONG)
				.flags3(M3.INFRAVISION, M3.INFRAVISIBLE).color(CLR.MAGENTA).add();
	}

	private static void initializePuddings() {

		MonsterType.of(MC.PUDDING, PM.GRAY_OOZE, "gray ooze").level(3).move(1).ac(8).mr(0).neutral().freq(2)
				.genocidable().noCorpse().attack(AT.BITE, AD.RUST, 2, Dice.D8).weight(500).nutrition(250)
				.sound(MS.SILENT).size(MZ.MEDIUM).resists(MR.FIRE, MR.COLD, MR.POISON, MR.ACID, MR.STONE)
				.conveys(MR.FIRE, MR.COLD, MR.POISON)
				.flags1(M1.ACID, M1.AMORPHOUS, M1.BREATHLESS, M1.CARNIVORE, M1.HERBIVORE, M1.MINDLESS, M1.NOEYES,
						M1.NOHANDS, M1.NOHEAD, M1.NOLIMBS, M1.OMNIVORE)
				.flags2(M2.HOSTILE, M2.NEUTER).color(CLR.GRAY).add();

		MonsterType.of(MC.PUDDING, PM.BROWN_PUDDING, "brown pudding").level(5).move(3).ac(8).mr(0).neutral().freq(1)
				.genocidable().noCorpse().attack(AT.BITE, AD.DCAY, 0, Dice.D0).weight(500).nutrition(250)
				.sound(MS.SILENT).size(MZ.MEDIUM).resists(MR.COLD, MR.ELEC, MR.POISON, MR.ACID, MR.STONE)
				.conveys(MR.COLD, MR.ELEC, MR.POISON)
				.flags1(M1.ACID, M1.AMORPHOUS, M1.BREATHLESS, M1.CARNIVORE, M1.HERBIVORE, M1.MINDLESS, M1.NOEYES,
						M1.NOHANDS, M1.NOHEAD, M1.NOLIMBS, M1.OMNIVORE)
				.flags2(M2.HOSTILE, M2.NEUTER).color(CLR.BROWN).add();

		MonsterType.of(MC.PUDDING, PM.GREEN_SLIME, "green slime").level(6).move(6).ac(6).mr(0).neutral().freq(1)
				.onlyInHell().genocidable().noCorpse().attack(AT.TUCH, AD.SLIM, 1, Dice.D4).weight(400).nutrition(150)
				.sound(MS.SILENT).size(MZ.LARGE).resists(MR.COLD, MR.ELEC, MR.POISON, MR.ACID, MR.STONE)
				.flags1(M1.ACID, M1.AMORPHOUS, M1.BREATHLESS, M1.CARNIVORE, M1.HERBIVORE, M1.MINDLESS, M1.NOEYES,
						M1.NOHANDS, M1.NOHEAD, M1.NOLIMBS, M1.OMNIVORE, M1.POIS)
				.flags2(M2.HOSTILE, M2.NEUTER).color(CLR.GREEN).add();

		MonsterType.of(MC.PUDDING, PM.BLACK_PUDDING, "black pudding").level(10).move(6).ac(6).mr(0).neutral().freq(1)
				.genocidable().noCorpse().attack(AT.BITE, AD.CORR, 3, Dice.D8).weight(900).nutrition(250)
				.sound(MS.SILENT).size(MZ.LARGE).resists(MR.COLD, MR.ELEC, MR.POISON, MR.ACID, MR.STONE)
				.conveys(MR.COLD, MR.ELEC, MR.POISON)
				.flags1(M1.ACID, M1.AMORPHOUS, M1.BREATHLESS, M1.CARNIVORE, M1.HERBIVORE, M1.MINDLESS, M1.NOEYES,
						M1.NOHANDS, M1.NOHEAD, M1.NOLIMBS, M1.OMNIVORE)
				.flags2(M2.HOSTILE, M2.NEUTER).color(CLR.BLACK).add();
	}

	private static void initializeQuantumMechanics() {

		MonsterType.of(MC.QUANTMECH, PM.QUANTUM_MECHANIC, "quantum mechanic").level(7).move(12).ac(3).mr(10).neutral()
				.freq(3).genocidable().attack(AT.CLAW, AD.TLPT, 1, Dice.D4).weight(1450).nutrition(20)
				.sound(MS.HUMANOID).size(MZ.HUMAN).resists(MR.POISON)
				.flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE, M1.POIS, M1.TPORT).flags2(M2.HOSTILE)
				.flags3(M3.INFRAVISIBLE).color(CLR.CYAN).add();
	}

	private static void initializeRustMonsters() {

		MonsterType.of(MC.RUSTMONST, PM.RUST_MONSTER, "rust monster").level(5).move(18).ac(2).mr(0).neutral().freq(2)
				.genocidable().attack(AT.TUCH, AD.RUST, 0, Dice.D0).attack(AT.TUCH, AD.RUST, 0, Dice.D0).weight(1000)
				.nutrition(250).sound(MS.SILENT).size(MZ.MEDIUM)
				.flags1(M1.ANIMAL, M1.METALLIVORE, M1.NOHANDS, M1.NOLIMBS, M1.SWIM).flags2(M2.HOSTILE)
				.flags3(M3.INFRAVISIBLE).color(CLR.BROWN).add();

		MonsterType.of(MC.RUSTMONST, PM.DISENCHANTER, "disenchanter").level(12).move(12).ac(-10).mr(0).chaotic(3)
				.freq(2).onlyInHell().genocidable().attack(AT.CLAW, AD.ENCH, 4, Dice.D4).weight(750).nutrition(200)
				.sound(MS.GROWL).size(MZ.LARGE).flags1(M1.ANIMAL, M1.CARNIVORE, M1.OMNIVORE).flags2(M2.HOSTILE)
				.flags3(M3.INFRAVISIBLE).color(CLR.BLUE).add();
	}

	private static void initializeSnakes() {

		MonsterType.of(MC.SNAKE, PM.GARTER_SNAKE, "garter snake").level(1).move(8).ac(8).mr(0).neutral().freq(1)
				.largeGroups().genocidable().attack(AT.BITE, AD.PHYS, 1, Dice.D2)
				.weight(50).nutrition(60).sound(MS.HISS).size(MZ.TINY).flags1(M1.ANIMAL, M1.CARNIVORE, M1.CONCEAL,
						M1.NOHANDS, M1.NOLIMBS, M1.NOTAKE, M1.OMNIVORE, M1.OVIPAROUS, M1.SLITHY, M1.SWIM)
				.color(CLR.GREEN).add();

		MonsterType.of(MC.SNAKE, PM.SNAKE, "snake").level(4).move(15).ac(3).mr(0).neutral().freq(2).genocidable()
				.attack(AT.BITE, AD.DRST, 1, Dice.D6).weight(100).nutrition(80).sound(MS.HISS).size(MZ.SMALL)
				.resists(MR.POISON)
				.conveys(MR.POISON).flags1(M1.ANIMAL, M1.CARNIVORE, M1.CONCEAL, M1.NOHANDS, M1.NOLIMBS, M1.NOTAKE,
						M1.OMNIVORE, M1.OVIPAROUS, M1.POIS, M1.SLITHY, M1.SWIM)
				.flags2(M2.HOSTILE).color(CLR.BROWN).add();

		MonsterType.of(MC.SNAKE, PM.WATER_MOCCASIN, "water moccasin").level(4).move(15).ac(3).mr(0).neutral().freq(0)
				.special().largeGroups().genocidable().attack(AT.BITE, AD.DRST, 1, Dice.D6).weight(150).nutrition(80)
				.sound(MS.HISS).size(MZ.SMALL)
				.resists(MR.POISON).conveys(MR.POISON).flags1(M1.ANIMAL, M1.CARNIVORE, M1.CONCEAL, M1.NOHANDS,
						M1.NOLIMBS, M1.NOTAKE, M1.OMNIVORE, M1.OVIPAROUS, M1.POIS, M1.SLITHY, M1.SWIM)
				.flags2(M2.HOSTILE).color(CLR.RED).add();

		MonsterType.of(MC.SNAKE, PM.PYTHON, "python").level(6).move(3).ac(5).mr(0).neutral().freq(1).genocidable()
				.attack(AT.BITE, AD.PHYS, 1, Dice.D4).attack(AT.TUCH, AD.PHYS, 0, Dice.D0)
				.attack(AT.HUGS, AD.WRAP, 1, Dice.D4).attack(AT.HUGS, AD.PHYS, 2, Dice.D4).weight(250).nutrition(100)
				.sound(MS.HISS).size(MZ.LARGE)
				.flags1(M1.ANIMAL, M1.CARNIVORE, M1.NOHANDS, M1.NOLIMBS, M1.NOTAKE, M1.OMNIVORE, M1.OVIPAROUS,
						M1.SLITHY, M1.SWIM)
				.flags2(M2.HOSTILE, M2.STRONG).flags3(M3.INFRAVISION).color(CLR.MAGENTA).add();

		MonsterType.of(MC.SNAKE, PM.PIT_VIPER, "pit viper").level(6).move(15).ac(2).mr(0).neutral().freq(1)
				.genocidable().attack(AT.BITE, AD.DRST, 1, Dice.D4).attack(AT.BITE, AD.DRST, 1, Dice.D4).weight(100)
				.nutrition(60).sound(MS.HISS).size(MZ.MEDIUM).resists(MR.POISON).conveys(MR.POISON)
				.flags1(M1.ANIMAL, M1.CARNIVORE, M1.CONCEAL, M1.NOHANDS, M1.NOLIMBS, M1.NOTAKE, M1.OMNIVORE,
						M1.OVIPAROUS, M1.POIS, M1.SLITHY, M1.SWIM)
				.flags2(M2.HOSTILE).flags3(M3.INFRAVISION).color(CLR.BLUE).add();

		MonsterType.of(MC.SNAKE, PM.COBRA, "cobra").level(6).move(18).ac(2).mr(0).neutral().freq(1).genocidable()
				.attack(AT.BITE, AD.DRST, 2, Dice.D4).attack(AT.SPIT, AD.BLND, 0, Dice.D0).weight(250).nutrition(100)
				.sound(MS.HISS).size(MZ.MEDIUM).resists(MR.POISON)
				.conveys(MR.POISON).flags1(M1.ANIMAL, M1.CARNIVORE, M1.CONCEAL, M1.NOHANDS, M1.NOLIMBS, M1.NOTAKE,
						M1.OMNIVORE, M1.OVIPAROUS, M1.POIS, M1.SLITHY, M1.SWIM)
				.flags2(M2.HOSTILE).color(CLR.BLUE).add();
	}

	private static void initializeTroll() {

		MonsterType.of(MC.TROLL, PM.TROLL, "troll").level(7).move(12).ac(4).mr(0).chaotic(3).freq(2).genocidable()
				.attack(AT.WEAP, AD.PHYS, 4, Dice.D2).attack(AT.CLAW, AD.PHYS, 4, Dice.D2)
				.attack(AT.BITE, AD.PHYS, 2, Dice.D6).weight(800).nutrition(350).sound(MS.GRUNT).size(MZ.LARGE)
				.flags1(M1.CARNIVORE, M1.HUMANOID, M1.OMNIVORE, M1.REGEN).flags2(M2.HOSTILE, M2.STALK, M2.STRONG)
				.flags3(M3.INFRAVISION, M3.INFRAVISIBLE).color(CLR.BROWN).add();

		MonsterType.of(MC.TROLL, PM.ICE_TROLL, "ice troll").level(9).move(10).ac(2).mr(20).chaotic(3).freq(1)
				.notInHell().genocidable().attack(AT.WEAP, AD.PHYS, 2, Dice.D6).attack(AT.CLAW, AD.COLD, 2, Dice.D6)
				.attack(AT.BITE, AD.PHYS, 2, Dice.D6).weight(1000).nutrition(300).sound(MS.GRUNT).size(MZ.LARGE)
				.resists(MR.COLD).conveys(MR.COLD).flags1(M1.CARNIVORE, M1.HUMANOID, M1.OMNIVORE, M1.REGEN)
				.flags2(M2.HOSTILE, M2.STALK, M2.STRONG).flags3(M3.INFRAVISION, M3.INFRAVISIBLE).color(CLR.WHITE).add();

		MonsterType.of(MC.TROLL, PM.ROCK_TROLL, "rock troll").level(9).move(12).ac(0).mr(0).chaotic(3).freq(1)
				.genocidable().attack(AT.WEAP, AD.PHYS, 3, Dice.D6).attack(AT.CLAW, AD.PHYS, 2, Dice.D8)
				.attack(AT.BITE, AD.PHYS, 2, Dice.D6).weight(1200).nutrition(300).sound(MS.GRUNT).size(MZ.LARGE)
				.flags1(M1.CARNIVORE, M1.HUMANOID, M1.OMNIVORE, M1.REGEN)
				.flags2(M2.COLLECT, M2.HOSTILE, M2.STALK, M2.STRONG).flags3(M3.INFRAVISION, M3.INFRAVISIBLE)
				.color(CLR.CYAN).add();

		MonsterType.of(MC.TROLL, PM.WATER_TROLL, "water troll").level(11).move(14).ac(4).mr(40).chaotic(3).freq(0)
				.special().genocidable().attack(AT.WEAP, AD.PHYS, 2, Dice.D8).attack(AT.CLAW, AD.PHYS, 2, Dice.D8)
				.attack(AT.BITE, AD.PHYS, 2, Dice.D6).weight(1200).nutrition(350).sound(MS.GRUNT).size(MZ.LARGE)
				.flags1(M1.CARNIVORE, M1.HUMANOID, M1.OMNIVORE, M1.REGEN, M1.SWIM)
				.flags2(M2.HOSTILE, M2.STALK, M2.STRONG).flags3(M3.INFRAVISION, M3.INFRAVISIBLE).color(CLR.BLUE).add();

		MonsterType.of(MC.TROLL, PM.OLOG_HAI, "Olog-hai").level(13).move(12).ac(-4).mr(0).chaotic(7).freq(1)
				.genocidable().attack(AT.WEAP, AD.PHYS, 3, Dice.D6).attack(AT.CLAW, AD.PHYS, 2, Dice.D8)
				.attack(AT.BITE, AD.PHYS, 2, Dice.D6).weight(1500).nutrition(400).sound(MS.GRUNT).size(MZ.LARGE)
				.flags1(M1.CARNIVORE, M1.HUMANOID, M1.OMNIVORE, M1.REGEN)
				.flags2(M2.COLLECT, M2.HOSTILE, M2.STALK, M2.STRONG).flags3(M3.INFRAVISION, M3.INFRAVISIBLE)
				.color(CLR.MAGENTA).add();
	}

	private static void initializeUmberHulks() {

		MonsterType.of(MC.UMBER, PM.UMBER_HULK, "umber hulk").level(9).move(6).ac(2).mr(25).neutral().freq(2)
				.genocidable().attack(AT.CLAW, AD.PHYS, 3, Dice.D4).attack(AT.CLAW, AD.PHYS, 3, Dice.D4)
				.attack(AT.BITE, AD.PHYS, 2, Dice.D5).attack(AT.GAZE, AD.CONF, 0, Dice.D0).weight(1200).nutrition(500)
				.sound(MS.SILENT).size(MZ.LARGE).flags1(M1.CARNIVORE, M1.OMNIVORE, M1.TUNNEL).flags2(M2.STRONG)
				.flags3(M3.INFRAVISIBLE).color(CLR.BROWN).add();
	}

	private static void initializeVampires() {

		MonsterType.of(MC.VAMPIRE, PM.VAMPIRE, "vampire").level(10).move(12).ac(1).mr(25).chaotic(8).freq(1)
				.genocidable().noCorpse().attack(AT.CLAW, AD.PHYS, 1, Dice.D6).attack(AT.BITE, AD.DRLI, 1, Dice.D6)
				.weight(1450).nutrition(400).sound(MS.VAMPIRE).size(MZ.HUMAN).resists(MR.SLEEP, MR.POISON)
				.flags1(M1.BREATHLESS, M1.FLY, M1.HUMANOID, M1.POIS, M1.REGEN)
				.flags2(M2.HOSTILE, M2.NASTY, M2.SHAPESHIFTER, M2.STALK, M2.STRONG, M2.UNDEAD).flags3(M3.INFRAVISIBLE)
				.color(CLR.RED).add();

		MonsterType.of(MC.VAMPIRE, PM.VAMPIRE_LORD, "vampire lord").level(12).move(14).ac(0).mr(50).chaotic(9).freq(1)
				.genocidable().noCorpse().attack(AT.CLAW, AD.PHYS, 1, Dice.D8).attack(AT.BITE, AD.DRLI, 1, Dice.D8)
				.weight(1450).nutrition(400).sound(MS.VAMPIRE).size(MZ.HUMAN).resists(MR.SLEEP, MR.POISON)
				.flags1(M1.BREATHLESS, M1.FLY, M1.HUMANOID, M1.POIS, M1.REGEN)
				.flags2(M2.HOSTILE, M2.LORD, M2.MALE, M2.NASTY, M2.SHAPESHIFTER, M2.STALK, M2.STRONG, M2.UNDEAD)
				.flags3(M3.INFRAVISIBLE).color(CLR.BLUE).add();

		MonsterType.of(MC.VAMPIRE, PM.VLAD_THE_IMPALER, "Vlad the Impaler").level(14).move(18).ac(-3).mr(80).chaotic(10)
				.freq(0).unique().special().noCorpse().attack(AT.WEAP, AD.PHYS, 1, Dice.D10)
				.attack(AT.BITE, AD.DRLI, 1, Dice.D10).weight(1450).nutrition(400).sound(MS.VAMPIRE).size(MZ.HUMAN)
				.resists(MR.SLEEP, MR.POISON).flags1(M1.BREATHLESS, M1.FLY, M1.HUMANOID, M1.POIS, M1.REGEN)
				.flags2(M2.HOSTILE, M2.MALE, M2.NASTY, M2.NOPOLY, M2.PNAME, M2.PRINCE, M2.SHAPESHIFTER, M2.STALK,
						M2.STRONG, M2.UNDEAD)
				.flags3(M3.WANTSCAND, M3.WANTSALL, M3.WAITFORU, M3.COVETOUS, M3.WAITMASK, M3.INFRAVISIBLE)
				.color(CLR.MAGENTA).add();
	}

	private static void initializeWraiths() {

		MonsterType.of(MC.WRAITH, PM.BARROW_WIGHT, "barrow wight").level(3).move(12).ac(5).mr(5).chaotic(3).freq(1)
				.genocidable().noCorpse().attack(AT.WEAP, AD.DRLI, 0, Dice.D0).attack(AT.MAGC, AD.SPEL, 0, Dice.D0)
				.attack(AT.CLAW, AD.PHYS, 1, Dice.D4).weight(1200).nutrition(0).sound(MS.SPELL).size(MZ.HUMAN)
				.resists(MR.COLD, MR.SLEEP, MR.POISON).flags1(M1.BREATHLESS, M1.HUMANOID)
				.flags2(M2.COLLECT, M2.HOSTILE, M2.STALK, M2.UNDEAD).color(CLR.GRAY).add();

		MonsterType.of(MC.WRAITH, PM.WRAITH, "wraith").level(6).move(12).ac(4).mr(15).chaotic(6).freq(2).genocidable()
				.attack(AT.TUCH, AD.DRLI, 1, Dice.D6).weight(0).nutrition(0).sound(MS.SILENT).size(MZ.HUMAN)
				.resists(MR.COLD, MR.SLEEP, MR.POISON, MR.STONE).flags1(M1.BREATHLESS, M1.FLY, M1.HUMANOID, M1.UNSOLID)
				.flags2(M2.HOSTILE, M2.STALK, M2.UNDEAD).color(CLR.BLACK).add();

		MonsterType.of(MC.WRAITH, PM.NAZGUL, "Nazgul").level(13).move(12).ac(0).mr(25).chaotic(17).freq(1).genocidable()
				.noCorpse().attack(AT.WEAP, AD.DRLI, 1, Dice.D4).attack(AT.BREA, AD.SLEE, 2, Dice.D25).weight(1450)
				.nutrition(0).sound(MS.SPELL).size(MZ.HUMAN).resists(MR.COLD, MR.SLEEP, MR.POISON)
				.flags1(M1.BREATHLESS, M1.HUMANOID)
				.flags2(M2.COLLECT, M2.HOSTILE, M2.MALE, M2.NOPOLY, M2.STALK, M2.STRONG, M2.UNDEAD).color(CLR.MAGENTA)
				.add();
	}

	private static void initializeXorns() {

		MonsterType.of(MC.XORN, PM.XORN, "xorn").level(8).move(9).ac(-2).mr(20).neutral().freq(1).genocidable()
				.attack(AT.CLAW, AD.PHYS, 1, Dice.D3).attack(AT.CLAW, AD.PHYS, 1, Dice.D3)
				.attack(AT.CLAW, AD.PHYS, 1, Dice.D3).attack(AT.BITE, AD.PHYS, 4, Dice.D6).weight(1200).nutrition(700)
				.sound(MS.ROAR).size(MZ.MEDIUM).resists(MR.FIRE, MR.COLD, MR.STONE).conveys(MR.STONE)
				.flags1(M1.BREATHLESS, M1.METALLIVORE, M1.THICK_HIDE, M1.WALLWALK).flags2(M2.HOSTILE, M2.STRONG)
				.color(CLR.BROWN).add();
	}

	private static void initializeYetis() {

		MonsterType.of(MC.YETI, PM.MONKEY, "monkey").level(2).move(12).ac(6).mr(0).neutral().freq(1).genocidable()
				.attack(AT.CLAW, AD.SITM, 0, Dice.D0).attack(AT.BITE, AD.PHYS, 1, Dice.D3).weight(100).nutrition(50)
				.sound(MS.GROWL).size(MZ.SMALL).flags1(M1.ANIMAL, M1.CARNIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags3(M3.INFRAVISIBLE).color(CLR.GRAY).add();

		MonsterType.of(MC.YETI, PM.APE, "ape").level(4).move(12).ac(6).mr(0).neutral().freq(2).smallGroups()
				.genocidable().attack(AT.CLAW, AD.PHYS, 1, Dice.D3).attack(AT.CLAW, AD.PHYS, 1, Dice.D3)
				.attack(AT.BITE, AD.PHYS, 1, Dice.D6).weight(1100).nutrition(500).sound(MS.GROWL).size(MZ.LARGE)
				.flags1(M1.ANIMAL, M1.CARNIVORE, M1.HUMANOID, M1.OMNIVORE).flags2(M2.STRONG).flags3(M3.INFRAVISIBLE)
				.color(CLR.BROWN).add();

		MonsterType.of(MC.YETI, PM.OWLBEAR, "owlbear").level(5).move(12).ac(5).mr(0).neutral().freq(3).genocidable()
				.attack(AT.CLAW, AD.PHYS, 1, Dice.D6).attack(AT.CLAW, AD.PHYS, 1, Dice.D6)
				.attack(AT.HUGS, AD.PHYS, 2, Dice.D8).weight(1700).nutrition(700).sound(MS.ROAR).size(MZ.LARGE)
				.flags1(M1.ANIMAL, M1.CARNIVORE, M1.HUMANOID, M1.OMNIVORE).flags2(M2.HOSTILE, M2.NASTY, M2.STRONG)
				.flags3(M3.INFRAVISIBLE).color(CLR.BROWN).add();

		MonsterType.of(MC.YETI, PM.YETI, "yeti").level(5).move(15).ac(6).mr(0).neutral().freq(2).genocidable()
				.attack(AT.CLAW, AD.PHYS, 1, Dice.D6).attack(AT.CLAW, AD.PHYS, 1, Dice.D6)
				.attack(AT.BITE, AD.PHYS, 1, Dice.D4).weight(1600).nutrition(700).sound(MS.GROWL).size(MZ.LARGE)
				.resists(MR.COLD).conveys(MR.COLD).flags1(M1.ANIMAL, M1.CARNIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.HOSTILE, M2.STRONG).flags3(M3.INFRAVISIBLE).color(CLR.WHITE).add();

		MonsterType.of(MC.YETI, PM.CARNIVOROUS_APE, "carnivorous ape").level(6).move(12).ac(6).mr(0).neutral().freq(1)
				.genocidable().attack(AT.CLAW, AD.PHYS, 1, Dice.D4).attack(AT.CLAW, AD.PHYS, 1, Dice.D4)
				.attack(AT.HUGS, AD.PHYS, 1, Dice.D8).weight(1250).nutrition(550).sound(MS.GROWL).size(MZ.LARGE)
				.flags1(M1.ANIMAL, M1.CARNIVORE, M1.HUMANOID, M1.OMNIVORE).flags2(M2.HOSTILE, M2.STRONG)
				.flags3(M3.INFRAVISIBLE).color(CLR.BLACK).add();

		MonsterType.of(MC.YETI, PM.SASQUATCH, "sasquatch").level(7).move(15).ac(6).mr(0).lawful(2).freq(1).genocidable()
				.attack(AT.CLAW, AD.PHYS, 1, Dice.D6).attack(AT.CLAW, AD.PHYS, 1, Dice.D6)
				.attack(AT.KICK, AD.PHYS, 1, Dice.D8).weight(1550).nutrition(750).sound(MS.GROWL).size(MZ.LARGE)
				.flags1(M1.ANIMAL, M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE, M1.SEE_INVIS).flags2(M2.STRONG)
				.flags3(M3.INFRAVISIBLE).color(CLR.GRAY).add();
	}

	private static void initializeZombies() {

		MonsterType.of(MC.ZOMBIE, PM.KOBOLD_ZOMBIE, "kobold zombie").level(0).move(6).ac(10).mr(0).chaotic(2).freq(1)
				.genocidable().noCorpse().attack(AT.CLAW, AD.PHYS, 1, Dice.D4).weight(400).nutrition(50)
				.sound(MS.SILENT).size(MZ.SMALL).resists(MR.COLD, MR.SLEEP, MR.POISON)
				.flags1(M1.BREATHLESS, M1.HUMANOID, M1.MINDLESS, M1.POIS).flags2(M2.HOSTILE, M2.STALK, M2.UNDEAD)
				.flags3(M3.INFRAVISION).color(CLR.BROWN).add();

		MonsterType.of(MC.ZOMBIE, PM.GNOME_ZOMBIE, "gnome zombie").level(1).move(6).ac(10).mr(0).chaotic(2).freq(1)
				.genocidable().noCorpse().attack(AT.CLAW, AD.PHYS, 1, Dice.D5).weight(650).nutrition(50)
				.sound(MS.SILENT).size(MZ.SMALL).resists(MR.COLD, MR.SLEEP, MR.POISON)
				.flags1(M1.BREATHLESS, M1.HUMANOID, M1.MINDLESS, M1.POIS)
				.flags2(M2.GNOME, M2.HOSTILE, M2.STALK, M2.UNDEAD).flags3(M3.INFRAVISION).color(CLR.BROWN).add();

		MonsterType.of(MC.ZOMBIE, PM.ORC_ZOMBIE, "orc zombie").level(2).move(6).ac(9).mr(0).chaotic(3).freq(1)
				.smallGroups().genocidable().noCorpse().attack(AT.CLAW, AD.PHYS, 1, Dice.D6).weight(850).nutrition(75)
				.sound(MS.SILENT).size(MZ.HUMAN).resists(MR.COLD, MR.SLEEP, MR.POISON)
				.flags1(M1.BREATHLESS, M1.HUMANOID, M1.MINDLESS, M1.POIS)
				.flags2(M2.HOSTILE, M2.ORC, M2.STALK, M2.UNDEAD).flags3(M3.INFRAVISION).color(CLR.GRAY).add();

		MonsterType.of(MC.ZOMBIE, PM.DWARF_ZOMBIE, "dwarf zombie").level(2).move(6).ac(9).mr(0).chaotic(3).freq(1)
				.smallGroups().genocidable().noCorpse().attack(AT.CLAW, AD.PHYS, 1, Dice.D6).weight(900).nutrition(150)
				.sound(MS.SILENT).size(MZ.HUMAN).resists(MR.COLD, MR.SLEEP, MR.POISON)
				.flags1(M1.BREATHLESS, M1.HUMANOID, M1.MINDLESS, M1.POIS)
				.flags2(M2.DWARF, M2.HOSTILE, M2.STALK, M2.UNDEAD).flags3(M3.INFRAVISION).color(CLR.RED).add();

		MonsterType.of(MC.ZOMBIE, PM.ELF_ZOMBIE, "elf zombie").level(3).move(6).ac(9).mr(0).chaotic(3).freq(1)
				.smallGroups().genocidable().noCorpse().attack(AT.CLAW, AD.PHYS, 1, Dice.D7).weight(800).nutrition(175)
				.sound(MS.SILENT).size(MZ.HUMAN).resists(MR.COLD, MR.SLEEP, MR.POISON)
				.flags1(M1.BREATHLESS, M1.HUMANOID, M1.MINDLESS).flags2(M2.ELF, M2.HOSTILE, M2.STALK, M2.UNDEAD)
				.flags3(M3.INFRAVISION).color(CLR.GREEN).add();

		MonsterType.of(MC.ZOMBIE, PM.HUMAN_ZOMBIE, "human zombie").level(4).move(6).ac(8).mr(0).chaotic(3).freq(1)
				.smallGroups().genocidable().noCorpse().attack(AT.CLAW, AD.PHYS, 1, Dice.D8).weight(1450).nutrition(200)
				.sound(MS.SILENT).size(MZ.HUMAN).resists(MR.COLD, MR.SLEEP, MR.POISON)
				.flags1(M1.BREATHLESS, M1.HUMANOID, M1.MINDLESS).flags2(M2.HOSTILE, M2.STALK, M2.UNDEAD)
				.flags3(M3.INFRAVISION).color(CLR.WHITE).add();

		MonsterType.of(MC.ZOMBIE, PM.ETTIN_ZOMBIE, "ettin zombie").level(6).move(8).ac(6).mr(0).chaotic(4).freq(1)
				.genocidable().noCorpse().attack(AT.CLAW, AD.PHYS, 1, Dice.D10).attack(AT.CLAW, AD.PHYS, 1, Dice.D10)
				.weight(1700).nutrition(250).sound(MS.SILENT).size(MZ.HUGE).resists(MR.COLD, MR.SLEEP, MR.POISON)
				.flags1(M1.BREATHLESS, M1.HUMANOID, M1.MINDLESS).flags2(M2.HOSTILE, M2.STALK, M2.STRONG, M2.UNDEAD)
				.flags3(M3.INFRAVISION).color(CLR.BLUE).add();

		MonsterType.of(MC.ZOMBIE, PM.GHOUL, "ghoul").level(3).move(6).ac(10).mr(0).chaotic(2).freq(1).genocidable()
				.noCorpse().attack(AT.CLAW, AD.PLYS, 1, Dice.D2).attack(AT.CLAW, AD.PHYS, 1, Dice.D3).weight(400)
				.nutrition(50).sound(MS.SILENT).size(MZ.SMALL).resists(MR.COLD, MR.SLEEP, MR.POISON)
				.flags1(M1.BREATHLESS, M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.MINDLESS, M1.OMNIVORE, M1.POIS)
				.flags2(M2.HOSTILE, M2.UNDEAD, M2.WANDER).flags3(M3.INFRAVISION).color(CLR.BLACK).add();

		MonsterType.of(MC.ZOMBIE, PM.GIANT_ZOMBIE, "giant zombie").level(8).move(8).ac(6).mr(0).chaotic(4).freq(1)
				.genocidable().noCorpse().attack(AT.CLAW, AD.PHYS, 2, Dice.D8).attack(AT.CLAW, AD.PHYS, 2, Dice.D8)
				.weight(2050).nutrition(375).sound(MS.SILENT).size(MZ.HUGE).resists(MR.COLD, MR.SLEEP, MR.POISON)
				.flags1(M1.BREATHLESS, M1.HUMANOID, M1.MINDLESS)
				.flags2(M2.GIANT, M2.HOSTILE, M2.STALK, M2.STRONG, M2.UNDEAD).flags3(M3.INFRAVISION).color(CLR.CYAN)
				.add();

		MonsterType.of(MC.ZOMBIE, PM.SKELETON, "skeleton").level(12).move(8).ac(4).mr(0).neutral().freq(0).special()
				.noCorpse().attack(AT.WEAP, AD.PHYS, 2, Dice.D6).attack(AT.TUCH, AD.SLOW, 1, Dice.D6).weight(300)
				.nutrition(5).sound(MS.BONES).size(MZ.HUMAN).resists(MR.COLD, MR.SLEEP, MR.POISON, MR.STONE)
				.flags1(M1.BREATHLESS, M1.HUMANOID, M1.MINDLESS, M1.THICK_HIDE)
				.flags2(M2.COLLECT, M2.HOSTILE, M2.NASTY, M2.STRONG, M2.UNDEAD, M2.WANDER).flags3(M3.INFRAVISION)
				.color(CLR.WHITE).add();
	}

	private static void initializeGolems() {

		MonsterType.of(MC.GOLEM, PM.STRAW_GOLEM, "straw golem").level(3).move(12).ac(10).mr(0).neutral().freq(1)
				.noCorpse().attack(AT.CLAW, AD.PHYS, 1, Dice.D2).attack(AT.CLAW, AD.PHYS, 1, Dice.D2).weight(400)
				.nutrition(0).sound(MS.SILENT).size(MZ.LARGE).resists(MR.COLD, MR.SLEEP, MR.POISON)
				.flags1(M1.BREATHLESS, M1.HUMANOID, M1.MINDLESS).flags2(M2.HOSTILE, M2.NEUTER).color(CLR.YELLOW).add();

		MonsterType.of(MC.GOLEM, PM.PAPER_GOLEM, "paper golem").level(3).move(12).ac(10).mr(0).neutral().freq(1)
				.noCorpse().attack(AT.CLAW, AD.PHYS, 1, Dice.D3).weight(400).nutrition(0).sound(MS.SILENT)
				.size(MZ.LARGE).resists(MR.COLD, MR.SLEEP, MR.POISON).flags1(M1.BREATHLESS, M1.HUMANOID, M1.MINDLESS)
				.flags2(M2.HOSTILE, M2.NEUTER).color(CLR.WHITE).add();

		MonsterType.of(MC.GOLEM, PM.ROPE_GOLEM, "rope golem").level(4).move(9).ac(8).mr(0).neutral().freq(1).noCorpse()
				.attack(AT.CLAW, AD.PHYS, 1, Dice.D4).attack(AT.CLAW, AD.PHYS, 1, Dice.D4)
				.attack(AT.HUGS, AD.PHYS, 6, Dice.D1).weight(450).nutrition(0).sound(MS.SILENT).size(MZ.LARGE)
				.resists(MR.SLEEP, MR.POISON).flags1(M1.BREATHLESS, M1.HUMANOID, M1.MINDLESS)
				.flags2(M2.HOSTILE, M2.NEUTER).color(CLR.BROWN).add();

		MonsterType.of(MC.GOLEM, PM.GOLD_GOLEM, "gold golem").level(5).move(9).ac(6).mr(0).neutral().freq(1).noCorpse()
				.attack(AT.CLAW, AD.PHYS, 2, Dice.D3).attack(AT.CLAW, AD.PHYS, 2, Dice.D3).weight(450).nutrition(0)
				.sound(MS.SILENT).size(MZ.LARGE).resists(MR.SLEEP, MR.POISON, MR.ACID)
				.flags1(M1.BREATHLESS, M1.HUMANOID, M1.MINDLESS, M1.THICK_HIDE).flags2(M2.HOSTILE, M2.NEUTER)
				.color(CLR.GOLD).add();

		MonsterType.of(MC.GOLEM, PM.LEATHER_GOLEM, "leather golem").level(6).move(6).ac(6).mr(0).neutral().freq(1)
				.noCorpse().attack(AT.CLAW, AD.PHYS, 1, Dice.D6).attack(AT.CLAW, AD.PHYS, 1, Dice.D6).weight(800)
				.nutrition(0).sound(MS.SILENT).size(MZ.LARGE).resists(MR.SLEEP, MR.POISON)
				.flags1(M1.BREATHLESS, M1.HUMANOID, M1.MINDLESS).flags2(M2.HOSTILE, M2.NEUTER).color(CLR.BROWN).add();

		MonsterType.of(MC.GOLEM, PM.WOOD_GOLEM, "wood golem").level(7).move(3).ac(4).mr(0).neutral().freq(1).noCorpse()
				.attack(AT.CLAW, AD.PHYS, 3, Dice.D4).weight(900).nutrition(0).sound(MS.SILENT).size(MZ.LARGE)
				.resists(MR.COLD, MR.SLEEP, MR.POISON).flags1(M1.BREATHLESS, M1.HUMANOID, M1.MINDLESS, M1.THICK_HIDE)
				.flags2(M2.HOSTILE, M2.NEUTER).color(CLR.BROWN).add();

		MonsterType.of(MC.GOLEM, PM.FLESH_GOLEM, "flesh golem").level(9).move(8).ac(9).mr(30).neutral().freq(1)
				.attack(AT.CLAW, AD.PHYS, 2, Dice.D8).attack(AT.CLAW, AD.PHYS, 2, Dice.D8).weight(1400).nutrition(600)
				.sound(MS.SILENT).size(MZ.LARGE).resists(MR.FIRE, MR.COLD, MR.SLEEP, MR.ELEC, MR.POISON)
				.conveys(MR.FIRE, MR.COLD, MR.SLEEP, MR.ELEC, MR.POISON).flags1(M1.BREATHLESS, M1.HUMANOID, M1.MINDLESS)
				.flags2(M2.HOSTILE, M2.STRONG).color(CLR.RED).add();

		MonsterType.of(MC.GOLEM, PM.CLAY_GOLEM, "clay golem").level(11).move(7).ac(7).mr(40).neutral().freq(1)
				.noCorpse().attack(AT.CLAW, AD.PHYS, 3, Dice.D10).weight(1550).nutrition(0).sound(MS.SILENT)
				.size(MZ.LARGE).resists(MR.SLEEP, MR.POISON)
				.flags1(M1.BREATHLESS, M1.HUMANOID, M1.MINDLESS, M1.THICK_HIDE).flags2(M2.HOSTILE, M2.STRONG)
				.color(CLR.BROWN).add();

		MonsterType.of(MC.GOLEM, PM.STONE_GOLEM, "stone golem").level(14).move(6).ac(5).mr(50).neutral().freq(1)
				.noCorpse().attack(AT.CLAW, AD.PHYS, 3, Dice.D8).weight(1900).nutrition(0).sound(MS.SILENT)
				.size(MZ.LARGE).resists(MR.SLEEP, MR.POISON, MR.STONE)
				.flags1(M1.BREATHLESS, M1.HUMANOID, M1.MINDLESS, M1.THICK_HIDE).flags2(M2.HOSTILE, M2.STRONG)
				.color(CLR.GRAY).add();

		MonsterType.of(MC.GOLEM, PM.GLASS_GOLEM, "glass golem").level(16).move(6).ac(1).mr(50).neutral().freq(1)
				.noCorpse().attack(AT.CLAW, AD.PHYS, 2, Dice.D8).attack(AT.CLAW, AD.PHYS, 2, Dice.D8).weight(1800)
				.nutrition(0).sound(MS.SILENT).size(MZ.LARGE).resists(MR.SLEEP, MR.POISON, MR.ACID)
				.flags1(M1.BREATHLESS, M1.HUMANOID, M1.MINDLESS, M1.THICK_HIDE).flags2(M2.HOSTILE, M2.STRONG)
				.color(CLR.CYAN).add();

		MonsterType.of(MC.GOLEM, PM.IRON_GOLEM, "iron golem").level(18).move(6).ac(3).mr(60).neutral().freq(1)
				.noCorpse().attack(AT.WEAP, AD.PHYS, 4, Dice.D10).attack(AT.BREA, AD.DRST, 4, Dice.D6).weight(2000)
				.nutrition(0).sound(MS.SILENT).size(MZ.LARGE).resists(MR.FIRE, MR.COLD, MR.SLEEP, MR.ELEC, MR.POISON)
				.flags1(M1.BREATHLESS, M1.HUMANOID, M1.MINDLESS, M1.POIS, M1.THICK_HIDE)
				.flags2(M2.COLLECT, M2.HOSTILE, M2.STRONG).color(CLR.CYAN).add();
	}

	private static void initializeHumans() {

		MonsterType.of(MC.HUMAN, PM.HUMAN, "human").level(0).move(12).ac(10).mr(0).neutral().freq(0).special()
				.attack(AT.WEAP, AD.PHYS, 1, Dice.D6).weight(1450).nutrition(400).sound(MS.HUMANOID).size(MZ.HUMAN)
				.flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.HUMAN, M2.NOPOLY, M2.STRONG).flags3(M3.INFRAVISIBLE).color(CLR.WHITE).add();

		MonsterType.of(MC.HUMAN, PM.WERERAT, "wererat").level(2).move(12).ac(10).mr(10).chaotic(7).freq(1)
				.attack(AT.WEAP, AD.PHYS, 2, Dice.D4).weight(1450).nutrition(400).sound(MS.WERE).size(MZ.HUMAN)
				.resists(MR.POISON).flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE, M1.POIS, M1.REGEN)
				.flags2(M2.COLLECT, M2.HOSTILE, M2.HUMAN, M2.NOPOLY, M2.WERE).flags3(M3.INFRAVISIBLE).color(CLR.BROWN)
				.add();

		MonsterType.of(MC.HUMAN, PM.WEREJACKAL, "werejackal").level(2).move(12).ac(10).mr(10).chaotic(7).freq(1)
				.attack(AT.WEAP, AD.PHYS, 2, Dice.D4).weight(1450).nutrition(400).sound(MS.WERE).size(MZ.HUMAN)
				.resists(MR.POISON).flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE, M1.POIS, M1.REGEN)
				.flags2(M2.COLLECT, M2.HOSTILE, M2.HUMAN, M2.NOPOLY, M2.WERE).flags3(M3.INFRAVISIBLE).color(CLR.RED)
				.add();

		MonsterType.of(MC.HUMAN, PM.WEREWOLF, "werewolf").level(5).move(12).ac(10).mr(20).chaotic(7).freq(1)
				.attack(AT.WEAP, AD.PHYS, 2, Dice.D4).weight(1450).nutrition(400).sound(MS.WERE).size(MZ.HUMAN)
				.resists(MR.POISON).flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE, M1.POIS, M1.REGEN)
				.flags2(M2.COLLECT, M2.HOSTILE, M2.HUMAN, M2.NOPOLY, M2.WERE).flags3(M3.INFRAVISIBLE).color(CLR.ORANGE)
				.add();

		MonsterType.of(MC.HUMAN, PM.ELF, "elf").level(10).move(12).ac(10).mr(2).chaotic(3).freq(0).special()
				.attack(AT.WEAP, AD.PHYS, 1, Dice.D8).weight(800).nutrition(350).sound(MS.HUMANOID).size(MZ.HUMAN)
				.resists(MR.SLEEP).conveys(MR.SLEEP)
				.flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE, M1.SEE_INVIS)
				.flags2(M2.COLLECT, M2.ELF, M2.NOPOLY, M2.STRONG).flags3(M3.INFRAVISION, M3.INFRAVISIBLE)
				.color(CLR.WHITE).add();

		MonsterType.of(MC.HUMAN, PM.WOODLAND_ELF, "Woodland-elf").level(4).move(12).ac(10).mr(10).chaotic(5).freq(2)
				.smallGroups().genocidable().attack(AT.WEAP, AD.PHYS, 2, Dice.D4).weight(800).nutrition(350)
				.sound(MS.HUMANOID).size(MZ.HUMAN).resists(MR.SLEEP).conveys(MR.SLEEP)
				.flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE, M1.SEE_INVIS).flags2(M2.COLLECT, M2.ELF)
				.flags3(M3.INFRAVISION, M3.INFRAVISIBLE).color(CLR.GREEN).add();

		MonsterType.of(MC.HUMAN, PM.GREEN_ELF, "Green-elf").level(5).move(12).ac(10).mr(10).chaotic(6).freq(2)
				.smallGroups().genocidable().attack(AT.WEAP, AD.PHYS, 2, Dice.D4).weight(800).nutrition(350)
				.sound(MS.HUMANOID).size(MZ.HUMAN).resists(MR.SLEEP).conveys(MR.SLEEP)
				.flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE, M1.SEE_INVIS).flags2(M2.COLLECT, M2.ELF)
				.flags3(M3.INFRAVISION, M3.INFRAVISIBLE).color(CLR.BRIGHT_GREEN).add();

		MonsterType.of(MC.HUMAN, PM.GREY_ELF, "Grey-elf").level(6).move(12).ac(10).mr(10).chaotic(7).freq(2)
				.smallGroups().genocidable().attack(AT.WEAP, AD.PHYS, 2, Dice.D4).weight(800).nutrition(350)
				.sound(MS.HUMANOID).size(MZ.HUMAN).resists(MR.SLEEP).conveys(MR.SLEEP)
				.flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE, M1.SEE_INVIS).flags2(M2.COLLECT, M2.ELF)
				.flags3(M3.INFRAVISION, M3.INFRAVISIBLE).color(CLR.GRAY).add();

		MonsterType.of(MC.HUMAN, PM.ELF_LORD, "elf-lord").level(8).move(12).ac(10).mr(20).chaotic(9).freq(2)
				.smallGroups().genocidable().attack(AT.WEAP, AD.PHYS, 2, Dice.D4).attack(AT.WEAP, AD.PHYS, 2, Dice.D4)
				.weight(800).nutrition(350).sound(MS.HUMANOID).size(MZ.HUMAN).resists(MR.SLEEP).conveys(MR.SLEEP)
				.flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE, M1.SEE_INVIS)
				.flags2(M2.COLLECT, M2.ELF, M2.LORD, M2.MALE, M2.STRONG).flags3(M3.INFRAVISION, M3.INFRAVISIBLE)
				.color(CLR.BRIGHT_BLUE).add();

		MonsterType.of(MC.HUMAN, PM.ELVENKING, "Elvenking").level(9).move(12).ac(10).mr(25).chaotic(10).freq(1)
				.genocidable().attack(AT.WEAP, AD.PHYS, 2, Dice.D4).attack(AT.WEAP, AD.PHYS, 2, Dice.D4).weight(800)
				.nutrition(350).sound(MS.HUMANOID).size(MZ.HUMAN).resists(MR.SLEEP).conveys(MR.SLEEP)
				.flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE, M1.SEE_INVIS)
				.flags2(M2.COLLECT, M2.ELF, M2.MALE, M2.PRINCE, M2.STRONG).flags3(M3.INFRAVISION, M3.INFRAVISIBLE)
				.color(CLR.MAGENTA).add();

		MonsterType.of(MC.HUMAN, PM.DOPPELGANGER, "doppelganger").level(9).move(12).ac(5).mr(20).neutral().freq(1)
				.genocidable().attack(AT.WEAP, AD.PHYS, 1, Dice.D12).weight(1450).nutrition(400).sound(MS.IMITATE)
				.size(MZ.HUMAN).resists(MR.SLEEP).flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.HOSTILE, M2.HUMAN, M2.NOPOLY, M2.SHAPESHIFTER, M2.STRONG).flags3(M3.INFRAVISIBLE)
				.color(CLR.WHITE).add();

		MonsterType.of(MC.HUMAN, PM.SHOPKEEPER, "shopkeeper").level(12).move(18).ac(0).mr(50).neutral().freq(0)
				.special().attack(AT.WEAP, AD.PHYS, 4, Dice.D4).attack(AT.WEAP, AD.PHYS, 4, Dice.D4).weight(1450)
				.nutrition(400).sound(MS.SELL).size(MZ.HUMAN)
				.flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.HUMAN, M2.MAGIC, M2.NOPOLY, M2.PEACEFUL, M2.STRONG).flags3(M3.INFRAVISIBLE)
				.color(CLR.WHITE).add();

		MonsterType.of(MC.HUMAN, PM.GUARD, "guard").level(12).move(12).ac(10).mr(40).lawful(10).freq(0).special()
				.attack(AT.WEAP, AD.PHYS, 4, Dice.D10).weight(1450).nutrition(400).sound(MS.GUARD).size(MZ.HUMAN)
				.flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.HUMAN, M2.MERC, M2.NOPOLY, M2.PEACEFUL, M2.STRONG).flags3(M3.INFRAVISIBLE)
				.color(CLR.BLUE).add();

		MonsterType.of(MC.HUMAN, PM.PRISONER, "prisoner").level(12).move(12).ac(10).mr(0).neutral().freq(0).special()
				.attack(AT.WEAP, AD.PHYS, 1, Dice.D6).weight(1450).nutrition(400).sound(MS.DJINNI).size(MZ.HUMAN)
				.flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.HUMAN, M2.NOPOLY, M2.PEACEFUL, M2.STRONG)
				.flags3(M3.CLOSE, M3.WAITMASK, M3.INFRAVISIBLE).color(CLR.WHITE).add();

		MonsterType.of(MC.HUMAN, PM.ORACLE, "Oracle").level(12).move(0).ac(0).mr(50).neutral().freq(0).unique()
				.special().weight(1450).nutrition(400).sound(MS.ORACLE).size(MZ.HUMAN)
				.flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.FEMALE, M2.HUMAN, M2.NOPOLY, M2.PEACEFUL).flags3(M3.INFRAVISIBLE).color(CLR.BRIGHT_BLUE)
				.add();

		MonsterType.of(MC.HUMAN, PM.ALIGNED_PRIEST, "aligned priest").level(12).move(12).ac(10).mr(50).neutral().freq(0)
				.special().attack(AT.WEAP, AD.PHYS, 4, Dice.D10).attack(AT.KICK, AD.PHYS, 1, Dice.D4)
				.attack(AT.MAGC, AD.CLRC, 0, Dice.D0).weight(1450).nutrition(400).sound(MS.PRIEST).size(MZ.HUMAN)
				.resists(MR.ELEC).flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.HUMAN, M2.LORD, M2.NOPOLY, M2.PEACEFUL).flags3(M3.INFRAVISIBLE).color(CLR.WHITE)
				.add();

		MonsterType.of(MC.HUMAN, PM.HIGH_PRIEST, "high priest").level(25).move(15).ac(7).mr(70).neutral().freq(0)
				.unique().special().attack(AT.WEAP, AD.PHYS, 4, Dice.D10).attack(AT.KICK, AD.PHYS, 2, Dice.D8)
				.attack(AT.MAGC, AD.CLRC, 2, Dice.D8).attack(AT.MAGC, AD.CLRC, 2, Dice.D8).weight(1450).nutrition(400)
				.sound(MS.PRIEST).size(MZ.HUMAN).resists(MR.FIRE, MR.SLEEP, MR.ELEC, MR.POISON)
				.flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE, M1.SEE_INVIS)
				.flags2(M2.COLLECT, M2.HUMAN, M2.MAGIC, M2.MINION, M2.NASTY, M2.NOPOLY, M2.PRINCE)
				.flags3(M3.INFRAVISIBLE).color(CLR.WHITE).add();

		MonsterType.of(MC.HUMAN, PM.SOLDIER, "soldier").level(6).move(10).ac(10).mr(0).chaotic(2).freq(1).smallGroups()
				.genocidable().attack(AT.WEAP, AD.PHYS, 1, Dice.D8).weight(1450).nutrition(400).sound(MS.SOLDIER)
				.size(MZ.HUMAN).flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.HOSTILE, M2.HUMAN, M2.MERC, M2.NOPOLY, M2.STALK, M2.STRONG)
				.flags3(M3.INFRAVISIBLE).color(CLR.GRAY).add();

		MonsterType.of(MC.HUMAN, PM.SERGEANT, "sergeant").level(8).move(10).ac(10).mr(5).chaotic(3).freq(1)
				.smallGroups().genocidable().attack(AT.WEAP, AD.PHYS, 2, Dice.D6).weight(1450).nutrition(400)
				.sound(MS.SOLDIER).size(MZ.HUMAN).flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.HOSTILE, M2.HUMAN, M2.MERC, M2.NOPOLY, M2.STALK, M2.STRONG)
				.flags3(M3.INFRAVISIBLE).color(CLR.RED).add();

		MonsterType.of(MC.HUMAN, PM.NURSE, "nurse").level(11).move(6).ac(0).mr(0).neutral().freq(3).genocidable()
				.attack(AT.CLAW, AD.HEAL, 2, Dice.D6).weight(1450).nutrition(400).sound(MS.NURSE).size(MZ.HUMAN)
				.resists(MR.POISON).conveys(MR.POISON).flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.HOSTILE, M2.HUMAN, M2.NOPOLY).flags3(M3.INFRAVISIBLE).color(CLR.WHITE).add();

		MonsterType.of(MC.HUMAN, PM.LIEUTENANT, "lieutenant").level(10).move(10).ac(10).mr(15).chaotic(4).freq(1)
				.genocidable().attack(AT.WEAP, AD.PHYS, 3, Dice.D4).attack(AT.WEAP, AD.PHYS, 3, Dice.D4).weight(1450)
				.nutrition(400).sound(MS.SOLDIER).size(MZ.HUMAN)
				.flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.HOSTILE, M2.HUMAN, M2.MERC, M2.NOPOLY, M2.STALK, M2.STRONG)
				.flags3(M3.INFRAVISIBLE).color(CLR.GREEN).add();

		MonsterType.of(MC.HUMAN, PM.CAPTAIN, "captain").level(12).move(10).ac(10).mr(15).chaotic(5).freq(1)
				.genocidable().attack(AT.WEAP, AD.PHYS, 4, Dice.D4).attack(AT.WEAP, AD.PHYS, 4, Dice.D4).weight(1450)
				.nutrition(400).sound(MS.SOLDIER).size(MZ.HUMAN)
				.flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.HOSTILE, M2.HUMAN, M2.MERC, M2.NOPOLY, M2.STALK, M2.STRONG)
				.flags3(M3.INFRAVISIBLE).color(CLR.BLUE).add();

		MonsterType.of(MC.HUMAN, PM.WATCHMAN, "watchman").level(6).move(10).ac(10).mr(0).chaotic(2).freq(1).special()
				.smallGroups().genocidable().attack(AT.WEAP, AD.PHYS, 1, Dice.D8).weight(1450).nutrition(400)
				.sound(MS.SOLDIER).size(MZ.HUMAN).flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.HUMAN, M2.MERC, M2.NOPOLY, M2.PEACEFUL, M2.STALK, M2.STRONG)
				.flags3(M3.INFRAVISIBLE).color(CLR.GRAY).add();

		MonsterType.of(MC.HUMAN, PM.WATCH_CAPTAIN, "watch captain").level(10).move(10).ac(10).mr(15).chaotic(4).freq(1)
				.special().genocidable().attack(AT.WEAP, AD.PHYS, 3, Dice.D4).attack(AT.WEAP, AD.PHYS, 3, Dice.D4)
				.weight(1450).nutrition(400).sound(MS.SOLDIER).size(MZ.HUMAN)
				.flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.HUMAN, M2.MERC, M2.NOPOLY, M2.PEACEFUL, M2.STALK, M2.STRONG)
				.flags3(M3.INFRAVISIBLE).color(CLR.GREEN).add();

		MonsterType.of(MC.HUMAN, PM.MEDUSA, "Medusa").level(20).move(12).ac(2).mr(50).chaotic(15).freq(0).unique()
				.special().attack(AT.WEAP, AD.PHYS, 2, Dice.D4).attack(AT.CLAW, AD.PHYS, 1, Dice.D8)
				.attack(AT.GAZE, AD.STON, 0, Dice.D0).attack(AT.BITE, AD.DRST, 1, Dice.D6).weight(1450).nutrition(400)
				.sound(MS.HISS).size(MZ.LARGE).resists(MR.POISON, MR.STONE).conveys(MR.POISON, MR.STONE)
				.flags1(M1.AMPHIBIOUS, M1.CARNIVORE, M1.FLY, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE, M1.POIS, M1.SWIM)
				.flags2(M2.FEMALE, M2.HOSTILE, M2.NOPOLY, M2.PNAME, M2.STRONG)
				.flags3(M3.WAITFORU, M3.WAITMASK, M3.INFRAVISIBLE).color(CLR.BRIGHT_GREEN).add();

		MonsterType.of(MC.HUMAN, PM.WIZARD_OF_YENDOR, "Wizard of Yendor").level(30).move(12).ac(-8).mr(100).chaotic(128)
				.freq(0).unique().special().attack(AT.CLAW, AD.SAMU, 2, Dice.D12).attack(AT.MAGC, AD.SPEL, 0, Dice.D0)
				.weight(1450).nutrition(400).sound(MS.CUSS).size(MZ.HUMAN).resists(MR.FIRE, MR.POISON)
				.conveys(MR.FIRE, MR.POISON)
				.flags1(M1.BREATHLESS, M1.CARNIVORE, M1.FLY, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE, M1.REGEN,
						M1.SEE_INVIS, M1.TPORT, M1.TPORT_CNTRL)
				.flags2(M2.HOSTILE, M2.HUMAN, M2.MAGIC, M2.MALE, M2.NASTY, M2.NOPOLY, M2.PRINCE, M2.STRONG)
				.flags3(M3.WANTSAMUL, M3.WANTSBELL, M3.WANTSBOOK, M3.WANTSCAND, M3.WANTSARTI, M3.WANTSALL, M3.WAITFORU,
						M3.COVETOUS, M3.WAITMASK, M3.INFRAVISIBLE)
				.color(CLR.MAGENTA).add();

		MonsterType.of(MC.HUMAN, PM.CROESUS, "Croesus").level(20).move(15).ac(0).mr(40).lawful(15).freq(0).unique()
				.special().attack(AT.WEAP, AD.PHYS, 4, Dice.D10).weight(1450).nutrition(400).sound(MS.GUARD)
				.size(MZ.HUMAN).flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE, M1.SEE_INVIS)
				.flags2(M2.COLLECT, M2.GREEDY, M2.HOSTILE, M2.HUMAN, M2.JEWELS, M2.MAGIC, M2.MALE, M2.NASTY, M2.NOPOLY,
						M2.PNAME, M2.PRINCE, M2.STALK, M2.STRONG)
				.flags3(M3.INFRAVISIBLE).color(CLR.MAGENTA).add();
	}

	private static void initializeGhosts() {

		MonsterType.of(MC.GHOST, PM.GHOST, "ghost").level(10).move(3).ac(-5).mr(50).chaotic(5).freq(0).special()
				.noCorpse().attack(AT.TUCH, AD.PHYS, 1, Dice.D1).weight(1450).nutrition(0).sound(MS.SILENT)
				.size(MZ.HUMAN).resists(MR.COLD, MR.SLEEP, MR.DISINT, MR.POISON, MR.STONE)
				.flags1(M1.BREATHLESS, M1.FLY, M1.HUMANOID, M1.UNSOLID, M1.WALLWALK)
				.flags2(M2.HOSTILE, M2.NOPOLY, M2.STALK, M2.UNDEAD).flags3(M3.INFRAVISION).color(CLR.GRAY).add();

		MonsterType.of(MC.GHOST, PM.SHADE, "shade").level(12).move(10).ac(10).mr(0).neutral().freq(0).special()
				.noCorpse().attack(AT.TUCH, AD.PLYS, 2, Dice.D6).attack(AT.TUCH, AD.SLOW, 1, Dice.D6).weight(1450)
				.nutrition(0).sound(MS.WAIL).size(MZ.HUMAN).resists(MR.COLD, MR.SLEEP, MR.DISINT, MR.POISON, MR.STONE)
				.flags1(M1.BREATHLESS, M1.FLY, M1.HUMANOID, M1.SEE_INVIS, M1.UNSOLID, M1.WALLWALK)
				.flags2(M2.HOSTILE, M2.NASTY, M2.NOPOLY, M2.STALK, M2.UNDEAD, M2.WANDER).flags3(M3.INFRAVISION)
				.color(CLR.BLACK).add();
	}

	private static void initializeDemons() {

		MonsterType.of(MC.DEMON, PM.WATER_DEMON, "water demon").level(8).move(12).ac(-4).mr(30).chaotic(7).freq(0)
				.special().noCorpse().attack(AT.WEAP, AD.PHYS, 1, Dice.D3).attack(AT.CLAW, AD.PHYS, 1, Dice.D3)
				.attack(AT.BITE, AD.PHYS, 1, Dice.D3).weight(1450).nutrition(400).sound(MS.DJINNI).size(MZ.HUMAN)
				.resists(MR.FIRE, MR.POISON).flags1(M1.HUMANOID, M1.POIS, M1.SWIM)
				.flags2(M2.COLLECT, M2.DEMON, M2.HOSTILE, M2.NASTY, M2.NOPOLY, M2.STALK)
				.flags3(M3.INFRAVISION, M3.INFRAVISIBLE).color(CLR.BLUE).add();

		MonsterType.of(MC.DEMON, PM.SUCCUBUS, "succubus").level(6).move(12).ac(0).mr(70).chaotic(9).freq(1).noCorpse()
				.attack(AT.BITE, AD.SSEX, 0, Dice.D0).attack(AT.CLAW, AD.PHYS, 1, Dice.D3)
				.attack(AT.CLAW, AD.PHYS, 1, Dice.D3).weight(1450).nutrition(400).sound(MS.SEDUCE).size(MZ.HUMAN)
				.resists(MR.FIRE, MR.POISON).flags1(M1.FLY, M1.HUMANOID, M1.POIS)
				.flags2(M2.DEMON, M2.FEMALE, M2.HOSTILE, M2.NASTY, M2.STALK).flags3(M3.INFRAVISION, M3.INFRAVISIBLE)
				.color(CLR.GRAY).add();

		MonsterType.of(MC.DEMON, PM.HORNED_DEVIL, "horned devil").level(6).move(9).ac(-5).mr(50).lawful(11).freq(2)
				.onlyInHell().noCorpse().attack(AT.WEAP, AD.PHYS, 1, Dice.D4).attack(AT.CLAW, AD.PHYS, 1, Dice.D4)
				.attack(AT.BITE, AD.PHYS, 2, Dice.D3).attack(AT.STNG, AD.PHYS, 1, Dice.D3).weight(1450).nutrition(400)
				.sound(MS.SILENT).size(MZ.HUMAN).resists(MR.FIRE, MR.POISON).flags1(M1.POIS, M1.THICK_HIDE)
				.flags2(M2.DEMON, M2.HOSTILE, M2.NASTY, M2.STALK).flags3(M3.INFRAVISION, M3.INFRAVISIBLE)
				.color(CLR.BROWN).add();

		MonsterType.of(MC.DEMON, PM.INCUBUS, "incubus").level(6).move(12).ac(0).mr(70).chaotic(9).freq(1).noCorpse()
				.attack(AT.BITE, AD.SSEX, 0, Dice.D0).attack(AT.CLAW, AD.PHYS, 1, Dice.D3)
				.attack(AT.CLAW, AD.PHYS, 1, Dice.D3).weight(1450).nutrition(400).sound(MS.SEDUCE).size(MZ.HUMAN)
				.resists(MR.FIRE, MR.POISON).flags1(M1.FLY, M1.HUMANOID, M1.POIS)
				.flags2(M2.DEMON, M2.HOSTILE, M2.MALE, M2.NASTY, M2.STALK).flags3(M3.INFRAVISION, M3.INFRAVISIBLE)
				.color(CLR.GRAY).add();

		MonsterType.of(MC.DEMON, PM.ERINYS, "erinys").level(7).move(12).ac(2).mr(30).lawful(10).freq(2).onlyInHell()
				.smallGroups().noCorpse().attack(AT.WEAP, AD.DRST, 2, Dice.D4).weight(1450).nutrition(400)
				.sound(MS.SILENT).size(MZ.HUMAN).resists(MR.FIRE, MR.POISON).flags1(M1.HUMANOID, M1.POIS)
				.flags2(M2.COLLECT, M2.DEMON, M2.FEMALE, M2.HOSTILE, M2.NASTY, M2.NOPOLY, M2.STALK, M2.STRONG)
				.flags3(M3.INFRAVISION, M3.INFRAVISIBLE).color(CLR.RED).add();

		MonsterType.of(MC.DEMON, PM.BARBED_DEVIL, "barbed devil").level(8).move(12).ac(0).mr(35).lawful(8).freq(2)
				.onlyInHell().smallGroups().noCorpse().attack(AT.CLAW, AD.PHYS, 2, Dice.D4)
				.attack(AT.CLAW, AD.PHYS, 2, Dice.D4).attack(AT.STNG, AD.PHYS, 3, Dice.D4).weight(1450).nutrition(400)
				.sound(MS.SILENT).size(MZ.HUMAN).resists(MR.FIRE, MR.POISON).flags1(M1.POIS, M1.THICK_HIDE)
				.flags2(M2.DEMON, M2.HOSTILE, M2.NASTY, M2.STALK).flags3(M3.INFRAVISION, M3.INFRAVISIBLE).color(CLR.RED)
				.add();

		MonsterType.of(MC.DEMON, PM.MARILITH, "marilith").level(7).move(12).ac(-6).mr(80).chaotic(12).freq(1)
				.onlyInHell().noCorpse().attack(AT.WEAP, AD.PHYS, 2, Dice.D4).attack(AT.WEAP, AD.PHYS, 2, Dice.D4)
				.attack(AT.CLAW, AD.PHYS, 2, Dice.D4).attack(AT.CLAW, AD.PHYS, 2, Dice.D4)
				.attack(AT.CLAW, AD.PHYS, 2, Dice.D4).attack(AT.CLAW, AD.PHYS, 2, Dice.D4).weight(1450).nutrition(400)
				.sound(MS.CUSS).size(MZ.LARGE).resists(MR.FIRE, MR.POISON)
				.flags1(M1.HUMANOID, M1.POIS, M1.SEE_INVIS, M1.SLITHY)
				.flags2(M2.COLLECT, M2.DEMON, M2.FEMALE, M2.HOSTILE, M2.NASTY, M2.STALK)
				.flags3(M3.INFRAVISION, M3.INFRAVISIBLE).color(CLR.RED).add();

		MonsterType.of(MC.DEMON, PM.VROCK, "vrock").level(8).move(12).ac(0).mr(50).chaotic(9).freq(2).onlyInHell()
				.smallGroups().noCorpse().attack(AT.CLAW, AD.PHYS, 1, Dice.D4).attack(AT.CLAW, AD.PHYS, 1, Dice.D4)
				.attack(AT.CLAW, AD.PHYS, 1, Dice.D8).attack(AT.CLAW, AD.PHYS, 1, Dice.D8)
				.attack(AT.BITE, AD.PHYS, 1, Dice.D6).weight(1450).nutrition(400).sound(MS.SILENT).size(MZ.LARGE)
				.resists(MR.FIRE, MR.POISON).flags1(M1.POIS).flags2(M2.DEMON, M2.HOSTILE, M2.NASTY, M2.STALK)
				.flags3(M3.INFRAVISION, M3.INFRAVISIBLE).color(CLR.RED).add();

		MonsterType.of(MC.DEMON, PM.HEZROU, "hezrou").level(9).move(6).ac(-2).mr(55).chaotic(10).freq(2).onlyInHell()
				.smallGroups().noCorpse().attack(AT.CLAW, AD.PHYS, 1, Dice.D3).attack(AT.CLAW, AD.PHYS, 1, Dice.D3)
				.attack(AT.BITE, AD.PHYS, 4, Dice.D4).weight(1450).nutrition(400).sound(MS.SILENT).size(MZ.LARGE)
				.resists(MR.FIRE, MR.POISON).flags1(M1.HUMANOID, M1.POIS)
				.flags2(M2.DEMON, M2.HOSTILE, M2.NASTY, M2.STALK).flags3(M3.INFRAVISION, M3.INFRAVISIBLE).color(CLR.RED)
				.add();

		MonsterType.of(MC.DEMON, PM.BONE_DEVIL, "bone devil").level(9).move(15).ac(-1).mr(40).chaotic(9).freq(2)
				.onlyInHell().smallGroups().noCorpse().attack(AT.WEAP, AD.PHYS, 3, Dice.D4)
				.attack(AT.STNG, AD.DRST, 2, Dice.D4).weight(1450).nutrition(400).sound(MS.SILENT).size(MZ.LARGE)
				.resists(MR.FIRE, MR.POISON).flags1(M1.POIS)
				.flags2(M2.COLLECT, M2.DEMON, M2.HOSTILE, M2.NASTY, M2.STALK).flags3(M3.INFRAVISION, M3.INFRAVISIBLE)
				.color(CLR.GRAY).add();

		MonsterType.of(MC.DEMON, PM.ICE_DEVIL, "ice devil").level(11).move(6).ac(-4).mr(55).chaotic(12).freq(2)
				.onlyInHell().noCorpse().attack(AT.CLAW, AD.PHYS, 1, Dice.D4).attack(AT.CLAW, AD.PHYS, 1, Dice.D4)
				.attack(AT.BITE, AD.PHYS, 2, Dice.D4).attack(AT.STNG, AD.COLD, 3, Dice.D4).weight(1450).nutrition(400)
				.sound(MS.SILENT).size(MZ.LARGE).resists(MR.FIRE, MR.COLD, MR.POISON).flags1(M1.POIS, M1.SEE_INVIS)
				.flags2(M2.DEMON, M2.HOSTILE, M2.NASTY, M2.STALK).flags3(M3.INFRAVISION, M3.INFRAVISIBLE)
				.color(CLR.WHITE).add();

		MonsterType.of(MC.DEMON, PM.NALFESHNEE, "nalfeshnee").level(11).move(9).ac(-1).mr(65).chaotic(11).freq(1)
				.onlyInHell().noCorpse().attack(AT.CLAW, AD.PHYS, 1, Dice.D4).attack(AT.CLAW, AD.PHYS, 1, Dice.D4)
				.attack(AT.BITE, AD.PHYS, 2, Dice.D4).attack(AT.MAGC, AD.SPEL, 0, Dice.D0).weight(1450).nutrition(400)
				.sound(MS.SPELL).size(MZ.LARGE).resists(MR.FIRE, MR.POISON).flags1(M1.HUMANOID, M1.POIS)
				.flags2(M2.DEMON, M2.HOSTILE, M2.NASTY, M2.STALK).flags3(M3.INFRAVISION, M3.INFRAVISIBLE).color(CLR.RED)
				.add();

		MonsterType.of(MC.DEMON, PM.PIT_FIEND, "pit fiend").level(13).move(6).ac(-3).mr(65).chaotic(13).freq(2)
				.onlyInHell().noCorpse().attack(AT.WEAP, AD.PHYS, 4, Dice.D2).attack(AT.WEAP, AD.PHYS, 4, Dice.D2)
				.attack(AT.HUGS, AD.PHYS, 2, Dice.D4).weight(1450).nutrition(400).sound(MS.GROWL).size(MZ.LARGE)
				.resists(MR.FIRE, MR.POISON).flags1(M1.POIS, M1.SEE_INVIS)
				.flags2(M2.COLLECT, M2.DEMON, M2.HOSTILE, M2.NASTY, M2.STALK).flags3(M3.INFRAVISION, M3.INFRAVISIBLE)
				.color(CLR.RED).add();

		MonsterType.of(MC.DEMON, PM.SANDESTIN, "sandestin").level(13).move(12).ac(4).mr(60).chaotic(5).freq(1)
				.onlyInHell().noCorpse().attack(AT.WEAP, AD.PHYS, 2, Dice.D6).attack(AT.WEAP, AD.PHYS, 2, Dice.D6)
				.weight(1500).nutrition(400).sound(MS.CUSS).size(MZ.HUMAN).resists(MR.STONE).flags1(M1.HUMANOID)
				.flags2(M2.COLLECT, M2.NOPOLY, M2.SHAPESHIFTER, M2.STALK, M2.STRONG)
				.flags3(M3.INFRAVISION, M3.INFRAVISIBLE).color(CLR.GRAY).add();

		MonsterType.of(MC.DEMON, PM.BALROG, "balrog").level(16).move(5).ac(-2).mr(75).chaotic(14).freq(1).onlyInHell()
				.noCorpse().attack(AT.WEAP, AD.PHYS, 8, Dice.D4).attack(AT.WEAP, AD.PHYS, 4, Dice.D6).weight(1450)
				.nutrition(400).sound(MS.SILENT).size(MZ.LARGE).resists(MR.FIRE, MR.POISON)
				.flags1(M1.FLY, M1.POIS, M1.SEE_INVIS)
				.flags2(M2.COLLECT, M2.DEMON, M2.HOSTILE, M2.NASTY, M2.STALK, M2.STRONG)
				.flags3(M3.INFRAVISION, M3.INFRAVISIBLE).color(CLR.RED).add();

		MonsterType.of(MC.DEMON, PM.JUIBLEX, "Juiblex").level(50).move(3).ac(-7).mr(65).chaotic(15).freq(0).unique()
				.special().onlyInHell().noCorpse().attack(AT.ENGL, AD.DISE, 4, Dice.D10)
				.attack(AT.SPIT, AD.ACID, 3, Dice.D6).weight(1500).nutrition(0).sound(MS.GURGLE).size(MZ.LARGE)
				.resists(MR.FIRE, MR.POISON, MR.ACID, MR.STONE)
				.flags1(M1.ACID, M1.AMORPHOUS, M1.AMPHIBIOUS, M1.FLY, M1.NOHEAD, M1.POIS, M1.SEE_INVIS)
				.flags2(M2.DEMON, M2.HOSTILE, M2.LORD, M2.MALE, M2.NASTY, M2.NOPOLY, M2.PNAME, M2.STALK)
				.flags3(M3.WANTSAMUL, M3.WANTSALL, M3.WAITFORU, M3.COVETOUS, M3.WAITMASK, M3.INFRAVISION)
				.color(CLR.BRIGHT_GREEN).add();

		MonsterType.of(MC.DEMON, PM.YEENOGHU, "Yeenoghu").level(56).move(18).ac(-5).mr(80).chaotic(15).freq(0).unique()
				.special().onlyInHell().noCorpse().attack(AT.WEAP, AD.PHYS, 3, Dice.D6)
				.attack(AT.WEAP, AD.CONF, 2, Dice.D8).attack(AT.CLAW, AD.PLYS, 1, Dice.D6)
				.attack(AT.MAGC, AD.MAGM, 2, Dice.D6).weight(900).nutrition(500).sound(MS.GRUNT).size(MZ.LARGE)
				.resists(MR.FIRE, MR.POISON).flags1(M1.FLY, M1.POIS, M1.SEE_INVIS)
				.flags2(M2.COLLECT, M2.DEMON, M2.HOSTILE, M2.LORD, M2.MALE, M2.NASTY, M2.NOPOLY, M2.PNAME, M2.STALK)
				.flags3(M3.WANTSAMUL, M3.WANTSALL, M3.COVETOUS, M3.INFRAVISION, M3.INFRAVISIBLE).color(CLR.MAGENTA)
				.add();

		MonsterType.of(MC.DEMON, PM.ORCUS, "Orcus").level(66).move(9).ac(-6).mr(85).chaotic(20).freq(0).unique()
				.special().onlyInHell().noCorpse().attack(AT.WEAP, AD.PHYS, 3, Dice.D6)
				.attack(AT.CLAW, AD.PHYS, 3, Dice.D4).attack(AT.CLAW, AD.PHYS, 3, Dice.D4)
				.attack(AT.MAGC, AD.SPEL, 8, Dice.D6).attack(AT.STNG, AD.DRST, 2, Dice.D4).weight(1500).nutrition(500)
				.sound(MS.GRUNT).size(MZ.HUGE).resists(MR.FIRE, MR.POISON).flags1(M1.FLY, M1.POIS, M1.SEE_INVIS)
				.flags2(M2.COLLECT, M2.DEMON, M2.HOSTILE, M2.MALE, M2.NASTY, M2.NOPOLY, M2.PNAME, M2.PRINCE, M2.STALK)
				.flags3(M3.WANTSAMUL, M3.WANTSBOOK, M3.WANTSALL, M3.WAITFORU, M3.COVETOUS, M3.WAITMASK, M3.INFRAVISION,
						M3.INFRAVISIBLE)
				.color(CLR.MAGENTA).add();

		MonsterType.of(MC.DEMON, PM.GERYON, "Geryon").level(72).move(3).ac(-3).mr(75).lawful(15).freq(0).unique()
				.special().onlyInHell().noCorpse().attack(AT.CLAW, AD.PHYS, 3, Dice.D6)
				.attack(AT.CLAW, AD.PHYS, 3, Dice.D6).attack(AT.STNG, AD.DRST, 2, Dice.D4).weight(1500).nutrition(500)
				.sound(MS.BRIBE).size(MZ.HUGE).resists(MR.FIRE, MR.POISON)
				.flags1(M1.FLY, M1.POIS, M1.SEE_INVIS, M1.SLITHY)
				.flags2(M2.DEMON, M2.HOSTILE, M2.MALE, M2.NASTY, M2.NOPOLY, M2.PNAME, M2.PRINCE, M2.STALK)
				.flags3(M3.WANTSAMUL, M3.WANTSALL, M3.COVETOUS, M3.INFRAVISION, M3.INFRAVISIBLE).color(CLR.MAGENTA)
				.add();

		MonsterType.of(MC.DEMON, PM.DISPATER, "Dispater").level(78).move(15).ac(-2).mr(80).lawful(15).freq(0).unique()
				.special().onlyInHell().noCorpse().attack(AT.WEAP, AD.PHYS, 4, Dice.D6)
				.attack(AT.MAGC, AD.SPEL, 6, Dice.D6).weight(1500).nutrition(500).sound(MS.BRIBE).size(MZ.HUMAN)
				.resists(MR.FIRE, MR.POISON).flags1(M1.FLY, M1.HUMANOID, M1.POIS, M1.SEE_INVIS)
				.flags2(M2.COLLECT, M2.DEMON, M2.HOSTILE, M2.MALE, M2.NASTY, M2.NOPOLY, M2.PNAME, M2.PRINCE, M2.STALK)
				.flags3(M3.WANTSAMUL, M3.WANTSALL, M3.COVETOUS, M3.INFRAVISION, M3.INFRAVISIBLE).color(CLR.MAGENTA)
				.add();

		MonsterType.of(MC.DEMON, PM.BAALZEBUB, "Baalzebub").level(89).move(9).ac(-5).mr(85).lawful(20).freq(0).unique()
				.special().onlyInHell().noCorpse().attack(AT.BITE, AD.DRST, 2, Dice.D6)
				.attack(AT.GAZE, AD.STUN, 2, Dice.D6).weight(1500).nutrition(500).sound(MS.BRIBE).size(MZ.LARGE)
				.resists(MR.FIRE, MR.POISON).flags1(M1.FLY, M1.POIS, M1.SEE_INVIS)
				.flags2(M2.DEMON, M2.HOSTILE, M2.MALE, M2.NASTY, M2.NOPOLY, M2.PNAME, M2.PRINCE, M2.STALK)
				.flags3(M3.WANTSAMUL, M3.WANTSALL, M3.WAITFORU, M3.COVETOUS, M3.WAITMASK, M3.INFRAVISION,
						M3.INFRAVISIBLE)
				.color(CLR.MAGENTA).add();

		MonsterType.of(MC.DEMON, PM.ASMODEUS, "Asmodeus").level(105).move(12).ac(-7).mr(90).lawful(20).freq(0).unique()
				.special().onlyInHell().noCorpse().attack(AT.CLAW, AD.PHYS, 4, Dice.D4)
				.attack(AT.MAGC, AD.COLD, 6, Dice.D6).weight(1500).nutrition(500).sound(MS.BRIBE).size(MZ.HUGE)
				.resists(MR.FIRE, MR.COLD, MR.POISON).flags1(M1.FLY, M1.HUMANOID, M1.POIS, M1.SEE_INVIS)
				.flags2(M2.DEMON, M2.HOSTILE, M2.MALE, M2.NASTY, M2.NOPOLY, M2.PNAME, M2.PRINCE, M2.STALK, M2.STRONG)
				.flags3(M3.WANTSAMUL, M3.WANTSALL, M3.WAITFORU, M3.COVETOUS, M3.WAITMASK, M3.INFRAVISION,
						M3.INFRAVISIBLE)
				.color(CLR.MAGENTA).add();

		MonsterType.of(MC.DEMON, PM.DEMOGORGON, "Demogorgon").level(106).move(15).ac(-8).mr(95).chaotic(20).freq(0)
				.unique().special().onlyInHell().noCorpse().attack(AT.MAGC, AD.SPEL, 8, Dice.D6)
				.attack(AT.STNG, AD.DRLI, 1, Dice.D4).attack(AT.CLAW, AD.DISE, 1, Dice.D6)
				.attack(AT.CLAW, AD.DISE, 1, Dice.D6).weight(1500).nutrition(500).sound(MS.GROWL).size(MZ.HUGE)
				.resists(MR.FIRE, MR.POISON).flags1(M1.FLY, M1.NOHANDS, M1.NOLIMBS, M1.POIS, M1.SEE_INVIS)
				.flags2(M2.DEMON, M2.HOSTILE, M2.MALE, M2.NASTY, M2.NOPOLY, M2.PNAME, M2.PRINCE, M2.STALK)
				.flags3(M3.WANTSAMUL, M3.WANTSALL, M3.COVETOUS, M3.INFRAVISION, M3.INFRAVISIBLE).color(CLR.MAGENTA)
				.add();

		MonsterType.of(MC.DEMON, PM.DEATH, "Death").level(30).move(12).ac(-5).mr(100).neutral().freq(0).unique()
				.special().attack(AT.TUCH, AD.DETH, 8, Dice.D8).attack(AT.TUCH, AD.DETH, 8, Dice.D8).weight(1450)
				.nutrition(1).sound(MS.RIDER).size(MZ.HUMAN)
				.resists(MR.FIRE, MR.COLD, MR.SLEEP, MR.ELEC, MR.POISON, MR.STONE)
				.flags1(M1.FLY, M1.HUMANOID, M1.REGEN, M1.SEE_INVIS, M1.TPORT_CNTRL)
				.flags2(M2.HOSTILE, M2.NASTY, M2.NOPOLY, M2.PNAME, M2.STALK, M2.STRONG)
				.flags3(M3.INFRAVISION, M3.INFRAVISIBLE, M3.DISPLACES).color(CLR.MAGENTA).add();

		MonsterType.of(MC.DEMON, PM.PESTILENCE, "Pestilence").level(30).move(12).ac(-5).mr(100).neutral().freq(0)
				.unique().special().attack(AT.TUCH, AD.PEST, 8, Dice.D8).attack(AT.TUCH, AD.PEST, 8, Dice.D8)
				.weight(1450).nutrition(1).sound(MS.RIDER).size(MZ.HUMAN)
				.resists(MR.FIRE, MR.COLD, MR.SLEEP, MR.ELEC, MR.POISON, MR.STONE)
				.flags1(M1.FLY, M1.HUMANOID, M1.REGEN, M1.SEE_INVIS, M1.TPORT_CNTRL)
				.flags2(M2.HOSTILE, M2.NASTY, M2.NOPOLY, M2.PNAME, M2.STALK, M2.STRONG)
				.flags3(M3.INFRAVISION, M3.INFRAVISIBLE, M3.DISPLACES).color(CLR.MAGENTA).add();

		MonsterType.of(MC.DEMON, PM.FAMINE, "Famine").level(30).move(12).ac(-5).mr(100).neutral().freq(0).unique()
				.special().attack(AT.TUCH, AD.FAMN, 8, Dice.D8).attack(AT.TUCH, AD.FAMN, 8, Dice.D8).weight(1450)
				.nutrition(1).sound(MS.RIDER).size(MZ.HUMAN)
				.resists(MR.FIRE, MR.COLD, MR.SLEEP, MR.ELEC, MR.POISON, MR.STONE)
				.flags1(M1.FLY, M1.HUMANOID, M1.REGEN, M1.SEE_INVIS, M1.TPORT_CNTRL)
				.flags2(M2.HOSTILE, M2.NASTY, M2.NOPOLY, M2.PNAME, M2.STALK, M2.STRONG)
				.flags3(M3.INFRAVISION, M3.INFRAVISIBLE, M3.DISPLACES).color(CLR.MAGENTA).add();

		MonsterType.of(MC.DEMON, PM.DJINNI, "djinni").level(7).move(12).ac(4).mr(30).neutral().freq(0).special()
				.noCorpse().attack(AT.WEAP, AD.PHYS, 2, Dice.D8).weight(1500).nutrition(400).sound(MS.DJINNI)
				.size(MZ.HUMAN).resists(MR.POISON, MR.STONE).flags1(M1.FLY, M1.HUMANOID, M1.POIS)
				.flags2(M2.COLLECT, M2.NOPOLY, M2.STALK).flags3(M3.INFRAVISIBLE).color(CLR.YELLOW).add();
	}

	private static void initializeEels() {

		MonsterType.of(MC.EEL, PM.JELLYFISH, "jellyfish").level(3).move(3).ac(6).mr(0).neutral().freq(0).special()
				.genocidable().attack(AT.STNG, AD.DRST, 3, Dice.D3).weight(80).nutrition(20).sound(MS.SILENT)
				.size(MZ.SMALL).resists(MR.POISON).conveys(MR.POISON)
				.flags1(M1.AMPHIBIOUS, M1.NOHANDS, M1.NOHEAD, M1.NOLIMBS, M1.NOTAKE, M1.POIS, M1.SLITHY, M1.SWIM)
				.flags2(M2.HOSTILE).color(CLR.BLUE).add();

		MonsterType.of(MC.EEL, PM.PIRANHA, "piranha").level(5).move(12).ac(4).mr(0).neutral().freq(0).special()
				.smallGroups().genocidable().attack(AT.BITE, AD.PHYS, 2, Dice.D6).weight(60).nutrition(30)
				.sound(MS.SILENT).size(MZ.SMALL).flags1(M1.AMPHIBIOUS, M1.ANIMAL, M1.CARNIVORE, M1.NOHANDS, M1.NOLIMBS,
						M1.NOTAKE, M1.OMNIVORE, M1.OVIPAROUS, M1.SLITHY, M1.SWIM)
				.flags2(M2.HOSTILE).color(CLR.RED).add();

		MonsterType.of(MC.EEL, PM.SHARK, "shark").level(7).move(12).ac(2).mr(0).neutral().freq(0).special()
				.genocidable().attack(AT.BITE, AD.PHYS, 5, Dice.D6).weight(500).nutrition(350).sound(MS.SILENT)
				.size(MZ.LARGE).flags1(M1.AMPHIBIOUS, M1.ANIMAL, M1.CARNIVORE, M1.NOHANDS, M1.NOLIMBS, M1.NOTAKE,
						M1.OMNIVORE, M1.OVIPAROUS, M1.SLITHY, M1.SWIM, M1.THICK_HIDE)
				.flags2(M2.HOSTILE).color(CLR.GRAY).add();

		MonsterType.of(MC.EEL, PM.GIANT_EEL, "giant eel").level(5).move(9).ac(-1).mr(0).neutral().freq(0).special()
				.genocidable().attack(AT.BITE, AD.PHYS, 3, Dice.D6).attack(AT.TUCH, AD.WRAP, 0, Dice.D0).weight(200)
				.nutrition(250).sound(MS.SILENT).size(MZ.HUGE)
				.flags1(M1.AMPHIBIOUS, M1.ANIMAL, M1.CARNIVORE, M1.NOHANDS, M1.NOLIMBS, M1.NOTAKE, M1.OMNIVORE,
						M1.OVIPAROUS, M1.SLITHY, M1.SWIM)
				.flags2(M2.HOSTILE).flags3(M3.INFRAVISIBLE).color(CLR.CYAN).add();

		MonsterType.of(MC.EEL, PM.ELECTRIC_EEL, "electric eel").level(7).move(10).ac(-3).mr(0).neutral().freq(0)
				.special().genocidable().attack(AT.BITE, AD.ELEC, 4, Dice.D6).attack(AT.TUCH, AD.WRAP, 0, Dice.D0)
				.weight(200).nutrition(250).sound(MS.SILENT).size(MZ.HUGE).resists(MR.ELEC).conveys(MR.ELEC)
				.flags1(M1.AMPHIBIOUS, M1.ANIMAL, M1.CARNIVORE, M1.NOHANDS, M1.NOLIMBS, M1.NOTAKE, M1.OMNIVORE,
						M1.OVIPAROUS, M1.SLITHY, M1.SWIM)
				.flags2(M2.HOSTILE).flags3(M3.INFRAVISIBLE).color(CLR.BRIGHT_BLUE).add();

		MonsterType.of(MC.EEL, PM.KRAKEN, "kraken").level(20).move(3).ac(6).mr(0).chaotic(3).freq(0).special()
				.genocidable().attack(AT.CLAW, AD.PHYS, 2, Dice.D4).attack(AT.CLAW, AD.PHYS, 2, Dice.D4)
				.attack(AT.HUGS, AD.WRAP, 2, Dice.D6).attack(AT.BITE, AD.PHYS, 5, Dice.D4).weight(1800).nutrition(1000)
				.sound(MS.SILENT).size(MZ.HUGE)
				.flags1(M1.AMPHIBIOUS, M1.ANIMAL, M1.CARNIVORE, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE, M1.SWIM)
				.flags2(M2.HOSTILE, M2.NOPOLY, M2.STRONG).flags3(M3.INFRAVISIBLE).color(CLR.RED).add();
	}

	private static void initializeLizards() {

		MonsterType.of(MC.LIZARD, PM.NEWT, "newt").level(0).move(6).ac(8).mr(0).neutral().freq(5).genocidable()
				.attack(AT.BITE, AD.PHYS, 1, Dice.D2).weight(10).nutrition(20).sound(MS.SILENT).size(MZ.TINY)
				.flags1(M1.AMPHIBIOUS, M1.ANIMAL, M1.CARNIVORE, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE, M1.SWIM)
				.flags2(M2.HOSTILE).color(CLR.YELLOW).add();

		MonsterType.of(MC.LIZARD, PM.GECKO, "gecko").level(1).move(6).ac(8).mr(0).neutral().freq(5).genocidable()
				.attack(AT.BITE, AD.PHYS, 1, Dice.D3).weight(10).nutrition(20).sound(MS.SQEEK).size(MZ.TINY)
				.flags1(M1.ANIMAL, M1.CARNIVORE, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE).flags2(M2.HOSTILE)
				.color(CLR.GREEN).add();

		MonsterType.of(MC.LIZARD, PM.IGUANA, "iguana").level(2).move(6).ac(7).mr(0).neutral().freq(5).genocidable()
				.attack(AT.BITE, AD.PHYS, 1, Dice.D4).weight(30).nutrition(30).sound(MS.SILENT).size(MZ.TINY)
				.flags1(M1.ANIMAL, M1.CARNIVORE, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE).flags2(M2.HOSTILE)
				.color(CLR.BROWN).add();

		MonsterType.of(MC.LIZARD, PM.BABY_CROCODILE, "baby crocodile").level(3).move(6).ac(7).mr(0).neutral().freq(0)
				.genocidable().attack(AT.BITE, AD.PHYS, 1, Dice.D4).weight(200).nutrition(200).sound(MS.SILENT)
				.size(MZ.MEDIUM)
				.flags1(M1.AMPHIBIOUS, M1.ANIMAL, M1.CARNIVORE, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE, M1.SWIM)
				.flags2(M2.HOSTILE).color(CLR.BROWN).add();

		MonsterType.of(MC.LIZARD, PM.LIZARD, "lizard").level(5).move(6).ac(6).mr(10).neutral().freq(5).genocidable()
				.attack(AT.BITE, AD.PHYS, 1, Dice.D6).weight(10).nutrition(40).sound(MS.SILENT).size(MZ.TINY)
				.resists(MR.STONE).conveys(MR.STONE)
				.flags1(M1.ANIMAL, M1.CARNIVORE, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE).flags2(M2.HOSTILE)
				.color(CLR.GREEN).add();

		MonsterType.of(MC.LIZARD, PM.CHAMELEON, "chameleon").level(6).move(5).ac(6).mr(10).neutral().freq(2)
				.genocidable().attack(AT.BITE, AD.PHYS, 4, Dice.D2).weight(100).nutrition(100).sound(MS.SILENT)
				.size(MZ.TINY).flags1(M1.ANIMAL, M1.CARNIVORE, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE)
				.flags2(M2.HOSTILE, M2.NOPOLY, M2.SHAPESHIFTER).color(CLR.BROWN).add();

		MonsterType.of(MC.LIZARD, PM.CROCODILE, "crocodile").level(6).move(9).ac(5).mr(0).neutral().freq(1)
				.genocidable().attack(AT.BITE, AD.PHYS, 4, Dice.D2).attack(AT.CLAW, AD.PHYS, 1, Dice.D12).weight(1450)
				.nutrition(400)
				.sound(MS.SILENT).size(MZ.LARGE).flags1(M1.AMPHIBIOUS, M1.ANIMAL, M1.CARNIVORE, M1.NOHANDS, M1.NOLIMBS,
						M1.OMNIVORE, M1.OVIPAROUS, M1.SWIM, M1.THICK_HIDE)
				.flags2(M2.HOSTILE, M2.STRONG).color(CLR.BROWN).add();

		MonsterType.of(MC.LIZARD, PM.SALAMANDER, "salamander").level(8).move(12).ac(-1).mr(0).chaotic(9).freq(1)
				.onlyInHell().attack(AT.WEAP, AD.PHYS, 2, Dice.D8).attack(AT.TUCH, AD.FIRE, 1, Dice.D6)
				.attack(AT.HUGS, AD.PHYS, 2, Dice.D6).attack(AT.HUGS, AD.FIRE, 3, Dice.D6).weight(1500).nutrition(400)
				.sound(MS.MUMBLE).size(MZ.MEDIUM).resists(MR.FIRE, MR.SLEEP).conveys(MR.FIRE)
				.flags1(M1.HUMANOID, M1.POIS, M1.SLITHY, M1.THICK_HIDE)
				.flags2(M2.COLLECT, M2.HOSTILE, M2.MAGIC, M2.STALK).flags3(M3.INFRAVISIBLE).color(CLR.ORANGE).add();
	}

	private static void initializeWormTails() {

		MonsterType.of(MC.WORM_TAIL, PM.LONG_WORM_TAIL, "long worm tail").level(0).move(0).ac(0).mr(0).neutral().freq(0)
				.unique().special().noCorpse().weight(0).nutrition(0).sound(MS.SILENT).size(MZ.TINY).flags2(M2.NOPOLY)
				.color(CLR.BROWN).add();
	}

	private static void initializeHumanRoles() {

		MonsterType.of(MC.HUMAN, PM.ARCHEOLOGIST, "archeologist").level(10).move(12).ac(10).mr(1).lawful(3).freq(0)
				.special().attack(AT.WEAP, AD.PHYS, 1, Dice.D6).attack(AT.WEAP, AD.PHYS, 1, Dice.D6).weight(1450)
				.nutrition(400).sound(MS.HUMANOID).size(MZ.HUMAN)
				.flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.NEEDPICK, M1.OMNIVORE, M1.TUNNEL)
				.flags2(M2.COLLECT, M2.HUMAN, M2.NOPOLY, M2.STRONG).flags3(M3.INFRAVISIBLE).color(CLR.WHITE).add();

		MonsterType.of(MC.HUMAN, PM.BARBARIAN, "barbarian").level(10).move(12).ac(10).mr(1).neutral().freq(0).special()
				.attack(AT.WEAP, AD.PHYS, 1, Dice.D6).attack(AT.WEAP, AD.PHYS, 1, Dice.D6).weight(1450).nutrition(400)
				.sound(MS.HUMANOID).size(MZ.HUMAN).resists(MR.POISON)
				.flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.HUMAN, M2.NOPOLY, M2.STRONG).flags3(M3.INFRAVISIBLE).color(CLR.WHITE).add();

		MonsterType.of(MC.HUMAN, PM.CAVEMAN, "caveman").level(10).move(12).ac(10).mr(0).lawful(1).freq(0).special()
				.attack(AT.WEAP, AD.PHYS, 2, Dice.D4).weight(1450).nutrition(400).sound(MS.HUMANOID).size(MZ.HUMAN)
				.flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.HUMAN, M2.MALE, M2.NOPOLY, M2.STRONG).flags3(M3.INFRAVISIBLE).color(CLR.WHITE)
				.add();

		MonsterType.of(MC.HUMAN, PM.CAVEWOMAN, "cavewoman").level(10).move(12).ac(10).mr(0).lawful(1).freq(0).special()
				.attack(AT.WEAP, AD.PHYS, 2, Dice.D4).weight(1450).nutrition(400).sound(MS.HUMANOID).size(MZ.HUMAN)
				.flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.FEMALE, M2.HUMAN, M2.NOPOLY, M2.STRONG).flags3(M3.INFRAVISIBLE).color(CLR.WHITE)
				.add();

		MonsterType.of(MC.HUMAN, PM.HEALER, "healer").level(10).move(12).ac(10).mr(1).neutral().freq(0).special()
				.attack(AT.WEAP, AD.PHYS, 1, Dice.D6).weight(1450).nutrition(400).sound(MS.HUMANOID).size(MZ.HUMAN)
				.resists(MR.POISON).flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.HUMAN, M2.NOPOLY, M2.STRONG).flags3(M3.INFRAVISIBLE).color(CLR.WHITE).add();

		MonsterType.of(MC.HUMAN, PM.KNIGHT, "knight").level(10).move(12).ac(10).mr(1).lawful(3).freq(0).special()
				.attack(AT.WEAP, AD.PHYS, 1, Dice.D6).attack(AT.WEAP, AD.PHYS, 1, Dice.D6).weight(1450).nutrition(400)
				.sound(MS.HUMANOID).size(MZ.HUMAN).flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.HUMAN, M2.NOPOLY, M2.STRONG).flags3(M3.INFRAVISIBLE).color(CLR.WHITE).add();

		MonsterType.of(MC.HUMAN, PM.MONK, "monk").level(10).move(12).ac(10).mr(2).neutral().freq(0).special()
				.attack(AT.CLAW, AD.PHYS, 1, Dice.D8).attack(AT.KICK, AD.PHYS, 1, Dice.D8).weight(1450).nutrition(400)
				.sound(MS.HUMANOID).size(MZ.HUMAN).flags1(M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.HUMAN, M2.MALE, M2.NOPOLY, M2.STRONG).flags3(M3.INFRAVISIBLE).color(CLR.WHITE)
				.add();

		MonsterType.of(MC.HUMAN, PM.PRIEST, "priest").level(10).move(12).ac(10).mr(2).neutral().freq(0).special()
				.attack(AT.WEAP, AD.PHYS, 1, Dice.D6).weight(1450).nutrition(400).sound(MS.HUMANOID).size(MZ.HUMAN)
				.flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.HUMAN, M2.MALE, M2.NOPOLY, M2.STRONG).flags3(M3.INFRAVISIBLE).color(CLR.WHITE)
				.add();

		MonsterType.of(MC.HUMAN, PM.PRIESTESS, "priestess").level(10).move(12).ac(10).mr(2).neutral().freq(0).special()
				.attack(AT.WEAP, AD.PHYS, 1, Dice.D6).weight(1450).nutrition(400).sound(MS.HUMANOID).size(MZ.HUMAN)
				.flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.FEMALE, M2.HUMAN, M2.NOPOLY, M2.STRONG).flags3(M3.INFRAVISIBLE).color(CLR.WHITE)
				.add();

		MonsterType.of(MC.HUMAN, PM.RANGER, "ranger").level(10).move(12).ac(10).mr(2).chaotic(3).freq(0).special()
				.attack(AT.WEAP, AD.PHYS, 1, Dice.D4).weight(1450).nutrition(400).sound(MS.HUMANOID).size(MZ.HUMAN)
				.flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.HUMAN, M2.NOPOLY, M2.STRONG).flags3(M3.INFRAVISIBLE).color(CLR.WHITE).add();

		MonsterType.of(MC.HUMAN, PM.ROGUE, "rogue").level(10).move(12).ac(10).mr(1).chaotic(3).freq(0).special()
				.attack(AT.WEAP, AD.PHYS, 1, Dice.D6).attack(AT.WEAP, AD.PHYS, 1, Dice.D6).weight(1450).nutrition(400)
				.sound(MS.HUMANOID).size(MZ.HUMAN).flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.GREEDY, M2.HUMAN, M2.JEWELS, M2.NOPOLY, M2.STRONG).flags3(M3.INFRAVISIBLE)
				.color(CLR.WHITE).add();

		MonsterType.of(MC.HUMAN, PM.SAMURAI, "samurai").level(10).move(12).ac(10).mr(1).lawful(3).freq(0).special()
				.attack(AT.WEAP, AD.PHYS, 1, Dice.D8).attack(AT.WEAP, AD.PHYS, 1, Dice.D8).weight(1450).nutrition(400)
				.sound(MS.HUMANOID).size(MZ.HUMAN).flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.HUMAN, M2.NOPOLY, M2.STRONG).flags3(M3.INFRAVISIBLE).color(CLR.WHITE).add();

		MonsterType.of(MC.HUMAN, PM.TOURIST, "tourist").level(10).move(12).ac(10).mr(1).neutral().freq(0).special()
				.attack(AT.WEAP, AD.PHYS, 1, Dice.D6).attack(AT.WEAP, AD.PHYS, 1, Dice.D6).weight(1450).nutrition(400)
				.sound(MS.HUMANOID).size(MZ.HUMAN).flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.HUMAN, M2.NOPOLY, M2.STRONG).flags3(M3.INFRAVISIBLE).color(CLR.WHITE).add();

		MonsterType.of(MC.HUMAN, PM.VALKYRIE, "valkyrie").level(10).move(12).ac(10).mr(1).chaotic(1).freq(0).special()
				.attack(AT.WEAP, AD.PHYS, 1, Dice.D8).attack(AT.WEAP, AD.PHYS, 1, Dice.D8).weight(1450).nutrition(400)
				.sound(MS.HUMANOID).size(MZ.HUMAN).resists(MR.COLD)
				.flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.FEMALE, M2.HUMAN, M2.NOPOLY, M2.STRONG).flags3(M3.INFRAVISIBLE).color(CLR.WHITE)
				.add();

		MonsterType.of(MC.HUMAN, PM.WIZARD, "wizard").level(10).move(12).ac(10).mr(3).neutral().freq(0).special()
				.attack(AT.WEAP, AD.PHYS, 1, Dice.D6).weight(1450).nutrition(400).sound(MS.HUMANOID).size(MZ.HUMAN)
				.flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.HUMAN, M2.MAGIC, M2.NOPOLY, M2.STRONG).flags3(M3.INFRAVISIBLE).color(CLR.WHITE)
				.add();
	}

	private static void initializeQuestLeaders() {

		MonsterType.of(MC.HUMAN, PM.LORD_CARNARVON, "Lord Carnarvon").level(20).move(12).ac(0).mr(30).lawful(20).freq(0)
				.unique().special().attack(AT.WEAP, AD.PHYS, 1, Dice.D6).weight(1450).nutrition(400).sound(MS.LEADER)
				.size(MZ.HUMAN).flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.NEEDPICK, M1.OMNIVORE, M1.TUNNEL)
				.flags2(M2.COLLECT, M2.HUMAN, M2.MAGIC, M2.MALE, M2.NOPOLY, M2.PEACEFUL, M2.PNAME, M2.STRONG)
				.flags3(M3.CLOSE, M3.WAITMASK, M3.INFRAVISIBLE).color(CLR.MAGENTA).add();

		MonsterType.of(MC.HUMAN, PM.PELIAS, "Pelias").level(20).move(12).ac(0).mr(30).neutral().freq(0).unique()
				.special().attack(AT.WEAP, AD.PHYS, 1, Dice.D6).weight(1450).nutrition(400).sound(MS.LEADER)
				.size(MZ.HUMAN).resists(MR.POISON).flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.HUMAN, M2.MAGIC, M2.MALE, M2.NOPOLY, M2.PEACEFUL, M2.PNAME, M2.STRONG)
				.flags3(M3.CLOSE, M3.WAITMASK, M3.INFRAVISIBLE).color(CLR.MAGENTA).add();

		MonsterType.of(MC.HUMAN, PM.SHAMAN_KARNOV, "Shaman Karnov").level(20).move(12).ac(0).mr(30).lawful(20).freq(0)
				.unique().special().attack(AT.WEAP, AD.PHYS, 2, Dice.D4).weight(1450).nutrition(400).sound(MS.LEADER)
				.size(MZ.HUMAN).flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.HUMAN, M2.MAGIC, M2.MALE, M2.NOPOLY, M2.PEACEFUL, M2.PNAME, M2.STRONG)
				.flags3(M3.CLOSE, M3.WAITMASK, M3.INFRAVISIBLE).color(CLR.MAGENTA).add();

		MonsterType.of(MC.HUMAN, PM.HIPPOCRATES, "Hippocrates").level(20).move(12).ac(0).mr(40).neutral().freq(0)
				.unique().special().attack(AT.WEAP, AD.PHYS, 1, Dice.D6).weight(1450).nutrition(400).sound(MS.LEADER)
				.size(MZ.HUMAN).resists(MR.POISON).flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.HUMAN, M2.MAGIC, M2.MALE, M2.NOPOLY, M2.PEACEFUL, M2.PNAME, M2.STRONG)
				.flags3(M3.CLOSE, M3.WAITMASK, M3.INFRAVISIBLE).color(CLR.MAGENTA).add();

		MonsterType.of(MC.HUMAN, PM.KING_ARTHUR, "King Arthur").level(20).move(12).ac(0).mr(40).lawful(20).freq(0)
				.unique().special().attack(AT.WEAP, AD.PHYS, 1, Dice.D6).attack(AT.WEAP, AD.PHYS, 1, Dice.D6)
				.weight(1450).nutrition(400).sound(MS.LEADER).size(MZ.HUMAN)
				.flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.HUMAN, M2.MAGIC, M2.MALE, M2.NOPOLY, M2.PEACEFUL, M2.PNAME, M2.STRONG)
				.flags3(M3.CLOSE, M3.WAITMASK, M3.INFRAVISIBLE).color(CLR.MAGENTA).add();

		MonsterType.of(MC.HUMAN, PM.GRAND_MASTER, "Grand Master").level(25).move(12).ac(0).mr(70).neutral().freq(0)
				.unique().special().attack(AT.CLAW, AD.PHYS, 4, Dice.D10).attack(AT.KICK, AD.PHYS, 2, Dice.D8)
				.attack(AT.MAGC, AD.CLRC, 2, Dice.D8).attack(AT.MAGC, AD.CLRC, 2, Dice.D8).weight(1450).nutrition(400)
				.sound(MS.LEADER).size(MZ.HUMAN).resists(MR.FIRE, MR.SLEEP, MR.ELEC, MR.POISON)
				.flags1(M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE, M1.SEE_INVIS)
				.flags2(M2.HUMAN, M2.MAGIC, M2.MALE, M2.NASTY, M2.NOPOLY, M2.PEACEFUL, M2.STRONG)
				.flags3(M3.CLOSE, M3.WAITMASK, M3.INFRAVISIBLE).color(CLR.BLACK).add();

		MonsterType.of(MC.HUMAN, PM.ARCH_PRIEST, "Arch Priest").level(25).move(12).ac(7).mr(70).neutral().freq(0)
				.unique().special().attack(AT.WEAP, AD.PHYS, 4, Dice.D10).attack(AT.KICK, AD.PHYS, 2, Dice.D8)
				.attack(AT.MAGC, AD.CLRC, 2, Dice.D8).attack(AT.MAGC, AD.CLRC, 2, Dice.D8).weight(1450).nutrition(400)
				.sound(MS.LEADER).size(MZ.HUMAN).resists(MR.FIRE, MR.SLEEP, MR.ELEC, MR.POISON)
				.flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE, M1.SEE_INVIS)
				.flags2(M2.COLLECT, M2.HUMAN, M2.MAGIC, M2.MALE, M2.NOPOLY, M2.PEACEFUL, M2.STRONG)
				.flags3(M3.CLOSE, M3.WAITMASK, M3.INFRAVISIBLE).color(CLR.WHITE).add();

		MonsterType.of(MC.HUMAN, PM.ORION, "Orion").level(20).move(12).ac(0).mr(30).neutral().freq(0).unique().special()
				.attack(AT.WEAP, AD.PHYS, 1, Dice.D6).weight(2200).nutrition(700).sound(MS.LEADER).size(MZ.HUGE)
				.flags1(M1.AMPHIBIOUS, M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE, M1.SEE_INVIS, M1.SWIM)
				.flags2(M2.COLLECT, M2.HUMAN, M2.MAGIC, M2.MALE, M2.NOPOLY, M2.PEACEFUL, M2.PNAME, M2.STRONG)
				.flags3(M3.CLOSE, M3.WAITMASK, M3.INFRAVISION, M3.INFRAVISIBLE).color(CLR.MAGENTA).add();

		MonsterType.of(MC.HUMAN, PM.MASTER_OF_THIEVES, "Master of Thieves").level(20).move(12).ac(0).mr(30).chaotic(20)
				.freq(0).unique().special().attack(AT.WEAP, AD.PHYS, 2, Dice.D6).attack(AT.WEAP, AD.PHYS, 2, Dice.D6)
				.attack(AT.CLAW, AD.SAMU, 2, Dice.D4).weight(1450).nutrition(400).sound(MS.LEADER).size(MZ.HUMAN)
				.resists(MR.STONE)
				.flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE).flags2(M2.COLLECT, M2.GREEDY, M2.HUMAN,
						M2.JEWELS, M2.MAGIC, M2.MALE, M2.NOPOLY, M2.PEACEFUL, M2.STRONG)
				.flags3(M3.CLOSE, M3.WAITMASK, M3.INFRAVISIBLE).color(CLR.MAGENTA).add();

		MonsterType.of(MC.HUMAN, PM.LORD_SATO, "Lord Sato").level(20).move(12).ac(0).mr(30).lawful(20).freq(0).unique()
				.special().attack(AT.WEAP, AD.PHYS, 1, Dice.D8).attack(AT.WEAP, AD.PHYS, 1, Dice.D6).weight(1450)
				.nutrition(400).sound(MS.LEADER).size(MZ.HUMAN)
				.flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.HUMAN, M2.MAGIC, M2.MALE, M2.NOPOLY, M2.PEACEFUL, M2.PNAME, M2.STRONG)
				.flags3(M3.CLOSE, M3.WAITMASK, M3.INFRAVISIBLE).color(CLR.MAGENTA).add();

		MonsterType.of(MC.HUMAN, PM.TWOFLOWER, "Twoflower").level(20).move(12).ac(10).mr(20).neutral().freq(0).unique()
				.special().attack(AT.WEAP, AD.PHYS, 1, Dice.D6).attack(AT.WEAP, AD.PHYS, 1, Dice.D6).weight(1450)
				.nutrition(400).sound(MS.LEADER).size(MZ.HUMAN)
				.flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.HUMAN, M2.MAGIC, M2.MALE, M2.NOPOLY, M2.PEACEFUL, M2.PNAME, M2.STRONG)
				.flags3(M3.CLOSE, M3.WAITMASK, M3.INFRAVISIBLE).color(CLR.WHITE).add();

		MonsterType.of(MC.HUMAN, PM.NORN, "Norn").level(20).move(12).ac(0).mr(80).neutral().freq(0).unique().special()
				.attack(AT.WEAP, AD.PHYS, 1, Dice.D8).attack(AT.WEAP, AD.PHYS, 1, Dice.D6).weight(1800).nutrition(550)
				.sound(MS.LEADER).size(MZ.HUGE).resists(MR.COLD)
				.flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.FEMALE, M2.HUMAN, M2.MAGIC, M2.NOPOLY, M2.PEACEFUL, M2.STRONG)
				.flags3(M3.CLOSE, M3.WAITMASK, M3.INFRAVISIBLE).color(CLR.MAGENTA).add();

		MonsterType.of(MC.HUMAN, PM.NEFERET_THE_GREEN, "Neferet the Green").level(20).move(12).ac(0).mr(60).neutral()
				.freq(0).unique().special().attack(AT.WEAP, AD.PHYS, 1, Dice.D6).attack(AT.MAGC, AD.SPEL, 2, Dice.D8)
				.weight(1450).nutrition(400).sound(MS.LEADER).size(MZ.HUMAN)
				.flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.FEMALE, M2.HUMAN, M2.MAGIC, M2.NOPOLY, M2.PEACEFUL, M2.PNAME, M2.STRONG)
				.flags3(M3.CLOSE, M3.WAITMASK, M3.INFRAVISIBLE).color(CLR.GREEN).add();
	}

	private static void initializeQuestNemeses() {

		MonsterType.of(MC.DEMON, PM.MINION_OF_HUHETOTL, "Minion of Huhetotl").level(16).move(12).ac(-2).mr(75)
				.chaotic(14).freq(0).unique().special().noCorpse().attack(AT.WEAP, AD.PHYS, 8, Dice.D4)
				.attack(AT.WEAP, AD.PHYS, 4, Dice.D6).attack(AT.MAGC, AD.SPEL, 0, Dice.D0)
				.attack(AT.CLAW, AD.SAMU, 2, Dice.D6).weight(1450).nutrition(400).sound(MS.NEMESIS).size(MZ.LARGE)
				.resists(MR.FIRE, MR.POISON, MR.STONE).flags1(M1.FLY, M1.POIS, M1.SEE_INVIS)
				.flags2(M2.COLLECT, M2.DEMON, M2.HOSTILE, M2.NASTY, M2.NOPOLY, M2.STALK, M2.STRONG).flags3(M3.WANTSARTI,
						M3.WANTSALL, M3.WAITFORU, M3.COVETOUS, M3.WAITMASK, M3.INFRAVISION, M3.INFRAVISIBLE)
				.color(CLR.RED).add();

		MonsterType.of(MC.HUMAN, PM.THOTH_AMON, "Thoth Amon").level(16).move(12).ac(0).mr(10).chaotic(14).freq(0)
				.unique().special().noCorpse().attack(AT.WEAP, AD.PHYS, 1, Dice.D6).attack(AT.MAGC, AD.SPEL, 0, Dice.D0)
				.attack(AT.MAGC, AD.SPEL, 0, Dice.D0).attack(AT.CLAW, AD.SAMU, 1, Dice.D4).weight(1450).nutrition(400)
				.sound(MS.NEMESIS).size(MZ.HUMAN).resists(MR.POISON, MR.STONE)
				.flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.HOSTILE, M2.HUMAN, M2.MAGIC, M2.MALE, M2.NASTY, M2.NOPOLY, M2.PNAME, M2.STALK,
						M2.STRONG)
				.flags3(M3.WANTSARTI, M3.WANTSALL, M3.WAITFORU, M3.COVETOUS, M3.WAITMASK, M3.INFRAVISIBLE)
				.color(CLR.MAGENTA).add();

		MonsterType.of(MC.DRAGON, PM.CHROMATIC_DRAGON, "Chromatic Dragon").level(16).move(12).ac(0).mr(30).chaotic(14)
				.freq(0).unique().special().attack(AT.BREA, AD.RBRE, 6, Dice.D8).attack(AT.MAGC, AD.SPEL, 0, Dice.D0)
				.attack(AT.CLAW, AD.SAMU, 2, Dice.D8).attack(AT.BITE, AD.PHYS, 4, Dice.D8)
				.attack(AT.BITE, AD.PHYS, 4, Dice.D8).attack(AT.STNG, AD.PHYS, 1, Dice.D6).weight(4500).nutrition(1700)
				.sound(MS.NEMESIS).size(MZ.GIGANTIC)
				.resists(MR.FIRE, MR.COLD, MR.SLEEP, MR.DISINT, MR.ELEC, MR.POISON, MR.ACID, MR.STONE)
				.conveys(MR.FIRE, MR.COLD, MR.SLEEP, MR.DISINT, MR.ELEC, MR.POISON, MR.STONE)
				.flags1(M1.CARNIVORE, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE, M1.POIS, M1.SEE_INVIS, M1.THICK_HIDE)
				.flags2(M2.FEMALE, M2.GREEDY, M2.HOSTILE, M2.JEWELS, M2.MAGIC, M2.NASTY, M2.NOPOLY, M2.STALK, M2.STRONG)
				.flags3(M3.WANTSARTI, M3.WANTSALL, M3.WAITFORU, M3.COVETOUS, M3.WAITMASK, M3.INFRAVISIBLE)
				.color(CLR.MAGENTA).add();

		MonsterType.of(MC.GIANT, PM.CYCLOPS, "Cyclops").level(18).move(12).ac(0).mr(0).chaotic(15).freq(0).unique()
				.special().attack(AT.WEAP, AD.PHYS, 4, Dice.D8).attack(AT.WEAP, AD.PHYS, 4, Dice.D8)
				.attack(AT.CLAW, AD.SAMU, 2, Dice.D6).weight(1900).nutrition(700).sound(MS.NEMESIS).size(MZ.HUGE)
				.resists(MR.STONE).flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.GIANT, M2.HOSTILE, M2.JEWELS, M2.MALE, M2.NASTY, M2.NOPOLY, M2.ROCKTHROW,
						M2.STALK, M2.STRONG)
				.flags3(M3.WANTSARTI, M3.WANTSALL, M3.WAITFORU, M3.COVETOUS, M3.WAITMASK, M3.INFRAVISION,
						M3.INFRAVISIBLE)
				.color(CLR.GRAY).add();

		MonsterType.of(MC.DRAGON, PM.IXOTH, "Ixoth").level(15).move(12).ac(-1).mr(20).chaotic(14).freq(0).unique()
				.special().attack(AT.BREA, AD.FIRE, 8, Dice.D6).attack(AT.BITE, AD.PHYS, 4, Dice.D8)
				.attack(AT.MAGC, AD.SPEL, 0, Dice.D0).attack(AT.CLAW, AD.PHYS, 2, Dice.D4)
				.attack(AT.CLAW, AD.SAMU, 2, Dice.D4).weight(4500).nutrition(1600).sound(MS.NEMESIS).size(MZ.GIGANTIC)
				.resists(MR.FIRE, MR.STONE).conveys(MR.FIRE)
				.flags1(M1.CARNIVORE, M1.FLY, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE, M1.SEE_INVIS, M1.THICK_HIDE)
				.flags2(M2.GREEDY, M2.HOSTILE, M2.JEWELS, M2.MAGIC, M2.MALE, M2.NASTY, M2.NOPOLY, M2.PNAME, M2.STALK,
						M2.STRONG)
				.flags3(M3.WANTSARTI, M3.WANTSALL, M3.WAITFORU, M3.COVETOUS, M3.WAITMASK, M3.INFRAVISIBLE)
				.color(CLR.RED).add();

		MonsterType.of(MC.HUMAN, PM.MASTER_KAEN, "Master Kaen").level(25).move(12).ac(-10).mr(10).chaotic(20).freq(0)
				.unique().special().attack(AT.CLAW, AD.PHYS, 16, Dice.D2).attack(AT.CLAW, AD.PHYS, 16, Dice.D2)
				.attack(AT.MAGC, AD.CLRC, 0, Dice.D0).attack(AT.CLAW, AD.SAMU, 1, Dice.D4).weight(1450).nutrition(400)
				.sound(MS.NEMESIS).size(MZ.HUMAN).resists(MR.POISON, MR.STONE).conveys(MR.POISON)
				.flags1(M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE, M1.SEE_INVIS)
				.flags2(M2.COLLECT, M2.HOSTILE, M2.HUMAN, M2.MAGIC, M2.MALE, M2.NASTY, M2.NOPOLY, M2.PNAME, M2.STALK,
						M2.STRONG)
				.flags3(M3.WANTSARTI, M3.WANTSALL, M3.WAITFORU, M3.COVETOUS, M3.WAITMASK, M3.INFRAVISIBLE)
				.color(CLR.MAGENTA).add();

		MonsterType.of(MC.DEMON, PM.NALZOK, "Nalzok").level(16).move(12).ac(-2).mr(85).chaotic(127).freq(0).unique()
				.special().noCorpse().attack(AT.WEAP, AD.PHYS, 8, Dice.D4).attack(AT.WEAP, AD.PHYS, 4, Dice.D6)
				.attack(AT.MAGC, AD.SPEL, 0, Dice.D0).attack(AT.CLAW, AD.SAMU, 2, Dice.D6).weight(1450).nutrition(400)
				.sound(MS.NEMESIS).size(MZ.LARGE).resists(MR.FIRE, MR.POISON, MR.STONE)
				.flags1(M1.FLY, M1.POIS, M1.SEE_INVIS)
				.flags2(M2.COLLECT, M2.DEMON, M2.HOSTILE, M2.MALE, M2.NASTY, M2.NOPOLY, M2.PNAME, M2.STALK, M2.STRONG)
				.flags3(M3.WANTSARTI, M3.WANTSALL, M3.WAITFORU, M3.COVETOUS, M3.WAITMASK, M3.INFRAVISION,
						M3.INFRAVISIBLE)
				.color(CLR.RED).add();

		MonsterType.of(MC.SPIDER, PM.SCORPIUS, "Scorpius").level(15).move(12).ac(10).mr(0).chaotic(15).freq(0).unique()
				.special().attack(AT.CLAW, AD.PHYS, 2, Dice.D6).attack(AT.CLAW, AD.SAMU, 2, Dice.D6)
				.attack(AT.STNG, AD.DISE, 1, Dice.D4).weight(750).nutrition(350).sound(MS.NEMESIS).size(MZ.HUMAN)
				.resists(MR.POISON, MR.STONE).conveys(MR.POISON)
				.flags1(M1.ANIMAL, M1.CARNIVORE, M1.NOHANDS, M1.NOLIMBS, M1.OMNIVORE, M1.OVIPAROUS, M1.POIS)
				.flags2(M2.COLLECT, M2.HOSTILE, M2.MAGIC, M2.MALE, M2.NASTY, M2.NOPOLY, M2.PNAME, M2.STALK, M2.STRONG)
				.flags3(M3.WANTSARTI, M3.WANTSALL, M3.WAITFORU, M3.COVETOUS, M3.WAITMASK).color(CLR.MAGENTA).add();

		MonsterType.of(MC.HUMAN, PM.MASTER_ASSASSIN, "Master Assassin").level(15).move(12).ac(0).mr(30).lawful(18)
				.freq(0).unique().special().attack(AT.WEAP, AD.DRST, 2, Dice.D6).attack(AT.WEAP, AD.PHYS, 2, Dice.D8)
				.attack(AT.CLAW, AD.SAMU, 2, Dice.D6).weight(1450).nutrition(400).sound(MS.NEMESIS).size(MZ.HUMAN)
				.resists(MR.STONE).flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.HOSTILE, M2.HUMAN, M2.MAGIC, M2.MALE, M2.NASTY, M2.NOPOLY, M2.STALK, M2.STRONG)
				.flags3(M3.WANTSARTI, M3.WANTSALL, M3.WAITFORU, M3.COVETOUS, M3.WAITMASK, M3.INFRAVISIBLE)
				.color(CLR.MAGENTA).add();

		MonsterType.of(MC.HUMAN, PM.ASHIKAGA_TAKAUJI, "Ashikaga Takauji").level(15).move(12).ac(0).mr(40).chaotic(13)
				.freq(0).unique().special().noCorpse().attack(AT.WEAP, AD.PHYS, 2, Dice.D6)
				.attack(AT.WEAP, AD.PHYS, 2, Dice.D6).attack(AT.CLAW, AD.SAMU, 2, Dice.D6).weight(1450).nutrition(400)
				.sound(MS.NEMESIS).size(MZ.HUMAN).resists(MR.STONE)
				.flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.HOSTILE, M2.HUMAN, M2.MAGIC, M2.MALE, M2.NASTY, M2.NOPOLY, M2.PNAME, M2.STALK,
						M2.STRONG)
				.flags3(M3.WANTSARTI, M3.WANTSALL, M3.WAITFORU, M3.COVETOUS, M3.WAITMASK, M3.INFRAVISIBLE)
				.color(CLR.MAGENTA).add();

		MonsterType.of(MC.GIANT, PM.LORD_SURTUR, "Lord Surtur").level(15).move(12).ac(2).mr(50).lawful(12).freq(0)
				.unique().special().attack(AT.WEAP, AD.PHYS, 2, Dice.D10).attack(AT.WEAP, AD.PHYS, 2, Dice.D10)
				.attack(AT.CLAW, AD.SAMU, 2, Dice.D6).weight(2250).nutrition(850).sound(MS.NEMESIS).size(MZ.HUGE)
				.resists(MR.FIRE, MR.STONE).conveys(MR.FIRE)
				.flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.GIANT, M2.HOSTILE, M2.JEWELS, M2.MALE, M2.NASTY, M2.NOPOLY, M2.PNAME,
						M2.ROCKTHROW, M2.STALK, M2.STRONG)
				.flags3(M3.WANTSARTI, M3.WANTSALL, M3.WAITFORU, M3.COVETOUS, M3.WAITMASK, M3.INFRAVISION,
						M3.INFRAVISIBLE)
				.color(CLR.MAGENTA).add();

		MonsterType.of(MC.HUMAN, PM.DARK_ONE, "Dark One").level(15).move(12).ac(0).mr(80).chaotic(10).freq(0).unique()
				.special().noCorpse().attack(AT.WEAP, AD.PHYS, 1, Dice.D6).attack(AT.WEAP, AD.PHYS, 1, Dice.D6)
				.attack(AT.CLAW, AD.SAMU, 1, Dice.D4).attack(AT.MAGC, AD.SPEL, 0, Dice.D0).weight(1450).nutrition(400)
				.sound(MS.NEMESIS).size(MZ.HUMAN).resists(MR.STONE)
				.flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.HOSTILE, M2.HUMAN, M2.MAGIC, M2.NASTY, M2.NOPOLY, M2.STALK, M2.STRONG)
				.flags3(M3.WANTSARTI, M3.WANTSALL, M3.WAITFORU, M3.COVETOUS, M3.WAITMASK, M3.INFRAVISIBLE)
				.color(CLR.BLACK).add();
	}

	private static void initializeQuestGuardians() {

		MonsterType.of(MC.HUMAN, PM.STUDENT, "student").level(5).move(12).ac(10).mr(10).lawful(3).freq(0).special()
				.attack(AT.WEAP, AD.PHYS, 1, Dice.D6).weight(1450).nutrition(400).sound(MS.GUARDIAN).size(MZ.HUMAN)
				.flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.NEEDPICK, M1.OMNIVORE, M1.TUNNEL)
				.flags2(M2.COLLECT, M2.HUMAN, M2.NOPOLY, M2.PEACEFUL, M2.STRONG).flags3(M3.INFRAVISIBLE)
				.color(CLR.WHITE).add();

		MonsterType.of(MC.HUMAN, PM.CHIEFTAIN, "chieftain").level(5).move(12).ac(10).mr(10).neutral().freq(0).special()
				.attack(AT.WEAP, AD.PHYS, 1, Dice.D6).weight(1450).nutrition(400).sound(MS.GUARDIAN).size(MZ.HUMAN)
				.resists(MR.POISON).flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.HUMAN, M2.NOPOLY, M2.PEACEFUL, M2.STRONG).flags3(M3.INFRAVISIBLE)
				.color(CLR.WHITE).add();

		MonsterType.of(MC.HUMAN, PM.NEANDERTHAL, "neanderthal").level(5).move(12).ac(10).mr(10).lawful(1).freq(0)
				.special().attack(AT.WEAP, AD.PHYS, 2, Dice.D4).weight(1450).nutrition(400).sound(MS.GUARDIAN)
				.size(MZ.HUMAN).flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.HUMAN, M2.NOPOLY, M2.PEACEFUL, M2.STRONG).flags3(M3.INFRAVISIBLE)
				.color(CLR.WHITE).add();

		MonsterType.of(MC.HUMAN, PM.ATTENDANT, "attendant").level(5).move(12).ac(10).mr(10).lawful(3).freq(0).special()
				.attack(AT.WEAP, AD.PHYS, 1, Dice.D6).weight(1450).nutrition(400).sound(MS.GUARDIAN).size(MZ.HUMAN)
				.resists(MR.POISON).flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.HUMAN, M2.NOPOLY, M2.PEACEFUL, M2.STRONG).flags3(M3.INFRAVISIBLE)
				.color(CLR.WHITE).add();

		MonsterType.of(MC.HUMAN, PM.PAGE, "page").level(5).move(12).ac(10).mr(10).lawful(3).freq(0).special()
				.attack(AT.WEAP, AD.PHYS, 1, Dice.D6).attack(AT.WEAP, AD.PHYS, 1, Dice.D6).weight(1450).nutrition(400)
				.sound(MS.GUARDIAN).size(MZ.HUMAN).flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.HUMAN, M2.NOPOLY, M2.PEACEFUL, M2.STRONG).flags3(M3.INFRAVISIBLE)
				.color(CLR.WHITE).add();

		MonsterType.of(MC.HUMAN, PM.ABBOT, "abbot").level(5).move(12).ac(10).mr(20).neutral().freq(0).special()
				.attack(AT.CLAW, AD.PHYS, 8, Dice.D2).attack(AT.KICK, AD.STUN, 3, Dice.D2)
				.attack(AT.MAGC, AD.CLRC, 0, Dice.D0).weight(1450).nutrition(400).sound(MS.GUARDIAN).size(MZ.HUMAN)
				.flags1(M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.HUMAN, M2.NOPOLY, M2.PEACEFUL, M2.STRONG).flags3(M3.INFRAVISIBLE)
				.color(CLR.WHITE).add();

		MonsterType.of(MC.HUMAN, PM.ACOLYTE, "acolyte").level(5).move(12).ac(10).mr(20).neutral().freq(0).special()
				.attack(AT.WEAP, AD.PHYS, 1, Dice.D6).attack(AT.MAGC, AD.CLRC, 0, Dice.D0).weight(1450).nutrition(400)
				.sound(MS.GUARDIAN).size(MZ.HUMAN).flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.HUMAN, M2.NOPOLY, M2.PEACEFUL, M2.STRONG).flags3(M3.INFRAVISIBLE)
				.color(CLR.WHITE).add();

		MonsterType.of(MC.HUMAN, PM.HUNTER, "hunter").level(5).move(12).ac(10).mr(10).chaotic(7).freq(0).special()
				.attack(AT.WEAP, AD.PHYS, 1, Dice.D4).weight(1450).nutrition(400).sound(MS.GUARDIAN).size(MZ.HUMAN)
				.flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE, M1.SEE_INVIS)
				.flags2(M2.COLLECT, M2.HUMAN, M2.NOPOLY, M2.PEACEFUL, M2.STRONG).flags3(M3.INFRAVISION, M3.INFRAVISIBLE)
				.color(CLR.WHITE).add();

		MonsterType.of(MC.HUMAN, PM.THUG, "thug").level(5).move(12).ac(10).mr(10).chaotic(3).freq(0).special()
				.attack(AT.WEAP, AD.PHYS, 1, Dice.D6).attack(AT.WEAP, AD.PHYS, 1, Dice.D6).weight(1450).nutrition(400)
				.sound(MS.GUARDIAN).size(MZ.HUMAN).flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.GREEDY, M2.HUMAN, M2.NOPOLY, M2.PEACEFUL, M2.STRONG).flags3(M3.INFRAVISIBLE)
				.color(CLR.WHITE).add();

		MonsterType.of(MC.HUMAN, PM.NINJA, "ninja").level(5).move(12).ac(10).mr(10).lawful(3).freq(0).special()
				.attack(AT.WEAP, AD.PHYS, 1, Dice.D8).attack(AT.WEAP, AD.PHYS, 1, Dice.D8).weight(1450).nutrition(400)
				.sound(MS.HUMANOID).size(MZ.HUMAN).flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.HOSTILE, M2.HUMAN, M2.NOPOLY, M2.STRONG).flags3(M3.INFRAVISIBLE).color(CLR.WHITE)
				.add();

		MonsterType.of(MC.HUMAN, PM.ROSHI, "roshi").level(5).move(12).ac(10).mr(10).lawful(3).freq(0).special()
				.attack(AT.WEAP, AD.PHYS, 1, Dice.D8).attack(AT.WEAP, AD.PHYS, 1, Dice.D8).weight(1450).nutrition(400)
				.sound(MS.GUARDIAN).size(MZ.HUMAN).flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.HUMAN, M2.NOPOLY, M2.PEACEFUL, M2.STRONG).flags3(M3.INFRAVISIBLE)
				.color(CLR.WHITE).add();

		MonsterType.of(MC.HUMAN, PM.GUIDE, "guide").level(5).move(12).ac(10).mr(20).neutral().freq(0).special()
				.attack(AT.WEAP, AD.PHYS, 1, Dice.D6).attack(AT.MAGC, AD.SPEL, 0, Dice.D0).weight(1450).nutrition(400)
				.sound(MS.GUARDIAN).size(MZ.HUMAN).flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.HUMAN, M2.MAGIC, M2.NOPOLY, M2.PEACEFUL, M2.STRONG).flags3(M3.INFRAVISIBLE)
				.color(CLR.WHITE).add();

		MonsterType.of(MC.HUMAN, PM.WARRIOR, "warrior").level(5).move(12).ac(10).mr(10).chaotic(1).freq(0).special()
				.attack(AT.WEAP, AD.PHYS, 1, Dice.D8).attack(AT.WEAP, AD.PHYS, 1, Dice.D8).weight(1450).nutrition(400)
				.sound(MS.GUARDIAN).size(MZ.HUMAN).flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.FEMALE, M2.HUMAN, M2.NOPOLY, M2.PEACEFUL, M2.STRONG).flags3(M3.INFRAVISIBLE)
				.color(CLR.WHITE).add();

		MonsterType.of(MC.HUMAN, PM.APPRENTICE, "apprentice").level(5).move(12).ac(10).mr(30).neutral().freq(0)
				.special().attack(AT.WEAP, AD.PHYS, 1, Dice.D6).attack(AT.MAGC, AD.SPEL, 0, Dice.D0).weight(1450)
				.nutrition(400).sound(MS.GUARDIAN).size(MZ.HUMAN)
				.flags1(M1.CARNIVORE, M1.HERBIVORE, M1.HUMANOID, M1.OMNIVORE)
				.flags2(M2.COLLECT, M2.HUMAN, M2.MAGIC, M2.NOPOLY, M2.PEACEFUL, M2.STRONG).flags3(M3.INFRAVISIBLE)
				.color(CLR.WHITE).add();
	}

	private static void initialize() {

		initializeAngel();
		initializeAnts();
		initializeBats();
		initializeBlobs();
		initializeCentaurs();
		initializeCockatrices();
		initializeDemons();
		initializeDogs();
		initializeDragons();
		initializeEels();
		initializeElementals();
		initializeEyes();
		initializeFelines();
		initializeFungi();
		initializeGhosts();
		initializeGiants();
		initializeGnomes();
		initializeGolems();
		initializeGremlins();
		initializeHumanoids();
		initializeHumanRoles();
		initializeHumans();
		initializeImps();
		initializeJabberwocks();
		initializeJellies();
		initializeKobolds();
		initializeKops();
		initializeLeprechauns();
		initializeLiches();
		initializeLights();
		initializeLizards();
		initializeMimics();
		initializeMummies();
		initializeNagas();
		initializeNymphs();
		initializeOgres();
		initializeOrcs();
		initializePiercers();
		initializePuddings();
		initializeQuadrupeds();
		initializeQuantumMechanics();
		initializeQuestGuardians();
		initializeQuestLeaders();
		initializeQuestNemeses();
		initializeRodents();
		initializeRustMonsters();
		initializeSnakes();
		initializeSpiders();
		initializeTrappers();
		initializeTroll();
		initializeUmberHulks();
		initializeUnicorns();
		initializeVampires();
		initializeVortices();
		initializeWorms();
		initializeWormTails();
		initializeWraiths();
		initializeXan();
		initializeXorns();
		initializeYetis();
		initializeZombies();
		initializeZruties();
	}
	
	public static MonsterType getMonsterType(int pmid) {
		if (monsterTypesByPMID.isEmpty()) {
			initialize();
		}
		if (monsterTypesByPMID.containsKey(pmid))
		{
			return monsterTypesByPMID.get(pmid);
		}
		return UNKNOWN_MONSTER_TYPE;
	}

	public static String name(int pmid) {
		return getMonsterType(pmid).name;
	}

	public static int monsterClass(int pmid) {
		return getMonsterType(pmid).monsterClass.id();
	}

	public static int monsterLevel(int pmid) {
		return getMonsterType(pmid).monsterLevel;
	}
	
	public static int monsterMovementSpeed(int pmid) {
		return getMonsterType(pmid).movementSpeed;
	}

	public static int monsterBaseArmorClass(int pmid) {
		return getMonsterType(pmid).baseArmorClass;
	}

}
