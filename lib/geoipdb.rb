require 'ip_information'

if defined?(JRUBY_VERSION)
  require 'jgeoipdb'
else
  require 'cgeoipdb'
end
