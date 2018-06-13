
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define katastasi0 0
#define katastasi1 1
#define katastasi2 2
#define katastasi3 3
#define katastasi4 4
#define katastasi5 5
#define katastasi6 6
#define katastasi7 7
#define katastasi8 8

#define pros8esitk 9
#define afairesitk 10
#define pollaplasiasmostk 11
#define metavlititk 12
#define sta8eratk 13
#define diairesitk 14
#define  isotitatk 15
#define ana8esitk 16
#define megalutero_i_isotk 17
#define megaluterotk 18
#define mikrotero_i_isotk 19
#define diaforotk 20
#define mikroterotk 21

#define erotimatikotk 22
#define kommatk 23
#define ano_kato_teleiatk 24
#define aristerokagkelotk 25
#define de3ikagkelotk 26
#define aristeriparen8esitk 27	
#define de3iaparen8esitk 28
#define aristeriagkilitk 29
#define de3iaagkilitk 30

#define syn 0
#define plin 1
#define epi 2
#define gramma 3 
#define ari8moi 4
#define dia 5
#define ison 6
#define megalutero 7
#define mikrotero 8
#define erotimatiko 9
#define komma 10
#define ano_kato_teleia 11
#define aristerokagkelo 12
#define de3ikagkelo 13
#define aristeriparen8esi 14
#define de3iaparen8esi 15
#define aristeriagkili 16
#define de3iaagkili 17


#define programtk 31
#define consttk 32
#define functiontk 33
#define iftk 34
#define switchtk 35
#define andtk 36
#define calltk 37
#define inputtk 38
#define vartk 39
#define proceduretk 40
#define elsetk 41
#define casetk 42
#define ortk 43
#define returntk 44
#define printtk 45
#define dotk 46
#define nottk 47
#define intk 48
#define whiletk 49
#define inouttk 50

#define metavliti 51
#define sta8era 52
#define synartisi 53
#define prosorinimetavliti 54
#define parametros 55


char factor_place[31],term_place[31],expression_place[31];

typedef struct quad {

	int number;
	char op[31];
	char x[31];
	char y[31];
	char z[31];
	struct quad *next;
} QUAD;

typedef struct ptr2quad{
	QUAD *q;
	struct ptr2quad *next;
} PTR2QUAD;

typedef struct recordargument{
	int parMode,type;
	struct recordargument *next;
} RECORDARGUMENT;






typedef struct recordentity{
	char name[31];
	int type;
	int typosmetavlitis;
	int offset;
	
	int typossynartisis;
	int startQuad;
	int framelength;
	RECORDARGUMENT *list;

	char value[31];

	int parMode;
	
	struct recordentity *next;
} RECORDENTITY;

typedef struct recordscope{

	RECORDENTITY *list;
	int nestingLevel;
	struct recordscope *next;
} RECORDSCOPE;

QUAD *first=NULL,*last=NULL;
RECORDSCOPE *scope_first=NULL,*scope_last=NULL;

FILE *infile,*outfile;
int grammi;
int temporary=1;
int offset=12;
int isglobal,islocal;
int level_metavlitis;
PTR2QUAD *term_true,*term_false;
PTR2QUAD *factor_true=NULL,*factor_false=NULL;
PTR2QUAD *condition_true,*condition_false;


RECORDENTITY *myfind_ptr(char name[31]){
	RECORDENTITY *newent;
	RECORDSCOPE *newscope;
	int myoffset;
	int flag=0,level_metavlitis;
	
	isglobal=1;
	islocal=0;
	newscope=scope_first;
	level_metavlitis=0;
	
	while(newscope!=NULL){
		newent=newscope->list;

		while(newent!=NULL){
			if(strcmp(newent->name,name)==0){
				flag=1;
				myoffset=newent->offset;
				return(newent);
			}
			newent=newent->next;
		}
		isglobal=0;
		level_metavlitis++;
		if(newscope->next==NULL)
			islocal=1;
		newscope=newscope->next;

	}

	return(NULL);
	

}
void gnlvcode(){
	int i=1;
	
	RECORDSCOPE *last_scope;
	RECORDSCOPE *myoffset;
	fprintf(outfile,"movi R[255],M[4+R[0]]\n");
	while(i<last_scope->nestingLevel-level_metavlitis){
		fprintf(outfile,"movi R[255],M[4+R[0]]\n");
		i++;
	}
	fprintf(outfile,"movi R[254],%d\n",myoffset);
	fprintf(outfile,"addi R[255],R[254],R[255]\n");
}

void loadvr(char v[31],int r){

	RECORDENTITY *newent;

	newent=myfind_ptr(v);

	if(isglobal==1){
		fprintf(outfile,"movi R[%d],M[%d]\n",r,600+newent->offset);
	}
	else if((newent->type==metavliti || newent->type==prosorinimetavliti || (newent->type==parametros && newent->parMode==intk)) && islocal==1){
		fprintf(outfile,"movi R[%d],M[%d+R[0]]\n",r,newent->offset);
	}
	else if(newent->type==parametros && newent->parMode==inouttk && islocal==1){
			fprintf(outfile,"movi R[255],M[%d+R[0]]\n",newent->offset);
			fprintf(outfile,"movi R[%d],M[R[255]]\n",r);
	}
	else if((newent->type==metavliti || (newent->type==parametros && newent->parMode==intk)) && islocal!=1){
		gnlvcode();
		fprintf(outfile,"movi R[%d],M[R[255]]\n",r);
	}
	else if(newent->type==parametros && newent->parMode==inouttk && islocal!=1){
		gnlvcode();
		fprintf(outfile,"movi R[255],M[R[255]]\n");
		fprintf(outfile,"movi R[%d],M[R[255]]\n",r);
	}
	else
		fprintf(outfile,"movi R[%d],%s\n",r,v);
		
}
void storerv(int r,char v[31]){

	RECORDENTITY *newent;

	newent=myfind_ptr(v);


	if(newent==NULL) exit(0);
	if(isglobal==1){
		fprintf(outfile,"movi M[%d],R[%d]\n",600+newent->offset,r);
	}
	else if((newent->type==metavliti || newent->type==prosorinimetavliti || (newent->type==parametros && newent->parMode==intk)) && islocal==1){
		fprintf(outfile,"M[%d+R[0]],movi R[%d]\n",newent->offset,r);
	}
	else if(newent->type==parametros && newent->parMode==inouttk && islocal==1){
			fprintf(outfile,"movi M[%d+R[0]],R[255]\n",newent->offset);
			fprintf(outfile,"movi M[R[255]],R[%d]\n",r);
	}
	else if((newent->type==metavliti || (newent->type==parametros && newent->parMode==intk)) && islocal!=1){
		gnlvcode();
		fprintf(outfile,"movi M[R[255]],R[%d]\n",r);
	}
	else if(newent->type==parametros && newent->parMode==inouttk && islocal!=1){
		gnlvcode();
		fprintf(outfile,"movi M[R[255]],R[255]\n");
		fprintf(outfile,"movi M[R[255]],R[%d]\n",r);
	}
	else
		fprintf(outfile,"movi R[%d],%s\n",v,r);
		
}

