import java.io.FileNotFoundException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;

public class IPDB
{
    private static final int MAX_CITY_COUNT = 1000000;
    private static final int MAX_RANGE_COUNT = 10000000;

    HashMap<Integer, City> cities;
    ArrayList<String> isps;
    ArrayList<IpRange> ranges;

    public IPDB(String citiesFileName, String rangesFileName) throws FileNotFoundException
    {
        cities = new HashMap<Integer, City>();
        isps = new ArrayList<String>();
        ranges = new ArrayList<IpRange>();

        readCitiesCSV(citiesFileName);
        readRangesCSV(rangesFileName);
    }

    public IpRange findRangeForIp(String ip)
    {
        if (ranges.isEmpty()) {
            System.out.println("ERROR: DB has no ranges data. Can not search!");
            return null;
        }

        int index = 0;
        IpRange search = new IpRange(ip, "0");
        index = Collections.binarySearch(ranges, search);
        if (index < 0)
            return null;

        return ranges.get(index);
    }

    public City findCityForIpRange(IpRange range)
    {
        if (range == null) {
            System.out.println("Cannot find city for no given range, right?");
            return null;
        }
        if (cities.isEmpty()) {
            System.out.println("ERROR: DB has no city data. Can not search!");
            return null;
        }

        if (range.cityCode == 0) {
            System.out.format("ERROR: Could not find city with index: %d\n", range.cityCode);
        }

        return cities.get(range.cityCode);
    }

    public ArrayList<IpRange> get_ranges()
    {
        return ranges;
    }

    private void readCitiesCSV(String file_name) throws FileNotFoundException
    {
        CsvReader reader = new CsvReader(file_name);
        String[] line = null;
        City city = null;

        reader.readLine(); // skip first line

        while ((line = reader.readLine()) != null) {
            if (cities.size() >= MAX_CITY_COUNT){
                System.out.format("ERROR: MAX_CITY_COUNT = %d limit reached - mek it bigger  :-(\n", MAX_CITY_COUNT);
                return;
            }
            city = new City(line);
            cities.put(city.cityCode, city);
        }
    }

    private void readRangesCSV(String file_name) throws FileNotFoundException
    {
        CsvReader reader = new CsvReader(file_name);
        String[] line = null;

        reader.readLine(); // skip first line

        while ((line = reader.readLine()) != null) {
            if (line.length < 5)
                continue;

            if (ranges.size() >= MAX_RANGE_COUNT){
                System.out.format("ERROR: MAX_RANGE_COUNT = %d limit reached - mek it bigger  :-(\n", MAX_RANGE_COUNT);
                return;
            }

            ranges.add(new IpRange(line));
        }
    }
}
