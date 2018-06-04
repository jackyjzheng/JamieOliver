package ramsey;

import org.springframework.web.bind.annotation.*;
import pojos.GraphRequest;
import pojos.GraphResponse;


import java.util.UUID;
import java.util.concurrent.atomic.AtomicLong;

@RestController
public class RamseyApi {

  private static final String DEFAULT_MESSAGE = "Service is running";
  private static final S3Manager s3Manager = new S3Manager();
  private static AtomicLong currRamseyNum = new AtomicLong();
  private static WorkerManager workerManager = new WorkerManager();

//  @RequestMapping("/test")
//  public Server server(@RequestParam(value="name", defaultValue="World") String name) {
//    return new Server(counter.incrementAndGet(),
//            String.format(template, name));
//  }


  @RequestMapping(value="/", method = RequestMethod.GET)
  public String printDefaultMessage() {
    return DEFAULT_MESSAGE;
  }

  @RequestMapping(value="/uploadGraph", method=RequestMethod.POST, consumes="application/json")
  public GraphResponse uploadGraph(@RequestBody GraphRequest req) {
    UUID uuid = UUID.randomUUID();
    String graphKey = req.getGraphName() + "/" + uuid;
    boolean success = s3Manager.uploadGraph(req, uuid);
    workerManager.addGraph(graphKey);
    return GraphResponse.builder()
            .success(success)
            .build();
  }

  @RequestMapping(value="/getGraph", method=RequestMethod.GET)
  public String getGraph(@RequestParam(value="graphFilename") String graphFilename) {
    return s3Manager.getGraph(graphFilename);
  }

  @RequestMapping(value="/updateRamsey", method=RequestMethod.GET)
  public String updateRamseyNum(@RequestParam(value="ramseyNum") String incRamseyNumString) {
    Long incRamseyNum = Long.parseLong(incRamseyNumString);
    currRamseyNum.set(incRamseyNum);
    return currRamseyNum.toString();
  }
}