void delete(){
	
	QUAD *temp;
	RECORDSCOPE *new;

	temp=first;
	while(temp!=NULL){

		if(strcmp(temp->op,"+")==0){
			fprintf(outfile,"L%d: ",temp->number);
			loadvr(temp->x,1);
			loadvr(temp->y,2);
			fprintf(outfile,"addi R[3],R[1],R[2]\n");
			storerv(3,temp->z);
		}
		else if(strcmp(temp->op,"-")==0){
			fprintf(outfile,"L%d: ",temp->number);
			loadvr(temp->x,1);
			loadvr(temp->y,2);
			fprintf(outfile,"subi R[3],R[1],R[2]\n");
			storerv(3,temp->z);
		}
		else if(strcmp(temp->op,"*")==0){
			fprintf(outfile,"L%d: ",temp->number);
			loadvr(temp->x,1);
			loadvr(temp->y,2);
			fprintf(outfile,"muli R[3],R[1],R[2]\n");
			storerv(3,temp->z);
		}
		else if(strcmp(temp->op,"/")==0){
			fprintf(outfile,"L%d: ",temp->number);
			loadvr(temp->x,1);
			loadvr(temp->y,2);
			fprintf(outfile,"divi R[3],R[1],R[2]\n");
			storerv(3,temp->z);
		}
		else if(strcmp(temp->op,"=")==0){
			fprintf(outfile,"L%d: ",temp->number);
			loadvr(temp->x,1);
			loadvr(temp->y,2);
			fprintf(outfile,"cmpi R[1],R[2]\n");
			fprintf(outfile,"je L%s\n",temp->z);
		}

		else if(strcmp(temp->op,"<>")==0){
			fprintf(outfile,"L%d: ",temp->number);
			loadvr(temp->x,1);
			loadvr(temp->y,2);
			fprintf(outfile,"cmpi R[3],R[1],R[2]\n");
			fprintf(outfile,"jne L%s\n",temp->z);
			storerv(3,temp->z);
		}
		else if(strcmp(temp->op,">")==0){
			fprintf(outfile,"L%d: ",temp->number);
			loadvr(temp->x,1);
			loadvr(temp->y,2);
			fprintf(outfile,"cmpi R[3],R[1],R[2]\n");
			fprintf(outfile,"jb L%s\n",temp->z);
			storerv(3,temp->z);
		}
		else if(strcmp(temp->op,">=")==0){
			fprintf(outfile,"L%d: ",temp->number);
			loadvr(temp->x,1);
			loadvr(temp->y,2);
			fprintf(outfile,"cmpi R[3],R[1],R[2]\n");
			fprintf(outfile,"jbe L%s\n",temp->z);
			storerv(3,temp->z);
		}
		else if(strcmp(temp->op,"<")==0){
			fprintf(outfile,"L%d: ",temp->number);
			loadvr(temp->x,1);
			loadvr(temp->y,2);
			fprintf(outfile,"cmpi R[3],R[1],R[2]\n");
			fprintf(outfile," L%s\n",temp->z);
			storerv(3,temp->z);
		}
		else if(strcmp(temp->op,"<=")==0){
			fprintf(outfile,"L%d: ",temp->number);
			loadvr(temp->x,1);
			loadvr(temp->y,2);
			fprintf(outfile,"cmpi R[3],R[1],R[2]\n");
			fprintf(outfile,"jae L%s\n",temp->z);
			storerv(3,temp->z);
		}
		else if(strcmp(temp->op,":=")==0){
			fprintf(outfile,"L%d: ",temp->number);
			loadvr(temp->x,1);
			storerv(1,temp->z);
		}
		else if(strcmp(temp->op,"out")==0){
			fprintf(outfile,"L%d: ",temp->number);
			loadvr(temp->x,1);
			fprintf(outfile,"outi R[1]\n");
		}
		else if(strcmp(temp->op,"in")==0){
			fprintf(outfile,"L%d: ",temp->number);
			fprintf(outfile,"ini R[1]\n");
			storerv(1,temp->z);
		}
		temp=temp->next;
	}
	new=scope_first;
	while(new->next!=NULL && new->next!=scope_last){
		new=new->next;
	}
	new->next=NULL;
	scope_last=new;
}

void scope(){
	
	RECORDSCOPE *new;

	
	new=(RECORDSCOPE *)malloc(sizeof(RECORDSCOPE));
	if(new==NULL){
		printf("La8os sthn desmeysh mnhmhs\n");
		exit(0);
	}

	new->next=NULL;
	new->list=NULL;

	if(scope_last==NULL)
		new->nestingLevel=0;
	else
		new->nestingLevel=scope_last->nestingLevel + 1;
	if(scope_first==NULL){
		scope_first=new;
		scope_last=new;
	}
	else{
		scope_last->next=new;
	}
	scope_last=new;	
}

void metavlitescheck(char name [31]){
	RECORDENTITY *newent;
	RECORDSCOPE *newscope;
	
	int flag;

	newent=scope_last->list;
	flag=0;
	while (flag=!0 && newent!=NULL){
		if (strcmp(newent->name,name)== 0){
		flag=1;
		}else{
			newent=newent->next;

		}
	}
	if(flag==1){
		printf("Yparhei la8os stin dilwsi metavlitwn\n");
		exit(0);
	}

}

void myprint_ph4(){
	RECORDENTITY *newent;
	RECORDSCOPE *newscope;

	newscope=scope_first;

	
	while(newscope!=NULL){
		newent=newscope->list;

		while(newent!=NULL){
			printf("%s\t",newent->name);
			newent=newent->next;
		}
		printf("\n");
		newscope=newscope->next;
	}
	
}

void myfind(char name[31]){
	RECORDENTITY *newent;
	RECORDSCOPE *newscope;
	int flag=0;

	newscope=scope_first;

	
	while(newscope!=NULL){
		newent=newscope->list;

		while(newent!=NULL){
			if(strcmp(newent->name,name)==0)
				flag=1;
			newent=newent->next;
		}
		printf("\n");
		newscope=newscope->next;
	}

	if(flag==0){
		printf("Den vrhka metavlhth\n");
		exit(0);	
	}
	

}


