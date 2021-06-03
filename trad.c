
#include "anlex.h"

token t; 			
FILE *archivo; 		
FILE *salida; 		
char trad[41]; 		
short pos_tab=0; 	
char mensaje[41]; 		
short bandera_error=0; 



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

char* tabulador(char*);
void destabulador(char*);


void error_mensaje(char* mensaje){
    bandera_error=1;
    printf("Lin %d:\t\nERROR: La sintaxis no es correcta. \n\t%s.\n",linea(),mensaje);	
    
}

char* tabulador(char* texto){
    char *tabs= calloc(10000,sizeof(texto));
    for(short i=0;i<pos_tab;i++){
        strcat(tabs,"\t");
    }
    strcat(tabs,texto);
    return tabs;
}

void destabulador(char* texto){   
    char *pr = texto, *pw = texto;
    while (*pr) {
        *pw = *pr++;
        pw += (*pw != '\t');
    }
    *pw = '\0';
}

void getToken(void) {
    sigLex();
}


void match(char* c){
    if (strcmp(t.compLex, c)==0){
        getToken(); 
    }else
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
        strcpy(trad,tabulador(trad));
        pos_tab--; 
    }
    else if(strcmp(t.compLex,"R_CORCHETE")==0||strcmp(t.compLex,"R_LLAVE")==0||strcmp(t.compLex,"COMA")==0){
     
        sprintf(mensaje,"Se esperaba se esperaba los siguientes tokens \"[\" no \"%s\"", t.pe->lexema);
        error_mensaje(mensaje);
    }
    else getToken();
}

void array_prima(){
    if(strcmp(t.compLex,"R_CORCHETE")==0){
        match("R_CORCHETE"); //
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
        pos_tab++; 
        element();
        el_prima();
    }
    else if(strcmp(t.compLex,"R_CORCHETE")!=0){       
        getToken(); 
    }
}

void object(){
    if(strcmp(t.compLex,"L_LLAVE")==0){
        match("L_LLAVE");
        o_prima(); 
        pos_tab--; 
    }
    else if(strcmp(t.compLex,"R_CORCHETE")==0||strcmp(t.compLex,"R_LLAVE")==0||strcmp(t.compLex,"COMA")==0){
        sprintf(mensaje,"Se esperaba se esperaba los siguientes tokens \"{\" no \"%s\"", t.pe->lexema);
        error_mensaje(mensaje);
    }
    else getToken();
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
        char* aux= calloc(1000,sizeof(char));
        aux=t.pe->lexema;
        aux++;
        aux[strlen(aux)-1] = '\0';
        
        sprintf(trad,"<%s>",aux);
		
        strcpy(trad,tabulador(trad)); 
		
        fputs(trad,salida); 
		attribute_name();
        match("DOS_PUNTOS");
		
        attribute_value(); 
		
        destabulador(trad); 

        sprintf(trad,"</%s>\n", aux);
        fputs(trad,salida); 
    }
    else if(strcmp(t.compLex,"R_LLAVE")==0||strcmp(t.compLex,"COMA")==0){
        sprintf(mensaje,"Se esperaba los siguientes tokens \"string\" no \"%s\"", t.pe->lexema);
        error_mensaje(mensaje);
    }
    else getToken();    
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
        fputs("\n",salida);
        pos_tab++; 
        element();
    }
    else if(strcmp(t.compLex,"LITERAL_CADENA")==0){
        fputs(t.pe->lexema,salida);
        match("LITERAL_CADENA");
    }
    else if(strcmp(t.compLex,"LITERAL_NUM")==0){
        fputs(t.pe->lexema,salida);
        match("LITERAL_NUM");
    }
    else if(strcmp(t.compLex,"PR_TRUE")==0){
        fputs(t.pe->lexema,salida);
        match("PR_TRUE");
    }
    else if(strcmp(t.compLex,"PR_FALSE")==0){
        fputs(t.pe->lexema,salida);
        match("PR_FALSE");
    }
    else if(strcmp(t.compLex,"PR_NULL")==0){
        fputs(t.pe->lexema,salida);
        match("PR_NULL");
    }
    else if(strcmp(t.compLex,"R_LLAVE")==0||strcmp(t.compLex,"COMA")==0||strcmp(t.compLex,"DOS_PUNTOS")==0){
        sprintf(mensaje,"Se esperaba los siguientes tokens \"{\" o \"[\" o \"string\" o \"number\" o \"true\" o \"false\" o \"null\" no \"%s\"", t.pe->lexema);
        error_mensaje(mensaje);
    }
    else getToken(); 
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
            salida = fopen( "salida.txt", "w" );
            parser();
            if(bandera_error==0)
                printf("La sintaxis de la fuente es correcta.\n");
            else
                printf("La sintaxis de la fuente no es correcta\n");
			fclose(salida);
		}
		fclose(archivo);
	}else{
    	printf("Debe pasar la direccion del archivo fuente\n");
    	exit(1);
    }
    return 0;
}
