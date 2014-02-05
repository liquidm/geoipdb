import java.io.*;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;

public class GeoIpDb
{
    private static final int MAX_CITY_COUNT = 1000000;
    private static final int MAX_RANGE_COUNT = 10000000;

    HashMap<Integer, City> cities;
    ArrayList<String> isps;
    ArrayList<IpRange> ranges;
    
    private volatile boolean loaded = false;
    private IOException exceptionReadingCVS = null;

    public GeoIpDb(final String citiesFileName, final String rangesFileName) throws FileNotFoundException
    {
        cities = new HashMap<Integer, City>();
        isps = new ArrayList<String>();
        ranges = new ArrayList<IpRange>();

        Thread t = new Thread("IPDB CVS readers") {
        	@Override
        	public void run() {
        		try {
					readCitiesCSV(citiesFileName);
					readRangesCSV(rangesFileName);
				} catch (IOException e) {
					exceptionReadingCVS = e;
				}
        		synchronized (GeoIpDb.this) {
        			loaded = true;
        			notifyAll();
				}
        	};
        };
        t.setDaemon(true);
        t.start();
    }
    
    private void ensureLoaded() {
    	if (!loaded) {
    		synchronized (this) {
    			if (!loaded) {
    				try {
						wait();
					} catch (InterruptedException e) {
						throw new RuntimeException();
					}
    			}
			}
    	}
    	if (exceptionReadingCVS != null) {
    		throw new RuntimeException("Asynchronous exception in CVS readers thread", exceptionReadingCVS);
    	}
    }

    public IpRange findRangeForIp(String ip)
    {
    	ensureLoaded();
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
    	ensureLoaded();
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
    	ensureLoaded();
        return ranges;
    }

    private void readCitiesCSV(String file_name) throws IOException
    {
        CsvReader reader = new CsvReader(file_name);
        try {
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
        } finally {
        	reader.close();
        }
    }

    private void readRangesCSV(String file_name) throws IOException
    {
        CsvReader reader = new CsvReader(file_name);
        try {
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
        } finally {
        	reader.close();
        }
    }
}