void entity(char name[31],int type,int typosmetavlitis,int offset,int typossynartisis,int startQuad,int framelength,RECORDARGUMENT *list,char value[31],int parMode){

	RECORDENTITY *new,*temp;
	

	metavlitescheck(name);
	new=(RECORDENTITY *)malloc(sizeof(RECORDENTITY));
	if(new==NULL){
		printf("La8os sthn desmeysh mnhmhs\n");
		exit(0);
	}


	strcpy(new->name,name);
	new->type=type;
	new->typosmetavlitis=typosmetavlitis;
	new->offset=offset;
	
	new->typossynartisis=typossynartisis;
	new->startQuad=startQuad;
	new->framelength=framelength;
	new->list=list;

	strcpy(new->value,value);

	new->parMode=parMode;
	
	if(scope_last->list==NULL){
		scope_last->list=new;
	}
	else{
		temp=scope_last->list;
		if(temp->next!=NULL){
			while(temp->next!=NULL){
					temp=temp->next;
			}
				temp->next=new;
			
		}	
		else{
			temp->next=new;
		}
	}
	
}
int nextquad(){

	if(last==NULL){
		return(1);
	}
	else{
		return(last->number+1);
	}
}

void genquad(	char op[31],char x[31],char y[31],char z[31]){
	
	QUAD *new;
	
	new=(QUAD *)malloc(sizeof(QUAD));
	if(new==NULL){
		printf("La8os sthn desmeysh mnhmhs\n");
		exit(0);
	}

	new->next=NULL;
	new->number=nextquad();
	sprintf(new->op,"%s",op);
	sprintf(new->x,"%s",x);
	sprintf(new->y,"%s",y);
	sprintf(new->z,"%s",z);	

	if(first==NULL){
		first=new;
	}
	else{
		last->next=new;
	}
	last=new;	
}

PTR2QUAD *makelist(){
	PTR2QUAD *new;

	new=(PTR2QUAD *)malloc(sizeof(PTR2QUAD));
	if(new==NULL){
		printf("La8os sthn desmeysh mnhmhs\n");
		exit(0);
	}

	new->next=NULL;
	new->q=last;
	return(new);
	
}

PTR2QUAD *merge(PTR2QUAD *list1,PTR2QUAD *list2){

	PTR2QUAD *temp;
	
	if(list1==NULL){
		return(list2);
	}
	else if (list2==NULL){
		return(list1);
	}
	else{
		temp=list1;
		while(temp->next!=NULL){
			temp=temp->next;
		}

		temp->next=list2;
	}
}

void backpatch(PTR2QUAD *list, int x){
	PTR2QUAD *temp;
	
	char z[31];
	sprintf(z,"%d",x);

	temp=list;
	while(temp!=NULL){
		sprintf(temp->q->z,"%s",z);
		temp=temp->next;
	}
	list=NULL;
}

