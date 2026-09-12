#include <stdio.h>
#include <stdlib.h>

#define MAX_PROC 5
#define Q 4 // Q = 4 según pseudocódigo

// Estructura que representa la lista L (PID y TS) y también la lista final N
typedef struct {
    int pid;
    int ts;           // Tiempo de servicio actual
    int ts_original;  // Guardamos el original para calcular tiempos finales
    int tiempo_retorno;
    int tiempo_espera;
} Proceso;

void ingresar_procesos(Proceso lista[], int *cantidad);
void ejecutar_round_robin(Proceso lista[], int cantidad);
int sumar_ts(Proceso lista[], int cantidad);

int main() {
    Proceso lista[MAX_PROC];
    int cantidad = 0;
    int opcion;

    // Menú interactivo con interfaz clara
    do {
        system("cls"); 
        printf("--- SIMULADOR ROUND ROBIN ---\n");
        printf("1. Ingresar procesos\n");
        printf("2. Ejecutar Round Robin\n");
        printf("3. Salir\n");
        printf("Seleccione una opcion: ");
        
        // Validación de buffer para evitar bucles infinitos por caracteres
        if (scanf("%d", &opcion) != 1) {
            while (getchar() != '\n'); // Limpia el buffer
            opcion = 0; // Fuerza a que caiga en 'default'
        }

        switch(opcion) {
            case 1:
                system("cls");
                ingresar_procesos(lista, &cantidad);
                system("pause");
                break;
            case 2:
                system("cls");
                if (cantidad > 0) {
                    ejecutar_round_robin(lista, cantidad);
                } else {
                    printf("No hay procesos cargados. Ingrese procesos primero.\n");
                }
                system("pause");
                break;
            case 3:
                printf("Saliendo del programa...\n");
                break;
            default:
                printf("Opcion invalida. Por favor, ingrese un numero del 1 al 3.\n");
                system("pause");
        }
    } while (opcion != 3);

    return 0;
}

void ingresar_procesos(Proceso lista[], int *cantidad) {
    // 1. Validar la cantidad de procesos
    do {
        printf("Ingrese la cantidad de procesos (entre 1 y %d): ", MAX_PROC);
        
        // Si ingresa una letra o un simbolo, limpiamos el buffer
        if (scanf("%d", cantidad) != 1) {
            while (getchar() != '\n'); 
            *cantidad = -1; // Forzamos un valor invalido para que repita el bucle
        } 
        else if (*cantidad < 1 || *cantidad > MAX_PROC) {
            printf("Error: Por favor ingrese un numero valido entre 1 y %d.\n", MAX_PROC);
        }
    } while (*cantidad < 1 || *cantidad > MAX_PROC);

    // 2. Validar el Tiempo de Servicio (TS) de cada proceso
    for (int i = 0; i < *cantidad; i++) {
        lista[i].pid = i + 1; 
        
        do {
            printf("Ingrese tiempo de servicio (TS) del proceso %d (debe ser mayor a 0): ", lista[i].pid);
            
            if (scanf("%d", &lista[i].ts) != 1) {
                while (getchar() != '\n');
                lista[i].ts = -1; 
            } 
            else if (lista[i].ts <= 0) {
                printf("Error: El tiempo de servicio no puede ser 0 ni negativo.\n");
            }
        } while (lista[i].ts <= 0);
        
        // Asignaciones iniciales del proceso
        lista[i].ts_original = lista[i].ts; 
        lista[i].tiempo_retorno = 0;
        lista[i].tiempo_espera = 0;
    }
    
    printf("\nProcesos cargados exitosamente.\n");
}

// Función auxiliar para evaluar la condición del bucle principal
int sumar_ts(Proceso lista[], int cantidad) {
    int suma = 0;
    for (int i = 0; i < cantidad; i++) {
        suma += lista[i].ts;
    }
    return suma;
}

void ejecutar_round_robin(Proceso lista[], int cantidad) {
    int reloj = 0; // R = 0 (Reloj)
    int pant = -1; // -1 representa Pant = vacío
    
    printf("--- EJECUCION ROUND ROBIN ---\n\n");
    
    // Mientras la sumatoria de TS de la lista sea mayor a cero
    while (sumar_ts(lista, cantidad) > 0) {
        
        for (int i = 0; i < cantidad; i++) {
            
            // Solo evaluamos si el proceso aún tiene tiempo de servicio pendiente
            if (lista[i].ts > 0) {
                
                // Guardamos el TS antes de ejecutar para mostrar el progreso
                int ts_al_iniciar = lista[i].ts;
                
                printf("=== TURNO PROCESO %d ===\n", lista[i].pid);
                printf("Info: TS Original = %d | TS al entrar = %d\n", lista[i].ts_original, ts_al_iniciar);
                
                // Si el proceso actual es distinto a Pant y Pant <> vacio
                if (lista[i].pid != pant && pant != -1) {
                    printf("[Reloj %d] Scheduler: Quitando proceso %d de pantalla\n", reloj, pant);
                }
                
                // Si L(i).TS > Q
                if (lista[i].ts > Q) {
                    if (lista[i].pid != pant) {
                        printf("[Reloj %d] Scheduler: Insertando proceso %d en pantalla\n", reloj, lista[i].pid);
                    }
                    
                    printf("[Reloj %d] Ejecutando por %d unidades (Quantum completo)\n", reloj, Q);
                    lista[i].ts = lista[i].ts - Q; // L(i).TS = L(i).TS - Q
                    reloj = reloj + Q;             // R = R + Q
                    
                } else {
                    if (lista[i].pid != pant) {
                        printf("[Reloj %d] Scheduler: Insertando proceso %d en pantalla\n", reloj, lista[i].pid);
                    }
                    
                    printf("[Reloj %d] Ejecutando por %d unidades (Fin de proceso)\n", reloj, lista[i].ts);
                    reloj = reloj + lista[i].ts;   // R = R + L(i).TS
                    lista[i].ts = 0;               // L(i).TS = 0
                    
                    lista[i].tiempo_retorno = reloj; 
                }
                
                // Mostrar por pantalla el progreso y el nuevo reloj
                printf("  -> Progreso: TS restante ahora es %d\n", lista[i].ts);
                printf("  -> Reloj avanza a: %d\n", reloj);
                
                pant = lista[i].pid;
                
                // --- PAUSE PARA QUE EL USUARIO PASE MANUALMENTE ---
                printf("\n");
                system("pause");
                printf("--------------------------------------------------\n\n");
            }
        }
    }
    
    // Con la lista N calcular e imprimir el tiempo de retorno y tiempo de espera promedio
    printf("--- RESULTADOS FINALES ---\n");
    float suma_retorno = 0;
    float suma_espera = 0;
    
    for (int i = 0; i < cantidad; i++) {
        lista[i].tiempo_espera = lista[i].tiempo_retorno - lista[i].ts_original;
        suma_retorno += lista[i].tiempo_retorno;
        suma_espera += lista[i].tiempo_espera;
        
        printf("Proceso %d: Tiempo Retorno = %d | Tiempo Espera = %d\n", 
               lista[i].pid, lista[i].tiempo_retorno, lista[i].tiempo_espera);
    }
    
    printf("\nTiempo de Retorno Promedio: %.2f\n", suma_retorno / cantidad);
    printf("Tiempo de Espera Promedio: %.2f\n\n", suma_espera / cantidad);
}