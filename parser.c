//librerias
#include "anlex.h"

// Variables 
token t;                 // token global para recibir componentes del Analizador Lexico
FILE *archivo;           // Fuente JSON
char mensaje[41];        // Mensaje de error.
short bandera_error=0;   // Bandera para mensajes.


//prototipos de funciones para cada NO TERMINAL
void json(void);
void element(void);
void array(void);
void array_prima(void);
void element_list(void);
void el_prima(void);
void object(void);
void o_prima(void);
void attribute_list(void);
void attribute(void);
void al_prima(void);
void attribute_name(void);
void attribute_value(void);


void error_mensaje(char* mensaje){
    bandera_error=1;
    printf("Lin %d:\t\nERROR: La sintaxis no es correcta. \n\t%s.\n",linea(),mensaje);	
}

void getToken(void) {
    sigLex();
}

void match(char* c){
    if (strcmp(t.compLex, c)==0)
        getToken(); 
    else
        error_mensaje("ERROR: error al hacer match.");
}

void json(){
    element();

    match("EOF");
}

void element(){
    if(strcmp(t.compLex,"L_LLAVE")==0){
        object();
    }else if (strcmp(t.compLex,"L_CORCHETE")==0){
        array();
    }else if(strcmp(t.compLex,"R_CORCHETE")==0 || strcmp(t.compLex,"R_LLAVE")==0||strcmp(t.compLex,"COMA")==0){
        sprintf(mensaje,"Se esperaba los siguientes tokens \"{\" o \"[\" no \"%s\"", t.pe->lexema);
        error_mensaje(mensaje);
    }else 
        getToken();
}

void array(){
    if(strcmp(t.compLex,"L_CORCHETE")==0){
        match("L_CORCHETE");
        array_prima();
    }else if(strcmp(t.compLex,"R_CORCHETE")==0||strcmp(t.compLex,"R_LLAVE")==0||strcmp(t.compLex,"COMA")==0){
        sprintf(mensaje,"Se esperaba se esperaba los siguientes tokens \"[\" no \"%s\"", t.pe->lexema);
        error_mensaje(mensaje);
    }else 
        getToken();
}

void array_prima(){
    if(strcmp(t.compLex,"R_CORCHETE")==0){
        match("R_CORCHETE"); 
    }else if(strcmp(t.compLex,"L_CORCHETE")==0||strcmp(t.compLex,"L_LLAVE")==0){
        element_list();
        match("R_CORCHETE");
    }else if(strcmp(t.compLex,"R_LLAVE")==0){
        sprintf(mensaje,"Se esperaba se esperaba los siguientes tokens \"[\" o \"]\" o \"{\" no \"%s\"", t.pe->lexema);
        error_mensaje(mensaje);
    }else
        getToken();
}

void element_list(){
    if(strcmp(t.compLex,"L_CORCHETE")==0||strcmp(t.compLex,"L_LLAVE")==0){
        element();
        el_prima();    
    }else if(strcmp(t.compLex,"R_CORCHETE")==0){
        sprintf(mensaje,"Se esperaba  se esperaba los siguientes tokens \"[\" o \"{\" no \"%s\"", t.pe->lexema);
        error_mensaje(mensaje);
    }else 
        getToken();    
}

void el_prima(){
    if(strcmp(t.compLex,"COMA")==0){
        match("COMA");
        element();
        el_prima();
    }else if(strcmp(t.compLex,"R_CORCHETE")!=0){       
        getToken(); 
    }
}

void object(){
    if(strcmp(t.compLex,"L_LLAVE")==0){
        match("L_LLAVE");
        o_prima(); 
    }else if(strcmp(t.compLex,"R_CORCHETE")==0||strcmp(t.compLex,"R_LLAVE")==0||strcmp(t.compLex,"COMA")==0){
        sprintf(mensaje,"Se esperaba se esperaba los siguientes tokens \"{\" no \"%s\"", t.pe->lexema);
        error_mensaje(mensaje);
    }else 
        getToken();
}