void myprint(){
	QUAD *tmp=first;
	while(tmp!=NULL){
		printf("%d %s,%s,%s,%s\n",tmp->number,tmp->op,tmp->x,tmp->y,tmp->z);
		tmp=tmp->next;
	}
}
int lektikos(char word[31]){

	int pinakas[9][18];
	int katastasi,symvolo,i;
	char c;

	pinakas[katastasi0][syn]=pros8esitk;
	pinakas[katastasi0][plin]=afairesitk;
	pinakas[katastasi0][epi]=pollaplasiasmostk;
	pinakas[katastasi0][gramma]=katastasi1;
	pinakas[katastasi0][ari8moi]=katastasi2;
	pinakas[katastasi0][dia]=katastasi3;
	pinakas[katastasi0][ison]=katastasi6;
	pinakas[katastasi0][megalutero]=katastasi7;
	pinakas[katastasi0][mikrotero]=katastasi8;
	pinakas[katastasi0][erotimatiko]=erotimatikotk;
	pinakas[katastasi0][komma]=kommatk;
	pinakas[katastasi0][ano_kato_teleia]=ano_kato_teleiatk;
	pinakas[katastasi0][aristerokagkelo]=aristerokagkelotk;
	pinakas[katastasi0][de3ikagkelo]=de3ikagkelotk;
	pinakas[katastasi0][aristeriparen8esi]=aristeriparen8esitk;
	pinakas[katastasi0][de3iaparen8esi]=de3iaparen8esitk;
	pinakas[katastasi0][aristeriagkili]=aristeriagkilitk;
	pinakas[katastasi0][de3iaagkili]=de3iaagkilitk;


	pinakas[katastasi1][plin]=metavlititk;
	pinakas[katastasi1][gramma]=katastasi1;
	pinakas[katastasi1][ari8moi]=katastasi1;
	pinakas[katastasi1][syn]=metavlititk;
	pinakas[katastasi1][epi]=metavlititk;

	pinakas[katastasi1][dia]=metavlititk;
	pinakas[katastasi1][ison]=metavlititk;
	pinakas[katastasi1][megalutero]=metavlititk;
	pinakas[katastasi1][mikrotero]=metavlititk;
	pinakas[katastasi1][erotimatiko]=metavlititk;
	pinakas[katastasi1][komma]=metavlititk;
	pinakas[katastasi1][ano_kato_teleia]=metavlititk;
	pinakas[katastasi1][aristerokagkelo]=metavlititk;
	pinakas[katastasi1][de3ikagkelo]=metavlititk;
	pinakas[katastasi1][aristeriparen8esi]=metavlititk;

	pinakas[katastasi1][de3iaparen8esi]=metavlititk;
	pinakas[katastasi1][aristeriagkili]=metavlititk;
	pinakas[katastasi1][de3iaagkili]=metavlititk;


	pinakas[katastasi2][plin]=sta8eratk;
	pinakas[katastasi2][gramma]=sta8eratk;
	pinakas[katastasi2][ari8moi]=katastasi2;
	pinakas[katastasi2][syn]=sta8eratk;
	pinakas[katastasi2][epi]=sta8eratk;

	pinakas[katastasi2][dia]=sta8eratk;
	pinakas[katastasi2][ison]=sta8eratk;
	pinakas[katastasi2][megalutero]=sta8eratk;
	pinakas[katastasi2][mikrotero]=sta8eratk;
	pinakas[katastasi2][erotimatiko]=sta8eratk;
	pinakas[katastasi2][komma]=sta8eratk;
	pinakas[katastasi2][ano_kato_teleia]=sta8eratk;
	pinakas[katastasi2][aristerokagkelo]=sta8eratk;
	pinakas[katastasi2][de3ikagkelo]=sta8eratk;
	pinakas[katastasi2][aristeriparen8esi]=sta8eratk;

	pinakas[katastasi2][de3iaparen8esi]=sta8eratk;
	pinakas[katastasi2][aristeriagkili]=sta8eratk;
	pinakas[katastasi2][de3iaagkili]=sta8eratk;


	pinakas[katastasi3][plin]=diairesitk;
	pinakas[katastasi3][gramma]=katastasi1;
	pinakas[katastasi3][ari8moi]=katastasi1;
	pinakas[katastasi3][syn]=diairesitk;
	pinakas[katastasi3][epi]=katastasi4;

	pinakas[katastasi3][dia]=diairesitk;
	pinakas[katastasi3][ison]=diairesitk;
	pinakas[katastasi3][megalutero]=diairesitk;
	pinakas[katastasi3][mikrotero]=diairesitk;
	pinakas[katastasi3][erotimatiko]=diairesitk;
	pinakas[katastasi3][komma]=diairesitk;
	pinakas[katastasi3][ano_kato_teleia]=diairesitk;
	pinakas[katastasi3][aristerokagkelo]=diairesitk;
	pinakas[katastasi3][de3ikagkelo]=diairesitk;
	pinakas[katastasi3][aristeriparen8esi]=diairesitk;

	pinakas[katastasi3][de3iaparen8esi]=diairesitk;
	pinakas[katastasi3][aristeriagkili]=diairesitk;
	pinakas[katastasi3][de3iaagkili]=diairesitk;


    	
    	pinakas[katastasi4][plin]=katastasi4;
	pinakas[katastasi4][gramma]=katastasi4;
	pinakas[katastasi4][ari8moi]=katastasi4;
	pinakas[katastasi4][syn]=katastasi4;
	pinakas[katastasi4][epi]=katastasi5;

	pinakas[katastasi4][dia]=katastasi4;
	pinakas[katastasi4][ison]=katastasi4;
	pinakas[katastasi4][megalutero]=katastasi4;
	pinakas[katastasi4][mikrotero]=katastasi4;
	pinakas[katastasi4][erotimatiko]=katastasi4;
	pinakas[katastasi4][komma]=katastasi4;
	pinakas[katastasi4][ano_kato_teleia]=katastasi4;
	pinakas[katastasi4][aristerokagkelo]=katastasi4;
	pinakas[katastasi4][de3ikagkelo]=katastasi4;
	pinakas[katastasi4][aristeriparen8esi]=katastasi4;

	pinakas[katastasi4][de3iaparen8esi]=katastasi4;
	pinakas[katastasi4][aristeriagkili]=katastasi4;
	pinakas[katastasi4][de3iaagkili]=katastasi4;

	pinakas[katastasi5][plin]=katastasi4;
	pinakas[katastasi5][gramma]=katastasi4;
	pinakas[katastasi5][ari8moi]=katastasi4;
	pinakas[katastasi5][syn]=katastasi4;
	pinakas[katastasi5][epi]=katastasi5;

	pinakas[katastasi5][dia]=katastasi0;
	pinakas[katastasi5][ison]=katastasi4;
	pinakas[katastasi5][megalutero]=katastasi4;
	pinakas[katastasi5][mikrotero]=katastasi4;
	pinakas[katastasi5][erotimatiko]=katastasi4;
	pinakas[katastasi5][komma]=katastasi4;
	pinakas[katastasi5][ano_kato_teleia]=katastasi4;
	pinakas[katastasi5][aristerokagkelo]=katastasi4;
	pinakas[katastasi5][de3ikagkelo]=katastasi4;
	pinakas[katastasi5][aristeriparen8esi]=katastasi4;

	pinakas[katastasi5][de3iaparen8esi]=katastasi4;
	pinakas[katastasi5][aristeriagkili]=katastasi4;
	pinakas[katastasi5][de3iaagkili]=katastasi4;

	pinakas[katastasi6][plin]=ana8esitk;
	pinakas[katastasi6][gramma]=ana8esitk;
	pinakas[katastasi6][ari8moi]=ana8esitk;
	pinakas[katastasi6][syn]=ana8esitk;
	pinakas[katastasi6][epi]=ana8esitk;

	pinakas[katastasi6][dia]=ana8esitk;
	pinakas[katastasi6][ison]=isotitatk;
	pinakas[katastasi6][megalutero]=ana8esitk;
	pinakas[katastasi6][mikrotero]=ana8esitk;
	pinakas[katastasi6][erotimatiko]=ana8esitk;
	pinakas[katastasi6][komma]=ana8esitk;
	pinakas[katastasi6][ano_kato_teleia]=ana8esitk;
	pinakas[katastasi6][aristerokagkelo]=ana8esitk;
	pinakas[katastasi6][de3ikagkelo]=ana8esitk;
	pinakas[katastasi6][aristeriparen8esi]=ana8esitk;

	pinakas[katastasi6][de3iaparen8esi]=ana8esitk;
	pinakas[katastasi6][aristeriagkili]=ana8esitk;
	pinakas[katastasi6][de3iaagkili]=ana8esitk;

    
	pinakas[katastasi7][plin]=megaluterotk;
	pinakas[katastasi7][gramma]=megaluterotk;
	pinakas[katastasi7][ari8moi]=megaluterotk;
	pinakas[katastasi7][syn]=megaluterotk;
	pinakas[katastasi7][epi]=megaluterotk;

	pinakas[katastasi7][dia]=megaluterotk;
	pinakas[katastasi7][ison]=megalutero_i_isotk;
	pinakas[katastasi7][megalutero]=megaluterotk;
	pinakas[katastasi7][mikrotero]=megaluterotk;
	pinakas[katastasi7][erotimatiko]=megaluterotk;
	pinakas[katastasi7][komma]=megaluterotk;
	pinakas[katastasi7][ano_kato_teleia]=megaluterotk;
	pinakas[katastasi7][aristerokagkelo]=megaluterotk;
	pinakas[katastasi7][de3ikagkelo]=megaluterotk;
	pinakas[katastasi7][aristeriparen8esi]=megaluterotk;

	pinakas[katastasi7][de3iaparen8esi]=megaluterotk;
	pinakas[katastasi7][aristeriagkili]=megaluterotk;
	pinakas[katastasi7][de3iaagkili]=megaluterotk;

	pinakas[katastasi8][plin]=mikroterotk;
	pinakas[katastasi8][gramma]=mikroterotk;
	pinakas[katastasi8][ari8moi]=mikroterotk;
	pinakas[katastasi8][syn]=mikroterotk;
	pinakas[katastasi8][epi]=mikroterotk;

	pinakas[katastasi8][dia]=mikroterotk;
	pinakas[katastasi8][ison]=mikrotero_i_isotk;
	pinakas[katastasi8][mikrotero]=mikroterotk;
	pinakas[katastasi8][megalutero]=diaforotk;
	pinakas[katastasi8][erotimatiko]=mikroterotk;
	pinakas[katastasi8][komma]=mikroterotk;
	pinakas[katastasi8][ano_kato_teleia]=mikroterotk;
	pinakas[katastasi8][aristerokagkelo]=mikroterotk;
	pinakas[katastasi8][de3ikagkelo]=mikroterotk;
	pinakas[katastasi8][aristeriparen8esi]=mikroterotk;

	pinakas[katastasi8][de3iaparen8esi]=mikroterotk;
	pinakas[katastasi8][aristeriagkili]=mikroterotk;
	pinakas[katastasi8][de3iaagkili]=mikroterotk;
        	
	

	i=0;
	katastasi=katastasi0;
	while(1){

		c=getc(infile);

		if(c==EOF)
			return(-1);

		if(c==' '){
		 if(katastasi==katastasi0) 
			continue;
		 else 
		   break;
		}
		if(c=='\t'){
		 if(katastasi==katastasi0) 
			continue;
		 else 
		   break;
		}
		if(c=='\n'){
			grammi++;
			if(katastasi==katastasi0){
			  i=0;
			  continue;
			}
			else {
			  break;
			}
		}
		
		
		if(i<=30 && katastasi!=katastasi3 && katastasi!=katastasi4 && katastasi!=katastasi5){
			word[i]=c;
			i++;
		}
		if(c=='+')
			symvolo=syn;
		else if(c=='-')
			symvolo=plin;
		else if(c=='*')
			symvolo=epi;
		else if(c=='/')
			symvolo=dia;
		else if(c=='>')
			symvolo=megalutero;
		else if(c=='<')
			symvolo=mikrotero;	
		else if(c=='=')
			symvolo=ison;
		else if((c>='a' && c<='z') || (c>='A' && c<='Z'))
			symvolo=gramma;
		else if(c>='0' && c<='9')
			symvolo=ari8moi;
		else if (c == ';')
			symvolo=erotimatiko;
		else if (c==',')
			symvolo=komma;
		else if(c==':')
			symvolo=ano_kato_teleia;
		else if(c=='{')
			symvolo=aristerokagkelo;
		else if (c=='}')
			symvolo=de3ikagkelo;
		else if (c=='(')
			symvolo=aristeriparen8esi;
		else if (c==')')
			symvolo=de3iaparen8esi;
		else if (c=='[')
			symvolo=aristeriagkili;
		else if (c==']')
			symvolo=de3iaagkili;
		else{
			printf("Grammi %d: Vrika mi apodekto xaraktira <%c>\n",grammi,c);
			exit(0);
		}


		katastasi=pinakas[katastasi][symvolo];

		if(katastasi>=pros8esitk){
			break;
		}
	}
	word[i]='\0';

	if(katastasi==katastasi1 &&  (c==' ' || c=='\n' || c=='\t')){
	  katastasi=metavlititk;
	}
	if(katastasi==katastasi2 &&  (c==' ' || c=='\n' || c=='\t')){
	  katastasi=sta8eratk;
	}
	
	if(katastasi==katastasi7 &&  (c==' ' || c=='\n' || c=='\t')){
	  katastasi=megaluterotk;
	}
	if(katastasi==katastasi8 &&  (c==' ' || c=='\n' || c=='\t')){
	  katastasi=mikroterotk;
	}
	if(katastasi==katastasi6 &&  (c==' ' || c=='\n' || c=='\t')){
	  katastasi=ana8esitk;
	}
	if(katastasi==katastasi3 &&  (c==' ' || c=='\n' || c=='\t')){
	  katastasi=diairesitk;
	}
	
	if(katastasi==sta8eratk){
		if(i>30){
			printf("Grammi %d: Diavasa ari8mo ektos oriwn \n",grammi);
			exit(0);
		}
	}
	if(katastasi==megaluterotk){

		if(c=='\n'){
			grammi--;
		}
		else if(c!=' ' && c!='\t'){
			word[i-1]='\0';
		}
		ungetc(c,infile);
	}
	if(katastasi==mikroterotk){
		if(c=='\n'){
			grammi--;
		}
		word[i-1]='\0';
		ungetc(c,infile);
	}

	if(katastasi==metavlititk){
		if(c=='\n'){
			grammi--;
		}
		else if(c!=' ' && c!='\t'){
			word[i-1]='\0';
		}
		ungetc(c,infile);
	}
	if(katastasi==sta8eratk){
		if(c=='\n'){
			grammi--;
		}
		word[i-1]='\0';
		ungetc(c,infile);
	}
	if(katastasi==ana8esitk){
		if(c=='\n'){
			grammi--;
		}
		word[i-1]='\0';
		ungetc(c,infile);
	}
	if(katastasi==diairesitk){
		if(c=='\n'){
			grammi--;
		}
		ungetc(c,infile);
		word[i-1]='\0';
	}
	

	if(katastasi==metavlititk){
		if(strcmp(word,"program")==0){
			return(programtk);
		}
		if(strcmp(word,"const")==0){
			return(consttk);
		}
		if(strcmp(word,"function")==0){
			return(functiontk);
		}
		if(strcmp(word,"if")==0){
			return(iftk);
		}
		if(strcmp(word,"switch")==0){
			return(switchtk);
		}
		if(strcmp(word,"and")==0){
			return(andtk);
		}
		if(strcmp(word,"call")==0){
			return(calltk);
		}
		if(strcmp(word,"input")==0){printf("eeee\n");
			return(inputtk);
		}
		if(strcmp(word,"var")==0){
			return(vartk);
		}
		if(strcmp(word,"procedure")==0){
			return(proceduretk);
		}
		if(strcmp(word,"else")==0){
			return(elsetk);
		}
		if(strcmp(word,"case")==0){
			return(casetk);
		}
		if(strcmp(word,"or")==0){
			return(ortk);
		}
		if(strcmp(word,"return")==0){
			return(returntk);
		}
		if(strcmp(word,"print")==0){
			return(printtk);
		}
		if(strcmp(word,"do")==0){
			return(dotk);
		}
		if(strcmp(word,"not")==0){
			return(nottk);
		}
		if(strcmp(word,"in")==0){
			return(intk);
		}
		if(strcmp(word,"while")==0){
			return(whiletk);
		}
		if(strcmp(word,"inout")==0){
			return(inouttk);
		}
	}
	return(katastasi);
}
int formalparitem(int x ){
	char word[31];
	if(x==intk){
			x=lektikos(word);
			if(x==metavlititk)
			entity(word,parametros,-1,12,-1,-1,-1,NULL,"",intk);
			x=lektikos(word);
			}
	else if(x==inouttk){
			x=lektikos(word);
			if(x==metavlititk)
			entity(word,parametros,-1,12,-1,-1,-1,NULL,"",inouttk);
			x=lektikos(word);
			
			}
	return(x);
}
int formalparlist(int x){
	char word[31];
if (x==intk || x==inouttk){
		x=formalparitem(x);
		}

					while(x==kommatk){
							x=lektikos(word);
							
								x=formalparitem(x);
								}
					
		
return(x);
}

