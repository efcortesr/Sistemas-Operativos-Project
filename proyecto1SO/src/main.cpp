#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <queue>
#include <map>
#include "../include/proceso.h"

using namespace std;

// Función para parsear el valor de un registro o número
int obtenerValor(Proceso &p, const string &operando)
{
    if (operando == "AX")
        return p.ax;
    else if (operando == "BX")
        return p.bx;
    else if (operando == "CX")
        return p.cx;
    else
    {
        try
        {
            return stoi(operando); // Es un número
        }
        catch (const std::invalid_argument &e)
        {
            cout << "Error: operando inválido '" << operando << "'" << endl;
            return 0;
        }
    }
}

// Función para asignar valor a un registro
void asignarRegistro(Proceso &p, const string &registro, int valor)
{
    if (registro == "AX")
        p.ax = valor;
    else if (registro == "BX")
        p.bx = valor;
    else if (registro == "CX")
        p.cx = valor;
}

// Función para limpiar espacios y comas de un string
string limpiarOperando(const string &operando)
{
    string limpio = operando;
    // Remover espacios al inicio y final
    while (!limpio.empty() && (limpio.front() == ' ' || limpio.front() == '\t'))
    {
        limpio.erase(0, 1);
    }
    while (!limpio.empty() && (limpio.back() == ' ' || limpio.back() == '\t' || limpio.back() == ','))
    {
        limpio.pop_back();
    }
    return limpio;
}

// Ejecuta una instrucción
void ejecutarInstruccion(Proceso &p)
{
    if (p.pc >= (int)p.instrucciones.size())
        return;

    string instr = p.instrucciones[p.pc];

    // Debug: mostrar la instrucción que se está parseando
    cout << "   [Debug] Parseando: '" << instr << "'" << endl;

    istringstream iss(instr);
    string comando;
    iss >> comando;

    if (comando == "ADD")
    {
        string resto;
        getline(iss, resto); // Obtener el resto de la línea

        // Buscar la coma para separar los operandos
        size_t coma_pos = resto.find(',');
        if (coma_pos != string::npos)
        {
            string reg1 = limpiarOperando(resto.substr(0, coma_pos));
            string reg2 = limpiarOperando(resto.substr(coma_pos + 1));

            cout << "   [Debug] ADD: reg1='" << reg1 << "', reg2='" << reg2 << "'" << endl;

            int valor1 = obtenerValor(p, reg1);
            int valor2 = obtenerValor(p, reg2);
            asignarRegistro(p, reg1, valor1 + valor2);
        }
    }
    else if (comando == "SUB")
    {
        string resto;
        getline(iss, resto);

        size_t coma_pos = resto.find(',');
        if (coma_pos != string::npos)
        {
            string reg1 = limpiarOperando(resto.substr(0, coma_pos));
            string reg2 = limpiarOperando(resto.substr(coma_pos + 1));

            int valor1 = obtenerValor(p, reg1);
            int valor2 = obtenerValor(p, reg2);
            asignarRegistro(p, reg1, valor1 - valor2);
        }
    }
    else if (comando == "MUL")
    {
        string resto;
        getline(iss, resto);

        size_t coma_pos = resto.find(',');
        if (coma_pos != string::npos)
        {
            string reg1 = limpiarOperando(resto.substr(0, coma_pos));
            string reg2 = limpiarOperando(resto.substr(coma_pos + 1));

            int valor1 = obtenerValor(p, reg1);
            int valor2 = obtenerValor(p, reg2);
            asignarRegistro(p, reg1, valor1 * valor2);
        }
    }
    else if (comando == "INC")
    {
        string reg;
        iss >> reg;
        reg = limpiarOperando(reg);
        int valor = obtenerValor(p, reg);
        asignarRegistro(p, reg, valor + 1);
    }
    else if (comando == "JMP")
    {
        string destino;
        iss >> destino;
        destino = limpiarOperando(destino);

        try
        {
            int pos = stoi(destino);
            if (pos >= 0 && pos < (int)p.instrucciones.size())
            {
                p.pc = pos;
                return; // salto válido: NO incrementar PC al final
            }
            else
            {
                cout << "[Instrucción inválida: JMP a posición fuera de rango (" << pos << ")]" << endl;
                p.pc++; // avanzar a la siguiente
                return; // ← IMPORTANTE: evitar el p.pc++ global
            }
        }
        catch (const invalid_argument &)
        {
            cout << "[Error: operando no válido en JMP: '" << destino << "']" << endl;
            p.pc++; // avanzar a la siguiente
            return; // ← IMPORTANTE
        }
    }
    else if (comando == "NOP")
    {
        // No hacer nada
    }
    else
    {
        cout << "[Instrucción desconocida: " << comando << "]\n";
    }

    p.pc++;
}

// Carga las instrucciones de un proceso desde su archivo correspondiente
vector<string> cargarInstrucciones(int pid)
{
    vector<string> instrucciones;
    string nombreArchivo = to_string(pid) + ".txt";
    ifstream archivo(nombreArchivo);
    string linea;

    if (!archivo)
    {
        cerr << "Error: No se pudo abrir el archivo de instrucciones " << nombreArchivo << endl;
        exit(1);
    }

    while (getline(archivo, linea))
    {
        if (!linea.empty())
        {
            instrucciones.push_back(linea);
        }
    }

    archivo.close();
    return instrucciones;
}

