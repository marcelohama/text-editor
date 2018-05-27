#include <conio.h>
#include <stdlib.h>
#include <allegro.h>

/*=============================================================*\
| DEFINIÇÕES DE MACROS                                          |
| Resolucao_X & Resolucao_Y - É a resolução usada pelo programa |
\*=============================================================*/
#define Resolucao_X 480
#define Resolucao_Y 480
/*=============================================================*/

/*=============================================================*\
| DECLARAÇÃO DAS ESTRUTURAS E VARIÁVEIS DE CONTROLE             |
| Coluna - Lista das colunas que guarda os caracteres           |
| Linha - Lista que guarda endereços para estruturas Coluna     |
| Bitmap - Lista que guarda bitmaps para o modo gráfico         |
| Matriz - É o ponteiro-mestre do arquivo a ser gerado          |
| X,Y,V,H - Ponteiros para manipulações gerais de linha/coluna  |
| e inteiros para guardar as posições do cursor                 |
| pal,menu,bmp,seta - Ponteiros para estrutura-paleta de cor,   |
| para guardar bitmaps, para manipular imagens em bitmap e para |
| guardar o desenho do curso                                    |
| nome - string para guardar o nome do arquivo em aberto        |
| r,g,b - Variáveis para o controle da cor das fontes do texto  |
\*=============================================================*/
typedef struct Coluna
{
	char Letra;
	struct Coluna *Cpro;
	struct Coluna *Cant;
} *pt_coluna;
typedef struct Linha
{
	struct Coluna *Head;
	struct Linha *Lpro;
	struct Linha *Lant;
} *pt_linha;
typedef struct Bitmap
{
	BITMAP *Tela;
	struct Bitmap *Bpro;
	struct Bitmap *Bant;
} *pt_bmp;
pt_linha  Matriz;
pt_linha  Y;
pt_coluna X;
PALLETE   *pal;
BITMAP 	  *menu, *seta;
pt_bmp    bmp;
char nome[20];
unsigned int H,V,r=255,g=255,b=255;
/*=============================================================*/

/*=============================================================*\
| ROTINAS/FUNÇOES DE INICIALIZAÇÃO E FINALIZAÇÃO DO EDITOR      |
| Inicialização - Gera um novo ponteiro para um início de texto |
| e seta as variáveis de controle em seus valores primários     |
| Inicializa_Allegro - Inicia a allegro e seus componentes      |
| Fechar - Finaliza a allegro e fecha o programa                |
\*=============================================================*/
void Inicializa_Matriz()
{
	if(Matriz!=NULL) free(Matriz);
	Matriz=(pt_linha)malloc(sizeof(struct Linha));
	Matriz->Head=(pt_coluna)malloc(sizeof(struct Coluna));
	Matriz->Head->Cpro=NULL;
	Matriz->Lpro=NULL;
	Matriz->Lant=NULL;
	Y=Matriz;
	X=Matriz->Head;
	V=16; H=9;
}
void Inicializa_Allegro()
{
	allegro_init();
	install_keyboard();
	set_color_depth(32);
	install_mouse();
	position_mouse(SCREEN_H/2, SCREEN_W/2);
	set_gfx_mode(GFX_AUTODETECT, Resolucao_X, Resolucao_Y, 0, 0);
	seta=load_bitmap("cursor.bmp",*pal);
	menu=load_bitmap("menu.bmp",*pal);
	if(bmp!=NULL) free(bmp);
	bmp=malloc(sizeof(struct Bitmap));
	bmp->Bpro=NULL;
	bmp->Bant=NULL;
	bmp->Tela=create_bitmap(Resolucao_X,Resolucao_Y);
	rectfill(bmp->Tela,0,0,Resolucao_X,Resolucao_Y,makecol(255,0,255));
	rect(bmp->Tela,6,14,Resolucao_X-7,Resolucao_Y-7,makecol(255,255,255));
	textout(bmp->Tela,font,"Editando...",10,5,makecol(255,255,255));
}
void Fechar()
{
	allegro_exit();
	exit(0);
}
/*=============================================================*/

