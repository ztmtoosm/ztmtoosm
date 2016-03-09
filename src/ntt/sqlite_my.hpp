#ifndef SQLITE_MY
#define SQLITE_MY
static int callback(void *NotUsed, int argc, char **argv, char **azColName){
   /*int i;
   for(i=0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");*/
   return 0;
}

void sqlite_execute_easy(sqlite3* db, string pol, int errorcode)
{
  char *zErrMsg = 0;
  int rc;
  rc = sqlite3_exec(db, pol.c_str(), callback, 0, &zErrMsg);
  if(rc != SQLITE_OK)
  {
    //cerr << "SQL error: " << zErrMsg << endl;
    sqlite3_free(zErrMsg);
    exit(errorcode);
  }
}
#endif
