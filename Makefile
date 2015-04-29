ROUTE_INFO_GEN_OBJECTS_O = src/RouteInfoGenerator/RouteInfoGenerator.o src/RouteInfoGenerator/ztmosmpor.o \
src/RouteInfoGenerator/PrzeglądanieCzyPrawidloweStareLinie.o src/RouteInfoGenerator/HTMLHeadersRouteInfo.o \
src/HTMLGenerator/HTMLGenerator.o src/stringspecial.o src/osmbase/osm_base.o src/ScheduleReader/ScheduleReader.o

RARE_TO_THICK_O = src/RareToThickRoutes/dijkstra.o src/RareToThickRoutes/dij_data.o src/RareToThickRoutes/RareToThickRoutes.o \
src/osmbase/osm_base.o src/stringspecial.o

TESTPARSE_O = src/testparse/testparse.o src/testparse/szkielet.o src/osmbase/osm_base.o src/stringspecial.o \
src/RareToThickRoutes/dijkstra.o

MRC = -std=c++11
CC = g++
all: $(ROUTE_INFO_GEN_OBJECTS_O) $(RARE_TO_THICK_O) $(TESTPARSE_O)
	$(CC) $(MRC) $(ROUTE_INFO_GEN_OBJECTS_O) -o bin/RouteInfoGenerator
	$(CC) $(MRC) $(RARE_TO_THICK_O) /usr/lib/libfcgi.a -o bin/RareToThickRoutes.fcg
	$(CC) $(MRC) $(TESTPARSE_O) -o bin/testparse
src/stringspecial.o : src/stringspecial.cpp include/stringspecial.hpp
	$(CC) $(MRC) -c -o src/stringspecial.o src/stringspecial.cpp
src/HTMLGenerator/HTMLGenerator.o : src/HTMLGenerator/HTMLGenerator.cpp include/HTMLGenerator/HTMLGenerator.hpp
	$(CC) $(MRC) -c -o src/HTMLGenerator/HTMLGenerator.o src/HTMLGenerator/HTMLGenerator.cpp
src/ScheduleReader/ScheduleReader.o : src/ScheduleReader/ScheduleReader.cpp include/ScheduleReader/ScheduleReader.hpp
	$(CC) $(MRC) -c -o src/ScheduleReader/ScheduleReader.o src/ScheduleReader/ScheduleReader.cpp
src/osmbase/osm_base.o : src/osmbase/osm_base.cpp include/osmbase/osm_base.hpp
	$(CC) $(MRC) -c -o src/osmbase/osm_base.o src/osmbase/osm_base.cpp
src/RouteInfoGenerator/HTMLHeadersRouteInfo.o : src/RouteInfoGenerator/HTMLHeadersRouteInfo.cpp include/RouteInfoGenerator/HTMLHeadersRouteinfo.hpp
	$(CC) $(MRC) -c -o src/RouteInfoGenerator/HTMLHeadersRouteInfo.o src/RouteInfoGenerator/HTMLHeadersRouteInfo.cpp
src/RouteInfoGenerator/PrzeglądanieCzyPrawidloweStareLinie.o : src/RouteInfoGenerator/PrzeglądanieCzyPrawidloweStareLinie.cpp include/RouteInfoGenerator/PrzegladanieCzyPrawidloweStareLinie.hpp
	$(CC) $(MRC) -c -o src/RouteInfoGenerator/PrzeglądanieCzyPrawidloweStareLinie.o src/RouteInfoGenerator/PrzeglądanieCzyPrawidloweStareLinie.cpp
src/RouteInfoGenerator/RouteInfoGenerator.o : src/RouteInfoGenerator/RouteInfoGenerator.cpp
	$(CC) $(MRC) -c -o src/RouteInfoGenerator/RouteInfoGenerator.o src/RouteInfoGenerator/RouteInfoGenerator.cpp
src/RouteInfoGenerator/ztmosmpor.o : src/RouteInfoGenerator/ztmosmpor.cpp include/RouteInfoGenerator/ztmosmpor.hpp
	$(CC) $(MRC) -c -o src/RouteInfoGenerator/ztmosmpor.o src/RouteInfoGenerator/ztmosmpor.cpp
	
	
src/RareToThickRoutes/dijkstra.o : src/RareToThickRoutes/dijkstra.cpp include/RareToThickRoutes/dijkstra.hpp
	$(CC) $(MRC) -c -o src/RareToThickRoutes/dijkstra.o src/RareToThickRoutes/dijkstra.cpp
src/RareToThickRoutes/dij_data.o : src/RareToThickRoutes/dij_data.cpp include/RareToThickRoutes/dij_data.hpp
	$(CC) $(MRC) -c -o src/RareToThickRoutes/dij_data.o src/RareToThickRoutes/dij_data.cpp
src/RareToThickRoutes/RareToThickRoutes.o : src/RareToThickRoutes/RareToThickRoutes.cpp
	$(CC) $(MRC) -c -o src/RareToThickRoutes/RareToThickRoutes.o src/RareToThickRoutes/RareToThickRoutes.cpp
	
src/testparse/szkielet.o : src/testparse/szkielet.cpp include/testparse/szkielet.hpp
	$(CC) $(MRC) -c -o src/testparse/szkielet.o src/testparse/szkielet.cpp
src/testparse/testparse.o : src/testparse/testparse.cpp
	$(CC) $(MRC) -c -o src/testparse/testparse.o src/testparse/testparse.cpp
	
clean:
	-rm $(ROUTE_INFO_GEN_OBJECTS_O)
	-rm $(RARE_TO_THICK_O)
	-rm $(TESTPARSE_O)