//Libreria creada por Figueroa Leonel Sebastian. Alias KonnorRK-800

#include <stdio.h>
#include <windows.h>
class serial
{
	private:
	char Puerto[7];
	HANDLE COM;
	COMMTIMEOUTS timeouts;
	//int TimeOut=50;

	public:
	serial(int); //Constructor, Le especificas el puerto serie que deseas abrir.
	int begin(int); // Abre e inicia la comunicacion serie en el puerto elegido en el constructor, ademas de usar la velocidad de baudios elegida en el parametro.
	int begin(int,int,int,int);//Lo mismo que lo anterior pero tambien puedes elegir los bits de parada, paridad y bits del paquete.
	void end(); //Cierra el puerto serie.
	find();
	findUntil();
	flush();
	parseFloat();
	parseInt();
	peek();
	print();
	println();
	int read(); //Lee un Byte del buffer y devuelve el valor numerico en ASCII
	int readBytes(char[],int); //Lees mas de 1 caracter en el puerto serie, tantos como le indiques
	void readBytesUntil(char,char[],int);//Lee tantos bytes del buffer como le hayas indicado, o hasta que encuentre un caracter finalizador que hayas indicado
	readString();
	readStringUntil();
	void setTimeout(int);// Sirve para establecer el tiempo de espera al leer el puerto serie
	write();
	serialEvent();
	~serial(); //Destruye la instancia
	
};

serial::serial(int valor)
{

	int Unidad;
	int Decena;
	int Centena;
	
	Centena=valor/100;
	Decena=(valor-(Centena*100))/10;
	Unidad=valor-(Centena*100)-(Decena*10);
	
	char PcC[2]={Centena+48,0};
	char PcD[2]={Decena+48,0};
	char PcU[2]={Unidad+48,0};
	char Port[4]={'C','O','M',0};
	
	if(valor<=9)
		{
		strcat(Puerto,Port);
		strcat(Puerto,PcU);
		}
	
	else if(valor>9 & valor<=99)
		{
			strcat(Puerto,Port);
			strcat(Puerto,PcD);
			strcat(Puerto,PcU);
		}
		else if(valor>99)
		{
			strcat(Puerto,Port);
			strcat(Puerto,PcC);
			strcat(Puerto,PcD);
			strcat(Puerto,PcU);
		}		


	
}

