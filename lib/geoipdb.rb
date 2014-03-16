require 'liquid/boot'

require 'fileutils'
require 'open-uri'
require 'singleton'
require 'uri'
require 'zlib'

java_import 'java.lang.NumberFormatException'
java_import 'java.net.InetAddress'

require File.expand_path('../geoipdb.jar', __FILE__)

class IPDB
  include Singleton

  attr_accessor :base_url

  DATA_FILES = [
    'cities.csv',
    'ip_ranges.csv',
  ]

  class IPDBError < Exception
  end

  def self.init(base_url)
    instance.update!(base_url)
  end

  def self.lookup(ip)
    instance.lookup(ip)
  end

  def initialize
    @version = Gem.loaded_specs['geoipdb'].version.to_s
    @cache_path = File.expand_path("~/.cache/geoipdb/v#{@version}")
    # by default load sample data
    sample_path = File.expand_path('../../sample_data', __FILE__)
    files = DATA_FILES.map { |file_name| File.join(sample_path, file_name) }
    self.load(*files)
  end

  def load(*files)
    @db = Java::IPDB.new(*files)
  end

  def update!(base_url)
    return nil if @initialized
    @start = Time.now.to_f
    @base_url = base_url
    @initialized = true

    $log.info("ipdb:load", cache_path: @cache_path)
    FileUtils.mkdir_p(@cache_path)

    @updating = false
    if !uptodate?
      @updating = true
      $log.info("ipdb:init", update: true)
      download_update
    end

    load_data_files
  end

  def load_data_files
    files = DATA_FILES.map { |file_name| File.join(@cache_path, file_name) }
    self.load(*files)
    self_test
    make_backup if @updating
    $log.info("ipdb:init", rt: Time.now.to_f - @start)
  rescue IPDBError => e
    $log.exception(e)
    $log.info("ipdb:init", revert: true)
    restore_backup
    load_data_files
    self_test
  end

  def lookup(ip)
    return nil unless @db
    ip = InetAddress.get_by_name(ip).address if ip.is_a?(String)
    range = @db.find_range_for_ip(ip)
    return nil unless range
    city = @db.find_city_for_ip_range(range)
    return nil unless city
    isp = range.isp_name
    info = IpInformation.new
    info.country_iso_code = city.country_iso2
    info.city_name = city.name
    info.city_code = city.city_code
    info.lng = city.lng
    info.lat = city.lat
    info.is_mobile  = range.is_mobile
    info.isp_name = isp && isp.to_sym
    info
  rescue EncodingError, NumberFormatException
    # TODO: do we really want to silence these exceptions?
    return nil
  end

  private

  def uptodate?
    days14 = 24*60*60*14 # 14days
    now = Time.now.to_f

    DATA_FILES.each do |file_name|
      path = File.join(@cache_path, file_name)
      if !File.exist?(path)
        $log.info("ipdb:check", result: false, file: file_name, reason: "#{path} does not exist")
        return false
      elsif File.stat(path).ctime.to_f + days14 < now
        $log.info("ipdb:check", result: false, file: file_name, reason: "#{path} is too old - needs update!")
        return false
      else
        $log.info("ipdb:check", result: true, file: file_name)
      end
    end
    return true
  end

  def download_update
    DATA_FILES.each do |file_name|
      download(file_name)
      unzip(file_name)
    end
  end

  def download(file_name)
    $log.info("ipdb:download", file: file_name)
    file_name = "#{file_name}.gz"
    source = URI.parse("#{@base_url}/#{file_name}")
    dest = "#{@cache_path}/#{file_name}"
    File.write(dest, source.read)
  end

  def unzip(file_name)
    $log.info("ipdb:unzip", file: file_name)
    source = File.join(@cache_path, "#{file_name}.gz")
    dest = File.join(@cache_path, file_name)
    Zlib::GzipReader.open(source) do |gz|
      File.write(dest, gz.read)
    end
  end

  def make_backup
    DATA_FILES.each do |file_name|
      path = File.join(@cache_path, file_name)
      backup_path = File.join(@cache_path, "#{file_name}.backup")
      next unless File.exists?(path)
      $log.info("ipdb:backup", path: path, backup_path: backup_path)
      FileUtils.cp(path, backup_path)
    end
  end

  def restore_backup
    DATA_FILES.each do |file_name|
      path = File.join(@cache_path, file_name)
      backup_path = File.join(@cache_path, "#{file_name}.backup")
      if File.exists?(backup_path)
        $log.info("ipdb:restore", backup_path: backup_path, path: path)
        FileUtils.cp(backup_path, path)
      else
        $log.error("ipdb:restore", reason: "#{backup_path} does not exist")
        return false
      end
    end
    return true
  end

  def self_test
    {
      '10.0.0.1'      => '--',
      '127.0.0.1'     => '--',
      '192.168.1.1'   => '--',
      '93.219.159.76' => 'de',
      '91.44.76.101'  => 'de',
      '82.113.100.1'  => 'de',
      '84.1.10.4'     => 'hu',
      '83.23.11.5'    => 'pl',
      '52.13.100.1'   => 'us',
    }.each do |ip, country|
      result = lookup(ip)
      next if country == result.country_iso_code
      raise IPDBError.new("IPDB selftest FAILED! IP lookup failed: #{ip} should be #{country} but was #{result.country_iso_code}.")
    end
    return true
  end

end

class IpInformation

  ATTRIBS = [
    :country_iso_code,
    :city_code,
    :city_name,
    :lat,
    :lng,
    :is_mobile,
    :isp_name,
  ]

  ATTRIBS.each do |attrib|
    attr_accessor attrib
  end

  def initialize(attribs = {})
    attribs.each do |key, value|
      send("#{key}=", value)
    end
  end

  def mobile?
    @is_mobile
  end

  def to_h
    Hash[ATTRIBS.map do |attrib|
      [attrib, send(attrib)]
    end]
  end

end
