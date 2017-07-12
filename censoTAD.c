#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "censoTAD.h"

#define CANT_INI 'Z'-'A'+2
#define CANT_VIVIENDAS 9
#define CANT_PROVINCIAS 24
#define DIVISION(a, b) ((b)>0?(double)(a)/(b):(b))

char * nombreProvincias[] = {"Ciudad Autonoma de Buenos Aires", "Buenos Aires", "Catamarca", "Cordoba", "Corrientes",
                             "Chaco", "Chubut", "Entre Rios", "Formosa", "Jujuy", "La Pampa", "La Rioja", "Mendoza",
                             "Misiones", "Neuquen", "Rio negro", "Salta", "San Juan", "San Luis", "Santa Cruz",
                             "Santa Fe", "Santiago del Estero", "Tucuman", "Tierra del Fuego" };

char * nombreVivienda[] = {"Casa", "Rancho", "Casilla", "Departamento", "Pieza en inquilinato", "Pieza en hotel o pension",
                           "Local no construido para habitacion", "Vivienda movil", "Persona/s viviendo en la calle"};

/* Es una estructura en la que se almacenan los datos de una vivienda. */
typedef struct vivienda {
  unsigned int habitantes;
  unsigned int analfabetos;
  unsigned long int sumaedades;
} vivienda;

/* Estructura que contendra el nombre del departamento y un vector de estructuras de tipo vivienda conlos datos de cada
  tipo de vivienda en ese departamento */
typedef struct nodoDepto {
  char *nombreDepto;
  struct vivienda viviendas[CANT_VIVIENDAS];
  struct nodoDepto * sig;
} nodoDepto;

typedef nodoDepto * TnodoDepto;

/* Estructura que contiene una matriz de punteros a la estructura nodoDepto, las filas son las provincias y las columnas son
  las letras con la que empiezan los departamentos (y la primera columna que no es letra), se usa para hacer una lista
  con todos los departamentos que comienzan con cada letra para agilizar el proceso. */
typedef struct censoCDT {
  TnodoDepto provincias[CANT_PROVINCIAS][CANT_INI];
} censoCDT;

/*Función auxiliar para ordenar las provincias*/
static int cmp (const int * a, const int * b) {
  return strcmp(nombreProvincias[*a], nombreProvincias[*b]);
}

censoADT nuevoCenso (){
  censoADT censo = calloc(sizeof(*censo), 1);
  return censo;
}

static TnodoDepto ingresarDatoR (TnodoDepto nodo, char *nombreDepto, char analfabeto, unsigned char codigoVivienda, unsigned char edad, int *flag) {
  int c;
  if (nodo == NULL || (c = strcmp(nodo->nombreDepto, nombreDepto)) > 0) {
    TnodoDepto aux = malloc (sizeof(*aux));
    if (aux == NULL)
      return nodo;
    aux->viviendas[codigoVivienda].habitantes = 1;
    aux->viviendas[codigoVivienda].analfabetos = analfabeto;
    aux->viviendas[codigoVivienda].sumaedades = edad;
    aux->sig = nodo;
    aux->nombreDepto = malloc(strlen(nombreDepto)+1);
    if(aux->nombreDepto == NULL) {
      free(aux);
      return nodo;
    }
    strcpy(aux->nombreDepto, nombreDepto);
    *flag = 1;
    return aux;
  }

  if (c == 0) {
    nodo->viviendas[codigoVivienda].habitantes++;
    nodo->viviendas[codigoVivienda].analfabetos += analfabeto;
    nodo->viviendas[codigoVivienda].sumaedades += edad;
    *flag = 1;
    return nodo;
  }

  nodo->sig = ingresarDatoR(nodo->sig, nombreDepto, analfabeto, codigoVivienda, edad, flag);
  return nodo;
}

int ingresarDato (censoADT censo, unsigned char codigoVivienda, unsigned char codigoProvincia, unsigned char edad, char analfabeto, char * nombreDepto) {
  if(codigoVivienda <= 0 || codigoVivienda > CANT_VIVIENDAS || codigoProvincia <= 0 || codigoProvincia > CANT_PROVINCIAS || (analfabeto != 1 && analfabeto != 0))
    return 0;
  codigoVivienda--;
  codigoProvincia--;
  int flag = 0;
  if (!isalpha(nombreDepto[0]))
    censo->provincias[codigoProvincia][0] = ingresarDatoR(censo->provincias[codigoProvincia][0], nombreDepto, analfabeto, codigoVivienda, edad, &flag);
  else {
    int c = toupper(nombreDepto[0])-'A'+1;
    censo->provincias[codigoProvincia][c] = ingresarDatoR(censo->provincias[codigoProvincia][c], nombreDepto, analfabeto, codigoVivienda, edad, &flag);
  }
  return flag;
}

