#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "HBASE.h"
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
extern int yylex();
extern char *yytext;
boolean follow_token=false;
typetoken token;

typetoken _lire_token(){
  if(follow_token)
   {follow_token=false;
    return token;
   }
  else return (typetoken)yylex();
}

typedef struct char_list1{
	char n1[50];
	char n2[50];
	struct char_list1 *next;
}char_list1;

//Notre base de données
DataBase *DB;

void _e()
{     char* chemin;
      DataBase* tab = (DataBase*)malloc(sizeof(DataBase));
      Table* fam = (Table*)malloc(sizeof(Table));
      Family* col = (Family*)malloc(sizeof(Family));
      Column* row = (Column*)malloc(sizeof(Column));
	

      FILE *LIST_TABLE, *LIST_FAMILY, *LIST_COLUMN, *LIST_ROW;
	  /*LIST_TABLE=NULL;
	  LIST_FAMILY=NULL;
	  LIST_COLUMN=NULL;
	  LIST_ROW=NULL;*/
	  chemin=(char*)malloc(100*sizeof(char));
	  strcpy(chemin,"BASE");
	  mkdir(chemin,0777);
	  tab=DB;
	
		strcat(chemin,"/list_table.txt");
		LIST_TABLE=fopen(chemin,"a+");
		
      
    while(tab!=NULL)
	{
	if(strcmp(tab->TName,"")!=0){
	  fprintf(LIST_TABLE,"{[%s],[%d]}.",tab->TName,tab->Etat);}
	  tab=tab->next;
	};
	fclose(LIST_TABLE);
    tab = (DataBase*)malloc(sizeof(DataBase));
    tab =DB;
    fam=(Table*)malloc(sizeof(Table));
    while(tab!=NULL )
    {
if(strcmp(tab->TName,"")!=0){
		strcpy(chemin,"BASE/");
		strcat(chemin,tab->TName);
		mkdir(chemin,0777);
		
		fam=tab->T;
		strcat(chemin,"/list_families.txt");
		    LIST_FAMILY=fopen(chemin,"a+");
		    
		while(fam!=NULL)
		{
		  fprintf(LIST_FAMILY,"{[%s],[%d]}.",fam->FName,fam->NVersions);
		  fam=fam->next; 
		};
		fclose(LIST_FAMILY);
		fam=(Table*)malloc(sizeof(Table));
		fam=tab->T;
		while(fam!=NULL)
		{
		    
		    strcpy(chemin,"BASE/");
		    strcat(chemin,tab->TName);
		    strcat(chemin,"/");
		    strcat(chemin,fam->FName);
		    mkdir(chemin,0777);
		    col=fam->F;
		    
				strcat(chemin,"/list_columns.txt");
				LIST_COLUMN=fopen(chemin,"a+");
				
			while(col !=NULL)
			{
				fprintf(LIST_COLUMN,"{[%s]}.",col->CName);
				col=col->next;
			};
			fclose(LIST_COLUMN);
			col=(Family*)malloc(sizeof(Family));
			col=fam->F;
			while(col !=NULL)
			{
				
				strcpy(chemin,"BASE/");
				strcat(chemin,tab->TName);
				strcat(chemin,"/");
				strcat(chemin,fam->FName);
				strcat(chemin,"/");
				strcat(chemin,col->CName);
				mkdir(chemin,0777);
				
				row=col->C;
				
				
				    strcat(chemin,"/list_row.txt");
				    LIST_ROW=fopen(chemin,"a+");
				    
				while(row!=NULL)
				{
					fprintf(LIST_ROW,"{[%s],[%s],[%ld]}.",row->Rowkey,row->CellValue,row->Timestamp);
					row=row->next;
				}
				fclose(LIST_ROW); 
				col=col->next;
			} fam=fam->next;
		}} tab = tab->next;
    }
	
}

//variables stockant les arguments essentiels de put en ignorant les espaces, les virgules et les apostrohpes
char nom_cmd[50],nom_table[50], nom_famille[50], nom_colonne[50], nom_row[50], value_a_inserer[50];
long timestamp_a_inserer;
long timevar=0;

void put_exe() /*procédure d'insertion dans la BD*/
{timestamp_a_inserer=0;
  DataBase *parkour_DB, *varT;
  Table *parkour_table, *varF;
  Family *parkour_family, *col, *varC;
  Column *parkour_row, *row, *varR;
char *ch[5];
int i=0, var=0;
long ts=0;

  parkour_DB = (DataBase*)malloc(sizeof(DataBase));
  varT = (DataBase*)malloc(sizeof(DataBase));
  varF = (Table*)malloc(sizeof(Table));
  varC = (Family*)malloc(sizeof(Family));
  varR = (Column*)malloc(sizeof(Column));
  parkour_table = (Table*)malloc(sizeof(Table));
  parkour_family = (Family*)malloc(sizeof(Family));
  col = (Family*)malloc(sizeof(Family));
  parkour_row = (Column*)malloc(sizeof(Column));
  row = (Column*)malloc(sizeof(Column));
  
  parkour_DB=DB;
  
  
  while(var==0 && parkour_DB!=NULL) /*chercher le table name dans la BD*/ 
  {
    if(strcmp(parkour_DB->TName,nom_table)==0)
      var = 1;
    else
      parkour_DB=parkour_DB->next;
  }
  if(var==1) 
  {
    
    var=0;
    parkour_table=parkour_DB->T;
    while(var == 0 && parkour_table!=NULL) /*chercher la column family une fois la table trouvée*/
    {
      if(strcmp(parkour_table->FName,nom_famille)==0)
        var = 1;
      else 
        parkour_table=parkour_table->next;
    }
    if(var==1)
    {
	  
      var=0;
      parkour_family=parkour_table->F;   
      while(var==0 && parkour_family!=NULL) /*chercher le column qualifier une fois la column family trouvée*/
      {
        if(strcmp(parkour_family->CName,nom_colonne)==0)
          var=1;
        else
          parkour_family=parkour_family->next;
      }
      if(var==1)
      {
	    
        var=0;
        parkour_row=parkour_family->C;
        while(parkour_row!=NULL) /*chercher la rowkey une fois le column qualifier trouvé*/
        {
          if(strcmp(parkour_row->Rowkey,nom_row)==0) {
            var++; if(ts>parkour_row->Timestamp) ts = parkour_row->Timestamp;}
          parkour_row=parkour_row->next;
        }
		
		if(var==parkour_table->NVersions)
		{	var = 0;
			parkour_row=parkour_family->C;
			while(var==0 && parkour_row!=NULL) /*chercher la rowkey une fois le column qualifier trouvé*/
			{
			  if(strcmp(parkour_row->Rowkey,nom_row)==0 && parkour_row->Timestamp == ts)
				var=1;
			  else
			    parkour_row=parkour_row->next;
			}printf("Commande réussite\n");
			strcpy(parkour_row->CellValue,value_a_inserer);
			if(timestamp_a_inserer == 0) {parkour_row->Timestamp=timevar; timevar++; }else parkour_row->Timestamp=timestamp_a_inserer;
		}
		else
		{	
			if(timestamp_a_inserer == 0) {row->Timestamp=timevar; timevar++; }else row->Timestamp=timestamp_a_inserer;
			strcpy(row->CellValue,value_a_inserer);
			strcpy(row->Rowkey,nom_row);
			row->next=parkour_family->C;
			parkour_family->C=row;
			printf("Commande réussite\n");
		}
	  }
	  else
	  { 	
		strcpy(col->CName,nom_colonne);
		if(timestamp_a_inserer == 0) {row->Timestamp=timevar; timevar++;} else row->Timestamp=timestamp_a_inserer;
		strcpy(row->CellValue,value_a_inserer);
		strcpy(row->Rowkey,nom_row);
		row->next=NULL;

		col->C = (Column*)malloc(sizeof(Column));
		col->C = row;

		col->next = parkour_table->F;
		parkour_table->F = col;
		printf("Commande réussite\n");
	  }
	}
	else printf("Erreur : La famille '%s' n'existe pas !\n",nom_famille);
  }
  else printf("Erreur : La table '%s' n'existe pas !\n",nom_table);
}

boolean pu_t() /*analyse lexical avec appel à put_exe()*/
{boolean result = false;
  
  
  

  
  if(token==PUT)
  {
    strcpy(nom_cmd,yytext);
    token=_lire_token();
    
    if(token==SPACE)
    {token=_lire_token();
      
      if(token==APOSTROPHE)
      {token=_lire_token();
	
	if(token==IDF) /*nom de la table où j'insère => 3ème token*/
	{
	  strcpy(nom_table,yytext);
	  token=_lire_token();
	  
	  if(token==APOSTROPHE)
	  {token=_lire_token();
	    
	    if(token==COMMA)
	    {token=_lire_token();
	      
	      if(token==SPACE)
	      {token=_lire_token();  
		
		if(token==APOSTROPHE)
		{token=_lire_token();
		  
		  if(token==IDF) /*nom du Rowkey où j'insère => 7ème token*/
		  {
		    strcpy(nom_row,yytext);
		    token=_lire_token();
		    
		    if(token==APOSTROPHE)
		    {token=_lire_token();
		      
		      if(token==COMMA)
		      {token=_lire_token();
			
			if(token==SPACE)
			{token=_lire_token();
			  
			  if(token==APOSTROPHE)
			  {token=_lire_token();
			    
			    if(token==IDF) /*columnFamily où j'insère => 11ème token*/
			    {
			      strcpy(nom_famille,yytext);
			      token=_lire_token();
			      
			      if(token==DBL_PT)
			      {token=_lire_token();
				
				if(token==IDF)/*ColumnQualifier où j'insère => 13ème token*/
				{
				  strcpy(nom_colonne,yytext);
				  token=_lire_token();
				  
				  if(token==APOSTROPHE)
				  {token=_lire_token();
				    
				    if(token==COMMA)
				    {token=_lire_token();
				      
				      if(token==SPACE)
				      {token=_lire_token();
					
					if(token==APOSTROPHE)
					{token=_lire_token();
					  
					  if(token==IDF||token==CH||token==INUMBER||token==DNUMBER) /*value que j'insère => 15ème token*/
					  {
					    strcpy(value_a_inserer,yytext);
					    token=_lire_token();
					    
					    if(token==APOSTROPHE)
					    {token=_lire_token();
					      
					      if(token==EOL) /*version élémentaire de put acceptée*/
					      {
						result=true;
						put_exe();
					      }else
					      {
						if(token==COMMA)
						{token=_lire_token();
						  
						  if(token==SPACE)
						  {token=_lire_token();
						    
						    if(token==DNUMBER || token==INUMBER) /*insérer le Timestamp => 18ème token éventuel*/
						    {
						      timestamp_a_inserer=strtoll(yytext, NULL, 0);
						      token=_lire_token();
						      if(token==EOL) /*put avec timestamp acceptée*/
						      {
							
							put_exe();result=true;
							
						      }else { printf("arguments en excès \n");};
						    }else { printf("il manque un timestamp\n");};
						  }else { printf("il manque un espace \n");};
						}else { printf("il manque une virgule \n");};
					      };
					    }else { printf("il manque un apostrophe \n");};
					  }else { printf("il manque la valeur à insérer \n");};
					}else { printf("il manque un apostrophe \n");};
				      }else { printf("il manque un espace \n");};
				    }else { printf("il manque un apostrophe \n");};
				  }else { printf("il manque un apostrophe \n");};
				}else { printf("il manque le nom de la colonne \n");};
			      }else { printf("il manque ':' \n");};
			    }else { printf("il manque la colum family \n");};
			  }else { printf("il manque un apostrophe \n");};
			}else { printf("il manque un espace \n");};
		      }else { printf("il manque une virgule \n");};
		    }else { printf("il manque un apostrophe \n");};
		  }else { printf("il manque la rowkey \n");};
		}else { printf("il manque un apostrophe \n");};
	      }else { printf("il manque un espace \n");};
	    }else { printf("il manque une virgule \n");};
	  }else { printf("il manque un apostrophe \n");};
	}else { printf("il manque le nom de la table \n");};
      }else { printf("il manque un apostrophe \n");};
    }else { printf("il manque un espace \n");};
  }else { printf("la commande tapée n'est pas 'put' \n");};
return result;
}

