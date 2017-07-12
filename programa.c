#include "censoTAD.h"

#include <stdio.h>
#include <stdlib.h>

#define PATH_ALFABETISMO "./alfabetismo.csv"
#define PATH_PROVINCIA "./provincia.csv"
#define PATH_DEPARTAMENTO "./departamentos.csv"

int leerCsv(const char * texto, unsigned char * edad, char * analfabeto, unsigned char * tipoVivienda, char * nombreDepto, unsigned char * tipoProvincia);

int main(){

    unsigned char edad, codigoVivienda, codigoProvincia;
    char nombreDepto[DEPTO_MAX], alfabetismo;

    censoADT censo = nuevoCenso();
    if(censo == NULL){
      printf("Error al reservar memoria.");
      return 1;
    }

    char linea[81];
    int i = 1;
    while(!feof(stdin)){

      if(fgets(linea, 81, stdin) != NULL){
        if(!leerCsv(linea, &edad, &alfabetismo, &codigoVivienda, nombreDepto, &codigoProvincia))
            printf("Error en la lectura de la linea: %d", i);


      if(!ingresarDato(censo, codigoVivienda, codigoProvincia, edad, alfabetismo-1, nombreDepto))
          printf("Error al procesar el dato de la linea: %d", i);
      }
      i++;
    }

    int error = 0;
    if( !almacenarCenso(censo, PATH_ALFABETISMO, PATH_PROVINCIA, PATH_DEPARTAMENTO)){
      printf("Hubo un problema al guardar los archivos");
      error = 1;
    }

    liberarCenso(censo);

    if(error)
      return 1;

    return 0;
}

int leerCsv(const char * texto, unsigned char * edad, char * analfabeto, unsigned char * tipoVivienda, char * nombreDepto, unsigned char* tipoProvincia){
  int i = 0, estado = 0, escritura = 0, numero = 0, c;
  while((c=*(texto+i))!=0 && estado < 5 && c != '\n'){
    if(c == ','){
      switch(estado){
        case 0:
          *edad = numero;
          break;
        case 1:
          *analfabeto = numero;
          break;
        case 2:
           *tipoVivienda = numero;
           break;
        default:
          break;
      }
      estado+=1;
      numero=0;

      //printf("Cambio %d a %d\n", i, cambio);
    }else if(estado == 3){
      *(nombreDepto + escritura) = c;
        escritura++;
    }else{
      if(isdigit(c)){
        numero*=10;
        numero += c-'0';
      }else{
        return 0;
      }
    }
    i++;
  }
  *(nombreDepto + escritura) = '\0';
  *tipoProvincia = numero;
  return 1;
}
