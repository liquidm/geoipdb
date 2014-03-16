import java.io.FileNotFoundException;
import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;

public class IPDB
{
    HashMap<Integer, City> cities;
    ArrayList<IpRange> ranges;

    public IPDB(String citiesFileName, String rangesFileName)
        throws FileNotFoundException
    {
        cities = new HashMap<Integer, City>();
        ranges = new ArrayList<IpRange>();
        readCitiesCSV(citiesFileName);
        readRangesCSV(rangesFileName);
    }

    public IpRange findRangeForIp(byte[] ip)
    {
        IpRange search = new IpRange(ip);
        int index = Collections.binarySearch(ranges, search);
        if (index < 0)
            return null;
        return ranges.get(index);
    }

    public City findCityForIpRange(IpRange range)
    {
        return cities.get(range.cityCode);
    }

    public ArrayList<IpRange> get_ranges()
    {
        return ranges;
    }

    private void readCitiesCSV(String file_name)
        throws FileNotFoundException
    {
        CsvReader reader = new CsvReader(file_name);
        String[] line = null;
        City city = null;
        reader.readLine(); // skip first line
        while ((line = reader.readLine()) != null) {
            city = new City(line);
            cities.put(city.cityCode, city);
        }
    }

    private void readRangesCSV(String file_name)
        throws FileNotFoundException
    {
        CsvReader reader = new CsvReader(file_name);
        String[] line = null;
        reader.readLine(); // skip first line
        while ((line = reader.readLine()) != null) {
            if (line.length < 5)
                continue;
            ranges.add(new IpRange(line));
        }
    }
}