int sequence(int x){
 char word[31];



x=lektikos(word);
                           
           if(x==aristerokagkelotk){
                     x=lektikos(word);
             
                           x=statement(x,word);
                         
                                             while(x==erotimatikotk){
				x=lektikos(word);
                                                      x=statement(x);
                                                      }
                                                 
                                                      
									                   if(x!=de3ikagkelotk){
                                                                      printf("Grammi %d: Perimena de3i kagkelo\n",grammi);
                                                       exit(0);
                                                       }
                                                
}

else{
     x=statement(x,word);

    
     if(x!=erotimatikotk){
		 printf("Grammi %d: Perimena ;\n",grammi);
         exit(0);
          
	}
	x=lektikos(word);
	}

return(x);
}



int elsepart(int x){
 char word[31];
if(x==elsetk){
  
		x=sequence(x);

   }
 

	return(x);
}

int formalpars(int x){
	char word[31];
	if (x==aristeriparen8esitk){
			x=lektikos(word);

			if(x==intk || x==inouttk ){ 
			x=formalparlist(x); printf("%d\n",x);
		}
			if(x!=de3iaparen8esitk){
					printf("Grammi %d: Perimena de3ia paren8esi\n",grammi);
					exit(0);
						}
				}
	
			else{
				printf("Grammi %d: Perimena aristeri paren8esi\n",grammi);
				exit(0);
		}
		
			
return(x);
}

