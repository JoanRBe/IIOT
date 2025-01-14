#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include "sql.h"

/**
 * @brief Función para insertar datos en la base de datos SQLite.
 *
 * @param id_sensor El ID del sensor.
 * @param val_sens El valor del sensor.
 */
void sql(const char *id_sensor, const char *val_sens) {
    sqlite3 *db;
    sqlite3_stmt *stmt = nullptr;  //Inicialización explícita de stmt a nullptr
    int rc = 0;
    const char *upload;
    char *errMessage = 0;
    //const char *Timestamp = "CURRENT_TIMESTAMP"; // Asumimos que es un string representando una fecha

    //Abrir la base de datos

    rc = sqlite3_open("report.db", &db);
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return;
    }

    std::ofstream outputfile("report.txt");
    fprintf(stderr, "Opened database successfully\n");
    if (!outputfile.is_open()) {
        fprintf(stderr, "Can't open the output file\n");
        sqlite3_close(db);
        return;
    }

    //Crear tabla

    const char *CreateTableSQL = "CREATE TABLE IF NOT EXISTS mesures ("
                                 "id_mesura INTEGER PRIMARY KEY AUTOINCREMENT,"
                                 "id_sensor TEXT, "
                                 "val_sens TEXT, "
                                 "Timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP);";

    rc = sqlite3_exec(db, CreateTableSQL, 0, 0, &errMessage);  // Ejecuta el SQL
    if (rc != SQLITE_OK) {
        std::cerr << "Error al crear la tabla: " << errMessage << std::endl;
        sqlite3_free(errMessage);  // Libera el mensaje de error
    } else {
        std::cout << "Tabla 'Measures' creada correctamente!" << std::endl;
    }

    //Crear declaración SQL

    upload = "INSERT INTO mesures (id_sensor, val_sens, Timestamp) VALUES (?, ?, CURRENT_TIMESTAMP);";

    //Preparar la declaración

    rc = sqlite3_prepare_v2(db, upload, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    //Vincular los valores

    sqlite3_bind_text(stmt, 2, id_sensor, -1, SQLITE_TRANSIENT);  // Vincula id_sensor al primer marcador de posición
    sqlite3_bind_text(stmt, 1, val_sens, -1, SQLITE_TRANSIENT);   // Vincula val_sens al segundo marcador de posición

    //Ejecutar la declaración SQL

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return;
    }

    fprintf(stderr, "Data inserted successfully\n");

    //Finalizar y limpiar

    sqlite3_finalize(stmt);

    //Cerrar archivo y base de datos

    outputfile.close();
    sqlite3_close(db);
}
