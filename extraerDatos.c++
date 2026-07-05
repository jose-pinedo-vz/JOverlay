#include <iostream>
#include <fstream>
#include <string>

#include <iomanip>
#include <sstream>

#include <unistd.h>

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


// /proc/meminfo | head -n 5

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
int use_cpu()
{
	ifstream archivo("/proc/stat");

	double valores[10];
	string linea;

	if(archivo.is_open())
	{
		getline(archivo, linea);
		cout<<linea<<"\n";
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

	for(int i=0;i<10;i++)
	{
		cout<<valores[i]<<" 1";
	}
	int tiempoTotal{valores[0] + valores[1] + valores[2] + valores[3]};
	int tiempoLibre{valores[3]};

	int tiempos[2] = {tiempoTotal, tiempoLibre};

	return tiempos;
}


void contro_use_cpu()
{
	int tiempos1 = use_cpu();
	cout<<"se completo una vuleta de manera exitosa\n";
	sleep(1);
	int tiempos2 = use_cpu();
	cout<<"se completo dos vueltas de manera exitosa\n";


}




int main()
{
	// cout<<"Tem prosesador "<<tem_cpu()<<"\n";
	// cout<<"Ram usada "<<use_ram()<<"\n";

	use_cpu();

	return 0;
}