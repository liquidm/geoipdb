require 'java'
require File.expand_path('../geoipdb.jar', __FILE__)

class GeoIpDb

  def self.init(cities_file, ranges_file, cache_file)
    self.new(cities_file, ranges_file, cache_file)
  rescue java.io.FileNotFoundException
    return nil
  end

  def initialize(cities_file, ranges_file, cache_file)
    # the Java implementation does not support cache files,
    # since the java serialisation is slower than the actual csv parsing
    @jdb = Java::GeoIpDb.new(cities_file, ranges_file)
  end

  def information_for_ip(ip)
    range = @jdb.find_range_for_ip(ip)
    return nil unless range

    city = @jdb.find_city_for_ip_range(range)
    return nil unless city

    isp = range.isp_name
    build_ip_information_object(range, city, isp)
  end

  def build_ip_information_object(range, city, isp)
    info = IpInformation.new
    info.country_iso_code = city.country_iso2
    info.city_name = city.name
    info.city_code = city.city_code
    info.lng = city.lng
    info.lat = city.lat
    info.is_mobile  = range.is_mobile
    info.isp_name = isp && isp.to_sym
    info
  end

end