boolean _delete_all()
{
    boolean result;
    int existt=0;
    int existc=0;
    int existr=0;
    int existf=0;
    int existts=0;
    long ts=0 ;
    int first=0 ;
    char tab[50] ;
    char col[50] ;
    char row[50] ;
    char fam[50] ;
    DataBase* p;
    p = (DataBase*)malloc(sizeof(DataBase));
    Table* newfam ;
    newfam=(Table*)malloc(sizeof(Table));
    Family* newcol ;
    newcol=(Family*)malloc(sizeof(Family));
    Column* entry ;
    entry=(Column*)malloc(sizeof(Column));
    Column* entry2 ;
    entry2=(Column*)malloc(sizeof(Column));
    token=_lire_token();

    if(token==SPACE)
    {
        token=_lire_token();

        if(token==APOSTROPHE)
        {
            token=_lire_token();

            if(token==IDF)
            {
                p=DB;
                strcpy(tab,yytext) ;
                while(p != NULL && existt != 1)
                {
                    if(strcmp(tab,p->TName)==0)
                    {
                        existt = 1;
                    }
                    else p=p->next ;
                }
                token=_lire_token();


                if(token==APOSTROPHE)
                {
                    token=_lire_token();

                    if(token==COMMA)
                    {
                        token=_lire_token();

                        if(token==SPACE)
                        {
                            token=_lire_token();

                            if(token==APOSTROPHE)
                            {
                                token=_lire_token();

                                if(token==IDF) /*nom du row*/
                                {
                                    strcpy(row,yytext) ;
                                    token=_lire_token();
                                    if(token==APOSTROPHE)
                                    {
                                        token=_lire_token();

                                        if(token==EOL) /*deleteall 'table', 'row' à refuser à cause de la conception de la BD*/
                                        {
                                            newfam=p->T;
                                       
                                            while (newfam != NULL && existt==1 )
                                            {
                                                

                                                newcol=newfam->F ;
                                                while(newcol != NULL && existt==1)
                                                {
                                                   
                                                    entry=newcol->C;
                                                    entry2=newcol->C->next;

                                                    
                                                    if(strcmp(entry->Rowkey,row)==0)
                                                    {
                                                        existr=1;
                                                        newcol->C=newcol->C->next;
                                                    }
                                                    else
                                                    {

                                                        while(entry2!=NULL )
                                                        {
                                                            if(strcmp(entry2->Rowkey,row)==0)
                                                            {
                                                                existr = 1;
                                                                entry->next=entry2->next ;
                                                            }
                                                            entry=entry->next;
                                                            entry2=entry2->next;
                                                        }
                                                    }
                                                    if (existt ==0)
                                                    {
                                                        printf ("La table %s n'existe pas\n",tab);
                                                    }
                                                    else
                                                    {
                                                        if (existr ==0)
                                                        {
                                                            printf ("L'enregistrement correspondant au Rowkey : %s n'existe pas\n",row);
                                                        }
                                                    }

                                                    //entry=newcol->C;
                                                    //entry2=newcol->C->next;
                                                    //newcol=newcol->next;
                                                    newcol=newcol->next;
                                                }

                                                newfam=newfam->next;
                                            }

                                            result=true;
                                        }
                                        else
                                        {
                                            if(token==COMMA)
                                            {
                                                token=_lire_token();
                                                if(token==SPACE)
                                                {
                                                    token=_lire_token();
                                                    if(token==APOSTROPHE)
                                                    {
                                                        token=_lire_token();
                                                        if(token==IDF) /*NOM DE LA COLUMN FAMILY*/
                                                        {
                                                            strcpy(fam,yytext) ;
                                                            token=_lire_token();
                                                            if(token==DBL_PT)
                                                            {
                                                                token=_lire_token();
                                                                if(token==IDF) /*NOM DU COLUMN QUALIFIER*/
                                                                {
                                                                    newfam=p->T;
                                                                    strcpy(col,yytext);
                                                                    
                                                                    while (newfam != NULL && existf != 1 && existt==1)
                                                                    {
                                                                        ;
                                                                        if(strcmp(fam,newfam->FName)==0)
                                                                        {
                                                                            existf = 1;
                                                                            newcol=newfam->F ;
                                                                            while(newcol != NULL && existc != 1 && existt==1 && existf==1)
                                                                            {
                                                                                
                                                                                if(strcmp(col,newcol->CName)==0)
                                                                                {
                                                                                    existc = 1;
                                                                                    entry=newcol->C;
                                                                                    entry2=newcol->C->next;
                                                                                }
                                                                                else  newcol=newcol->next;
                                                                            }
                                                                        }
                                                                        else newfam=newfam->next;
                                                                    }

                                                                    token=_lire_token();
                                                                    if(token==APOSTROPHE)
                                                                    {
                                                                        token=_lire_token();
                                                                        if(token==EOL) /*deleteall 'table', 'row', 'column_fam:column_qualif' acceptée*/
                                                                        {
                                                                            
                                                                            if(strcmp(entry->Rowkey,row)==0)
                                                                            {
                                                                                existr=1;
                                                                                newcol->C=newcol->C->next;
                                                                            }
                                                                            else
                                                                            {

                                                                                while(entry2!=NULL )
                                                                                {
                                                                                    if(strcmp(entry2->Rowkey,row)==0)
                                                                                    {
                                                                                        existr = 1;
                                                                                        entry->next=entry2->next ;
                                                                                    }
                                                                                    entry=entry->next;
                                                                                    entry2=entry2->next;
                                                                                }
                                                                            }

                                                                            if (existt ==0)
                                                                            {
                                                                                printf ("La table %s n'existe pas\n",tab);
                                                                            }
                                                                            else
                                                                            {
                                                                                if (existf ==0)
                                                                                {
                                                                                    printf ("La famille %s n'existe pas\n",fam);
                                                                                }
                                                                                else
                                                                                {
                                                                                    if (existc ==0)
                                                                                    {
                                                                                        printf ("La colonne %s n'existe pas\n",col);
                                                                                    }
                                                                                    else
                                                                                    {
                                                                                        if (existr ==0)
                                                                                        {
                                                                                            printf ("L'enregistrement correspondant au Rowkey : %s n'existe pas\n",row);
                                                                                        }
                                                                                    }
                                                                                }
                                                                            }
                                                                            result=true;
                                                                        }
                                                                        else
                                                                        {
                                                                            if(token==COMMA)
                                                                            {
                                                                                token=_lire_token();
                                                                                if(token==SPACE)
                                                                                {
                                                                                    token=_lire_token();
                                                                                    if(token==INUMBER) /*TIMESTAMP*/
                                                                                    {

                                                                                        ts=strtoll(yytext, NULL, 0);
                                                                                        token=_lire_token();
                                                                                        if(token==EOL)
                                                                                        {
                                                                                            
                                                                                            if(ts==entry->Timestamp && strcmp(entry->Rowkey,row)==0)
                                                                                            {
                                                                                               
                                                                                                existr=1;
                                                                                                newcol->C=newcol->C->next;
                                                                                            }
                                                                                            else
                                                                                            {

                                                                                                while(entry2!=NULL && existr != 1)
                                                                                                {
                                                                                                    if(ts==entry2->Timestamp && strcmp(entry2->Rowkey,row)==0)
                                                                                                    {
                                                                                                        existr = 1;
                                                                                                        entry->next=entry2->next ;
                                                                                                    }
                                                                                                    else
                                                                                                    {
                                                                                                        entry=entry->next;
                                                                                                        entry2=entry2->next;
                                                                                                    }
                                                                                                }
                                                                                            }

                                                                                            if (existt ==0)
                                                                                            {
                                                                                                printf ("La table %s n'existe pas\n",tab);
                                                                                            }
                                                                                            else
                                                                                            {
                                                                                                if (existf ==0)
                                                                                                {
                                                                                                    printf ("La famille %s n'existe pas\n",fam);
                                                                                                }
                                                                                                else
                                                                                                {
                                                                                                    if (existc ==0)
                                                                                                    {
                                                                                                        printf ("La colonne %s n'existe pas\n",col);
                                                                                                    }
                                                                                                    else
                                                                                                    {
                                                                                                        if (existr ==0)
                                                                                                        {
                                                                                                            printf ("L'enregistrement correspondant au Rowkey : %s et au Timestamp : %ld n'existe pas\n",row,ts);
                                                                                                        }
                                                                                                    }
                                                                                                }
                                                                                            }
                                                                                            result=true;


                                                                                        }
                                                                                        else {printf("argument(s) en excès \n");
                                                                                        result=false;}
                                                                                    }
                                                                                    else {printf("il manque un timestamp \n");
                                                                                    result=false;}
                                                                                }
                                                                                else {printf("il manque un espace \n");
                                                                                result=false;}
                                                                            }
                                                                            else {printf("il manque une virgule \n");
                                                                            result=false;}
                                                                        };
                                                                    }
                                                                    else {printf("il manque un apostrophe1\n");
                                                                    result=false;}
                                                                }
                                                                else {printf("il manque un column qualifier \n");
                                                                result=false;}
                                                            }
                                                            else {printf("il manque un ':'\n");
                                                            result=false;}
                                                        }
                                                        else {printf("il manque le nom de la famille \n");
                                                        result=false;}
                                                    }
                                                    else {printf("il manque un apostrophe2\n");
                                                    result=false;}
                                                }
                                                else {printf("il manque un espace \n");
                                                result=false;}
                                            }
                                            else {printf("il manque une virgule \n");
                                            result=false;}
                                        };
                                    }
                                    else {printf("il manque un apostrophe3\n");
                                    result=false;}
                                }
                                else {printf("il manque le nom du row \n");
                                result=false;}
                            }
                            else {printf("il manque un apostrophe4\n");
                            result=false;}

                        }
                        else {printf("il manque un espace \n");
                        result=false;}
                    }
                    else {printf("il manque une virgule \n");
                    result=false;}
                }
                else {printf("il manque un apostrophe5\n");
                result=false;}
            }
            else {printf("il manque le nom de la table \n");
            result=false;}
        }
        else {printf("il manque un apostrophe6\n");
        result=false;}
    }
    else {printf("il manque un espace\n");
    result=false;}
    return result;
}


