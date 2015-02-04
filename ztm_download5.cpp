#include "HafasBaza.hpp"
int main(int argc, char** argv)
{
	string sciezka, sciezka2, sciezka3;
	sciezka=argv[1];
	sciezka2=argv[2];
	HafasBazaSQL loader(sciezka, sciezka2);
}