int serial::begin(int BPS)
{
    
	DCB dcbSerialParams = {0}; //Setea toda la estructura DBC a 0
    COMMTIMEOUTS timeouts = {0}; //Setea toda la estructura COMMTIMEOUTS a 0

    // Abre el puerto serie
    COM = CreateFile(Puerto, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (COM == INVALID_HANDLE_VALUE) 
	{
        printf("Error al abrir el puerto serie!\n");
        return 1;
	}

	//Inicializa el DBC e intenta verificar si se logro correctamente.
    dcbSerialParams.DCBlength = sizeof(DCB);
    
    if (GetCommState(COM, &dcbSerialParams) == 0)
	{
        printf("Error al obtener la configuración del puerto serie!\n");
        CloseHandle(COM);
        return 1;
    }
    // Configura la velocidad de transmision, bits del paquete, bits de parada y paridad. 
    dcbSerialParams.BaudRate = BPS;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;
    
	if (SetCommState(COM, &dcbSerialParams) == 0)
	{
        printf("Error al configurar el puerto serie!\n");
        CloseHandle(COM);
        return 1;
    }

    // Configura los tiempos de espera
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;
    
	if (SetCommTimeouts(COM, &timeouts) == 0)
	{
        printf("Error al configurar los tiempos de espera del puerto serie!\n");
        CloseHandle(COM);
        return 1;
    }
	PurgeComm(COM, PURGE_RXCLEAR | PURGE_TXCLEAR); //vacia el buffer
    // el puerto serie está abierto y configurado, se puede usar para enviar y recibir datos
    printf("Puerto serie abierto y configurado con exito!\n");


}
int serial::begin(int BPS, int ByteSize, int StopBits, int Parity)
{
	
	DCB dcbSerialParams = {0}; //Setea todo el DBC a 0
    COMMTIMEOUTS timeouts = {0};//Setea todo el COMMTIMEOUTS a 0

    // Abre el puerto serie
    COM = CreateFile(Puerto, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (COM == INVALID_HANDLE_VALUE) 
	{
        //printf("Error al abrir el puerto serie!\n");
        return -1;
	}

	//Inicializa el DBC e intenta verificar si se logro correctamente.
    dcbSerialParams.DCBlength = sizeof(DCB);
    
    if (GetCommState(COM, &dcbSerialParams) == 0)
	{
        //printf("Error al obtener la configuración del puerto serie!\n");
        CloseHandle(COM);
        return -2;
    }
    // Configura la velocidad de transmision, bits del paquete, bits de parada y paridad. 
    dcbSerialParams.BaudRate = BPS;
    dcbSerialParams.ByteSize = ByteSize;
    dcbSerialParams.StopBits = StopBits;
    dcbSerialParams.Parity = Parity;
    
	if (SetCommState(COM, &dcbSerialParams) == 0)
	{
        //printf("Error al configurar el puerto serie!\n");
        CloseHandle(COM);
        return -3;
    }

    // Configura los tiempos de espera
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;
    
	if (SetCommTimeouts(COM, &timeouts) == 0)
	{
        //printf("Error al configurar los tiempos de espera del puerto serie!\n");
        CloseHandle(COM);
        return -4;
    }

    // el puerto serie está abierto y configurado, se puede usar para enviar y recibir datos
    //printf("Puerto serie abierto y configurado con exito!\n");
    return 0;

}
void serial::end()
{
	CloseHandle(COM);
	printf("Puerto serie cerrado\n");
}

int serial::read()
{
	char buffer=0; 	 //Almaceno el caracter leído
	DWORD bytesRead; //Pongo esto pero no lo uso jeje.
	
	if (ReadFile(COM, &buffer, sizeof(buffer), &bytesRead, NULL)) //lee el primer byte del puerto serie, si la lectura fue exitosa, devuelve "TRUE" y continua con el programa
	{
		int dato=buffer; 			//transformo el caracter a un entero
		if ((dato>=0)&(dato<=127))  //Corrobora si el numero esta entre los de los caracteres ASCII.
		{
			return dato; 			//Si todo lo anterior se corrobora, devielve el dato
    	}
    	
		return -2;//devuelve -2 si no es un caracter ASCII reducido válido
    }
	
	return -1; // Si no se pudo leer el puerto serie o el buffer esta vacio -1
}

int serial::readBytes(char Buffer[], int Ndatos)
{
	DWORD bytesRead; //Aqui se guarda cuantos bytes se han leido
	
	if (ReadFile(COM, Buffer, Ndatos , &bytesRead, NULL)) //lee el primer byte del puerto serie, si la lectura fue exitosa, devuelve "TRUE" y continua con el programa.
	{													  //A demas, lee la longitud de bytes que le indicamos y lo guarda en el array buffer que especificamos como parametro.
		if(bytesRead==Ndatos) 
		{
		return 0;	//Para diagnostico, si la cantidad de bytes leidos es igual a la que se pidio, devuelve 0.
		}
	return -2; //Si no es la misma cantidad, devuelve -1 
	}
return -1;	// Si no se pudo leer el puerto serie o el buffer esta vacio -1
}

void serial::readBytesUntil(char finalizador, char Buffer[], int Ndatos)
{
	for(int i=0; i<Ndatos;i++)
	{
		int Retorno=serial::read();
		
		if((Retorno>0) & (Retorno!=finalizador))
		{
			Buffer[i]=Retorno;
		}
		else
		{	
			//i=Ndatos;
		}
	}
	
}	

void readString()
{
}

void readStringUntil()
{
}

void serial::setTimeout(int Tiempo)
{
	timeouts.ReadIntervalTimeout = Tiempo; 
	SetCommTimeouts(COM, &timeouts);
}
serial::~serial()
{
}
