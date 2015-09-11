typedef enum { 
VERSION, WHOAMI, ALTER, CREATE, DESCRIBE, DISABLE, IS_DISABLED, DROP, ENABLE, IS_ENABLED, LIST, DELETE, DELETEALL, GET, PUT, SCAN, TRUNCATE, HELP, COMMA, APOSTROPHE, SEMICOLON, OBRACKET, CBRACKET, OBRACES, CBRACES, IDF, INUMBER, DNUMBER, CH, NAME, VERSIONS, COLUMN, TIMESTAMP, DBL_PT, TIMERANGE, ARROW, SPACE, EOL, EXIT, EXISTS
}typetoken;
typedef enum {false=0, true=1} boolean;

//La colonne (chaque élément de cette liste chainée représente un enregistrement)
typedef struct Column
{
char Rowkey[50];
char CellValue[50];
long Timestamp;
struct Column *next;
}Column;
//La famille (chaque élément de cette liste chainée représente une colonne)
typedef struct Family
{
char CName[50];
Column *C;
struct Family *next;
}Family;
//La table (chaque élément de cette liste chainée représente une famille)
typedef struct Table
{
char FName[50];
int NVersions;
Family *F;
struct Table *next;
}Table;
//La base de données (chaque élément de cette liste chainée représente une table)
typedef struct DataBase
{
char TName[50];
int Etat;
Table *T;
struct DataBase *next;
}DataBase;

/* */

boolean _version( );
boolean _whoami( );
/* */
boolean _alter( );
boolean _create( );
boolean _describe( );
boolean _disable( );

boolean _is_disabled( );
boolean _drop( );

boolean _enable( );

boolean _is_enabled( );

boolean _list( );
boolean _exists();


/* */

boolean _delete( );
boolean _deleteall( );
boolean _get( );


boolean pu_t( );
boolean _scan();
boolean _truncate( );
