require File.expand_path(File.dirname(__FILE__)+'/spec_helper')


describe GeoIpDb do

  CACHE_FILE = 'sample_data/ipdb.cache'

  def init_db
    @db = GeoIpDb.init './sample_data/cities.csv', './sample_data/ip_ranges.csv', CACHE_FILE
  end

  it "should not throw an exception fault if data is corrupt" do
    @db = GeoIpDb.init './sample_data/cities_corrupt.csv', './sample_data/ip_ranges_corrupt.csv', CACHE_FILE
  end

  it "should not init a db object if data files are missing" do
    GeoIpDb.init( './sample_data/bla.csv', './sample_data/blubb.csv', CACHE_FILE ).should be_nil
  end

  it "should init correctly with sample data" do
    init_db
    @db.should_not be_nil
  end

  unless defined? JRUBY_VERSION
    it "initializes cache_file correctly with sample data" do
      init_db
      File.exist?(CACHE_FILE).should be_true
    end
  end

  it "sould find the sample cities correcty" do
    init_db
    #afg,no region,kabul,-1,3,34.5167,69.1833
    info = @db.information_for_ip "1.1.0.254"
    info.city_code.should == 3
    info.city_name.should == 'kabul'
    info.country_iso_code.should == 'af'
    info.lat.should == 34.5167
    info.lng.should == 69.1833
    info.should_not be_mobile
  end

  it 'should return correct is_mobile information' do
    init_db
    @db.information_for_ip("1.0.0.1").should_not be_mobile
    @db.information_for_ip("1.1.1.1").should be_mobile
  end

  it 'should return correct isp_name in ip_information' do
    init_db
    @db.information_for_ip("1.0.0.1").isp_name.should == :vodafone
    @db.information_for_ip("1.1.1.1").isp_name.should == "1vodafone2vodafone3vodafone4vodafone5vodafone1vodafone2vodafone3vodafone4vodafone5vodafone1vodafone2vodafone3vodafone4vodafone5vodafone1vodafone2vodafone3vodafone4vodafone5vodafone"[0..99].to_sym
    @db.information_for_ip("1.2.1.1").isp_name.should == :"?"
  end

  after :each do
    File.unlink CACHE_FILE if File.exist? CACHE_FILE
  end

end
