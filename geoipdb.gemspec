# -*- encoding: utf-8 -*-

Gem::Specification.new do |spec|
  spec.name          = "geoipdb"
  spec.version       = "0.5.6"
  spec.authors       = ["LiquidM, Inc."]
  spec.email         = ["opensource@liquidm.com"]
  spec.description   = "Fast GeoIpDb implementation for Ruby"
  spec.summary       = "Fast GeoIpDb implementation for Ruby"
  spec.homepage      = "http://github.com/liquidm/geoipdb"
  spec.licenses      = ["MIT"]

  spec.files         = `git ls-files`.split($/)
  spec.executables   = spec.files.grep(%r{^bin/}) { |f| File.basename(f) }
  spec.test_files    = spec.files.grep(%r{^(test|spec|features)/})
  spec.require_paths = ["lib", "ext"]

  if RUBY_PLATFORM =~ /java/
    spec.platform = "java"
    spec.files << "lib/geoipdb.jar"
  else
    spec.extensions = ["ext/geoipdb/extconf.rb"]
  end

  spec.add_development_dependency "rake-compiler"
end
