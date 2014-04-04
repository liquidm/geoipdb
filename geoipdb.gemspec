# -*- encoding: utf-8 -*-

Gem::Specification.new do |spec|
  spec.name          = "geoipdb"
  spec.version       = "1.0.1"
  spec.authors       = ["LiquidM, Inc."]
  spec.email         = ["opensource@liquidm.com"]
  spec.description   = "Fast IPDB implementation for JRuby"
  spec.summary       = "Fast IPDB implementation for JRuby"
  spec.homepage      = "http://github.com/liquidm/geoipdb"
  spec.licenses      = ["MIT"]

  spec.files         = `git ls-files`.split($/)
  spec.executables   = spec.files.grep(%r{^bin/}) { |f| File.basename(f) }
  spec.test_files    = spec.files.grep(%r{^(test|spec|features)/})
  spec.require_paths = ["lib"]

  spec.add_dependency "liquid-ext"
end
