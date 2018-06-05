package ramsey;

import com.amazonaws.AmazonServiceException;
import com.amazonaws.auth.EnvironmentVariableCredentialsProvider;
import com.amazonaws.services.s3.AmazonS3;
import com.amazonaws.services.s3.AmazonS3ClientBuilder;
import com.amazonaws.services.s3.model.*;
import pojos.GraphRequest;

import java.io.*;
import java.util.ArrayList;
import java.util.List;
import java.util.UUID;

public class S3Manager {
  private static final String BUCKET_NAME_INCREMENT = "ramsey-graphs-increment";
  private static final String BUCKET_NAME_GENERATE = "ramsey-graphs-generate";
  private AmazonS3 s3Client;

  public S3Manager() {
    s3Client = AmazonS3ClientBuilder.standard()
            .withCredentials(new EnvironmentVariableCredentialsProvider())
            .withRegion("us-west-1")
            .build();
  }

  public boolean uploadGraph(GraphRequest graphRequest, UUID uuid) {
    try {
      s3Client.putObject(BUCKET_NAME_INCREMENT, graphRequest.getGraphName() + "/" + uuid, graphRequest.getGraphString());
    } catch (AmazonServiceException e) {
      System.err.println(e.getErrorMessage());
      return false;
    }
    return true;
  }

  public String getGraph(String graphKey) {
    S3Object object = s3Client.getObject(new GetObjectRequest(BUCKET_NAME_INCREMENT, graphKey));
    InputStream objectData = object.getObjectContent();
    ByteArrayOutputStream result = new ByteArrayOutputStream();
    byte[] buffer = new byte[1024];
    int length;
    try {
      while ((length = objectData.read(buffer)) != -1) {
        result.write(buffer, 0, length);
      }
    } catch (IOException e) {
      System.out.println(e);
      return "error";
    }

    try {
      return result.toString("UTF-8");
    } catch (UnsupportedEncodingException e) {
      System.out.println(e);
      return "error";
    }
  }

  /**
   * Returns all the names of the graph files under a certain prefix
   */
  public List<String> getGraphList(String prefix) {
    List<String> graphList = new ArrayList<String>();
    ObjectListing listing = s3Client.listObjects(BUCKET_NAME_INCREMENT, prefix);
    List<S3ObjectSummary> summaries = listing.getObjectSummaries();

    while (listing.isTruncated()) {
      listing = s3Client.listNextBatchOfObjects (listing);
      summaries.addAll (listing.getObjectSummaries());
    }

    for (S3ObjectSummary item : summaries) {
      if (!item.getKey().contains("old"))
        graphList.add(item.getKey());
    }
    return graphList;
  }

  /**
   * Example is giving a prefix of "200" and will return the number of files under that directory
   */
  public int getSubdirectorySize(String prefix) {
    ObjectListing listing = s3Client.listObjects(BUCKET_NAME_INCREMENT, prefix );
    List<S3ObjectSummary> summaries = listing.getObjectSummaries();
    return summaries.size();
  }

  /**
   * newDirectory must include the "/" character
   */
  public void moveFolders(String graphKey, String newDirectory) {
    try {
      s3Client.copyObject(BUCKET_NAME_INCREMENT, graphKey, BUCKET_NAME_INCREMENT, newDirectory + graphKey);
      s3Client.deleteObject(BUCKET_NAME_INCREMENT, graphKey);
    } catch (AmazonServiceException e) {
      System.err.println(e.getErrorMessage());
    }
  }
}
