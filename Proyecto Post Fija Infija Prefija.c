#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define MAX 100

// ==================== PILA PARA CARACTERES ====================
typedef struct {
    char data[MAX];
    int top;
} CharStack;

void initCharStack(CharStack *s) { s->top = -1; }
int isEmptyChar(CharStack *s) { return s->top == -1; }
int isFullChar(CharStack *s) { return s->top == MAX - 1; }

void pushChar(CharStack *s, char c) {
    if (!isFullChar(s)) s->data[++s->top] = c;
}

char popChar(CharStack *s) {
    if (!isEmptyChar(s)) return s->data[s->top--];
    return '\0';
}

char peekChar(CharStack *s) {
    if (!isEmptyChar(s)) return s->data[s->top];
    return '\0';
}

// ==================== PILA PARA CADENAS ====================
typedef struct {
    char data[MAX][MAX];
    int top;
} StringStack;

void initStringStack(StringStack *s) { s->top = -1; }
int isEmptyString(StringStack *s) { return s->top == -1; }
int isFullString(StringStack *s) { return s->top == MAX - 1; }

void pushString(StringStack *s, char *str) {
    if (!isFullString(s)) strcpy(s->data[++s->top], str);
}

void popString(StringStack *s, char *str) {
    if (!isEmptyString(s)) strcpy(str, s->data[s->top--]);
}

// ==================== FUNCIONES AUXILIARES ====================
int esOperador(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '^';
}

int precedencia(char op) {
    switch(op) {
        case '^': return 3;
        case '*':
        case '/': return 2;
        case '+':
        case '-': return 1;
        default: return 0;
    }
}

int esOperando(char c) {
    return isalnum(c) || c == '.';  // Letras, números o punto decimal
}

// ==================== INFIJA A POSTFIJA ====================
void infijaAPostfija(char *infija, char *postfija) {
    CharStack pila;
    initCharStack(&pila);
    int i, j = 0;
    
    printf("\n Conversión Infija → Postfija:\n");
    printf("   Expresión infija: %s\n", infija);
    
    for (i = 0; infija[i] != '\0'; i++) {
        printf("   Caracter: %c", infija[i]);
        
        // Si es operando, agregar a salida
        if (esOperando(infija[i])) {
            postfija[j++] = infija[i];
            printf(" → Operando, salida: %.*s\n", j, postfija);
        }
        // Si es '(', push a la pila
        else if (infija[i] == '(') {
            pushChar(&pila, infija[i]);
            printf(" → '(', push a pila\n");
        }
        // Si es ')', pop hasta encontrar '('
        else if (infija[i] == ')') {
            printf(" → ')', pop hasta '('\n");
            while (!isEmptyChar(&pila) && peekChar(&pila) != '(') {
                postfija[j++] = popChar(&pila);
                printf("     Pop %c, salida: %.*s\n", postfija[j-1], j, postfija);
            }
            popChar(&pila);  // Eliminar '('
        }
        // Si es operador
        else if (esOperador(infija[i])) {
            printf(" → Operador %c", infija[i]);
            while (!isEmptyChar(&pila) && precedencia(peekChar(&pila)) >= precedencia(infija[i])) {
                if (peekChar(&pila) == '^' && infija[i] == '^') break;  // Asociatividad derecha
                postfija[j++] = popChar(&pila);
                printf(", pop %c", postfija[j-1]);
            }
            pushChar(&pila, infija[i]);
            printf(", push %c a pila\n", infija[i]);
        }
    }
    
    // Pop todos los operadores restantes
    printf("   Fin expresión, vaciar pila:\n");
    while (!isEmptyChar(&pila)) {
        postfija[j++] = popChar(&pila);
        printf("     Pop %c, salida: %.*s\n", postfija[j-1], j, postfija);
    }
    
    postfija[j] = '\0';
    printf(" Expresión postfija final: %s\n", postfija);
}