/*=============================================================*\
| FUNÇÕES DAS MANIPULAÇÕES DE ARQUIVO                           |
| Imprime_Txt - Rotina para imprimir um txt exterior, usado na  |
| impressão do help                                             |
| Nome_Arq - Rotina que recebe uma string e faz dela o nome a   |
| ser usado na criação ou gravação de um texto                  |
| Imprime - Rotina que printa o texto no bitmap recebido como   |
| parâmetro                                                     |
| Gravar - Rotina que grava as letras do texto-matriz recebido  |
| no ponteiro e gera um txt com o nome igual à string recebida  |
| Carregar - Rotina que carrega um texto salvo anteriormente a  |
| fim de realizar modificações                                  |
\*=============================================================*/
int Conta_No(pt_linha ptl)
{
	int i=0;
	pt_coluna ptc;
	for(ptc=ptl->Head->Cpro;ptc!=NULL;ptc=ptc->Cpro)
		i++;
	return i;
}
int No_Atual(pt_linha ptl, pt_coluna ptc)
{
	int i=0;
	pt_coluna c;
	for(c=ptl->Head;c!=ptc;c=c->Cpro)
		i++;
	return i;
}
void Reposiciona(pt_coluna *c, int L)
{
	int i=0;
	pt_coluna ptc=(*c);
	while(i<=L)
	{
		ptc=ptc->Cpro;
		if(ptc->Cpro==NULL) break;
		i++;
	}
	(*c)=ptc;
}
void Imprime_Txt(char arq[])
{
    FILE *fp;
    char caracter;
    int h=0,v=0;
	fp=fopen(arq,"r");
	clear_bitmap(screen);
	while(caracter!=EOF)
	{
	    caracter=fgetc(fp);
	    if(caracter!=10&&caracter!=13&&caracter!=EOF)
	    {
	        textprintf(screen,font,10+h,10+v,makecol(255,v*3/4,100),"%c",caracter);
            h+=8;
        }
        if(caracter==10)
        {
            v+=10;
            h=0;
        }
    }
    fclose(fp);
	readkey();
}
void Nome_Arq(BITMAP *tela, char nome[])
{
	char tecla;
	int i;
	rectfill(tela,5,Resolucao_Y-20,Resolucao_X-5,Resolucao_Y-5,makecol(0,0,0));
	textout(tela,font,"NOME DO ARQUIVO: ",10,Resolucao_Y-15,makecol(r,g,b));
	for(i=0;i<20;i++) nome[i]='\0';
 	for(i=0;i<20;i++)
	{
		tecla=readkey();
		if(tecla!=13)
  		{
	 		if(tecla!=27 && tecla!=8)
	 		{
  				textprintf(tela,font,(i+1)*8+140,Resolucao_Y-15,makecol(r,g,b),"%c",tecla);
    			nome[i]=tecla;
			}
			else i--;
		}
		else break;
	}
}
void Imprime(BITMAP *tela, pt_linha *l)
{
    pt_coluna ptc=(*l)->Head->Cpro;
    pt_linha  ptl=(*l);
    while(ptl!=NULL)
    {
        Y=ptl;
        while(ptc!=NULL)
        {
            X=ptc;
            textprintf(tela,font,H,V,makecol(r,g,b),"%c",ptc->Letra);
            H+=9;
            if(H>Resolucao_X-18) {H=9; V+=9;}
            ptc=ptc->Cpro;
        }
        ptl=ptl->Lpro;
        if(ptl!=NULL) {V+=8; H=9; ptc=ptl->Head->Cpro;}
    }
}
void Gravar(pt_linha *l, char nome[])
{
    pt_coluna ptc;
    pt_linha  ptl=(*l);
    FILE *arq;
    arq=fopen(nome,"w");
    if(arq!=NULL)
    {
        while(ptl!=NULL)
        {
           	for(ptc=ptl->Head->Cpro;ptc!=NULL&&No_Atual(ptl,ptc)<=52;ptc=ptc->Cpro)
               	fputc(ptc->Letra,arq);
        	if(ptc==NULL)
         		fputc(10,arq);
       		ptl=ptl->Lpro;
        }
        fclose(arq);
        return;
    }
    else return;
}
void Carregar(pt_linha *l, char nome[])
{
	if(*l!=NULL) Inicializa_Matriz();
	FILE *arq;
	arq=fopen(nome, "r+");
    if(arq!=NULL)
    {
    	char caracter=fgetc(arq);
    	pt_linha  ptl=(*l);
    	pt_coluna ptc=NULL,
		  		  ptant;
        while(caracter!=EOF)
        {
            if(caracter==10)
            {
                ptl->Lpro=(pt_linha)malloc(sizeof(struct Linha));
                ptl->Lpro->Head=(pt_coluna)malloc(sizeof(struct Coluna));
                ptl->Lpro->Lant=ptl;
                ptl->Lpro->Lpro=NULL;
                ptl->Lpro->Head->Cpro=NULL;
                ptl->Lpro->Head->Cant=NULL;
                ptc=ptl->Lpro->Head->Cpro;
                ptl=ptl->Lpro;
			}
            else
            {
                ptc=(pt_coluna)malloc(sizeof(struct Coluna));
                ptc->Letra=caracter;
                ptc->Cpro=NULL;
                if(ptl->Head->Cpro==NULL)
                {
                    ptc->Cant=ptl->Head;
                    ptant=ptc;
                    ptl->Head->Cpro=ptc;
                }
                else
                {
                    ptc->Cant=ptant;
                    ptc->Cant->Cpro=ptc;
                    ptant=ptc;
                }
                ptc=ptc->Cpro;
            }
            caracter=fgetc(arq);
        }
        X=ptc; Y=ptl;
        fclose(arq);
        Imprime(bmp->Tela,&Matriz);
    }
    else
    {
    	textout(screen,font,"Arquivo Inexistente",148,Resolucao_Y-15,makecol(r,g,b));
    	readkey();
	}
}
/*=============================================================*/

