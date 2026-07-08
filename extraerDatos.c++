#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <sstream>
#include <unistd.h>
#include <cstdio>

using namespace std;

double tem_cpu()
{
	string valor;
	ifstream archivo("/sys/class/hwmon/hwmon5/temp1_input");
	if(archivo.is_open())
	{
		string linea;
		while(getline(archivo, linea))
		{
			archivo.close();
			return (stod(linea) / 1000.0);
		}
	}

	return 0.0;
}


double use_ram()
{
	ifstream archivo("/proc/meminfo");
	string linea;
	string valores[8];
	if(archivo.is_open())
	{
		for(int i=0;i<8;i++)
		{
			string contenedor;
			getline(archivo, linea);
			for(int j=0;j<linea.length();j++)
			{
				if(linea[j] >= '0' && linea[j] <= '9')
				{
					contenedor = contenedor + linea[j];
				}
			}
			valores[i] = contenedor;
		}
		archivo.close();
	}
	else 
	{
		cout<<"Hubo un error\n";
	}
	double ramConsumida{0};
	ramConsumida = stod(valores[0]) - stod(valores[2]);
	ostringstream consumida;
	consumida << fixed << setprecision(2) << ramConsumida / (1024.0 * 1024.0);

	return stod(consumida.str());
}

// extraccion de el uso de la cpu
void use_cpu(double tiempos[2])
{
	ifstream archivo("/proc/stat");

	double valores[10];
	string linea;

	if(archivo.is_open())
	{
		getline(archivo, linea);
	}

	
	int contador{0};
	string acumulador;

	for(int i=0;i<linea.length();i++)
	{
		if(linea[i] >= '0' && linea[i] <= '9') {
			acumulador += linea[i];
		}
		else
		{
			if (!acumulador.empty()) {
                valores[contador] = stod(acumulador);
                acumulador = "";
                contador++;
            }
		}
	}

	double tiempoTotal{valores[0] + valores[1] + valores[2] + valores[3]};
	double tiempoLibre{valores[3]};

	tiempos[0] = tiempoTotal;
	tiempos[1] = tiempoLibre;
}


double contro_use_cpu()
{
	double tiempos[2];
	use_cpu(tiempos);
	double t1 = tiempos[0];
	double t2 = tiempos[1];
	sleep(1);
	use_cpu(tiempos);

	float tiempoTotalPasado = t1 -  tiempos[0];
	float tiempoLibrePasado = t2 - tiempos[1];

	float resultado = (tiempoTotalPasado - tiempoLibrePasado) / tiempoTotalPasado;
	resultado = resultado * 100;

	ostringstream resultadoRedondeado;
	resultadoRedondeado << fixed << setprecision(2) << resultado;
	return stod(resultadoRedondeado.str());


}

double tem_gpu()
{
	// extraccion de temperaturas de la gpu con nvidia-smi, se necesita tener instalado los drivers de nvidia y la herramienta nvidia-smi
	// es una vercion primitiva, por el momento funcina pero si sobrepasa los 100 grados mostrara las primeras 2 sigras
	// unicamente, eso se puede considerar un error

	// refactorizar a futuro

	FILE* tuberia = popen("/bin/nvidia-smi --query-gpu=temperature.gpu,utilization.gpu,utilization.memory --format=csv,noheader,nounits", "r");
	if(!tuberia)
	{
		cout<<"Hubo un error. no se encontro la terjeta de video\n";
	}

    char buffer[128];
    while (fgets(buffer, sizeof(buffer), tuberia) != NULL) { }
	pclose(tuberia);

	// cout<<buffer<<"\n";

	int valor1 = buffer[0] - '0';
	int valor2 = buffer[1] - '0';
	int tem = (valor1 * 10) + valor2;
	return tem;
}

// refactorizar a futuro
double use_gpu()
{
	FILE* tuberia = popen("/bin/nvidia-smi --query-gpu=utilization.gpu --format=csv,noheader,nounits", "r");
	if(!tuberia)
	{
		cout<<"Hubo un error. no se encontro la terjeta de video\n";
	}

    char buffer[128];
    while (fgets(buffer, sizeof(buffer), tuberia) != NULL) { }
	pclose(tuberia);
	// cout<<buffer<<"\n";

	int valor1 = buffer[0] - '0';

	int use;
	if (buffer[1] == '\n' || buffer[1] == ',' || buffer[1] == ' ' || buffer[1] == '\0') {
		use = valor1; 
	} else {
		int valor2 = buffer[1] - '0';
		use = (valor1 * 10) + valor2;
	}

	return use;
}

// refactorizar a futuro
double use_vram()
{
	FILE* tuberia = popen("/bin/nvidia-smi --query-gpu=utilization.memory --format=csv,noheader,nounits", "r");
	if(!tuberia)
	{
		cout<<"Hubo un error. no se encontro la terjeta de video\n";
	}

    char buffer[128];
    while (fgets(buffer, sizeof(buffer), tuberia) != NULL) { }
	pclose(tuberia);
	// cout<<buffer<<"\n";

	int valor1 = buffer[0] - '0';

	int use;
	if (buffer[1] == '\n' || buffer[1] == ',' || buffer[1] == ' ' || buffer[1] == '\0') {
		use = valor1; 
	} else {
		int valor2 = buffer[1] - '0';
		use = (valor1 * 10) + valor2;
	}

	return use;
}



int main()
{
	cout<<"Prosesador use "<<contro_use_cpu()<<"%\n";
	cout<<"Tem prosesador "<<tem_cpu()<<"C \n";
	cout<<"Ram usada "<<use_ram()<<" GIB\n";
	cout<<"temp gpu "<<tem_gpu()<<"C\n";
	cout<<"use gpu "<<use_gpu()<<"%\n";
	cout<<"use vram "<<use_vram()<<"%\n";

	return 0;
}