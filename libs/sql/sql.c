/**
* @file sql.c
* @brief El programa construeix unes queries en sql per consultar una base de dades.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>
#include <iconv.h>

/**
 *
 * @code
 *
 * -$ arm-linux-gnueabihf-gcc sql.c -o sql -L. -lsqlite3 -I/usr/include
 * -$ g++ sql.c -l sqlite3
 *
 * @endcode
 *
 * @author Joan Ramos Belencoso
 *
 * @version 1.0
 *
 * @date 05.11.2024
 *
 * @param statement
 *
 * @return no retorna res
 *
*/

static int callback (void *data, int argc, char **argv, char **azColName)
{
	int i;
	fprintf(stderr, "%s: ", (const char *)data);

	for (i = 0; i < argc; i++)
	printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");

	printf("\n");
	return 0;
}

/**
 * @brief Funció principal del programa.
*/
int main(int argc, char *argv[])
{
	sqlite3 *db;
	//sqlite_stmt *pStmt; //Variable usada para iterar los resultados.
	char *zErrMsg = 0;
	char report[1024];
	int rc = 0;
	//char statement[64];
	const char *query;
	const char *data = "Callback function called";
	char maxim[64];
	char minim[64];
	char mitjana[64];
	char primer[64];
	char ultim[64];

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

	query = "SELECT MAX (valor) FROM mesures;" \
			"SELECT MIN (valor) FROM mesures;" \
			"SELECT AVG (valor) FROM mesures;" \
			"SELECT MIN (temps) FROM mesures;" \
			"SELECT MAX (temps) FROM mesures;" ;


	/*sprintf(statement, "SELECT * FROM mesures;");
	sprintf(maxim, "SELECT MAX (valor) FROM mesures;");
	sprintf(minim, "SELECT MIN (valor) FROM mesures;");
	sprintf(mitjana, "SELECT AVG (valor) FROM mesures;");
	sprintf(primer, "SELECT MIN (temps) FROM mesures;");
	sprintf(ultim, "SELECT AVG (temps) FROM mesures;");*/

	memset(maxim, 0, 64);
	memset(minim, 0, 64);
	memset(mitjana, 0, 64);
	memset(primer, 0, 64);
	memset(ultim, 0, 64);

/* Execute SQL statement */

	rc = sqlite3_exec(db, query, callback, (void *)data, &zErrMsg);
	//double sqlite3_column_double(sqlite3_stmt*, int iCol);
	sprintf(report, data);

	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		}

/*Creating the report*/
/**
 * @brief Aquesta part intenta generar un informe.
*/

	else {
		FILE *file = fopen("report.txt", "w");
		if (file != NULL) {
			//sprintf(report, "VALORS DEMANATS:\nValor Maxim: %s\nValor Minim: %s\nMitjana: %s\nPrimera Lectura: %s\nUltima Lectura: %s\n", maxim, minim, mitjana, primer, ultim);
			fwrite(report, sizeof(char), 1024, file);
			fclose(file);
			fprintf(stdout, "Report generated successfully\n");
			}
		else {
			perror("Arxiu amb text");
			exit(EXIT_FAILURE);
			}
		}
	sqlite3_close(db);
	memset(report, 0, 1024);
	return 0;
}
