#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>

static int callback (void *data, int argc, char **argv, char **azColName)
{
	int i;
	fprintf(stderr, "%s: ", (const char *)data);

	for (i = 0; i < argc; i++)
	printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");

	printf("\n");
	return 0;
}

int main(int argc, char *argv[])
{
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	char statement[64];
	const char *data = "Callback function called";
	char maxim[64];
	char minim[64];
	char mitjana[64];
	char primer[64];
	char ultim[64];
	char report[1024];

/* Open DataBase */

	rc = sqlite3_open("basedades_iiot.db", &db);

	if (rc) {
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		return 0;
		}
	else {
		fprintf(stderr, "Opened database successfully\n");
		}

/* Create SQL Statement */

	sprintf(statement, "SELECT * FROM mesures;");
	sprintf(maxim, "SELECT MAX (valor) FROM mesures;");
	sprintf(minim, "SELECT MIN (valor) FROM mesures;");
	sprintf(mitjana, "SELECT AVG (valor) FROM mesures;");
	sprintf(primer, "SELECT MIN (temps) FROM mesures;");
	sprintf(ultim, "SELECT AVG (temps) FROM mesures;");

/* Execute SQL statement */

	rc = sqlite3_exec(db, maxim, callback, (void *)data, &zErrMsg);

	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);}

/*Creating the report*/

	else {
		FILE *file = fopen(report, "w");
		if (file != NULL) {
			sprintf(report, "VALORS DEMANATS:\nValor Maxim: %s\nValor Minim: %s\nMitjana: %s\nPrimera Lectura: %s\nUltima Lectura: %s\n", maxim, minim, mitjana, primer, ultim);
			fwrite(report, sizeof(char), 1024, file);
			fclose(file);
			fprintf(stdout, "Report generated successfully\n");
			}
		else{
			perror("Arxiu amb text");
			exit(EXIT_FAILURE);
			}
		}
	sqlite3_close(db);
	return 0;
}