// ==================== INFIJA A PREFIJA ====================
void infijaAPrefija(char *infija, char *prefija) {
    CharStack pila;
    initCharStack(&pila);
    char temp[MAX], resultado[MAX];
    int i, j = 0;
    
    printf("\n Conversión Infija → Prefija:\n");
    printf("   Expresión infija: %s\n", infija);
    
    // 1. Invertir la expresión infija
    int len = strlen(infija);
    for (i = 0; i < len; i++) {
        temp[i] = infija[len - 1 - i];
    }
    temp[len] = '\0';
    
    // 2. Intercambiar '(' por ')' y viceversa
    for (i = 0; temp[i] != '\0'; i++) {
        if (temp[i] == '(') temp[i] = ')';
        else if (temp[i] == ')') temp[i] = '(';
    }
    
    printf("   Expresión invertida: %s\n", temp);
    
    // 3. Convertir a postfija la expresión invertida
    j = 0;
    for (i = 0; temp[i] != '\0'; i++) {
        if (esOperando(temp[i])) {
            resultado[j++] = temp[i];
        }
        else if (temp[i] == '(') {
            pushChar(&pila, temp[i]);
        }
        else if (temp[i] == ')') {
            while (!isEmptyChar(&pila) && peekChar(&pila) != '(') {
                resultado[j++] = popChar(&pila);
            }
            popChar(&pila);
        }
        else if (esOperador(temp[i])) {
            while (!isEmptyChar(&pila) && precedencia(peekChar(&pila)) > precedencia(temp[i])) {
                resultado[j++] = popChar(&pila);
            }
            pushChar(&pila, temp[i]);
        }
    }
    
    while (!isEmptyChar(&pila)) {
        resultado[j++] = popChar(&pila);
    }
    resultado[j] = '\0';
    
    // 4. Invertir el resultado para obtener prefija
    for (i = 0; i < j; i++) {
        prefija[i] = resultado[j - 1 - i];
    }
    prefija[j] = '\0';
    
    printf(" Expresión prefija final: %s\n", prefija);
}

// ==================== PREFIJA A INFIJA ====================
void prefijaAInfija(char *prefija, char *infija) {
    StringStack pila;
    initStringStack(&pila);
    char op1[MAX], op2[MAX], temp[MAX];
    int i, len = strlen(prefija);
    
    printf("\n Conversión Prefija → Infija:\n");
    printf("   Expresión prefija: %s\n", prefija);
    
    // Recorrer de derecha a izquierda
    for (i = len - 1; i >= 0; i--) {
        printf("   Caracter: %c", prefija[i]);
        
        if (esOperando(prefija[i])) {
            temp[0] = prefija[i];
            temp[1] = '\0';
            pushString(&pila, temp);
            printf(" → Operando, push '%s' a pila\n", temp);
        }
        else if (esOperador(prefija[i])) {
            popString(&pila, op1);
            popString(&pila, op2);
            
            // Construir expresión infija con paréntesis
            sprintf(temp, "(%s%c%s)", op2, prefija[i], op1);
            pushString(&pila, temp);
            
            printf(" → Operador %c, pop '%s' y '%s', push '%s'\n", 
                   prefija[i], op1, op2, temp);
        }
    }
    
    popString(&pila, infija);
    printf("Expresión infija final: %s\n", infija);
}

// ==================== POSTFIJA A INFIJA ====================
void postfijaAInfija(char *postfija, char *infija) {
    StringStack pila;
    initStringStack(&pila);
    char op1[MAX], op2[MAX], temp[MAX];
    int i;
    
    printf("\n Conversión Postfija → Infija:\n");
    printf("   Expresión postfija: %s\n", postfija);
    
    // Recorrer de izquierda a derecha
    for (i = 0; postfija[i] != '\0'; i++) {
        printf("   Caracter: %c", postfija[i]);
        
        if (esOperando(postfija[i])) {
            temp[0] = postfija[i];
            temp[1] = '\0';
            pushString(&pila, temp);
            printf(" → Operando, push '%s' a pila\n", temp);
        }
        else if (esOperador(postfija[i])) {
            popString(&pila, op2);
            popString(&pila, op1);
            
            // Construir expresión infija con paréntesis
            sprintf(temp, "(%s%c%s)", op1, postfija[i], op2);
            pushString(&pila, temp);
            
            printf(" → Operador %c, pop '%s' y '%s', push '%s'\n", 
                   postfija[i], op1, op2, temp);
        }
    }
    
    popString(&pila, infija);
    printf(" Expresión infija final: %s\n", infija);
}