/*=============================================================*\
| FUNÇÕES DE INSERÇÃO/REMOÇÃO DE CARACTERES NA MATRIZ           |
| Imput - Aloca letras, pontuações, números ou símbolos         |
| Enter - Aloca um novo nó na lista das linhas                  |
| BackSpace - Desaloca um nó na lista de Linha ou de Coluna,    |
| dependendo do ponteiro recebido                               |
\*=============================================================*/
void Imput(pt_linha *l, pt_coluna *c, char info)
{
    pt_coluna pt=(pt_coluna)malloc(sizeof(struct Coluna)),
              ptc=*c;
	pt_linha ptl=*l;
    pt->Letra=info;
    if(ptc==ptl->Head)
    {
        pt->Cant=ptl->Head;
        if(ptl->Head->Cpro!=NULL)
        	pt->Cpro=ptl->Head->Cpro;
		else
  			pt->Cpro=NULL;
    	(*l)->Head->Cpro=pt;
    }
    else
    {
        pt->Cpro=ptc->Cpro;
        pt->Cant=ptc;
        if(ptc->Cpro!=NULL)
            ptc->Cpro->Cant=pt;
 		(*c)->Cpro=pt;
    }
    (*c)=pt;
}
void Quebra_Linha(pt_linha *l, pt_coluna *c, char info)
{
	pt_linha ptl, pt;
	pt_coluna ptc;
	pt=(pt_linha)malloc(sizeof(struct Linha));
	pt->Head=(pt_coluna)malloc(sizeof(struct Coluna));
	pt->Head->Cpro=(pt_coluna)malloc(sizeof(struct Coluna));
	pt->Head->Cpro->Letra=info;
	if((*c)->Cpro!=NULL) (*c)->Cpro->Cant = pt->Head->Cpro;
	pt->Head->Cpro->Cpro=(*c)->Cpro;
	(*c)->Cpro=pt->Head->Cpro;
	pt->Head->Cant=(*c);
	if((*l)->Lpro!=NULL) (*l)->Lpro->Lant=pt;
	pt->Lpro=(*l)->Lpro;
	(*l)->Lpro=pt;
	pt->Lant=(*l);
	(*c)=pt->Head->Cpro;
	(*l)=pt;
}
void Enter(pt_linha *l, pt_coluna *c)
{
    pt_linha pt=(pt_linha)malloc(sizeof(struct Linha)),
    		 ptl=*l;
	pt_coluna ptc=*c;
	pt->Head=(pt_coluna)malloc(sizeof(struct Coluna));
	pt->Head->Cpro=ptc->Cpro;
	pt->Head->Cant=NULL;
	if(ptc->Cpro!=NULL)
		ptc->Cpro->Cant=pt->Head;
    ptc->Cpro=NULL;
    pt->Lant=ptl;
    pt->Lpro=ptl->Lpro;
    if(ptl->Lpro!=NULL)
        ptl->Lpro->Lant=pt;
    ptl->Lpro=pt;
	(*c)=pt->Head;
	(*l)=pt;
}
void BackSpace(pt_linha *l, pt_coluna *c)
{
    pt_linha  ptl=*l;
    pt_coluna ptc=*c,
    		  ptaux=*c;
    if(ptl==Matriz && ptc==ptl->Head);
    else
    {
        if(ptc!=ptl->Head)
        {
        	ptc->Cant->Cpro=ptc->Cpro;
            if(ptc->Cpro!=NULL)
            	ptc->Cpro->Cant=ptc->Cant;
            (*c)=ptc->Cant;
            free(ptc);
        }
        else
        {
        	ptaux=ptl->Lant->Head;
        	while(ptaux->Cpro!=NULL)
        		ptaux=ptaux->Cpro;
    		(*c)=ptaux;
    		if(ptc->Cpro!=NULL)
    			ptc->Cpro->Cant=ptaux;
    		ptaux->Cpro=ptc->Cpro;
            if(ptl->Lpro!=NULL)
                ptl->Lpro->Lant=ptl->Lant;
            if(ptl->Lant!=NULL)
                ptl->Lant->Lpro=ptl->Lpro;
            (*l)=ptl->Lant;
            free(ptl->Head);
            free(ptl);
        }
    }
}
/*=============================================================*/