boolean _whoami()
{
  if(token == WHOAMI)
  {
    token=_lire_token();
    if(token == EOL)
    {
      printf("soundouss\n");
      return true;
    }
    else
      return false;
  }
  else
    return false;
}

boolean _exists()
{
  int Texist = 0;
  DataBase *Tab;
  Tab = (DataBase*)malloc(sizeof(DataBase));
  char *ch;
  ch = (char*)malloc(50*sizeof(char));
    	
	if(token == EXISTS)
	{
		token=_lire_token();
		if(token == SPACE)
		{
			token=_lire_token();
			if(token == APOSTROPHE)
			{
				token=_lire_token();
				if(token == IDF)
				{
					strcpy(ch, yytext);
					token=_lire_token();
					if(token == APOSTROPHE)
					{
						token=_lire_token();
						if(token == EOL)
						{
							Tab=DB;
							while(Tab != NULL && Texist==0) 
							{
							if(strcmp(Tab->TName, ch)==0)
								Texist = 1;
							else
							Tab=Tab->next;
							}
							if(Texist==1)
								printf("Oui\n");
							if(Texist==0)
								printf("Non\n");
							return true;
						}
						else return false;
					}
					else return false;
				}
				else return false;
			}
			else return false;
		}
		else return false;
	}
	else return false;	              
}
boolean _list()
{
  DataBase *Tab;
  Tab = (DataBase*)malloc(sizeof(DataBase));
  if(token == LIST)
  {
    token=_lire_token();
    if(token == EOL)
    {
	Tab=DB;
	if(Tab->next==NULL) printf("Il n'y a aucune table dans la base de données !\n");
	else
	{
		while(Tab != NULL) 
		{
			printf("%s\t",Tab->TName);
			Tab=Tab->next;
		}
		printf("Commande réussite\n");
	}
      return true;
    }
    else
      return false;
  }
  else
    return false;
}

boolean _drop()
{
  int Texist = 0, error = 0;
  char *ch;
  ch = (char*)malloc(50*sizeof(char));
  DataBase *Tab1;
  Tab1 = (DataBase*)malloc(sizeof(DataBase));
  DataBase *Tab2;
  Tab2 = (DataBase*)malloc(sizeof(DataBase));
    	
	if(token == DROP)
	{
		token=_lire_token();
		if(token == SPACE)
		{
			token=_lire_token();
			if(token == APOSTROPHE)
			{
				token=_lire_token();
				if(token == IDF)
				{	
					strcpy(ch,yytext);
					token=_lire_token();
					if(token == APOSTROPHE)
					{
						token=_lire_token();
						if(token == EOL)
						{
							Tab1=DB->next;
							Tab2=DB;
							if(Tab2 != NULL && strcmp(Tab2->TName, ch)==0)
							{	
								Texist=1;
								if(Tab2->Etat==1) printf("La table doit etre d'abord en mode 'disable'\n");
								if(Tab2->Etat==0)
								{
									DB=DB->next;
									printf("La table a été supprimée avec succès\n");
									
								}
							}
							else
							{
							while(Tab1 != NULL && Texist==0) 
							{
							if(strcmp(Tab1->TName, yytext)==0)
								Texist = 1;
								else{
									Tab1=Tab1->next;
									Tab2=Tab2->next;}
									}
							if(Texist==1)
						
			{	
								if(Tab1->Etat==1) printf("La table doit etre d'abord en mode 'disable'\n");
								if(Tab1->Etat==0)
								{
									Tab2->next=Tab1->next;
									printf("La table a été supprimée avec succès\n");
									
								}
								
							}
							}
							if(Texist==0) {error=1;}
							if(error==1) {printf("La table '%s' n'existe pas\n",ch);}
							return true;
						}
						else return false;
					}
					else return false;
				}
				else return false;
			}
			else return false;
		}
		else return false;
	}
	else return false;	              
}

boolean _is_disabled()
{
  int Texist = 0, error = 0;
  DataBase *Tab;
  Tab = (DataBase*)malloc(sizeof(DataBase));
  char *ch;
  ch = (char*)malloc(50*sizeof(char));
    	
	if(token == IS_DISABLED)
	{
		token=_lire_token();
		if(token == SPACE)
		{
			token=_lire_token();
			if(token == APOSTROPHE)
			{
				token=_lire_token();
				if(token == IDF)
				{
					strcpy(ch, yytext);
					token=_lire_token();
					if(token == APOSTROPHE)
					{
						token=_lire_token();
						if(token == EOL)
						{
						Tab=DB;
						while(Tab != NULL && Texist==0) 
						{
						if(strcmp(Tab->TName, ch)==0)
							Texist = 1;
						else
						Tab=Tab->next;
						}
						if(Texist==1)
						{	
							if(Tab->Etat==0) printf("Oui\n");
							if(Tab->Etat==1) printf("Non\n");
							
						}
						if(Texist==0) {error=1;}
						if(error==1) {printf("La table '%s' n'existe pas\n", ch);}
						
						return true;
						}
						else return false;
					}
					else return false;
				}
				else return false;
			}
			else return false;
		}
		else return false;
	}
	else return false;	              
}

boolean _disable()
{
  int Texist = 0, error = 0;
  DataBase *Tab;
  Tab = (DataBase*)malloc(sizeof(DataBase));
  char *ch;
  ch = (char*)malloc(50*sizeof(char));
    	
	if(token == DISABLE)
	{
		token=_lire_token();
		if(token == SPACE)
		{
			token=_lire_token();
			if(token == APOSTROPHE)
			{
				token=_lire_token();
				if(token == IDF)
				{	
					strcpy(ch, yytext);
					token=_lire_token();
					if(token == APOSTROPHE)
					{
						token=_lire_token();
						if(token == EOL)
						{	
							Tab=DB;
							while(Tab != NULL && Texist==0) 
							{
							if(strcmp(Tab->TName, ch)==0)
								Texist = 1;
							else
							Tab=Tab->next;
							}
							if(Texist==1)
							{	
								Tab->Etat=0;
							}
							if(Texist==0) {error=1;}
							if(error==1) {printf("La table '%s' n'existe pas\n",ch);}
							else if(error==0) {printf("Commande réussite\n");}
							return true;
						}
						else return false;
					}
					else return false;
				}
				else return false;
			}
			else return false;
		}
		else return false;
	}
	else return false;	              
}

boolean _is_enabled()
{
  int Texist = 0, error = 0;
  DataBase *Tab;
  Tab = (DataBase*)malloc(sizeof(DataBase));
  char *ch;
  ch = (char*)malloc(50*sizeof(char));
    	
	if(token == IS_ENABLED)
	{
		token=_lire_token();
		if(token == SPACE)
		{
			token=_lire_token();
			if(token == APOSTROPHE)
			{
				token=_lire_token();
				if(token == IDF)
				{	
					strcpy(ch, yytext);
					token=_lire_token();
					if(token == APOSTROPHE)
					{
						token=_lire_token();
						if(token == EOL)
						{
							Tab=DB;
							while(Tab != NULL && Texist==0) 
							{
							if(strcmp(Tab->TName, ch)==0)
								Texist = 1;
							else
							Tab=Tab->next;
							}
							if(Texist==1)
							{	
								if(Tab->Etat==1) printf("Oui\n");
								if(Tab->Etat==0) printf("Non\n");
								
							}
							if(Texist==0) {error=1;}
							if(error==1) {printf("La table '%s' n'existe pas\n",ch);}
							
							return true;
						}
						else return false;
					}
					else return false;
				}
				else return false;
			}
			else return false;
		}
		else return false;
	}
	else return false;	              
}

boolean _enable()
{
  int Texist = 0, error = 0;
  DataBase *Tab;
  Tab = (DataBase*)malloc(sizeof(DataBase));
  char *ch;
  ch = (char*)malloc(50*sizeof(char));  
    	
	if(token == ENABLE)
	{
		token=_lire_token();
		if(token == SPACE)
		{
			token=_lire_token();
			if(token == APOSTROPHE)
			{
				token=_lire_token();
				if(token == IDF)
				{
					strcpy(ch, yytext);
					token=_lire_token();
					if(token == APOSTROPHE)
					{
						token=_lire_token();
						if(token == EOL)
						{
							Tab=DB;
							while(Tab != NULL && Texist==0) 
							{
							if(strcmp(Tab->TName, ch)==0)
								Texist = 1;
							else
							Tab=Tab->next;
							}
							if(Texist==1)
							{	
								Tab->Etat=1;
							}
							if(Texist==0) {error=1;}
							if(error==1) {printf("La table '%s' n'existe pas\n",ch);}
							else if(error==0) {printf("Commande réussite\n");}
							return true;
						}
						else return false;
					}
					else return false;
				}
				else return false;
			}
			else return false;
		}
		else return false;
	}
	else return false;	              
}

boolean _truncate()
{
  int Texist = 0, error = 0;
  DataBase *NewTab, *Tab;
  Table *NewFam, *Fam;
  NewTab = (DataBase*)malloc(sizeof(DataBase));
  NewFam = (Table*)malloc(sizeof(Table));
  Tab = (DataBase*)malloc(sizeof(DataBase));
  Fam = (Table*)malloc(sizeof(Table));
  char *ch;
  ch = (char*)malloc(50*sizeof(char));
  
	if(token == TRUNCATE)
	{
		token=_lire_token();
		if(token == SPACE)
		{
			token=_lire_token();
			if(token == APOSTROPHE)
			{
				token=_lire_token();
				if(token == IDF)
				{	
					strcpy(ch, yytext);
					token=_lire_token();
					if(token == APOSTROPHE)
					{
						token=_lire_token();
						if(token == EOL)
						{	
							Tab=DB;
							while(Tab != NULL && Texist==0) 
							{
							if(strcmp(Tab->TName, ch)==0)
								Texist = 1;
							else
							Tab=Tab->next;
							}
							if(Texist==1)
							{	
								Fam=Tab->T;
								
								while(Fam != NULL)
								{
									Fam->F=NULL;
									Fam=Fam->next;
								}
							}
							Tab=NewTab;
							if(Texist==0) {error=1;}
							if(error==1) {printf("La table '%s' n'existe pas\n",ch);}
							else if(error==0) {printf("Commande réussite\n");}
							return true;
						}
						else return false;
					}
					else return false;
				}
				else return false;
			}
			else return false;
		}
	else return false;	              
	}
else return false;	              
}


