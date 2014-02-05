import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;

public class CsvReader
{
    private BufferedReader reader = null;

    public CsvReader(String file_name) throws FileNotFoundException {
        reader = new BufferedReader(new FileReader(file_name));
    }

    public String[] readLine()
    {
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

	public void close() throws IOException {
		reader.close();
	}
}