/*=============================================================*\
| FUNÇÕES DO GERENCIADOR GRÁFICO                                |
| Cursor - Desenha o cursor na posição X/Y especifcada          |
| Conta_No - Conta o número de letras que a linha atual possui  |
| No_Atual - Retorna a posição do cursor na linha atual         |
| Reposiciona - Reposiciona o cursor em linhas que quebram      |
| Verifica_Linha - Verifica se há mais texto após o cursor      |
| Imput_Grafico - Imprime no bitmap especificado o caracter que |
| foi recebido como parâmetro                                   |
| Enter_Grafico - Atualiza o cursor e as posições para uma nova |
| linha                                                         |
| BackSpace_Grafico - Imprime um quadrado na posição anterior a |
| fim de "apagar" o caracter impresso anteriormente             |
\*=============================================================*/
void Cursor(BITMAP *cursor, BITMAP *texto)
{
	clear_bitmap(cursor);
	line(cursor,H-1,V-1,H-1,V+9,makecol(255,255,255));
	draw_sprite(cursor,texto,0,0);
}
void Verifica_Linha(BITMAP *texto, pt_coluna ptc, pt_linha ptl, int h, int v)
{
	for(ptc=ptc->Cpro;ptc!=NULL;ptc=ptc->Cpro)
	{
		textprintf(texto,font,h,v,makecol(r,g,b),"%c",ptc->Letra);
		h+=9;
		if(h>Resolucao_X-18)
  		{
    		h=9;
      		v+=8;
    	}
	}
	rectfill(texto,h,v,Resolucao_X-8,v+8,makecol(255,0,255));
	h=9; v+=8;
	for(ptl=ptl->Lpro;ptl!=NULL;ptl=ptl->Lpro)
	{
		for(ptc=ptl->Head->Cpro;ptc!=NULL;ptc=ptc->Cpro)
		{
			textprintf(texto,font,h,v,makecol(r,g,b),"%c",ptc->Letra);
			h+=9;
			if(h>Resolucao_X-18)
  			{
    			h=9;
      			v+=8;
    		}
		}
		rectfill(texto,h,v,Resolucao_X-8,v+8,makecol(255,0,255));
		v+=8; h=9;
	}
}
void Imput_Grafico(BITMAP *cursor, BITMAP *texto, char C)
{
	if(H>Resolucao_X-18)
 	{
  		{H=9; V+=8;}
		textprintf(texto,font,H,V,makecol(r,g,b),"%c",C);
	}
	else textprintf(texto,font,H,V,makecol(r,g,b),"%c",C);
	draw_sprite(cursor,texto,0,0);
	H+=9;
	if(H>Resolucao_X-9) {H=9; V+=8;}
}
void Enter_Grafico(BITMAP *cursor, BITMAP *texto)
{
	rectfill(texto,H,V,Resolucao_X-8,V+8,makecol(0,0,0));
	H=9; V+=8;
}
void BackSpace_Grafico(BITMAP *cursor, BITMAP *texto, pt_coluna ptc, pt_linha ptl)
{
	pt_coluna c;
	if(H==9)
	{
		if(V!=16)
		{
			rectfill(texto,9,V,Resolucao_X-8,Resolucao_Y-8,makecol(255,0,255));
			V-=8;
			for(c=ptl->Head->Cpro;c!=ptc;c=c->Cpro)
			{
				if(c==NULL) {H-=9; break;}
			   	textprintf(texto,font,H,V,makecol(r,g,b),"%c",c->Letra);
	    	  	H+=9;
   				if(H>Resolucao_X-18) {H=9; V+=8;}
  			}
  			H+=9;
		}
	}
	else
	{
		rectfill(texto,H,V,H-8,V+8,makecol(255,0,255));
		H-=9;
	}
}
/*=============================================================*/

