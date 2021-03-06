package ramsey;

import lombok.Data;

import java.io.DataOutputStream;
import java.io.IOException;
import java.net.Socket;
import java.util.*;

@Data
public class WorkerManager {
  private static final int COUNTER_EXAMPLE_THRESHOLD = 100;
  private List<Worker> counterExampleWorkers; // Map of the workers building counter examples
  private List<Worker> advanceWorkers; // Map of the workers advancing the ramseyNum
  private S3Manager s3Manager;
  private Map<String, String> graphMap;
  private int counterExampleNum;
  private int advanceNum;


  public WorkerManager(int counterExampleNum, int advanceNum) {
    counterExampleWorkers = new ArrayList<>();
    advanceWorkers = new ArrayList<>();
    s3Manager = new S3Manager();
    graphMap = new HashMap<>();
    this.counterExampleNum = counterExampleNum;
    this.advanceNum = advanceNum;
    initializeWorkersList(325);
  }

  private void initializeWorkersList(int ramseyNum) {
    counterExampleWorkers.add(Worker.builder()
            .ip("34.235.152.153")
            .port(9090)
            .ramseyNum(ramseyNum)
            .build());

    counterExampleWorkers.add(Worker.builder()
            .ip("18.204.245.243")
            .port(9090)
            .ramseyNum(325)
            .build());

    counterExampleWorkers.add(Worker.builder()
            .ip("34.228.196.116")
            .port(9090)
            .ramseyNum(325)
            .build());

    counterExampleWorkers.add(Worker.builder()
            .ip("52.23.250.133")
            .port(9090)
            .ramseyNum(325)
            .build());

    counterExampleWorkers.add(Worker.builder()
            .ip("54.208.204.102")
            .port(9090)
            .ramseyNum(325)
            .build());

    counterExampleWorkers.add(Worker.builder()
            .ip("54.82.246.180")
            .port(9090)
            .ramseyNum(325)
            .build());

    counterExampleWorkers.add(Worker.builder()
            .ip("34.201.6.179")
            .port(9090)
            .ramseyNum(325)
            .build());

    counterExampleWorkers.add(Worker.builder()
            .ip("18.205.7.217")
            .port(9090)
            .ramseyNum(325)
            .build());

    advanceWorkers.add(Worker.builder()
            .ip("18.232.91.179")
            .port(9090)
            .ramseyNum(325)
            .build());

    advanceWorkers.add(Worker.builder()
            .ip("18.206.171.173")
            .port(9090)
            .ramseyNum(325)
            .build());
  }

  public void incrementCounterExample(String graphString) {
    // Send messages to all the counterExampleWorkers to increment their ramseyNumber
    for (Worker worker : counterExampleWorkers) {
      sendMessage(worker, "DATA," + graphString+ ",DONE ");
      worker.setRamseyNum(counterExampleNum);
    }
  }

  /**
   *
   * Returns String representation of the graph if there is a graph under the directory of ramseyNum/
   * else it returns the string "NONE"
   */
  public String serveGraphRequest(int ramseyNum) {
    List<String> possibleGraphs = s3Manager.getGraphList(Integer.toString(ramseyNum));
    if (!possibleGraphs.isEmpty()) {
      Random rand = new Random();
      String randGraph = possibleGraphs.get(rand.nextInt(possibleGraphs.size()));
      String graphString = s3Manager.getGraph(randGraph);
      s3Manager.moveFolders(randGraph, "old/");
      System.out.println("GRAPH REQUEST GAVE " + randGraph);
      return graphString;
    } else {
      return "NONE";
    }
  }

  public Worker findCounterExampleWorker(String ip) {
    for (Worker worker : counterExampleWorkers) {
      if (worker.getIp().equals("ip")) {
        return worker;
      }
    }
    return null;
  }

  /**
   * If the subdirectory size is greater than a certain threshold, then workers will be put onto running on those
   * counter examples by updating workersMap
   */
  public void addGraph(String graphKey) {
    graphMap.put(graphKey, "");
    String prefix = graphKey.split("/")[0];
    // Check if enough counter examples for that subdirectory to move workers
    if (s3Manager.getSubdirectorySize(prefix) > COUNTER_EXAMPLE_THRESHOLD) {
      System.out.println("REACHED THE THRESHOLD");
      sendIncrementRamsey(prefix);
    }
  }

  /**
   * Once one of the 8 nodes reaches the threshold for graphs, all 8 become a counterexample worker for currRamseyNumber + 2
   */
  public void sendIncrementRamsey(String prefix) {
    for (Worker worker : counterExampleWorkers) {
      int newNum = Integer.parseInt(prefix);
      newNum += 2;
      sendMessage(worker, "NEW" + Integer.toString(newNum) + ", " + Integer.toString(newNum/10));
    }
  }

  /**
   * Simple function to send a message to one worker
   */
  public void sendMessage(Worker worker, String message) {
    try {
      System.out.println("Sending message to " + worker.getIp() + " with message " + message);
      Socket socket = new Socket(worker.getIp(), worker.getPort());
      DataOutputStream dOut = new DataOutputStream(socket.getOutputStream());

      dOut.writeUTF(message);
      dOut.flush(); // Send off the data

      dOut.close();
      socket.close();
    } catch (IOException e) {
      System.out.println(e);
    }
  }
}
