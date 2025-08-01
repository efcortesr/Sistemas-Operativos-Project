#ifndef PROCESO_H
#define PROCESO_H

#include <string>
#include <vector>

struct Proceso{
	int pid;
	int pc;
	int ax, bx, cx;
	int quantum;
	std::string estado;
	std::vector<std::string> instrucciones;
};

#endif
