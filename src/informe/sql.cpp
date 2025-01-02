/**
* @file sql.c
* @brief El programa construeix unes queries en sql per consultar una base de dades.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>
#include <iconv.h>
#include <iostream>
#include <fstream>
#include <string>

/**
 *
 * @code
 *
 * CC = arm-linux-gnueabihf-g++
 * CFLAGS = -Wall -I/usr/include
 * LDFLAGS = -L. -lsqlite3 -lstdc++
 *
 * @endcode
 *
 * @author Joan Ramos Belencoso
 *
 * @version 1.0
 *
 * @date 18.12.2024
 *
 * @param statement
 *
 * @return no retorna res
 *
*/

static int callback (void *data, int argc, char **argv, char **azColName) //Función callback para gestionar datos de consulta.
{
	std::ofstream *outputfile = reinterpret_cast<std::ofstream*>(data);
	int i;
	fprintf(stderr, "Callback function called\n");
	for (i = 0; i < argc; i++) {
	*outputfile << azColName[i] << ": " << (argv[i] ? argv[i] : "NULL") << " | ";
	printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}

	*outputfile << "\n";
	printf("\n");
	return 0;
}

/**
 * @brief Funció principal del programa.
*/
int main()
{
	//sqlite_stmt *pStmt; //Variable usada para iterar los resultados.
	sqlite3 *db;
	sqlite3_stmt *stmt = nullptr;
	char *zErrMsg = 0;
	int rc = 0;
	const char *report;
	//const char *upload;
	int val1 = 23;
	int sens_id = 401;

	/* Bind the values */
    sqlite3_bind_int(stmt, 1, sens_id);  // Bind sens_id to the first placeholder
    sqlite3_bind_int(stmt, 2, val1);    // Bind val1 to the second placeholder

/* Create SQL Statement */

    /*upload = "INSERT INTO mesures (id_mesura, id_sensor, valor, temps) VALUES (NULL, ?, ?, CURRENT_TIMESTAMP);";*/

	report = "SELECT MAX (valor) FROM mesures;" \
			"SELECT MIN (valor) FROM mesures;" \
			"SELECT AVG (valor) FROM mesures;" \
			"SELECT MIN (temps) FROM mesures;" \
			"SELECT MAX (temps) FROM mesures;" ;

/* Open DataBase */

	rc = sqlite3_open("basedades_iiot.db", &db);

	if (rc) {
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		return 1;
		}

		std::ofstream outputfile("report.txt");
		fprintf(stderr, "Opened database successfully\n");
		if (!outputfile.is_open()) {
			fprintf(stderr, "Can't open the output file\n");
			sqlite3_close(db);
			return 1;
		}

/* Execute SQL statement */

	rc = sqlite3_exec(db, report, callback, &outputfile, &zErrMsg); //Cambio: (void *)data pot &outputfile
	//double sqlite3_column_double(sqlite3_stmt*, int iCol);

	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		}

	outputfile.close();
	sqlite3_close(db);
	return 0;
}