boolean _create()
{
  int Texist = 0,j, Fexist = 0, error = 0, ver;
  DataBase *NewTab, *Tab;
  Table *NewFam, *Fam;
  NewTab = (DataBase*)malloc(sizeof(DataBase));
  NewFam = (Table*)malloc(sizeof(Table));
  Tab = (DataBase*)malloc(sizeof(DataBase));
  Fam = (Table*)malloc(sizeof(Table));

  if(token == CREATE)
  {
    token=_lire_token();
    if(token == SPACE)
    {
		token=_lire_token();
		if(token == APOSTROPHE)
		{
			token=_lire_token();
			if(token == IDF)
			{
				Tab=DB;
				while(Tab != NULL) {

					if(strcmp(yytext,Tab->TName)==0) {
						Texist = 1; error = 1; }
					Tab=Tab->next;
				}

				if(Texist == 0) { 
					strcpy(NewTab->TName,yytext); 
					NewTab->next=NULL; }
				else
					printf("ERROR : Cette table existe déjà !\n");

				token=_lire_token();
				if(token == APOSTROPHE)
				{
					token=_lire_token();
					if(token == COMMA)
					{
						token=_lire_token();
						if(token == SPACE)
						{
							token=_lire_token();
							if(token == OBRACES)
							{
								token=_lire_token();
								if(token == NAME)
								{
									token=_lire_token();
									if(token == SPACE)
									{
										token=_lire_token();
										if(token == ARROW)
										{
											token=_lire_token();
											if(token == SPACE)
											{
												token=_lire_token();
												if(token == APOSTROPHE)
												{
													token=_lire_token();
													if(token == IDF)
													{
														if(error == 0)
														{
															NewTab->T = (Table*)malloc(sizeof(Table));
															strcpy(NewTab->T->FName,yytext);								
															
														}
														token=_lire_token();
														if(token == APOSTROPHE)
														{
															token=_lire_token();
															if(token == COMMA)
															{
																token=_lire_token();
																if(token == SPACE)
																{
																	token=_lire_token();
																	if(token == VERSIONS)
																	{
																		token=_lire_token();
																		if(token == SPACE)
																		{
																			token=_lire_token();
																			if(token == ARROW)
																			{
																				token=_lire_token();
																				if(token == SPACE)
																				{
																					token=_lire_token();
																					if(token == INUMBER)
																					{
																						 
																							if(error = 0)
																							{
																								
																									NewTab->T->NVersions = atoi(yytext);
																									NewTab->T->next = NULL;
																								
																							}
																						
																						token=_lire_token();
																						if(token == CBRACES)
																						{
																							token=_lire_token();
																							while(token != EOL)
																							{
																								if(token == COMMA)
																								{
																									token=_lire_token();
																									if(token == SPACE)
																									{
																										token=_lire_token();
																										if(token == OBRACES)
																										{
																											token=_lire_token();
																											if(token == NAME)
																											{
																												token=_lire_token();
																												if(token == SPACE)
																												{
																													token=_lire_token();
																													if(token == ARROW)
																													{
																														token=_lire_token();
																														if(token == SPACE)
																														{
																															token=_lire_token();
																															if(token == APOSTROPHE)
																															{
																																token=_lire_token();
																																if(token == IDF)
																																{
																																	if(error == 0)
																																	{
																																		Fam = NewTab->T;
																																		Fexist = 0;
																																		while(Fam != NULL)
																																		{
																																			if(strcmp(Fam->FName,yytext)==0)
																																				Fexist = 1;
																																			Fam = Fam->next;
																																		}
																																		if(Fexist == 0)
																																		{
																																			NewFam = (Table*)malloc(sizeof(Table));
																																			strcpy(NewFam->FName,yytext);
																														
																																		}
																																		else 
																																		{
																																			printf("ERROR : Deux familles prennent le même nom dans cette table --> '%s'\n", yytext);
																																			error = 1;
																																		}
																																	}
																																	token=_lire_token();
																																	if(token == APOSTROPHE)
																																	{
																																		token=_lire_token();
																																		if(token == COMMA)
																																		{
																																			token=_lire_token();
																																			if(token == SPACE)
																																			{
																																				token=_lire_token();
																																				if(token == VERSIONS)
																																				{
																																					token=_lire_token();
																																					if(token == SPACE)
																																					{
																																						token=_lire_token();
																																						if(token == ARROW)
																																						{
																																							token=_lire_token();
																																							if(token == SPACE)
																																							{
																																								token=_lire_token();
																																								if(token == INUMBER)
																																								{
																																									
																																									if(error = 0)
																																									{
																																										
																																											NewFam->NVersions = atoi(yytext);
																																											NewFam->next = NewTab->T;
																																											NewTab->T = NewFam;
																																										
																																									}
																																									token=_lire_token();
																																									if(token == CBRACES)
																																										token=_lire_token();
																																									else return false;
																																								}
																																								else return false;
																																							}
																																							else return false;
																																						}
																																						else return false;
																																					}
																																					else return false;
																																				}
																																				else return false;
																																			}
																																			else return false;
																																		}
																																		else
																																		{
																																			if(token == CBRACES)
																																			{
																																				NewFam->NVersions = 1;
																																				NewFam->next = NewTab->T;
																																				NewTab->T = NewFam;
																																				
																																				token=_lire_token();
																																			}
																																		}
																																	}
																																	else return false;
																																}
																																else return false;
																															}
																															else return false;
																														}
																														else return false;
																													}
																													else return false;
																												}
																												else return false;
																											}
																											else return false;
																										}
																										else return false;
																									}
																									else return false;
																								}
																								else return false;
																							}
																						}
																						else return false;
																					}
																					else return false;
																				}
																				else return false;
																			}
																			else return false;
																		}
																		else return false;
																	}
																	else return false;
																}
																else return false;
															}
															else
															{
																if(token == CBRACES)
																{
																	NewTab->T->NVersions=1;
																	NewTab->T->next=NULL;
																	
																	token = _lire_token();
																	while(token != EOL)
																	{
																		if(token == COMMA)
																		{
																			token=_lire_token();
																			if(token == SPACE)
																			{
																				token=_lire_token();
																				if(token == OBRACES)
																				{
																					token=_lire_token();
																					if(token == NAME)
																					{
																						token=_lire_token();
																						if(token == SPACE)
																						{
																							token=_lire_token();
																							if(token == ARROW)
																							{
																								token=_lire_token();
																								if(token == SPACE)
																								{
																									token=_lire_token();
																									if(token == APOSTROPHE)
																									{
																										token=_lire_token();
																										if(token == IDF)
																										{
																											if(error == 0)
																											{
																												Fam = NewTab->T;
																												Fexist = 0;
																												while(Fam != NULL)
																												{
																													if(strcmp(Fam->FName,yytext)==0)
																														Fexist = 1;
																													Fam = Fam->next;
																												}
																												if(Fexist == 0)
																												{
																													NewFam = (Table*)malloc(sizeof(Table));
																													strcpy(NewFam->FName,yytext);
																								
																												}
																												else 
																												{
																													printf("ERROR : Deux familles prennent le même nom dans cette table --> '%s'\n", yytext);
																													error = 1;
																												}
																											}
																											token=_lire_token();
																											if(token == APOSTROPHE)
																											{
																												token=_lire_token();
																												if(token == COMMA)
																												{
																													token=_lire_token();
																													if(token == SPACE)
																													{
																														token=_lire_token();
																														if(token == VERSIONS)
																														{
																															token=_lire_token();
																															if(token == SPACE)
																															{
																																token=_lire_token();
																																if(token == ARROW)
																																{
																																	token=_lire_token();
																																	if(token == SPACE)
																																	{
																																		token=_lire_token();
																																		if(token == INUMBER)
																																		{
																																			
																																			if(error = 0)
																																			{
																																				
																																					NewFam->NVersions = atoi(yytext);
																																					NewFam->next = NewTab->T;
																																					NewTab->T = NewFam;
																																				
																																			}
																																			token=_lire_token();
																																			if(token == CBRACES)
																																				token=_lire_token();
																																			else return false;
																																		}
																																		else return false;
																																	}
																																	else return false;
																																}
																																else return false;
																															}
																															else return false;
																														}
																														else return false;
																													}
																													else return false;
																												}
																												else
																												{
																													if(token == CBRACES)
																													{
																														NewFam->NVersions = 1;
																														NewFam->next = NewTab->T;
																														NewTab->T = NewFam;
																														
																														token=_lire_token();
																													}
																												}
																											}
																											else return false;
																										}
																										else return false;
																									}
																									else return false;
																								}
																								else return false;
																							}
																							else return false;
																						}
																						else return false;
																					}
																					else return false;
																				}
																				else return false;
																			}
																			else return false;
																		}
																		else return false;
																	}
																}
															}
														}
														else return false;
													}
													else return false;
												}
												else return false;
											}
											else return false;
										}
										else return false;
									}
									else return false;
								}
								else return false;
							}
							else
							{
								if(token == APOSTROPHE)
								{
									token=_lire_token();
									if(token == IDF)
									{
										if(error == 0)
										{
											NewTab->T = (Table*)malloc(sizeof(Table));
											strcpy(NewTab->T->FName,yytext);
											NewTab->T->NVersions = 1;
											NewTab->T->next = NULL;
											
										}
										token=_lire_token();
										if(token == APOSTROPHE)
										{
											token=_lire_token();
											while(token != EOL)
											{
												if(token == COMMA)
												{
													token=_lire_token();
													if(token == SPACE)
													{
														token=_lire_token();
														if(token == APOSTROPHE)
														{
															token=_lire_token();
															if(token == IDF)
															{
																if(Texist == 0)
																{
																	Fam = NewTab->T;
																	Fexist = 0;
																	while(Fam != NULL)
																	{
																		if(strcmp(Fam->FName,yytext)==0){
																			Fexist = 1;}
																		Fam = Fam->next;
																	}
																	if(Fexist == 0)
																	{NewFam = (Table*)malloc(sizeof(Table));
																		strcpy(NewFam->FName,yytext);
																		NewFam->NVersions = 1;
																		NewFam->next = NewTab->T;
																		NewTab->T = NewFam;
																		
																	}
																	else 
																	{
																		printf("ERROR : Deux familles prennent le même nom dans cette table --> '%s'\n", yytext);
																		error = 1;
																	}
																}
																token=_lire_token();
																if(token == APOSTROPHE)
																{
																	token=_lire_token();
																}
																else return false;
															}
															else return false;
														}
														else return false;
													}
													else return false;
												}
												else return false;
											}
										}
										else return false;
									}
									else return false;
								}
								else return false;
							}
						}
						else return false;
					}
					else return false;
				}
				else return false;
			}
			else return false;
		}
		else return false;
	}
    else return false;
}
else return false;
if(error==0 && Texist ==0){

	NewTab->next=DB;
  DB=NewTab;

  printf("Commande réussite\n");
  }              
}

