package pojos;

import lombok.Builder;
import lombok.Data;

/***
 * For the POST at /uploadGraph
 */
@Data
@Builder
public class GraphResponse {
  private boolean success;
}
