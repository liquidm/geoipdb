import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;

public class CsvReader {
  private BufferedReader reader = null;

  public CsvReader(String file_name) throws FileNotFoundException {
    //try {
      reader = new BufferedReader(new FileReader(file_name));
    //} catch (FileNotFoundException e) {
      //System.out.format("Could not load file: %s", file_name);
    //}
  }

  public String[] readLine() {
    try {
      String line = "";
      String[] token = null;

      if ((line = reader.readLine()) != null) {
        token = line.split(",");
      }

      return token;
    } catch (IOException e) {
      return null;
    }
  }
}