int almacenarCenso (censoADT censo, char * direccionAlfabetismo, char * direccionProvincias, char * direccionDepartamentos) {
  FILE * archivoProvincias = fopen(direccionProvincias, "w");
  FILE * archivoAlfabetismo = fopen(direccionAlfabetismo, "w");
  FILE * archivoDepartamentos = fopen(direccionDepartamentos, "w");

  if (archivoProvincias == NULL || archivoAlfabetismo == NULL || archivoDepartamentos == NULL) {
    if (archivoProvincias != NULL)
      fclose(archivoProvincias);
    if (archivoAlfabetismo != NULL)
      fclose(archivoAlfabetismo);
    if (archivoDepartamentos != NULL)
      fclose(archivoDepartamentos);
    return 0;
  }

  unsigned int habitantesVivienda[CANT_VIVIENDAS] = {0};
  unsigned int analfabetosVivienda[CANT_VIVIENDAS] = {0};
  unsigned int habitantesProvincia, edadProvincia, analfabetosProvincia;
  unsigned int habitantesDepto, analfabetosDepto;

  /*Se crea un vector que vinculará los códigos de las provincias
  respecto del orden alfabético de las mismas*/
  int ordenProvincias[CANT_PROVINCIAS];
  for (int i = 0; i < CANT_PROVINCIAS; i++)
    ordenProvincias[i] = i;
  qsort(ordenProvincias, CANT_PROVINCIAS, sizeof(int), (int (*)(const void *, const void *))cmp);

  for (int i = 0; i < CANT_PROVINCIAS; i++) {
    habitantesProvincia = 0;
    edadProvincia = 0;
    analfabetosProvincia = 0;
    for (int j = 0; j < CANT_INI; j++) {
      TnodoDepto aux = censo->provincias[ordenProvincias[i]][j];
      while (aux != NULL) {
        habitantesDepto = 0;
        analfabetosDepto = 0;
        for (int k = 0; k < CANT_VIVIENDAS; k++) {
          habitantesDepto += aux->viviendas[k].habitantes;
          analfabetosDepto += aux->viviendas[k].analfabetos;
          edadProvincia += aux->viviendas[k].sumaedades;
          habitantesVivienda[k] += aux->viviendas[k].habitantes;
          analfabetosVivienda[k] += aux->viviendas[k].analfabetos;
        }
        habitantesProvincia += habitantesDepto;
        analfabetosProvincia += analfabetosDepto;
        fprintf(archivoDepartamentos, "%s,%s,%d,%.2f\n", nombreProvincias[ordenProvincias[i]], aux->nombreDepto, habitantesDepto, DIVISION (analfabetosDepto, habitantesDepto));
        aux = aux->sig;
      }
    }
    fprintf(archivoProvincias, "%s,%d,%.2f,%.2f\n", nombreProvincias[ordenProvincias[i]], habitantesProvincia, DIVISION(edadProvincia, habitantesProvincia),
                        DIVISION(analfabetosProvincia, habitantesProvincia));
  }

  for(int i = 0; i < CANT_VIVIENDAS; i++){
    fprintf(archivoAlfabetismo, "%d,%s,%d,%.2f\n", i+1, nombreVivienda[i], habitantesVivienda[i], DIVISION(analfabetosVivienda[i], habitantesVivienda[i]));
  }

  fclose(archivoProvincias);
  fclose(archivoAlfabetismo);
  fclose(archivoDepartamentos);
  return 1;
}

void liberarCenso (censoADT censo) {
  int i,j;
  for(i = 0; i<CANT_PROVINCIAS; i++) {
    for(j = 0; j<CANT_INI; j++) {
      TnodoDepto aux = censo->provincias[i][j];
      TnodoDepto aux2;
      while(aux != NULL) {
        free(aux->nombreDepto);
        aux2 = aux->sig;
        free(aux);
        aux = aux2;
      }
    }
  }
  free(censo);
}