/*=============================================================*\
| LOOPING PRINCIPAL DO PROGRAMA E GERENCIAMENTO DE TECLAS       |
| Captura_Tecla - Recebe as teclas e em função de seus valores  |
| em ASCII, manipula a Matriz ou insere/retira caracteres       |
| Cor_Fonte - Muda as variáveis r, g e b para mudar a cor da    |
| fonte do texto                                                |
\*=============================================================*/
void Captura_Tecla()
{
    char tecla;
    Cursor(screen, bmp->Tela);
    set_mouse_sprite(seta);
    for(;;)
    {
    	textprintf(screen,font,150,5,makecol(255,255,255),"L=%2.d C=%2.d",(V-8)/8,H/9);
    	textprintf(screen,font,300,5,makecol(255,255,255),"<ESC> para retornar");
		show_mouse(screen);
        tecla=readkey();
    	switch(tecla)
    	{
           	case 27: return;
        	case 13: if(V<Resolucao_Y-24)
        			 {
 						 Enter(&Y, &X);
        				 Enter_Grafico(screen, bmp->Tela);
        				 Verifica_Linha(bmp->Tela,X,Y,H,V);
        			     Cursor(screen, bmp->Tela);
    			     }
               	     break;
        	case 8 : BackSpace(&Y, &X);
         			 BackSpace_Grafico(screen,bmp->Tela,X,Y);
			 		 Verifica_Linha(bmp->Tela,X,Y,H,V);
 				     Cursor(screen, bmp->Tela);
               	     break;
        	default: 
     			 	if(key[KEY_HOME])
          			{
          				while(X!=Y->Head)
       					{
            				X=X->Cant;
				            H-=9;
            				if(H==0) {H=468; V-=8;}
        				}
             			Cursor(screen, bmp->Tela);
                		break;
            		}
					if(key[KEY_UP])
					{
     					if(Y!=Matriz || No_Atual(Y,X)>=(Resolucao_X-12)/9)
          				{
      						Y=Y->Lant; X=Y->Head;
             				H=9; V-=8;
                       		Cursor(screen, bmp->Tela);
                     	}
          				break;
          			}
					if(key[KEY_LEFT])
					{
     					if(X!=Y->Head)
          				{
			                X=X->Cant; H-=9;
			                if(H<7) {H=9; V-=8;}
                   			Cursor(screen, bmp->Tela);
                  		}
          				break;
              		}
					if(key[KEY_RIGHT])
					{if(X==NULL)textout(screen,font,"!",50,50,makecol(255,255,255));
     					if(X->Cpro!=NULL)
          				{
              				if(H>Resolucao_X-20) {H=9; V+=8;}
              				else {X=X->Cpro; H+=9;}
                     		Cursor(screen, bmp->Tela);
                       	}
          				break;
              		}
					if(key[KEY_END])
					{
 						while(X->Cpro!=NULL)
       					{
            				X=X->Cpro;
            				H+=9;
            				if(H>Resolucao_X-18) {H=9; V+=8;}
        				}
        				Cursor(screen,bmp->Tela);
       					break;
        			}
					if(key[KEY_DOWN])
					{
						if(Y->Lpro!=NULL)
      					{
           					Y=Y->Lpro; X=Y->Head;
         					H=9; V+=8;
                   			Cursor(screen, bmp->Tela);
               			}
          				break;
              		}
              		if(key[KEY_PGDN])
              		{
              			// page up
              			break;
          			}
              		if(key[KEY_PGUP])
              		{
              			// page down
              			break;
              		}
              		if(V<Resolucao_Y-20)
              		{
              			if(H>=Resolucao_X-20) Quebra_Linha(&Y, &X, tecla);
               			else Imput(&Y, &X, tecla);
           				Imput_Grafico(screen, bmp->Tela, tecla);
         				Verifica_Linha(bmp->Tela,X,Y,H,V);
						Cursor(screen,bmp->Tela);
					}
		}
        fflush(stdin);
    }
}
void Cor_Fonte(BITMAP *tela)
{
	char tecla;
	rectfill(tela,5,Resolucao_Y-20,Resolucao_X-5,Resolucao_Y-5,makecol(0,0,0));
	textout(tela,font,"Cor da Fonte: ",10,Resolucao_Y-15,makecol(r,g,b));
	textout(tela,font,"[r]ed [g]reen [b]lue [y]ellow [w]hrite", 120,Resolucao_Y-15,makecol(r,g,b));
	tecla=readkey();
	switch(tecla)
	{
		case 'r': r=255;g=0;b=0; break;
		case 'g': r=0;g=255;b=0; break;
		case 'b': r=150;g=150;b=255; break;
		case 'y': r=255;g=255;b=0; break;
		case 'w': r=255;g=255;b=255; break;
		default : r=255;g=255;b=255;
	}
}
/*=============================================================*/

