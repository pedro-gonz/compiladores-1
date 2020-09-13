
/*********** Inclusión de cabecera **************/
#include "anlex.h"
#include<string.h>

/************* Variables globales **************/

int consumir;			/* 1 indica al analizador lexico que debe devolver
						el sgte componente lexico, 0 debe devolver el actual */

char cad[5*TAMLEX];		// string utilizado para cargar mensajes de error
token t;				// token global para recibir componentes del Analizador Lexico

// variables para el analizador lexico

FILE *archivo;			// Fuente JSON
char buff[2*TAMBUFF];	// Buffer para lectura de archivo fuente
FILE* fichero;
char id[TAMLEX];		// Utilizado por el analizador lexico
int delantero=-1;		// Utilizado por el analizador lexico
int fin=0;				// Utilizado por el analizador lexico
int numLinea=1;			// Numero de Linea
int contador_espacio=0;
char tokens[TAMTOK];

/**************** Funciones **********************/


// Rutinas del analizador lexico

void error(const char* mensaje)
{
	printf("Lin %d: Error Lexico. %s.\n",numLinea,mensaje);	
	strcpy(tokens, "");
	contador_espacio = 0;
}

void sigLex()
{
	int i=0;
	char c=0;
	int acepto=0;
	int estado=0;
	char msg[41];
	entrada e;

	while((c=fgetc(archivo))!=EOF)
	{
		if (c==' '){
			contador_espacio += 1;
			continue;	
		}else if(c=='\t'){
			contador_espacio += 4;
		}
		else if(c=='\n')
		{
			numLinea++;
			fputs("\n", fichero);
			contador_espacio = 0;
			continue;
		}
		else if (c=='{')
		{
			t.compLex='{';
			t.pe=buscar("{");			
			strcat(tokens, "L_LLAVE ");
			break;
		}
		else if (c=='}')
		{
			t.compLex='}';
			t.pe=buscar("}");		
			strcat(tokens, "R_LLAVE ");
			break;
		}
		else if (c == '"')
		{
			//es una cadena literal
			i=0;
			do{
				c=fgetc(archivo);
				id[i]=c;
				i++;
			}while(c != '"');
	c=fgetc(archivo);
			id[i-1]='\0';
			if (c!=EOF)
				ungetc(c,archivo);
			else
				c=0;
			t.pe=buscar(id);
			t.compLex=t.pe->compLex;
			if (t.pe->compLex==-1)
			{
				strcpy(e.lexema,id);
				
				e.compLex = LITERAL_CADENA;
				insertar(e);
				t.pe=buscar(id);
				t.compLex=LITERAL_CADENA;
			}
			
			strcat(tokens, "STRING ");

			break;
		}
		
		
		else if (isalpha(c))
		{	i=0;
			do{
				id[i]=tolower(c);
				i++;
				c=fgetc(archivo);
				
			}while(isalpha(c));
			id[i]='\0';
			if (c!=EOF)
				ungetc(c,archivo);
			else
				c=0;
			t.pe=buscar(id);
			t.compLex=t.pe->compLex;
			
			
			if (t.pe->compLex==-1)
			{
				t.compLex=VACIO;
				printf("\nLin %d: Error Lexico. '%s' no esperado.\n",numLinea,id);	
				strcat(tokens, "ERROR ");
			}
			
			
			if(strcmp(t.pe->lexema, "true") == 0 || strcmp(t.pe->lexema, "TRUE") == 0){
				strcat(tokens, "PR_TRUE ");
			}else if(strcmp(t.pe->lexema, "false") == 0 || strcmp(t.pe->lexema, "FALSE") == 0){
				strcat(tokens, "PR_FALSE ");
			}else if(strcmp(t.pe->lexema, "null") == 0 || strcmp(t.pe->lexema, "NULL") == 0){
				strcat(tokens, "PR_NULL ");
			}
			break;
		
		}

		else if (isdigit(c))
		{
				//es un numero
				i=0;
				estado=0;
				acepto=0;
				id[i]=c;
				strcpy(tokens,"NUMBER ");
				while(!acepto)
				{
					
					switch(estado){
					case 0: //una secuencia netamente de digitos, puede ocurrir . o e
						c=fgetc(archivo);
						if (isdigit(c))
						{
							id[++i]=c;
							estado=0;
							
						}
						else if(c=='.'){
							id[++i]=c;
							estado=1;
						}
						else if(tolower(c)=='e'){
							id[++i]=c;
							estado=3;
						}
						else{
							estado=6;
						}
						break;
					
					case 1://un punto, debe seguir un digito
						c=fgetc(archivo);
												
						if (isdigit(c))
						{
							
							id[++i]=c;
							estado=2;
						}
						else{
							strcpy(tokens,"");
							sprintf(msg,"No se esperaba '%c'",c);
							estado=-1;

						}
						break;
					case 2://la fraccion decimal, pueden seguir los digitos o e
						c=fgetc(archivo);
						if (isdigit(c))
						{
							id[++i]=c;
							estado=2;
							
						}
						else if(tolower(c)=='e')
						{
							id[++i]=c;
							estado=3;
						}
						else
							estado=6;
						break;
					case 3://una e, puede seguir +, - o una secuencia de digitos
						c=fgetc(archivo);
						if (c=='+' || c=='-')
						{
							id[++i]=c;
							estado=4;
						}
						else if(isdigit(c))
						{
							id[++i]=c;
							estado=5;
						}
						else{
							strcpy(tokens,"");
							sprintf(msg,"No se esperaba '%c'",c);
							estado=-1;

						}
						break;
					case 4://necesariamente debe venir por lo menos un digito
						c=fgetc(archivo);
						if (isdigit(c))
						{
							id[++i]=c;
							estado=5;
						}
						else{
							strcpy(tokens,"");
							sprintf(msg,"No se esperaba '%c'",c);
							
							estado=-1;

						}
						break;
					case 5://una secuencia de digitos correspondiente al exponente
						c=fgetc(archivo);
						if (isdigit(c))
						{
							id[++i]=c;
							estado=5;
					
						}
						else{
							estado=6;
						}break;
					case 6://estado de aceptacion, devolver el caracter correspondiente a otro componente lexico
						if (c!=EOF)
							ungetc(c,archivo);
						else
							c=0;
						id[++i]='\0';
						acepto=1;
						t.pe=buscar(id);
						if (t.pe->compLex==-1)
						{
							strcpy(e.lexema,id);
							e.compLex=LITERAL_NUM;
							insertar(e);
							t.pe=buscar(id);
						}
						t.compLex=LITERAL_NUM;
						break;
					case -1:
						if (c==EOF){
							strcpy(tokens,"");
							error("No se esperaba el fin de archivo");							
						}else{
							strcpy(tokens,"");
							error(msg);
						}
						exit(1);
					}
				}
				
				
			break;
		}

		else if (c==',')
		{
			t.compLex=',';
			t.pe=buscar(",");
			
			strcat(tokens, "COMA ");
			break;
		}

		else if (c==':')
		{
			t.compLex=':';
			t.pe=buscar(":");
			
			strcat(tokens, "DOS_PUNTOS ");
			break;
		}
		else if (c=='[')
		{
			t.compLex='[';
			t.pe=buscar("[");
			
			strcat(tokens, "L_CORCHETE ");
			break;
		}
		else if (c==']')
		{
			t.compLex=']';
			t.pe=buscar("]");
			
			strcat(tokens, "R_CORCHETE ");
			break;
		}

		else if (c!=EOF)
		{
			sprintf(msg,"%c no esperado",c);
			strcpy(tokens,"");
			error(msg);
		}
	}
	if (c==EOF)
	{
		t.compLex=EOF;
		sprintf(e.lexema,"EOF");
		t.pe=&e;
	}
	
}

int main(int argc,char* args[])
{
	// inicializar analizador lexico

	initTabla();
	initTablaSimbolos();
	int z;
	
	if(argc > 1)
	{
		if (!(archivo=fopen(args[1],"rt")))
		{
			printf("Archivo no encontrado.\n");
			exit(1);
		}
			
		fichero = fopen("output", "a");	
		
		while (t.compLex!=EOF){
			sigLex();
			z = 0;
			while(z < contador_espacio){
				fputs(" ", fichero);
				z = z + 1;
			}
			fputs(tokens, fichero);
			contador_espacio = 0;
			strcpy(tokens,"");
			
			printf("%s", t.pe->lexema);
		}
		fclose(archivo);
		fclose(fichero);
	}else{
		printf("Debe pasar como parametro el path al archivo fuente.\n");
		exit(1);
	}

	return 0;
}
