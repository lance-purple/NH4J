package rec.games.roguelike.nh4j;

public class OccupiedRooms {

	private static final int MAXIMUM_COUNT = 5;
	
	private static int previouslyOccupiedRoomIDs[] = new int[MAXIMUM_COUNT];
	private static int freshlyEnteredRoomIDs[]     = new int[MAXIMUM_COUNT];
	
	private static int previouslyOccupiedShopIDs[] = new int[MAXIMUM_COUNT];
	private static int freshlyExitedShopIDs[] = new int[MAXIMUM_COUNT];

	public static int maximumCount() {
		return MAXIMUM_COUNT;
	}
	
	public static int previouslyOccupiedRoomIDs(int index) {
		return previouslyOccupiedRoomIDs[index];
	}

	public static void setPreviouslyOccupiedRoomIDs(int index, int roomID) {
		previouslyOccupiedRoomIDs[index] = roomID;
	}

	public static int freshlyEnteredRoomIDs(int index) {
		return freshlyEnteredRoomIDs[index];
	}

	public static void setFreshlyEnteredRoomIDs(int index, int roomID) {
		freshlyEnteredRoomIDs[index] = roomID;
	}

	public static int previouslyOccupiedShopIDs(int index) {
		return previouslyOccupiedShopIDs[index];
	}

	public static void setPreviouslyOccupiedShopIDs(int index, int roomID) {
		previouslyOccupiedShopIDs[index] = roomID;
	}

	public static int freshlyExitedShopIDs(int index) {
		return freshlyExitedShopIDs[index];
	}

	public static void setFreshlyExitedShopIDs(int index, int roomID) {
		freshlyExitedShopIDs[index] = roomID;
	}

}