boolean _scan()
{ 
  int exist = 0,t=0;
  
DataBase* p;
p = (DataBase*)malloc(sizeof(DataBase));


    token=_lire_token();

    if(token == SPACE)
    {

      token=_lire_token();
      if(token == APOSTROPHE)
      {

        token=_lire_token();
	if(token == IDF)
	{
p=DB ;
         while(p != NULL && exist != 1) {

	     if(strcmp(yytext,p->TName)==0){

               exist = 1;
		}
else p=p->next ;

}

         
	  token=_lire_token();
	  if(token == APOSTROPHE)
	  {
	    token=_lire_token();
	   if(token == EOL)
{
if(exist == 1)
{ 


Table* newfam ;
newfam=(Table*)malloc(sizeof(Table));
newfam=p->T ;
printf ("Table : %s\n",p->TName);

while (newfam!=NULL)
{

printf("Famille : %s\t Version: %d\n",newfam->FName, newfam->NVersions);
puts("");
Family* newcol ;
newcol=(Family*)malloc(sizeof(Family));
newcol = newfam->F ;
while (newcol!=NULL)
{

printf("Colonne : %s\n",newcol->CName);
puts("");
printf("-------------------------------------------------------------------------\n");
printf("RowKey\t\tTimeStamp\t\tCellValue\t\n");
printf("-------------------------------------------------------------------------\n");
Column* entry ;
entry=(Column*)malloc(sizeof(Column));
entry = newcol->C ;
while(entry != NULL)
{
printf("%s\t\t%ld\t\t%s\t\n", entry->Rowkey, entry->Timestamp, entry->CellValue) ;
entry=entry->next ;
}

printf("-------------------------------------------------------------------------\n");
 newcol=newcol->next;

}
newfam=newfam->next;
}}
if (exist ==0) { printf ("cette table n'existe pas");
}
return true;
}
else return false;
            
	  }
          else
            return false;
        }
        else
          return false;
	}
        else
          return false;
      }
      else
        return false;
    
 
if(exist==0){
  printf("Commande réussite\n");
  }
else
printf("cette table n'existe pas\n");	              
}


boolean _delete()
{

boolean result;
int existt=0;
int existc=0;
int existr=0;
int existf=0;
int existts=0;
long ts=0 ;
int first=0 ;
char tab[50] ;
char col[50] ;
char row[50] ;
char fam[50] ;
DataBase* p;
p = (DataBase*)malloc(sizeof(DataBase));
Table* newfam ;
newfam=(Table*)malloc(sizeof(Table));
Family* newcol ;
newcol=(Family*)malloc(sizeof(Family));
Column* entry ;
entry=(Column*)malloc(sizeof(Column));
Column* entry2 ;
entry2=(Column*)malloc(sizeof(Column));
	
		token=_lire_token();
	    if(token==SPACE)
	    {
			token=_lire_token();
	        if(token==APOSTROPHE)
			{
				token=_lire_token();
				if(token==IDF) 
				{
					p=DB;
					strcpy(tab,yytext) ;
					while(p != NULL && existt != 1)
					{
						if(strcmp(tab,p->TName)==0)
						{
							existt = 1;
						}
						else p=p->next ;
					}
					token=_lire_token();
				}
				if(token==APOSTROPHE)
				{
					token=_lire_token();
					if(token==COMMA)
					{
						token=_lire_token();
						if(token==SPACE)
						{
							token=_lire_token();
							if(token==APOSTROPHE)
							{
								token=_lire_token();
								if(token==IDF)
								{
									strcpy(row,yytext) ;
									token=_lire_token();
									if(token==APOSTROPHE)
									{
										token=_lire_token();
										if(token==COMMA)
										{
											token=_lire_token();
											if(token==SPACE)
											{
												token=_lire_token();
												if(token==APOSTROPHE)
												{
													token=_lire_token();
													if(token==IDF)
													{
														strcpy(fam,yytext) ;
														token=_lire_token();
														if(token==DBL_PT)
														{
															token=_lire_token();							
															if(token==IDF)
															{
																newfam=p->T;
																strcpy(col,yytext);
																while (newfam != NULL && existf != 1 && existt==1)
																{
																	if(strcmp(fam,newfam->FName)==0)
																	{
																		existf = 1;
																		newcol=newfam->F ;
																		while(newcol != NULL && existc != 1 && existt==1 && existf==1)
																		{
																			if(strcmp(col,newcol->CName)==0)
																			{
																				existc = 1;
																				entry=newcol->C;
																				entry2=newcol->C->next;
																			}
																			else  newcol=newcol->next;
																		}
																	}
																	else newfam=newfam->next;
																}
																token=_lire_token();						           
																if(token==APOSTROPHE)
																{
																	token=_lire_token();
																	if(token==COMMA)
																	{
																		token=_lire_token();
																		if(token==SPACE)
																		{
																			token=_lire_token();
																			if(token==INUMBER)
																			{
																				ts=strtoll(yytext, NULL, 0);
																				token=_lire_token();
																				if(token==EOL)
																				{
																				if(ts==entry->Timestamp && strcmp(entry->Rowkey,row)==0){existr=1;
																					newcol->C=newcol->C->next;}
																					else{
																					
																					while(entry2!=NULL && existr != 1)
																					{
																						if(ts==entry2->Timestamp && strcmp(entry2->Rowkey,row)==0)
																						{
																							existr = 1;
																							entry->next=entry2->next ;
																						}
																						else  {entry=entry->next;entry2=entry2->next;}
																					}}
																					}
																					if (existt ==0) 
																					{
																						printf ("La table %s n'existe pas\n",tab);
																					}
																					else
																					{
																						if (existf ==0) 
																						{
																							printf ("La famille %s n'existe pas\n",fam);
																						}
																						else
																						{
																							if (existc ==0) 
																							{
																								printf ("La colonne %s n'existe pas\n",col);
																							}
																							else
																							{
																								if (existr ==0) 
																								{
																									int A=0;
																								}
																							}
																						}
																					}
																					result=true;
																				}
																				else result=false;
																			}
																			else result=false;
																		}
																		else result=false;
																	}
																	else result=false;
																}
																else result=false;
															}
															else result=false;
														}
														else result=false;
													}
													else result=false;
												}
												else result=false;
											}
											else result=false;
										}
										else result=false;
									}
									else result=false;
								}
								else result=false;
							}
							else result=false;
						}
						else result=false;
					}
					else result=false;
				}
				else result=false;
			}
			else result=false;
	return result;
}

//fonction describe :
//describe 't1'
boolean _describe()
{ 
  int exist = 0,t=0;
  
DataBase* p;
p = (DataBase*)malloc(sizeof(DataBase));


    token=_lire_token();

    if(token == SPACE)
    {

      token=_lire_token();
      if(token == APOSTROPHE)
      {

        token=_lire_token();
	if(token == IDF)
	{
p=DB ;
         while(p != NULL && exist != 1) {

	     if(strcmp(yytext,p->TName)==0){

               exist = 1;
		}
else p=p->next ;

}

         
	  token=_lire_token();
	  if(token == APOSTROPHE)
	  {
	    token=_lire_token();
	   if(token == EOL)
{
if(exist == 1)
{ 


Table* newfam ;
newfam=(Table*)malloc(sizeof(Table));
newfam=p->T ;
printf ("Table Name => %s\n",p->TName);

while (newfam!=NULL)
{

printf("{Family Name =>%s\n",newfam->FName);
printf("Versions => %d\n",newfam->NVersions);
puts("");
Family* newcol ;
newcol=(Family*)malloc(sizeof(Family));
newcol = newfam->F ;
printf("}\n ");
newfam=newfam->next;
}}
if (exist ==0) { printf ("cette table n'existe pas");
}
return true;
}
else return false;
            
	  }
          else
            return false;
        }
        else
          return false;
	}
        else
          return false;
      }
      else
        return false;
    
 
if(exist==0){
  printf("Commande réussite\n");
  }
else
printf("cette table n'existe pas\n");	              
}

boolean _version()
{
  if(token == VERSION)
  {
    token=_lire_token();
    if(token == EOL)
    {
      printf("Interpréteur basé sur HBase 0.94.1, développé par:\n - MACHKOUR Moncef.\n - MACHACHATE Khalid. \n - MAIOUAK Mariem. \n - MESSOUDI Soundouss.\n\n Projet de compilation encadré par: \n - M. BAÏNA Karim.  \n");
      return true;
    }
    else
      return false;
  }
  else
    return false;
}

