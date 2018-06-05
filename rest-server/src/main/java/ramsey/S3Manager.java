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
  private static final String BUCKET_NAME = "ramsey-graphs";
  private AmazonS3 s3Client;

  public S3Manager() {
    s3Client = AmazonS3ClientBuilder.standard()
            .withCredentials(new EnvironmentVariableCredentialsProvider())
            .withRegion("us-west-1")
            .build();
  }

  public boolean uploadGraph(GraphRequest graphRequest, UUID uuid) {
    try {
      s3Client.putObject(BUCKET_NAME, graphRequest.getGraphName() + "/" + uuid, graphRequest.getGraphString());
    } catch (AmazonServiceException e) {
      System.err.println(e.getErrorMessage());
      return false;
    }
    return true;
  }

  public String getGraph(String graphFilename) {
    S3Object object = s3Client.getObject(new GetObjectRequest(BUCKET_NAME, graphFilename));
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

  public List<String> getGraphList() {
    List<String> graphList = new ArrayList<String>();
    ObjectListing listing = s3Client.listObjects(BUCKET_NAME);
    List<S3ObjectSummary> summaries = listing.getObjectSummaries();

    while (listing.isTruncated()) {
      listing = s3Client.listNextBatchOfObjects (listing);
      summaries.addAll (listing.getObjectSummaries());
    }

    for (S3ObjectSummary item : summaries) {
      graphList.add(item.getKey());
    }
    return graphList;
  }

  public int getSubdirectorySize(String prefix) {
    ObjectListing listing = s3Client.listObjects( BUCKET_NAME, prefix );
    List<S3ObjectSummary> summaries = listing.getObjectSummaries();
    return summaries.size();
  }
}
