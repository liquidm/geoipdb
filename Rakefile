require "bundler/setup"
require "bundler/gem_tasks"
require "liquid/tasks"

case RUBY_PLATFORM
when 'java'
  require 'rake/javaextensiontask'
  Rake::JavaExtensionTask.new('geoipdb')
else
  require 'rake/extensiontask'
  Rake::ExtensionTask.new('geoipdb')
end

task :default => [:spec]
task :build => :compile
task :spec => :compile