int return_stat(int x){
	char word[31];
if(x==returntk)
  x=lektikos(word);
   if(x==aristeriparen8esitk){
    x=lektikos(word);
			x=expression(x);
			if(x!=de3iaparen8esitk){
				printf("Grammi %d: Perimena de3ia paren8esi\n",grammi);
				exit(0);
				}
				genquad("retv",factor_place,"_","_");
            }
x=lektikos(word);

return(x);
}
int call_stat(int x){
	char word[31],onoma[31];
if(x==calltk)
  x=lektikos(word);
   if(x==metavlititk){
		 myfind(word);
	strcpy(onoma,word);
     x=lektikos(word);

       x=actualpars(x);
	genquad("call",onoma,"_","_");
       }
  
return(x);
}
int print_stat(int x){
	char word[31];
if(x==printtk)
  x=lektikos(word);
   if(x==aristeriparen8esitk){
    x=lektikos(word);
			x=expression(x,word);
			genquad("out",expression_place,"_","_");
			if(x!=de3iaparen8esitk){
				printf("Grammi %d: Perimena de3ia paren8esi\n",grammi);
				exit(0);
				}
            }
x=lektikos(word);
return(x);
}


int input_stat(int x){
	char word[31];
if(x==inputtk){
  x=lektikos(word);
  if(x==aristeriparen8esitk){
   x=lektikos(word);
   if(x==metavlititk){
		 myfind(word);
	x=lektikos(word);

		genquad("inp",factor_place,"_","_");
		if(x!=de3iaparen8esitk){
				printf("Grammi %d: Perimena de3ia paren8esi\n",grammi);
				exit(0);
                               }
            }else{
				printf("Grammi %d: Perimena metavliti\n",grammi);
				exit(0);
                               }
		}else{
				printf("Grammi %d: Perimena aristeri paren8esi\n",grammi);
				exit(0);
                               }
	}
x=lektikos(word);
return(x);
}

int switch_stat(int x){
	char word[31];
	int y;
	PTR2QUAD *jlist;
if(x==switchtk){
	x=lektikos(word);
	if(x==aristeriparen8esitk){
		x=lektikos(word);
			x=expression(x);
			x=condition(x,word);


			y=nextquad();
			backpatch(condition_true,y);
			x=sequence(x);


			genquad("jump","_","_","_");
			jlist=makelist();

			y=nextquad();
			backpatch(condition_false,y);

			y=nextquad();
			backpatch(jlist,y);




			if(x!=de3iaparen8esitk){
				printf("Grammi %d: Perimena de3ia paren8esi\n",grammi);
				exit(0);
				}
			x=lektikos(word);
         
            if(x==aristerokagkelotk){
              x=lektikos(word);
                    while(x==casetk){
				x=lektikos(word);
				x=expression(x);
				
				if(x==ano_kato_teleiatk){
				   x=sequence(x); 
						}
 else {
					printf("Grammi %d: Perimena ano kato teleia \n",grammi);
					exit(0);
				}
}
						if(x==de3ikagkelotk){
						x=lektikos(word);
						}
						 else {
							printf("Grammi %d: Perimena de3i kagkelo paren8esi\n",grammi);
							exit(0);
						}
			}
   		else {
					printf("Grammi %d: Perimena aristero kagkelo\n",grammi);
					exit(0);
				}
	}

   		else {
					printf("Grammi %d: Perimena aristerh parenthesi\n",grammi);
					exit(0);
				}
}
return(x);
}
int do_while_stat(int x){
int y;
	char word[31];
if(x==dotk){
	y=nextquad();
                    x=sequence(x);
	if(x==whiletk){
	x=lektikos(word);
	if(x==aristeriparen8esitk){
		x=lektikos(word);
			x=condition(x,word);
			if(x!=de3iaparen8esitk){
				printf("Grammi %d: Perimena de3ia paren8esi\n",grammi);
				exit(0);
				}
				backpatch(condition_true,y);
				y=nextquad();
				backpatch(condition_false,y);
		x=lektikos(word);
            }
	}
}
return(x);
}


int while_stat(int x){
	int y;
	char word[31];
	int Bquad;
if(x==whiletk){
	x=lektikos(word);
	if(x==aristeriparen8esitk){
		x=lektikos(word);
			y=nextquad();
			x=condition(x,word);

			Bquad=nextquad();
			backpatch(condition_true,Bquad);

			if(x!=de3iaparen8esitk){
				printf("Grammi %d: Perimena de3ia paren8esi\n",grammi);
				exit(0);
				}
			x=sequence(x);
			}else{
			printf("Grammi %d: Perimena aristeri paren8esi\n",grammi);
			exit(0);
				}
				
				sprintf(word,"%d",y);
				genquad("jump","_","_",word);
				backpatch(condition_false,nextquad());
}
	
return(x);
}

int if_stat( int x){
 char word[31];
	int y;
	PTR2QUAD *jlist;
if(x==iftk)
	x=lektikos(word);
	if(x==aristeriparen8esitk){
		x=lektikos(word);
			x=condition(x,word);
			if(x!=de3iaparen8esitk){
				printf("Grammi %d: Perimena de3ia paren8esi %d\n",grammi,x);
				exit(0);
				}
 printf("if condition true %d\n",condition_true->q->number); printf("condition false %d\n",condition_false->q->number); 
			y=nextquad();
			backpatch(condition_true,y);
			x=sequence(x);


			genquad("jump","_","_","_");
			jlist=makelist();

			y=nextquad();
			backpatch(condition_false,y);

			x=elsepart(x);

			y=nextquad();
			backpatch(jlist,y);
			
				}
		else{
			printf("Grammi %d: Perimena aristeri paren8esi\n",grammi);
			exit(0);
			}
			
			
	
	return(x);
	
}

int factor(int x,char w[31]){
	char word[31];
    if(x==sta8eratk){
	strcpy(factor_place,w);
    	x=lektikos(word);
		}
   else if(x==aristeriparen8esitk){
    x=lektikos(word);
    x=expression(x,word);
    x=lektikos(word);
     if(x!=de3iaparen8esitk){
     printf("Grammi %d: Perimena de3ia paren8esi\n",grammi);
     exit(0);
     } 
		x=lektikos(word);
		}                                                    

    
     else if(x==metavlititk){
		 myfind(w);
strcpy(factor_place,w);
     x=lektikos(w);
	
     x=idtail(x);

     }
     return(x);

}
    