/* Fonction ALTER */
//alter 't1', name => 'f1', versions =>5
boolean _alter()
{boolean result;
DataBase* p;
char nom[50] ;
char fam[50] ;
char fam2[50] ;
char fam3[50] ;
int nversion=1 ;
int existf = 0;
int exist = 0;
int first=0;
Table* newfam ;
newfam=(Table*)malloc(sizeof(Table));
Table* varfam ;
varfam=(Table*)malloc(sizeof(Table));
Table* varfam2 ;
varfam2=(Table*)malloc(sizeof(Table));
Table* newfam2 ;
newfam2=(Table*)malloc(sizeof(Table));
p = (DataBase*)malloc(sizeof(DataBase));
p=DB ;
	if(token==ALTER)
	{token=_lire_token();
	   if(token==SPACE)
	    {token=_lire_token();
	      if(token==APOSTROPHE)
	      {token=_lire_token();
		if(token==IDF)
{strcpy(nom,yytext) ;
while(p != NULL && exist != 1) {

	     if(strcmp(nom,p->TName)==0){

               exist = 1;
		
		
		}
else p=p->next ;}
        
		token=_lire_token();
		  if(token==APOSTROPHE)
		  {token=_lire_token();
			if(token==COMMA)
			{token=_lire_token();
		           if(token==SPACE)
			   {token=_lire_token();
			        if(token==NAME)
				{token=_lire_token();
				    if(token==SPACE)
				    {token=_lire_token();
					if(token==ARROW)
					{token=_lire_token();
					    if(token==SPACE)
					    {token=_lire_token();
						if(token==APOSTROPHE)
						{token=_lire_token();
						   if(token==IDF)
					{strcpy(fam,yytext) ;
newfam=(Table*)malloc(sizeof(Table));
newfam2=(Table*)malloc(sizeof(Table));
newfam =p ->T ;
newfam2=p ->T->next;
while(newfam != NULL && existf != 1) {

	     if(strcmp(fam,newfam->FName)==0){

               existf = 1;
		
		}
else {newfam=newfam->next ;}}
 					
						   token=_lire_token();
							if(token==APOSTROPHE)
							{token=_lire_token();
						           if(token==COMMA)
							   {token=_lire_token();
								if(token==SPACE)
								{token=_lire_token();
							            if(token==VERSIONS)
								    {token=_lire_token();
									if(token==SPACE)
									{token=_lire_token();
									     if(token==ARROW)
									     {token=_lire_token();
										if(token==INUMBER)
							{nversion=atoi(yytext) ;
										token=_lire_token();
										   if(token==EOL)

{ if(exist==1){
if (existf==1)
{newfam->NVersions=nversion;}
if (existf==0)
{ 
varfam->NVersions = nversion;
strcpy(varfam->FName,fam) ;
varfam->next=(Table*)malloc(sizeof(Table));
varfam->next = p->T ;
p->T=varfam;
}
}

if (exist ==0) { printf ("cette table n'existe pas");}


										   result=true;}
										   else result=false;
										}else result=false;
									     }else result=false;
									}else result=false;
								    }else result=false;
								}else result=false;
							   }else result=false;
							}else result=false;
						   }else result=false;
						}else result=false;
					    }else result=false;
					}else result=false;
				    }else result=false;
				}else result=false;
			    }else
			    { //alter 't1','delete' => 'f1'
				if(token==APOSTROPHE)
				{token=_lire_token();
				   if(token==DELETE)
				   {token=_lire_token();
					if(token==APOSTROPHE)
					{token=_lire_token();
				            if(token==SPACE)
					    {token=_lire_token();
						if(token==ARROW)
						{token=_lire_token();
						     if(token==SPACE)
						     {token=_lire_token();
							if(token==APOSTROPHE)
							{token=_lire_token();
							   if(token==IDF)
							   {strcpy(fam2,yytext) ;

newfam=(Table*)malloc(sizeof(Table));
newfam2=(Table*)malloc(sizeof(Table));
newfam =p ->T ;
newfam2=p ->T->next;
if (strcmp(fam2,newfam->FName)==0)
{
existf=1;
first=1;
}
while(newfam2 != NULL && existf != 1) {
	     if(strcmp(fam,newfam2->FName)==0){
               existf = 1;
		}
else {newfam=newfam->next ;newfam2=newfam2->next ;}}
token=_lire_token();
								if(token==APOSTROPHE)
								{token=_lire_token();
							             if(token==EOL)
								    { if(exist == 1 && existf == 1)

{if (first==1){p->T=p->T->next;}else if (first==0) {newfam->next=newfam2->next;}}

if (exist ==0) { printf ("cette table n'existe pas");}
//if (exist ==1 && existf ==0) { printf ("cette table existe mais cette famille n'existe pas dans cette table");}
return true;}
								     else result=false;
								}else result=false;
							    }else result=false;
							}else result=false;
						    }else result=false;
						}else result=false;
					    }else result=false;
					}else result=false;
				    }else//alter 't1','f1'
{
if(token==IDF)
	{strcpy(fam3,yytext) ;
token=_lire_token();
		if(token==APOSTROPHE)
		     {token=_lire_token();
			if(token==EOL)
				{
 if(exist==1){
if (existf==1)
//{newfam->NVersions=nversion;}
if (existf==0)
{ 
varfam2->NVersions = nversion;
strcpy(varfam2->FName,fam3) ;
varfam2->next=(Table*)malloc(sizeof(Table));
varfam2->next = p->T ;
p->T=varfam2;
}
}

if (exist ==0) { printf ("cette table n'existe pas");}


result=true;
}else result=false;
				}else result=false;
				}else result=false;

				};
}else result=false;
			    };
			}else result=false;
		    }else result=false;
		}else result=false;
	    }else result=false;
	}else result=false;
    }else result=false;
return result;
}

