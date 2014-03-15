require 'geoipdb'

describe IPDB do

  it "sould find the sample cities correcty" do
    info = IPDB.lookup "1.1.0.254"
    info.city_code.should == 3
    info.city_name.should == 'kabul'
    info.country_iso_code.should == 'af'
    info.lat.should == 34.5167
    info.lng.should == 69.1833
    info.should_not be_mobile
  end

  it 'should return correct is_mobile information' do
    IPDB.lookup("1.0.0.1").should_not be_mobile
    IPDB.lookup("1.1.1.1").should be_mobile
  end

  it 'should return correct isp_name in ip_information' do
    IPDB.lookup("1.0.0.1").isp_name.should == :vodafone
    IPDB.lookup("1.1.1.1").isp_name.should == :o2
    IPDB.lookup("1.2.1.1").isp_name.should == :"?"
  end

end
