%{
#include <stdio.h>
#include "HBASE.h"
%}
%option noyywrap
IDF			[a-zA-Z][a-zA-Z0-9]*
DNUMBER			[0-9]+"."[0-9]* 
INUMBER			[0-9]+
CH			[0-9]+[A-Za-z0-9]*

%%
"exit"			{return EXIT;}
"help"			{return HELP;}
"NAME"			{return NAME;}
"VERSIONS"		{return VERSIONS;}
"COLUMN"		{return COLUMN;}
"TIMESTAMP"		{return TIMESTAMP;}
"TIMERANGE"		{return TIMERANGE;}

"version"		{return VERSION;}
"whoami"		{return WHOAMI;}

"alter"			{return ALTER;}
"create"		{return CREATE;}
"describe"		{return DESCRIBE;}
"disable"		{return DISABLE;}
"is_disabled"		{return IS_DISABLED;}
"drop"			{return DROP;}
"enable"		{return ENABLE;}
"is_enabled"		{return IS_ENABLED;}
"list"			{return LIST;}
"exists"		{return EXISTS;}

"delete" 		{return DELETE;}
"deleteall"		{return DELETEALL;}
"get"			{return GET;}
"put"			{return PUT;}
"scan"			{return SCAN;}
"truncate"		{return TRUNCATE;}

"["			{return OBRACKET;}
"]"			{return CBRACKET;}
"{"			{return OBRACES;}
"}"			{return CBRACES;}
","			{return COMMA;}
"'"			{return APOSTROPHE;}
"=>"			{return ARROW;}
":"			{return DBL_PT;}

{IDF}			{return IDF;}
[0-9]+"."[0-9]*		{return DNUMBER;}
[0-9]+			{return INUMBER;}
[0-9]+[A-Za-z0-9]*	{return CH;}

" "			{return SPACE;}
"\n"			{return EOL;}

%%
