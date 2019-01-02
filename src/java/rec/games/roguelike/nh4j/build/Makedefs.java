package rec.games.roguelike.nh4j.build;

import rec.games.roguelike.nh4j.MonsterType;

public class Makedefs {

	private static final String EPITAPH_FILE = null;
	private static final String ENGRAVE_FILE = null;
	private static final String BOGUS_MONSTER_FILE = null;

	public static void main(String[] args) {
		processVersionInfo();		
		process(args[0]);
	}

	// private static void processVersionInfo() {
	// int i;
	//
	// /*
	// * integer version number
	// */
	// version.incarnation = ((unsigned long) VERSION_MAJOR << 24)
	// | ((unsigned long) VERSION_MINOR << 16)
	// | ((unsigned long) PATCHLEVEL << 8)
	// | ((unsigned long) EDITLEVEL);
	// /*
	// * encoded feature list
	// * Note: if any of these magic numbers are changed or reassigned,
	// * EDITLEVEL in patchlevel.h should be incremented at the same time.
	// * The actual values have no special meaning, and the category
	// * groupings are just for convenience.
	// */
	// version.feature_set = (unsigned long) (0L
	// /* levels and/or topology (0..4) */
	// /* monsters (5..9) */
	//
	// /* objects (10..14) */
	// /* flag bits and/or other global variables (15..26) */
	//
	// | (1L << 17)
	//
	// | (1L << 18)
	//
	// /* data format (27..31)
	// * External compression methods such as COMPRESS and ZLIB_COMP
	// * do not affect the contents and are thus excluded from here */
	//
	// );
	// /*
	// * Value used for object & monster sanity check.
	// * (NROFARTIFACTS<<24) | (NUM_OBJECTS<<12) | (NUMMONS<<0)
	// */
	// for (i = 1; artifact_names[i]; i++) {
	// continue;
	// }
	//
	// version.entity_count = (unsigned long) (i - 1);
	//
	// for (i = 1; objects[i].oc_class != ILLOBJ_CLASS; i++) {
	// continue;
	// }
	//
	// version.entity_count = (version.entity_count << 12) | (unsigned long) i;
	//
	// i += MonsterType.getNumberOfMonsterTypes();
	//
	// version.entity_count = (version.entity_count << 12) | (unsigned long) i;
	// /*
	// * Value used for compiler (word size/field alignment/padding) check.
	// */
	//
	// version.struct_sizes1 =
	// (((unsigned long) sizeof(struct context_info) << 24)
	// | ((unsigned long) sizeof(struct obj) << 17)
	// | ((unsigned long) sizeof(struct monst) << 10)
	// | ((unsigned long) sizeof(struct you)));
	//
	// version.struct_sizes2 = (((unsigned long) sizeof(struct flag) << 10) |
	// /* free bits in here */
	// ((unsigned long) 0L));
	// return;
	// }

	private static void process(String option) {
		switch (option.toLowerCase()) {
		case "-d":
			processData();
			break;
		case "-e":
			processDungeon();
			break;
		case "-h":
			processOracles();
			break;
		case "-q":
			processQuestText();
			break;
		case "-r":
			processRumors();
			break;
		case "-s":
			processRandomAccessFile(EPITAPH_FILE);
			processRandomAccessFile(ENGRAVE_FILE);
			processRandomAccessFile(BOGUS_MONSTER_FILE);
			break;
		case "-v":
			processDate();
			processOptions();
			break;
		case "monster.count":
            processMonsterCount();
			break;
		default:
			throw new IllegalArgumentException("UNKNOWN OPTION " + option);
		}
	}

	private static void processData() {
		throw new UnsupportedOperationException();
	}

	private static void processDate() {
		throw new UnsupportedOperationException();
	}

	private static void processDungeon() {
		throw new UnsupportedOperationException();
	}

	private static void processMonsterCount() {
		System.out.println(MonsterType.getNumberOfMonsterTypes());
	}

	private static void processOptions() {
		throw new UnsupportedOperationException();
	}

	private static void processOracles() {
		throw new UnsupportedOperationException();
	}

	private static void processQuestText() {
		throw new UnsupportedOperationException();
	}

	private static void processRandomAccessFile(String filename) {
		throw new UnsupportedOperationException();
	}

	private static void processRumors() {
		throw new UnsupportedOperationException();
	}

	private static void processVersionInfo() {
		
		 /*
		 * integer version number
		 */
		 long versionIncarnation = (PatchLevel.VERSION_MAJOR << 24)
		 | (PatchLevel.VERSION_MINOR << 16)
		 | (PatchLevel.PATCHLEVEL << 8)
		 | (PatchLevel.EDITLEVEL);
		
	//	 throw new UnsupportedOperationException();
	}

}
