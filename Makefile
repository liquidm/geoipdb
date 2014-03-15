all: jar

clean:
	rm -rf lib/geoipdb.jar

compile: clean
	javac `find src/ -name '*.java'`

jar: compile
	jar cvf lib/geoipdb.jar -C src/ .
