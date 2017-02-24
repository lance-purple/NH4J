package rec.games.roguelike.nh4j;

import java.util.HashMap;

public class Qualities {
	
	private static HashMap<Integer,Long> extrinsics = new HashMap<>();
	private static HashMap<Integer,Long> intrinsics = new HashMap<>();
	private static HashMap<Integer,Long> blockers = new HashMap<>();

	public static long extrinsic(int index) {
		if (! extrinsics.containsKey(index)) {
			return 0L;
		} else {
			return extrinsics.get(index).longValue();
		}		
	}

	public static void setExtrinsic(int index, long value) {
		if ((value != 0L) || (extrinsics.containsKey(index))) {
	        extrinsics.put(index, value);
		}
	}

	public static long intrinsic(int index) {
		if (! intrinsics.containsKey(index)) {
			return 0L;
		} else {
			return intrinsics.get(index).longValue();
		}		
	}

	public static void setIntrinsic(int index, long value) {
		if ((value != 0L) || (intrinsics.containsKey(index))) {
	        intrinsics.put(index, value);
		}
	}

	public static long blocker(int index) {
		if (! blockers.containsKey(index)) {
			return 0L;
		} else {
			return blockers.get(index).longValue();
		}		
	}

	public static void setBlocker(int index, long value) {
		if ((value != 0L) || (blockers.containsKey(index))) {
	        blockers.put(index, value);
		}
	}


}
