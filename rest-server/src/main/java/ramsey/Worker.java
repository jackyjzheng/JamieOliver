package ramsey;

import lombok.Builder;
import lombok.Data;

@Data
@Builder
public class Worker {
  private String ip;
  private int port;
  private int ramseyNum;
}
