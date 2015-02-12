g++ database1.cpp src/stringspecial.cpp -std=c++11 -o bin/database1 -lmysqlcppconn
g++ hafas1.cpp src/stringspecial.cpp /usr/local/lib/libfcgi.a -o bin/hafas1.fcg -std=c++11 -Werror -lmysqlcppconn
g++ ztm_download5.cpp src/stringspecial.cpp -std=c++11 -Werror -o bin/ztm_download5 -lmysqlcppconn
g++ dijkstra.cpp src/stringspecial.cpp -std=c++11 -Werror -o bin/dijkstra -lmysqlcppconn