int term(int x,char w[31]){
	int symvolo;
	char word[31];
	char factor1[31],temp[31];

            x=factor(x,w);
	strcpy(factor1,factor_place);
			         while(x==pollaplasiasmostk || x==diairesitk){
				  	symvolo=x;
					  x=mul_oper(x,word);
	x=factor(x,word);
	sprintf(temp,"T_%d",temporary);
	temporary++;
		entity(temp,prosorinimetavliti,-1,offset,-1,-1,-1,NULL,"",-1);
	if(symvolo==pollaplasiasmostk)
		genquad("*",factor1,factor_place,temp);
	else
		genquad("/",factor1,factor_place,temp);
	strcpy(factor1,temp);
	}              

	strcpy(term_place,factor1);
	return(x);
}





int boolfactor(int x, char word[31]){

	char expression1[31];
	char relop[31];
	
    if(x==nottk){
   		x=lektikos(word);
    	if(x==aristeriagkilitk){
				x=lektikos(word);
        x=condition(x);
       	x=lektikos(word);
   	           if(x!=de3iaagkilitk){
				printf("Grammi %d: Perimena de3ia agkili\n",grammi);
				exit(0);
				}
				}
					else{
				printf("Grammi %d: Perimena aristeri agkili\n",grammi);
				exit(0);
		
            }
       }
	else if(x==aristeriagkilitk){   
                    x=condition();
                    x=lektikos(word);
   	           if(x!=de3iaagkilitk){
				printf("Grammi %d: Perimena de3ia agkili\n",grammi);
				exit(0);
				}
			}
		
	else{

                x=expression(x,word);
				strcpy(expression1,expression_place);
				strcpy(relop,word);
                  x=relational_oper(x,relop);
                  x=lektikos(word);
                  x=expression(x,word);
  			
			genquad(relop,expression1,expression_place,"_");
			
			factor_true=makelist();
			genquad("jump","_","_","_");
			
			factor_false=makelist();  	
    return(x);
	}
	
	printf("goiat\n");
	if(factor_true==NULL) printf("ok\n");
    return(x);
	                                                         
}



int boolterm(int x, char word[31]){

	char z[31];
	
	PTR2QUAD *false1,*true1,*false2,*true2;
        x=boolfactor(x,word);

		true1=factor_true;
		false1=factor_false;
	        	while(x==andtk){
				x=nextquad();
				
				backpatch(true1,x);
	        	x=lektikos(word);
	        	
				x=boolfactor(x,word);
				true2=factor_true;
				false2=factor_false;
				false1=merge(false1,false2);
				true1=true2;
 				}
		term_true=true1;
		term_false=false1;
	
	return(x);
}




int condition(int x,char word[31]){
	
	PTR2QUAD *false1,*true1,*false2,*true2;
        x=boolterm(x,word);
		true1=term_true;
		false1=term_false;


	        	 while(x==ortk){

				x=nextquad();
				
				backpatch(false1,x);
	x=lektikos(word);
  x=boolterm(x,word);

		true2=term_true;
		false2=term_false;

				true1=merge(true1,true2);
				false1=false2;
              }
        condition_true=true1;
		condition_false=false1;      
	        	return(x);
}	

int actualparitem(int x){
	char word[31];
    if(x==intk){
		x=lektikos(word);
        x=expression(x,word);
	genquad("par",expression_place,"CV","_");printf("item %d\n",x);
                  
		}else if(x==inouttk){
		x=lektikos(word);
		if (x==metavlititk){
		 myfind(word);
		genquad("par",word,"REF","_");
		x=lektikos(word);
	}
		}
return(x);
}

int actualparlist(int x){
	char word[31];
if (x==intk || x==inouttk){
		x=actualparitem(x);
		}
			
			
					
						while(x==kommatk){
							x=lektikos(word);
							if(x==intk || x==inouttk){
								x=actualparitem(x);
								}
								
									
								}
							

}

int actualpars(int x){
	char word[31];
    if(x==aristeriparen8esitk){
		x=lektikos(word);
		if(x==intk || x==inouttk){
			x=actualparlist(x);
			}
			printf("%d\n",x);
			if(x!=de3iaparen8esitk){
				printf("Grammi %d: Perimena de3ia paren8esi\n",grammi);
				exit(0);
				}
		x=lektikos(word);
		}
					else{
				printf("Grammi %d: Perimena aristeri paren8esi\n",grammi);
				exit(0);}
		
    return(x);

}

int idtail(int x){
	char word[31];
	if(x==aristeriparen8esitk){
                 x=actualpars(x);
                 x=lektikos(word);
                                     }
    return(x);
} 

int relational_oper(int x,char word[31]){
    if(x==isotitatk || x==megalutero_i_isotk || x==mikrotero_i_isotk ||x==diaforotk || x==megaluterotk || x==mikroterotk){

  return(x); 
}else{
	 
    printf("Grammi %d: perimena kapoio apo ta symvola ==|>=|<=|<>|>|< \n",grammi);
exit(0);

}    

}
int mul_oper(int x,char word[31]){
if(x==pollaplasiasmostk || x==diairesitk){
	x=lektikos(word);
	return(x);
}else{
	printf("Grammi %d: Perimena pros8esi i aferesi\n",grammi);
	exit(0);
}
	

}


int add_oper(int x,char word[31]){
    
	
if(x==pros8esitk || x==afairesitk){
	x=lektikos(word);
	return(x);
}else{
	printf("Grammi %d: Perimena pros8esi i aferesi\n",grammi);
	exit(0);
}


}




int optional_sign(int x){
	char word[31];
                    x=add_oper(x,word);
                    x=lektikos(word);
                                 
                           
	
             return(x);
}


