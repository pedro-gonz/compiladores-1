/*********** Librerias utilizadas **************/
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>

/************* Definiciones ********************/

//Codigos
#define L_CORCHETE 91
#define R_CORCHETE 93
#define L_LLAVE 123
#define R_LLAVE 125
#define COMA 44
#define DOS_PUNTOS 58 
#define LITERAL_CADENA 256 
#define LITERAL_NUM 257
#define PR_TRUE 258
#define PR_FALSE 259
#define PR_NULL 260
#define VACIO 00
// Fin Codigos
#define TAMBUFF 	5
#define TAMLEX 		50
#define TAMHASH 	101
#define TAMTOK 50

/************* Estructuras ********************/

typedef struct entrada{
	//definir los campos de 1 entrada de la tabla de simbolos
	int compLex;
	char lexema[TAMLEX];	
	struct entrada *tipoDato; // null puede representar variable no declarada	
	// aqui irian mas atributos para funciones y procedimientos...
	
} entrada;

typedef struct {
	int compLex;
	entrada *pe;
} token;

/************* Prototipos ********************/
void insertar(entrada e);
entrada* buscar(const char *clave);
void initTabla();
void initTablaSimbolos();
void sigLex();
