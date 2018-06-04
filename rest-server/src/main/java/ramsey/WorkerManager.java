package ramsey;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class WorkerManager {
  private static final int COUNTER_EXAMPLE_THRESHOLD = 100;
  private Map<String, Boolean> workersMap; // Map of the workers running cyclic.cpp
  private S3Manager s3Manager;
  private Map<String, String> graphMap;

  public WorkerManager() {
    workersMap = new HashMap<String, Boolean>();
    s3Manager = new S3Manager();
    graphMap = new HashMap<String, String>();
    initializeGraphs();
  }

  /***
   * Workers will be in form {ip: ramseyGraphTheyAreWorkingOn}
   */
  public void initializeWorkersMap() {
  }

  private void initializeGraphs() {
    List<String> graphList = s3Manager.getGraphList();
    for (String graphKey : graphList) {
      addGraph(graphKey);
    }
  }

  /***
   * If the subdirectory size is greater than a certain threshold, then workers will be put onto running on those
   * counter examples by updating workersMap
   */
  public void addGraph(String graphKey) {
    graphMap.put(graphKey, "");
    String prefix = graphKey.split("/")[0];
    // Check if enough counter examples for that subdirectory to move workers
    System.out.println(s3Manager.getSubdirectorySize(prefix));
  }
}
