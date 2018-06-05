package pojos;

import lombok.Builder;
import lombok.Data;

/***
 * For the POST at /uploadGraph
 */
@Data
@Builder
public class GraphRequest {
  private String graphString;
  private String graphName;
}
