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
 * @date 15.12.2024
 *
 * @param *CreateTableSQL *Upload
 *
 * @return No retorna res
 *
*/

/*static int callback (void *data, int argc, char **argv, char **azColName) //Función callback para gestionar datos de consulta.
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
}*/

/**
 * @brief Funció principal del programa. El programa crea una base de dades o obre la existent per enregistrar dades.
*/
int main() {
    sqlite3 *db;
    sqlite3_stmt *stmt = nullptr;  // Inicialización explícita de stmt a nullptr
    int rc = 0;
    const char *upload;
    char *errMessage = 0;
    int id_mesura = 1;
    const char *id_sensor = "402";
    float val_sens = 235;
    const char *Timestamp = "CURRENT_TIMESTAMP"; // Asumimos que es un string representando una fecha

    /* Open DataBase */
    rc = sqlite3_open("report.db", &db);

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
    
        const char *CreateTableSQL = "CREATE TABLE IF NOT EXISTS mesures ("
                                 "id_mesura INTEGER PRIMARY KEY AUTOINCREMENT,"
                                 "id_sensor STRING, "
                                 "val_sens STRING, "
                                 "Timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP);";

    rc = sqlite3_exec(db, CreateTableSQL, 0, 0, &errMessage);  // Ejecuta el SQL

    if (rc != SQLITE_OK) {
        std::cerr << "Error al crear la tabla: " << errMessage << std::endl;
        sqlite3_free(errMessage);  // Libera el mensaje de error
    } else {
        std::cout << "Tabla 'Measures' creada correctamente!" << std::endl;
    }

    /* Create SQL Statement with placeholders */
    upload = "INSERT INTO mesures (id_mesura, id_sensor, val_sens, Timestamp) VALUES (NULL, ?, ?, CURRENT_TIMESTAMP);";

    /* Prepare the statement */
    rc = sqlite3_prepare_v2(db, upload, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    /* Bind the values */
	sqlite3_bind_int(stmt, 3, id_mesura); // Vincula id_mesura al primer marcador de posición
    sqlite3_bind_text(stmt, 2, id_sensor, -1, SQLITE_TRANSIENT);    // Vincula id_sensor al segundo marcador de posición
    sqlite3_bind_double(stmt, 1, val_sens); // Vincula valor al tercer marcador de posición
    sqlite3_bind_text(stmt, 4, Timestamp, -1, SQLITE_STATIC);

    /* Execute the SQL statement */
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return 1;
    }

    fprintf(stderr, "Data inserted successfully\n");

    /* Finalize and clean up */
    sqlite3_finalize(stmt);

    // Close file and database
    outputfile.close();
    sqlite3_close(db);

    return 0;
}