void o_prima(){
    if(strcmp(t.compLex,"R_LLAVE")==0){
        match("R_LLAVE");   
    }else if(strcmp(t.compLex,"LITERAL_CADENA")==0){
        attribute_list(); 
        match("R_LLAVE");
    }else if(strcmp(t.compLex,"R_CORCHETE")==0||strcmp(t.compLex,"COMA")==0){
        sprintf(mensaje,"Se esperaba se esperaba los siguientes tokens \"}\" o \"string\" no \"%s\"", t.pe->lexema);
        error_mensaje(mensaje);
    }else 
        getToken();     
}

void attribute_list(){
    if(strcmp(t.compLex,"LITERAL_CADENA")==0){
        attribute();
        al_prima(); 
    }else if(strcmp(t.compLex,"R_LLAVE")==0){
        sprintf(mensaje,"Se esperaba los siguientes tokens \"string\" no \"%s\"", t.pe->lexema);
        error_mensaje(mensaje);
    }else 
        getToken(); 
}

void attribute(){
    if(strcmp(t.compLex,"LITERAL_CADENA")==0){
        attribute_name();
        match("DOS_PUNTOS");
        attribute_value(); 
    }else if(strcmp(t.compLex,"R_LLAVE")==0||strcmp(t.compLex,"COMA")==0){
        sprintf(mensaje,"Se esperaba los siguientes tokens \"string\" no \"%s\"", t.pe->lexema);
        error_mensaje(mensaje);
    }else 
        getToken();    
}

void al_prima(){
    if(strcmp(t.compLex,"COMA")==0){
        match("COMA");
        attribute();
        al_prima();
    }else if(strcmp(t.compLex,"R_LLAVE")!=0){ 
        getToken(); 
    }       
}

void attribute_name(){
    if(strcmp(t.compLex,"LITERAL_CADENA")==0){
        match("LITERAL_CADENA");
    }else
        getToken();
}

void attribute_value(){
    if(strcmp(t.compLex,"L_LLAVE")==0||strcmp(t.compLex,"L_CORCHETE")==0){    
        element();
    }
    else if(strcmp(t.compLex,"LITERAL_CADENA")==0){
        match("LITERAL_CADENA");
    }
    else if(strcmp(t.compLex,"LITERAL_NUM")==0){
        match("LITERAL_NUM");
    }
    else if(strcmp(t.compLex,"PR_TRUE")==0){        
        match("PR_TRUE");
    }
    else if(strcmp(t.compLex,"PR_FALSE")==0){        
        match("PR_FALSE");
    }
    else if(strcmp(t.compLex,"PR_NULL")==0){        
        match("PR_NULL");
    }
    else if(strcmp(t.compLex,"R_LLAVE")==0||strcmp(t.compLex,"COMA")==0||strcmp(t.compLex,"DOS_PUNTOS")==0){
        sprintf(mensaje,"Se esperaba los siguientes tokens \"{\" o \"[\" o \"string\" o \"number\" o \"true\" o \"false\" o \"null\" no \"%s\"", t.pe->lexema);
        error_mensaje(mensaje);
    }else 
        getToken(); 
}

void inicio(void){
    json(); 
    if(strcmp(t.compLex,"EOF")!=0) error_mensaje("ERROR: No se espera EOF.");
}

void parser(){
    while (strcmp(t.compLex,"EOF")!=0){
        getToken();
        inicio();
    }   
}

int main(int argc,char* args[]){
    
    initTabla();

    initTablaSimbolos();
     
    if(argc > 1){

    	if ( !( archivo=fopen(args[1],"rt") ) ){
                printf("Archivo no encontrado.\n");
                exit(1);
    	}

    	while (strcmp(t.compLex,"EOF")!=0){
                parser();
                if(bandera_error==0)
                    printf("La sintaxis de la fuente es correcta.\n");
    	}

    	fclose(archivo);

    }else{
    	printf("Debe pasar la direccion del archivo fuente\n");
    	exit(1);
    }

    return 0;
}
