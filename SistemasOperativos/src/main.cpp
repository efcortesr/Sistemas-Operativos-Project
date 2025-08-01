#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <queue>
#include "../include/proceso.h"

using namespace std;

// Ejecuta una instrucción
void ejecutarInstruccion(Proceso& p) {
    if (p.pc >= p.instrucciones.size()) return;

    string instr = p.instrucciones[p.pc];

    if (instr == "ADD_AX_BX") {
        p.ax += p.bx;
    } else if (instr == "INC_AX") {
        p.ax++;
    } else if (instr == "SUB_CX_1") {
        p.cx -= 1;
    } else if (instr == "NOP") {

    } else {
        cout << "[Instrucción desconocida: " << instr << "]\n";
    }

    p.pc++;
}

// Carga procesos desde un archivo .txt
vector<Proceso> cargarProcesosDesdeArchivo(const string& nombreArchivo) {
    vector<Proceso> procesos;
    ifstream archivo(nombreArchivo);
    string linea;

    if (!archivo) {
        cerr << "Error: No se pudo abrir el archivo " << nombreArchivo << endl;
        exit(1);
    }

    while (getline(archivo, linea)) {
        istringstream iss(linea);
        Proceso p;
        string instruccion;

        iss >> p.pid >> p.pc >> p.ax >> p.bx >> p.cx >> p.quantum >> p.estado;

        // Todos los procesos inician Listos en la cola
        p.estado = "Listo";

        while (iss >> instruccion) {
            p.instrucciones.push_back(instruccion);
        }

        procesos.push_back(p);
    }

    return procesos;
}

// Función principal    
int main(int argc, char* argv[]) {
    if (argc != 3 || string(argv[1]) != "-f") {
        cerr << "Uso: " << argv[0] << " -f <archivo_procesos.txt>" << endl;
        return 1;
    }

    string archivoEntrada = argv[2];
    vector<Proceso> procesos = cargarProcesosDesdeArchivo(archivoEntrada);

    queue<Proceso> cola;
    for (auto& p : procesos) {
        cola.push(p);
    }

    cout << "===== Simulación Round-Robin =====\n";
    cout << "Quantum individual por proceso\n\n";

    Proceso* proceso_anterior = nullptr;

    while (!cola.empty()) {
        Proceso actual = cola.front();
        cola.pop();

        if (actual.pc >= actual.instrucciones.size()) {
            continue;
        }

        // Guardar estado del proceso anterior si hay cambio de proceso
        if (proceso_anterior != nullptr && proceso_anterior->pid != actual.pid) {
            cout << "----------------------------------------------\n";
            cout << "[Guardando estado de Proceso " << proceso_anterior->pid
                 << ": PC=" << proceso_anterior->pc
                 << ", AX=" << proceso_anterior->ax
                 << ", BX=" << proceso_anterior->bx
                 << ", CX=" << proceso_anterior->cx
                 << ", Estado=" << proceso_anterior->estado << "]\n";
        }

        // Mostrar que el proceso estaba Listo antes de ejecutar
        cout << "[Cargando estado de Proceso " << actual.pid
             << ": PC=" << actual.pc
             << ", AX=" << actual.ax
             << ", BX=" << actual.bx
             << ", CX=" << actual.cx
             << ", Estado=" << actual.estado << "]\n";

        int quantum_actual = actual.quantum;
        int ciclos = 0;

        cout << "\n>>> Cambio de contexto <<<\n";
        cout << "Proceso " << actual.pid << " pasa de " 
             << actual.estado << " -> Ejecutando\n\n";

        // Cambiar estado a Ejecutando
        actual.estado = "Ejecutando";

        // Ejecutar hasta agotar quantum o terminar instrucciones
        while (ciclos < quantum_actual && actual.pc < actual.instrucciones.size()) {
            cout << "   PC=" << actual.pc
                 << " | AX=" << actual.ax
                 << " | BX=" << actual.bx
                 << " | CX=" << actual.cx
                 << " | Estado=" << actual.estado << "\n";
            cout << "   Instrucción: " << actual.instrucciones[actual.pc] << "\n";

            ejecutarInstruccion(actual);
            ciclos++;
        }

        // Verificar si terminó o vuelve a la cola
        if (actual.pc >= actual.instrucciones.size()) {
            actual.estado = "Terminado";
            cout << "\n[Proceso " << actual.pid << " ha terminado]\n";
            cout << "Estado final -> PC=" << actual.pc
                 << ", AX=" << actual.ax
                 << ", BX=" << actual.bx
                 << ", CX=" << actual.cx
                 << ", Estado=" << actual.estado << "\n\n";
            proceso_anterior = nullptr;
        } else {
            actual.estado = "Esperando";
            cola.push(actual);
            delete proceso_anterior;
            proceso_anterior = new Proceso(actual);
            cout << "[Proceso " << actual.pid << " espera próximo turno]"
                 << " -> Estado=" << actual.estado << "\n\n";
        }
    }

    cout << "===== Todos los procesos han terminado =====\n";

    delete proceso_anterior;
    return 0;
}
