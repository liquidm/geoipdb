# GeoIPDB

Fast GeoIPDB implementation for JRuby.

[![Gem Version](https://badge.fury.io/rb/geoipdb.png)](http://badge.fury.io/rb/geoipdb)
[![Build Status](https://secure.travis-ci.org/liquidm/geoipdb.png)](http://travis-ci.org/liquidm/geoipdb)
[![Code Climate](https://codeclimate.com/github/liquidm/geoipdb.png)](https://codeclimate.com/github/liquidm/geoipdb)
[![Dependency Status](https://gemnasium.com/liquidm/geoipdb.png)](https://gemnasium.com/liquidm/geoipdb)

## Installation

Add this line to your application's Gemfile:

    gem 'geoipdb'

And then execute:

    $ bundle

Or install it yourself as:

    $ gem install geoipdb

## Usage

    db = IpDb.init "city_codes.csv", "ip_city.txt", "ip_city.cache"
    ip_info = db.information_for_ip("178.0.0.1")
    ip_info.inspect
    => #<IpInformation:0x101385c78 @city_name="eschborn", @city_code="ax5", @lng=8.55, @country_iso_code="de", @lat=50.133333, @is_mobile=true>

## Contributing

1. Fork it ( http://github.com/liquidm/geoipdb/fork )
2. Create your feature branch (`git checkout -b my-new-feature`)
3. Commit your changes (`git commit -am 'Added some feature'`)
4. Push to the branch (`git push origin my-new-feature`)
5. Create new Pull Request