// ==================== MENÚ PRINCIPAL ====================
void mostrarMenu() {
    printf("\n");
    printf("╔══════════════════════════════════════════╗\n");
    printf("║       CONVERSOR DE NOTACIONES EN C       ║\n");
    printf("╠══════════════════════════════════════════╣\n");
    printf("║ 1. Infija → Postfija                     ║\n");
    printf("║ 2. Infija → Prefija                      ║\n");
    printf("║ 3. Prefija → Infija                      ║\n");
    printf("║ 4. Postfija → Infija                     ║\n");
    printf("║ 5. Ejemplos predefinidos                 ║\n");
    printf("║ 0. Salir                                 ║\n");
    printf("╚══════════════════════════════════════════╝\n");
    printf("Seleccione una opción: ");
}

void ejemplosPredefinidos() {
    printf("\n📚 EJEMPLOS PREDEFINIDOS:\n");
    printf("==================================================\n");
    
    // Ejemplo 1: A+B*C
    char infija1[] = "A+B*C";
    char resultado[MAX];
    
    printf("\n1. Expresión: %s\n", infija1);
    infijaAPostfija(infija1, resultado);
    infijaAPrefija(infija1, resultado);
    
    // Ejemplo 2: (A+B)*C
    char infija2[] = "(A+B)*C";
    
    printf("\n2. Expresión: %s\n", infija2);
    infijaAPostfija(infija2, resultado);
    infijaAPrefija(infija2, resultado);
    
    // Ejemplo 3: A+B*C-D/E
    char infija3[] = "A+B*C-D/E";
    
    printf("\n3. Expresión: %s\n", infija3);
    infijaAPostfija(infija3, resultado);
    infijaAPrefija(infija3, resultado);
}

// ==================== MAIN ====================
int main(int argc, char *argv[]) {
    int opcion;
    char expresion[MAX];
    char resultado[MAX];

    // Modo no interactivo: `Proyecto.exe <opcion> <expresion>`
    if (argc >= 3) {
        opcion = atoi(argv[1]);
        strncpy(expresion, argv[2], MAX - 1);
        expresion[MAX - 1] = '\0';

        switch (opcion) {
            case 1:
                infijaAPostfija(expresion, resultado);
                break;
            case 2:
                infijaAPrefija(expresion, resultado);
                break;
            case 3:
                prefijaAInfija(expresion, resultado);
                printf("\nExpresión infija: %s\n", resultado);
                break;
            case 4:
                postfijaAInfija(expresion, resultado);
                printf("\nExpresión infija: %s\n", resultado);
                break;
            default:
                printf("Opción inválida en modo no interactivo.\n");
        }
        return 0;
    }
    
    printf("=== CONVERSOR DE NOTACIONES MATEMÁTICAS ===\n");
    printf("Operadores soportados: + - * / ^\n");
    printf("Operandos: letras (A-Z, a-z) y números\n");
    
    do {
        mostrarMenu();
        scanf("%d", &opcion);
        getchar();  // Limpiar buffer
        
        switch(opcion) {
            case 1:
                printf("\nIntroduzca expresión INFIJA (ej: A+B*C): ");
                fgets(expresion, MAX, stdin);
                expresion[strcspn(expresion, "\n")] = '\0';  // Eliminar salto de línea
                infijaAPostfija(expresion, resultado);
                break;
                
            case 2:
                printf("\nIntroduzca expresión INFIJA (ej: A+B*C): ");
                fgets(expresion, MAX, stdin);
                expresion[strcspn(expresion, "\n")] = '\0';
                infijaAPrefija(expresion, resultado);
                break;
                
            case 3:
                printf("\nIntroduzca expresión PREFIJA (ej: +A*BC): ");
                fgets(expresion, MAX, stdin);
                expresion[strcspn(expresion, "\n")] = '\0';
                prefijaAInfija(expresion, resultado);
                printf("\nExpresión infija: %s\n", resultado);
                break;
                
            case 4:
                printf("\nIntroduzca expresión POSTFIJA (ej: ABC*+): ");
                fgets(expresion, MAX, stdin);
                expresion[strcspn(expresion, "\n")] = '\0';
                postfijaAInfija(expresion, resultado);
                printf("\nExpresión infija: %s\n", resultado);
                break;
                
            case 5:
                ejemplosPredefinidos();
                break;
                
            case 0:
                printf("\n¡Hasta luego!\n");
                break;
                
            default:
                printf("\nOpción inválida. Intente de nuevo.\n");
        }
        
        if (opcion != 0) {
            printf("\nPresione Enter para continuar...");
            getchar();
        }
        
    } while (opcion != 0);
    
    return 0;
}