boolean _get(){
	DataBase *Tab;
	Table *Fam;
	Family *Col;
	Column *Rec;
	Tab = (DataBase*)malloc(sizeof(DataBase));
	Fam = (Table*)malloc(sizeof(Table));
	Col = (Family*)malloc(sizeof(Family));
	Rec = (Column*)malloc(sizeof(Column));
	char_list1 *list1, *vlist1;
	list1 = (char_list1*)malloc(sizeof(char_list1));
	vlist1 = (char_list1*)malloc(sizeof(char_list1));
	int cl=0, tr=0, ts=0, vr=0;
	int cl1=0, clv=0;
	char RKey[50];
	int Texist=0, Fexist=0, Cexist=0;
	long tr1, tr2, ts1;
	if(token == GET)
	{
		token=_lire_token();
		if(token == SPACE)
		{
			token=_lire_token();
			if(token == APOSTROPHE)
			{
				token=_lire_token();
				if(token == IDF)
				{
					Tab=DB;
					while(Tab != NULL && Texist != 1) {

						if(strcmp(yytext,Tab->TName)==0) {
							Texist = 1;}
						else
							Tab=Tab->next;
					}
					if(Texist==0)
						printf("La table %s n'existe pas\n",yytext);
					token=_lire_token();
					if(token == APOSTROPHE)
					{
						token=_lire_token();
						if(token == COMMA)
						{
							token=_lire_token();
							if(token == SPACE)
							{
								token=_lire_token();
								if(token == APOSTROPHE)
								{
									token=_lire_token();
									if(token == IDF)
									{
										strcpy(RKey,yytext);
										token=_lire_token();
										if(token == APOSTROPHE)
										{
											token=_lire_token();
											if(token == COMMA)
											{
												token=_lire_token();
												if(token == SPACE)
												{
													token=_lire_token();
													if(token == OBRACES)
													{
														token=_lire_token();
														switch(token)
														{
															case COLUMN : { cl = 1;
																token=_lire_token();
																if(token == SPACE)
																{
																	token=_lire_token();
																	if(token == ARROW)
																	{
																		token=_lire_token();
																		if(token == SPACE)
																		{
																			token=_lire_token();
																			if(token == OBRACKET)
																			{
																				token=_lire_token();
																				if(token == APOSTROPHE)
																				{
																					token=_lire_token();
																					if(token == IDF)
																					{
																						strcpy(vlist1->n1,yytext);
																						token=_lire_token();
																						if(token == DBL_PT)
																						{
																							token=_lire_token();
																							if(token == IDF)
																							{
																								strcpy(vlist1->n2,yytext);
																								vlist1->next=NULL;
																								list1=vlist1;
																								token=_lire_token();
																								if(token == APOSTROPHE)
																								{
																									token=_lire_token();
																									while(token != CBRACKET)
																									{
																										if(token == COMMA)
																										{
																											token=_lire_token();
																											if(token == SPACE)
																											{
																												token=_lire_token();
																												if(token == APOSTROPHE)
																												{
																													token=_lire_token();
																													if(token == IDF)
																													{
																														vlist1 = (char_list1*)malloc(sizeof(char_list1));
																														strcpy(vlist1->n1,yytext);
																														token=_lire_token();
																														if(token == DBL_PT)
																														{
																															token=_lire_token();
																															if(token == IDF)
																															{
																																strcpy(vlist1->n2,yytext);
																																vlist1->next=list1;
																																list1=vlist1;
																																token=_lire_token();
																																if(token == APOSTROPHE)
																																{
																																	token=_lire_token();
																																}
																																else return false;
																															}
																															else return false;
																														}
																														else return false;
																													}
																													else return false;
																												}
																												else return false;
																											}
																											else return false;
																										}
																										else return false;
																									}
																									token=_lire_token();
																								}
																								else return false;
																							}
																							else return false;
																						}
																						else return false;
																					}
																					else return false;
																				}
																				else return false;
																			}
																			else
																			{
																				if(token == APOSTROPHE)
																				{
																					token=_lire_token();
																					if(token == IDF)
																					{
																						strcpy(vlist1->n1,yytext);
																						token=_lire_token();
																						if(token == DBL_PT)
																						{
																							token=_lire_token();
																							if(token == IDF)
																							{
																								strcpy(vlist1->n2,yytext);
																								vlist1->next=NULL;
																								list1=vlist1;
																								token=_lire_token();
																								if(token == APOSTROPHE)
																								{
																									token=_lire_token();
																								}
																								else return false;
																							}
																							else return false;
																						}
																						else return false;
																					}
																					else return false;
																				}
																				else return false;
																			}

																		}
																		else return false;
																	}
																	else return false;
																}
																else return false;
																break;
															}
															case TIMESTAMP : { ts = 1;
																token=_lire_token();
																if(token == SPACE)
																{
																	token=_lire_token();
																	if(token == ARROW)
																	{
																		token=_lire_token();
																		if(token == SPACE)
																		{
																			token=_lire_token();
																			if(token == INUMBER)
																			{
																				ts1=strtoll(yytext, NULL, 0);
																				token=_lire_token();
																			}
																			else return false;
																		}
																		else return false;
																	}
																	else return false;
																}
																else return false;
																break;
															}
															case VERSIONS : { vr = 1;
																token=_lire_token();
																if(token == SPACE)
																{
																	token=_lire_token();
																	if(token == ARROW)
																	{
																		token=_lire_token();
																		if(token == SPACE)
																		{
																			token=_lire_token();
																			if(token == INUMBER)
																			{
																				token=_lire_token();
																			}
																			else return false;
																		}
																		else return false;
																	}
																	else return false;
																}
																else return false;
																break;
															}
															case TIMERANGE : { tr = 1;
																token=_lire_token();
																if(token == SPACE)
																{
																	token=_lire_token();
																	if(token == ARROW)
																	{
																		token=_lire_token();
																		if(token == SPACE)
																		{
																			token=_lire_token();
																			if(token == OBRACKET)
																			{
																				token=_lire_token();
																				if(token == INUMBER)
																				{
																					tr1=strtoll(yytext, NULL, 0);
																					token=_lire_token();
																					if(token == COMMA)
																					{
																						token=_lire_token();
																						if(token == SPACE)
																						{
																							token=_lire_token();
																							if(token == INUMBER)
																							{
																								tr2=strtoll(yytext, NULL, 0);
																								token=_lire_token();
																								if(token == CBRACKET)
																								{
																									token=_lire_token();
																								}
																								else return false;
																							}
																							else return false;
																						}
																						else return false;
																					}
																					else return false;
																				}
																				else return false;
																			}
																			else return false;
																		}
																		else return false;
																	}
																	else return false;
																}
																else return false;
																break;
															}
															default : return false ; 
														}
														while(token != CBRACES)
														{
															if(token == COMMA)
															{
																token=_lire_token();
																if(token == SPACE)
																{
																	token=_lire_token();
																	switch(token)
																	{
																		case COLUMN : { if(cl == 1) return false;
																			cl=1;
																			token=_lire_token();
																			if(token == SPACE)
																			{
																				token=_lire_token();
																				if(token == ARROW)
																				{
																					token=_lire_token();
																					if(token == SPACE)
																					{
																						token=_lire_token();
																						if(token == OBRACKET)
																						{
																							token=_lire_token();
																							if(token == APOSTROPHE)
																							{
																								token=_lire_token();
																								if(token == IDF)
																								{
																									strcpy(vlist1->n1,yytext);
																									token=_lire_token();
																									if(token == DBL_PT)
																									{
																										token=_lire_token();
																										if(token == IDF)
																										{
																											strcpy(vlist1->n2,yytext);
																											vlist1->next=NULL;
																											list1=vlist1;
																											token=_lire_token();
																											if(token == APOSTROPHE)
																											{
																												token=_lire_token();
																												while(token != CBRACKET)
																												{
																													if(token == COMMA)
																													{
																														token=_lire_token();
																														if(token == SPACE)
																														{
																															token=_lire_token();
																															if(token == APOSTROPHE)
																															{
																																token=_lire_token();
																																if(token == IDF)
																																{
																																	vlist1 = (char_list1*)malloc(sizeof(char_list1));
																																	strcpy(vlist1->n1,yytext);
																																	token=_lire_token();
																																	if(token == DBL_PT)
																																	{
																																		token=_lire_token();
																																		if(token == IDF)
																																		{
																																			strcpy(vlist1->n2,yytext);
																																			vlist1->next=list1;
																																			list1=vlist1;
																																			token=_lire_token();
																																			if(token == APOSTROPHE)
																																			{
																																				token=_lire_token();
																																			}
																																			else return false;
																																		}
																																		else return false;
																																	}
																																	else return false;
																																}
																																else return false;
																															}
																															else return false;
																														}
																														else return false;
																													}
																													else return false;
																												}
																												token=_lire_token();
																											}
																											else return false;
																										}
																										else return false;
																									}
																									else return false;
																								}
																								else return false;
																							}
																							else return false;
																						}
																						else
																						{
																							if(token == APOSTROPHE)
																							{
																								token=_lire_token();
																								if(token == IDF)
																								{
																									strcpy(vlist1->n1,yytext);
																									token=_lire_token();
																									if(token == DBL_PT)
																									{
																										token=_lire_token();
																										if(token == IDF)
																										{
																											strcpy(vlist1->n2,yytext);
																											vlist1->next=NULL;
																											list1=vlist1;
																											token=_lire_token();
																											if(token == APOSTROPHE)
																											{
																												token=_lire_token();
																											}
																											else return false;
																										}
																										else return false;
																									}
																									else return false;
																								}
																								else return false;
																							}
																							else return false;
																						}
																						
																					}
																					else return false;
																				}
																				else return false;
																			}
																			else return false;
																			break;
																		}
																		case TIMESTAMP : { if(ts == 1 || tr ==1) return false;
																			ts = 1;
																			token=_lire_token();
																			if(token == SPACE)
																			{
																				token=_lire_token();
																				if(token == ARROW)
																				{
																					token=_lire_token();
																					if(token == SPACE)
																					{
																						token=_lire_token();
																						if(token == INUMBER)
																						{
																							ts1=strtoll(yytext, NULL, 0);
																							token=_lire_token();
																						}
																						else return false;
																					}
																					else return false;
																				}
																				else return false;
																			}
																			else return false;
																			break;
																		}
																		case VERSIONS : { if(vr == 1) return false;
																			vr=1;
																			token=_lire_token();
																			if(token == SPACE)
																			{
																				token=_lire_token();
																				if(token == ARROW)
																				{
																					token=_lire_token();
																					if(token == SPACE)
																					{
																						token=_lire_token();
																						if(token == INUMBER)
																						{
																							token=_lire_token();
																						}
																						else return false;
																					}
																					else return false;
																				}
																				else return false;
																			}
																			else return false;
																			break;
																		}
																		case TIMERANGE : { if(tr == 1 || ts == 1) return false;
																			tr=1;
																			token=_lire_token();
																			if(token == SPACE)
																			{
																				token=_lire_token();
																				if(token == ARROW)
																				{
																					token=_lire_token();
																					if(token == SPACE)
																					{
																						token=_lire_token();
																						if(token == OBRACKET)
																						{
																							token=_lire_token();
																							if(token == INUMBER)
																							{
																								tr1=strtoll(yytext, NULL, 0);
																								token=_lire_token();
																								if(token == COMMA)
																								{
																									token=_lire_token();
																									if(token == SPACE)
																									{
																										token=_lire_token();
																										if(token == INUMBER)
																										{
																											tr2=strtoll(yytext, NULL, 0);
																											token=_lire_token();
																											if(token == CBRACKET)
																											{
																												token=_lire_token();
																											}
																											else return false;
																										}
																										else return false;
																									}
																									else return false;
																								}
																								else return false;
																							}
																							else return false;
																						}
																						else return false;
																					}
																					else return false;
																				}
																				else return false;
																			}
																			else return false;
																			break;
																		}
																		default : return false ; 
																	}
																}
																else  return false;
															}
															else  return false;
														}
														token=_lire_token();
														if(token == EOL)
														{
															if(cl == 1)
															{
																vlist1 = (char_list1*)malloc(sizeof(char_list1));
																vlist1=list1;
																if(Texist == 1)
																{
																	while(vlist1!=NULL)
																	{
																		Fexist=0;
																		Cexist=0;
																		Fam=Tab->T;
																		while(Fam != NULL) 
																		{
																			if(strcmp(vlist1->n1,Fam->FName)==0)
																			{
																				Fexist=1;
																				Col=Fam->F;
																				while(Col != NULL)
																				{
																					if(strcmp(vlist1->n2,Col->CName)==0)
																					{
																						Cexist=1;
																						printf("\n=================================================================\n");
																						printf("Famille : %s\tColonne : %s\n",vlist1->n1,vlist1->n2);
																						printf("=================================================================\n");
																						printf("\n------------------------------------------------------\n");
																						printf("CellValue\tTimesStamp\n");
																						printf("------------------------------------------------------\n");
																						Rec=Col->C;
																						if(ts==1)
																						{
																							while(Rec!=NULL)
																							{
																								if(strcmp(RKey,Rec->Rowkey)==0 && Rec->Timestamp==ts1) 
																								{
																									printf("%s\t%ld\n",Rec->CellValue,Rec->Timestamp);
																								}
																								Rec=Rec->next;
																							}
																						}
																						else
																						{
																							if(tr==1)
																							{
																								while(Rec!=NULL)
																								{
																									if(strcmp(RKey,Rec->Rowkey)==0 && Rec->Timestamp<=tr2 && Rec->Timestamp>=tr1) 
																									{
																										printf("%s\t%ld\n",Rec->CellValue,Rec->Timestamp);
																									}
																									Rec=Rec->next;
																								}
																							}
																							else
																							{
																								while(Rec!=NULL)
																								{
																									if(strcmp(RKey,Rec->Rowkey)==0) 
																									{
																										printf("%s\t%ld\n",Rec->CellValue,Rec->Timestamp);
																									}
																									Rec=Rec->next;
																								}
																							}
																						}
																						printf("------------------------------------------------------\n\n");
																					}
																					Col=Col->next;
																				}
																			}
																			Fam=Fam->next;
																		}
																		if(Fexist == 0)
																			printf("La famille %s n'existe pas\n", vlist1->n1);
																		else
																			if(Cexist == 0)
																				printf("La colonne %s n'existe pas dans la famille %s\n",vlist1->n2, vlist1->n1);
																		vlist1=vlist1->next;
																	}
																}
															}
															else
															{
																vlist1 = (char_list1*)malloc(sizeof(char_list1));
																vlist1=list1;
																if(Texist == 1)
																{
																	while(vlist1!=NULL)
																	{
																		Fexist=0;
																		Cexist=0;
																		Fam=Tab->T;
																		while(Fam != NULL) 
																		{
																			if(strcmp(vlist1->n1,Fam->FName)==0)
																			{
																				Fexist=1;
																				Col=Fam->F;
																				while(Col != NULL)
																				{
																					if(strcmp(vlist1->n2,Col->CName)==0)
																					{
																						Cexist=1;
																						printf("\n=================================================================\n");
																						printf("Famille : %s\tColonne : %s\n",vlist1->n1,vlist1->n2);
																						printf("=================================================================\n");
																						printf("\n------------------------------------------------------\n");
																						printf("CellValue\tTimesStamp\n");
																						printf("------------------------------------------------------\n");
																						Rec=Col->C;
																						if(ts==1)
																						{
																							while(Rec!=NULL)
																							{
																								if(Rec->Timestamp==ts1) 
																								{
																									printf("%s\t%ld\n",Rec->CellValue,Rec->Timestamp);
																								}
																								Rec=Rec->next;
																							}
																						}
																						else
																						{
																							if(tr==1)
																							{
																								while(Rec!=NULL)
																								{
																									if(Rec->Timestamp<=tr2 && Rec->Timestamp>=tr1) 
																									{
																										printf("%s\t%ld\n",Rec->CellValue,Rec->Timestamp);
																									}
																									Rec=Rec->next;
																								}
																							}
																						}
																						printf("------------------------------------------------------\n\n");
																					}
																					Col=Col->next;
																				}
																			}
																			Fam=Fam->next;
																		}
																		if(Fexist == 0)
																			printf("La famille %s n'existe pas\n", vlist1->n1);
																		else
																			if(Cexist == 0)
																				printf("La colonne %s n'existe pas dans la famille %s\n",vlist1->n2, vlist1->n1);
																		vlist1=vlist1->next;
																	}
																}
															}
															return true;
														}
														else return false;
													}
													else
													{
														if(token == OBRACKET)
														{
															token=_lire_token();
															if(token == APOSTROPHE)
															{
																token=_lire_token();
																if(token == IDF)
																{
																	strcpy(vlist1->n1,yytext);
																	token=_lire_token();
																	if(token == DBL_PT)
																	{
																		token=_lire_token();
																		if(token == IDF)
																		{
																			strcpy(vlist1->n2,yytext);
																			vlist1->next=NULL;
																			list1=vlist1;
																			token=_lire_token();
																			if(token == APOSTROPHE)
																			{
																				token=_lire_token();
																				while(token != CBRACKET)
																				{
																					if(token == COMMA)
																					{
																						token=_lire_token();
																						if(token == SPACE)
																						{
																							token=_lire_token();
																							if(token == APOSTROPHE)
																							{
																								token=_lire_token();
																								if(token == IDF)
																								{
																									vlist1 = (char_list1*)malloc(sizeof(char_list1));
																									strcpy(vlist1->n1,yytext);
																									token=_lire_token();
																									if(token == DBL_PT)
																									{
																										token=_lire_token();
																										if(token == IDF)
																										{
																											strcpy(vlist1->n2,yytext);
																											vlist1->next=list1;
																											list1=vlist1;
																											token=_lire_token();
																											if(token == APOSTROPHE)
																											{
																												token=_lire_token();
																											}
																											else return false;
																										}
																										else return false;
																									}
																									else return false;
																								}
																								else return false;
																							}
																							else return false;
																						}
																						else return false;
																					}
																					else return false;
																				}
																				token=_lire_token();
																				if(token == EOL)
																				{
																					vlist1 = (char_list1*)malloc(sizeof(char_list1));
																					vlist1=list1;
																					if(Texist == 1)
																					{
																						while(vlist1!=NULL)
																						{
																							Fexist=0;
																							Cexist=0;
																							Fam=Tab->T;
																							while(Fam != NULL) 
																							{
																								if(strcmp(vlist1->n1,Fam->FName)==0)
																								{
																									Fexist=1;
																									Col=Fam->F;
																									while(Col != NULL)
																									{
																										if(strcmp(vlist1->n2,Col->CName)==0)
																										{
																											Cexist=0;
																											printf("\n=================================================================\n");
																											printf("Famille : %s\tColonne : %s\n",vlist1->n1,vlist1->n2);
																											printf("=================================================================\n");
																											printf("\n------------------------------------------------------\n");
																											printf("CellValue\tTimesStamp\n");
																											printf("------------------------------------------------------\n");
																											Rec=Col->C;
																											while(Rec!=NULL)
																											{
																												if(strcmp(RKey,Rec->Rowkey)==0) 
																												{
																													printf("%s\t%ld\n",Rec->CellValue,Rec->Timestamp);
																												}
																												Rec=Rec->next;
																											}
																											printf("------------------------------------------------------\n\n");
																										}
																										Col=Col->next;
																									}
																								}
																								Fam=Fam->next;
																							}
																							if(Fexist == 0)
																								printf("La famille %s n'existe pas\n", vlist1->n1);
																							else
																								if(Cexist == 0)
																									printf("La colonne %s n'existe pas dans la famille %s\n",vlist1->n2, vlist1->n1);
																							vlist1=vlist1->next;
																						}
																					}
																					return true; 
																				}
																			}
																			else return false;
																		}
																		else return false;
																	}
																	else return false;
																}
																else return false;
															}
															else return false;
														}
														else
														{
															if(token == APOSTROPHE)
															{
																token=_lire_token();
																if(token == IDF)
																{
																	strcpy(vlist1->n1,yytext);
																	token=_lire_token();
																	if(token == DBL_PT)
																	{
																		token=_lire_token();
																		if(token == IDF)
																		{
																			strcpy(vlist1->n2,yytext);
																			vlist1->next=NULL;
																			list1=vlist1;
																			token=_lire_token();
																			if(token == APOSTROPHE)
																			{
																				token=_lire_token();
																				while(token != EOL)
																				{
																					if(token == COMMA)
																					{
																						token=_lire_token();
																						if(token == SPACE)
																						{
																							token=_lire_token();
																							if(token == APOSTROPHE)
																							{
																								token=_lire_token();
																								if(token == IDF)
																								{
																									vlist1 = (char_list1*)malloc(sizeof(char_list1));
																									strcpy(vlist1->n1,yytext);
																									token=_lire_token();
																									if(token == DBL_PT)
																									{
																										token=_lire_token();
																										if(token == IDF)
																										{
																											strcpy(vlist1->n2,yytext);
																											vlist1->next=list1;
																											list1=vlist1;
																											token=_lire_token();
																											if(token == APOSTROPHE)
																											{
																												token=_lire_token();
																											}
																											else return false;
																										}
																										else return false;
																									}
																									else return false;
																								}
																								else return false;
																							}
																							else return false;
																						}
																						else return false;
																					}
																					else return false;
																				}
																				vlist1 = (char_list1*)malloc(sizeof(char_list1));
																				vlist1=list1;
																				if(Texist == 1)
																				{
																					while(vlist1!=NULL)
																					{
																						Fexist=0;
																						Cexist=0;
																						Fam=Tab->T;
																						while(Fam != NULL) 
																						{
																							if(strcmp(vlist1->n1,Fam->FName)==0)
																							{
																								Fexist=1;
																								Col=Fam->F;
																								while(Col != NULL)
																								{
																									if(strcmp(vlist1->n2,Col->CName)==0)
																									{
																										Cexist=1;
																										printf("\n=================================================================\n");
																										printf("Famille : %s\tColonne : %s\n",vlist1->n1,vlist1->n2);
																										printf("=================================================================\n");
																										printf("\n------------------------------------------------------\n");
																										printf("CellValue\tTimesStamp\n");
																										printf("------------------------------------------------------\n");
																										Rec=Col->C;
																										while(Rec!=NULL)
																										{
																											if(strcmp(RKey,Rec->Rowkey)==0) 
																											{
																												printf("%s\t%ld\n",Rec->CellValue,Rec->Timestamp);
																											}
																											Rec=Rec->next;
																										}
																										printf("------------------------------------------------------\n\n");
																									}
																									Col=Col->next;
																								}
																							}
																							Fam=Fam->next;
																						}
																						if(Fexist == 0)
																							printf("La famille %s n'existe pas\n", vlist1->n1);
																						else
																							if(Cexist == 0)
																								printf("La colonne %s n'existe pas dans la famille %s\n",vlist1->n2, vlist1->n1);
																						vlist1=vlist1->next;
																					}
																				}
																				return true;
																			}
																			else return false;
																		}
																		else return false;
																	}
																	else return false;
																}
																else return false;
															}
															else return false;
														}
													}
												}
												else return false;
											}
											else
											{
												if(token == EOL)
												{
													if(Texist == 1)
													{
														Fam=Tab->T;
														while(Fam != NULL) 
														{
															Col=Fam->F;
															printf("================================================================\n|\t\t\t\t\tFamille : %s\t\t\t\t\t|\n================================================================\n",Fam->FName);
															while(Col != NULL)
															{
																Rec=Col->C;
																
																printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n|\t\t\t\t\tColonne : %s\t\t\t\t\t|\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n",Col->CName);
																printf("\n------------------------------------------------------\n");
																printf("CellValue\tTimesStamp\n");
																printf("------------------------------------------------------\n");
																while(Rec!=NULL)
																{
																	if(strcmp(RKey,Rec->Rowkey)==0) 
																	{
																		printf("%s\t%ld\n",Rec->CellValue,Rec->Timestamp);
																	}
																	Rec=Rec->next;
																}
																printf("------------------------------------------------------\n\n");
																Col=Col->next;
															}
															Fam=Fam->next;
														}
													}
													return true;
												}
												else
													return false;
											}
										}
										else return false;
									}
									else return false;
								}
								else return false;
							}
							else return false;
						}
						else return false;
					}
					else return false;
				}
				else return false;
			}
			else return false;
		}
		else return false;
	}
	else return false;                       
}