int expression(int x,char w[31]){
	int symvolo;
	char word[31];
	char term1[31],temp[31],factor1[31];
if(x==pros8esitk || x==afairesitk){
  x=optional_sign(x);
   x=lektikos(word);
}

	x=term(x,w);
strcpy(term1,term_place);
			while(x==pros8esitk || x==afairesitk){
					symvolo=x;
					x=add_oper(x,word);
				  x=term(x,word);
		sprintf(temp,"T_%d",temporary);
		temporary++;
		entity(temp,prosorinimetavliti,-1,offset,-1,-1,-1,NULL,"",-1);
		if(symvolo==pros8esitk)
			    genquad("+",term1,term_place,temp);
		else
		    	genquad("-",term1,term_place,temp);
		      strcpy(term1,temp);
							}
	strcpy(expression_place,term1);

return(x);

}

			
int assignment_stat(int x,char metavliti1[31]){
	char word[31];
	char temp[31];


if(x==metavlititk){
strcpy(temp,metavliti1);
	x=lektikos(word);
		if(x==ana8esitk){
			x=lektikos(word);
			x=expression(x,word);
			genquad(":=",expression_place,"_",temp);
			}else{
			printf("Grammi %d %d: Perimena ison '=' \n",grammi,x);
			exit(0);
			}
	}

return(x);
}
int statement(int x,char w[31]){
	char word[31];
if (x==metavlititk)
	x=assignment_stat(x,w);
if (x==iftk)
	x=if_stat(x);
if(x==whiletk)
	x=while_stat(x);
if(x==dotk)
	x=do_while_stat(x);
if(x==switchtk)
	x=switch_stat(x);
if(x==inputtk)
	x=input_stat(x);
if(x==printtk)
	x=print_stat(x);
if(x==calltk)
	x=call_stat(x);
if(x==returntk)
	x=return_stat(x);

	return(x);
}

int subprograms(int x){
	char word[31];


	if(x==proceduretk){
		x=lektikos(word);
			if(x==metavlititk){
				entity(word,synartisi,-1,-1,proceduretk,0,0,NULL,"",-1);
				scope();
				x=lektikos(word);
					if(x==aristeriparen8esitk){
						x=formalpars(x);
						x=lektikos(word);
							if(x==aristerokagkelotk){

								x=lektikos(word);
								x=body(x);
								if (x!=de3ikagkelotk){
									printf("Grammi %d: Perimena de3i kagkelo 3\n",grammi);
									exit(0);
								}
							}
							else{
									printf("Grammi %d: Perimena aristero kagkeloooooo %d\n",grammi,x);
									exit(0);
							}
						}
						else{
									printf("Grammi %d: Perimena aristeri parenthesi\n",grammi);
									exit(0);
							

						}}
							else{
									printf("Grammi %d: Perimena metavliti\n",grammi);
									exit(0);
							}
						}
							if(x==functiontk){
		x=lektikos(word);
			if(x==metavlititk){

				entity(word,synartisi,-1,-1,functiontk,0,0,NULL,"",-1);
				scope();

				x=lektikos(word);
					if(x==aristeriparen8esitk){
						x=formalpars(x);
						x=lektikos(word);
							if(x==aristerokagkelotk){

								x=lektikos(word);
								x=body(x);
								if (x!=de3ikagkelotk){
									printf("Grammi %d: Perimena de3i kagkelo 3\n",grammi);
									exit(0);
								}
							}
							else{
									printf("Grammi %d: Perimena aristero kagkeloooooo %d\n",grammi,x);
									exit(0);
							}
						}
						else{
									printf("Grammi %d: Perimena aristeri parenthesi\n",grammi);
									exit(0);
							

						}}
							else{
									printf("Grammi %d: Perimena metavliti\n",grammi);
									exit(0);
							}
						}
		myprint_ph4();	
		delete();				
	return(x);
}
int varlist(int x){
	char word[31];
	x=lektikos(word);
if (x==metavlititk){

		entity(word,metavliti,-1,offset,-1,-1,-1,NULL,"",-1);
		offset+=4;

			x=lektikos(word);
						while(x==kommatk){
							x=lektikos(word);
							if(x==metavlititk){

								entity(word,metavliti,-1,offset,-1,-1,-1,NULL,"",-1);
								x=lektikos(word);
									
								}
							else{
								printf("Grammi %d: Perimena metavliti \n",grammi);
								exit(0);
							}

							}
			}
			
return(x);
}
int declarations(int x){
	char word[31];
	if (x==vartk){
		x=varlist(x);
	}
	if(x!=erotimatikotk){
		printf("Grammi %d: Perimena perimena erotimatiko ';' \n",grammi);
		exit(0);
	}
	return(x);
}

int body(int x,char w[31]){
	char word[31];
	while(x==vartk){
		x=declarations(x);
		x=lektikos(word);
	}
	while(x==proceduretk || x==functiontk){
		x=subprograms(x);
		x=lektikos(word);
	}
  x=statement(x,w);
	while(x==erotimatikotk){
		x=lektikos(word);
		x=statement(x,word);
	}
	return(x);

}

int consts(int x,char w[31]){
	char word[31];
	char name[31],value[31];
	x=lektikos(word);

	strcpy(name,word);
	if(x==metavlititk){
		x=lektikos(word);
	
		if(x==ana8esitk){
			x=lektikos(word);
			strcpy(value,word);
			if(x==sta8eratk){
				entity(name,sta8era,-1,offset,-1,-1,-1,NULL,value,-1);
				offset+=4;
				x=lektikos(word);
				if(x==erotimatikotk){
					x=lektikos(word);
					strcpy(w,word);
				}
				else{
					printf("Grammi %d: Perimena erotimatiko\n",grammi);	
					exit(0);
				}
			}
			else{
				printf("Grammi %d: Perimena ari8mo\n",grammi);	
				exit(0);
			}
	}
	else{
		printf("Grammi %d: Perimena anathesi\n",grammi);	
		exit(0);
	}
	}
	else{
					printf("Grammi %d: Perimena metavliti\n",grammi);	
					exit(0);
				}
	
		
		return(x);
		

	
}

void syntaktikos(){
	int x;
	char word[31];
	x=lektikos(word);

	if(x==programtk){
		scope();
		x=lektikos(word);
		if(x==metavlititk){
			x=lektikos(word);
			if(x==aristerokagkelotk){
				x=lektikos(word);
				while(x==consttk){
					x=consts(x,word);
				}	
				x=body(x,word);
				if(x==de3ikagkelotk){
					myprint_ph4();
					printf("Grammi %d: Swsto programma se Cimple\n",grammi);
				}
				else{
					printf("Grammi %d: Perimena de3i kagkelo 2\n",grammi);
				}
			}
			else{
				printf("Grammi %d: Perimena aristero kagkelo\n",grammi);
			}

		}
		else{
			printf("Grammi %d: Perimena onoma programmatos\n",grammi);
		}
	}
	else{
		printf("Grammi %d:Perimena thn lexi program \n",grammi);
		exit(0);
	}

	myprint();
		delete();				
}

main(int argc,char *argv[]){

	int x;
	char word[31];
	grammi=1;

	if(argc!=3){
		printf("La8os ari8mos orismatwn\n");
		exit(0);
	}

	if((infile=fopen(argv[1],"r"))==NULL){
		printf("La8os sto anoigma tou arxeiou.\n");
		exit(0);
	}

	if((outfile=fopen(argv[2],"w"))==NULL){
		printf("La8os sto anoigma tou arxeiou.\n");
		exit(0);
	}
	scope_last=NULL;
	/*while(1){
		x=lektikos(word);
		if(x!=-1)
			printf("%s--->%d\n",word,x);
		else{
			printf("EOF--->%d\n",x);
			break;
		}
	}*/
	syntaktikos();
}
