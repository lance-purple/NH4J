package rec.games.roguelike.nh4j;

public class M3 {

	public static final long  WANTSAMUL = 0x0001; /* would like to steal the amulet */
	public static final long  WANTSBELL = 0x0002; /* wants the bell */
	public static final long  WANTSBOOK = 0x0004; /* wants the book */
	public static final long  WANTSCAND = 0x0008; /* wants the candelabrum */
	public static final long  WANTSARTI = 0x0010; /* wants the quest artifact */
	public static final long  WANTSALL = 0x001f;  /* wants any major artifact */
	public static final long  WAITFORU = 0x0040;  /* waits to see you or get attacked */
	public static final long  CLOSE = 0x0080;     /* lets you close unless attacked */

	public static final long  COVETOUS = 0x001f; /* wants something */
	public static final long  WAITMASK = 0x00c0; /* waiting... */

	/* Infravision is currently implemented for players only */
	public static final long  INFRAVISION = 0x0100;  /* has infravision */
	public static final long  INFRAVISIBLE = 0x0200; /* visible by infravision */

	public static final long  DISPLACES = 0x0400; /* moves monsters out of its way */

}
