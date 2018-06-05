package ramsey;

import org.springframework.web.bind.annotation.*;
import pojos.GraphRequest;
import pojos.GraphResponse;


import java.util.UUID;

@RestController
public class RamseyApi {

  private static final String DEFAULT_MESSAGE = "Service is running";
  private static final S3Manager s3Manager = new S3Manager();
  private static WorkerManager workerManager = new WorkerManager(325, 325);

//  @RequestMapping("/test")
//  public Server server(@RequestParam(value="name", defaultValue="World") String name) {
//    return new Server(counter.incrementAndGet(),
//            String.format(template, name));
//  }


  @RequestMapping(value="/", method = RequestMethod.GET)
  public String printDefaultMessage() {
    return DEFAULT_MESSAGE;
  }

  @RequestMapping(value="/test", method = RequestMethod.GET)
  public void test() {
  }

  @RequestMapping(value="/uploadGraph", method=RequestMethod.POST, consumes="application/json")
  public GraphResponse uploadGraph(@RequestBody GraphRequest req) {
    UUID uuid = UUID.randomUUID();
    String graphKey = req.getGraphName() + "/" + uuid;
    boolean success = s3Manager.uploadGraph(req, uuid, "search");
    workerManager.addGraph(graphKey);
    return GraphResponse.builder()
            .success(success)
            .build();
  }

  @RequestMapping(value="/incrementGraph", method=RequestMethod.POST, consumes="application/json")
  public GraphResponse incrementGraph(@RequestBody GraphRequest req) {
    UUID uuid = UUID.randomUUID();
    String graphKey = req.getGraphName() + "/" + uuid;
    boolean success = s3Manager.uploadGraph(req, uuid, "increment");
    workerManager.addGraph(graphKey);
    return GraphResponse.builder()
            .success(success)
            .build();
  }

  @RequestMapping(value="/getGraph", method=RequestMethod.GET)
  public String getGraph(@RequestParam(value="graphFilename") String graphFilename) {
    return s3Manager.getGraph(graphFilename);
  }

  // Maybe don't need since worker manager handles the updating?
  @RequestMapping(value="/updateAdvanceNum", method=RequestMethod.GET)
  public void updateRamseyNum(@RequestParam(value="advanceNum") String incRamseyNumString) {
    int incRamseyNum = Integer.parseInt(incRamseyNumString);
    workerManager.setAdvanceNum(incRamseyNum);
  }

  @RequestMapping(value="/requestGraph", method=RequestMethod.GET)
  public String requestGraph(@RequestParam(value="ramseyNum") int ramseyNum) {
    return workerManager.serveGraphRequest(ramseyNum);
  }

  @RequestMapping(value="/getBucketSize", method=RequestMethod.GET)
  public int getBucketSize(@RequestParam(value="size") String ramseyNum) {
    return s3Manager.getSubdirectorySize(ramseyNum);
  }
}