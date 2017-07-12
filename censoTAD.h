
typedef struct censoCDT * censoADT;

censoADT nuevoCenso();

/* Ingresa los datos de una linea a censo.
	Si no lo pudo agregar porque el codigo de vivienda o el codigo de provincia es invalido retorna 0.
	Si no hay memoria retorna 0.
	Si lo puede agregar retorna 1*/
int ingresarDato (censoADT censo, unsigned char codigoVivienda, unsigned char codigoProvincia, unsigned char edad, char analfabeto, char * nombreDepto);

/* Almacena los datos procesados del censo.
	Si no se pudieron crear los archivos o almacernar los datos en los mismos retorna 0
	En caso contrario retorna 1*/
int almacenarCenso (censoADT censo, char * direccionAlfabetismo, char * direccionProvincias, char * direccionDepartamentos);
void liberarCenso (censoADT censo);