int main()
{

DB = (DataBase*)malloc(sizeof(DataBase));
do {
token=_lire_token();
switch(token)
{
	case CREATE : { if(!_create()) 
                            printf("Syntaxe incorrecte1\n");} break ;
	case TRUNCATE : { if(!_truncate()) 
                            printf("Syntaxe incorrecte2\n");} break ;
	case ENABLE : { if(!_enable()) 
                            printf("Syntaxe incorrecte3\n");} break ;
	case IS_ENABLED : { if(!_is_enabled()) 
                            printf("Syntaxe incorrecte4\n");} break ;
	case DISABLE : { if(!_disable()) 
                            printf("Syntaxe incorrecte5\n");} break ;
	case DROP : { if(!_drop()) 
                            printf("Syntaxe incorrecte6\n");} break ;
	case IS_DISABLED : { if(!_is_disabled()) 
                            printf("Syntaxe incorrecte7\n");} break ;
	case SCAN : {if(!_scan()) 
                            printf("Syntaxe incorrecte8\n");} break ;
	case LIST : {if(!_list()) 
                            printf("Syntaxe incorrecte9\n");} break ;
	case EXISTS : {if(!_exists()) 
                            printf("Syntaxe incorrecte10\n");} break ;
	case WHOAMI : {if(!_whoami()) 
                            printf("Syntaxe incorrecte11\n");} break ;
	case VERSION : {if(!_version()) 
                            printf("Syntaxe incorrecte12\n");} break ;
	case PUT : {if(!pu_t()) 
                            printf("Syntaxe incorrecte13\n");} break ;
	case DELETE : { if(_delete()) printf("Commande réussite\n");
                  else          printf("Syntaxe incorrecte14\n");} break ;
	case DELETEALL : { if(_delete_all()) printf("Commande réussite\n");
                  else          printf("Syntaxe incorrecte15\n");} break ;
	case DESCRIBE : {if(!_describe()) 
                            printf("Syntaxe incorrecte16\n");} break ;
	case ALTER : {if(!_alter()) printf("Syntaxe incorrecte17\n");
                            printf("Commande réussite\n");} break ;
	case GET : {if(!_get()) printf("Syntaxe incorrecte18\n");
                            printf("Commande réussite\n");} break ;
	case EXIT : _e(); break ;
	case HELP : { printf("- whoami : whoami\n\n- version :  version\n\n- list : list\n\n- describe : describe ‘t1'\n\n- exists : exists 't1'\n\n- create : create ‘t1', ‘f1', ‘f2', ‘f3'\n           create ‘t1', {NAME => ‘f1', VERSIONS => 5}\n           create ‘t1', {NAME => ‘f1'}, {NAME => ‘f2'}, {NAME => ‘f3'}\n\n- get :  get ‘t1', ‘r1'\n         get ‘t1', ‘r1', ‘c1'\n         get ‘t1', ‘r1', [‘c1', ‘c2']\n	 get ‘t1', ‘r1', {TIMERANGE => [ts1, ts2]}\n         get ‘t1', ‘r1', {COLUMN => ‘c1'}\n         get ‘t1', ‘r1', {COLUMN => [‘c1', ‘c2', ‘c3']}\n         get ‘t1', ‘r1', {COLUMN => ‘c1', TIMESTAMP => ts1}\n         get ‘t1', ‘r1', {COLUMN => ‘c1', TIMERANGE => [ts1, ts2], VERSIONS => 4}\n         get ‘t1', ‘r1', {COLUMN => ‘c1', TIMESTAMP => ts1, VERSIONS => 4}\n\n- put :  put 'table', 'row', 'column_family:column_qualifier', value\n	 put 'table', 'row', 'column_family:column_qualifier', value, timestamp\n\n- alter : alter ‘t1', NAME => ‘f1', VERSIONS => 5\n          alter ‘t1', ‘delete’ => ‘f1'\n	  alter 't1', 'f1'\n\n- scan : scan ‘t1'\n\n- delete : delete ‘t1', ‘r1', ‘c1', ts1\n\n- delete all : deleteall ‘t1', ‘r1', ‘c1'\n	       deleteall 't1', 'r1'\n               deleteall 't1', 'r1', 'c1', timestamp\n\n- drop : drop ‘t1'\n\n- enable :  enable ‘t1'\n\n- disable : disable ‘t1'\n\n- is enabled :  is_enabled ‘t1'\n\n- is disabled : is_disabled ‘t1'\n\n- truncate : truncate ‘t1'\n");
				} break;
	default : printf("Commande inconnue\n");
}
}
while(token != EXIT);

return 0;
}
