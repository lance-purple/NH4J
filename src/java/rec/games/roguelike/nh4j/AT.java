package rec.games.roguelike.nh4j;

public enum AT {

	ANY(-1), /* fake attack; dmgtype_fromattack wildcard */
	NONE(0), /* passive monster (ex. acid blob) */
	CLAW(1), /* claw (punch, hit, etc.) */
	BITE(2), /* bite */
	KICK(3), /* kick */
	BUTT(4), /* head butt (ex. a unicorn) */
	TUCH(5), /* touches */
	STNG(6), /* sting */
	HUGS(7), /* crushing bearhug */
	SPIT(10), /* spits substance - ranged */
	ENGL(11), /* engulf (swallow or by a cloud) */
	BREA(12), /* breath - ranged */
	EXPL(13), /* explodes - proximity */
	BOOM(14), /* explodes when killed */
	GAZE(15), /* gaze - ranged */
	TENT(16), /* tentacles */
	WEAP(254), /* uses weapon */
	MAGC(255); /* uses magic spell(s) */


	int id;

	AT(int id) {
		this.id = id;
	}

	public int id() {
		return this.id;
	}

}
