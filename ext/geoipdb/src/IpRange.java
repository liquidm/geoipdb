public class IpRange implements Comparable<IpRange>
{
    long from;
    long to;
    boolean isMobile;
    int cityCode;
    String ispName;

    public IpRange(String[] rangeValues)
    {
        this.from = ipToLong(rangeValues[0]);
        this.to = ipToLong(rangeValues[1]);
        this.isMobile = conTypeToBool(rangeValues[2]);
        this.cityCode = Integer.parseInt(rangeValues[3]);
        // Use only the first 100 chars to be compliant with the c implementation
        this.ispName = rangeValues[4].length() > 100 ? rangeValues[4].substring(0, 100) : rangeValues[4];
    }

    public IpRange(String from, String to)
    {
        this.from = ipToLong(from);
        this.to = ipToLong(to);
    }

    public int getCityCode()
    {
        return cityCode;
    }

    public void setCityCode(int cityCode)
    {
        this.cityCode = cityCode;
    }

    public long getFrom()
    {
        return from;
    }

    public long getTo()
    {
        return to;
    }

    public boolean getIsMobile()
    {
        return isMobile;
    }

    public String getIspName()
    {
        return ispName;
    }

    private boolean conTypeToBool(String conType)
    {
        return (conType.length() > 0) && (conType.charAt(0) == 'm');
    }

    private long ipToLong(String ip)
    {
        long result = 0;

        if (!(ip == null || ip.equals(""))) {
            for (String octet : ip.split("\\.")) {
                result = (result << 8) | Integer.parseInt(octet);
            }
        }

        return result;
    }

    @Override
    public int compareTo(IpRange other)
    {
        if (other == null) {
            return 0;
        }

        if (other.from > 0 && other.to > 0 && this.from > 0 && this.to > 0) {
            if (other.from < this.from)
                return 1;
            else if (other.from > this.to)
                return -1;
            else
                return 0;
        } else if (other.to == 0 && this.to > 0) {
            if (other.from < this.from)
                return 1;
            else if (other.from > this.to)
                return -1;
            else
                return 0;
        } else if (this.to == 0 && other.to > 0) {
            if (this.from < other.from)
                return 1;
            else if (this.from > other.from)
                return -1;
            else
                return 0;
        } else if (other.to == 0 && this.to == 0) {
            return (int)(other.from - this.from);
        }

        return 0;
    }
}