/*=============================================================*\
| ROTINAS DA INTERFACE DE INICIALIZAÇÃO                         |
| Menu - Carrega imagens, desenha primitivas dinâmicas e faz a  |
| interface geral com o usuário criando as opções iniciais      |
\*=============================================================*/
void Menu()
{
	int pos=292;
	for(;;)
	{
		set_mouse_sprite(NULL);
		draw_sprite(screen, menu, 0, 0);
		rect(screen,348,pos,445,pos+26,makecol(255,255,0));
		show_mouse(screen);
		readkey();
		if(key[KEY_UP]) pos -= 29;
		if(key[KEY_DOWN]) pos += 29;
		if(pos < 292) pos = 408;
		if(pos > 408) pos = 292;
		if((key[KEY_ENTER])&&(pos == 292))
		{
			Cor_Fonte(screen);
		    clear_bitmap(screen);
		    Captura_Tecla();
        }
		if((key[KEY_ENTER])&&(pos == 321))
		{
			Imprime_Txt("help.edt");
        }
		if((key[KEY_ENTER])&&(pos == 350))
  		{
    		Nome_Arq(screen,nome);
    		Carregar(&Matriz,nome);
     	}
        if((key[KEY_ENTER])&&(pos == 379))
        {
        	Nome_Arq(screen,nome);
        	Gravar(&Matriz,nome);
     	}
		if((key[KEY_ENTER])&&(pos == 408))
  			Fechar();
	}
}
/*=============================================================*/

/*=============================================================*\
| main - Apenas chama as rotinas/funções cruciais do editor,    |
| sendo as restantes chamadas pelo menu                         |
\*=============================================================*/
int main()
{
	char tecla;
	Inicializa_Allegro();
	Inicializa_Matriz();
	Menu();
	return 0;
}
END_OF_MAIN();
/*=============================================================*/