// Parser mejorado para el archivo de procesos según la sintaxis especificada
vector<Proceso> cargarProcesosDesdeArchivo(const string &nombreArchivo)
{
    vector<Proceso> procesos;
    ifstream archivo(nombreArchivo);
    string linea;

    if (!archivo)
    {
        cerr << "Error: No se pudo abrir el archivo " << nombreArchivo << endl;
        exit(1);
    }

    while (getline(archivo, linea))
    {
        if (linea.empty())
            continue;

        Proceso p;
        // Inicializar valores por defecto
        p.pc = 0;
        p.ax = 0;
        p.bx = 0;
        p.cx = 0;
        p.quantum = 1;
        p.estado = "Listo";

        // Parsear línea: PID:1,AX=2,BX=3,CX=1,Quantum=2
        istringstream iss(linea);
        string token;

        // Obtener tokens separados por comas
        while (getline(iss, token, ','))
        {
            size_t pos = token.find(':');
            if (pos != string::npos)
            {
                // Es PID
                string pidStr = token.substr(pos + 1);
                p.pid = stoi(pidStr);
            }
            else
            {
                pos = token.find('=');
                if (pos != string::npos)
                {
                    string clave = token.substr(0, pos);
                    string valor = token.substr(pos + 1);

                    if (clave == "AX")
                        p.ax = stoi(valor);
                    else if (clave == "BX")
                        p.bx = stoi(valor);
                    else if (clave == "CX")
                        p.cx = stoi(valor);
                    else if (clave == "Quantum")
                        p.quantum = stoi(valor);
                }
            }
        }

        // Cargar instrucciones desde archivo correspondiente
        p.instrucciones = cargarInstrucciones(p.pid);
        procesos.push_back(p);
    }

    archivo.close();
    return procesos;
}

// Mostrar el estado de un proceso
void mostrarEstado(const Proceso &p, const string &prefijo = "")
{
    cout << prefijo << "Proceso " << p.pid
         << ": PC=" << p.pc
         << ", AX=" << p.ax
         << ", BX=" << p.bx
         << ", CX=" << p.cx
         << ", Estado=" << p.estado << endl;
}

// Función principal
int main(int argc, char *argv[])
{
    if (argc != 3 || string(argv[1]) != "-f")
    {
        cerr << "Uso: " << argv[0] << " -f <archivo_procesos.txt>" << endl;
        return 1;
    }

    string archivoEntrada = argv[2];
    vector<Proceso> procesos = cargarProcesosDesdeArchivo(archivoEntrada);

    if (procesos.empty())
    {
        cerr << "No se encontraron procesos en el archivo." << endl;
        return 1;
    }

    // Crear cola de procesos listos
    queue<Proceso> colaListos;
    for (auto &p : procesos)
    {
        colaListos.push(p);
    }

    cout << "===== SIMULADOR DE PLANIFICACIÓN ROUND-ROBIN =====" << endl;
    cout << "Procesos cargados: " << procesos.size() << endl;
    cout << "Algoritmo: Round-Robin con quantum individual por proceso" << endl;
    cout << "=====================================================" << endl
         << endl;

    Proceso *procesoAnterior = nullptr;
    int cicloGlobal = 0;

    while (!colaListos.empty())
    {
        Proceso actual = colaListos.front();
        colaListos.pop();

        cout << ">>> CICLO " << ++cicloGlobal << " <<<" << endl;

        // Mostrar cambio de contexto
        if (procesoAnterior != nullptr && procesoAnterior->pid != actual.pid)
        {
            cout << "[CAMBIO DE CONTEXTO]" << endl;
            mostrarEstado(*procesoAnterior, "Guardando estado -> ");
        }

        mostrarEstado(actual, "Cargando estado -> ");

        // Cambiar estado a Ejecutando
        actual.estado = "Ejecutando";
        cout << "Proceso " << actual.pid << " pasa a estado: " << actual.estado << endl;
        cout << "Quantum asignado: " << actual.quantum << " ciclos" << endl;
        cout << "Instrucciones totales: " << actual.instrucciones.size() << endl;
        cout << "----------------------------------------------" << endl;

        int ciclosUsados = 0;
        int quantumRestante = actual.quantum;

        // Ejecutar hasta agotar quantum o terminar instrucciones
        while (quantumRestante > 0 && actual.pc < (int)actual.instrucciones.size())
        {
            cout << "Ciclo " << (ciclosUsados + 1) << "/" << actual.quantum
                 << " - PC=" << actual.pc
                 << " | AX=" << actual.ax
                 << " | BX=" << actual.bx
                 << " | CX=" << actual.cx << endl;

            if (actual.pc < (int)actual.instrucciones.size())
            {
                cout << "Ejecutando: " << actual.instrucciones[actual.pc] << endl;
                ejecutarInstruccion(actual);
            }

            ciclosUsados++;
            quantumRestante--;
            cout << endl;
        }

        // Verificar si el proceso terminó
        if (actual.pc >= (int)actual.instrucciones.size())
        {
            actual.estado = "Terminado";
            cout << "*** PROCESO " << actual.pid << " TERMINADO ***" << endl;
            mostrarEstado(actual, "Estado final -> ");
            delete procesoAnterior;
            procesoAnterior = nullptr;
        }
        else
        {
            // El proceso no terminó, vuelve a la cola
            actual.estado = "Listo";
            cout << "Quantum agotado. Proceso " << actual.pid
                 << " vuelve a la cola de listos." << endl;
            mostrarEstado(actual, "Estado actual -> ");
            colaListos.push(actual);

            // Guardar referencia del proceso actual
            delete procesoAnterior;
            procesoAnterior = new Proceso(actual);
        }

        cout << "==============================================" << endl
             << endl;
    }

    cout << "===== SIMULACIÓN COMPLETADA =====" << endl;
    cout << "Todos los procesos han terminado su ejecución." << endl;

    delete procesoAnterior;
    return 0;
}
