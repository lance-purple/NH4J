package rec.games.roguelike.nh4j;

public class SysOpt {

	private static final Attack[] SEDUCTION_ATTACKS = {
			new Attack(AT.BITE, AD.SSEX, 0, Dice.D0),
			new Attack(AT.CLAW, AD.PHYS, 1, Dice.D3),
			new Attack(AT.CLAW, AD.PHYS, 1, Dice.D3) };

	private static final Attack[] NON_SEDUCTION_ATTACKS = {
			new Attack(AT.CLAW, AD.PHYS, 1, Dice.D3),
			new Attack(AT.CLAW, AD.PHYS, 1, Dice.D3),
			new Attack(AT.BITE, AD.DRLI, 2, Dice.D6) };

	private static void enableSeductionAttackFor(PM pm, boolean enabled) {
		MonsterType.getMonsterType(pm.id()).setAttacks(
				enabled ? SEDUCTION_ATTACKS : NON_SEDUCTION_ATTACKS);
	}

	public static void enableSeductionAttacks(boolean enabled) {
		enableSeductionAttackFor(PM.INCUBUS, enabled);
		enableSeductionAttackFor(PM.SUCCUBUS, enabled);
	}

}
